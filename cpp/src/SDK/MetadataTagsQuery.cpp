/**
 * @file MetadataTagsQuery.cpp
 * @brief Implementation of MetadataTagsQuery
 */

#include "dnv/vista/sdk/MetadataTagsQuery.h"

#include "dnv/vista/sdk/LocalId.h"

namespace dnv::vista::sdk
{
    MetadataTagsQuery::MetadataTagsQuery( const MetadataTagsQueryBuilder* builder )
        : m_tags{ builder->m_tags },
          m_matchExact{ builder->m_matchExact }
    {
    }

    bool MetadataTagsQuery::match( const LocalId& localId ) const
    {
        std::unordered_map<CodebookName, MetadataTag> metadataTags;
        for( const auto& tag : localId.metadataTags() )
        {
            metadataTags.emplace( tag.name(), tag );
        }

        if( !m_tags.empty() )
        {
            if( m_matchExact )
            {
                if( m_tags.size() != metadataTags.size() )
                {
                    return false;
                }

                for( const auto& [name, tag] : m_tags )
                {
                    auto it = metadataTags.find( name );
                    if( it == metadataTags.end() || it->second != tag )
                    {
                        return false;
                    }
                }
                return true;
            }
            else
            {
                // Subset match: must have AT LEAST these tags (can have more)
                for( const auto& [name, tag] : m_tags )
                {
                    auto it = metadataTags.find( name );
                    if( it == metadataTags.end() )
                    {
                        return false;
                    }
                    if( it->second != tag )
                    {
                        return false;
                    }
                }
                return true;
            }
        }
        else
        {
            // No tags specified:
            // - If matchExact (allowOthers=false): match only LocalIds with no tags
            // - If !matchExact (allowOthers=true): match any LocalId
            return !m_matchExact;
        }
    }

    MetadataTagsQueryBuilder MetadataTagsQueryBuilder::from( const LocalId& localId, bool allowOtherTags )
    {
        MetadataTagsQueryBuilder builder;
        for( const auto& tag : localId.metadataTags() )
        {
            builder = std::move( builder ).withTag( tag );
        }
        return std::move( builder ).withAllowOtherTags( allowOtherTags );
    }
} // namespace dnv::vista::sdk
