/**
 * @file GmodPathQuery.cpp
 * @brief Implementation of GmodPathQuery and GmodPathQueryBuilder
 * @details Provides query builders for filtering GmodPaths based on nodes and locations
 */

#include "dnv/vista/sdk/GmodPathQuery.h"

#include "dnv/vista/sdk/VIS.h"

#include <iostream>
#include <stdexcept>

namespace dnv::vista::sdk
{
    GmodPathQueryBuilder::NodeItem::NodeItem( const GmodNode& n, const std::vector<Location>& locs )
        : node{ n },
          locations{ locs }
    {
    }

    bool GmodPathQueryBuilder::matchFilterAgainstTarget(
        const std::unordered_map<std::string, NodeItem>& filter, const GmodPath& target, bool checkIgnoreFlag )
    {
        const auto& vis = VIS::instance();
        auto latestVisVersion = vis.latest();

        // Build target nodes with locations
        std::unordered_map<std::string, std::vector<Location>> targetNodes;
        for( const auto& [depth, node] : target.fullPath() )
        {
            const std::string codeStr( node.code() );
            auto& locs = targetNodes[codeStr];
            if( node.location().has_value() )
            {
                locs.push_back( node.location().value() );
            }
        }

        // Match filters
        for( const auto& [code, item] : filter )
        {
            // Skip nodes marked as ignorable (Path only)
            if( checkIgnoreFlag && item.ignoreInMatching )
            {
                continue;
            }

            // Ensure node version matches
            const GmodNode* node = &item.node;
            std::optional<GmodNode> convertedNode;
            if( node->version() < latestVisVersion )
            {
                convertedNode = VIS::instance().convertNode( *node, latestVisVersion );
                if( !convertedNode.has_value() )
                {
                    return false;
                }
                node = &convertedNode.value();
            }

            // Check node exists in target
            const std::string nodeCodeStr( node->code() );
            auto it = targetNodes.find( nodeCodeStr );
            if( it == targetNodes.end() )
            {
                return false;
            }

            const auto& potentialLocations = it->second;

            // Check location matching
            if( item.matchAllLocations )
            {
                // Don't care about locations
                continue;
            }

            if( !item.locations.empty() )
            {
                // Must have specific locations
                if( potentialLocations.empty() )
                {
                    return false;
                }

                bool hasMatch = false;
                for( const auto& loc : potentialLocations )
                {
                    // Check if loc exists in item.locations vector
                    if( std::find( item.locations.begin(), item.locations.end(), loc ) != item.locations.end() )
                    {
                        hasMatch = true;
                        break;
                    }
                }
                if( !hasMatch )
                {
                    return false;
                }
            }
            else
            {
                // Must have NO locations
                if( !potentialLocations.empty() )
                {
                    return false;
                }
            }
        }

        return true;
    }

    GmodPathQuery::GmodPathQuery( const GmodPathQueryBuilder* builder )
        : m_builder{ builder->clone() }
    {
    }

    GmodPathQuery::GmodPathQuery( const GmodPathQuery& other )
        : m_builder{ other.m_builder ? other.m_builder->clone() : nullptr }
    {
    }

    GmodPathQuery& GmodPathQuery::operator=( const GmodPathQuery& other )
    {
        if( this != &other )
        {
            m_builder = other.m_builder ? other.m_builder->clone() : nullptr;
        }
        return *this;
    }

    bool GmodPathQuery::match( const GmodPath& other ) const
    {
        return m_builder->match( &other );
    }

    const GmodPathQueryBuilder* GmodPathQuery::builder() const noexcept
    {
        return m_builder.get();
    }

    GmodPathQueryBuilder::Nodes GmodPathQueryBuilder::empty()
    {
        return Nodes{};
    }

    GmodPathQueryBuilder::Path GmodPathQueryBuilder::from( const GmodPath& path )
    {
        return Path{ path };
    }

    GmodPathQuery GmodPathQueryBuilder::build() const
    {
        return GmodPathQuery{ this };
    }

    GmodPathQueryBuilder::Path::Path( const GmodPath& path )
        : m_gmodPath{ path }
    {
        // Build set nodes (last node of each individualizable set)
        const auto sets = path.individualizableSets();
        for( const auto& set : sets )
        {
            const auto nodeIndices = set.nodeIndices();
            if( nodeIndices.empty() )
            {
                continue;
            }

            // Get stable pointer from path using last node index
            const int lastIndex = nodeIndices.back();
            const GmodNode* setNode = &path[static_cast<size_t>( lastIndex )];
            const std::string codeStr( setNode->code() );
            m_setNodes.emplace( codeStr, setNode );

            // Add to filter with location if present
            std::vector<Location> locations;
            if( set.location().has_value() )
            {
                locations.push_back( set.location().value() );
            }
            m_filter.emplace( codeStr, NodeItem( *setNode, locations ) );
        }

        // Build full path nodes
        for( const auto& [depth, node] : path.fullPath() )
        {
            const std::string codeStr( node.code() );
            m_nodes.emplace( codeStr, &node );
        }
    }

    const GmodPath& GmodPathQueryBuilder::Path::path() const noexcept
    {
        return m_gmodPath;
    }

    GmodPathQueryBuilder::Path GmodPathQueryBuilder::Path::withNode(
        std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select,
        bool matchAllLocations ) const&
    {
        Path copy{ *this };
        return std::move( copy ).withNode( select, matchAllLocations );
    }

    GmodPathQueryBuilder::Path GmodPathQueryBuilder::Path::withNode(
        std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select,
        bool matchAllLocations ) &&
    {
        const GmodNode* node = select( m_setNodes );
        if( !node )
        {
            throw std::invalid_argument{ "Selected node is null" };
        }

        const std::string codeStr( node->code() );
        auto it = m_filter.find( codeStr );
        if( it == m_filter.end() )
        {
            throw std::invalid_argument{ "Expected to find a filter on the node in the path" };
        }

        it->second.locations.clear();
        it->second.matchAllLocations = matchAllLocations;
        return std::move( *this );
    }

    GmodPathQueryBuilder::Path GmodPathQueryBuilder::Path::withNode(
        std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select,
        const std::vector<Location>& locations ) const&
    {
        Path copy{ *this };
        return std::move( copy ).withNode( select, locations );
    }

    GmodPathQueryBuilder::Path GmodPathQueryBuilder::Path::withNode(
        std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select,
        const std::vector<Location>& locations ) &&
    {
        const GmodNode* node = select( m_setNodes );
        if( !node )
        {
            throw std::invalid_argument{ "Selected node is null" };
        }

        const std::string codeStr( node->code() );
        auto it = m_filter.find( codeStr );
        if( it == m_filter.end() )
        {
            throw std::invalid_argument{ "Expected to find a filter on the node in the path" };
        }

        it->second.locations.clear();
        for( const auto& loc : locations )
        {
            it->second.locations.push_back( loc );
        }
        it->second.matchAllLocations = false;

        return std::move( *this );
    }

    GmodPathQueryBuilder::Path GmodPathQueryBuilder::Path::withAnyNodeBefore(
        std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select ) const&
    {
        Path copy{ *this };
        return std::move( copy ).withAnyNodeBefore( select );
    }

    GmodPathQueryBuilder::Path GmodPathQueryBuilder::Path::withAnyNodeBefore(
        std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select ) &&
    {
        const GmodNode* node = select( m_nodes );
        if( !node )
        {
            throw std::invalid_argument{ "Selected node is null" };
        }

        // Check node is in path
        const std::string targetCode( node->code() );
        bool found = false;
        for( const auto& [depth, pathNode] : m_gmodPath.fullPath() )
        {
            if( pathNode.code() == targetCode )
            {
                found = true;
                break;
            }
        }

        if( !found )
        {
            throw std::invalid_argument{ "Node is not in the path" };
        }

        // Mark all nodes before this one as ignorable
        for( const auto& [depth, pathNode] : m_gmodPath.fullPath() )
        {
            if( pathNode.code() == targetCode )
            {
                break;
            }

            const std::string codeStr( pathNode.code() );
            auto it = m_filter.find( codeStr );
            if( it != m_filter.end() )
            {
                it->second.ignoreInMatching = true;
            }
        }

        return std::move( *this );
    }

    GmodPathQueryBuilder::Path GmodPathQueryBuilder::Path::withoutLocations() const&
    {
        Path copy{ *this };
        return std::move( copy ).withoutLocations();
    }

    GmodPathQueryBuilder::Path GmodPathQueryBuilder::Path::withoutLocations() &&
    {
        for( auto& [code, item] : m_filter )
        {
            item.locations.clear();
            item.matchAllLocations = true;
        }
        return std::move( *this );
    }

    bool GmodPathQueryBuilder::Path::match( const GmodPath* other ) const
    {
        if( !other )
        {
            return false;
        }

        const GmodPath* target = other;
        std::optional<GmodPath> converted;
        const auto& vis = VIS::instance();
        auto latestVisVersion = vis.latest();
        if( other->version() < latestVisVersion )
        {
            converted = vis.convertPath( *other, latestVisVersion );
            if( !converted.has_value() )
            {
                return false;
            }
            target = &converted.value();
        }

        return matchFilterAgainstTarget( m_filter, *target, true );
    }

    std::unique_ptr<GmodPathQueryBuilder> GmodPathQueryBuilder::Path::clone() const
    {
        return std::make_unique<Path>( *this );
    }

    GmodPathQueryBuilder::Nodes GmodPathQueryBuilder::Nodes::withNode(
        const GmodNode& node, bool matchAllLocations ) const&
    {
        Nodes copy{ *this };
        return std::move( copy ).withNode( node, matchAllLocations );
    }

    GmodPathQueryBuilder::Nodes GmodPathQueryBuilder::Nodes::withNode( const GmodNode& node, bool matchAllLocations ) &&
    {
        const std::string codeStr( node.code() );
        auto it = m_filter.find( codeStr );
        if( it != m_filter.end() )
        {
            it->second.locations.clear();
            it->second.matchAllLocations = matchAllLocations;
        }
        else
        {
            NodeItem item( node, {} );
            item.matchAllLocations = matchAllLocations;
            m_filter.emplace( codeStr, item );
        }
        return std::move( *this );
    }

    GmodPathQueryBuilder::Nodes GmodPathQueryBuilder::Nodes::withNode(
        const GmodNode& node, const std::vector<Location>& locations ) const&
    {
        Nodes copy{ *this };
        return std::move( copy ).withNode( node, locations );
    }

    GmodPathQueryBuilder::Nodes GmodPathQueryBuilder::Nodes::withNode(
        const GmodNode& node, const std::vector<Location>& locations ) &&
    {
        const std::string codeStr( node.code() );
        auto it = m_filter.find( codeStr );
        if( it != m_filter.end() )
        {
            it->second.locations = locations;
            it->second.matchAllLocations = false;
        }
        else
        {
            m_filter.emplace( codeStr, NodeItem( node, locations ) );
        }
        return std::move( *this );
    }

    bool GmodPathQueryBuilder::Nodes::match( const GmodPath* other ) const
    {
        if( !other )
        {
            return false;
        }

        GmodPath target = *other;
        const auto& vis = VIS::instance();
        auto latestVisVersion = vis.latest();
        if( target.version() < latestVisVersion )
        {
            auto converted = VIS::instance().convertPath( target, latestVisVersion );
            if( !converted.has_value() )
            {
                return false;
            }
            target = converted.value();
        }

        return matchFilterAgainstTarget( m_filter, target, false );
    }

    std::unique_ptr<GmodPathQueryBuilder> GmodPathQueryBuilder::Nodes::clone() const
    {
        return std::make_unique<Nodes>( *this );
    }
} // namespace dnv::vista::sdk
