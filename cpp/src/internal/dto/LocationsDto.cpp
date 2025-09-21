/**
 * @file LocationsDto.cpp
 * @brief Implementation of data transfer objects for locations in the VIS standard
 */

#include <cstdio>

#include <nfx/string/StringBuilderPool.h>
#include <nfx/string/Utils.h>

#include "LocationsDto.h"

#include "internal/dto/Keys.h"

namespace dnv::vista::sdk
{
	namespace internal
	{
		//=====================================================================
		// JSON parsing helper functions
		//=====================================================================

		static std::string_view extractCodeHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				const auto it = json.find( dto::locations::KEY_CODE );
				if ( it != json.end() && it->is_string() )
				{
					const auto& str = it->get_ref<const std::string&>();

					return std::string_view{ str };
				}

				return dto::error::UNKNOWN_CODE;
			}
			catch ( ... )
			{
				return dto::error::UNKNOWN_CODE;
			}
		}

		static std::string_view extractVisHint( const nlohmann::json& json ) noexcept
		{
			try
			{
				const auto it = json.find( dto::KEY_VIS_RELEASE );
				if ( it != json.end() && it->is_string() )
				{
					const auto& str = it->get_ref<const std::string&>();

					return std::string_view{ str };
				}

				return dto::error::UNKNOWN_VERSION;
			}
			catch ( ... )
			{
				return dto::error::UNKNOWN_VERSION;
			}
		}
	}

	//=====================================================================
	// Relative Location data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	RelativeLocationsDto::RelativeLocationsDto( char code, std::string name, std::optional<std::string> definition ) noexcept
		: m_code{ code },
		  m_name{ std::move( name ) },
		  m_definition{ std::move( definition ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	char RelativeLocationsDto::code() const noexcept
	{
		return m_code;
	}

	std::string_view RelativeLocationsDto::name() const noexcept
	{
		return m_name;
	}

	const std::optional<std::string>& RelativeLocationsDto::definition() const noexcept
	{
		return m_definition;
	}

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<RelativeLocationsDto> RelativeLocationsDto::tryFromJson( const nlohmann::json& json )
	{
		const auto codeHint = internal::extractCodeHint( json );

		try
		{
			if ( !json.is_object() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: JSON value for RelativeLocationsDto is not an object\n" );

				std::fprintf( stderr, "%s", lease.toString().c_str() );

				return std::nullopt;
			}

			// Cache iterators to avoid multiple lookups
			const auto codeIt = json.find( internal::dto::locations::KEY_CODE );
			const auto nameIt = json.find( internal::dto::locations::KEY_NAME );
			const auto defIt = json.find( internal::dto::locations::KEY_DEFINITION );

			if ( codeIt == json.end() || !codeIt->is_string() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: RelativeLocationsDto JSON missing required '" );
				builder.append( internal::dto::locations::KEY_CODE );
				builder.append( "' field or not a string\n" );
				std::fprintf( stderr, "%s", lease.toString().c_str() );

				return std::nullopt;
			}
			if ( nameIt == json.end() || !nameIt->is_string() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: RelativeLocationsDto JSON missing required '" );
				builder.append( internal::dto::locations::KEY_NAME );
				builder.append( "' field or not a string\n" );
				std::fprintf( stderr, "%s", lease.toString().c_str() );

				return std::nullopt;
			}

			std::string codeStr = codeIt->get<std::string>();
			if ( nfx::string::isEmpty( codeStr ) || !nfx::string::hasExactLength( codeStr, 1 ) )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: RelativeLocationsDto (hint: code='" );
				builder.append( codeHint );
				builder.append( "') has invalid code format\n" );

				std::fprintf( stderr, "%s", lease.toString().c_str() );

				return std::nullopt;
			}

			std::string tempName = nameIt->get<std::string>();
			if ( nfx::string::isEmpty( tempName ) )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "WARN: Empty name field found in RelativeLocationsDto code='" );
				builder.append( codeStr );
				builder.append( "'\n" );

				std::fprintf( stderr, "%s", lease.toString().c_str() );
			}

			char tempCode = codeStr[0];
			std::optional<std::string> tempDefinition = std::nullopt;

			if ( defIt != json.end() )
			{
				if ( defIt->is_string() )
				{
					tempDefinition = defIt->get<std::string>();
				}
				else if ( !defIt->is_null() )
				{
					auto lease = nfx::string::StringBuilderPool::lease();
					auto builder = lease.builder();
					builder.append( "WARN: RelativeLocationsDto code='" );
					builder.append( codeStr );
					builder.append( "' has non-string definition field\n" );

					std::fprintf( stderr, "%s", lease.toString().c_str() );
				}
			}

			RelativeLocationsDto result( std::move( tempCode ), std::move( tempName ), std::move( tempDefinition ) );

			return result;
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during RelativeLocationsDto parsing (hint: code='" );
			builder.append( codeHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );

			return std::nullopt;
		}
		catch ( const std::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: Standard exception during RelativeLocationsDto parsing (hint: code='" );
			builder.append( codeHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );

			return std::nullopt;
		}
	}

	RelativeLocationsDto RelativeLocationsDto::fromJson( const nlohmann::json& json )
	{
		auto dtoOpt = RelativeLocationsDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			throw std::invalid_argument{ "Failed to deserialize RelativeLocationsDto from JSON" };
		}

		return std::move( dtoOpt ).value();
	}

	//----------------------------------------------
	// Private serialization methods
	//---------------------------------------------

	nlohmann::json RelativeLocationsDto::toJson() const
	{
		nlohmann::json j = *this;

		return j;
	}

	void to_json( nlohmann::json& j, const RelativeLocationsDto& dto )
	{
		j = nlohmann::json{ { internal::dto::locations::KEY_CODE, std::string{ 1, dto.m_code } }, { internal::dto::locations::KEY_NAME, dto.m_name } };

		if ( dto.m_definition.has_value() )
		{
			j[internal::dto::locations::KEY_DEFINITION] = dto.m_definition.value();
		}
	}

	void from_json( const nlohmann::json& j, RelativeLocationsDto& dto )
	{
		const auto codeIt = j.find( internal::dto::locations::KEY_CODE );
		const auto nameIt = j.find( internal::dto::locations::KEY_NAME );
		const auto defIt = j.find( internal::dto::locations::KEY_DEFINITION );

		if ( codeIt == j.end() || !codeIt->is_string() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u, "Missing or invalid 'code' field", nullptr );
		}
		if ( nameIt == j.end() || !nameIt->is_string() )
		{
			throw nlohmann::json::parse_error::create( 101, 0u, "Missing or invalid 'name' field", nullptr );
		}

		std::string codeStr = codeIt->get<std::string>();
		if ( nfx::string::isEmpty( codeStr ) || !nfx::string::hasExactLength( codeStr, 1 ) )
		{
			throw nlohmann::json::type_error::create( 302, "'code' field must be single character", nullptr );
		}

		dto.m_code = codeStr[0];
		dto.m_name = nameIt->get<std::string>();
		if ( nfx::string::isEmpty( dto.m_name ) )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "WARN: Empty name field found in RelativeLocationsDto code='" );
			builder.append( std::string{ 1, dto.m_code } );
			builder.append( "'\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );
		}

		if ( defIt != j.end() && defIt->is_string() )
		{
			dto.m_definition = defIt->get<std::string>();

			if ( dto.m_definition.has_value() && nfx::string::isEmpty( *dto.m_definition ) )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "WARN: Empty definition field found in RelativeLocationsDto code='" );
				builder.append( std::string{ 1, dto.m_code } );
				builder.append( "'\n" );

				std::fprintf( stderr, "%s", lease.toString().c_str() );
			}
		}
		else
		{
			dto.m_definition.reset();
		}
	}

	//=====================================================================
	// Location data transfer objects
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	LocationsDto::LocationsDto( std::string visVersion, std::vector<RelativeLocationsDto> items ) noexcept
		: m_visVersion{ std::move( visVersion ) },
		  m_items{ std::move( items ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	std::string_view LocationsDto::visVersion() const noexcept
	{
		return m_visVersion;
	}

	const std::vector<RelativeLocationsDto>& LocationsDto::items() const noexcept
	{
		return m_items;
	}

	//----------------------------------------------
	// Serialization
	//----------------------------------------------

	std::optional<LocationsDto> LocationsDto::tryFromJson( const nlohmann::json& json )
	{
		const auto visHint = internal::extractVisHint( json );

		try
		{
			if ( !json.is_object() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: JSON value for LocationsDto is not an object\n" );

				std::fprintf( stderr, "%s", lease.toString().c_str() );

				return std::nullopt;
			}

			// Cache iterators to avoid multiple lookups
			const auto visIt = json.find( internal::dto::KEY_VIS_RELEASE );
			const auto itemsIt = json.find( internal::dto::locations::KEY_ITEMS );

			if ( visIt == json.end() || !visIt->is_string() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: LocationsDto JSON missing required '" );
				builder.append( internal::dto::KEY_VIS_RELEASE );
				builder.append( "' field or not a string\n" );

				std::fprintf( stderr, "%s", lease.toString().c_str() );

				return std::nullopt;
			}
			if ( itemsIt == json.end() || !itemsIt->is_array() )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "ERROR: LocationsDto JSON missing required '" );
				builder.append( internal::dto::locations::KEY_ITEMS );
				builder.append( "' array\n" );

				std::fprintf( stderr, "%s", lease.toString().c_str() );

				return std::nullopt;
			}

			LocationsDto dto = json.get<LocationsDto>();

			return std::optional<LocationsDto>{ std::move( dto ) };
		}
		catch ( const nlohmann::json::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: JSON exception during LocationsDto parsing (hint: visRelease='" );
			builder.append( visHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );

			return std::nullopt;
		}
		catch ( const std::exception& ex )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "ERROR: Standard exception during LocationsDto parsing (hint: visRelease='" );
			builder.append( visHint );
			builder.append( "'): " );
			builder.append( ex.what() );
			builder.append( "\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );

			return std::nullopt;
		}
	}

	LocationsDto LocationsDto::fromJson( const nlohmann::json& json )
	{
		auto dtoOpt = LocationsDto::tryFromJson( json );
		if ( !dtoOpt.has_value() )
		{
			throw std::invalid_argument{ "Failed to deserialize LocationsDto from JSON" };
		}

		return std::move( dtoOpt ).value();
	}

	nlohmann::json LocationsDto::toJson() const
	{
		return *this;
	}

	//----------------------------------------------
	// Private serialization methods
	//----------------------------------------------

	void to_json( nlohmann::json& j, const LocationsDto& dto )
	{
		j = nlohmann::json{
			{ internal::dto::KEY_VIS_RELEASE, dto.m_visVersion },
			{ internal::dto::locations::KEY_ITEMS, dto.m_items } };
	}

	void from_json( const nlohmann::json& j, LocationsDto& dto )
	{
		dto.m_items.clear();

		const auto visIt = j.find( internal::dto::KEY_VIS_RELEASE );
		const auto itemsIt = j.find( internal::dto::locations::KEY_ITEMS );

		if ( visIt == j.end() || !visIt->is_string() )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "LocationsDto JSON missing required '" );
			builder.append( internal::dto::KEY_VIS_RELEASE );
			builder.append( "' field" );

			throw nlohmann::json::parse_error::create(
				101, 0u, lease.toString(), nullptr );
		}
		if ( itemsIt == j.end() || !itemsIt->is_array() )
		{
			auto lease2 = nfx::string::StringBuilderPool::lease();
			auto builder2 = lease2.builder();
			builder2.append( "LocationsDto JSON missing required '" );
			builder2.append( internal::dto::locations::KEY_ITEMS );
			builder2.append( "' field" );

			throw nlohmann::json::parse_error::create(
				101, 0u, lease2.toString(), nullptr );
		}

		dto.m_visVersion = visIt->get<std::string>();

		if ( nfx::string::isEmpty( dto.m_visVersion ) )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "WARN: Empty visVersion field found in LocationsDto\n" );

			std::fprintf( stderr, "%s", lease.toString().c_str() );
		}

		const auto& jsonArray = *itemsIt;
		const size_t totalItems = jsonArray.size();
		size_t successCount = 0;

		const size_t reserveSize = totalItems < 1000
									   ? totalItems + totalItems / 4
									   : totalItems + totalItems / 16;

		dto.m_items.reserve( reserveSize );

		for ( const auto& itemJson : jsonArray )
		{
			auto itemOpt = RelativeLocationsDto::tryFromJson( itemJson );
			if ( itemOpt.has_value() )
			{
				dto.m_items.emplace_back( std::move( *itemOpt ) );
				successCount++;
			}
			else
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "WARN: Skipping invalid RelativeLocationsDto item during parsing\n" );

				std::fprintf( stderr, "%s", lease.toString().c_str() );
			}
		}

		if ( totalItems > 0 && successCount < totalItems * 9 / 10 )
		{
			if ( dto.m_items.capacity() > dto.m_items.size() * 4 / 3 )
			{
				dto.m_items.shrink_to_fit();
			}
		}
	}
}
