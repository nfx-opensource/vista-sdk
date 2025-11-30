/**
 * @file MetadataTag.h
 * @brief VISTA Metadata Tag System for Maritime Data Classification and Validation
 *
 * @details
 * This file implements the **VISTA Metadata Tag System** for creating, managing, and
 * validating standardized metadata tags used in maritime data classification. It provides
 * comprehensive tag creation, validation, and serialization capabilities according to
 * VIS standards, supporting both standard and custom tag definitions.
 *
 * ## System Purpose:
 *
 * The **VISTA Metadata Tag System** serves as the foundation for:
 * - **Data Classification**: Standardized tagging for maritime data categorization and organization
 * - **Metadata Validation**: Ensuring tags conform to VIS codebook specifications and vocabulary
 * - **Tag Serialization**  : Converting tags to string representations for storage and transmission
 * - **Custom Extensions**  : Supporting domain-specific custom tags while maintaining standard compliance
 * - **Query Integration**  : Enabling efficient metadata-based data queries and filtering operations
 * - **Type-Safe Creation** : Enum-based tag names prevent invalid metadata construction
 * - **Immutable Design**   : Thread-safe tag objects with guaranteed consistency after creation
 *
 * ## Core Architecture:
 *
 * ### Metadata Components
 * - **MetadataTag**      : Immutable tag with name, value, and custom flag
 * - **CodebookName**     : Enumeration of standardized tag names
 * - **Tag Validation**   : Codebook-based validation for tag creation
 * - **String Conversion**: Flexible serialization with prefix and separator support
 *
 * ### Tag Structure
 * - **Name Component** : CodebookName enum-based standardized name (e.g., Position, Detail, Quantity)
 * - **Value Component**: String value conforming to codebook vocabulary and validation rules
 * - **Custom Flag**    : Boolean flag indicating standard (false) vs. custom (true) tag classification
 * - **Prefix System**  : Visual differentiation with '-' for standard and '~' for custom tags
 * - **Immutable State**: All components fixed after construction ensuring thread safety
 * - **Compact Storage**: Efficient memory layout with enum name and string value
 *
 * ## Data Flow Architecture:
 *
 * ```
 *         Codebook Validation
 *                  ↓
 *        MetadataTag Creation
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │           MetadataTag               │
 * ├─────────────────────────────────────┤
 * │      std::size_t m_hashCode         │ ← Cached hash (8 bytes, O(1) access)
 * ├─────────────────────────────────────┤
 * │ ┌─────────────────────────────────┐ │
 * │ │      CodebookName               │ │ ← Standardized name enum
 * │ │     (m_name)                    │ │
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │      String Value               │ │ ← Validated tag value
 * │ │     (m_value)                   │ │
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │      Custom Flag                │ │ ← Standard vs. custom
 * │ │     (m_custom)                  │ │
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *                  ↓
 *      String Serialization & Usage
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │    Tag Output & Formatting          │
 * ├─────────────────────────────────────┤
 * │ - Prefix-based tag identification   │
 * │ - Standard (-) vs. custom (~) tags  │
 * │ - Flexible separator configuration  │
 * │ - StringBuilder-based serialization │
 * │ - Query format generation           │
 * │ - URI-compatible string encoding    │
 * └─────────────────────────────────────┘
 *```
 *
 * ## Usage Patterns:
 *
 * ### Standard Tag Creation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Tag Serialization and Formatting
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Performance Characteristics:
 *
 * - **Immutable Design**   : Thread-safe operations with no post-construction changes
 * - **Zero-Copy Access**   : `string_view` interfaces minimize memory allocation overhead
 * - **Efficient Storage**  : Compact representation with CodebookName enum and std::string
 * - **Fast Comparison**    : Optimized equality operations for tag matching and filtering
 * - **Lazy Serialization** : String conversion only when explicitly requested via toString()
 * - **Enum-Based Names**   : O(1) name comparison using CodebookName enum values
 * - **Memory Optimization**: Minimal footprint with three-member structure design
 *
 * ## Tag Classification System:
 *
 * ### Standard Tags
 * - **Validation Required**: Must conform to VIS codebook vocabulary standards and rules
 * - **Prefix Character**   : '-' (hyphen) for visual identification in serialized form
 * - **Name Validation**    : CodebookName must exist in official VIS standards enumeration
 * - **Value Validation**   : Value must be valid according to specific codebook validation rules
 * - **Interoperability**   : Guaranteed compatibility across all VIS-compliant systems
 *
 * ### Custom Tags
 * - **Extended Vocabulary**  : Support for domain-specific, proprietary, or experimental values
 * - **Prefix Character**     : '~' (tilde) for visual identification in serialized form
 * - **Relaxed Validation**   : Value validation may be less strict or domain-specific
 * - **Forward Compatibility**: Maintains interoperability with standard VIS systems
 * - **Custom Extensions**    : Enables innovation while preserving core standard compliance
 *
 * ## String Representation Format:
 *
 * ### Serialization Pattern
 * ```
 * [prefix][name_abbreviation][separator][value][terminator]
 *
 * Examples:
 * - Standard tag: "pos-centre/"
 * - Custom tag:   "pos~custom_value/"
 * - Query format: "det-temperature"
 * ```
 *
 * ## Design Philosophy:
 *
 * - **Standards Compliance**: Full adherence to VIS metadata specifications and codebook validation
 * - **Type Safety**         : CodebookName enum typing prevents invalid tag name construction
 * - **Performance Focus**   : Optimized for high-frequency maritime data processing operations
 * - **Immutable Objects**   : Thread-safe design with guaranteed immutable tag state
 * - **Extensibility**       : Custom tag support while maintaining standard system compatibility
 * - **Intuitive API**       : Clear, discoverable interfaces for common metadata operations
 * - **Memory Efficiency**   : Compact three-member design with minimal memory overhead
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include <nfx/string/StringBuilder.h>

#include "config/config.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	enum class CodebookName : std::uint8_t;

	//=====================================================================
	// MetadataTag class
	//=====================================================================

	/**
	 * @brief Represents a metadata tag in the VIS system.
	 *
	 * A metadata tag consists of a name (from a codebook), a value, and an optional
	 * flag indicating whether it is a custom tag. It is used to store and manage
	 * metadata associated with various entities. This class is immutable;
	 * all properties are set during construction.
	 */
	class MetadataTag final
	{
		friend class Codebook;
		friend class MetadataTagsQuery;

	private:
		//-------------------------------------------------------------------------
		// Construction
		//-------------------------------------------------------------------------

		/**
		 * @brief Constructs a MetadataTag object.
		 * @param name The name of the metadata tag (from the CodebookName enumeration).
		 * @param value The value of the metadata tag.
		 * @param isCustom Indicates whether the tag is custom (default is false).
		 */
		MetadataTag( CodebookName name, const std::string& value, bool isCustom = false );

	public:
		/** @brief Default constructor. */
		MetadataTag() = delete;

		/**
		 * @brief Copy constructor.
		 * @param other The object to copy from
		 */
		MetadataTag( const MetadataTag& other ) = default;

		/**
		 * @brief Move constructor.
		 * @param other The object to move from
		 */
		MetadataTag( MetadataTag&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor. */
		~MetadataTag() = default;

		//-------------------------------------------------------------------------
		// Assignment operators
		//-------------------------------------------------------------------------

		/**
		 * @brief Copy assignment operator.
		 * @param other The object to copy from
		 * @return Reference to this MetadataTag after assignment.
		 */
		MetadataTag& operator=( const MetadataTag& other ) = default;

		/**
		 * @brief Move assignment operator.
		 * @param other The object to move from
		 * @return Reference to this MetadataTag after assignment.
		 */
		MetadataTag& operator=( MetadataTag&& other ) noexcept = default;

		//-------------------------------------------------------------------------
		// Operators
		//-------------------------------------------------------------------------

		/**
		 * @brief Equality operator.
		 * Compares two MetadataTag objects for equality. Tags are considered equal if
		 * their names are the same and their values are equal.
		 * @param other The other MetadataTag to compare with this instance.
		 * @return True if the tags are equal, false otherwise.
		 * @throws std::invalid_argument If the CodebookName of the tags are different.
		 */
		inline bool operator==( const MetadataTag& other ) const;

		/**
		 * @brief Inequality operator.
		 * @param other The other MetadataTag to compare with this instance.
		 * @return True if the tags are not equal, false otherwise.
		 * @throws std::invalid_argument If the CodebookName of the tags are different (propagated from operator==).
		 */
		inline bool operator!=( const MetadataTag& other ) const;

		/**
		 * @brief Implicit conversion to a std::string.
		 * Returns the 'Value' part of the metadata tag.
		 * @return The 'Value' of the metadata tag as a string.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline operator std::string() const;

		//-------------------------------------------------------------------------
		// Accessors
		//-------------------------------------------------------------------------

		/**
		 * @brief Gets the name of the metadata tag.
		 * @return The CodebookName representing the name of the metadata tag.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline CodebookName name() const noexcept;

		/**
		 * @brief Gets the value of the metadata tag.
		 * @return A constant reference to the string value of the metadata tag.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::string_view value() const noexcept;

		/**
		 * @brief Gets the prefix character used for string representation of the metadata tag.
		 * @return '~' if the tag is custom, '-' otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline char prefix() const noexcept;

		//-------------------------------------------------------------------------
		// State inspection methods
		//-------------------------------------------------------------------------

		/**
		 * @brief Checks if the metadata tag is custom.
		 * @return True if the tag is custom, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isCustom() const noexcept;

		//-------------------------------------------------------------------------
		// String conversion methods
		//-------------------------------------------------------------------------

		/**
		 * @brief Converts the metadata tag to its string value.
		 * This method returns only the 'Value' part of the tag.
		 * For a fully formatted string including prefix and name, use toString(std::string&, char).
		 * @return The 'Value' of the metadata tag as a string.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const noexcept;

		// In MetadataTag.h:
		/**
		 * @brief Appends a fully formatted string representation of the metadata tag to a string builder.
		 * The format is: prefix + delimiter + value + separator
		 * where delimiter is '~' for custom tags or '-' for standard tags.
		 * @param builder The string to append to (capacity will be reserved automatically).
		 * @param separator The separator character to append after the value (default is '/').
		 * @throws std::invalid_argument If the CodebookName is not recognized for string conversion.
		 *
		 * @code
		 *     MetadataTag tag{CodebookName::Position, "centre", false};
		 *     std::string result;
		 *     tag.toString(result);  // result becomes "pos-centre/"
		 * @endcode
		 */
		void toString( nfx::string::StringBuilder& builder, char separator = '/' ) const;

		//----------------------------------------------
		// Hashing
		//----------------------------------------------

		/**
		 * @brief Gets the cached hash value for this MetadataTag instance
		 * @details **Purpose**: Enables MetadataTag instances to be used efficiently as keys in hash-based
		 *          collections (std::unordered_map, nfx::HashMap, etc.) without performance penalties.
		 *
		 *          **Performance Critical**: Hash is pre-computed during construction using optimized
		 *          NFX FNV-1a algorithms and cached to avoid expensive recomputation. This transforms
		 *          hash lookups from O(n) tag component processing to O(1) cached access.
		 *
		 *          **Implementation**: Combines hash codes of the CodebookName enum, string value,
		 *          and custom flag using NFX hash combination algorithms for consistent, high-quality
		 *          distribution across different tag configurations.
		 *
		 * @return The pre-computed hash value for this MetadataTag's complete tag state
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::size_t hashCode() const noexcept;

	private:
		//-------------------------------------------------------------------------
		// Private member variables
		//-------------------------------------------------------------------------

		/** @brief Cached hash value computed during construction for O(1) hash access */
		std::size_t m_hashCode;

		/** @brief The name of the metadata tag, represented by a CodebookName enum value. */
		CodebookName m_name;

		/** @brief A boolean flag indicating whether this is a custom tag (true) or a standard tag (false). */
		bool m_custom;

		/** @brief The string value associated with the metadata tag. */
		std::string m_value;
	};
} // namespace dnv::vista::sdk

#include "detail/MetadataTag.inl"

namespace std
{
	/**
	 * @brief Hash specialization for dnv::vista::sdk::MetadataTag
	 * @details Enables MetadataTag instances to be used as keys in all hash-based STL containers.
	 *          This specialization provides seamless integration with std::unordered_map,
	 *          std::unordered_set, and other standard library hash containers.
	 */
	template <>
	struct hash<dnv::vista::sdk::MetadataTag>
	{
		/**
		 * @brief Returns the cached hash value for optimal performance
		 * @param[in] tag The MetadataTag instance to hash
		 * @return Pre-computed hash value (O(1) access) combining name, value, and custom flag
		 */
		std::size_t operator()( const dnv::vista::sdk::MetadataTag& tag ) const noexcept
		{
			return tag.hashCode();
		}
	};
} // namespace std
