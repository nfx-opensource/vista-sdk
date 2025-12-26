/**
 * @file UniversalIdBuilder.cpp
 * @brief Implementation of VIS UniversalIdBuilder
 * @details Provides implementations for UniversalIdBuilder class for building UniversalId instances.
 */

#include "dnv/vista/sdk/UniversalIdBuilder.h"

#include "internal/parsing/LocalIdParsingErrorBuilder.h"
#include "dnv/vista/sdk/detail/ISO19848AnnexC.h"
#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/StringBuilder.h"
#include "dnv/vista/sdk/UniversalId.h"

#include <nfx/StringUtils.h>

#include <stdexcept>

namespace dnv::vista::sdk
{
    UniversalId UniversalIdBuilder::build() const
    {
        if( !isValid() )
        {
            throw std::invalid_argument{
                "Cannot build UniversalId: builder is invalid (missing IMO number or LocalId)"
            };
        }
        return UniversalId{ *this };
    }

    std::optional<UniversalId> UniversalIdBuilder::fromString( std::string_view universalIdStr ) noexcept
    {
        return fromString( universalIdStr, nullptr );
    }

    std::optional<UniversalId> UniversalIdBuilder::fromString(
        std::string_view universalIdStr, ParsingErrors& errors ) noexcept
    {
        return fromString( universalIdStr, &errors );
    }

    std::optional<UniversalId> UniversalIdBuilder::fromString(
        std::string_view universalIdStr, ParsingErrors* outErrors ) noexcept
    {
        using namespace internal;

        // Format: data.dnv.com/IMO1234567/dnv-v2/vis-3-4a/...
        LocalIdParsingErrorBuilder errorBuilder;

        if( universalIdStr.empty() )
        {
            if( outErrors )
            {
                errorBuilder.addError( LocalIdParsingState::NamingRule, "Failed to find localId start segment" );
                *outErrors = errorBuilder.build();
            }
            return std::nullopt;
        }

        // Find the start of LocalId (search for "/{NamingRule}-v")
        const std::string localIdPrefix = std::string{ "/" } + std::string{ iso19848::annexC::NamingRule } + "-v";
        const auto localIdStartPos = universalIdStr.find( localIdPrefix );
        if( localIdStartPos == std::string_view::npos )
        {
            if( outErrors )
            {
                errorBuilder.addError( LocalIdParsingState::NamingRule, "Failed to find localId start segment" );
                *outErrors = errorBuilder.build();
            }
            return std::nullopt;
        }

        // Split into universal ID segment and LocalId segment
        const auto universalIdSegment = universalIdStr.substr( 0, localIdStartPos );
        const auto localIdSegment = universalIdStr.substr( localIdStartPos );

        // Parse LocalId - use version without errors if we don't need them
        std::optional<LocalId> localId;
        if( outErrors )
        {
            ParsingErrors localIdErrors;
            localId = LocalIdBuilder::fromString( localIdSegment, localIdErrors );
            if( !localId.has_value() )
            {
                // Pass through errors from LocalIdBuilder
                *outErrors = localIdErrors;
                return std::nullopt;
            }
        }
        else
        {
            localId = LocalIdBuilder::fromString( localIdSegment );
            if( !localId.has_value() )
            {
                return std::nullopt;
            }
        }

        // Get the LocalIdBuilder from the parsed LocalId
        const auto& localIdBuilder = localId->builder();

        // Parse universal ID segment: "data.dnv.com/9999999"
        // Expected format: {NamingEntity}/{ImoNumber}
        std::optional<ImoNumber> imoNumber;

        // Parse naming entity
        const auto namingEntitySegment = nfx::string::substringBefore( universalIdSegment, "/" );
        const auto afterNamingEntity = nfx::string::substringAfter( universalIdSegment, "/" );

        // Check if slash was found (afterNamingEntity will be empty if no slash)
        if( afterNamingEntity.empty() )
        {
            if( outErrors )
            {
                errorBuilder.addError(
                    LocalIdParsingState::NamingEntity, "Invalid Universal ID format: missing IMO number" );
                *outErrors = errorBuilder.build();
            }
            return std::nullopt;
        }

        // Validate naming entity
        if( namingEntitySegment != iso19848::annexC::NamingEntity )
        {
            if( outErrors )
            {
                errorBuilder.addError(
                    LocalIdParsingState::NamingEntity,
                    std::string{ "Naming entity segment didn't match. Found: " } + std::string{ namingEntitySegment } );
                *outErrors = errorBuilder.build();
            }
            return std::nullopt;
        }

        // Parse IMO number (everything after naming entity, before any additional slash)
        const auto imoSegment = nfx::string::substringBefore( afterNamingEntity, "/" );

        if( !imoSegment.empty() )
        {
            auto imo = ImoNumber::fromString( imoSegment );
            if( !imo.has_value() )
            {
                if( outErrors )
                {
                    errorBuilder.addError( LocalIdParsingState::IMONumber, "Invalid IMO number segment" );
                    *outErrors = errorBuilder.build();
                }
                return std::nullopt;
            }
            imoNumber = imo;
        }

        // Get VisVersion from localIdBuilder
        if( !localIdBuilder.version().has_value() )
        {
            if( outErrors )
            {
                errorBuilder.addError( LocalIdParsingState::VisVersion, "Invalid VisVersion in LocalId" );
                *outErrors = errorBuilder.build();
            }
            return std::nullopt;
        }

        // Build UniversalIdBuilder
        auto builder = UniversalIdBuilder::create( *localIdBuilder.version() ).withLocalId( localIdBuilder );

        if( imoNumber.has_value() )
        {
            builder = builder.withImoNumber( *imoNumber );
        }

        // Validate and build UniversalId
        if( !builder.isValid() )
        {
            if( outErrors )
            {
                errorBuilder.addError(
                    LocalIdParsingState::Completeness, "UniversalIdBuilder is invalid after parsing" );
                *outErrors = errorBuilder.build();
            }
            return std::nullopt;
        }

        if( outErrors )
        {
            *outErrors = errorBuilder.build();
        }

        return builder.build();
    }
} // namespace dnv::vista::sdk
