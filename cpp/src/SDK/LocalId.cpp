/**
 * @file LocalId.cpp
 * @brief Implementation of VIS LocalId class
 * @details Provides implementation for the immutable LocalId class.
 */

#include "dnv/vista/sdk/LocalId.h"

#include "dnv/vista/sdk/LocalIdBuilder.h"
#include "dnv/vista/sdk/ParsingErrors.h"

#include <stdexcept>

namespace dnv::vista::sdk
{
    LocalId::LocalId( const LocalIdBuilder& builder )
        : m_builder{ builder }
    {
        if( m_builder.isEmpty() )
        {
            throw std::invalid_argument{ "LocalId cannot be constructed from empty LocalIdBuilder" };
        }
        if( !m_builder.isValid() )
        {
            throw std::invalid_argument{ "LocalId cannot be constructed from invalid LocalIdBuilder" };
        }
    }

    std::optional<LocalId> LocalId::fromString( std::string_view localIdStr ) noexcept
    {
        return LocalIdBuilder::fromString( localIdStr );
    }

    std::optional<LocalId> LocalId::fromString( std::string_view localIdStr, ParsingErrors& errors ) noexcept
    {
        return LocalIdBuilder::fromString( localIdStr, errors );
    }
} // namespace dnv::vista::sdk

namespace dnv::vista::sdk::mqtt
{
    LocalId::LocalId( const sdk::LocalIdBuilder& builder )
        : sdk::LocalId{ builder }
    {
    }
} // namespace dnv::vista::sdk::mqtt
