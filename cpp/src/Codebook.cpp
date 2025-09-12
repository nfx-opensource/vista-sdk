/**
 * @file Codebook.cpp
 * @brief Implementation of codebook-related components
 */

#include <nfx/string/Utils.h>

#include "internal/constants/Codebook.h"

#include "dnv/vista/sdk/Codebook.h"
#include "dnv/vista/sdk/Codebooks.h"
#include "dnv/vista/sdk/MetadataTag.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Magic numbers
	//=====================================================================

	namespace internal
	{
		/** @brief Stack allocation limit for position parsing arrays to avoid heap allocation during position validation. */
		static constexpr size_t MAX_POSITIONS = 16;

		/** @brief Stack allocation limit for group tracking arrays to avoid heap allocation during position grouping validation. */
		static constexpr size_t MAX_GROUPS = 16;

		/** @brief Stack allocation limit for non-numeric position tracking to avoid heap allocation during order validation. */
		static constexpr size_t MAX_NON_NUMERIC = 8;

		/** @brief Character set for null or whitespace detection in string parsing operations. */
		inline constexpr std::string_view NULL_OR_WHITESPACE = " \t\n\r\f\v";
	}

	//=====================================================================
	// Character lookup tables
	//=====================================================================

	namespace
	{
		/**
		 * @brief Cache-aligned lookup table for whitespace character detection
		 * @details Uses constexpr lambda initialization for compile-time optimization.
		 *          Aligned to 64-byte boundary for optimal cache line utilization.
		 */
		alignas( 64 ) constexpr std::array<bool, 256> s_whitespaceLookup = []() constexpr {
			std::array<bool, 256> lookup{};
			for ( char c : internal::NULL_OR_WHITESPACE )
			{
				lookup[static_cast<unsigned char>( c )] = true;
			}

			return lookup;
		}();

		constexpr bool isWhitespace( char c ) noexcept { return s_whitespaceLookup[static_cast<unsigned char>( c )]; }

		constexpr bool isDigit( char c ) noexcept { return static_cast<unsigned char>( c - '0' ) <= 9u; }

		constexpr std::string_view trimString( std::string_view str ) noexcept
		{
			if ( str.empty() )
			{
				return {};
			}

			// Find first non-whitespace
			size_t first = 0;
			const size_t size = str.size();
			while ( first < size && isWhitespace( str[first] ) )
			{
				++first;
			}

			if ( first == size )
			{
				return {};
			}

			// Find last non-whitespace
			size_t last = size - 1;
			while ( last > first && isWhitespace( str[last] ) )
			{
				--last;
			}

			return str.substr( first, last - first + 1 );
		}
	}

	//=====================================================================
	// PositionValidationResults class
	//=====================================================================

	//----------------------------------------------
	// Static utility methods
	//----------------------------------------------

	PositionValidationResult PositionValidationResults::fromString( std::string_view name )
	{
		if ( name.empty() )
		{
			throw std::invalid_argument{ "PositionValidationResult name cannot be empty" };
		}

		const std::string lowerName = nfx::string::toLower( name );

		if ( lowerName == internal::constants::codebook::POSITION_VALIDATION_INVALID )
		{
			return PositionValidationResult::Invalid;
		}
		if ( lowerName == internal::constants::codebook::POSITION_VALIDATION_INVALID_ORDER )
		{
			return PositionValidationResult::InvalidOrder;
		}
		if ( lowerName == internal::constants::codebook::POSITION_VALIDATION_INVALID_GROUPING )
		{
			return PositionValidationResult::InvalidGrouping;
		}
		if ( lowerName == internal::constants::codebook::POSITION_VALIDATION_VALID )
		{
			return PositionValidationResult::Valid;
		}
		if ( lowerName == internal::constants::codebook::POSITION_VALIDATION_CUSTOM )
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
			std::string_view groupTrimmed = trimString( groupKey );
			std::string groupStr{ groupTrimmed };

			std::vector<std::string> trimmedValues;
			trimmedValues.reserve( values.size() );

			for ( auto& value : values )
			{
				std::string_view valueTrimmed = trimString( value );
				std::string valueStr{ valueTrimmed };
				trimmedValues.emplace_back( std::move( valueStr ) );

				if ( trimmedValues.back() != internal::constants::codebook::GROUP_NUMBER )
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

	//----------------------------------------------
	// Metadata tag creation
	//----------------------------------------------

	std::optional<MetadataTag> Codebook::tryCreateTag( std::string_view valueView ) const
	{
		if ( valueView.empty() )
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
			if ( !VIS::isISOString( valueView ) )
			{
				return std::nullopt;
			}

			if ( m_name != CodebookName::Detail && !m_standardValues.contains( valueView ) )
			{
				isCustom = true;
			}
		}

		std::string value{ valueView };
		return MetadataTag( m_name, value, isCustom );
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
		if ( position.empty() )
		{
			return PositionValidationResult::Invalid;
		}

		bool allDigits = true;
		for ( char c : position )
		{
			if ( isWhitespace( c ) )
			{
				return PositionValidationResult::Invalid;
			}
			if ( allDigits && !isDigit( c ) )
			{
				allDigits = false;
			}
		}

		if ( !VIS::isISOString( position ) )
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

		const size_t hyphenPos = position.find( '-' );
		if ( hyphenPos == std::string_view::npos )
		{
			return PositionValidationResult::Custom;
		}

		std::array<std::string_view, internal::MAX_POSITIONS> positions;
		size_t positionCount = 0;

		const char* data = position.data();
		const char* end = data + position.size();
		const char* start = data;

		while ( start < end && positionCount < internal::MAX_POSITIONS )
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

		if ( positionCount >= internal::MAX_POSITIONS )
		{
			return PositionValidationResult::Invalid;
		}

		std::array<bool, internal::MAX_POSITIONS> isDigitArray;
		std::array<std::string_view, internal::MAX_NON_NUMERIC> nonNumericPositions;
		size_t nonNumericCount = 0;
		bool hasNumberNotAtEnd = false;
		PositionValidationResult worstResult = PositionValidationResult::Valid;
		bool isNotSorted = false;

		for ( size_t i = 0; i < positionCount; ++i )
		{
			isDigitArray[i] = !positions[i].empty() &&
							  std::all_of( positions[i].begin(), positions[i].end(), isDigit );

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
				if ( nonNumericCount < internal::MAX_NON_NUMERIC )
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
			uniqueGroups.reserve( internal::MAX_GROUPS );
			bool hasDefaultGroup = false;

			for ( size_t i = 0; i < positionCount; ++i )
			{
				std::string_view group;

				if ( isDigitArray[i] )
				{
					group = internal::constants::codebook::GROUP_NUMBER;
				}
				else
				{
					auto it = m_groupMap.find( positions[i] );
					group = ( it != m_groupMap.end() ) ? std::string_view{ it->second }
													   : internal::constants::codebook::GROUP_UNKNOWN;
				}

				uniqueGroups.emplace( group );

				if ( group == internal::constants::codebook::GROUP_DEFAULT )
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
