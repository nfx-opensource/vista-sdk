/**
 * @file Gmod.h
 * @brief Generic Product Model (Gmod) container and tree structure
 * @details Manages the complete Gmod hierarchy for a specific VIS version.
 *          Provides node lookup, tree navigation, and iteration capabilities.
 *          All GmodNode instances are owned and managed by this class.
 */

#pragma once

#include "detail/Containers.h"
#include "GmodNode.h"
#include "VisVersions.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace dnv::vista::sdk
{
    struct GmodDto;

    /**
     * @brief Result type controlling tree traversal flow
     */
    enum class TraversalHandlerResult : std::uint8_t
    {
        Stop,        ///< Stop traversal immediately
        SkipSubtree, ///< Skip children of current node but continue traversal
        Continue     ///< Continue normal traversal
    };

    /**
     * @brief Options for controlling Gmod tree traversal behavior
     */
    struct TraversalOptions
    {
        /**
         * @brief Maximum number of times a node can occur in a traversal path
         * @details The traversal stops when a node occurrence reaches this limit.
         * Example: Path "411.1/C101.63/S206.22/S110.2/C101" with maxTraversalOccurrence=1
         * stops at the second C101 but includes it in the result.
         * Higher values drastically reduce performance.
         */
        int maxTraversalOccurrence = 1;
    };

    /**
     * @class Gmod
     * @brief Generic Product Model container for a specific VIS version
     * @details The Gmod class manages the complete tree hierarchy of GmodNode instances
     *          for a specific VIS version. It provides:
     *          - Node storage and ownership
     *          - Fast node lookup by code with operator[] and node()
     *          - Root node access ("VE")
     *          - Static helper methods for node classification
     */
    class Gmod final
    {
        friend class VIS;
        friend class internal::GmodVersioning;
        friend class internal::LocationSetsVisitor;
        friend internal::GmodParsePathResult internal::fromShortPath(
            std::string_view, const Gmod&, const Locations& ) noexcept;

    private:
        /**
         * @brief Construct Gmod from DTO data
         * @param version The VIS version for this Gmod
         * @param dto DTO containing nodes and relations
         */
        explicit Gmod( VisVersion version, const GmodDto& dto );

    public:
        /** @brief Default constructor */
        Gmod() = delete;

        /** @brief Copy constructor */
        Gmod( const Gmod& ) = delete;

        /** @brief Move constructor */
        Gmod( Gmod&& ) noexcept = default;

        /** @brief Destructor */
        ~Gmod() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this object
         */
        Gmod& operator=( const Gmod& ) = delete;

        /**
         * @brief Move assignment operator
         * @return Reference to this object
         */
        Gmod& operator=( Gmod&& ) noexcept = default;

        /**
         * @brief Access node by code with bounds checking
         * @param code Node code (e.g., "411.1", "VE", "C101")
         * @return Const reference to the GmodNode
         * @throws std::out_of_range if code not found
         */
        [[nodiscard]] const GmodNode& operator[]( std::string_view code ) const;

        /**
         * @brief Get the VIS version for this Gmod
         * @return The VisVersion enum value
         */
        [[nodiscard]] inline VisVersion version() const noexcept;

        /**
         * @brief Get the root node of the tree
         * @return Const reference to the root GmodNode (code "VE" - Vessel Equipment)
         */
        [[nodiscard]] inline const GmodNode& rootNode() const noexcept;

        /**
         * @brief Try to get a node by code
         * @param code Node code to search for (e.g., "411.1", "VE")
         * @return Optional pointer to the node if found, std::nullopt otherwise
         */
        [[nodiscard]] std::optional<const GmodNode*> node( std::string_view code ) const noexcept;

        /**
         * @brief Get iterator to first node
         * @return Iterator to the first element in the node map
         */
        [[nodiscard]] auto begin() const noexcept;
        /**
         * @brief Get iterator past last node
         * @return Iterator to the element following the last element
         */
        [[nodiscard]] auto end() const noexcept;
        /**
         * @brief Get const iterator to first node
         * @return Const iterator to the first element in the node map
         */
        [[nodiscard]] auto cbegin() const noexcept;
        /**
         * @brief Get const iterator past last node
         * @return Const iterator to the element following the last element
         */
        [[nodiscard]] auto cend() const noexcept;

    public:
        /**
         * @brief Callback function type for tree traversal without state
         * @param parents Current path of parent node pointers from root to current position
         * @param node Current node being visited
         * @return TraversalHandlerResult indicating how to continue traversal
         */
        using TraverseHandler = std::function<TraversalHandlerResult(
            const SmallVector<const GmodNode*, 16>& parents, const GmodNode& node )>;

        /**
         * @brief Callback function type for tree traversal with custom state
         * @tparam TState Type of the state object passed to the handler
         * @param state Reference to mutable state object
         * @param parents Current path of parent node pointers from root to current position
         * @param node Current node being visited
         * @return TraversalHandlerResult indicating how to continue traversal
         */
        template <typename TState>
        using TraverseHandlerWithState = std::function<TraversalHandlerResult(
            TState& state, const SmallVector<const GmodNode*, 16>& parents, const GmodNode& node )>;

        /**
         * @brief Traverse the Gmod tree from root with a callback handler
         * @param handler Callback function invoked for each node
         * @param options Traversal options (pass by value, cheap to copy)
         * @return True if traversal completed, false if stopped early
         */
        bool traverse( TraverseHandler handler, TraversalOptions options = TraversalOptions{} ) const;

        /**
         * @brief Traverse the Gmod tree from root with state and callback handler
         * @tparam TState Type of state object
         * @param state State object passed to handler (by reference, can be modified)
         * @param handler Callback function invoked for each node
         * @param options Traversal options (pass by value, cheap to copy)
         * @return True if traversal completed, false if stopped early
         */
        template <typename TState>
        bool traverse(
            TState& state,
            TraverseHandlerWithState<TState> handler,
            TraversalOptions options = TraversalOptions{} ) const;

    private:
        /**
         * @brief Traverse the Gmod tree from a specific node with a callback handler
         * @param rootNode Starting node for traversal
         * @param handler Callback function invoked for each node
         * @param options Traversal options (pass by value, cheap to copy)
         * @return True if traversal completed, false if stopped early
         */
        bool traverse( const GmodNode& rootNode, TraverseHandler handler, TraversalOptions options ) const;

        /**
         * @brief Traverse the Gmod tree from a specific node with state and callback handler
         * @tparam TState Type of state object
         * @param state State object passed to handler (by reference, can be modified)
         * @param rootNode Starting node for traversal
         * @param handler Callback function invoked for each node
         * @param options Traversal options (pass by value, cheap to copy)
         * @return True if traversal completed, false if stopped early
         */
        template <typename TState>
        bool traverse(
            TState& state,
            const GmodNode& rootNode,
            TraverseHandlerWithState<TState> handler,
            TraversalOptions options ) const;

        /**
         * @brief Check if parent-child relationship is a product type assignment
         * @param parent Parent node (can be nullptr)
         * @param child Child node (can be nullptr)
         * @return True if parent is a FUNCTION node and child is a PRODUCT TYPE
         * @details Product type assignments link functions to their product implementations.
         */
        [[nodiscard]] static inline bool isProductTypeAssignment(
            const GmodNode* parent, const GmodNode* child ) noexcept;

        /**
         * @brief Check if parent-child relationship is a product selection assignment
         * @param parent Parent node (can be nullptr)
         * @param child Child node (can be nullptr)
         * @return True if parent is a FUNCTION node and child is a PRODUCT SELECTION
         * @details Product selection assignments link functions to product choice points.
         */
        [[nodiscard]] static inline bool isProductSelectionAssignment(
            const GmodNode* parent, const GmodNode* child ) noexcept;

        /**
         * @brief Check if a node type can have child scopes
         * @param type Node type string (e.g., "SELECTION", "GROUP", "LEAF", "COMPOSITION")
         * @return True if type is "SELECTION", "GROUP", or "LEAF"
         * @details Internal helper - these types can have location-based child scopes in the tree hierarchy.
         */
        [[nodiscard]] static constexpr bool isPotentialParent( std::string_view type ) noexcept;

        /**
         * @brief Check if a path exists from a given path to a target node
         * @param fromPath Current path of parent nodes
         * @param to Target node to reach
         * @param[out] remainingParents Remaining parent nodes in the path to target
         * @return True if path exists, false otherwise
         */
        template <std::size_t N, std::size_t M>
        bool pathExistsBetween(
            const SmallVector<const GmodNode*, N>& fromPath,
            const GmodNode& to,
            SmallVector<const GmodNode*, M>& remainingParents ) const;

    private:
        VisVersion m_visVersion; ///< VIS version for this Gmod
        GmodNode* m_rootNode;    ///< Pointer to root node ("VE") in m_nodeMap
        PerfectHashMap<std::string, GmodNode>
            m_nodeMap; ///< All nodes indexed by code (owns all nodes, transparent hash)

    private:
        class TraversalParents
        {
        public:
            TraversalParents()
            {
                m_occurrences.reserve( 4 );
            }

            void push( const GmodNode* parent )
            {
                m_parents.push_back( parent );
                if( auto* count = m_occurrences.find( parent->code() ) )
                {
                    ++( *count );
                }
                else
                {
                    m_occurrences.emplace( parent->code(), 1 );
                }
            }

            void pop()
            {
                const GmodNode* parent = m_parents.back();
                m_parents.pop_back();

                if( auto* count = m_occurrences.find( parent->code() ) )
                {
                    if( *count == 1 )
                    {
                        m_occurrences.erase( parent->code() );
                    }
                    else
                    {
                        --( *count );
                    }
                }
            }

            [[nodiscard]] int occurrences( const GmodNode& node ) const
            {
                if( const auto* count = m_occurrences.find( node.code() ) )
                {
                    return *count;
                }
                return 0;
            }

            [[nodiscard]] const GmodNode* lastOrDefault() const noexcept
            {
                return m_parents.isEmpty() ? nullptr : m_parents.back();
            }

            [[nodiscard]] const SmallVector<const GmodNode*, 16>& asList() const noexcept
            {
                return m_parents;
            }

            [[nodiscard]] bool empty() const noexcept
            {
                return m_parents.isEmpty();
            }

        private:
            SmallVector<const GmodNode*, 16> m_parents;
            HashMap<std::string_view, int> m_occurrences;
        };
    };
} // namespace dnv::vista::sdk

#include "detail/Gmod.inl"
