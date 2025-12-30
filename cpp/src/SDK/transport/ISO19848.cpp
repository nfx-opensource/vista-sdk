/**
 * @file ISO19848.cpp
 * @brief Implementation of ISO 19848 time series data type system
 * @details Provides validation and parsing for data channel types and format data types per ISO 19848 standard
 */

#include "dnv/vista/sdk/transport/ISO19848.h"

#include "dnv/vista/sdk/detail/Containers.h"
#include "SDK/dto/ISO19848Dtos.h"
#include "ISO19848VersionsExtensions.h"

#include <EmbeddedResources/EmbeddedResources.h>

#include <nfx/StringUtils.h>

#include <algorithm>
#include <charconv>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>

namespace dnv::vista::sdk::transport
{
    namespace
    {
        auto& dataChannelTypeNamesCache()
        {
            static HashMap<ISO19848Version, DataChannelTypeNames> cache;
            return cache;
        }

        auto& dataChannelTypeNamesMutex()
        {
            static std::shared_mutex mutex;
            return mutex;
        }

        auto& formatDataTypesCache()
        {
            static HashMap<ISO19848Version, FormatDataTypes> cache;
            return cache;
        }

        auto& formatDataTypesMutex()
        {
            static std::shared_mutex mutex;
            return mutex;
        }
    } // namespace

    ParseResult<DataChannelTypeName> DataChannelTypeNames::fromString( std::string_view dataChannelTypeName ) const
    {
        if( nfx::string::isNullOrWhiteSpace( dataChannelTypeName ) )
        {
            return ParseResult<DataChannelTypeName>::invalid();
        }

        const auto trimmedType = nfx::string::trim( dataChannelTypeName );

        for( const auto& value : m_values )
        {
            if( nfx::string::equals( value.type, trimmedType ) )
            {
                return ParseResult<DataChannelTypeName>::ok( value );
            }
        }

        return ParseResult<DataChannelTypeName>::invalid();
    }

    ValidateResult<Value> FormatDataType::validate( std::string_view value ) const
    {
        if( nfx::string::iequals( type, "String" ) )
        {
            return ValidateResult<Value>::ok( Value::String{ std::string{ value } } );
        }
        else if( nfx::string::iequals( type, "Boolean" ) )
        {
            bool b;
            if( !nfx::string::fromString( value, b ) )
            {
                return ValidateResult<Value>::invalid(
                    { "Invalid boolean value - Value='" + std::string{ value } + "'" } );
            }
            return ValidateResult<Value>::ok( Value::Boolean{ b } );
        }
        else if( nfx::string::iequals( type, "Integer" ) )
        {
            int32_t i;
            if( !nfx::string::fromString( value, i ) )
            {
                return ValidateResult<Value>::invalid(
                    { "Invalid integer value - Value='" + std::string{ value } + "'" } );
            }
            return ValidateResult<Value>::ok( Value::Integer{ i } );
        }
        else if( nfx::string::iequals( type, "Decimal" ) )
        {
            try
            {
                return ValidateResult<Value>::ok( Value::Decimal{ value } );
            }
            catch( const std::exception& ex )
            {
                return ValidateResult<Value>::invalid(
                    { "Invalid decimal value - Value='" + std::string{ value } + "'. " + ex.what() } );
            }
        }
        else if( nfx::string::iequals( type, "DateTime" ) )
        {
            try
            {
                return ValidateResult<Value>::ok( Value::DateTime{ value } );
            }
            catch( const std::exception& ex )
            {
                return ValidateResult<Value>::invalid( { "Invalid datetime value - Value='" + std::string{ value } +
                                                         "'. Expected format: yyyy-MM-ddTHH:mm:ssZ. " + ex.what() } );
            }
        }
        else
        {
            throw std::invalid_argument{ "Invalid format type: " + type };
        }
    }

    ParseResult<FormatDataType> FormatDataTypes::fromString( std::string_view formatDataType ) const
    {
        if( nfx::string::isNullOrWhiteSpace( formatDataType ) )
        {
            return ParseResult<FormatDataType>::invalid();
        }

        const auto trimmedType = nfx::string::trim( formatDataType );

        for( const auto& value : m_values )
        {
            if( nfx::string::equals( value.type, trimmedType ) )
            {
                return ParseResult<FormatDataType>::ok( value );
            }
        }

        return ParseResult<FormatDataType>::invalid();
    }

    ISO19848& ISO19848::instance()
    {
        static ISO19848 instance;
        return instance;
    }

    const std::vector<ISO19848Version>& ISO19848::versions() const noexcept
    {
        static const std::vector<ISO19848Version> versions = []() {
            const auto arr = ISO19848Versions::all();
            return std::vector<ISO19848Version>( arr.begin(), arr.end() );
        }();

        return versions;
    }

    ISO19848Version ISO19848::latest() const noexcept
    {
        return ISO19848Versions::latest();
    }

    DataChannelTypeNames ISO19848::dataChannelTypeNames( ISO19848Version version )
    {
        // Fast path: read-only access
        {
            std::shared_lock lock( dataChannelTypeNamesMutex() );
            if( auto* cached = dataChannelTypeNamesCache().find( version ) )
            {
                return *cached;
            }
        }

        // Slow path: load and cache
        std::unique_lock lock( dataChannelTypeNamesMutex() );

        // Double-check (another thread might have loaded it)
        if( auto* cached = dataChannelTypeNamesCache().find( version ) )
        {
            return *cached;
        }

        // Load from embedded resource
        auto versionStr = ISO19848Versions::toString( version );
        auto dto = EmbeddedResources::dataChannelTypeNames( versionStr );

        if( !dto.has_value() )
        {
            throw std::out_of_range{ "DataChannelTypeNames not available for version: " + std::string{ versionStr } };
        }

        // Build DataChannelTypeName vector from DTO
        std::vector<DataChannelTypeName> values;
        values.reserve( dto->values.size() );

        for( const auto& entry : dto->values )
        {
            values.emplace_back( entry.type, entry.description );
        }

        dataChannelTypeNamesCache().emplace( version, DataChannelTypeNames{ std::move( values ) } );
        return *dataChannelTypeNamesCache().find( version );
    }

    FormatDataTypes ISO19848::formatDataTypes( ISO19848Version version )
    {
        // Fast path: read-only access
        {
            std::shared_lock lock( formatDataTypesMutex() );
            if( auto* cached = formatDataTypesCache().find( version ) )
            {
                return *cached;
            }
        }

        // Slow path: load and cache
        std::unique_lock lock( formatDataTypesMutex() );

        // Double-check
        if( auto* cached = formatDataTypesCache().find( version ) )
        {
            return *cached;
        }

        // Load from embedded resource
        auto versionStr = ISO19848Versions::toString( version );
        auto dto = EmbeddedResources::formatDataTypes( versionStr );

        if( !dto.has_value() )
        {
            throw std::out_of_range{ "FormatDataTypes not available for version: " + std::string{ versionStr } };
        }

        // Build FormatDataType vector from DTO
        std::vector<FormatDataType> values;
        values.reserve( dto->values.size() );

        for( const auto& entry : dto->values )
        {
            values.emplace_back( entry.type, entry.description );
        }

        formatDataTypesCache().emplace( version, FormatDataTypes{ std::move( values ) } );
        return *formatDataTypesCache().find( version );
    }
} // namespace dnv::vista::sdk::transport
