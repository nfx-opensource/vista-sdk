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
 * - **VIS Standard Compliance**  : Full ISO 19848 Local ID specification support
 * - **Immutable Value Semantics**: Thread-safe Local ID instances with value ownership
 * - **High-Performance Access**   Zero-overhead property accessors via direct storage
 * - **Comprehensive Validation**  Builder-based construction with validation guarantees
 * - **Parsing and Serialization** String-to-LocalId conversion with error handling
 * - **Metadata Management**       Complete support for all VIS metadata tag types
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
 * - **Builder Pattern** : Fluent interface for step-by-step Local ID construction
 * - **Move Semantics**  : Efficient ownership transfer from builder to LocalId
 * - **Validation Gates**: Builder validation before LocalId construction
 * - **Immutable Result**: LocalId instances cannot be modified after creation
 *
 * ## Memory Layout & Performance:
 *
 * ```
 * LocalId Structure:
 * ┌─────────────────────────────────────┐
 * │              LocalId                │
 * ├─────────────────────────────────────┤
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
 *
 * Key Performance Features:
 * - Value storage (no heap indirection)
 * - Inline property access (zero overhead)
 * - Move semantics for construction
 * - Thread-safe read operations
 * ```
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
 * - **Property Access**    : O(1) inline accessors with no indirection overhead
 * - **Construction**       : O(1) move from validated builder to LocalId
 * - **String Conversion**  : O(n) where n is total string length of all components
 * - **Equality Comparison**: O(1) builder equality delegation
 * - **Memory Footprint**   : Minimal overhead with direct value storage
 * - **Thread Safety**      : Lock-free concurrent read access to immutable data
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
	class LocalId final
	{
		friend class LocalIdBuilder;

	private:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs LocalId from validated LocalIdBuilder.
		 * @param[in] builder Valid LocalIdBuilder instance (moved).
		 * @throws std::invalid_argument If builder is invalid or empty.
		 */
		explicit LocalId( LocalIdBuilder builder );

	public:
		/** @brief Copy constructor */
		LocalId( const LocalId& other ) = default;

		/** @brief Move constructor */
		LocalId( LocalId&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~LocalId() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		LocalId& operator=( const LocalId& other ) = delete;

		/** @brief Move assignment operator */
		LocalId& operator=( LocalId&& other ) noexcept = default;

		//----------------------------------------------
		// Equality operators
		//----------------------------------------------

		/**
		 * @brief Checks equality with another LocalId.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator==( const LocalId& other ) const noexcept;

		/**
		 * @brief Checks inequality with another LocalId.
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

		LocalIdBuilder m_builder;
	};
}

//=====================================================================
// std::hash specialization for LocalId
//=====================================================================

namespace std
{
	template <>
	struct hash<dnv::vista::sdk::LocalId>
	{
		std::size_t operator()( const dnv::vista::sdk::LocalId& localId ) const noexcept
		{
			return std::hash<std::string>{}( localId.toString() );
		}
	};
}

#include "LocalId.inl"
