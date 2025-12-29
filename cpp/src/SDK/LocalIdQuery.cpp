/**
 * @file LocalIdQuery.cpp
 * @brief Implementation of LocalIdQuery and LocalIdQueryBuilder
 * @details Provides complex query matching and accessor methods
 */

#include "dnv/vista/sdk/LocalIdQuery.h"

#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
    LocalIdQuery::LocalIdQuery( const LocalIdQueryBuilder& builder )
        : m_primaryItem{ builder.m_primaryItem },
          m_secondaryItem{ builder.m_secondaryItem },
          m_tags{ builder.m_tags },
          m_requireSecondaryItem{ builder.m_requireSecondaryItem }
    {
    }

    bool LocalIdQuery::match( const LocalId& localId ) const
    {
        const LocalId* targetLocalId = &localId;
        std::optional<LocalId> converted;
        const auto latest = VIS::instance().latest();

        if( localId.version() < latest )
        {
            converted = VIS::instance().convertLocalId( localId, latest );
            if( !converted.has_value() )
            {
                return false;
            }
            targetLocalId = &converted.value();
        }

        // Check primary item
        if( m_primaryItem.has_value() )
        {
            if( !m_primaryItem->match( targetLocalId->primaryItem() ) )
            {
                return false;
            }
        }

        // Check secondary item
        if( m_secondaryItem.has_value() )
        {
            const auto& secondaryOpt = targetLocalId->secondaryItem();
            if( !secondaryOpt.has_value() )
            {
                return false;
            }
            if( !m_secondaryItem->match( secondaryOpt.value() ) )
            {
                return false;
            }
        }

        // Check secondary item requirement (presence/absence)
        if( m_requireSecondaryItem.has_value() )
        {
            const bool hasSecondary = targetLocalId->secondaryItem().has_value();
            if( m_requireSecondaryItem.value() != hasSecondary )
            {
                return false;
            }
        }

        // Check metadata tags
        if( m_tags.has_value() )
        {
            if( !m_tags->match( *targetLocalId ) )
            {
                return false;
            }
        }

        return true;
    }

    const GmodPath* LocalIdQueryBuilder::primaryItem() const noexcept
    {
        if( !m_primaryItem.has_value() )
        {
            return nullptr;
        }

        const auto* builder = m_primaryItem->builder();
        if( const auto* pathBuilder = dynamic_cast<const GmodPathQueryBuilder::Path*>( builder ) )
        {
            return &pathBuilder->path();
        }

        return nullptr;
    }

    const GmodPath* LocalIdQueryBuilder::secondaryItem() const noexcept
    {
        if( !m_secondaryItem.has_value() )
        {
            return nullptr;
        }

        const auto* builder = m_secondaryItem->builder();
        if( const auto* pathBuilder = dynamic_cast<const GmodPathQueryBuilder::Path*>( builder ) )
        {
            return &pathBuilder->path();
        }

        return nullptr;
    }

    LocalIdQueryBuilder LocalIdQueryBuilder::withoutLocations() const&
    {
        LocalIdQueryBuilder builder{ *this };
        return std::move( builder ).withoutLocations();
    }

    LocalIdQueryBuilder LocalIdQueryBuilder::withoutLocations() &&
    {
        // Remove locations from primary item if it's a Path query
        if( m_primaryItem.has_value() )
        {
            const auto* builder = m_primaryItem->builder();
            if( const auto* pathBuilder = dynamic_cast<const GmodPathQueryBuilder::Path*>( builder ) )
            {
                auto newPath = GmodPathQueryBuilder::Path::from( pathBuilder->path() );
                m_primaryItem = newPath.withoutLocations().build();
            }
        }

        // Remove locations from secondary item if it's a Path query
        if( m_secondaryItem.has_value() )
        {
            const auto* builder = m_secondaryItem->builder();
            if( const auto* pathBuilder = dynamic_cast<const GmodPathQueryBuilder::Path*>( builder ) )
            {
                auto newPath = GmodPathQueryBuilder::Path::from( pathBuilder->path() );
                m_secondaryItem = newPath.withoutLocations().build();
            }
        }

        return std::move( *this );
    }
} // namespace dnv::vista::sdk
