/**
 * @file GmodPathParsing.h
 * @brief Internal parsing functions for Gmod path string conversion
 * @details Free functions for parsing short and full Gmod path strings with detailed error reporting.
 *          Returns GmodParsePathResult for error propagation. Not exposed in public API.
 */

#pragma once

#include "dnv/vista/sdk/detail/Containers.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/Locations.h"
#include "GmodParsePathResult.h"
#include "LocationSetsVisitor.h"

#include <nfx/StringUtils.h>

namespace dnv::vista::sdk::internal
{
    // Capacity constants for pre-allocation
    static constexpr size_t TYPICAL_PATH_DEPTH = 8;
    static constexpr size_t MAX_LOCATION_SETS = 16;

    // Static error strings to avoid allocations
    static constexpr const char* ERROR_EMPTY_ITEM = "Item is empty";
    static constexpr const char* ERROR_DIFFERENT_VERSIONS =
        "Got different VIS versions for Gmod and Locations arguments";
    static constexpr const char* ERROR_NODE_LOOKUP_FAILED = "Node lookup failed";
    static constexpr const char* ERROR_NO_NODES_FOUND = "No nodes found";
    static constexpr const char* ERROR_INVALID_SEQUENCE = "Sequence of nodes are invalid";
    static constexpr const char* ERROR_FAILED_TRAVERSAL = "Failed to find path after traversal";
    static constexpr const char* ERROR_NO_PARTS = "Failed find any parts";
    static constexpr const char* ERROR_NO_BASE_NODE = "Failed to find base node";
    static constexpr const char* ERROR_PREFIX_FAILED_NODE = "Failed to get GmodNode for ";
    static constexpr const char* ERROR_PREFIX_FAILED_LOCATION = "Failed to parse location ";
    static constexpr const char* ERROR_PREFIX_PATH_START = "Path must start with '";

    /**
     * @brief Represents a single node in a path being parsed
     * @details Holds the node code and optional location for short path parsing.
     *          Used during tree traversal to match path segments.
     */
    struct PathNode
    {
        std::string_view code;
        std::optional<Location> location;

        PathNode( std::string_view c )
            : code{ c },
              location{ std::nullopt }
        {
        }

        PathNode( std::string_view c, const Location& loc )
            : code{ c },
              location{ loc }
        {
        }
    };

    /**
     * @brief Context state for short path parsing with tree traversal
     * @details Maintains parsing state during gmod.traverse() calls, including
     *          remaining path parts, current search target, accumulated locations,
     *          and the final constructed path.
     */
    struct ParseContext
    {
        std::vector<PathNode> parts;
        size_t currentPartIndex;
        PathNode toFind;
        std::optional<HashMap<std::string, Location>> locations;
        std::optional<GmodPath> path;
        const Gmod* gmod;

        ParseContext(
            std::vector<PathNode>&& pathNodeVector,
            PathNode&& t,
            std::optional<HashMap<std::string, Location>>&& l,
            std::optional<GmodPath>&& gmodPath,
            const Gmod& g )
            : parts{ std::move( pathNodeVector ) },
              currentPartIndex{ 0 },
              toFind{ std::move( t ) },
              locations{ std::move( l ) },
              path{ std::move( gmodPath ) },
              gmod{ &g }
        {
        }
    };

    /**
     * @brief Traversal handler for short path parsing
     * @param context Parsing context with state
     * @param parents Parent nodes in current traversal path
     * @param current Current node being visited
     * @return TraversalHandlerResult indicating whether to continue, skip, or stop
     * @details Searches for matching nodes, builds parent chain, applies locations,
     *          and constructs the final GmodPath when all parts are found.
     */
    inline TraversalHandlerResult parseHandler(
        ParseContext& context, const StackVector<const GmodNode*, 16>& parents, const GmodNode& current )
    {
        PathNode& toFind = context.toFind;
        bool found = ( current.code() == toFind.code );

        if( !found && current.isLeafNode() )
        {
            return TraversalHandlerResult::SkipSubtree;
        }

        if( !found )
        {
            return TraversalHandlerResult::Continue;
        }

        if( toFind.location.has_value() )
        {
            if( !context.locations.has_value() )
            {
                context.locations = HashMap<std::string, Location>{};
                context.locations->reserve( 8 ); // Pre-allocate for typical path depth
            }
            context.locations->insertOrAssign( std::string{ toFind.code }, toFind.location.value() );
        }
        if( context.currentPartIndex < context.parts.size() )
        {
            toFind = context.parts[context.currentPartIndex++];
            return TraversalHandlerResult::Continue;
        }

        // Determine starting node for prefix building
        const GmodNode* firstParent = parents.isEmpty() ? nullptr : parents[0];
        const GmodNode* startNode = nullptr;

        if( firstParent && firstParent->parents().size() == 1 )
        {
            startNode = firstParent->parents()[0];
        }
        else if( current.parents().size() == 1 )
        {
            startNode = current.parents()[0];
        }

        if( startNode == nullptr || startNode->parents().size() > 1 )
        {
            return TraversalHandlerResult::Stop;
        }

        // Build list of prefix node pointers (just pointers, no copy yet)
        StackVector<const GmodNode*, 16> prefixNodePtrs;
        StackVector<std::string_view, 16> seenCodes;

        // Mark existing parents as seen
        for( const GmodNode* parent : parents )
        {
            seenCodes.push_back( parent->code() );
        }

        // Walk up to find prefix nodes
        while( startNode->parents().size() == 1 )
        {
            bool alreadySeen = false;
            for( const auto& code : seenCodes )
            {
                if( code == startNode->code() )
                {
                    alreadySeen = true;
                    break;
                }
            }
            if( !alreadySeen )
            {
                prefixNodePtrs.push_back( startNode );
                seenCodes.push_back( startNode->code() );
            }

            startNode = startNode->parents()[0];
            if( startNode->parents().size() > 1 )
            {
                return TraversalHandlerResult::Stop;
            }
        }

        // Check if root needed
        bool rootFound = false;
        for( const auto& code : seenCodes )
        {
            if( code == context.gmod->rootNode().code() )
            {
                rootFound = true;
                break;
            }
        }
        if( !rootFound )
        {
            prefixNodePtrs.push_back( &context.gmod->rootNode() );
        }

        // Now build final pathParents in ONE pass with exact size
        std::vector<GmodNode> pathParents;
        pathParents.reserve( prefixNodePtrs.size() + parents.size() );

        // Add prefix nodes in reverse order (they were built backwards)
        for( size_t i = prefixNodePtrs.size(); i > 0; --i )
        {
            pathParents.emplace_back( *prefixNodePtrs[i - 1] );
        }

        // Add traversal parents with locations if needed
        for( const GmodNode* parent : parents )
        {
            pathParents.emplace_back( *parent );
            if( context.locations.has_value() )
            {
                if( const auto* loc = context.locations->find( parent->code() ) )
                {
                    pathParents.back().setLocation( *loc );
                }
            }
        }

        // Create endNode with location if needed
        GmodNode endNode = current;
        if( toFind.location.has_value() )
        {
            endNode.setLocation( toFind.location.value() );
        }

        internal::LocationSetsVisitor visitor;

        for( size_t i = 0; i < pathParents.size() + 1; ++i )
        {
            GmodNode& n = ( i < pathParents.size() ) ? pathParents[i] : endNode;

            auto set = visitor.visit( n, i, pathParents, endNode );
            if( !set.has_value() )
            {
                if( n.location().has_value() )
                {
                    return TraversalHandlerResult::Stop;
                }
                continue;
            }

            const auto& [setStart, setEnd, location] = set.value();
            if( setStart == setEnd )
            {
                continue;
            }

            if( !location.has_value() )
            {
                continue;
            }

            for( size_t j = setStart; j <= setEnd; ++j )
            {
                if( j < pathParents.size() )
                {
                    pathParents[j].setLocation( *location );
                }
                else
                {
                    endNode.setLocation( *location );
                }
            }
        }

        context.path = GmodPath{ std::move( pathParents ), std::move( endNode ), true /* skipVerify */ };

        return TraversalHandlerResult::Stop;
    }

    /**
     * @brief Parse a short Gmod path string with detailed error reporting
     * @param item Short path string (e.g., "411.1/513-14C")
     * @param gmod Gmod instance to resolve nodes from
     * @param locations Locations instance to parse location suffixes
     * @return GmodParsePathResult containing either the parsed GmodPath or detailed error message
     * @details Uses tree traversal with gmod.traverse() to find complete paths from root.
     *          More complex than fromFullPathString as it must search the tree.
     */
    inline GmodParsePathResult fromShortPath(
        std::string_view item, const Gmod& gmod, const Locations& locations ) noexcept
    {
        if( gmod.version() != locations.version() )
        {
            return GmodParsePathResult::Error{ std::string( ERROR_DIFFERENT_VERSIONS ) };
        }

        if( item.empty() )
        {
            return GmodParsePathResult::Error{ ERROR_EMPTY_ITEM };
        }

        item = nfx::string::trim( item );
        if( item.empty() )
        {
            return GmodParsePathResult::Error{ ERROR_EMPTY_ITEM };
        }

        if( !item.empty() && item[0] == '/' )
        {
            item = item.substr( 1 );
        }

        StackVector<PathNode, 16> parts;

        for( const auto partStr : nfx::string::splitView( item, '/' ) )
        {
            if( partStr.empty() )
            {
                continue;
            }

            const size_t dashPos = partStr.find( '-' );
            std::string_view codePart = dashPos != std::string_view::npos ? partStr.substr( 0, dashPos ) : partStr;

            if( !gmod.node( codePart ).has_value() )
            {
                return GmodParsePathResult::Error{ std::string{ ERROR_PREFIX_FAILED_NODE } + std::string{ partStr } };
            }

            if( dashPos != std::string_view::npos )
            {
                const std::string_view locationPart = partStr.substr( dashPos + 1 );
                auto parsedLocationOpt = locations.fromString( locationPart );
                if( !parsedLocationOpt.has_value() )
                {
                    return GmodParsePathResult::Error{ std::string{ ERROR_PREFIX_FAILED_LOCATION } +
                                                       std::string{ locationPart } };
                }
                parts.emplace_back( codePart, parsedLocationOpt.value() );
            }
            else
            {
                parts.emplace_back( codePart );
            }
        }

        if( parts.isEmpty() )
        {
            return GmodParsePathResult::Error{ ERROR_NO_PARTS };
        }

        PathNode toFind = std::move( parts[0] );
        std::vector<PathNode> remainingParts(
            std::make_move_iterator( parts.begin() + 1 ), std::make_move_iterator( parts.end() ) );

        auto baseNode = gmod.node( toFind.code );
        if( !baseNode.has_value() )
        {
            return GmodParsePathResult::Error{ ERROR_NO_BASE_NODE };
        }

        ParseContext context( std::move( remainingParts ), std::move( toFind ), std::nullopt, std::nullopt, gmod );

        Gmod::TraverseHandlerWithState<ParseContext> handler = parseHandler;
        gmod.traverse( context, *baseNode.value(), handler, TraversalOptions{} );
        if( !context.path.has_value() )
        {
            return GmodParsePathResult::Error{ ERROR_FAILED_TRAVERSAL };
        }

        return GmodParsePathResult::Ok{ std::move( context.path.value() ) };
    }

    /**
     * @brief Parse a full Gmod path string with detailed error reporting
     * @param fullPathStr Full path string starting with root (e.g., "VE/400a/410/411/411i/411.1")
     * @param gmod Gmod instance to resolve nodes from
     * @param locations Locations instance to parse location suffixes
     * @return GmodParsePathResult containing either the parsed GmodPath or detailed error message
     * @details Direct parsing without tree traversal. Validates parent-child relationships
     *          and location individualization rules. Provides specific error messages for
     *          each failure point (empty string, missing root, invalid nodes, bad locations, etc.).
     */
    inline GmodParsePathResult fromFullPath(
        std::string_view fullPathStr, const Gmod& gmod, const Locations& locations ) noexcept
    {
        if( fullPathStr.empty() )
        {
            return GmodParsePathResult::Error{ ERROR_EMPTY_ITEM };
        }

        const std::string_view rootCode = gmod.rootNode().code();
        if( fullPathStr.size() < rootCode.size() || fullPathStr.substr( 0, rootCode.size() ) != rootCode )
        {
            return GmodParsePathResult::Error{ std::string{ ERROR_PREFIX_PATH_START } + std::string{ rootCode } + "'" };
        }

        std::vector<GmodNode> nodes;
        nodes.reserve( TYPICAL_PATH_DEPTH );

        for( const auto segment : nfx::string::splitView( fullPathStr, '/' ) )
        {
            if( segment.empty() )
            {
                continue;
            }

            const size_t dashPos = segment.find( '-' );

            if( dashPos != std::string_view::npos )
            {
                const std::string_view codePart = segment.substr( 0, dashPos );
                const std::string_view locationPart = segment.substr( dashPos + 1 );

                auto nodePtr = gmod.node( codePart );
                if( !nodePtr.has_value() )
                {
                    return GmodParsePathResult::Error{ ERROR_NODE_LOOKUP_FAILED };
                }

                auto parsedLocationOpt = locations.fromString( locationPart );
                if( !parsedLocationOpt.has_value() )
                {
                    return GmodParsePathResult::Error{ "Failed to parse location " + std::string{ locationPart } };
                }

                nodes.emplace_back( **nodePtr );
                nodes.back().setLocation( std::move( parsedLocationOpt.value() ) );
            }
            else
            {
                auto nodePtr = gmod.node( segment );
                if( !nodePtr.has_value() )
                {
                    return GmodParsePathResult::Error{ ERROR_NODE_LOOKUP_FAILED };
                }

                nodes.emplace_back( **nodePtr );
            }
        }
        if( nodes.empty() )
        {
            return GmodParsePathResult::Error{ ERROR_NO_NODES_FOUND };
        }

        GmodNode endNode = std::move( nodes.back() );
        nodes.pop_back();

        if( nodes.empty() )
        {
            return GmodParsePathResult::Ok{ GmodPath{ {}, std::move( endNode ) } };
        }

        if( !nodes[0].isRoot() )
        {
            return GmodParsePathResult::Error{ ERROR_INVALID_SEQUENCE };
        }

        bool hasLocations = endNode.location().has_value();
        for( const auto& node : nodes )
        {
            if( node.location().has_value() )
            {
                hasLocations = true;

                break;
            }
        }

        if( !hasLocations )
        {
            return GmodParsePathResult::Ok{ GmodPath{
                std::move( nodes ), std::move( endNode ), true /* skipVerify */ } };
        }

        LocationSetsVisitor locationSetsVisitor;
        std::optional<size_t> prevNonNullLocation;

        std::pair<size_t, size_t> sets[MAX_LOCATION_SETS];
        size_t setCounter = 0;

        for( size_t i = 0; i < nodes.size() + 1; ++i )
        {
            const GmodNode& n = ( i < nodes.size() ) ? nodes[i] : endNode;

            auto set = locationSetsVisitor.visit( n, i, nodes, endNode );
            if( !set.has_value() )
            {
                if( !prevNonNullLocation.has_value() && n.location().has_value() )
                {
                    prevNonNullLocation = i;
                }

                continue;
            }

            const auto& [setStart, setEnd, location] = set.value();

            if( prevNonNullLocation.has_value() )
            {
                for( size_t j = prevNonNullLocation.value(); j < setStart; ++j )
                {
                    const GmodNode& pn = ( j < nodes.size() ) ? nodes[j] : endNode;

                    if( pn.location().has_value() )
                    {
                        return GmodParsePathResult::Error{
                            "Expected all nodes in the set to be without individualization"
                        };
                    }
                }
            }
            prevNonNullLocation = std::nullopt;

            if( setCounter < MAX_LOCATION_SETS )
            {
                sets[setCounter++] = { setStart, setEnd };
            }

            if( setStart == setEnd )
            {
                continue;
            }

            if( !location.has_value() )
            {
                continue;
            }

            for( size_t j = setStart; j <= setEnd; ++j )
            {
                if( j < nodes.size() )
                {
                    nodes[j].setLocation( *location );
                }
                else
                {
                    endNode.setLocation( *location );
                }
            }
        }

        std::pair<size_t, size_t> currentSet = { std::numeric_limits<size_t>::max(),
                                                 std::numeric_limits<size_t>::max() };
        size_t currentSetIndex = 0;

        for( size_t i = 0; i < nodes.size() + 1; ++i )
        {
            while( currentSetIndex < setCounter &&
                   ( currentSet.second == std::numeric_limits<size_t>::max() || currentSet.second < i ) )
            {
                currentSet = sets[currentSetIndex++];
            }

            bool insideSet =
                ( currentSet.first != std::numeric_limits<size_t>::max() && i >= currentSet.first &&
                  i <= currentSet.second );
            const GmodNode& n = ( i < nodes.size() ) ? nodes[i] : endNode;

            if( insideSet )
            {
                const GmodNode& expectedLocationNode =
                    ( currentSet.second < nodes.size() ) ? nodes[currentSet.second] : endNode;

                if( n.location() != expectedLocationNode.location() )
                {
                    return GmodParsePathResult::Error{ "Expected all nodes in the set to be individualized the same" };
                }
            }
            else
            {
                if( n.location().has_value() )
                {
                    return GmodParsePathResult::Error{
                        "Expected all nodes in the set to be without individualization"
                    };
                }
            }
        }

        return GmodParsePathResult::Ok{ GmodPath{ std::move( nodes ), std::move( endNode ), true /* skipVerify */ } };
    }
} // namespace dnv::vista::sdk::internal
