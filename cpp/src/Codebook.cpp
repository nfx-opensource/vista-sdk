/**
 * @file Codebook.cpp
 * @brief Implementation of codebook-related components
 */

#include "dnv/vista/sdk/Codebook.h"

#include "internal/constants/Codebook.h"
#include "internal/dto/CodebooksDto.h"
#include "dnv/vista/sdk/MetadataTag.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// PositionValidationResults class
	//=====================================================================

	//----------------------------------------------
	// Static utility methods
	//----------------------------------------------

	PositionValidationResult PositionValidationResults::fromString( std::string_view name )
	{
		if ( nfx::string::isEmpty( name ) )
		{
			throw std::invalid_argument{ "PositionValidationResult name cannot be empty" };
		}

		if ( nfx::string::iequals( name, internal::codebook::POSITION_VALIDATION_INVALID ) )
		{
			return PositionValidationResult::Invalid;
		}
		if ( nfx::string::iequals( name, internal::codebook::POSITION_VALIDATION_INVALID_ORDER ) )
		{
			return PositionValidationResult::InvalidOrder;
		}
		if ( nfx::string::iequals( name, internal::codebook::POSITION_VALIDATION_INVALID_GROUPING ) )
		{
			return PositionValidationResult::InvalidGrouping;
		}
		if ( nfx::string::iequals( name, internal::codebook::POSITION_VALIDATION_VALID ) )
		{
			return PositionValidationResult::Valid;
		}
		if ( nfx::string::iequals( name, internal::codebook::POSITION_VALIDATION_CUSTOM ) )
		{
			return PositionValidationResult::Custom;
		}

		throw std::invalid_argument{
			"Unknown PositionValidationResult name: '" +
			std::string{ name } +
			"'. Valid values are: Invalid, InvalidOrder, InvalidGrouping, Valid, Custom" };
	}

	//=====================================================================
	// Codebook class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Codebook::Codebook( const CodebookDto& dto )
		: m_name{ CodebookNames::fromString( dto.name() ) },
		  m_groupMap{},
		  m_standardValues{},
		  m_groups{},
		  m_rawData{}
	{
		// Pre-calculate total size for better memory allocation
		size_t totalEstimate = 0;
		const auto& dtoValues = dto.values();
		const size_t groupCount = dtoValues.size();

		for ( const auto& [_, values] : dtoValues )
		{
			totalEstimate += values.size();
		}

		// Load factor calculation
		const size_t capacity = totalEstimate + ( totalEstimate >> 1 ); // 1.5x

		m_groupMap.reserve( capacity );
		m_rawData.reserve( groupCount + ( groupCount >> 2 ) ); // 1.25x

		nfx::containers::StringSet valueSet;
		nfx::containers::StringSet groupSet;
		valueSet.reserve( capacity );
		groupSet.reserve( groupCount + ( groupCount >> 2 ) );

		for ( auto& [groupKey, values] : dto.values() )
		{
			std::string_view groupTrimmed = nfx::string::trim( groupKey );
			std::string groupStr{ groupTrimmed };

			std::vector<std::string> trimmedValues;
			trimmedValues.reserve( values.size() );

			for ( auto& value : values )
			{
				std::string_view valueTrimmed = nfx::string::trim( value );
				std::string valueStr{ valueTrimmed };
				trimmedValues.emplace_back( std::move( valueStr ) );

				if ( trimmedValues.back() != internal::codebook::GROUP_NUMBER )
				{
					m_groupMap.emplace( trimmedValues.back(), groupStr );
					valueSet.emplace( trimmedValues.back() );
					groupSet.emplace( groupStr );
				}
			}

			m_rawData.emplace( std::move( groupStr ), std::move( trimmedValues ) );
		}

		m_standardValues = CodebookStandardValues{ m_name, std::move( valueSet ) };
		m_groups = CodebookGroups{ std::move( groupSet ) };
	}

	Codebook::Codebook( const Codebook& other )
		: m_name{ other.m_name },
		  m_groupMap{ other.m_groupMap },
		  m_standardValues{ other.m_standardValues },
		  m_groups{ other.m_groups },
		  m_rawData{ other.m_rawData }
	{
		// Copy constructor properly implemented - all members copied
	}

	//----------------------------------------------
	// Metadata tag creation
	//----------------------------------------------

	std::optional<MetadataTag> Codebook::tryCreateTag( std::string_view valueView ) const
	{
		if ( nfx::string::isEmpty( valueView ) )
		{
			return std::nullopt;
		}

		bool isCustom = false;

		if ( m_name == CodebookName::Position )
		{
			PositionValidationResult positionValidity = validatePosition( valueView );

			if ( positionValidity < PositionValidationResult::Valid )
			{
				return std::nullopt;
			}

			if ( positionValidity == PositionValidationResult::Custom )
			{
				isCustom = true;
			}
		}
		else
		{
			if ( !nfx::string::isURIUnreserved( valueView ) )
			{
				return std::nullopt;
			}

			if ( m_name != CodebookName::Detail && !m_standardValues.contains( valueView ) )
			{
				isCustom = true;
			}
		}

		std::string value{ valueView };

		return MetadataTag{ m_name, value, isCustom };
	}

	MetadataTag Codebook::createTag( std::string_view value ) const
	{
		auto tagOpt = tryCreateTag( value );
		if ( !tagOpt )
		{
			throw std::invalid_argument{
				"Invalid value for metadata tag: codebook=" +
				std::to_string( static_cast<int>( m_name ) ) +
				", value=" + std::string{ value } };
		}

		return tagOpt.value();
	}

	//----------------------------------------------
	// Position validation
	//----------------------------------------------

	PositionValidationResult Codebook::validatePosition( std::string_view position ) const
	{
		if ( nfx::string::isEmpty( position ) )
		{
			return PositionValidationResult::Invalid;
		}

		bool allDigits = true;
		for ( char c : position )
		{
			if ( nfx::string::isWhitespace( c ) )
			{
				return PositionValidationResult::Invalid;
			}
			if ( allDigits && !nfx::string::isDigit( c ) )
			{
				allDigits = false;
			}
		}

		if ( !nfx::string::isURIUnreserved( position ) )
		{
			return PositionValidationResult::Invalid;
		}

		if ( m_standardValues.contains( position ) )
		{
			return PositionValidationResult::Valid;
		}

		if ( allDigits )
		{
			return PositionValidationResult::Valid;
		}

		if ( !nfx::string::contains( position, "-" ) )
		{
			return PositionValidationResult::Custom;
		}

		std::array<std::string_view, internal::codebook::MAX_POSITIONS> positions;
		size_t positionCount = 0;

		const char* data = position.data();
		const char* end = data + position.size();
		const char* start = data;

		while ( start < end && positionCount < internal::codebook::MAX_POSITIONS )
		{
			const char* hyphen = std::find( start, end, '-' );
			positions[positionCount] = std::string_view( start, static_cast<size_t>( hyphen - start ) );
			++positionCount;

			if ( hyphen == end )
			{
				break;
			}

			start = hyphen + 1;
		}

		if ( positionCount >= internal::codebook::MAX_POSITIONS )
		{
			return PositionValidationResult::Invalid;
		}

		std::array<bool, internal::codebook::MAX_POSITIONS> isDigitArray;
		std::array<std::string_view, internal::codebook::MAX_NON_NUMERIC> nonNumericPositions;
		size_t nonNumericCount = 0;
		bool hasNumberNotAtEnd = false;
		PositionValidationResult worstResult = PositionValidationResult::Valid;
		bool isNotSorted = false;

		for ( size_t i = 0; i < positionCount; ++i )
		{
			isDigitArray[i] = nfx::string::isAllDigits( positions[i] );

			PositionValidationResult result;
			if ( m_standardValues.contains( positions[i] ) )
			{
				result = PositionValidationResult::Valid;
			}
			else if ( isDigitArray[i] )
			{
				result = PositionValidationResult::Valid;
			}
			else
			{
				result = PositionValidationResult::Custom;
			}

			if ( result < PositionValidationResult::Valid )
			{
				return result;
			}
			if ( result > worstResult )
			{
				worstResult = result;
			}

			if ( isDigitArray[i] )
			{
				if ( i < positionCount - 1 )
				{
					hasNumberNotAtEnd = true;
				}
			}
			else
			{
				if ( nonNumericCount < internal::codebook::MAX_NON_NUMERIC )
				{
					if ( nonNumericCount > 0 && positions[i] < nonNumericPositions[nonNumericCount - 1] )
					{
						isNotSorted = true;
					}
					nonNumericPositions[nonNumericCount] = positions[i];
					++nonNumericCount;
				}
			}
		}

		if ( hasNumberNotAtEnd || isNotSorted )
		{
			return PositionValidationResult::InvalidOrder;
		}

		if ( worstResult == PositionValidationResult::Valid )
		{
			nfx::containers::StringSet uniqueGroups;
			uniqueGroups.reserve( internal::codebook::MAX_GROUPS );
			bool hasDefaultGroup = false;

			for ( size_t i = 0; i < positionCount; ++i )
			{
				std::string_view group;

				if ( isDigitArray[i] )
				{
					group = internal::codebook::GROUP_NUMBER;
				}
				else
				{
					auto it = m_groupMap.find( std::string{ positions[i] } );
					group = ( it != m_groupMap.end() ) ? std::string_view{ it->second }
													   : internal::codebook::GROUP_UNKNOWN;
				}

				uniqueGroups.emplace( group );

				if ( group == internal::codebook::GROUP_DEFAULT )
				{
					hasDefaultGroup = true;
				}
			}

			if ( !hasDefaultGroup && uniqueGroups.size() != positionCount )
			{
				return PositionValidationResult::InvalidGrouping;
			}
		}

		return worstResult;
	}
}
