/**
 * @file Codebook.cpp
 * @brief Implementation of Codebook class methods
 * @details Provides constructor, validation logic, and MetadataTag creation methods
 */

#include "dnv/vista/sdk/Codebook.h"
#include "dto/CodebooksDto.h"

#include <algorithm>
#include <vector>

namespace dnv::vista::sdk
{
    //=====================================================================
    // Helper functions
    //=====================================================================

    namespace
    {
        constexpr CodebookName parseCodebookName( std::string_view name )
        {
            if( name == "positions" )
            {
                return CodebookName::Position;
            }
            if( name == "calculations" )
            {
                return CodebookName::Calculation;
            }
            if( name == "quantities" )
            {
                return CodebookName::Quantity;
            }
            if( name == "states" )
            {
                return CodebookName::State;
            }
            if( name == "contents" )
            {
                return CodebookName::Content;
            }
            if( name == "commands" )
            {
                return CodebookName::Command;
            }
            if( name == "types" )
            {
                return CodebookName::Type;
            }
            if( name == "functional_services" )
            {
                return CodebookName::FunctionalServices;
            }
            if( name == "maintenance_category" )
            {
                return CodebookName::MaintenanceCategory;
            }
            if( name == "activity_type" )
            {
                return CodebookName::ActivityType;
            }
            if( name == "detail" )
            {
                return CodebookName::Detail;
            }

            throw std::invalid_argument{ "Unknown codebook name: " + std::string{ name } };
        }
    } // namespace

    //=====================================================================
    // Constructor
    //=====================================================================

    Codebook::Codebook( const CodebookDto& dto )
        : m_name{ parseCodebookName( dto.name ) }
    {
        // Reserve capacity - single allocation for performance
        size_t estimatedSize = 0;
        for( const auto& [group, values] : dto.values )
        {
            estimatedSize += values.size();
        }

        m_groupMap.reserve( estimatedSize );
        m_standardValues.reserve( estimatedSize );
        m_groups.reserve( dto.values.size() );

        // Build all data structures - cache-friendly single pass
        for( const auto& [group, values] : dto.values )
        {
            auto trimmedGroupView = nfx::string::trim( group );

            // Add to groups set (skip <number> and Number as they're not real groups)
            if( trimmedGroupView != "<number>" && trimmedGroupView != "Number" )
            {
                m_groups.emplace( std::string{ trimmedGroupView } );
            }

            for( const auto& value : values )
            {
                auto trimmedValueView = nfx::string::trim( value );

                // Skip "<number>" placeholder
                if( trimmedValueView == "<number>" )
                {
                    continue;
                }

                m_groupMap.emplace( std::string{ trimmedValueView }, std::string{ trimmedGroupView } );

                m_standardValues.emplace( std::string{ trimmedValueView } );
            }
        }
    }

    //=====================================================================
    // MetadataTag creation
    //=====================================================================

    std::optional<MetadataTag> Codebook::createTag( std::string_view value ) const noexcept
    {
        if( nfx::string::isNullOrWhiteSpace( value ) )
        {
            return std::nullopt;
        }

        bool isCustom = false;

        if( m_name == CodebookName::Position )
        {
            // Validate position
            auto validity = validatePosition( value );
            if( validity < PositionValidationResult::Valid )
            {
                return std::nullopt;
            }

            if( validity == PositionValidationResult::Custom )
            {
                isCustom = true;
            }
        }
        else
        {
            if( !nfx::string::isUriUnreserved( value ) )
            {
                return std::nullopt;
            }
            if( m_name != CodebookName::Detail && !m_standardValues.find( value ) )
            {
                isCustom = true;
            }
        }

        return MetadataTag{ m_name, std::string{ value }, isCustom };
    }

    //=====================================================================
    // Position validation
    //=====================================================================

    PositionValidationResult Codebook::validatePosition( std::string_view position ) const noexcept
    {
        if( nfx::string::isNullOrWhiteSpace( position ) || !nfx::string::isUriUnreserved( position ) )
        {
            return PositionValidationResult::Invalid;
        }

        auto trimmed = nfx::string::trim( position );
        if( trimmed.size() != position.size() )
        {
            return PositionValidationResult::Invalid;
        }

        if( m_standardValues.find( position ) != nullptr )
        {
            return PositionValidationResult::Valid;
        }

        if( !nfx::string::contains( position, "-" ) )
        {
            // Check if it's a number - numbers are valid in positions
            if( nfx::string::isAllDigits( position ) )
            {
                return PositionValidationResult::Valid;
            }
            return PositionValidationResult::Custom;
        }

        // Split and validate each part
        StackVector<std::string_view, 16> parts;
        StackVector<PositionValidationResult, 16> validations;

        for( auto part : nfx::string::splitView( position, '-' ) )
        {
            parts.push_back( part );
            validations.push_back( validatePosition( part ) );
        }
        auto maxValidation = std::max_element(
            validations.begin(), validations.end(), []( PositionValidationResult a, PositionValidationResult b ) {
                return static_cast<int>( a ) < static_cast<int>( b );
            } );

        bool anyErrors = maxValidation != validations.end() && static_cast<int>( *maxValidation ) < 100;

        // Check if any number appears before the last position
        bool numberNotAtEnd = false;
        for( size_t i = 0; i < parts.size(); ++i )
        {
            if( nfx::string::isAllDigits( parts[i] ) && i < parts.size() - 1 )
            {
                numberNotAtEnd = true;
                break;
            }
        }

        // Check alphabetical sorting of non-number parts
        std::vector<std::string_view> nonNumbers;
        nonNumbers.reserve( parts.size() );
        for( auto part : parts )
        {
            if( !nfx::string::isAllDigits( part ) )
            {
                nonNumbers.push_back( part );
            }
        }

        if( !std::is_sorted( nonNumbers.begin(), nonNumbers.end() ) )
        {
            return PositionValidationResult::InvalidOrder;
        }

        // Combine order validation results
        if( numberNotAtEnd )
        {
            return PositionValidationResult::InvalidOrder;
        }

        // If any part had errors, return now (after order check)
        if( anyErrors && maxValidation != validations.end() )
        {
            return *maxValidation;
        }

        // Grouping validation - only if all parts are exactly Valid (100)
        if( std::all_of( validations.begin(), validations.end(), []( PositionValidationResult v ) {
                return static_cast<int>( v ) == 100;
            } ) )
        {
            std::vector<std::string_view> groups;
            groups.reserve( parts.size() );

            for( auto part : parts )
            {
                if( nfx::string::isAllDigits( part ) )
                {
                    groups.push_back( "<number>" );
                }
                else
                {
                    if( auto* value = m_groupMap.find( part ) )
                    {
                        groups.push_back( *value );
                    }
                }
            }

            // Check for duplicates (except DEFAULT_GROUP)
            bool hasDefaultGroup = std::find( groups.begin(), groups.end(), "DEFAULT_GROUP" ) != groups.end();
            if( !hasDefaultGroup )
            {
                std::unordered_set<std::string_view> uniqueGroups( groups.begin(), groups.end() );
                if( uniqueGroups.size() != groups.size() )
                {
                    return PositionValidationResult::InvalidGrouping;
                }
            }
        }

        // Return maximum validation result
        return *maxValidation;
    }
} // namespace dnv::vista::sdk
