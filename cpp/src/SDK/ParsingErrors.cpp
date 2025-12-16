/**
 * @file ParsingErrors.cpp
 * @brief Implementation of ParsingErrors container
 */

#include "dnv/vista/sdk/ParsingErrors.h"

#include "dnv/vista/sdk/StringBuilder.h"

namespace dnv::vista::sdk
{
    std::string ParsingErrors::toString() const
    {
        if( m_errors.empty() )
        {
            return "Success";
        }

        StringBuilder builder{ 32 };
        builder << "Parsing errors:\n";

        for( const auto& [type, message] : m_errors )
        {
            builder << '\t' << type << " - " << message << '\n';
        }

        return builder.toString();
    }
} // namespace dnv::vista::sdk
