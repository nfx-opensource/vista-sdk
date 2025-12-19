/**
 * @file ImoNumber.cpp
 * @brief Implementation of ImoNumber class methods
 * @details Provides constructors, validation logic, parsing, and string conversion for IMO numbers
 */

#include "dnv/vista/sdk/ImoNumber.h"

#include <nfx/StringUtils.h>

#include <stdexcept>

namespace dnv::vista::sdk
{
    ImoNumber::ImoNumber( int value )
    {
        if( !isValid( value ) )
        {
            throw std::invalid_argument{ "Invalid IMO number: " + std::to_string( value ) };
        }

        m_value = value;
    }

    ImoNumber::ImoNumber( std::string_view value )
    {
        if( nfx::string::isEmpty( value ) )
        {
            throw std::invalid_argument{ "Invalid IMO number: " + std::string{ value } };
        }

        if( nfx::string::isNullOrWhiteSpace( value ) )
        {
            throw std::invalid_argument{ "Invalid IMO number: " + std::string{ value } };
        }

        std::string_view sv = value;
        if( sv.length() >= 3 && nfx::string::iequals( sv.substr( 0, 3 ), "IMO" ) )
        {
            sv = sv.substr( 3 );
        }

        int num = 0;
        if( !nfx::string::fromString<int>( sv, num ) )
        {
            throw std::invalid_argument{ "Invalid IMO number: " + std::string{ value } };
        }
        if( num == 0 || !isValid( num ) )
        {
            throw std::invalid_argument{ "Invalid IMO number: " + std::string{ value } };
        }

        m_value = num;
    }

    std::optional<ImoNumber> ImoNumber::fromString( std::string_view value ) noexcept
    {
        if( nfx::string::isNullOrWhiteSpace( value ) )
        {
            return std::nullopt;
        }

        std::string_view sv = value;
        if( sv.length() >= 3 && nfx::string::iequals( sv.substr( 0, 3 ), "IMO" ) )
        {
            sv = sv.substr( 3 );
        }

        int num = 0;
        if( !nfx::string::fromString<int>( sv, num ) )
        {
            return std::nullopt;
        }

        if( num == 0 || !isValid( num ) )
        {
            return std::nullopt;
        }

        return ImoNumber{ num };
    }

    std::string ImoNumber::toString() const
    {
        return "IMO" + std::to_string( m_value );
    }
} // namespace dnv::vista::sdk
