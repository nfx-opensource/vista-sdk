/**
 * @file UniversalId.cpp
 * @brief Implementation of VIS UniversalId
 * @details Provides implementation for UniversalId class representing globally unique sensor identifiers.
 */

#include "dnv/vista/sdk/UniversalId.h"

#include "dnv/vista/sdk/ParsingErrors.h"

#include <stdexcept>

namespace dnv::vista::sdk
{
    UniversalId::UniversalId( const UniversalIdBuilder& builder )
        : m_builder{ builder },
          m_localId{ builder.localId()->build() }
    {
        if( !m_builder.isValid() )
        {
            throw std::invalid_argument{
                "Cannot create UniversalId: builder is invalid (missing IMO number or LocalId)"
            };
        }
    }

    std::optional<UniversalId> UniversalId::fromString( std::string_view universalIdStr ) noexcept
    {
        return UniversalIdBuilder::fromString( universalIdStr );
    }

    std::optional<UniversalId> UniversalId::fromString(
        std::string_view universalIdStr, ParsingErrors& errors ) noexcept
    {
        return UniversalIdBuilder::fromString( universalIdStr, errors );
    }
} // namespace dnv::vista::sdk
