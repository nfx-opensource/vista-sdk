/**
 * @file Gmod.cpp
 * @brief Implementation of Gmod class methods
 * @details Provides constructors and static helper methods for Gmod tree management
 */

#include "dnv/vista/sdk/Gmod.h"

#include "dto/GmodDto.h"

#include <algorithm>
#include <stdexcept>

namespace dnv::vista::sdk
{
    Gmod::Gmod( VisVersion version, const GmodDto& dto )
        : m_visVersion{ version },
          m_rootNode{ nullptr }
    {
        const auto& items = dto.items;
        std::vector<std::pair<std::string, GmodNode>> nodePairs;
        nodePairs.reserve( items.size() );

        for( const auto& nodeDto : items )
        {
            nodePairs.emplace_back( std::string{ nodeDto.code }, GmodNode{ version, nodeDto } );
        }

        m_nodeMap = PerfectHashMap<std::string, GmodNode>( std::move( nodePairs ) );

        for( const auto& relation : dto.relations )
        {
            /*
             * Each relation defines a parent-child relationship and must contain at least 2 elements:
             * relation[0] = parent node code (e.g., "VE", "400", "410")
             * relation[1] = child node code (e.g., "400", "410", "411")
             * We need both parent and child codes to establish the bidirectional relationship.
             */
            const auto* parentNodePtr = m_nodeMap.find( relation[0] );
            if( !parentNodePtr )
            {
                throw std::runtime_error{ "Parent node not found in Gmod" };
            }
            const auto* childNodePtr = m_nodeMap.find( relation[1] );
            if( !childNodePtr )
            {
                throw std::runtime_error{ "Child node not found in Gmod" };
            }
            auto& parentNode = const_cast<GmodNode&>( *parentNodePtr );
            auto& childNode = const_cast<GmodNode&>( *childNodePtr );
            parentNode.addChild( &childNode );
            childNode.addParent( &parentNode );
        }

        for( auto& [key, node] : m_nodeMap )
        {
            const_cast<GmodNode&>( node ).trim();
        }

        const auto* rootPtr = m_nodeMap.find( "VE" );
        if( !rootPtr )
        {
            throw std::runtime_error{ "Root node 'VE' not found in Gmod" };
        }
        m_rootNode = const_cast<GmodNode*>( rootPtr );
    }

    bool Gmod::traverse( TraverseHandler handler, TraversalOptions options ) const
    {
        return traverse( *m_rootNode, handler, options );
    }

    bool Gmod::traverse( const GmodNode& rootNode, TraverseHandler handler, TraversalOptions options ) const
    {
        struct DummyState
        {
        };

        DummyState state;

        TraverseHandlerWithState<DummyState> wrappedHandler =
            [&handler]( DummyState&, const TraversalPath& parents, const GmodNode& node )
            -> TraversalHandlerResult { return handler( parents, node ); };

        return traverse( state, rootNode, wrappedHandler, options );
    }
} // namespace dnv::vista::sdk
