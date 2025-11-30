/**
 * @file ISO19848.cpp
 * @brief Implementation of ISO 19848 standard data access
 */

#include <nfx/cache/LruCache.h>
#include <nfx/StringUtils.h>

#include "dnv/vista/sdk/transport/ISO19848.h"
#include "dnv/vista/sdk/transport/ISO19848Constants.h"

#include "internal/core/EmbeddedResource.h"
#include "internal/transport/dto/ISO19848Dtos.h"

namespace dnv::vista::sdk
{
	namespace internal::iso19848
	{
		//=====================================================================
		// Internal Cache Infrastructure
		//=====================================================================

		//----------------------------------------------
		// Memory caching
		//----------------------------------------------

		//-----------------------------
		// DTOs
		//-----------------------------

		static auto dataChannelTypeNamesDtoCache{
			nfx::cache::LruCache<sdk::transport::ISO19848Version, internal::transport::DataChannelTypeNamesDto>{ nfx::cache::LruCacheOptions{ 10, std::chrono::hours{ 1 } } } };
		static auto formatDataTypesDtoCache{
			nfx::cache::LruCache<sdk::transport::ISO19848Version, internal::transport::FormatDataTypesDto>{ nfx::cache::LruCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		//-----------------------------
		// Domain objects
		//-----------------------------

		static auto dataChannelTypeNamesCache{
			nfx::cache::LruCache<sdk::transport::ISO19848Version, sdk::transport::DataChannelTypeNames>{ nfx::cache::LruCacheOptions{ 10, std::chrono::hours{ 1 } } } };
		static auto formatDataTypesCache{
			nfx::cache::LruCache<sdk::transport::ISO19848Version, sdk::transport::FormatDataTypes>{ nfx::cache::LruCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		//----------------------------------------------
		// Resource loading
		//----------------------------------------------

		static std::string_view iso19848ToVesionString( sdk::transport::ISO19848Version version )
		{
			switch ( version )
			{
				case sdk::transport::ISO19848Version::v2018:
				{
					return sdk::transport::ISO19848_v2018;
				}
				case sdk::transport::ISO19848Version::v2024:
				{
					return sdk::transport::ISO19848_v2024;
				}
				default:
				{
					throw std::invalid_argument{ "Invalid ISO19848Version" };
				}
			}
		}

		static std::optional<internal::transport::DataChannelTypeNamesDto> loadDataChannelTypeNamesDto( sdk::transport::ISO19848Version version )
		{
			return internal::EmbeddedResource::dataChannelTypeNames( iso19848ToVesionString( version ) );
		}

		static std::optional<internal::transport::FormatDataTypesDto> loadFormatDataTypesDto( sdk::transport::ISO19848Version version )
		{
			return internal::EmbeddedResource::formatDataTypes( iso19848ToVesionString( version ) );
		}

		//----------------------------------------------
		// DTO access
		//----------------------------------------------

		static const internal::transport::DataChannelTypeNamesDto& dataChannelTypeNamesDto( nfx::cache::LruCache<sdk::transport::ISO19848Version, internal::transport::DataChannelTypeNamesDto>& dtoCache, sdk::transport::ISO19848Version version )
		{
			return *dtoCache.get(
				version,
				[version]() -> internal::transport::DataChannelTypeNamesDto {
					const auto dto = loadDataChannelTypeNamesDto( version );
					if ( !dto.has_value() )
					{
						throw std::runtime_error{ "Invalid state: Failed to load DataChannelTypeNamesDto" };
					}

					return dto.value();
				},
				[]( nfx::cache::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		static const internal::transport::FormatDataTypesDto& formatDataTypesDto( nfx::cache::LruCache<sdk::transport::ISO19848Version, internal::transport::FormatDataTypesDto>& dtoCache, sdk::transport::ISO19848Version version )
		{
			return *dtoCache.get(
				version,
				[version]() -> internal::transport::FormatDataTypesDto {
					const auto dto = loadFormatDataTypesDto( version );
					if ( !dto.has_value() )
					{
						throw std::runtime_error{ "Invalid state: Failed to load FormatDataTypesDto" };
					}

					return dto.value();
				},
				[]( nfx::cache::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}
	} // namespace internal::iso19848

	namespace transport
	{
		//=====================================================================
		// ISO19848 class
		//=====================================================================

		//----------------------------------------------
		// Singleton access
		//----------------------------------------------

		ISO19848& ISO19848::instance() noexcept
		{
			static ISO19848 s_instance;
			return s_instance;
		}

		//----------------------------------------------
		// Singleton construction
		//----------------------------------------------

		ISO19848::ISO19848() noexcept
		{
		}

		//----------------------------------------------
		// Public interface
		//----------------------------------------------

		const DataChannelTypeNames& ISO19848::dataChannelTypeNames( ISO19848Version version )
		{
			return *internal::iso19848::dataChannelTypeNamesCache.get(
				version,
				[version]() -> DataChannelTypeNames {
					const auto dto = internal::iso19848::dataChannelTypeNamesDto( internal::iso19848::dataChannelTypeNamesDtoCache, version );

					std::vector<DataChannelTypeName> values;
					values.reserve( dto.values.size() );

					for ( const auto& x : dto.values )
					{
						values.emplace_back( x.type, x.description );
					}

					return DataChannelTypeNames{ std::move( values ) };
				},
				[]( nfx::cache::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		const FormatDataTypes& ISO19848::formatDataTypes( ISO19848Version version )
		{
		return *internal::iso19848::formatDataTypesCache.get(
				version,
				[version]() -> FormatDataTypes {
					const auto dto = internal::iso19848::formatDataTypesDto( internal::iso19848::formatDataTypesDtoCache, version );

					std::vector<FormatDataType> values;
					values.reserve( dto.values.size() );

					for ( const auto& x : dto.values )
					{
						values.emplace_back( x.type, x.description );
					}

					return FormatDataTypes{ std::move( values ) };
				},
				[]( nfx::cache::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		//=====================================================================
		// DataChannelTypeNames class
		//=====================================================================

		//----------------------------------------------
		// DataChannelTypeNames::ParseResult  class
		//----------------------------------------------

		//-----------------------------
		// Parsing
		//-----------------------------

		DataChannelTypeNames::ParseResult DataChannelTypeNames::parse( std::string_view type ) const
		{
			if ( nfx::string::isNullOrWhiteSpace( type ) )
			{
				return ParseResult{ ParseResult::Invalid{} };
			}

			const auto trimmedType = nfx::string::trim( type );

			for ( const auto& value : m_values )
			{
				if ( nfx::string::equals( value.type(), trimmedType ) )
				{
					return ParseResult{ ParseResult::Ok{ value } };
				}
			}

			return ParseResult{ ParseResult::Invalid{} };
		}

		//=====================================================================
		// FormatDataType class
		//=====================================================================

		//----------------------------------------------
		// Validation
		//----------------------------------------------

		ValidateResult FormatDataType::validate( std::string_view value, Value& outValue ) const
		{
			outValue = Value{ Value::String{ value } };

			if ( nfx::string::iequals( m_type, iso19848::formatdatatypes::DECIMAL ) )
			{
				nfx::datatypes::Decimal d;
				if ( !nfx::datatypes::Decimal::fromString( value, d ) )
				{
					return ValidateResult{ ValidateResult::Invalid{ { "Invalid decimal value - Value='" + std::string{ value } + "'" } } };
				}

				outValue = Value{ Value::Decimal{ d } };
				return ValidateResult{ ValidateResult::Ok{} };
			}
			else if ( nfx::string::iequals( m_type, iso19848::formatdatatypes::INTEGER ) )
			{
				int i;
				if ( !nfx::string::fromString<int>( value, i ) )
				{
					return ValidateResult{ ValidateResult::Invalid{ { "Invalid integer value - Value='" + std::string{ value } + "'" } } };
				}

				outValue = Value{ Value::Integer{ i } };
				return ValidateResult{ ValidateResult::Ok{} };
			}
			else if ( nfx::string::iequals( m_type, iso19848::formatdatatypes::BOOLEAN ) )
			{
				bool b;
				if ( !nfx::string::fromString<bool>( value, b ) )
				{
					return ValidateResult{ ValidateResult::Invalid{ { "Invalid boolean value - Value='" + std::string{ value } + "'" } } };
				}

				outValue = Value{ Value::Boolean{ b } };
				return ValidateResult{ ValidateResult::Ok{} };
			}
			else if ( nfx::string::iequals( m_type, iso19848::formatdatatypes::STRING ) )
			{
				return ValidateResult{ ValidateResult::Ok{} };
			}
			else if ( nfx::string::iequals( m_type, iso19848::formatdatatypes::DATETIME ) )
			{
				nfx::time::DateTimeOffset dt;
				if ( !nfx::time::DateTimeOffset::fromString( value, dt ) )
				{
					return ValidateResult{ ValidateResult::Invalid{ { "Invalid datetime value - Value='" + std::string{ value } + "'" } } };
				}

				outValue = Value{ Value::DateTime{ std::move( dt ) } };
				return ValidateResult{ ValidateResult::Ok{} };
			}
			else
			{
				throw std::runtime_error{ "Invalid format type " + m_type };
			}
		}

		//=====================================================================
		// FormatDataTypes class
		//=====================================================================

		//----------------------------------------------
		// FormatDataTypes::ParseResult class
		//----------------------------------------------

		//-----------------------------
		// Parsing
		//-----------------------------

		FormatDataTypes::ParseResult FormatDataTypes::parse( std::string_view type ) const
		{
			if ( nfx::string::isNullOrWhiteSpace( type ) )
			{
				return ParseResult{ ParseResult::Invalid{} };
			}

			const auto trimmedType = nfx::string::trim( type );

			for ( const auto& value : m_values )
			{
				if ( nfx::string::equals( value.type(), trimmedType ) )
				{
					return ParseResult{ ParseResult::Ok{ value } };
				}
			}

			return ParseResult{ ParseResult::Invalid{} };
		}
	} // namespace transport
} // namespace dnv::vista::sdk
