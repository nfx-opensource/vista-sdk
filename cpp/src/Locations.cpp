/**
 * @file Locations.cpp
 * @brief Implements the Locations, Location, RelativeLocation, and related helper classes.
 */

#include <nfx/Hashing.h>
#include <nfx/string/StringBuilder.h>
#include <nfx/string/Utils.h>

#include "dnv/vista/sdk/Locations.h"

#include "internal/core/Locations.h"
#include "internal/dto/LocationsDto.h"
#include "internal/parsing/LocationCharDict.h"
#include "internal/parsing/LocationParsingErrorBuilder.h"
#include "dnv/vista/sdk/config/config.h"
#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk
{
	namespace internal::locations
	{
		/**
		 * @brief Convert a LocationGroup enum value to its string representation
		 * @param group The LocationGroup enum value to convert
		 * @return String view representation of the location group
		 */
		static constexpr std::string_view groupNameToString( LocationGroup group )
		{
			switch ( group )
			{
				case LocationGroup::Number:
				{
					return GROUP_NAME_NUMBER;
				}
				case LocationGroup::Side:
				{
					return GROUP_NAME_SIDE;
				}
				case LocationGroup::Vertical:
				{
					return GROUP_NAME_VERTICAL;
				}
				case LocationGroup::Transverse:
				{
					return GROUP_NAME_TRANSVERSE;
				}
				case LocationGroup::Longitudinal:
				{
					return GROUP_NAME_LONGITUDINAL;
				}
				default:
				{
					return GROUP_NAME_UNKNOWN;
				}
			}
		}

		//----------------------------------------------
		// Internal parsing implementation
		//----------------------------------------------

		/**
		 * @brief Internal core method to parse a location string
		 * @param locationCodes The set of valid location codes for quick lookup
		 * @param reversedGroups The map from character codes to their location groups
		 * @param span The string_view representing the current segment of the location string to parse
		 * @param originalStr The original, full location string (optional, used for context in errors)
		 * @param locationStringToParse Output parameter: if parsing succeeds, this is set to the parsed location string
		 * @param errorBuilder The `LocationParsingErrorBuilder` to accumulate errors
		 * @return True if parsing was successful to the point of forming a valid `Location`, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		static bool tryParse(
			const std::unordered_set<char>& locationCodes, const std::map<char, LocationGroup>& reversedGroups,
			std::string_view span, const std::optional<std::string>& originalStr, std::string& locationStringToParse,
			LocationParsingErrorBuilder& errorBuilder )
		{
			auto displayString =
				[&span, &originalStr]() -> std::string { return originalStr.has_value()
																	? *originalStr
																	: std::string{ span }; };

			if ( nfx::string::isNullOrWhiteSpace( span ) )
			{
				errorBuilder.addError(
					LocationValidationResult::NullOrWhiteSpace,
					"Invalid location: contains only whitespace" );

				return false;
			}

			std::string result;
			internal::LocationCharDict charDict{};

			int digitStartIndex = -1;
			int prevDigitIndex = -1;
			int charsStartIndex = -1;

			for ( size_t i = 0; i < span.length(); ++i )
			{
				char ch = span[i];

				if ( nfx::string::isDigit( ch ) )
				{
					if ( prevDigitIndex != -1 && prevDigitIndex != static_cast<int>( i ) - 1 )
					{
						auto lease = nfx::string::StringBuilderPool::lease();
						auto builder = lease.create();
						builder.append( "Invalid location: cannot have multiple separated digits in location: '" );
						builder.append( displayString() );
						builder.append( "'" );

						errorBuilder.addError( LocationValidationResult::Invalid, lease.toString() );

						return false;
					}

					if ( charsStartIndex != -1 )
					{
						auto lease = nfx::string::StringBuilderPool::lease();
						auto builder = lease.create();
						builder.append( "Invalid location: numeric location should start before location code(s) in location: '" );
						builder.append( displayString() );
						builder.append( "'" );

						errorBuilder.addError( LocationValidationResult::InvalidOrder, lease.toString() );

						return false;
					}

					if ( digitStartIndex == -1 )
					{
						digitStartIndex = static_cast<int>( i );
					}

					prevDigitIndex = static_cast<int>( i );

					result.push_back( ch );

					continue;
				}

				if ( charsStartIndex == -1 )
				{
					charsStartIndex = static_cast<int>( i );
				}

				bool valid = locationCodes.find( ch ) != locationCodes.end();

				if ( !valid )
				{
					auto lease = nfx::string::StringBuilderPool::lease();
					auto builder = lease.create();
					const std::string& source = displayString();
					bool first = true;

					for ( char c : source )
					{
						if ( !nfx::string::isDigit( c ) &&
							 ( c == CHAR_NUMBER || locationCodes.find( c ) == locationCodes.end() ) )
						{
							if ( !first )
							{
								builder.append( "," );
							}
							first = false;
							builder.append( "'" );
							builder.append( std::string_view{ &c, 1 } );
							builder.append( "'" );
						}
					}

					auto errorLease = nfx::string::StringBuilderPool::lease();
					auto errorMsgBuilder = errorLease.create();
					errorMsgBuilder.append( "Invalid location code: '" );
					errorMsgBuilder.append( displayString() );
					errorMsgBuilder.append( "' with invalid location code(s): " );
					errorMsgBuilder.append( lease.toString() );

					errorBuilder.addError( LocationValidationResult::InvalidCode, errorLease.toString() );

					return false;
				}

				if ( reversedGroups.find( ch ) != reversedGroups.end() )
				{
					LocationGroup group = reversedGroups.at( ch );
					std::optional<char> existingValue;

					if ( !charDict.tryAdd( group, ch, existingValue ) )
					{
						const std::string_view groupName = groupNameToString( group );
						auto lease = nfx::string::StringBuilderPool::lease();
						auto builder = lease.create();
						builder.append( "Invalid location: Multiple '" );
						builder.append( groupName );
						builder.append( "' values. Got both '" );
						builder.append( std::string_view{ &existingValue.value(), 1 } );
						builder.append( "' and '" );
						builder.append( std::string_view{ &ch, 1 } );
						builder.append( "' in '" );
						builder.append( displayString() );
						builder.append( "'" );

						errorBuilder.addError( LocationValidationResult::Invalid, lease.toString() );

						return false;
					}
				}

				if ( i > 0 && charsStartIndex != static_cast<int>( i ) )
				{
					char prevCh = span[i - 1];
					if ( !nfx::string::isDigit( prevCh ) && ch < prevCh )
					{
						auto lease = nfx::string::StringBuilderPool::lease();
						auto builder = lease.create();
						builder.append( "Invalid location: '" );
						builder.append( displayString() );
						builder.append( "' not alphabetically sorted" );

						errorBuilder.addError( LocationValidationResult::InvalidOrder, lease.toString() );

						return false;
					}
				}

				result.push_back( ch );
			}

			locationStringToParse = originalStr.has_value()
										? *originalStr
										: result;

			return true;
		}
	} // namespace internal::locations

	//=====================================================================
	// Location Class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Location::Location( std::string_view value )
		: m_hashCode{ static_cast<std::size_t>(
			  nfx::hashing::hash<std::string_view, uint32_t, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS>( value ) ) },
		  m_value{ value }
	{
	}

	//=====================================================================
	// RelativeLocation Class
	//=====================================================================

	RelativeLocation::RelativeLocation(
		char code, std::string_view name, const Location& location, const std::optional<std::string> definition )
		: m_code{ code },
		  m_name{ name },
		  m_location{ location },
		  m_definition{ definition }
	{
	}

	//=====================================================================
	// Locations Class
	//=====================================================================

	Locations::Locations( VisVersion version, const LocationsDto& dto )
		: m_visVersion{ version }
	{
		m_locationCodes.reserve( dto.items.size() );
		for ( const auto& item : dto.items )
		{
			m_locationCodes.emplace( item.code );
		}

		m_relativeLocations.reserve( dto.items.size() );
		for ( const auto& relLocDto : dto.items )
		{
			auto code = relLocDto.code;
			Location loc{ std::string{ 1, code } };

			m_relativeLocations.push_back( RelativeLocation{ code, relLocDto.name, loc, relLocDto.definition } );

			if ( code == internal::locations::CHAR_HORIZONTAL ||
				 code == internal::locations::CHAR_VERTICAL )
			{
				continue;
			}

			LocationGroup key;
			if ( code == internal::locations::CHAR_NUMBER )
			{
				key = LocationGroup::Number;
			}
			else if ( code == internal::locations::CHAR_PORT ||
					  code == internal::locations::CHAR_CENTER ||
					  code == internal::locations::CHAR_STARBOARD )
			{
				key = LocationGroup::Side;
			}
			else if ( code == internal::locations::CHAR_UPPER ||
					  code == internal::locations::CHAR_MIDDLE ||
					  code == internal::locations::CHAR_LOWER )
			{
				key = LocationGroup::Vertical;
			}
			else if ( code == internal::locations::CHAR_INBOARD ||
					  code == internal::locations::CHAR_OUTBOARD )
			{
				key = LocationGroup::Transverse;
			}
			else if ( code == internal::locations::CHAR_FORWARD ||
					  code == internal::locations::CHAR_AFT )
			{
				key = LocationGroup::Longitudinal;
			}
			else
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.create();
				builder.append( "Unsupported code: " );
				builder.append( std::string_view{ &code, 1 } );

				throw std::invalid_argument{ lease.toString() };
			}

			if ( m_groups.find( key ) == m_groups.end() )
			{
				m_groups[key] = std::vector<RelativeLocation>();
			}

			if ( key == LocationGroup::Number )
			{
				continue;
			}

			m_reversedGroups[code] = key;
			m_groups[key].push_back( RelativeLocation{ code, relLocDto.name, loc, relLocDto.definition } );
		}
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	VisVersion Locations::visVersion() const noexcept
	{
		return m_visVersion;
	}

	const std::vector<RelativeLocation>& Locations::relativeLocations() const noexcept
	{
		return m_relativeLocations;
	}

	const std::unordered_map<LocationGroup, std::vector<RelativeLocation>>& Locations::groups() const noexcept
	{
		return m_groups;
	}

	const std::map<char, LocationGroup>& Locations::reversedGroups() const noexcept
	{
		return m_reversedGroups;
	}

	//----------------------------------------------
	// Public methods - Parsing
	//----------------------------------------------

	Location Locations::parse( std::string_view locationStr ) const
	{
		Location location;
		if ( !tryParse( locationStr, location ) )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.create();
			builder.append( "Invalid location: " );
			builder.append( locationStr );
			throw std::invalid_argument{ lease.toString() };
		}

		return location;
	}

	bool Locations::tryParse( const std::optional<std::string>& value, Location& location, ParsingErrors& errors ) const
	{
		if ( !value.has_value() )
		{
			internal::LocationParsingErrorBuilder errorBuilder = internal::LocationParsingErrorBuilder::create();

			errorBuilder.addError( internal::LocationValidationResult::NullOrWhiteSpace, "Location is null" );

			errors = errorBuilder.build();

			location = Location{};

			return false;
		}

		internal::LocationParsingErrorBuilder errorBuilder = internal::LocationParsingErrorBuilder::create();
		std::string parsedLocationString;

		bool result = internal::locations::tryParse( m_locationCodes, m_reversedGroups, value.value(), value, parsedLocationString, errorBuilder );
		errors = errorBuilder.build();

		if ( result )
		{
			location = Location{ parsedLocationString };
		}

		return result;
	}

	bool Locations::tryParse( std::string_view value, Location& location ) const
	{
		internal::LocationParsingErrorBuilder errorBuilder;
		std::string parsedLocationString;

		if ( internal::locations::tryParse( m_locationCodes, m_reversedGroups, value, std::nullopt, parsedLocationString, errorBuilder ) )
		{
			location = Location{ parsedLocationString };
			return true;
		}

		return false;
	}

	bool Locations::tryParse( std::string_view value, Location& location, ParsingErrors& errors ) const
	{
		internal::LocationParsingErrorBuilder errorBuilder = internal::LocationParsingErrorBuilder::create();
		std::string parsedLocationString;

		bool result = internal::locations::tryParse( m_locationCodes, m_reversedGroups, value, std::nullopt, parsedLocationString, errorBuilder );
		errors = errorBuilder.build();

		if ( result )
		{
			location = Location{ parsedLocationString };
		}

		return result;
	}
} // namespace dnv::vista::sdk
