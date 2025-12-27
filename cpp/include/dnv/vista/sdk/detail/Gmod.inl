/**
 * @file Gmod.inl
 * @brief Inline implementations for Gmod class
 * @details Provides inline implementations for simple accessor methods.
 */

#include <nfx/StringUtils.h>

namespace dnv::vista::sdk
{
    inline const GmodNode& Gmod::operator[]( std::string_view key ) const
    {
        auto* nodePtr = m_nodeMap.find( key );
        if( nodePtr != nullptr )
        {
            return *nodePtr;
        }

        throw std::out_of_range{ "Key not found in Gmod node map: " + std::string{ key } };
    }

    inline VisVersion Gmod::version() const noexcept
    {
        return m_visVersion;
    }

    inline const GmodNode& Gmod::rootNode() const noexcept
    {
        return *m_rootNode;
    }

    inline std::optional<const GmodNode*> Gmod::node( std::string_view code ) const noexcept
    {
        auto* nodePtr = m_nodeMap.find( code );
        if( nodePtr != nullptr )
        {
            return nodePtr;
        }
        return std::nullopt;
    }

    inline auto Gmod::begin() const noexcept
    {
        return m_nodeMap.begin();
    }

    inline auto Gmod::end() const noexcept
    {
        return m_nodeMap.end();
    }

    inline auto Gmod::cbegin() const noexcept
    {
        return m_nodeMap.cbegin();
    }

    inline auto Gmod::cend() const noexcept
    {
        return m_nodeMap.cend();
    }

    template <typename TState>
    inline bool Gmod::traverse(
        TState& state, TraverseHandlerWithState<TState> handler, TraversalOptions options ) const
    {
        return traverse( state, *m_rootNode, handler, options );
    }

    template <typename TState>
    inline bool Gmod::traverse(
        TState& state,
        const GmodNode& rootNode,
        TraverseHandlerWithState<TState> handler,
        TraversalOptions options ) const
    {
        TraversalParents parents;

        std::function<TraversalHandlerResult( const GmodNode& )> traverseNode;
        traverseNode = [&]( const GmodNode& node ) -> TraversalHandlerResult {
            auto result = handler( state, parents.asList(), node );
            if( result == TraversalHandlerResult::Stop || result == TraversalHandlerResult::SkipSubtree )
            {
                return result;
            }

            const GmodNode* lastParent = parents.lastOrDefault();
            bool skipOccurrenceCheck = isProductSelectionAssignment( lastParent, &node );

            if( !skipOccurrenceCheck )
            {
                int occ = parents.occurrences( node );

                if( occ == options.maxTraversalOccurrence )
                {
                    return TraversalHandlerResult::SkipSubtree;
                }
                if( occ > options.maxTraversalOccurrence )
                {
                    throw std::runtime_error{ "Invalid state - node occurred more than expected" };
                }
            }

            parents.push( &node );

            for( const auto* child : node.children() )
            {
                result = traverseNode( *child );
                if( result == TraversalHandlerResult::Stop )
                {
                    parents.pop();
                    return result;
                }
            }

            parents.pop();

            return TraversalHandlerResult::Continue;
        };

        return traverseNode( rootNode ) == TraversalHandlerResult::Continue;
    }

    inline bool Gmod::isProductTypeAssignment( const GmodNode* parent, const GmodNode* child ) noexcept
    {
        if( !parent || !child )
        {
            return false;
        }
        if( !parent->isFunctionNode() )
        {
            return false;
        }
        return child->isProductType();
    }

    inline bool Gmod::isProductSelectionAssignment( const GmodNode* parent, const GmodNode* child ) noexcept
    {
        if( !parent || !child )
        {
            return false;
        }
        if( !parent->isFunctionNode() )
        {
            return false;
        }
        return child->isProductSelection();
    }

    constexpr bool Gmod::isPotentialParent( std::string_view type ) noexcept
    {
        return type == "SELECTION" || type == "GROUP" || type == "LEAF";
    }

    template <std::size_t N, std::size_t M>
    inline bool Gmod::pathExistsBetween(
        const SmallVector<const GmodNode*, N>& fromPath,
        const GmodNode& to,
        SmallVector<const GmodNode*, M>& remainingParents ) const
    {
        remainingParents.clear();

        const GmodNode* lastAssetFunction = nullptr;
        size_t assetFunctionIndex = std::numeric_limits<size_t>::max();
        for( size_t i = fromPath.size(); i > 0; --i )
        {
            size_t idx = i - 1;
            if( fromPath[idx]->isAssetFunctionNode() )
            {
                lastAssetFunction = fromPath[idx];
                assetFunctionIndex = idx;
                break;
            }
        }

        const GmodNode& startNode = lastAssetFunction ? *lastAssetFunction : rootNode();

        struct PathExistsState
        {
            const GmodNode& targetNode;
            const SmallVector<const GmodNode*, N>& fromPath;
            SmallVector<const GmodNode*, M>& remainingParents;
            size_t assetFunctionIndex;
            bool found = false;

            PathExistsState(
                const GmodNode& target,
                const SmallVector<const GmodNode*, N>& from,
                SmallVector<const GmodNode*, M>& remaining,
                size_t afIndex )
                : targetNode{ target },
                  fromPath{ from },
                  remainingParents{ remaining },
                  assetFunctionIndex{ afIndex }
            {
            }

            PathExistsState( const PathExistsState& ) = delete;
            PathExistsState( PathExistsState&& ) = delete;
            PathExistsState& operator=( const PathExistsState& ) = delete;
            PathExistsState& operator=( PathExistsState&& ) = delete;
        };

        PathExistsState state( to, fromPath, remainingParents, assetFunctionIndex );

        auto handler = [this](
                           PathExistsState& s,
                           const SmallVector<const GmodNode*, 16>& parents,
                           const GmodNode& node ) -> TraversalHandlerResult {
            if( !nfx::string::equals( node.code(), s.targetNode.code() ) )
            {
                return TraversalHandlerResult::Continue;
            }

            SmallVector<const GmodNode*, 16> completePath;
            completePath.reserve( parents.size() );
            for( const GmodNode* parent : parents )
            {
                if( !parent->isRoot() )
                {
                    completePath.push_back( parent );
                }
            }
            size_t startIndex = 0;

            if( s.assetFunctionIndex != std::numeric_limits<size_t>::max() )
            {
                startIndex = s.assetFunctionIndex;
            }

            size_t requiredNodes = s.fromPath.size() - startIndex;
            if( completePath.size() < requiredNodes )
            {
                return TraversalHandlerResult::Continue;
            }

            bool match = true;
            for( size_t i = 0; i < requiredNodes; ++i )
            {
                size_t fromPathIdx = startIndex + i;
                if( fromPathIdx >= s.fromPath.size() || i >= completePath.size() )
                {
                    match = false;
                    break;
                }
                if( !nfx::string::equals( completePath[i]->code(), s.fromPath[fromPathIdx]->code() ) )
                {
                    match = false;
                    break;
                }
            }

            if( match )
            {
                for( size_t i = requiredNodes; i < completePath.size(); ++i )
                {
                    s.remainingParents.push_back( completePath[i] );
                }
                s.found = true;
                return TraversalHandlerResult::Stop;
            }

            return TraversalHandlerResult::Continue;
        };

        TraverseHandlerWithState<PathExistsState> wrappedHandler = handler;

        traverse( state, startNode, wrappedHandler, TraversalOptions{} );

        return state.found;
    }
} // namespace dnv::vista::sdk
