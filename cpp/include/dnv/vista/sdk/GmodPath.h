/**
 * @file GmodPath.h
 * @brief VISTA Generic Product Model (GMOD) Path System for Maritime Hierarchical Navigation
 *
 * @details
 * This file implements the **VISTA GmodPath System** for representing, parsing, and navigating
 * hierarchical paths within maritime vessel structures according to ISO 19848 standards.
 * It provides comprehensive path representation, parsing capabilities, location management,
 * and individualization support for maritime component identification.
 *
 * ## System Purpose:
 *
 * The **VISTA GmodPath System** serves as the foundation for:
 * - **Hierarchical Path Representation**: Complete vessel component paths with value semantics
 * - **Path Parsing and Validation**     : String-to-path conversion with comprehensive error handling
 * - **Location Individualization**      : Component instance identification with physical locations
 * - **Path Navigation and Enumeration** : Efficient traversal of hierarchical structures
 * - **String Representation**           : Multiple output formats (short, full, debug) with pooled builders
 * - **Value Semantics Integration**     : Efficient copying and storage of GmodNode components
 * - **Performance Optimization**        : Balanced approach between copying and shared reference benefits
 *
 * ## Core Architecture:
 *
 * ### Path Classes
 * - **GmodPath**               : Main hierarchical path representation with validation
 * - **GmodIndividualizableSet**: Groups of nodes that can be individualized together
 * - **GmodParsePathResult**    : Result type for parsing operations with error handling
 * - **GmodPath::Enumerator**   : Iterator for efficient path traversal
 *
 * ### Path Components
 * - **Parent Nodes** : Vector of hierarchical parent components using value semantics
 * - **Target Node**  : Final component in the path (optional for empty paths)
 * - **Location Data**: Optional individualization information for specific instances
 * - **VIS Version**  : Version-specific validation and compatibility information
 *
 * ### Architecture Integration
 * - **Value Semantics**       : GmodPath stores GmodNode copies for immutable path representation
 * - **Shared Container Reuse**: Copied GmodNodes retain shared_ptr containers for efficiency
 * - **Memory Balance**        : Path copying is lightweight due to shared relationship containers
 * - **Thread Safety**         : Immutable paths safe for concurrent access and parsing operations
 *
 * ## Data Flow Architecture:
 *
 * ```
 *          Path String Input
 *                  ↓
 *   Parse Validation (Gmod + Locations)
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │            GmodPath                 │
 * ├─────────────────────────────────────┤
 * │      std::size_t m_hashCode         │ ← Cached hash (8 bytes, O(1) access)
 * ├─────────────────────────────────────┤
 * │ ┌─────────────────────────────────┐ │
 * │ │    std::vector<GmodNode>        │ │ ← Parent nodes
 * │ │         m_parents               │ │
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │    std::optional<GmodNode>      │ │ ← Target node
 * │ │          m_node                 │ │
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │        VisVersion               │ │ ← Version compatibility
 * │ │       m_visVersion              │ │
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *                  ↓
 *     Path Operations & Navigation
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │      Path Access & Validation       │
 * ├─────────────────────────────────────┤
 * │ - O(1) indexed node access          │
 * │ - Hierarchical relationship checks  │
 * │ - Location individualization        │
 * │ - Path enumeration & traversal      │
 * │ - String conversion (multiple fmts) │
 * │ - Thread-safe immutable operations  │
 * └─────────────────────────────────────┘
 * ```
 *
 * ## Usage Patterns:
 *
 * ### Basic Path Operations
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Path Parsing with Locations
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Path Enumeration and Navigation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Performance Characteristics:
 *
 * - **Path Construction**   : O(n) where n is path depth (typically 3-8 levels)
 * - **Node Access**         : O(1) random access via operator[] with bounds checking
 * - **Path Validation**     : O(n) hierarchical relationship verification using shared containers
 * - **String Parsing**      : O(m) where m is string length with optimized tokenization
 * - **Memory Usage**        : Value semantics for path storage, leverages GmodNode shared containers
 * - **Copy Operations**     : Efficient GmodNode copying through shared_ptr relationship containers
 * - **String Building**     : Thread-local StringBuilderPool for zero-allocation string operations
 * - **Location Integration**: Optional location data with minimal memory overhead per path
 *
 * ## Thread Safety & Memory Model:
 *
 * - **Read Operations**     : Thread-safe for concurrent access to immutable path data
 * - **Parse Operations**    : Thread-safe with local parsing contexts and no shared state
 * - **String Building**     : Uses thread-local StringBuilderPool for zero-contention performance
 * - **Node Relationship**   : Leverages GmodNode shared containers for safe concurrent access
 * - **Modification**        : Individual paths are immutable after construction
 * - **Copy Semantics**      : Safe copying across threads due to shared_ptr container architecture
 * - **Location Integration**: Thread-safe location lookup and individualization operations
 */

#pragma once

#include <limits>
#include <variant>
#include <vector>

#include <nfx/string/StringBuilder.h>

#include "config/config.h"
#include "GmodNode.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	class Gmod;
	class GmodIndividualizableSet;
	class Location;
	class Locations;

	struct GmodParsePathResult;

	enum class TraversalHandlerResult;
	enum class VisVersion : std::uint16_t;

	//=====================================================================
	// GmodPath class
	//=====================================================================

	/**
	 * @brief Represents a hierarchical path within a vessel structure according to ISO 19848.
	 *
	 * @details
	 * The GmodPath class provides a robust, validated representation of a component path in the VISTA GMOD system.
	 * It supports hierarchical parent/child relationships, path parsing and validation, location individualization,
	 * and efficient traversal and string conversion. Each GmodPath consists of a sequence of parent nodes and a target node,
	 * with optional location data for precise instance identification. The class is immutable after construction and
	 * supports thread-safe read and parse operations. It is the core type for all path-based operations in the VISTA SDK.
	 */
	class GmodPath final
	{
	public:
		class Enumerator;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		// TODO: Consider the CTORS move the parents ??
	private:
		GmodPath( std::vector<GmodNode> parents, GmodNode node, bool skipVerify );

	public:
		/**
		 * @brief Constructs a GmodPath from parent nodes and a target node.
		 * @param parents Vector of parent nodes leading to the target node.
		 * @param node The target node of the path.
		 */
		GmodPath( std::vector<GmodNode> parents, GmodNode node );

		/**
		 * @brief Default constructor.
		 */
		GmodPath();

		/**
		 * @brief Copy constructor
		 * @param other The GmodPath to copy from
		 */
		inline GmodPath( const GmodPath& other );

		/**
		 * @brief Move constructor
		 * @param other The GmodPath to move from
		 */
		inline GmodPath( GmodPath&& other ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/**
		 * @brief Destructor
		 */
		~GmodPath() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The GmodPath to copy from
		 * @return Reference to this GmodPath after assignment
		 */
		inline GmodPath& operator=( const GmodPath& other );

		/**
		 * @brief Move assignment operator
		 * @param other The GmodPath to move from
		 * @return Reference to this GmodPath after assignment
		 */
		inline GmodPath& operator=( GmodPath&& other ) noexcept;

		//----------------------------------------------
		// Equality operators
		//----------------------------------------------

		/**
		 * @brief Compares two GmodPath instances for equality
		 * @param other The other GmodPath to compare with
		 * @return True if paths are equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator==( const GmodPath& other ) const noexcept;

		/**
		 * @brief Compares two GmodPath instances for inequality
		 * @param other The other GmodPath to compare with
		 * @return True if paths are not equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator!=( const GmodPath& other ) const noexcept;

		//----------------------------------------------
		// Lookup operators
		//----------------------------------------------

		/**
		 * @brief Accesses a node in the path by index (const version)
		 * @param index Zero-based index into the path (0 = first parent, length-1 = target node)
		 * @return Const reference to the GmodNode at the specified index
		 * @throws std::out_of_range If index is out of bounds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const GmodNode& operator[]( size_t index ) const;

		/**
		 * @brief Accesses a node in the path by index (non-const version)
		 * @param index Zero-based index into the path (0 = first parent, length-1 = target node)
		 * @return Reference to the GmodNode at the specified index
		 * @throws std::out_of_range If index is out of bounds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline GmodNode& operator[]( size_t index );

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the VIS version associated with these codebooks.
		 * @return The `VisVersion` enum value.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline VisVersion visVersion() const noexcept;

		/**
		 * @brief Gets the target node of this path
		 * @return Reference to the target GmodNode (the last node in the path)
		 * @throws std::runtime_error If the path has no target node
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const GmodNode& node() const;

		/**
		 * @brief Gets the parent nodes in this path
		 * @return Vector of parent GmodNodes leading to the target node
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::vector<GmodNode>& parents() const noexcept;

		/**
		 * @brief Gets the total length of the path (parents + target node)
		 * @return Number of nodes in the complete path
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline size_t length() const noexcept;

		/**
		 * @brief Gets all individualizable sets in this path
		 * @return Vector of individualizable sets that can be modified independently
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::vector<GmodIndividualizableSet> individualizableSets() const;

		/**
		 * @brief Gets the normal assignment name for a node at specified depth
		 * @param nodeDepth The depth of the node to get assignment name for (0-based index)
		 * @return Optional assignment name, nullopt if not found
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::optional<std::string> normalAssignmentName( size_t nodeDepth ) const;

		/**
		 * @brief Gets common names for all function nodes in the path
		 * @return Vector of pairs containing depth and common name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::vector<std::pair<size_t, std::string>> commonNames() const;

		//----------------------------------------------
		// State inspection methods
		//----------------------------------------------

		/**
		 * @brief Validates the hierarchical relationship between parents and target node
		 * @param parents Vector of parent nodes in hierarchical order
		 * @param node The target node to validate
		 * @return True if the path is valid, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool isValid( const std::vector<GmodNode>& parents, const GmodNode& node );

		/**
		 * @brief Checks if this path can be mapped to physical locations
		 * @return True if the target node is mappable, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isMappable() const;

		/**
		 * @brief Checks if this path contains individualizable nodes
		 * @return True if any nodes can be individualized, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isIndividualizable() const;

		//----------------------------------------------
		// String conversion methods
		//----------------------------------------------

		/**
		 * @brief Converts the path to its short string representation
		 * @return Short path string showing only leaf nodes and target
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::string toString() const;

		/**
		 * @brief Writes the path's string representation to a StringBuilder.
		 * @param builder The StringBuilder to write to.
		 * @param separator Character to use between path segments (default: '/').
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		inline void toString( nfx::string::StringBuilder& builder, char separator = '/' ) const;

		/**
		 * @brief Converts the path to full hierarchical string representation
		 * @return Complete path string from root to target
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toFullPathString() const;

		/**
		 * @brief Writes the path's full hierarchical string representation to a StringBuilder.
		 * @param builder The StringBuilder to write to.
		 */
		inline void toFullPathString( nfx::string::StringBuilder& builder ) const;

		/**
		 * @brief Creates detailed debug representation of the path
		 * @return Detailed string with codes, names, and assignment names
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::string toStringDump() const;

		/**
		 * @brief Writes the path's detailed debug representation to a StringBuilder.
		 * @param builder The StringBuilder to write to.
		 */
		inline void toStringDump( nfx::string::StringBuilder& builder ) const;

		//----------------------------------------------
		// Hashing
		//----------------------------------------------

		/**
		 * @brief Gets the cached hash value for this GmodPath instance
		 * @details **Purpose**: Enables GmodPath instances to be used efficiently as keys in hash-based
		 *          collections (std::unordered_map, nfx::HashMap, etc.) without performance penalties.
		 *
		 *          **Performance Critical**: Hash is pre-computed during construction using optimized
		 *          NFX FNV-1a algorithms and cached to avoid expensive recomputation. This transforms
		 *          hash lookups from O(n) path traversal to O(1) cached access.
		 *
		 *          **Implementation**: Combines hash codes of all parent nodes and the target node
		 *          using NFX hash combination algorithms for consistent, high-quality distribution.
		 *
		 * @return The pre-computed hash value for this GmodPath's complete hierarchical structure
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::size_t hashCode() const noexcept;

		//----------------------------------------------
		// Path manipulation methods
		//----------------------------------------------

		/**
		 * @brief Creates a copy of this path with all location information removed
		 * @return New GmodPath instance without any location data
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] GmodPath withoutLocations() const;

		//----------------------------------------------
		// Path enumeration methods
		//----------------------------------------------

		/**
		 * @brief Gets the full hierarchical path with all intermediate nodes
		 * @return Enumerator for the complete path from root to target
		 */
		Enumerator inline fullPath() const;

		/**
		 * @brief Gets the full hierarchical path starting from a specific depth
		 * @param fromDepth The depth to start enumeration from (0 = root, -1 = from beginning)
		 * @return Enumerator starting from the specified depth
		 */
		Enumerator inline fullPathFrom( size_t fromDepth ) const;

		//----------------------------------------------
		// Parsing methods
		//----------------------------------------------

		/**
		 * @brief Parses a path string using specified VIS version
		 * @param item The path string to parse
		 * @param visVersion The VIS version to use for parsing
		 * @return Parsed GmodPath instance
		 * @throws std::invalid_argument if parsing fails
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static GmodPath parse( std::string_view item, VisVersion visVersion );

		/**
		 * @brief Parses a path string using specified GMOD and locations
		 * @param item The path string to parse
		 * @param gmod The GMOD instance to use for node lookup
		 * @param locations The locations instance for location parsing
		 * @return Parsed GmodPath instance
		 * @throws std::invalid_argument if parsing fails
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static GmodPath parse( std::string_view item, const Gmod& gmod, const Locations& locations );

		/**
		 * @brief Parses a full path string using specified VIS version
		 * @param item The full path string to parse (must start from root)
		 * @param visVersion The VIS version to use for parsing
		 * @return Parsed GmodPath instance
		 * @throws std::invalid_argument if parsing fails
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static GmodPath parseFullPath( std::string_view item, VisVersion visVersion );

		/**
		 * @brief Attempts to parse a path string, returning success/failure
		 * @param item The path string to parse
		 * @param visVersion The VIS version to use for parsing
		 * @param outPath [out] The parsed path if successful
		 * @return True if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static bool tryParse(
			std::string_view item, VisVersion visVersion,
			std::optional<GmodPath>& outPath );

		/**
		 * @brief Attempts to parse a path string using GMOD and locations
		 * @param item The path string to parse
		 * @param gmod The GMOD instance to use for node lookup
		 * @param locations The locations instance for location parsing
		 * @param outPath [out] The parsed path if successful
		 * @return True if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool tryParse(
			std::string_view item, const Gmod& gmod, const Locations& locations, std::optional<GmodPath>& outPath );

		/**
		 * @brief Attempts to parse a full path string using VIS version
		 * @param item The full path string to parse (must start from root)
		 * @param visVersion The VIS version to use for parsing
		 * @param outPath [out] The parsed path if successful
		 * @return True if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static bool tryParseFullPath(
			std::string_view item,
			VisVersion visVersion, std::optional<GmodPath>& outPath );

		/**
		 * @brief Attempts to parse a full path string using GMOD and locations
		 * @param item The full path string to parse (must start from root)
		 * @param gmod The GMOD instance to use for node lookup
		 * @param locations The locations instance for location parsing
		 * @param outPath [out] The parsed path if successful
		 * @return True if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool tryParseFullPath(
			std::string_view item, const Gmod& gmod, const Locations& locations, std::optional<GmodPath>& outPath );

		//----------------------------------------------
		// Enumeration
		//----------------------------------------------

		/**
		 * @brief Creates an enumerator for traversing nodes in this path
		 * @param fromDepth The depth to start enumeration from (default: from target node backwards)
		 * @return Enumerator instance for iterating through path nodes
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Enumerator enumerator( size_t fromDepth = std::numeric_limits<size_t>::max() ) const;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Cached hash value computed during construction for O(1) hash access */
		std::size_t m_hashCode;

		/** @brief VIS version associated with this path for compatibility and validation */
		VisVersion m_visVersion;

		/** @brief Vector of parent nodes in hierarchical order leading to the target node */
		std::vector<GmodNode> m_parents;

		/** @brief Optional target node of the path (nullopt for empty paths) */
		std::optional<GmodNode> m_node;

	public:
		//----------------------------------------------
		// GmodPath::enumerator
		//----------------------------------------------

		/**
		 * @brief Iterator for traversing nodes in a GmodPath.
		 *
		 * @details
		 * The Enumerator class provides efficient, index-based iteration over the nodes of a GmodPath,
		 * supporting both forward and partial traversal from any specified depth. It exposes the depth and
		 * node pointer for each element, and is used for path navigation, enumeration, and string conversion
		 * operations. Enumerator instances are lightweight and non-owning, referencing the underlying GmodPath.
		 */
		class Enumerator final
		{
			friend class GmodPath;

		public:
			//----------------------------
			// Types definitions
			//----------------------------

			/** @brief Represents a single path element entry. */
			using PathElement = std::pair<size_t, const GmodNode*>;

		private:
			//----------------------------
			// Construction
			//----------------------------

			/**
			 * @brief Internal constructor for creating Enumerator.
			 * @param pathInst Pointer to the GmodPath to enumerate.
			 * @param startIndex Starting index for enumeration.
			 */
			Enumerator( const GmodPath* pathInst, size_t startIndex = std::numeric_limits<size_t>::max() );

		public:
			/** @brief Default constructor */
			Enumerator() = delete;

			/** @brief Copy constructor */
			Enumerator( const Enumerator& ) = default;

			/** @brief Move constructor */
			Enumerator( Enumerator&& ) noexcept = default;

			//----------------------------
			// Destruction
			//----------------------------

			/** @brief Destructor */
			~Enumerator() = default;

			//----------------------------
			// Assignment operators
			//----------------------------

			/**
			 * @brief Copy assignment operator
			 * @return Reference to this Enumerator after assignment
			 */
			Enumerator& operator=( const Enumerator& ) = default;

			/**
			 * @brief Move assignment operator
			 * @return Reference to this Enumerator after assignment
			 */
			Enumerator& operator=( Enumerator&& ) noexcept = default;

			//----------------------------
			// Enumeration methods
			//----------------------------

			/**
			 * @brief Advances the enumerator to the next element.
			 * @return True if the enumerator successfully moved to the next element;
			 *         false if the enumerator has passed the end of the collection.
			 */
			inline bool next();

			/**
			 * @brief Gets the current element.
			 * @return The current path element (depth, node).
			 */
			inline const PathElement& current() const;

			/**
			 * @brief Resets the enumerator to its initial position before first element
			 */
			inline void reset();

		private:
			//----------------------------
			// Private member variables
			//----------------------------

			const GmodPath* m_pathInstance;
			size_t m_currentIndex;
			mutable PathElement m_current;
		};
	};

	//=====================================================================
	// GmodIndividualizableSet class
	//=====================================================================

	/**
	 * @brief Represents a set of nodes within a GmodPath that can be individualized together.
	 *
	 * @details
	 * The GmodIndividualizableSet class encapsulates a group of nodes in a GmodPath that can be
	 * assigned unique location information, enabling precise identification of component instances
	 * within a hierarchical maritime structure. This is essential for supporting the ISO 19848
	 * standard's requirements for component individualization and location mapping.
	 *
	 * Typical usage involves retrieving individualizable sets from a GmodPath, assigning or modifying
	 * their location data, and rebuilding the path to reflect these changes. The class provides access
	 * to the indices and pointers of the nodes that can be individualized, as well as utility methods
	 * for string conversion and location management.
	 */
	class GmodIndividualizableSet final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs a GmodIndividualizableSet with specified node indices and source path.
		 * @param nodeIndices Indices of nodes in the path that can be individualized.
		 * @param sourcePath The source GmodPath from which this set is derived.
		 */
		GmodIndividualizableSet( const std::vector<int>& nodeIndices, const GmodPath& sourcePath );

		/**
		 * @brief Default constructor
		 */
		GmodIndividualizableSet() = delete;
		/**
		 * @brief Copy constructor
		 */
		GmodIndividualizableSet( const GmodIndividualizableSet& ) = delete;
		/**
		 * @brief Move constructor
		 * @param other The GmodIndividualizableSet to move from
		 */
		GmodIndividualizableSet( GmodIndividualizableSet&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/**
		 * @brief Destructor
		 */
		~GmodIndividualizableSet() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 */
		GmodIndividualizableSet& operator=( const GmodIndividualizableSet& ) = delete;
		/**
		 * @brief Move assignment operator
		 * @param other The GmodIndividualizableSet to move from
		 * @return Reference to this object
		 */
		GmodIndividualizableSet& operator=( GmodIndividualizableSet&& other ) noexcept = default;

		//----------------------------------------------
		// Build
		//----------------------------------------------

		/**
		 * @brief Builds a new GmodPath with the current individualization settings applied.
		 * @return A new GmodPath instance reflecting the individualized nodes and locations.
		 */
		GmodPath build();

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Gets the nodes that can be individualized in this set
		 * @return Vector of pointers to GmodNodes that can be modified independently
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::vector<GmodNode*> nodes() const;

		/**
		 * @brief Gets the indices of nodes in this individualizable set
		 * @return Vector of indices identifying which nodes can be individualized
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::vector<int>& nodeIndices() const noexcept;

		/**
		 * @brief Gets the location associated with this individualizable set
		 * @return Optional location, or nullopt if no location is set
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::optional<Location> location() const;

		//----------------------------------------------
		// Utility methods
		//----------------------------------------------

		/**
		 * @brief Sets the location for this individualizable set
		 * @param location The location to associate with this set, or nullopt to clear
		 */
		void setLocation( const std::optional<Location>& location );

		/**
		 * @brief Converts this individualizable set to its string representation
		 * @return String representation showing the set's nodes and location
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		std::vector<int> m_nodeIndices;
		GmodPath m_path;
		bool m_isBuilt;
	};

	//=====================================================================
	// GmodParsePathResult
	//=====================================================================

	/**
	 * @brief Result type for GMOD path parsing operations with error handling.
	 *
	 * @details
	 * The GmodParsePathResult struct encapsulates the outcome of parsing a GMOD path string.
	 * It provides a discriminated union (variant) that holds either a successfully parsed GmodPath
	 * (via the Ok struct) or an error message (via the Error struct). This enables robust error
	 * handling and clear separation of success and failure cases when parsing hierarchical paths
	 * according to the ISO 19848 standard.
	 *
	 * Typical usage involves checking the result with isOk() or isError(), then accessing the parsed
	 * path or error message as appropriate. This struct is used by all GMOD path parsing functions
	 * to provide safe and expressive error reporting.
	 */
	struct GmodParsePathResult
	{
		/**
		 * @brief Represents a successful GMOD path parsing result.
		 *
		 * @details
		 * The Ok struct holds a successfully parsed GmodPath instance. It is used as the
		 * success variant in GmodParsePathResult, enabling access to the parsed path when
		 * parsing operations complete without error.
		 */
		struct Ok
		{
			/**
			 * @brief The parsed GMOD path.
			 */
			GmodPath path;

			/**
			 * @brief Constructs an Ok result with the given path.
			 * @param p The successfully parsed GmodPath.
			 */
			inline explicit Ok( GmodPath p );
		};

		/**
		 * @brief Represents a parsing error in GmodParsePathResult.
		 *
		 * @details
		 * The Error struct holds an error message describing why a GMOD path parsing operation failed.
		 * It is used as the error variant in GmodParsePathResult, enabling robust error reporting and
		 * diagnostics for invalid or malformed path strings.
		 */
		struct Error
		{
			/**
			 * @brief Error message describing the parsing failure.
			 */
			std::string error;

			/**
			 * @brief Constructs an Error with the given message.
			 * @param e The error message string.
			 */
			inline explicit Error( std::string e );
		};

		/**
		 * @brief Holds either a successful result (Ok) or an error (Error).
		 */
		std::variant<Ok, Error> result;

		/**
		 * @brief Constructs a GmodParsePathResult with a successful result.
		 * @param ok The Ok result containing the parsed path.
		 */
		inline GmodParsePathResult( Ok ok );

		/**
		 * @brief Constructs a GmodParsePathResult with an error result.
		 * @param err The Error result containing the error message.
		 */
		inline GmodParsePathResult( Error err );

		/**
		 * @brief Checks if the result is a successful parse.
		 * @return True if the result is Ok, false if Error.
		 */
		inline bool isOk() const noexcept;

		/**
		 * @brief Checks if the result is an error.
		 * @return True if the result is Error, false if Ok.
		 */
		inline bool isError() const noexcept;

		/**
		 * @brief Returns a reference to the Ok result.
		 * @return Reference to the Ok struct.
		 * @throws std::bad_variant_access if the result is not Ok.
		 */
		inline Ok& ok();

		/**
		 * @brief Returns a const reference to the Ok result.
		 * @return Const reference to the Ok struct.
		 * @throws std::bad_variant_access if the result is not Ok.
		 */
		inline const Ok& ok() const;

		/**
		 * @brief Returns a reference to the Error result.
		 * @return Reference to the Error struct.
		 * @throws std::bad_variant_access if the result is not Error.
		 */
		inline Error& error();

		/**
		 * @brief Returns a const reference to the Error result.
		 * @return Const reference to the Error struct.
		 * @throws std::bad_variant_access if the result is not Error.
		 */
		inline const Error& error() const;
	};
} // namespace dnv::vista::sdk

#include "detail/GmodPath.inl"

namespace std
{
	/**
	 * @brief Hash specialization for dnv::vista::sdk::GmodPath
	 * @details Enables GmodPath instances to be used as keys in all hash-based STL containers.
	 *          This specialization provides seamless integration with std::unordered_map,
	 *          std::unordered_set, and other standard library hash containers.
	 */
	template <>
	struct hash<dnv::vista::sdk::GmodPath>
	{
		/**
		 * @brief Returns the cached hash value for optimal performance
		 * @param[in] path The GmodPath instance to hash
		 * @return Pre-computed hash value (O(1) access) combining all nodes in the hierarchical path
		 */
		std::size_t operator()( const dnv::vista::sdk::GmodPath& path ) const noexcept
		{
			return path.hashCode();
		}
	};
} // namespace std
