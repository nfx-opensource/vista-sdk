/**
 * @file Gmod.h
 * @brief VISTA Generic Product Model (GMOD) System for Maritime Vessel Structure Management
 *
 * @details
 * This file implements the **VISTA Generic Product Model (GMOD) System** for managing
 * and navigating hierarchical vessel component structures. It provides comprehensive
 * tree traversal, path parsing, node lookup, and relationship management capabilities
 * for maritime vessel information structures according to ISO 19848 standards.
 *
 * ## System Purpose:
 *
 * The **VISTA GMOD System** serves as the foundation for:
 * - **Vessel Structure Navigation** : Hierarchical traversal of ship components and systems
 * - **Path Resolution**             : Parsing and validating GMOD path strings for component identification
 * - **Node Relationship Management**: Managing parent-child relationships in vessel hierarchies
 * - **Component Lookup**            : High-performance O(1) node access by component codes
 * - **Standards Compliance**        : Full adherence to ISO 19848 maritime equipment standards
 * - **Tree Traversal Operations**   : Flexible iteration patterns with occurrence tracking
 *
 * ## Core Architecture:
 *
 * ### GMOD Classes
 * - **Gmod**            : Main container managing the complete vessel structure tree
 * - **GmodNode**        : Individual vessel component with metadata and relationships
 * - **GmodNodeMetadata**: Descriptive information (category, type, name) for components
 * - **GmodPath**        : Parsed representation of hierarchical component paths
 *
 * ### Traversal Framework
 * - **TraverseHandler**         : Stateless function pointer for tree traversal operations
 * - **TraverseHandlerWithState**: Stateful traversal with user-defined context
 * - **TraversalOptions**        : Configuration for occurrence limits and cycle prevention
 * - **TraversalHandlerResult**  : Flow control for traversal continuation decisions
 *
 * ## Data Flow Architecture:
 *
 * ```
 * GmodDto (External Data)
 *         ↓
 * Gmod Construction (VIS Factory)
 *         ↓
 * ┌─────────────────────────────────────┐
 * │              Gmod                   │
 * ├─────────────────────────────────────┤
 * │ ┌─────────────────────────────────┐ │
 * │ │      ChdHashMap<GmodNode>       │ │ ← O(1) node lookup by code
 * │ │    (Perfect hash for nodes)     │ │
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │        GmodNode* rootNode       │ │ ← Tree root ("VE" vessel entry)
 * │ │      (Vessel Entry Point)       │ │
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │      VisVersion metadata        │ │ ← Version-specific validation
 * │ │    (Standards compliance)       │ │
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *         ↓
 * Path Parsing & Tree Navigation
 * ```
 *
 * ## Traversal Patterns:
 *
 * ### Basic Tree Traversal
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Path-Based Navigation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Performance Characteristics:
 *
 * - **O(1) Node Lookup** : Perfect hash-based node access by component codes
 * - **Memory Efficient** : Optimized tree structure with minimal pointer overhead
 * - **Zero-Copy Access** : `string_view` interfaces for path and metadata access
 * - **Thread Safe Reads**: Immutable design safe for concurrent navigation
 * - **Cycle Prevention** : Occurrence tracking prevents infinite loops in traversal
 * - **Path Caching**     : Efficient path parsing with validation caching
 *
 * ## Node Classification System:
 *
 * ### Component Types
 * - **Vessel Entry (VE)** : Root node representing the complete vessel
 * - **System (SYS)**      : Major ship systems (propulsion, navigation, etc.)
 * - **Equipment (E)**     : Individual equipment items and machinery
 * - **Product Types**     : Standardized equipment classifications
 * - **Product Selections**: Specific equipment choices and configurations
 * - **Function Nodes**    : Functional categorization of components
 * - **Asset Nodes**       : Physical assets with lifecycle management
 *
 * ### Relationship Patterns
 * - **Parent-Child Links**     : Hierarchical containment relationships
 * - **Product Type Assignment**: Equipment type classification relationships
 * - **Product Selection**      : Specific equipment selection relationships
 * - **Function Assignment**    : Functional role assignments
 *
 * ## Path Resolution System:
 *
 * ### Path Format Support
 * - **Relative Paths**  :
 * - **Absolute Paths**  :
 * - **Bracket Notation**:
 * - **Code-Only Paths** :
 *
 * ### Validation Levels
 * - **Syntax Validation**      : Correct path format and bracket matching
 * - **Component Existence**    : Verify all path components exist in GMOD
 * - **Relationship Validation**: Confirm parent-child relationships are valid
 * - **Standards Compliance**   : Adherence to ISO 19848 naming conventions
 *
 * ## Design Philosophy:
 *
 * - **Standards Compliance**: Full adherence to ISO 19848 maritime equipment standards
 * - **Performance Focus**   : Optimized for high-frequency navigation operations
 * - **Type Safety**         : Strong typing with comprehensive error handling
 * - **Extensibility**       : Support for custom traversal patterns and node types
 * - **Usability**           : Intuitive API for common vessel structure operations
 * - **Immutability**        : Thread-safe design with controlled mutation patterns
 * - **Factory Pattern**     : Controlled construction through VIS factory class
 *
 * @note This system is designed for high-performance maritime vessel structure
 *       navigation with full ISO 19848 standard compliance. All traversal and
 *       lookup operations are optimized for frequent use in vessel data processing.
 *
 * @see ISO 19848 - Ships and marine technology - Standard data for shipboard machinery and equipment
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <nfx/containers/ChdHashMap.h>
#include <nfx/containers/HashMap.h>

#include "GmodNode.h"

#include "config/config.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	enum class VisVersion : std::uint16_t;

	class GmodDto;
	class GmodNodeMetadata;
	class GmodPath;
	class VIS;

	namespace internal
	{
		class EmbeddedResource;
	}

	//=====================================================================
	// Traversal
	//=====================================================================

	//----------------------------------------------
	// Traversal handler
	//----------------------------------------------

	/**
	 * @enum TraversalHandlerResult
	 * @brief Controls GMOD tree traversal flow and continuation behavior
	 * @details Return values from traversal handler functions that determine
	 *          how the traversal algorithm should proceed with the current node
	 *          and its subtree.
	 */
	enum class TraversalHandlerResult
	{
		/** @brief Stop traversal entirely and return immediately */
		Stop = 0,

		/** @brief Skip traversing the current node's children, but continue with siblings */
		SkipSubtree,

		/** @brief Continue normal traversal of children and siblings */
		Continue,
	};

	/**
	 * @typedef TraverseHandler
	 * @brief Function pointer type for stateless GMOD tree traversal
	 * @details Handler function called for each node during traversal. Receives
	 *          the parent chain and current node, returns continuation instruction.
	 * @param parents Vector of parent nodes from root to immediate parent
	 * @param node Current node being visited
	 * @return TraversalHandlerResult indicating how to continue traversal
	 */
	using TraverseHandler = TraversalHandlerResult ( * )( const std::vector<const GmodNode*>& parents, const GmodNode& node );

	/**
	 * @typedef TraverseHandlerWithState
	 * @brief Function pointer type for stateful GMOD tree traversal
	 * @details Handler function with user-defined state for complex traversal scenarios.
	 *          Allows maintaining custom data across traversal operations.
	 * @tparam TState User-defined state type passed to handler
	 * @param state Reference to user-defined state object
	 * @param parents Vector of parent nodes from root to immediate parent
	 * @param node Current node being visited
	 * @return TraversalHandlerResult indicating how to continue traversal
	 */
	template <typename TState>
	using TraverseHandlerWithState = TraversalHandlerResult ( * )( TState& state, const std::vector<const GmodNode*>& parents, const GmodNode& node );

	/**
	 * @struct TraversalOptions
	 * @brief Traversal configuration options for GMOD tree traversal
	 * @details Configures behavior during tree traversal operations, including
	 *          occurrence limits to prevent infinite loops in cyclic structures.
	 */
	struct TraversalOptions
	{
		static constexpr size_t DEFAULT_MAX_TRAVERSAL_OCCURRENCE = 1;
		constexpr TraversalOptions() = default;
		size_t maxTraversalOccurrence = DEFAULT_MAX_TRAVERSAL_OCCURRENCE;
	};

	//=====================================================================
	// Gmod class
	//=====================================================================

	/**
	 * @brief Represents the Generic Product Model (GMOD) for a specific VIS version.
	 * @details The Gmod class provides an interface to the hierarchical structure of
	 *          vessel components and systems. It allows for node lookup, path parsing,
	 *          and iteration over its constituent nodes. The GMOD is typically initialized
	 *          from a GmodDto object or a pre-populated map of nodes.
	 */
	class Gmod final
	{
		friend class VIS;

	public:
		//----------------------------------------------
		// Forward declarations
		//----------------------------------------------

		class Enumerator;

	private:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs a Gmod instance from a GmodDto.
		 * @details Private constructor accessible only through friend classes (VIS, EmbeddedResource).
		 *          This ensures controlled creation of GMOD instances with proper validation.
		 * @param version The VIS version this GMOD corresponds to.
		 * @param dto The data transfer object containing the GMOD structure and nodes.
		 * @throws std::invalid_argument If the provided DTO is invalid or essential data is missing.
		 * @throws std::out_of_range If essential nodes (like 'VE') are not found during construction.
		 * @throws Other exceptions based on underlying operations (e.g., memory allocation).
		 */
		Gmod( VisVersion version, const GmodDto& dto );

		/**
		 * @brief Constructs a Gmod instance from an initial map of nodes.
		 * @details Private constructor typically used for testing or specialized GMOD setup.
		 *          Only accessible through friend classes. The GMOD copies these nodes.
		 *          It's the caller's responsibility to ensure the provided nodes form a
		 *          consistent, linkable structure.
		 * @param version The VIS version this GMOD corresponds to.
		 * @param initialNodeMap An unordered map where keys are node codes and values are GmodNode objects.
		 * @throws std::out_of_range If the root node 'VE' is not found in `initialNodeMap`.
		 */
		Gmod( VisVersion version, const std::unordered_map<std::string, GmodNode>& initialNodeMap );

		/** @brief Default constructor */
		Gmod() = delete;

		/** @brief Copy constructor */
		Gmod( const Gmod& );

	public:
		/** @brief Move constructor */
		Gmod( Gmod&& ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~Gmod() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		Gmod& operator=( const Gmod& ) = default;

		/** @brief Move assignment operator */
		Gmod& operator=( Gmod&& ) noexcept = default;

		//----------------------------------------------
		// Lookup operators
		//----------------------------------------------

		/**
		 * @brief Accesses a GmodNode by its code.
		 * @param key The code of the GmodNode to access.
		 * @return A const reference to the GmodNode.
		 * @throws std::out_of_range If no node with the specified key exists.
		 */
		inline const GmodNode& operator[]( std::string_view key ) const;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Gets the VIS version of this GMOD instance.
		 * @return The VisVersion enum value.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE VisVersion visVersion() const;

		/**
		 * @brief Gets the root node of the GMOD hierarchy.
		 * @details The root node (typically with code "VE") serves as the entry point
		 *          for traversing the GMOD structure.
		 * @return A const reference to the root GmodNode.
		 * @throws std::runtime_error If the GMOD is not properly initialized or has no root node.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const GmodNode& rootNode() const;

		//----------------------------------------------
		// Node query methods
		//----------------------------------------------

		/**
		 * @brief Tries to get a GmodNode by its code.
		 * @param code The code of the GmodNode to find (as a string_view).
		 * @param[out] node Output parameter; set to a pointer to the GmodNode if found, otherwise nullptr.
		 * @return True if the node was found, false otherwise.
		 */
		VISTA_SDK_CPP_INLINE bool tryGetNode( std::string_view code, GmodNode*& node ) const noexcept;

		//----------------------------------------------
		// Path parsing & navigation
		//----------------------------------------------

		/**
		 * @brief Parses a GMOD path string relative to the GMOD's root node.
		 * @param item The GMOD path string (e.g., "SYS[Propulsion System]/E[Main Engine]").
		 * @return A GmodPath object representing the parsed path.
		 * @throws std::invalid_argument If the path string is invalid or cannot be parsed.
		 */
		GmodPath parsePath( std::string_view item ) const;

		/**
		 * @brief Tries to parse a GMOD path string relative to the GMOD's root node.
		 * @param item The GMOD path string.
		 * @param[out] path Output parameter; set to the parsed GmodPath if successful.
		 * @return True if parsing was successful, false otherwise.
		 */
		bool tryParsePath( std::string_view item, std::optional<GmodPath>& path ) const noexcept;

		/**
		 * @brief Parses a full GMOD path string, which must start from the root "VE".
		 * @param item The full GMOD path string (e.g., "VE/SYS[Propulsion System]/E[Main Engine]").
		 * @return A GmodPath object representing the parsed path.
		 * @throws std::invalid_argument If the path string is invalid, not a full path, or cannot be parsed.
		 */
		GmodPath parseFromFullPath( std::string_view item ) const;

		/**
		 * @brief Tries to parse a full GMOD path string.
		 * @param item The full GMOD path string.
		 * @param[out] path Output parameter; set to the parsed GmodPath if successful.
		 * @return True if parsing was successful, false otherwise.
		 */
		bool tryParseFromFullPath( std::string_view item, std::optional<GmodPath>& path ) const noexcept;

		//----------------------------------------------
		// Traversal methods
		//----------------------------------------------

		/**
		 * @brief Traverse GMOD tree with handler
		 * @param handler Function to call for each node
		 * @param options Traversal configuration
		 * @return true if completed, false if stopped early
		 */
		VISTA_SDK_CPP_INLINE bool traverse( TraverseHandler handler, const TraversalOptions& options = {} ) const;

		/**
		 * @brief Traverse GMOD tree with stateful handler
		 * @tparam TState User-defined state type
		 * @param state User state reference
		 * @param handler Stateful handler function
		 * @param options Traversal configuration
		 * @return true if completed, false if stopped early
		 */
		template <typename TState>
		VISTA_SDK_CPP_INLINE bool traverse( TState& state, TraverseHandlerWithState<TState> handler, const TraversalOptions& options = {} ) const;

		/**
		 * @brief Traverse from specific root node
		 * @param rootNode Starting node
		 * @param handler Function to call for each node
		 * @param options Traversal configuration
		 * @return true if completed, false if stopped early
		 */
		VISTA_SDK_CPP_INLINE bool traverse( const GmodNode& rootNode, TraverseHandler handler, const TraversalOptions& options = {} ) const;

		/**
		 * @brief Traverse from specific root with stateful handler
		 * @tparam TState User-defined state type
		 * @param state User state reference
		 * @param rootNode Starting node
		 * @param handler Stateful handler function
		 * @param options Traversal configuration
		 * @return true if completed, false if stopped early
		 */
		template <typename TState>
		VISTA_SDK_CPP_INLINE bool traverse(
			TState& state, const GmodNode& rootNode, TraverseHandlerWithState<TState> handler, const TraversalOptions& options = {} ) const;

		/**
		 * @brief Checks if a traversal path exists between given parents and target node
		 * @param fromPath Vector of parent nodes representing the starting path
		 * @param to Target node to find
		 * @param remainingParents Output vector for additional parents not in fromPath
		 * @return true if path exists, false otherwise
		 */
		bool pathExistsBetween( const std::vector<const GmodNode*>& fromPath, const GmodNode& to, std::vector<const GmodNode*>& remainingParents ) const;

		//----------------------------------------------
		// Static state inspection methods
		//----------------------------------------------

		/**
		 * @brief Checks if a GMOD node type string indicates a potential parent node.
		 * @param type The node type string (e.g., "SYS", "E", "VE").
		 * @return True if the type suggests it can be a parent, false otherwise.
		 */
		static bool isPotentialParent( std::string_view type ) noexcept;

		/**
		 * @brief Checks if a GMOD node, based on its metadata, is a leaf node.
		 * @param metadata The metadata of the GmodNode.
		 * @return True if the node is considered a leaf node, false otherwise.
		 */
		static bool isLeafNode( const GmodNodeMetadata& metadata ) noexcept;

		/**
		 * @brief Checks if a GMOD node, based on its metadata, is a function node.
		 * @param metadata The metadata of the GmodNode.
		 * @return True if the node is a function node, false otherwise.
		 */
		static bool isFunctionNode( const GmodNodeMetadata& metadata ) noexcept;

		/**
		 * @brief Checks if a GMOD node, based on its metadata, represents a product selection.
		 * @param metadata The metadata of the GmodNode.
		 * @return True if the node is a product selection, false otherwise.
		 */
		static bool isProductSelection( const GmodNodeMetadata& metadata ) noexcept;

		/**
		 * @brief Checks if a GMOD node, based on its metadata, represents a product type.
		 * @param metadata The metadata of the GmodNode.
		 * @return True if the node is a product type, false otherwise.
		 */
		static bool isProductType( const GmodNodeMetadata& metadata ) noexcept;

		/**
		 * @brief Checks if a GMOD node, based on its metadata, represents an asset.
		 * @param metadata The metadata of the GmodNode.
		 * @return True if the node is an asset, false otherwise.
		 */
		static bool isAsset( const GmodNodeMetadata& metadata ) noexcept;

		/**
		 * @brief Checks if a GMOD node, based on its metadata, is an asset function node.
		 * @param metadata The metadata of the GmodNode.
		 * @return True if the node is an asset function node, false otherwise.
		 */
		static bool isAssetFunctionNode( const GmodNodeMetadata& metadata ) noexcept;

		/**
		 * @brief Checks if the relationship between a parent and child node constitutes a product type assignment.
		 * @param parent Pointer to the parent GmodNode. Can be nullptr.
		 * @param child Pointer to the child GmodNode. Can be nullptr.
		 * @return True if it's a product type assignment, false otherwise or if inputs are invalid.
		 */
		static bool isProductTypeAssignment( const GmodNode* parent, const GmodNode* child ) noexcept;

		/**
		 * @brief Checks if the relationship between a parent and child node constitutes a product selection assignment.
		 * @param parent Pointer to the parent GmodNode. Can be nullptr.
		 * @param child Pointer to the child GmodNode. Can be nullptr.
		 * @return True if it's a product selection assignment, false otherwise or if inputs are invalid.
		 */
		static bool isProductSelectionAssignment( const GmodNode* parent, const GmodNode* child ) noexcept;

		//----------------------------------------------
		// Enumeration
		//----------------------------------------------

		/**
		 * @brief Gets an enumerator for iterating over all nodes within this Gmod instance.
		 * @return An Enumerator object.
		 */
		inline Enumerator enumerator() const;

		//----------------------------------------------
		// Gmod::Enumerator class
		//----------------------------------------------

		/**
		 * @brief An enumerator for iterating over all nodes within a Gmod instance.
		 * @details Provides a way to access each GmodNode in the GMOD's internal collection.
		 *          The order of iteration depends on the underlying ChdHashMap.
		 */
		class Enumerator final
		{
			friend class Gmod;

			//-----------------------------
			// Construction
			//-----------------------------

		private:
			/**
			 * @brief Private constructor, typically called by Gmod::enumerator().
			 * @param map Pointer to the ChdHashMap of GmodNodes to iterate over.
			 */
			VISTA_SDK_CPP_INLINE Enumerator( const nfx::containers::ChdHashMap<GmodNode>* map ) noexcept;

		public:
			/** @brief Default constructor. */
			Enumerator() = delete;

			/** @brief Copy constructor */
			Enumerator( const Enumerator& ) = delete;

			/** @brief Move constructor */
			Enumerator( Enumerator&& ) noexcept = delete;

			//-----------------------------
			// Assignment operators
			//-----------------------------

			/** @brief Copy assignment operator */
			Enumerator& operator=( const Enumerator& ) = delete;

			/** @brief Move assignment operator */
			Enumerator& operator=( Enumerator&& ) noexcept = delete;

			//-----------------------------
			// Iteration interface
			//-----------------------------

			/**
			 * @brief Gets the current GmodNode in the iteration.
			 * @return A const reference to the current GmodNode.
			 * @throws std::runtime_error If called when the enumerator is in an invalid state
			 *                            (e.g., before the first moveNext() or after iteration has ended).
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] VISTA_SDK_CPP_INLINE const GmodNode& current() const;

			/**
			 * @brief Advances the enumerator to the next GmodNode in the collection.
			 * @return True if the enumerator was successfully advanced to the next node;
			 *         false if the end of the collection has been passed.
			 */
			bool VISTA_SDK_CPP_INLINE next() noexcept;

			/**
			 * @brief Resets the enumerator to its initial state, positioned before the first node.
			 * @details After calling reset, next() must be called to access the first node.
			 */
			void VISTA_SDK_CPP_INLINE reset() noexcept;

			//-----------------------------
			// Private member variables
			//-----------------------------

			const nfx::containers::ChdHashMap<GmodNode>* m_sourceMapPtr;
			nfx::containers::ChdHashMap<GmodNode>::Iterator m_currentMapIterator;
			bool m_isInitialState;
		};

	private:
		//----------------------------------------------
		// Private traversal implementation
		//----------------------------------------------

		/**
		 * @class Parents
		 * @brief Parent stack with occurrence tracking
		 */
		class Parents
		{
		public:
			/**
			 * @brief Constructor for parent stack
			 * @details Initializes an empty parent stack for traversal operations
			 */
			VISTA_SDK_CPP_INLINE Parents();

			/**
			 * @brief Pushes a parent node onto the stack
			 * @param parent Pointer to the parent node to add
			 */
			VISTA_SDK_CPP_INLINE void push( const GmodNode* parent );

			/**
			 * @brief Pops the last parent node from the stack
			 */
			VISTA_SDK_CPP_INLINE void pop();

			/**
			 * @brief Gets the number of times a node appears in the parent stack
			 * @param node The node to count occurrences for
			 * @return Number of times the node appears in the stack
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline size_t occurrences( const GmodNode& node ) const noexcept;

			/**
			 * @brief Gets the last parent node in the stack
			 * @return Pointer to the last parent node, or nullptr if stack is empty
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const GmodNode* lastOrDefault() const noexcept;

			/**
			 * @brief Gets the parent stack as a vector
			 * @return Reference to the vector containing all parent nodes
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const std::vector<const GmodNode*>& asList() const noexcept;

		private:
			nfx::containers::HashMap<std::string, size_t> m_occurrences;
			std::vector<const GmodNode*> m_parents;
		};

		/**
		 * @brief Traversal context for stateful traversal operations
		 * @tparam TState User-defined state type
		 */
		template <typename TState>
		struct TraversalContext
		{
			/** @brief Reference to the parent stack for tracking traversal hierarchy */
			Parents& parents;

			/** @brief The handler function to call for each traversed node */
			TraverseHandlerWithState<TState> handler;

			/** @brief Reference to user-defined state passed to the handler */
			TState& state;

			/** @brief Maximum number of times a node can be traversed */
			size_t maxTraversalOccurrence;

			/**
			 * @brief Constructor for traversal context
			 * @param p Reference to parent stack
			 * @param h Handler function for processing nodes
			 * @param s Reference to user state
			 * @param maxOcc Maximum traversal occurrence count
			 */
			TraversalContext( Parents& p, TraverseHandlerWithState<TState> h, TState& s, size_t maxOcc )
				: parents{ p },
				  handler{ h },
				  state{ s },
				  maxTraversalOccurrence{ maxOcc }
			{
			}

			/** @brief Copy constructor */
			TraversalContext( const TraversalContext& ) = delete;

			/** @brief Move constructor */
			TraversalContext( TraversalContext&& ) = delete;

			/** @brief Copy assignment operator */
			TraversalContext& operator=( const TraversalContext& ) = delete;

			/** @brief Move assignment operator */
			TraversalContext& operator=( TraversalContext&& ) = delete;
		};

		/**
		 * @brief Core traversal implementation
		 * @details Recursively traverses the GMOD tree starting from the specified node,
		 *          calling the handler function for each visited node while managing
		 *          occurrence tracking and parent stack state.
		 * @tparam TState User-defined state type passed to the handler
		 * @param context Traversal context containing handler, state, and configuration
		 * @param node The current node being traversed
		 * @return TraversalHandlerResult indicating how to continue traversal
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename TState>
		[[nodiscard]] inline TraversalHandlerResult traverseNode( TraversalContext<TState>& context, const GmodNode& node ) const;

		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief The VIS version this GMOD instance conforms to. */
		VisVersion m_visVersion;

		/**
		 * @brief Pointer to the root node ("VE") of the GMOD hierarchy.
		 * @details This pointer refers to a GmodNode instance managed within m_nodeMap.
		 *          It is initialized during Gmod construction.
		 */
		GmodNode* m_rootNode;

		/**
		 * @brief The primary storage for GMOD nodes.
		 * @details This dictionary maps node codes (strings) to GmodNode objects.
		 *          It owns the GmodNode instances.
		 */
		nfx::containers::ChdHashMap<GmodNode> m_nodeMap;
	};
}

#include "Gmod.inl"
