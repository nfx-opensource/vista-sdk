/**
 * @file GmodNode.cpp
 * @brief Implementation of GmodNodeMetadata and GmodNode
 */

#include "dnv/vista/sdk/GmodNode.h"

#include "dto/GmodDto.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
    GmodNodeMetadata::GmodNodeMetadata(
        std::string_view category,
        std::string_view type,
        std::string_view name,
        std::optional<std::string> commonName,
        std::optional<std::string> definition,
        std::optional<std::string> commonDefinition,
        std::optional<bool> installSubstructure,
        HashMap<std::string, std::string> normalAssignmentNames ) noexcept
        : m_category{ category },
          m_type{ type },
          m_name{ name },
          m_commonName{ commonName },
          m_definition{ definition },
          m_commonDefinition{ commonDefinition },
          m_installSubstructure{ installSubstructure },
          m_normalAssignmentNames{ normalAssignmentNames },
          m_fullType{ std::string{ category } + ' ' + std::string{ type } }
    {
    }

    GmodNode::GmodNode( VisVersion version, const GmodNodeDto& dto ) noexcept
        : m_visVersion{ version },
          m_code{ dto.code },
          m_location{ std::nullopt },
          m_metadata{ new GmodNodeMetadata(
              dto.category,
              dto.type,
              dto.name.value_or( "" ),
              dto.commonName,
              dto.definition,
              dto.commonDefinition,
              dto.installSubstructure,
              dto.normalAssignmentNames ? HashMap<std::string, std::string>{ dto.normalAssignmentNames->begin(),
                                                                             dto.normalAssignmentNames->end() }
                                        : HashMap<std::string, std::string>{} ) },
          m_children{},
          m_parents{},
          m_childrenSet{ nullptr }
    {
    }

    std::optional<GmodNode> GmodNode::withLocation( std::string_view locationStr ) const
    {
        const Locations& locations = VIS::instance().locations( m_visVersion );
        auto locationOpt = locations.fromString( locationStr );

        if( !locationOpt.has_value() )
        {
            return std::nullopt;
        }

        return withLocation( locationOpt.value() );
    }

    std::optional<GmodNode> GmodNode::withLocation( std::string_view locationStr, ParsingErrors& errors ) const
    {
        const Locations& locations = VIS::instance().locations( m_visVersion );

        auto locationOpt = locations.fromString( locationStr, errors );
        if( !locationOpt.has_value() )
        {
            return std::nullopt;
        }

        return withLocation( locationOpt.value() );
    }

    void GmodNode::addChild( GmodNode* child ) noexcept
    {
        if( !child )
        {
            return;
        }

        m_children.push_back( child );
        if( m_childrenSet )
        {
            m_childrenSet->emplace( child->code() );
        }
    }

    void GmodNode::addParent( GmodNode* parent ) noexcept
    {
        if( !parent )
        {
            return;
        }

        m_parents.push_back( parent );
    }

    void GmodNode::trim() noexcept
    {
        if( !m_childrenSet )
        {
            m_childrenSet = std::make_shared<HashSet<std::string>>();
            m_childrenSet->clear();
            m_childrenSet->reserve( m_children.size() );

            for( const auto* child : m_children )
            {
                if( child )
                {
                    m_childrenSet->emplace( child->code() );
                }
            }
        }
    }
} // namespace dnv::vista::sdk
