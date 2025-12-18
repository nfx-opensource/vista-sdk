/**
 * @file LocationBuilder.cpp
 * @brief Implementation of VIS location parsing and validation
 * @details Provides implementations for Location, RelativeLocation, and Locations classes
 *          for parsing and validating VIS location strings.
 */

#include "dnv/vista/sdk/LocationBuilder.h"

#include <nfx/StringUtils.h>

namespace dnv::vista::sdk
{
    LocationBuilder LocationBuilder::withLocation( const Location& location ) const&
    {
        LocationBuilder builder{ *this };

        std::string locationStr = location.value();
        std::string_view span = locationStr;
        std::optional<int> number;

        for( size_t i = 0; i < span.length(); ++i )
        {
            char ch = span[i];

            if( std::isdigit( ch ) )
            {
                if( !number.has_value() )
                {
                    // Convert ASCII digit character to numeric value: '0'=48, '1'=49, ..., '9'=57
                    // Subtracting '0' (ASCII 48) gives: '0'-'0'=0, '1'-'0'=1, ..., '9'-'0'=9
                    number = ch - '0';
                }
                else
                {
                    // Extract substring from start to current position (i+1)
                    std::string_view numberSubstr = span.substr( 0, i + 1 );
                    int parsedNumber;
                    if( !nfx::string::fromString<int>( numberSubstr, parsedNumber ) )
                    {
                        throw std::invalid_argument{ "Should include a valid number" };
                    }
                    number = parsedNumber;
                }
                continue;
            }

            builder = builder.withCode( ch );
        }

        if( number.has_value() )
        {
            builder = builder.withNumber( number.value() );
        }

        return builder;
    }

    LocationBuilder LocationBuilder::withLocation( const Location& location ) &&
    {
        return static_cast<const LocationBuilder&>( *this ).withLocation( location );
    }
} // namespace dnv::vista::sdk
