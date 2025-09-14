/**
 * @file ISO19848.cpp
 * @brief Implementation of ISO 19848 standard data access
 */

#include <nfx/containers/HashMap.h>
#include <nfx/memory/MemoryCache.h>
#include <nfx/string/Utils.h>

#include "dnv/vista/sdk/transport/ISO19848.h"
#include "dnv/vista/sdk/transport/ISO19848Constants.h"

#include "internal/constants/ISO19848.h"
#include "internal/core/EmbeddedResource.h"
#include "internal/transport/dto/ISO19848Dtos.h"

namespace dnv::vista::sdk
{
	namespace
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
			nfx::memory::MemoryCache<transport::ISO19848Version, internal::transport::DataChannelTypeNamesDto>{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours{ 1 } } } };
		static auto formatDataTypesDtoCache{
			nfx::memory::MemoryCache<transport::ISO19848Version, internal::transport::FormatDataTypesDto>{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		//-----------------------------
		// Domain objects
		//-----------------------------

		static auto dataChannelTypeNamesCache{
			nfx::memory::MemoryCache<transport::ISO19848Version, transport::DataChannelTypeNames>{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours{ 1 } } } };
		static auto formatDataTypesCache{
			nfx::memory::MemoryCache<transport::ISO19848Version, transport::FormatDataTypes>{ nfx::memory::MemoryCacheOptions{ 10, std::chrono::hours{ 1 } } } };

		//----------------------------------------------
		// Resource loading
		//----------------------------------------------

		static std::string_view iso19848ToVesionString( transport::ISO19848Version version )
		{
			switch ( version )
			{
				case transport::ISO19848Version::v2018:
				{
					return transport::ISO19848_v2018;
				}
				case transport::ISO19848Version::v2024:
				{
					return transport::ISO19848_v2024;
				}
				default:
				{
					throw std::invalid_argument{ "Invalid ISO19848Version" };
				}
			}
		}

		static std::optional<internal::transport::DataChannelTypeNamesDto> loadDataChannelTypeNamesDto( transport::ISO19848Version version )
		{
			return internal::EmbeddedResource::dataChannelTypeNames( iso19848ToVesionString( version ) );
		}

		static std::optional<internal::transport::FormatDataTypesDto> loadFormatDataTypesDto( transport::ISO19848Version version )
		{
			return internal::EmbeddedResource::formatDataTypes( iso19848ToVesionString( version ) );
		}

		//----------------------------------------------
		// DTO access
		//----------------------------------------------

		static const dnv::vista::sdk::internal::transport::DataChannelTypeNamesDto& dataChannelTypeNamesDto( nfx::memory::MemoryCache<transport::ISO19848Version, dnv::vista::sdk::internal::transport::DataChannelTypeNamesDto>& dtoCache, transport::ISO19848Version version )
		{
			return dtoCache.getOrCreate(
				version,
				[version]() -> dnv::vista::sdk::internal::transport::DataChannelTypeNamesDto {
					const auto dto = loadDataChannelTypeNamesDto( version );
					if ( !dto.has_value() )
					{
						throw std::runtime_error{ "Invalid state: Failed to load DataChannelTypeNamesDto" };
					}

					return dto.value();
				},
				[]( nfx::memory::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		static const dnv::vista::sdk::internal::transport::FormatDataTypesDto& formatDataTypesDto( nfx::memory::MemoryCache<transport::ISO19848Version, dnv::vista::sdk::internal::transport::FormatDataTypesDto>& dtoCache, transport::ISO19848Version version )
		{
			return dtoCache.getOrCreate(
				version,
				[version]() -> dnv::vista::sdk::internal::transport::FormatDataTypesDto {
					const auto dto = loadFormatDataTypesDto( version );
					if ( !dto.has_value() )
					{
						throw std::runtime_error{ "Invalid state: Failed to load FormatDataTypesDto" };
					}

					return dto.value();
				},
				[]( nfx::memory::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}
	}

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
			return dataChannelTypeNamesCache.getOrCreate(
				version,
				[version]() -> DataChannelTypeNames {
					const auto dto = dataChannelTypeNamesDto( dataChannelTypeNamesDtoCache, version );

					std::vector<DataChannelTypeName> values;
					values.reserve( dto.values().size() );

					for ( const auto& x : dto.values() )
					{
						values.emplace_back( x.type(), x.description() );
					}

					return DataChannelTypeNames{ std::move( values ) };
				},
				[]( nfx::memory::CacheEntry& entry ) {
					entry.size = 1;
					entry.slidingExpiration = std::chrono::hours( 1 );
				} );
		}

		const FormatDataTypes& ISO19848::formatDataTypes( ISO19848Version version )
		{
			return formatDataTypesCache.getOrCreate(
				version,
				[version]() -> FormatDataTypes {
					const auto dto = formatDataTypesDto( formatDataTypesDtoCache, version );

					std::vector<FormatDataType> values;
					values.reserve( dto.values().size() );

					for ( const auto& x : dto.values() )
					{
						values.emplace_back( x.type(), x.description() );
					}

					return FormatDataTypes{ std::move( values ) };
				},
				[]( nfx::memory::CacheEntry& entry ) {
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
			if ( !nfx::string::iequals( m_type, dnv::vista::sdk::internal::constants::iso19848::FORMAT_TYPE_STRING ) &&
				 nfx::string::isNullOrWhiteSpace( value ) )
			{
				return ValidateResult{ ValidateResult::Invalid{ { "Value cannot be null, empty, or whitespace for non-string types - Value='" + std::string{ value } + "'" } } };
			}

			const auto trimmedValue = nfx::string::iequals( m_type, dnv::vista::sdk::internal::constants::iso19848::FORMAT_TYPE_STRING ) ? value : nfx::string::trim( value );

			outValue = Value{ Value::String{ trimmedValue } };

			if ( nfx::string::iequals( m_type, dnv::vista::sdk::internal::constants::iso19848::FORMAT_TYPE_DECIMAL ) )
			{
				nfx::datatypes::Decimal d;
				if ( !nfx::datatypes::Decimal::tryParse( trimmedValue, d ) )
				{
					return ValidateResult{ ValidateResult::Invalid{ { "Invalid decimal value - Value='" + std::string{ trimmedValue } + "'" } } };
				}

				outValue = Value{ Value::Decimal{ d } };
				return ValidateResult{ ValidateResult::Ok{} };
			}
			else if ( nfx::string::iequals( m_type, dnv::vista::sdk::internal::constants::iso19848::FORMAT_TYPE_DOUBLE ) )
			{
				double d;
				if ( !nfx::string::tryParseDouble( trimmedValue, d ) )
				{
					return ValidateResult{ ValidateResult::Invalid{ { "Invalid double value - Value='" + std::string{ trimmedValue } + "'" } } };
				}

				outValue = Value{ Value::Double{ d } };
				return ValidateResult{ ValidateResult::Ok{} };
			}
			else if ( nfx::string::iequals( m_type, dnv::vista::sdk::internal::constants::iso19848::FORMAT_TYPE_INTEGER ) )
			{
				int i;
				if ( !nfx::string::tryParseInt( trimmedValue, i ) )
				{
					return ValidateResult{ ValidateResult::Invalid{ { "Invalid integer value - Value='" + std::string{ trimmedValue } + "'" } } };
				}

				outValue = Value{ Value::Integer{ i } };
				return ValidateResult{ ValidateResult::Ok{} };
			}
			else if ( nfx::string::iequals( m_type, dnv::vista::sdk::internal::constants::iso19848::FORMAT_TYPE_BOOLEAN ) )
			{
				bool b;
				if ( !nfx::string::tryParseBool( trimmedValue, b ) )
				{
					return ValidateResult{ ValidateResult::Invalid{ { "Invalid boolean value - Value='" + std::string{ trimmedValue } + "'" } } };
				}

				outValue = Value{ Value::Boolean{ b } };
				return ValidateResult{ ValidateResult::Ok{} };
			}
			else if ( nfx::string::iequals( m_type, dnv::vista::sdk::internal::constants::iso19848::FORMAT_TYPE_CHAR ) )
			{
				if ( !nfx::string::hasExactLength( trimmedValue, 1 ) )
				{
					return ValidateResult{ ValidateResult::Invalid{ { "Invalid char value - Value='" + std::string{ trimmedValue } + "'" } } };
				}

				outValue = Value{ Value::Char{ trimmedValue[0] } };
				return ValidateResult{ ValidateResult::Ok{} };
			}
			else if ( nfx::string::iequals( m_type, dnv::vista::sdk::internal::constants::iso19848::FORMAT_TYPE_UNSIGNED_INTEGER ) )
			{
				std::uint32_t ui;
				if ( !nfx::string::tryParseUInt( trimmedValue, ui ) )
				{
					return ValidateResult{ ValidateResult::Invalid{ { "Invalid unsigned integer value - Value='" + std::string{ trimmedValue } + "'" } } };
				}

				outValue = Value{ Value::UnsignedInteger{ ui } };
				return ValidateResult{ ValidateResult::Ok{} };
			}
			else if ( nfx::string::iequals( m_type, dnv::vista::sdk::internal::constants::iso19848::FORMAT_TYPE_LONG ) )
			{
				std::int64_t l;
				if ( !nfx::string::tryParseLong( trimmedValue, l ) )
				{
					return ValidateResult{ ValidateResult::Invalid{ { "Invalid long value - Value='" + std::string{ trimmedValue } + "'" } } };
				}

				outValue = Value{ Value::Long{ l } };
				return ValidateResult{ ValidateResult::Ok{} };
			}
			else if ( nfx::string::iequals( m_type, dnv::vista::sdk::internal::constants::iso19848::FORMAT_TYPE_STRING ) )
			{
				return ValidateResult{ ValidateResult::Ok{} };
			}
			else if ( nfx::string::iequals( m_type, dnv::vista::sdk::internal::constants::iso19848::FORMAT_TYPE_DATETIME ) )
			{
				nfx::time::DateTimeOffset dt;
				if ( !nfx::time::DateTimeOffset::tryParse( trimmedValue, dt ) )
				{
					return ValidateResult{ ValidateResult::Invalid{ { "Invalid datetime value - Value='" + std::string{ trimmedValue } + "'" } } };
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
	}
}
