/**
 * @file GmodParsePathResult.h
 * @brief Internal result type for Gmod path parsing operations
 * @details Result monad (Ok/Error) for propagating detailed parsing errors from internal parsing functions.
 *          Not exposed in public API - only used internally between parsing implementation layers.
 */

#pragma once

#include "dnv/vista/sdk/GmodPath.h"

#include <string>
#include <variant>

namespace dnv::vista::sdk::internal
{
    struct GmodParsePathResult
    {
        struct Ok
        {
            GmodPath path;

            inline explicit Ok( GmodPath p )
                : path{ std::move( p ) }
            {
            }
        };

        struct Error
        {
            std::string error;

            inline explicit Error( std::string&& e ) noexcept
                : error{ std::move( e ) }
            {
            }
        };

        std::variant<Ok, Error> result;

        inline GmodParsePathResult( Ok ok )
            : result{ std::move( ok ) }
        {
        }

        inline GmodParsePathResult( Error err )
            : result{ std::move( err ) }
        {
        }

        inline bool isOk() const noexcept
        {
            return std::holds_alternative<Ok>( result );
        }

        inline bool isError() const noexcept
        {
            return std::holds_alternative<Error>( result );
        }

        inline Ok& ok()
        {
            return std::get<Ok>( result );
        }

        inline const Ok& ok() const
        {
            return std::get<Ok>( result );
        }

        inline Error& error()
        {
            return std::get<Error>( result );
        }

        inline const Error& error() const
        {
            return std::get<Error>( result );
        }
    };
} // namespace dnv::vista::sdk::internal
