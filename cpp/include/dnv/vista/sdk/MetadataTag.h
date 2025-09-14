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
 * - **Data Classification**: Standardized tagging for maritime data categorization
 * - **Metadata Validation**: Ensuring tags conform to VIS codebook specifications
 * - **Tag Serialization**  : Converting tags to string representations for storage/transmission
 * - **Custom Extensions**  : Supporting custom tags while maintaining standard compliance
 * - **Query Integration**  : Enabling efficient metadata-based data queries and filtering
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
 * - **Name Component** : Codebook-based standardized name (e.g., Position, Detail)
 * - **Value Component**: String value conforming to codebook vocabulary
 * - **Custom Flag**    : Indicates standard vs. custom tag classification
 * - **Prefix System**  : Visual differentiation between standard (-) and custom (~) tags
 *
 * ## Data Flow Architecture:
 *
 * ```
 * Codebook Validation
 *         ↓
 * MetadataTag Creation
 *         ↓
 * ┌─────────────────────────────────────┐
 * │           MetadataTag               │
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
 *         ↓
 * String Serialization & Usage
 * ```
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
 * - **Immutable Design**  : Thread-safe operations with no post-construction changes
 * - **Zero-Copy Access**  : `string_view` interfaces minimize memory allocation
 * - **Efficient Storage** : Compact representation with enum-based names
 * - **Fast Comparison**   : Optimized equality operations for tag matching
 * - **Lazy Serialization**: String conversion only when explicitly requested
 *
 * ## Tag Classification System:
 *
 * ### Standard Tags
 * - **Validation Required**: Must conform to codebook vocabulary standards
 * - **Prefix Character**   : '-' (hyphen) for visual identification
 * - **Name Validation**    : CodebookName must exist in VIS standards
 * - **Value Validation**   : Value must be valid according to codebook rules
 *
 * ### Custom Tags
 * - **Extended Vocabulary**: Support for domain-specific or custom values
 * - **Prefix Character**   : '~' (tilde) for visual identification
 * - **Relaxed Validation** : Value validation may be less strict
 * - **Compatibility**      : Maintains interoperability with standard systems
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
 * - **Standards Compliance**: Full adherence to VIS metadata specifications
 * - **Type Safety**         : Strong typing prevents invalid tag construction
 * - **Performance Focus**   : Optimized for high-frequency maritime data operations
 * - **Immutability**        : Thread-safe design with immutable tag objects
 * - **Extensibility**       : Support for custom tags while maintaining compatibility
 * - **Usability**           : Clear, intuitive API for common metadata operations
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include <nfx/string/StringBuilderPool.h>

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

		/** @brief Copy constructor. */
		MetadataTag( const MetadataTag& ) = default;

		/** @brief Move constructor. */
		MetadataTag( MetadataTag&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor. */
		~MetadataTag() = default;

		//-------------------------------------------------------------------------
		// Assignment operators
		//-------------------------------------------------------------------------

		/** @brief Copy assignment operator. */
		MetadataTag& operator=( const MetadataTag& ) = default;

		/** @brief Move assignment operator. */
		MetadataTag& operator=( MetadataTag&& ) noexcept = default;

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

	private:
		//-------------------------------------------------------------------------
		// Private member variables
		//-------------------------------------------------------------------------

		/** @brief The name of the metadata tag, represented by a CodebookName enum value. */
		CodebookName m_name;

		/** @brief A boolean flag indicating whether this is a custom tag (true) or a standard tag (false). */
		bool m_custom;

		/** @brief The string value associated with the metadata tag. */
		std::string m_value;
	};
}

#include "MetadataTag.inl"
