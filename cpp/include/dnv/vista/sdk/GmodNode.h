/**
 * @file GmodNode.h
 * @brief VISTA Generic Product Model (GMOD) Node and Metadata System for Maritime Component Management
 *
 * @details
 * This file implements the **VISTA GmodNode System** for representing and managing
 * individual components within maritime vessel hierarchical structures. It provides
 * comprehensive node representation, relationship management, location assignment,
 * and type classification capabilities according to ISO 19848 maritime standards.
 *
 * ## System Purpose:
 *
 * The **VISTA GmodNode System** serves as the foundation for:
 * - **Component Representation**: Individual vessel components with metadata and shared relationships
 * - **Hierarchical Navigation** : Parent-child relationship management via shared container architecture
 * - **Location Assignment**     : Individualization of components with copy-efficient shared references
 * - **Type Classification**     : Product types, selections, assets, and function categorization
 * - **Metadata Management**     : Rich descriptive information for each component
 * - **Performance Optimization**: Copy-efficient shared containers and optimized memory management
 *
 * ## Core Architecture:
 *
 * ### Node Classes
 * - **GmodNode**        : Main component representation with relationships and location
 * - **GmodNodeMetadata**: Descriptive information (category, type, name, definitions)
 * - **Location**        : Optional individualization data for component instances
 * - **VisVersion**      : Version-specific validation and compatibility
 *
 * ### Relationship Framework
 * - **Shared Container Links**   : Hierarchical relationships via shared_ptr containers
 * - **Copy-Efficient References**: Lightweight node copying through shared relationship storage
 * - **Product Type Assignment**  : Equipment type classification with shared reference efficiency
 * - **Product Selection**        : Specific equipment choice relationships with minimal memory overhead
 * - **Function Assignment**      : Functional role and composition relationships via shared containers
 *
 * ## Data Flow Architecture:
 *
 * ```
 *      GmodNodeDto (External Data)
 *                  ↓
 * GmodNode Construction (Gmod/VIS Factory)
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │            GmodNode                 │
 * ├─────────────────────────────────────┤
 * │      std::size_t m_hashCode         │ ← Cached hash (8 bytes, O(1) access)
 * ├─────────────────────────────────────┤
 * │ ┌─────────────────────────────────┐ │
 * │ │      GmodNodeMetadata           │ │ ← Descriptive information
 * │ │   (category, type, name, etc)   │ │
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │ std::shared_ptr<std::vector<    │ │ ← Shared parent relationships
 * │ │        GmodNode*>> (m_parents)  │ │   (Copy-efficient references)
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │ std::shared_ptr<std::vector<    │ │ ← Shared child relationships
 * │ │       GmodNode*>> (m_children)  │ │   (Copy-efficient references)
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │ std::shared_ptr<StringSet>      │ │ ← Shared O(1) child code lookup
 * │ │         (m_childrenSet)         │ │   (Copy-efficient caching)
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │    std::optional<Location>      │ │ ← Individual instance data
 * │ │         (m_location)            │ │
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *                  ↓
 *  Shared Reference & Location Assignment
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │     Shared Reference Benefits       │
 * ├─────────────────────────────────────┤
 * │ - Efficient node copying            │
 * │ - Reduced memory overhead           │
 * │ - Copy-on-write semantics           │
 * │ - Safe concurrent access            │
 * │ - Automatic lifetime management     │
 * └─────────────────────────────────────┘
 * ```
 *
 * ## Node Classification System:
 *
 * ### Component Types
 * - **Vessel Entry (VE)**    : Root node representing the complete vessel
 * - **System Components**    : Major ship systems (propulsion, navigation, etc.)
 * - **Equipment Items**      : Individual machinery and equipment pieces
 * - **Product Types**        : Standardized equipment classifications
 * - **Product Selections**   : Specific equipment choices and configurations
 * - **Function Compositions**: Functional groupings and relationships
 * - **Asset Components**     : Physical assets with lifecycle management
 *
 * ### Metadata Categories
 * - **Category Classification**: "PRODUCT", "ASSET", "FUNCTION", etc.
 * - **Type Specification**     : "TYPE", "SELECTION", "COMPOSITION", etc.
 * - **Naming Information**     : Human-readable names and common aliases
 * - **Definition Data**        : Detailed and common definitions
 * - **Assignment Mapping**     : Normal assignment name relationships
 * - **Installation Flags**     : Substructure installation requirements
 *
 * ## Location and Individualization:
 *
 * ### Location Assignment Patterns
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Individualization Rules
 * - **Location-Aware Equality**: Nodes with same code but different locations are distinct
 * - **Individualization Logic**: Complex rules based on node type and context
 * - **Function Composition**   : Special handling for function composition nodes
 * - **Group and Selection**    : Non-individualizable node types
 *
 * ## Performance Characteristics:
 *
 * - **O(1) Child Lookups**     : Shared StringSet-based child code verification
 * - **Copy-Efficient Design**  : shared_ptr containers enable lightweight node copying
 * - **Memory Shared Storage**  : Multiple nodes share relationship containers reducing overhead
 * - **Zero-Copy Access**       : string_view interfaces for metadata access
 * - **Thread Safe Reads**      : Shared immutable containers safe for concurrent navigation
 * - **Relationship Caching**   : Shared parent-child relationship queries with minimal duplication
 * - **Location Optimization**  : Optional location data minimizes per-instance memory overhead
 * - **Copy-on-Write Semantics**: Automatic container sharing until modification required
 *
 * ## Relationship Management:
 *
 * ### Parent-Child Navigation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Type Classification Queries
 * - **Node Type Checking**  : isProductType(), isAsset(), isFunctionNode()
 * - **Hierarchy Validation**: isRoot(), isLeafNode(), isChild()
 * - **Mapping Capabilities**: isMappable(), isIndividualizable()
 * - **Function Analysis**   : isFunctionComposition(), isAssetFunctionNode()
 *
 * ## String Representation:
 *
 * ### ToString Patterns
 * - **Code Only**        :
 * - **Code-Location**    :
 * - **StringBuilder**    :
 * - **Pool Optimization**:
 *
 * ## Design Philosophy:
 *
 * - **Standards Compliance**  : Full adherence to ISO 19848 maritime equipment standards
 * - **Performance Focus**     : Optimized for high-frequency navigation with shared container efficiency
 * - **Copy-Efficient Design** : shared_ptr containers enable lightweight node duplication
 * - **Type Safety**           : Strong typing with comprehensive error handling
 * - **Immutable Core**        : Thread-safe design with shared immutable containers
 * - **Memory Efficiency**     : Shared relationship storage minimizes memory overhead
 * - **Relationship Integrity**: Consistent parent-child management via shared container architecture
 * - **STL Compatibility**     : Public constructors for container compatibility with shared semantics
 *
 * ## Memory Management:
 *
 * ### Ownership Model
 * - **Container Ownership**     : Gmod owns all GmodNode instances
 * - **Shared Container Model**  : Relationship containers use shared_ptr for efficient sharing
 * - **Non-Owning Pointers**     : Parent-child links use raw pointers (nodes owned by Gmod)
 * - **Copy-Efficient Semantics**: Shared containers enable lightweight node copying for individualization
 * - **Memory Optimization**     : shrink_to_fit() on shared containers and efficient storage usage
 *
 * ### Lifecycle Management
 * - **Construction**         : DTO-based factory construction through Gmod
 * - **Relationship Building**: Post-construction shared container establishment
 * - **Trimming**             : Memory optimization with shrink_to_fit() on shared containers
 * - **Copy Operations**      : Efficient copying via shared container references for individualization
 * - **Container Sharing**    : Automatic shared_ptr reference counting for relationship containers
 */

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <nfx/Containers.h>
#include <nfx/string/StringBuilder.h>

#include "Locations.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	enum class VisVersion : std::uint16_t;

	class ParsingErrors;
	class Gmod;
	struct GmodNodeDto;

	//=====================================================================
	// GmodNodeMetadata class
	//=====================================================================

	/**
	 * @brief Metadata for a GMOD (Generic Product Model) node.
	 * @details Contains descriptive information about a node including its category, type,
	 *          name, and other optional properties as defined in ISO 19848.
	 *          This class is designed to be immutable after construction.
	 */
	class GmodNodeMetadata final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs a new GmodNodeMetadata object.
		 * @param category The category classification of the node (e.g., "PRODUCT", "ASSET").
		 * @param type The type classification within the category (e.g., "SELECTION", "TYPE").
		 * @param name The human-readable name of the node.
		 * @param commonName Optional common name or alias for the node. Defaults to std::nullopt.
		 * @param definition Optional detailed definition of the node. Defaults to std::nullopt.
		 * @param commonDefinition Optional common definition for the node. Defaults to std::nullopt.
		 * @param installSubstructure Optional flag indicating if substructure should be installed. Defaults to std::nullopt.
		 * @param normalAssignmentNames Optional mapping of normal assignment names. Defaults to an empty map.
		 */
		inline GmodNodeMetadata(
			std::string_view category,
			std::string_view type, std::string_view name,
			const std::optional<std::string>& commonName = std::nullopt,
			const std::optional<std::string>& definition = std::nullopt,
			const std::optional<std::string>& commonDefinition = std::nullopt,
			const std::optional<bool>& installSubstructure = std::nullopt,
			const nfx::containers::FastHashMap<std::string, std::string>& normalAssignmentNames = {} ) noexcept;

		/** @brief Default constructor. */
		GmodNodeMetadata() = default;

		/**
		 * @brief Copy constructor
		 * @param other The object to copy from
		 */
		inline GmodNodeMetadata( const GmodNodeMetadata& other ) noexcept;

		/**
		 * @brief Move constructor
		 * @param other The object to move from
		 */
		GmodNodeMetadata( GmodNodeMetadata&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~GmodNodeMetadata() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The object to copy from
		 * @return Reference to this GmodNodeMetadata after assignment
		 */
		inline GmodNodeMetadata& operator=( const GmodNodeMetadata& other ) noexcept;

		/**
		 * @brief Move assignment operator
		 * @param other The object to move from
		 * @return Reference to this GmodNodeMetadata after assignment
		 */
		GmodNodeMetadata& operator=( GmodNodeMetadata&& other ) noexcept = default;

		//----------------------------------------------
		// Operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison operator.
		 * @param other The metadata object to compare with.
		 * @return True if all properties of this metadata object are equal to the other, false otherwise.
		 */
		inline bool operator==( const GmodNodeMetadata& other ) const;

		/**
		 * @brief Inequality comparison operator.
		 * @param other The metadata object to compare with.
		 * @return True if any property of this metadata object differs from the other, false otherwise.
		 */
		inline bool operator!=( const GmodNodeMetadata& other ) const;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the category of the node
		 * @return Reference to the category string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::string_view category() const noexcept;

		/**
		 * @brief Get the type of the node
		 * @return Reference to the type string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::string_view type() const noexcept;

		/**
		 * @brief Get the full type string (category + type)
		 * @return Reference to the full type string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::string_view fullType() const noexcept;

		/**
		 * @brief Get the name of the node
		 * @return Reference to the name string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::string_view name() const noexcept;

		/**
		 * @brief Get the common name of the node
		 * @return Optional reference to the common name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<std::string>& commonName() const noexcept;

		/**
		 * @brief Get the definition of the node
		 * @return Optional reference to the definition
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<std::string>& definition() const noexcept;

		/**
		 * @brief Get the common definition of the node
		 * @return Optional reference to the common definition
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<std::string>& commonDefinition() const noexcept;

		/**
		 * @brief Get the install substructure flag
		 * @return Optional reference to the install substructure flag
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<bool>& installSubstructure() const noexcept;

		/**
		 * @brief Get the normal assignment names mapping
		 * @return Reference to the map of normal assignment names
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const nfx::containers::FastHashMap<std::string, std::string>& normalAssignmentNames() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Category classification of the node. */
		std::string m_category;

		/** @brief Type classification within the category. */
		std::string m_type;

		/** @brief Human-readable name of the node. */
		std::string m_name;

		/** @brief Optional common name or alias for the node. */
		std::optional<std::string> m_commonName;

		/** @brief Optional detailed definition of the node. */
		std::optional<std::string> m_definition;

		/** @brief Optional common definition for the node. */
		std::optional<std::string> m_commonDefinition;

		/** @brief Optional flag indicating if substructure should be installed. */
		std::optional<bool> m_installSubstructure;

		/** @brief Optional mapping of normal assignment names. */
		nfx::containers::FastHashMap<std::string, std::string> m_normalAssignmentNames;

		/** @brief Combined category and type string, e.g., "PRODUCT TYPE", generated at construction. */
		std::string m_fullType;
	};

	//=====================================================================
	// GmodNode class
	//=====================================================================

	/**
	 * @brief A node in the Generic Product Model (GMOD).
	 * @details Represents a single node in the hierarchical structure of the Generic Product Model
	 *          as defined by ISO 19848. Contains metadata, relationships to parent/child
	 *          nodes, and optional location information.
	 */
	class GmodNode final
	{
	public:
		friend class Gmod; // TODO: Remove later

	private:
		//----------------------------------------------
		//  Construction
		//----------------------------------------------

		/**
		 * @brief Constructs a GmodNode from a Data Transfer Object (DTO).
		 * @param version The VIS version associated with this node.
		 * @param dto The DTO containing the node's data.
		 */
		GmodNode( VisVersion version, const GmodNodeDto& dto ) noexcept;

	public:
		/** @brief Default constructor. */
		GmodNode() = default;

		/**
		 * @brief Copy constructor
		 * @param other The object to copy from
		 */
		inline GmodNode( const GmodNode& other ) noexcept;

		/**
		 * @brief Move constructor
		 * @param other The object to move from
		 */
		inline GmodNode( GmodNode&& other ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~GmodNode() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The object to copy from
		 * @return Reference to this GmodNode after assignment
		 */
		inline GmodNode& operator=( const GmodNode& other ) noexcept;

		/**
		 * @brief Move assignment operator
		 * @param other The object to move from
		 * @return Reference to this GmodNode after assignment
		 */
		inline GmodNode& operator=( GmodNode&& other ) noexcept;

		//----------------------------------------------
		// Operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison operator.
		 * @details Nodes are considered equal if they have the same code and location.
		 * @param other The node to compare with.
		 * @return True if this node is equal to the other node, false otherwise.
		 */
		inline bool operator==( const GmodNode& other ) const;

		/**
		 * @brief Inequality comparison operator.
		 * @param other The node to compare with.
		 * @return True if this node is not equal to the other node, false otherwise.
		 */
		inline bool operator!=( const GmodNode& other ) const;

		/**
		 * @brief Explicit equality comparison method.
		 * @details Nodes are considered equal if they have the same code and location.
		 *          This method provides an explicit way to check equality, complementing `operator==`.
		 * @param other The node to compare with.
		 * @return True if this node is equal to the other node, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool equals( const GmodNode& other ) const;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the unique code identifier
		 * @return Reference to the code string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::string& code() const noexcept;

		/**
		 * @brief Get the location information
		 * @return Optional reference to the location
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<Location>& location() const noexcept;

		/**
		 * @brief Get the VIS version
		 * @return The VIS version enum value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline VisVersion visVersion() const noexcept;

		/**
		 * @brief Get the node metadata
		 * @return Reference to the metadata
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const GmodNodeMetadata& metadata() const noexcept;

		//----------------------------------------------
		// Relationship accessors
		//----------------------------------------------

		/**
		 * @brief Get the child nodes
		 * @return Reference to the vector of child node pointers
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::vector<GmodNode*>& children() const noexcept;

		/**
		 * @brief Get the parent nodes
		 * @return Reference to the vector of parent node pointers
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::vector<GmodNode*>& parents() const noexcept;

		/**
		 * @brief Gets the product type node associated with this function node, if applicable.
		 * @details For a function node, this typically refers to its single child node if that child is a product type.
		 * @return Optional GmodNode value, or std::nullopt if not applicable or not found.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::optional<GmodNode> productType() const noexcept;

		/**
		 * @brief Gets the product selection node associated with this function node, if applicable.
		 * @details For a function node, this typically refers to its single child node if that child is a product selection.
		 * @return Optional GmodNode value, or std::nullopt if not applicable or not found.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::optional<GmodNode> productSelection() const noexcept;

		//----------------------------------------------
		// Node location methods
		//----------------------------------------------

		/**
		 * @brief Creates a new GmodNode instance identical to this one but without location information.
		 * @return A new `GmodNode` instance with `location` set to `std::nullopt`.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] GmodNode withoutLocation() const;

		/**
		 * @brief Creates a new GmodNode instance identical to this one but with the specified Location object.
		 * @param location The Location object to set for the new node.
		 * @return A new `GmodNode` instance with the given location.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] GmodNode withLocation( const Location& location ) const;

		/**
		 * @brief Creates a new GmodNode instance identical to this one but with the specified location string.
		 * @param locationStr The location string to parse and set.
		 * @return A new `GmodNode` instance with the parsed location.
		 * @throws std::invalid_argument If the `locationStr` is invalid or cannot be parsed.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] GmodNode withLocation( std::string_view locationStr ) const;

		/**
		 * @brief Tries to create a new GmodNode instance with the specified location string, returning a copy on failure.
		 * @details This method attempts to parse the `locationStr`. If successful, a new node with the
		 *          parsed location is returned. If parsing fails, a new node identical to the original
		 *          (without the new location) is returned. No exceptions are thrown for parsing errors.
		 * @param locationStr The location string to parse and set.
		 * @return A new `GmodNode` instance, with the location set if parsing was successful.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] GmodNode tryWithLocation( std::string_view locationStr ) const;

		/**
		 * @brief Tries to create a new GmodNode instance with the specified location string, capturing parsing errors.
		 * @details Attempts to parse `locationStr`. If successful, a new node with the parsed location is returned.
		 *          If parsing fails, errors are added to the `errors` object, and a new node identical
		 *          to the original is returned.
		 * @param locationStr The location string to parse and set.
		 * @param errors A `ParsingErrors` object to collect detailed error information if parsing fails.
		 * @return A new `GmodNode` instance, with the location set if parsing was successful.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] GmodNode tryWithLocation( std::string_view locationStr, ParsingErrors& errors ) const;

		/**
		 * @brief Tries to create a new GmodNode instance with the specified optional Location object.
		 * @details If `location` has a value, a new node with that location is returned.
		 *          Otherwise, a new node identical to the original is returned.
		 * @param location An `std::optional<Location>` to set.
		 * @return A new `GmodNode` instance, with the location set if provided.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] GmodNode tryWithLocation( const std::optional<Location>& location ) const;

		//----------------------------------------------
		// Node type checking methods
		//----------------------------------------------

		/**
		 * @brief Checks if the node is individualizable.
		 * @details A node is individualizable if it can be assigned a unique instance identifier
		 *          based on its GMOD classification and context.
		 * @param isTargetNode Flag indicating if this node is the target of an operation (e.g., in mapping). Defaults to false.
		 * @param isInSet Flag indicating if this node is part of a set. Defaults to false.
		 * @return True if the node is individualizable under the given conditions, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isIndividualizable( bool isTargetNode = false, bool isInSet = false ) const noexcept;

		/**
		 * @brief Checks if the node represents a function composition.
		 * @details A function composition node typically groups other function or product nodes.
		 * @return True if the node's metadata indicates it's an asset or product function composition, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isFunctionComposition() const noexcept;

		/**
		 * @brief Checks if the node is mappable.
		 * @details A mappable node is one that can be related to other nodes in a specific context,
		 *          often used in creating Local IDs or other semantic links.
		 * @return True if the node is considered mappable based on its type and properties, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isMappable() const noexcept;

		/**
		 * @brief Checks if the node is a product selection.
		 * @return True if the node's metadata identifies it as a product selection, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isProductSelection() const;

		/**
		 * @brief Checks if the node is a product type.
		 * @return True if the node's metadata identifies it as a product type, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isProductType() const;

		/**
		 * @brief Checks if the node is an asset.
		 * @return True if the node's metadata identifies it as an asset, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isAsset() const;

		/**
		 * @brief Checks if the node is a leaf node in the GMOD hierarchy (has no children).
		 * @return True if the node has no children, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isLeafNode() const;

		/**
		 * @brief Checks if the node is a function node (asset function or product function).
		 * @return True if the node's metadata identifies it as any kind of function node, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isFunctionNode() const;

		/**
		 * @brief Checks if the node is an asset function node.
		 * @return True if the node's metadata identifies it as an asset function, false otherwise.
		 */
		[[nodiscard]] bool isAssetFunctionNode() const;

		/**
		 * @brief Checks if the node is the root node of the GMOD hierarchy.
		 * @details The root node is conventionally identified by the code "VE".
		 * @return True if the node's code is "VE", false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isRoot() const noexcept;

		//----------------------------------------------
		// Node relationship query methods
		//----------------------------------------------

		/**
		 * @brief Checks if this node has a specific GmodNode instance as a direct child.
		 * @param node The potential child node to check for.
		 * @return True if the provided `node` is a direct child of this node (checked by code), false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isChild( const GmodNode& node ) const noexcept;

		/**
		 * @brief Checks if this node has a direct child with a specific code.
		 * @details This check is efficient due to an internal set of child codes (`m_childrenSet`).
		 * @param code The code of the potential child node.
		 * @return True if a direct child with the specified `code` exists, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isChild( std::string_view code ) const noexcept;

		//----------------------------------------------
		// String conversion methods
		//----------------------------------------------

		/**
		 * @brief Converts the node to its string representation.
		 * @details The format is typically "<code>" or "<code>-<location>" if location is present.
		 * @return A `std::string` representing the node.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::string toString() const noexcept;

		/**
		 * @brief Appends the node's string representation to a StringBuilder.
		 * @param builder The StringBuilder to write to.
		 */
		inline void toString( nfx::string::StringBuilder& builder ) const noexcept;

		//----------------------------------------------
		// Hashing
		//----------------------------------------------

		/**
		 * @brief Gets the cached hash value for this GmodNode instance
		 * @details **Purpose**: Enables GmodNode instances to be used efficiently as keys in hash-based
		 *          collections (std::unordered_map, nfx::HashMap, etc.) without performance penalties.
		 *
		 *          **Performance Critical**: Hash is computed combining the node code and location (if present)
		 *          using optimized SSE4.2/FNV-1a algorithms and cached to avoid expensive recomputation.
		 *          This transforms hash lookups from O(n) string processing to O(1) cached access.
		 *
		 *          **Hash Strategy**: Combines node code hash with location hash (0 if no location).
		 *
		 * @return The pre-computed hash value combining this GmodNode's code and location
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::size_t hashCode() const noexcept;

	private:
		//----------------------------------------------
		// Relationship management methods
		//----------------------------------------------

		/*
		 * TODO: Move to internal
		 */

		/**
		 * @brief Adds a GmodNode instance as a child to this node.
		 * @details This method is intended for use by the `Gmod` class during graph construction.
		 *          It updates both the children list and the children set for quick lookups.
		 * @param child A non-owning pointer to the GmodNode to add as a child. Must not be null.
		 */
		inline void addChild( GmodNode* child ) noexcept;

		/**
		 * @brief Adds a GmodNode instance as a parent to this node.
		 * @details This method is intended for use by the `Gmod` class during graph construction.
		 * @param parent A non-owning pointer to the GmodNode to add as a parent. Must not be null.
		 */
		inline void addParent( GmodNode* parent ) noexcept;

		/**
		 * @brief Optimizes memory usage for child/parent vectors and rebuilds the child code set.
		 * @details This method is intended for use by the `Gmod` class after the graph structure is finalized.
		 *          It calls `shrink_to_fit()` on internal vectors and ensures `m_childrenSet` is consistent.
		 */
		inline void trim() noexcept;

		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Cached hash value computed during construction for O(1) hash access */
		std::size_t m_hashCode;

		/** @brief Unique identifier code of the node with precomputed hash for performance. */
		struct Code
		{
			/** @brief The node's unique identifier string. */
			std::string code;

			/** @brief Precomputed hash of the code string for O(1) hash operations. */
			std::size_t hash;
		} m_code;

		/** @brief Optional location information for this specific node instance. */
		std::optional<Location> m_location;

		/** @brief VIS (Vessel Information Structure) version associated with this node. */
		VisVersion m_visVersion;

		/** @brief Metadata object containing descriptive information about this node. */
		GmodNodeMetadata m_metadata;

		/** @brief Shared vector of non-owning pointers to direct child nodes. */
		std::shared_ptr<std::vector<GmodNode*>> m_children;

		/** @brief Shared vector of non-owning pointers to direct parent nodes. */
		std::shared_ptr<std::vector<GmodNode*>> m_parents;

		/** @brief Shared set of child codes for efficient `isChild(std::string_view)` lookups. */
		std::shared_ptr<nfx::containers::FastHashSet<std::string>> m_childrenSet;
	};
} // namespace dnv::vista::sdk

#include "detail/GmodNode.inl"

namespace std
{
	/**
	 * @brief Hash specialization for dnv::vista::sdk::GmodNode
	 * @details Enables GmodNode instances to be used as keys in all hash-based STL containers.
	 */
	template <>
	struct hash<dnv::vista::sdk::GmodNode>
	{
		/**
		 * @brief Returns the cached hash value for optimal performance
		 * @param[in] node The GmodNode instance to hash
		 * @return Pre-computed hash value (O(1) access) combining code and location using hardware-accelerated algorithms
		 */
		std::size_t operator()( const dnv::vista::sdk::GmodNode& node ) const noexcept
		{
			return node.hashCode();
		}
	};
} // namespace std
