/**
 * @file ShipId.cpp
 * @brief Implementation of ShipId class methods
 * @details Provides constructors, parsing, and string conversion for ship identifiers
 */

#include "dnv/vista/sdk/transport/ShipId.h"

#include <nfx/StringUtils.h>

#include <stdexcept>

namespace dnv::vista::sdk
{
    ShipId::ShipId( const ImoNumber& imoNumber )
        : m_value{ imoNumber }
    {
    }

    ShipId::ShipId( std::string otherId )
        : m_value{ std::move( otherId ) }
    {
        if( std::holds_alternative<std::string>( m_value ) && std::get<std::string>( m_value ).empty() )
        {
            throw std::invalid_argument{ "ShipId otherId cannot be empty" };
        }
    }

    bool ShipId::operator==( const ShipId& other ) const noexcept
    {
        return m_value == other.m_value;
    }

    bool ShipId::operator!=( const ShipId& other ) const noexcept
    {
        return m_value != other.m_value;
    }

    bool ShipId::isImoNumber() const noexcept
    {
        return std::holds_alternative<ImoNumber>( m_value );
    }

    bool ShipId::isOtherId() const noexcept
    {
        return std::holds_alternative<std::string>( m_value );
    }

    std::optional<ImoNumber> ShipId::imoNumber() const noexcept
    {
        if( std::holds_alternative<ImoNumber>( m_value ) )
        {
            return std::get<ImoNumber>( m_value );
        }
        return std::nullopt;
    }

    std::optional<std::string_view> ShipId::otherId() const noexcept
    {
        if( std::holds_alternative<std::string>( m_value ) )
        {
            return std::get<std::string>( m_value );
        }
        return std::nullopt;
    }

    std::string ShipId::toString() const
    {
        return std::visit(
            []( const auto& value ) -> std::string {
                using T = std::decay_t<decltype( value )>;
                if constexpr( std::is_same_v<T, ImoNumber> )
                {
                    return value.toString();
                }
                else
                {
                    return value;
                }
            },
            m_value );
    }

    std::optional<ShipId> ShipId::fromString( std::string_view value ) noexcept
    {
        if( nfx::string::isNullOrWhiteSpace( value ) )
        {
            return std::nullopt;
        }

        // Try to parse as IMO number if it starts with "IMO" (case-insensitive)
        if( value.length() >= 3 && nfx::string::iequals( value.substr( 0, 3 ), "IMO" ) )
        {
            auto imoOpt = ImoNumber::fromString( value );
            if( imoOpt.has_value() )
            {
                return std::make_optional( ShipId{ *imoOpt } );
            }
        }

        // Otherwise, treat as alternative identifier
        return std::make_optional( ShipId{ std::string{ value } } );
    }
} // namespace dnv::vista::sdk
