/**
 * @file VIS.h
 * @brief Vessel Information Structure (VIS) Central Management System
 *
 * @section overview System Overview
 *
 * The VIS (Vessel Information Structure) system provides comprehensive access to maritime
 * data standards including GMOD (Generic Product Model), Codebooks, and Location
 * hierarchies across multiple VIS versions. This singleton-based architecture ensures
 * efficient resource management, thread-safe operations, and consistent data access
 * patterns throughout the Vista SDK.
 *
 * @section architecture Core Architecture
 *
 * **Singleton Pattern** : Ensures single point of access and eliminates resource duplication
 * **Memory Caching**    : Multi-layer caching system with sliding expiration (1-hour default - TODO: Complete the implementation)
 * **Version Management**: Seamless conversion between VIS versions with fallback handling
 * **Resource Loading**  : Embedded resource management with lazy initialization
 * **Thread Safety**     : Concurrent access support through internal synchronization
 *
 * @section components Key Components
 *
 * - **GMOD Access**         : Generic Product Model retrieval and version conversion
 * - **Codebooks Management**: Maritime standard codebook access and validation
 * - **Location Hierarchies**: Ship location tree structures and navigation
 * - **Version Conversion**  : Cross-version data transformation and compatibility
 * - **Bulk Operations**     : Efficient multi-version data collection methods
 *
 * @section performance Performance Characteristics
 *
 * - **Initial Load**     : TODO: measure, before and after caching
 * - **Cache Access**     : TODO: measure, before and after caching
 * - **Memory Usage**     : TODO: measure, before and after caching
 * - **Conversion Speed** : TODO: measure, before and after caching
 * - **Thread Contention**: Minimal overhead with concurrent access patterns
 *
 * @section usage Usage Patterns
 *
 * ```cpp
 * // Basic resource access
 * const auto& vis = VIS::instance();
 * const auto& gmod = vis.gmod(VisVersion::v3_9a);
 * const auto& codebooks = vis.codebooks(VisVersion::v3_9a);
 *
 * // Version conversion
 * auto converted = vis.convertPath(sourcePath, VisVersion::v3_9a);
 * if (converted) {
 *     // Use converted path
 * }
 *
 * // Bulk operations
 * auto allGmods = vis.gmodsMap(vis.visVersions());
 * ```
 *
 * @section threading Thread Safety
 *
 * All VIS operations are thread-safe through a const-correct public interface design.
 * The singleton instance() returns const VIS&, and all accessor and conversion methods
 * are const, ensuring safe concurrent read access without external synchronization.
 * Internal caching operations use carefully controlled const_cast for implementation
 * flexibility while maintaining external immutability guarantees (TODO: Complete the implementation).
 *
 * @section error_handling Error Handling
 *
 * - **Invalid Versions**   : std::invalid_argument for unsupported VIS versions
 * - **Resource Loading**   : std::runtime_error for embedded resource failures
 * - **Conversion Failures**: std::nullopt return for impossible conversions
 * - **Memory Issues**      : Standard exception propagation for allocation failures
 *
 * @section version Version Support
 *
 * Supports VIS versions 3.4a through 3.9a with automatic version detection
 * and compatibility matrices. Version conversion uses embedded transformation
 * rules with fallback mechanisms for unsupported conversions.
 */

#pragma once

#include <vector>

#include <nfx/containers/StringMap.h>
#include <nfx/string/Utils.h>

#include "Codebooks.h"
#include "Gmod.h"
#include "Locations.h"
#include "VISVersion.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	class GmodNode;
	class GmodPath;
	class LocalId;
	class LocalIdBuilder;

	namespace internal
	{
		class GmodVersioning;
	}

	//=====================================================================
	// VIS Central Management System
	//=====================================================================

	/**
	 * @brief Central Vessel Information Structure Management System
	 *
	 * The VIS class serves as the primary entry point for all maritime data operations
	 * within the Vista SDK. It implements a sophisticated singleton pattern with
	 * multi-layer caching, thread-safe operations, and comprehensive version management
	 * for GMOD, Codebooks, and Location hierarchies.
	 *
	 * **Design Philosophy**:
	 * - Single point of truth for VIS data access
	 * - Lazy loading with intelligent caching strategies
	 * - Zero-allocation access patterns for cached resources
	 * - Seamless version conversion with fallback mechanisms
	 * - Const-correct public API ensuring thread-safe read-only operations
	 *
	 * **Memory Management**:
	 * - Sliding expiration cache (1-hour default - TODO: Complete the implementation)
	 * - Automatic resource cleanup and memory optimization
	 * - Embedded resource loading with compression support
	 * - Reference-based access to eliminate unnecessary copies
	 *
	 * **Concurrency Model**:
	 * - Thread-safe singleton initialization
	 * - Const-correct public interface for safe concurrent read access
	 * - Internal implementation uses const_cast for caching flexibility
	 * - Lock-free access paths for performance-critical operations
	 * - All public methods are const, ensuring immutable external interface
	 *
	 * **Const Correctness Design**:
	 * - Public API presents const interface for thread-safe read operations
	 * - Internal caching implementation uses const_cast where necessary
	 * - Ensures that external callers cannot modify shared VIS state
	 * - Enables safe concurrent access patterns without external locking
	 *
	 * @note This class cannot be copied, moved, or instantiated directly.
	 *       Use VIS::instance() to access the singleton instance.
	 *
	 * @invariant The singleton instance remains valid throughout application lifetime
	 * @invariant All cached resources maintain referential integrity
	 * @invariant Version conversion operations are deterministic and repeatable
	 * @invariant Public const interface guarantees thread-safe read-only access
	 */
	class VIS final
	{
	private:
		//----------------------------------------------
		// Construction and Destruction Control
		//----------------------------------------------

		/**
		 * @brief Private constructor enforcing singleton pattern
		 *
		 * Initializes the VIS singleton instance with default configuration.
		 * Constructor is private to prevent direct instantiation.
		 *
		 * @note Access through VIS::instance() only
		 */
		inline VIS();

		/** @brief Copy constructor */
		VIS( const VIS& ) = delete;

		/** @brief Move constructor */
		VIS( VIS&& ) noexcept = delete;

	public:
		//----------------------------------------------
		// Lifecycle Management
		//----------------------------------------------

		/**
		 * @brief Default destructor
		 *
		 * Automatically manages cleanup of cached resources and
		 * internal state. Called automatically at program termination.
		 */
		~VIS() = default;

		//----------------------------------------------
		// Assignment Control
		//----------------------------------------------

		/** @brief Copy assignment operator */
		VIS& operator=( const VIS& ) = delete;

		/** @brief Move assignment operator */
		VIS& operator=( VIS&& ) noexcept = delete;

		//----------------------------------------------
		// Singleton Access Interface
		//----------------------------------------------

		/**
		 * @brief Provides thread-safe access to the VIS singleton instance
		 *
		 * Returns a const reference to the unique VIS instance, creating it on first
		 * access using thread-safe static local initialization. Subsequent calls
		 * return the same instance with zero overhead.
		 *
		 * **Thread Safety**: Guaranteed thread-safe initialization (C++11 standard)
		 * **Performance**  : Zero overhead after first initialization
		 * **Lifetime**     : Instance persists until program termination
		 *
		 * The const interface design ensures that all VIS operations are inherently
		 * thread-safe for concurrent read access, while internal caching operations
		 * use carefully controlled mutable state or const_cast for implementation flexibility.
		 *
		 * @return Const reference to the singleton VIS instance
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * const auto& vis = VIS::instance();
		 * const auto& gmod = vis.gmod(VisVersion::v3_9a);
		 * @endcode
		 */
		[[nodiscard]] inline const static VIS& instance();

		//----------------------------------------------
		// Version Information Interface
		//----------------------------------------------

		//-----------------------------
		// VisVersion Queries
		//-----------------------------

		/**
		 * @brief Retrieves all supported VIS versions
		 *
		 * Returns a reference to a static vector containing all VIS versions
		 * supported by the current SDK build. The vector is ordered from
		 * oldest to newest version for consistent iteration patterns.
		 *
		 * **Performance**: Zero allocation, constant time access
		 * **Memory**     : References static data, no dynamic allocation
		 * **Ordering**   : Chronological from oldest to newest version
		 *
		 * @return Const reference to vector of all supported VisVersion values
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * for (const auto& version : VIS::instance().visVersions()) {
		 *     const auto& gmod = vis.gmod(version);
		 *     // Process each version's GMOD
		 * }
		 * @endcode
		 */
		[[nodiscard]] inline const std::vector<VisVersion>& visVersions() const noexcept;

		/**
		 * @brief Retrieves the latest supported VIS version
		 *
		 * Returns the most recent VIS version supported by the current SDK build.
		 * This is typically the recommended version for new applications and
		 * represents the most complete feature set.
		 *
		 * **Performance**   : Compile-time constant, zero overhead
		 * **Stability**     : Version remains constant for SDK build
		 * **Recommendation**: Use for new development when version is not specified
		 *
		 * @return The latest VisVersion enumeration value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * const auto latestVersion = VIS::instance().latestVisVersion();
		 * const auto& gmod = vis.gmod(latestVersion);
		 * @endcode
		 */
		[[nodiscard]] inline VisVersion latestVisVersion() const noexcept;

		//----------------------------------------------
		// Core Resource Access Interface
		//----------------------------------------------

		/**
		 * @brief Retrieves GMOD (Generic Product Model) for specified VIS version
		 *
		 * Provides access to the complete GMOD hierarchy for a specific VIS version.
		 * The GMOD contains the standardized tree structure of marine equipment,
		 * systems, and data points used for vessel data identification and navigation.
		 *
		 * **Caching Behavior**: First access loads and caches the GMOD, subsequent
		 * accesses return cached reference - TODO: benchmark.
		 *
		 * **Memory Management**: Returns const reference to cached instance, zero
		 * allocation for repeated access patterns.
		 *
		 * **Thread Safety**: Concurrent access supported with internal synchronization.
		 *
		 * @param visVersion The VIS version for which to retrieve GMOD data
		 * @return Const reference to the GMOD instance for the specified version
		 * @throws std::invalid_argument If visVersion is invalid or unsupported
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * const auto& gmod = VIS::instance().gmod(VisVersion::v3_9a);
		 * auto rootNode = gmod.rootNode();
		 * @endcode
		 */
		[[nodiscard]] const Gmod& gmod( VisVersion visVersion ) const;

		/**
		 * @brief Retrieves Codebooks for specified VIS version
		 *
		 * Provides access to maritime standard codebooks containing enumerated
		 * values, validation rules, and standardized naming conventions for
		 * the specified VIS version.
		 *
		 * **Validation Support**: Codebooks enable validation of LocalId components,
		 * data channel types, and format specifications.
		 *
		 * **Caching Behavior**: Intelligent caching with sliding expiration,
		 * optimized for repeated validation operations.
		 *
		 * **Memory Efficiency**: Reference-based access eliminates copy overhead
		 * for performance-critical validation scenarios.
		 *
		 * @param visVersion The VIS version for which to retrieve codebook data
		 * @return Const reference to the Codebooks instance for the specified version
		 * @throws std::invalid_argument If visVersion is invalid or unsupported
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * const auto& codebooks = VIS::instance().codebooks(VisVersion::v3_9a);
		 * bool isValid = codebooks.isValidDataChannelType("temperature");
		 * @endcode
		 */
		[[nodiscard]] const Codebooks& codebooks( VisVersion visVersion ) const;

		/**
		 * @brief Retrieves Locations hierarchy for specified VIS version
		 *
		 * Provides access to the standardized vessel location tree structure
		 * defining physical and logical locations within marine vessels.
		 * The hierarchy supports navigation, containment relationships,
		 * and location-specific data point identification.
		 *
		 * **Hierarchy Navigation**: Enables traversal of parent-child relationships
		 * and location containment structures.
		 *
		 * **Performance Optimization**: Cached location trees with optimized
		 * lookup operations for path resolution.
		 *
		 * **Memory Model**: Reference-based access to cached location hierarchy,
		 * supporting zero-allocation navigation patterns.
		 *
		 * @param visVersion The VIS version for which to retrieve location hierarchy
		 * @return Const reference to the Locations instance for the specified version
		 * @throws std::invalid_argument If visVersion is invalid or unsupported
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * const auto& locations = VIS::instance().locations(VisVersion::v3_9a);
		 * auto engineRoom = locations.find("/Ship/PropulsionSystem/EngineRoom");
		 * @endcode
		 */
		[[nodiscard]] const Locations& locations( VisVersion visVersion ) const;

		//-----------------------------
		// Bulk Resource Access Interface
		//-----------------------------

		/**
		 * @brief Retrieves GMOD instances for multiple VIS versions efficiently
		 *
		 * Constructs a map containing GMOD instances for all specified VIS versions
		 * using optimized bulk loading strategies. This method is more efficient
		 * than individual gmod() calls when multiple versions are needed.
		 *
		 * **Bulk Optimization**: Single allocation for result map with reserved capacity.
		 * **Cache Utilization**: Leverages existing cache entries and performs
		 * batch loading for uncached versions.
		 * **Memory Efficiency**: Move semantics for GMOD instances minimize copying.
		 *
		 * @param visVersions Vector of VIS versions for which to retrieve GMODs
		 * @return New unordered_map with VisVersion keys and Gmod values
		 * @throws std::invalid_argument If any visVersion is invalid or unsupported
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * auto allVersions = VIS::instance().visVersions();
		 * auto gmodMap = VIS::instance().gmodsMap(allVersions);
		 * for (const auto& [version, gmod] : gmodMap) {
		 *     // Process each version's GMOD
		 * }
		 * @endcode
		 */
		[[nodiscard]] std::unordered_map<VisVersion, Gmod> gmodsMap( const std::vector<VisVersion>& visVersions );

		/**
		 * @brief Retrieves Codebooks instances for multiple VIS versions efficiently
		 *
		 * Constructs a map containing Codebooks instances for all specified VIS versions
		 * using optimized bulk loading strategies with intelligent cache management.
		 *
		 * **Validation Optimization**: Ideal for cross-version validation scenarios
		 * where multiple codebook versions need comparison.
		 * **Memory Management**: Efficient bulk allocation with move semantics.
		 * **Cache Strategy**: Batch loading minimizes individual cache operations.
		 *
		 * @param visVersions Vector of VIS versions for which to retrieve Codebooks
		 * @return New unordered_map with VisVersion keys and Codebooks values
		 * @throws std::invalid_argument If any visVersion is invalid or unsupported
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * auto targetVersions = {VisVersion::v3_8a, VisVersion::v3_9a};
		 * auto codebooksMap = VIS::instance().codebooksMap(targetVersions);
		 * @endcode
		 */
		[[nodiscard]] std::unordered_map<VisVersion, Codebooks> codebooksMap( const std::vector<VisVersion>& visVersions );

		/**
		 * @brief Retrieves Locations instances for multiple VIS versions efficiently
		 *
		 * Constructs a map containing Locations instances for all specified VIS versions
		 * using optimized bulk loading strategies and intelligent cache utilization.
		 *
		 * **Location Analysis**: Enables cross-version location hierarchy comparison
		 * and migration analysis between VIS versions.
		 * **Memory Efficiency**: Bulk operations with optimized allocation patterns.
		 * **Navigation Support**: Multiple location hierarchies for version comparison.
		 *
		 * @param visVersions Vector of VIS versions for which to retrieve Locations
		 * @return New unordered_map with VisVersion keys and Locations values
		 * @throws std::invalid_argument If any visVersion is invalid or unsupported
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * auto versions = {VisVersion::v3_7a, VisVersion::v3_8a, VisVersion::v3_9a};
		 * auto locationsMap = VIS::instance().locationsMap(versions);
		 * @endcode
		 */
		[[nodiscard]] std::unordered_map<VisVersion, Locations> locationsMap( const std::vector<VisVersion>& visVersions );

		//----------------------------------------------
		// Version Conversion Interface
		//----------------------------------------------

		//-----------------------------
		// GMOD Node Conversion
		//-----------------------------

		/**
		 * @brief Converts GMOD node between VIS versions with explicit version specification
		 *
		 * Performs intelligent conversion of GMOD nodes between different VIS versions
		 * using embedded transformation rules and compatibility matrices. The conversion
		 * process handles structural changes, naming conventions, and semantic evolution
		 * between VIS versions.
		 *
		 * **Conversion Strategy**: Uses embedded transformation rules with fallback
		 * mechanisms for handling breaking changes between versions.
		 *
		 * **Performance**: TODO: benchmark.
		 *
		 * **Compatibility**: Handles forward/backward conversion with intelligent
		 * mapping for renamed or restructured GMOD elements.
		 *
		 * @param sourceVersion VIS version of the source GMOD node
		 * @param sourceNode GMOD node instance to convert
		 * @param targetVersion Target VIS version for conversion
		 * @return Optional containing converted GmodNode, or nullopt if conversion impossible
		 * @throws std::invalid_argument If any VIS version is invalid or unsupported
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * auto sourceNode = gmod_v3_8a.findNode("/Ship/Engine");
		 * auto converted = VIS::instance().convertNode(
		 *     VisVersion::v3_8a, sourceNode, VisVersion::v3_9a);
		 * if (converted) {
		 *     // Use converted node in v3_9a context
		 * }
		 * @endcode
		 */
		[[nodiscard]] std::optional<GmodNode> convertNode( VisVersion sourceVersion, const GmodNode& sourceNode, VisVersion targetVersion ) const;

		/**
		 * @brief Converts GMOD node with automatic source version detection
		 *
		 * Performs GMOD node conversion by automatically detecting the source VIS version
		 * from the node's embedded version information. This overload provides simplified
		 * usage when the source version context is embedded in the node.
		 *
		 * **Version Detection**: Automatically extracts source VIS version from node
		 * metadata, eliminating manual version specification.
		 *
		 * **Context Support**: Optional parent node parameter provides additional
		 * context for complex conversion scenarios involving hierarchical relationships.
		 *
		 * **Error Handling**: Graceful handling of version detection failures with
		 * detailed error information for debugging.
		 *
		 * @param sourceNode GMOD node with embedded version information
		 * @param targetVersion Target VIS version for conversion
		 * @param sourceParent Optional parent node for contextual conversion (default: nullptr)
		 * @return Optional containing converted GmodNode, or nullopt if conversion impossible
		 * @throws std::invalid_argument If target VIS version is invalid or unsupported
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * auto converted = VIS::instance().convertNode(sourceNode, VisVersion::v3_9a);
		 * if (converted) {
		 *     // Source version was automatically detected
		 * }
		 * @endcode
		 */
		[[nodiscard]] std::optional<GmodNode> convertNode( const GmodNode& sourceNode, VisVersion targetVersion, const GmodNode* sourceParent = nullptr ) const;

		//-----------------------------
		// GMOD Path Conversion
		//-----------------------------

		/**
		 * @brief Converts GMOD path between VIS versions with explicit version specification
		 *
		 * Performs comprehensive path conversion between VIS versions, handling path
		 * restructuring, element renaming, and hierarchical changes in GMOD organization.
		 * The conversion maintains semantic meaning while adapting to structural evolution.
		 *
		 * **Path Transformation**: Intelligent mapping of path segments with support
		 * for renamed elements, moved hierarchies, and structural reorganization.
		 *
		 * **Semantic Preservation**: Maintains logical meaning of paths even when
		 * underlying GMOD structure changes between versions.
		 *
		 * **Performance Optimization**: Cached transformation rules and optimized
		 * path parsing for high-throughput conversion scenarios.
		 *
		 * @param sourceVersion VIS version of the source GMOD path
		 * @param sourcePath GMOD path instance to convert
		 * @param targetVersion Target VIS version for conversion
		 * @return Optional containing converted GmodPath, or nullopt if conversion impossible
		 * @throws std::invalid_argument If any VIS version is invalid or unsupported
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * GmodPath sourcePath("/Ship/Engine/Temperature", VisVersion::v3_8a);
		 * auto converted = VIS::instance().convertPath(
		 *     VisVersion::v3_8a, sourcePath, VisVersion::v3_9a);
		 * @endcode
		 */
		[[nodiscard]] std::optional<GmodPath> convertPath( VisVersion sourceVersion, const GmodPath& sourcePath, VisVersion targetVersion ) const;

		/**
		 * @brief Converts GMOD path with automatic source version detection
		 *
		 * Performs GMOD path conversion by automatically detecting the source VIS version
		 * from the path's embedded version information, providing simplified usage
		 * for scenarios where version context is available in the path.
		 *
		 * **Automatic Detection**: Extracts source VIS version from path metadata,
		 * eliminating need for explicit version specification.
		 *
		 * **Path Integrity**: Maintains path validity and navigational correctness
		 * throughout the conversion process.
		 *
		 * **Conversion Reliability**: Robust handling of edge cases and invalid
		 * paths with comprehensive error reporting.
		 *
		 * @param sourcePath GMOD path with embedded version information
		 * @param targetVersion Target VIS version for conversion
		 * @return Optional containing converted GmodPath, or nullopt if conversion impossible
		 * @throws std::invalid_argument If target VIS version is invalid or unsupported
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * auto converted = VIS::instance().convertPath(sourcePath, VisVersion::v3_9a);
		 * if (converted) {
		 *     auto newPathString = converted->toString();
		 * }
		 * @endcode
		 */
		[[nodiscard]] std::optional<GmodPath> convertPath( const GmodPath& sourcePath, VisVersion targetVersion ) const;

		//-----------------------------
		// LocalId Conversion
		//-----------------------------

		/**
		 * @brief Converts LocalIdBuilder instance between VIS versions
		 *
		 * Performs comprehensive conversion of LocalIdBuilder instances, handling
		 * changes in validation rules, codebook references, and structural requirements
		 * between different VIS versions. The conversion maintains builder state
		 * and validation context throughout the process.
		 *
		 * **Builder State Preservation**: Maintains all builder configuration including
		 * IMO number, naming rule, and metadata during conversion.
		 *
		 * **Validation Context**: Updates validation rules and codebook references
		 * to match target VIS version requirements.
		 *
		 * **Fluent Interface**: Returned builder maintains fluent interface patterns
		 * for continued construction in target version context.
		 *
		 * @param sourceLocalId LocalIdBuilder instance to convert
		 * @param targetVersion Target VIS version for conversion
		 * @return Optional containing converted LocalIdBuilder, or nullopt if conversion impossible
		 * @throws std::invalid_argument If target VIS version is invalid or unsupported
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * auto builder = LocalIdBuilder::forImoNumber(1234567)
		 *     .withGmodPath(path);
		 * auto converted = VIS::instance().convertLocalId(builder, VisVersion::v3_9a);
		 * if (converted) {
		 *     auto localId = converted->build();
		 * }
		 * @endcode
		 */
		[[nodiscard]] std::optional<LocalIdBuilder> convertLocalId( const LocalIdBuilder& sourceLocalId, VisVersion targetVersion ) const;

		/**
		 * @brief Converts LocalId instance between VIS versions
		 *
		 * Performs conversion of finalized LocalId instances between VIS versions,
		 * handling format changes, validation rule updates, and structural evolution
		 * in LocalId specification across versions.
		 *
		 * **Format Adaptation**: Handles changes in LocalId string format, component
		 * structure, and encoding rules between VIS versions.
		 *
		 * **Validation Update**: Applies target version validation rules while
		 * preserving semantic meaning of the identifier.
		 *
		 * **Metadata Preservation**: Maintains LocalId metadata and context
		 * information during conversion process.
		 *
		 * @param sourceLocalId LocalId instance to convert
		 * @param targetVersion Target VIS version for conversion
		 * @return Optional containing converted LocalId, or nullopt if conversion impossible
		 * @throws std::invalid_argument If target VIS version is invalid or unsupported
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @code
		 * LocalId sourceId("1234567-/Ship/Engine/Temperature-temperature");
		 * auto converted = VIS::instance().convertLocalId(sourceId, VisVersion::v3_9a);
		 * if (converted) {
		 *     std::string newIdString = converted->toString();
		 * }
		 * @endcode
		 */
		[[nodiscard]] std::optional<LocalId> convertLocalId( const LocalId& sourceLocalId, VisVersion targetVersion ) const;
	};
}

#include "VIS.inl"
