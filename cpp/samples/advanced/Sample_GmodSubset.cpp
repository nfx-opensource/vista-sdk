/**
 * @file Sample_GmodSubset.cpp
 * @brief Demonstrates building Asset Models (Digital Twin structures) from GmodPaths
 * @details This sample shows how to construct hierarchical asset models from GmodPaths,
 *          perform code-based lookups, extract paths from LocalIds, and export to JSON.
 *          Asset models form the foundation for defining vessel equipment, creating
 *          visualizations, and generating LocalIds for sensor data.
 */

#include <dnv/vista/sdk/VIS.h>

#include <nfx/json/Builder.h>
#include <nfx/json/Document.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

//========================================================================
// Asset Model Classes
//========================================================================

namespace
{
    using namespace dnv::vista::sdk;

    /// A node in the asset model tree.
    /// Wraps a GmodNode with tree structure for visualization.
    class AssetNode
    {
    private:
        GmodNode m_node;
        std::optional<GmodPath> m_path;
        int m_depth;
        std::string m_displayName;
        std::vector<std::shared_ptr<AssetNode>> m_children;
        AssetNode* m_parent = nullptr;

    public:
        AssetNode( const GmodNode& n, std::optional<GmodPath> p, int d )
            : m_node{ n },
              m_path{ std::move( p ) },
              m_depth{ d }
        {
            if( m_path.has_value() )
            {
                auto names = m_path->commonNames();
                if( !names.empty() )
                {
                    m_displayName = names.back().second;
                }
            }
            if( m_displayName.empty() )
            {
                m_displayName = m_node.metadata().name();
            }
        }

        const GmodNode& node() const
        {
            return m_node;
        }
        const std::optional<GmodPath>& path() const
        {
            return m_path;
        }
        int depth() const
        {
            return m_depth;
        }
        const std::string& displayName() const
        {
            return m_displayName;
        }
        const std::vector<std::shared_ptr<AssetNode>>& children() const
        {
            return m_children;
        }
        AssetNode* parent() const
        {
            return m_parent;
        }

        void addChild( std::shared_ptr<AssetNode> child )
        {
            child->m_parent = this;
            m_children.push_back( std::move( child ) );
        }

        std::string toJson() const
        {
            nfx::json::Builder builder;
            builder.writeStartObject();

            builder.write( "path", m_path.has_value() ? m_path->toString() : "" );
            builder.write( "code", std::string( m_node.code() ) );
            builder.write( "name", std::string( m_node.metadata().name() ) );
            if( m_node.metadata().commonName().has_value() )
            {
                builder.write( "commonName", *m_node.metadata().commonName() );
            }
            builder.write( "displayName", m_displayName );
            builder.write( "category", std::string( m_node.metadata().category() ) );
            builder.write( "type", std::string( m_node.metadata().type() ) );

            if( m_node.location().has_value() )
            {
                builder.write( "location", m_node.location()->value() );
            }

            builder.writeKey( "children" );
            builder.writeStartArray();
            for( const auto& child : m_children )
            {
                if( child->m_path.has_value() )
                {
                    builder.writeRawJson( child->toJson() );
                }
            }
            builder.writeEndArray();

            builder.writeEndObject();
            return builder.toString();
        }

        void printTree( const std::string& prefix = "", bool isLast = true, bool isRoot = true ) const
        {
            // Print current node with tree connector
            if( !isRoot )
            {
                std::cout << prefix << ( isLast ? "└─ " : "├─ " );
            }
            std::cout << m_node.code();

            if( m_node.location().has_value() )
            {
                std::cout << " [" << m_node.location()->value() << "]";
            }

            std::cout << ": " << m_displayName << "\n";

            // Sort and print children
            std::vector<std::shared_ptr<AssetNode>> sortedChildren;
            for( const auto& child : m_children )
            {
                if( child && child->m_path.has_value() )
                {
                    sortedChildren.push_back( child );
                }
            }

            std::sort( sortedChildren.begin(), sortedChildren.end(), []( const auto& a, const auto& b ) {
                return a->m_path->toString() < b->m_path->toString();
            } );

            for( size_t i = 0; i < sortedChildren.size(); ++i )
            {
                bool childIsLast = ( i == sortedChildren.size() - 1 );
                std::string newPrefix = prefix;
                if( !isRoot )
                {
                    newPrefix += ( isLast ? "   " : "│  " );
                }
                sortedChildren[i]->printTree( newPrefix, childIsLast, false );
            }
        }
    };

    /// An asset model built from GmodPaths.
    /// This class builds a tree structure from a list of GmodPaths.
    class AssetModel
    {
    private:
        VisVersion m_visVersion;
        std::shared_ptr<AssetNode> m_root;
        std::map<std::string, std::shared_ptr<AssetNode>> m_nodeMap;
        std::map<std::string, std::vector<std::shared_ptr<AssetNode>>> m_nodesByCode;

        AssetModel( VisVersion version )
            : m_visVersion{ version }
        {
        }

        void addPath( const GmodPath& path )
        {
            // Convert fullPath iterator to vector
            std::vector<std::pair<size_t, GmodNode>> fullPathVec;
            for( const auto& [depth, node] : path.fullPath() )
            {
                fullPathVec.push_back( { depth, node } );
            }

            for( size_t i = 0; i < fullPathVec.size(); ++i )
            {
                const auto& [depth, node] = fullPathVec[i];

                // Build the full path ID up to this point
                std::string nodeId;
                for( size_t j = 0; j <= i; ++j )
                {
                    if( j > 0 )
                        nodeId += "/";
                    nodeId += std::string( fullPathVec[j].second.code() );
                }

                // Skip if already added
                if( m_nodeMap.find( nodeId ) != m_nodeMap.end() )
                {
                    continue;
                }

                // Build the GmodPath for this node
                std::optional<GmodPath> nodePath;
                if( depth > 0 )
                {
                    std::vector<GmodNode> parents;
                    for( size_t j = 0; j < i; ++j )
                    {
                        parents.push_back( fullPathVec[j].second );
                    }
                    nodePath = GmodPath( std::move( parents ), node );
                }

                // Create the asset node
                auto assetNode = std::make_shared<AssetNode>( node, nodePath, depth );

                // Add to map
                m_nodeMap[nodeId] = assetNode;

                // Add to nodes_by_code index
                std::string code( node.code() );
                m_nodesByCode[code].push_back( assetNode );

                // Handle root node
                if( depth == 0 )
                {
                    if( !m_root )
                    {
                        m_root = assetNode;
                    }
                    continue;
                }

                // Find parent and link
                std::string parentId;
                for( size_t j = 0; j < i; ++j )
                {
                    if( j > 0 )
                        parentId += "/";
                    parentId += std::string( fullPathVec[j].second.code() );
                }

                auto parentIt = m_nodeMap.find( parentId );
                if( parentIt != m_nodeMap.end() )
                {
                    parentIt->second->addChild( assetNode );
                }
            }
        }

    public:
        static AssetModel fromPaths( VisVersion version, const std::vector<GmodPath>& paths )
        {
            AssetModel model( version );
            for( const auto& path : paths )
            {
                model.addPath( path );
            }
            return model;
        }

        static AssetModel fromPathStrings( VisVersion version, const std::vector<std::string_view>& pathStrings )
        {
            const auto& vis = VIS::instance();
            const auto& gmod = vis.gmod( version );
            const auto& locations = vis.locations( version );

            std::vector<GmodPath> paths;
            for( const auto& pathStr : pathStrings )
            {
                auto path = GmodPath::fromShortPath( pathStr, gmod, locations );
                if( path.has_value() )
                {
                    paths.push_back( std::move( *path ) );
                }
                else
                {
                    std::cout << "  Warning: Could not parse path '" << pathStr << "'\n";
                }
            }
            return fromPaths( version, paths );
        }

        std::shared_ptr<AssetNode> node( const std::string& nodeId ) const
        {
            auto it = m_nodeMap.find( nodeId );
            return it != m_nodeMap.end() ? it->second : nullptr;
        }

        const std::map<std::string, std::vector<std::shared_ptr<AssetNode>>>& allCodes() const
        {
            return m_nodesByCode;
        }

        std::vector<std::shared_ptr<AssetNode>> nodesByCode( const std::string& code ) const
        {
            auto it = m_nodesByCode.find( code );
            return it != m_nodesByCode.end() ? it->second : std::vector<std::shared_ptr<AssetNode>>{};
        }

        size_t nodeCount() const
        {
            return m_nodeMap.size();
        }

        int maxDepth() const
        {
            int max = 0;
            for( const auto& [_, node] : m_nodeMap )
            {
                max = std::max( max, node->depth() );
            }
            return max;
        }

        std::string toJson() const
        {
            return m_root ? m_root->toJson() : "{}";
        }

        void printTree() const
        {
            if( m_root )
            {
                m_root->printTree();
            }
        }
    };

} // anonymous namespace

int main()
{
    using namespace dnv::vista::sdk;

    std::cout << "=== vista-sdk-cpp GmodSubset Sample ===\n\n";

    //=====================================================================
    // 1. AssetModel: Building from equipment paths (dual-engine vessel)
    //=====================================================================
    {
        std::cout << "1. AssetModel: Building from equipment paths (dual-engine vessel)\n";
        std::cout << "--------------------------------------------------------------------\n";

        auto visVersion = VisVersion::v3_4a;

        // Define the equipment that exists on this vessel
        // Each path is a "leaf" item e.g. ISO19848 Annex C short path - the full hierarchy is implicit
        std::vector<std::string_view> assetPaths = {
            // === Port Main Engine (411.1-P) with 6 cylinders ===
            "411.1-P/C101/C101.31-1",    // Port engine, cylinder 1
            "411.1-P/C101/C101.31-2",    // Port engine, cylinder 2
            "411.1-P/C101/C101.31-3",    // Port engine, cylinder 3
            "411.1-P/C101/C101.31-4",    // Port engine, cylinder 4
            "411.1-P/C101/C101.31-5",    // Port engine, cylinder 5
            "411.1-P/C101/C101.31-6",    // Port engine, cylinder 6
            "411.1-P/C101/C101.63/S206", // Port engine, cooling system

            // === Starboard Main Engine (411.1-S) with 6 cylinders ===
            "411.1-S/C101/C101.31-1",    // Starboard engine, cylinder 1
            "411.1-S/C101/C101.31-2",    // Starboard engine, cylinder 2
            "411.1-S/C101/C101.31-3",    // Starboard engine, cylinder 3
            "411.1-S/C101/C101.31-4",    // Starboard engine, cylinder 4
            "411.1-S/C101/C101.31-5",    // Starboard engine, cylinder 5
            "411.1-S/C101/C101.31-6",    // Starboard engine, cylinder 6
            "411.1-S/C101/C101.63/S206", // Starboard engine, cooling system

            // === Generator Sets ===
            "511.11-1/C101", // Generator 1, diesel engine
            "511.11-2/C101", // Generator 2, diesel engine

            // === Fuel System ===
            "621.21/S90", // Fuel oil transfer system, piping
        };

        std::cout << "Defining " << assetPaths.size() << " equipment paths\n";
        std::cout << "(Each path implicitly includes all parent nodes)\n\n";

        // Build the asset model
        auto model = AssetModel::fromPathStrings( visVersion, assetPaths );

        std::cout << "Built model with " << model.nodeCount() << " total nodes\n";
        std::cout << "Maximum depth   : " << model.maxDepth() << "\n\n";

        // Demonstrate code-based lookup - "show me all engines"
        auto engines = model.nodesByCode( "C101" );
        std::cout << "Lookup: All Engines (C101)\n";
        std::cout << "Found " << engines.size() << " instances of C101 (Internal combustion engine):\n";
        for( const auto& engine : engines )
        {
            if( engine->path().has_value() )
            {
                std::cout << "  - " << engine->path()->toString() << "\n";
            }
        }

        // Demonstrate looking up all cylinders
        std::cout << "\nLookup: All Cylinders (C101.31)\n";
        auto cylinders = model.nodesByCode( "C101.31" );
        std::cout << "Found " << cylinders.size() << " instances of C101.31 (Cylinder):\n";
        size_t count = 0;
        for( const auto& cyl : cylinders )
        {
            if( count++ >= 6 )
                break;
            if( cyl->path().has_value() )
            {
                std::cout << "  - " << cyl->path()->toString() << "\n";
            }
        }
        if( cylinders.size() > 6 )
        {
            std::cout << "  ... and " << ( cylinders.size() - 6 ) << " more\n";
        }

        // Show available codes in the model
        std::cout << "\nAll available codes in model:\n";
        std::vector<std::string> codes;
        for( const auto& [code, _] : model.allCodes() )
        {
            codes.push_back( code );
        }
        std::sort( codes.begin(), codes.end() );
        std::cout << codes.size() << " unique codes: [";
        for( size_t i = 0; i < codes.size(); ++i )
        {
            if( i > 0 )
                std::cout << ", ";
            std::cout << codes[i];
        }
        std::cout << "]\n\n";

        std::cout << "Asset Model Tree:\n\n";
        model.printTree();

        std::cout << "\n";
    }

    //=====================================================================
    // 2. AssetModel: Building from LocalIds (runtime flow)
    //=====================================================================
    {
        std::cout << "2. AssetModel: Building from LocalIds (runtime flow)\n";
        std::cout << "-------------------------------------------------------\n";

        auto visVersion = VisVersion::v3_4a;

        // Simulate LocalIds coming from a DataChannelList or data source
        std::vector<std::string_view> localIdStrings = {
            "/dnv-v2/vis-3-4a/411.1-1/C101.31-1/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
            "/dnv-v2/vis-3-4a/411.1-1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
            "/dnv-v2/vis-3-4a/411.1-1/C101.31-3/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
            "/dnv-v2/vis-3-4a/411.1-2/C101.31-1/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
            "/dnv-v2/vis-3-4a/411.1-2/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
            "/dnv-v2/vis-3-4a/511.11-1/C101/meta/qty-revolution",
        };

        std::cout << "Received " << localIdStrings.size() << " LocalIds from data source\n\n";

        // Parse LocalIds and extract GmodPaths
        std::vector<GmodPath> paths;
        for( const auto& lidStr : localIdStrings )
        {
            auto localIdOpt = LocalId::fromString( lidStr );
            if( localIdOpt.has_value() )
            {
                const auto& localId = *localIdOpt;
                paths.push_back( localId.primaryItem() );
                if( localId.secondaryItem().has_value() )
                {
                    paths.push_back( *localId.secondaryItem() );
                }
            }
        }

        std::cout << "Extracted " << paths.size() << " GmodPaths from LocalIds\n";

        // Build model from extracted paths
        auto model = AssetModel::fromPaths( visVersion, paths );

        std::cout << "Built model with " << model.nodeCount() << " nodes\n";
        std::cout << "Maximum depth   : " << model.maxDepth() << "\n\n";

        std::cout << "Asset Model Tree (derived from LocalIds):\n\n";
        model.printTree();

        std::cout << "\n";
    }

    //=====================================================================
    // 3. AssetModel: JSON export for visualization
    //=====================================================================
    {
        std::cout << "3. AssetModel: JSON export for visualization\n";
        std::cout << "-----------------------------------------------\n";

        auto visVersion = VisVersion::v3_4a;

        // Small example for readable JSON
        std::vector<std::string_view> assetPaths = { "411.1/C101/C101.31-1", "411.1/C101/C101.31-2" };

        auto model = AssetModel::fromPathStrings( visVersion, assetPaths );

        std::cout << "JSON output (for D3.js, visualization tools, etc.):\n\n";
        std::cout << model.toJson() << "\n";

        std::cout << "\n";
    }

    return 0;
}
