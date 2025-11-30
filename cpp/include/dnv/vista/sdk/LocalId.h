/**
 * @file LocalId.h
 * @brief VISTA Local ID System for VIS Standard Maritime Data Identification
 *
 * @details
 * This file implements the **VISTA Local ID System** for representing and managing
 * immutable Local ID instances according to VIS standard (ISO 19848). It provides
 * validated Local ID representation with efficient property access, thread-safe
 * operations, and comprehensive parsing capabilities for maritime data identification.
 *
 * ## System Purpose:
 *
 * The **VISTA Local ID System** serves as the foundation for:
 * - **VIS Standard Compliance**   : Full ISO 19848 Local ID specification support
 * - **Immutable Value Semantics** : Thread-safe Local ID instances with direct value ownership
 * - **Zero-Overhead Architecture**: Direct property access via LocalIdBuilder delegation
 * - **Builder Pattern Validation**: Fluent construction with comprehensive validation guarantees
 * - **High-Performance Parsing**  : Optimized string-to-LocalId conversion with detailed error handling
 * - **Metadata Management**       : Complete support for all 8 VIS metadata tag types
 * - **Memory Efficiency**         : Direct value storage eliminating heap indirection overhead
 *
 * ## Core Architecture:
 *
 * ### Local ID Classes
 * - **LocalId**       : Immutable validated Local ID with value storage semantics
 * - **LocalIdBuilder**: Fluent builder for constructing valid Local ID instances
 * - **LocalIdItems**  : Container for primary and secondary GMOD path items
 * - **MetadataTag**   : Individual metadata components (quantity, content, etc.)
 *
 * ### Construction Pattern
 * - **Builder Pattern**       : Fluent interface for step-by-step Local ID construction and validation
 * - **Move Semantics**        : Efficient ownership transfer from mutable builder to immutable LocalId
 * - **Validation Gates**      : Comprehensive builder validation before LocalId construction
 * - **Direct Value Storage**  : LocalId wraps LocalIdBuilder for zero-overhead property access
 * - **Immutable Result**      : LocalId instances cannot be modified after creation
 * - **Protected Construction**: Only LocalIdBuilder and parsing methods can create LocalId instances
 *
 * ## Memory Layout & Performance:
 *
 * ```
 * LocalId Structure:
 * ┌─────────────────────────────────────┐
 * │              LocalId                │
 * │ ┌─────────────────────────────────┐ │
 * │ │        LocalIdBuilder           │ │ ← Direct value storage
 * │ │         m_builder               │ │
 * │ │ ┌─────────────────────────────┐ │ │
 * │ │ │    VisVersion               │ │ │ ← 2 bytes
 * │ │ │    LocalIdItems             │ │ │ ← Primary/Secondary paths
 * │ │ │    MetadataTag (quantity)   │ │ │ ← Optional metadata
 * │ │ │    MetadataTag (content)    │ │ │
 * │ │ │    MetadataTag (...)        │ │ │ ← Up to 8 metadata types
 * │ │ │    bool verboseMode         │ │ │ ← 1 byte
 * │ │ └─────────────────────────────┘ │ │
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *                  ↓
 *    Zero-Overhead Property + Hash Access
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │       Direct Access Operations      │
 * ├─────────────────────────────────────┤
 * │ - Inline property accessors         │
 * │ - Builder delegation (zero-copy)    │
 * │ - Cached hash access (O(1))         │
 * │ - Direct member access patterns     │
 * │ - No heap indirection overhead      │
 * │ - Thread-safe immutable reads       │
 * └─────────────────────────────────────┘
 *```
 *
 * ## Usage Examples:
 *
 * ### Basic Local ID Construction
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Local ID Parsing and Validation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Metadata Access and Properties
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Performance Characteristics:
 *
 * - **Property Access**    : O(1) inline accessors with zero indirection overhead
 * - **Construction**       : O(1) move from validated LocalIdBuilder to immutable LocalId
 * - **String Conversion**  : O(n) where n is total string length of all components
 * - **Parsing Operations** : O(m) where m is input string length with optimized tokenization
 * - **Equality Comparison**: O(1) builder equality delegation with short-circuit optimization
 * - **Memory Footprint**   : Minimal overhead with direct LocalIdBuilder value storage
 * - **Hash Computation**   : O(n) using nfx optimized SSE4.2/FNV-1a string hashing
 * - **Thread Safety**      : Lock-free concurrent read access to immutable data
 * - **Copy Operations**    : Efficient copying due to direct value semantics
 *
 * ## Design Philosophy:
 *
 * - **Standards Compliance**: Full adherence to VIS Local ID specification (ISO 19848)
 * - **Immutability First**  : Value semantics with no post-construction modification
 * - **Performance Critical**: Zero-overhead abstractions for high-frequency operations
 * - **Type Safety**         : Strong typing prevents invalid Local ID construction
 * - **Error Handling**      : Comprehensive validation with detailed error reporting
 * - **Builder Pattern**     : Fluent interface separating construction from usage
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <nfx/Hashing.h>

#include "config/config.h"
#include "LocalIdBuilder.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	class GmodPath;
	class MetadataTag;
	class ParsingErrors;
	enum class VisVersion : std::uint16_t;

	namespace mqtt
	{
		class LocalId;
	}

	//=====================================================================
	// LocalId class
	//=====================================================================

	/**
	 * @class LocalId
	 * @brief Immutable representation of a VIS Local ID with optimal performance.
	 *
	 * @details Represents a validated Local ID according to VIS standard (ISO 19848).
	 * Uses direct value storage for zero-overhead access. All instances are immutable
	 * and thread-safe for concurrent read access.
	 *
	 * Construction via LocalIdBuilder or static parse() methods.
	 * All property accessors are inline and noexcept for maximum performance.
	 */
	class LocalId
	{
		friend class LocalIdBuilder;
		friend class mqtt::LocalId;

	protected:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs LocalId from validated LocalIdBuilder.
		 * @details Protected constructor accessible only to friend classes:
		 *          - LocalIdBuilder (for normal construction)
		 *          - mqtt::LocalId (for MQTT inheritance)
		 * @param[in] builder Valid LocalIdBuilder instance.
		 * @throws std::invalid_argument If builder is invalid or empty.
		 */
		explicit LocalId( LocalIdBuilder builder );

	public:
		/** @brief Default constructor - creates an empty/invalid LocalId - Todo: check this */
		inline LocalId();

		/**
		 * @brief Copy constructor
		 * @param other The object to copy from
		 */
		LocalId( const LocalId& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The object to move from
		 */
		LocalId( LocalId&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		virtual ~LocalId() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The LocalId to copy from
		 * @return Reference to this LocalId after assignment
		 */
		LocalId& operator=( const LocalId& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The LocalId to move from
		 * @return Reference to this LocalId after assignment
		 */
		LocalId& operator=( LocalId&& other ) noexcept = default;

		//----------------------------------------------
		// Equality operators
		//----------------------------------------------

		/**
		 * @brief Checks equality with another LocalId.
		 * @param other The LocalId to compare with
		 * @return true if the LocalIds are equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator==( const LocalId& other ) const noexcept;

		/**
		 * @brief Checks inequality with another LocalId.
		 * @param other The LocalId to compare with
		 * @return true if the LocalIds are not equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator!=( const LocalId& other ) const noexcept;

		/**
		 * @brief Performs deep equality comparison.
		 * @param[in] other LocalId to compare against.
		 * @return true if semantically equal.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool equals( const LocalId& other ) const noexcept;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Gets direct access to underlying LocalIdBuilder.
		 * @return Const reference to internal builder.
		 * @note Zero-overhead access via direct reference.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const LocalIdBuilder& builder() const noexcept;

		/**
		 * @brief Gets VIS version of this Local ID.
		 * @return VIS version enum value.
		 * @note Guaranteed to contain value for valid LocalId.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline VisVersion visVersion() const noexcept;

		/**
		 * @brief Gets primary GMOD path item.
		 * @return Const reference to optional containing primary item.
		 * @note Zero-copy access. Guaranteed to contain value for valid LocalId.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<GmodPath>& primaryItem() const noexcept;

		/**
		 * @brief Gets optional secondary GMOD path item.
		 * @return Const reference to optional containing secondary item.
		 * @note Zero-copy access. May be empty if no secondary item specified.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<GmodPath>& secondaryItem() const noexcept;

		/**
		 * @brief Gets all metadata tags as collection.
		 * @return Vector containing copies of all present metadata tags.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::vector<MetadataTag> metadataTags() const noexcept;

		//----------------------------------------------
		// Metadata accessors
		//----------------------------------------------

		/**
		 * @brief Gets quantity metadata tag.
		 * @return Const reference to optional quantity metadata.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& quantity() const noexcept;

		/**
		 * @brief Gets content metadata tag.
		 * @return Const reference to optional content metadata.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& content() const noexcept;

		/**
		 * @brief Gets calculation metadata tag.
		 * @return Const reference to optional calculation metadata.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& calculation() const noexcept;

		/**
		 * @brief Gets state metadata tag.
		 * @return Const reference to optional state metadata.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& state() const noexcept;

		/**
		 * @brief Gets command metadata tag.
		 * @return Const reference to optional command metadata.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& command() const noexcept;

		/**
		 * @brief Gets type metadata tag.
		 * @return Const reference to optional type metadata.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& type() const noexcept;

		/**
		 * @brief Gets position metadata tag.
		 * @return Const reference to optional position metadata.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& position() const noexcept;

		/**
		 * @brief Gets detail metadata tag.
		 * @return Const reference to optional detail metadata.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<MetadataTag>& detail() const noexcept;

		//----------------------------------------------
		// State inspection
		//----------------------------------------------

		/**
		 * @brief Checks if Local ID is in verbose mode.
		 * @return true if verbose mode enabled.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isVerboseMode() const noexcept;

		/**
		 * @brief Checks if Local ID contains custom metadata tags.
		 * @return true if any custom tags present.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool hasCustomTag() const noexcept;

		//----------------------------------------------
		// Hashing
		//----------------------------------------------

		/**
		 * @brief Gets the cached hash value for this LocalId.
		 * @details **Purpose**: Enables LocalId instances to be used efficiently as keys in hash-based
		 *          collections (std::unordered_map, nfx::HashMap, etc.) without performance penalties.
		 *
		 *          **Performance Critical**: Hash is pre-computed during construction using optimized
		 *          SSE4.2/FNV-1a algorithms and cached to avoid expensive recomputation. This transforms
		 *          hash lookups from O(n) string processing to O(1) cached access.
		 *
		 *          **VIS Version Hash Behavior**: LocalIds with identical content but different VIS
		 *          versions (e.g., vis-3-6a vs vis-3-7a) intentionally (?) produce the same hash value.
		 *          This design ensures hash consistency across VIS version upgrades, allowing
		 *          data systems to maintain stable hash-based indexes when transitioning between VIS
		 *          versions. The VIS version is excluded from hash computation by design to preserve
		 *          semantic equivalence across specification versions.
		 *
		 * @return The cached hash value computed using hardware-accelerated algorithms.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 * @note LocalIds with different VIS versions but identical content will have the same hash
		 */
		[[nodiscard]] inline std::size_t hashCode() const noexcept;

		//----------------------------------------------
		// String conversion
		//----------------------------------------------

		/**
		 * @brief Converts LocalId to canonical string representation.
		 * @return VIS-compliant Local ID string.
		 * @throws std::invalid_argument If conversion fails.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::string toString() const;

		//----------------------------------------------
		// Static parsing methods
		//----------------------------------------------

		/**
		 * @brief Parses Local ID string into LocalId object.
		 * @param[in] localIdStr VIS Local ID string to parse.
		 * @return Parsed LocalId object.
		 * @throws std::invalid_argument If parsing fails.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static LocalId parse( std::string_view localIdStr );

		/**
		 * @brief Attempts to parse Local ID string with error reporting.
		 * @param[in] localIdStr String to parse.
		 * @param[out] errors Detailed parsing errors.
		 * @param[out] localId Parsed result on success.
		 * @return true if parsing succeeded.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool tryParse( std::string_view localIdStr, ParsingErrors& errors, std::optional<LocalId>& localId );

		/**
		 * @brief Attempts to parse Local ID string.
		 * @param[in] localIdStr String to parse.
		 * @param[out] localId Parsed result on success.
		 * @return true if parsing succeeded.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool tryParse( std::string_view localIdStr, std::optional<LocalId>& localId );

	private:
		//----------------------------------------------
		// Private members variables
		//----------------------------------------------

		/** @brief Immutable LocalIdBuilder instance containing all LocalId data and validation state */
		LocalIdBuilder m_builder;
	};
} // namespace dnv::vista::sdk

#include "detail/LocalId.inl"

namespace std
{
	/**
	 * @brief Hash specialization for dnv::vista::sdk::LocalId.
	 * @details Enables LocalId instances to be used as keys in all hash-based STL containers.
	 */
	template <>
	struct hash<dnv::vista::sdk::LocalId>
	{
		/**
		 * @brief Returns the cached hash value for optimal performance.
		 * @param[in] localId The LocalId instance to hash.
		 * @return Pre-computed hash value (O(1) access) using hardware-accelerated algorithms.
		 */
		std::size_t operator()( const dnv::vista::sdk::LocalId& localId ) const noexcept
		{
			return localId.hashCode();
		}
	};
} // namespace std
