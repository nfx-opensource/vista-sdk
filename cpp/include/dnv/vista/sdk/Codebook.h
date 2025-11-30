/**
 * @file Codebook.h
 * @brief VISTA Codebook Management System for Maritime Data Validation
 *
 * @details
 * This file implements the **VISTA Codebook System** for validating and managing
 * standardized maritime data vocabularies. It provides comprehensive validation,
 * lookup capabilities, and metadata management for VISTA standard codebooks.
 *
 * ## System Purpose:
 *
 * The **VISTA Codebook System** serves as the foundation for:
 * - **Data Validation**    : Ensuring maritime data conforms to standardized vocabularies
 * - **Standard Compliance**: Enforcing VISTA naming conventions and value standards
 * - **Metadata Management**: Creating and validating metadata tags for data classification
 * - **Position Validation**: Specialized validation for maritime position and location data
 * - **Group Organization** : Managing hierarchical groupings of related codebook values
 *
 * ## Core Architecture:
 *
 * ### Codebook Classes
 * - **Codebook**                 : Main class containing complete codebook with validation logic
 * - **CodebookStandardValues**   : Container for standardized vocabulary values
 * - **CodebookGroups**           : Container for hierarchical groupings of related values
 * - **PositionValidationResults**: Specialized validation results for position data
 *
 * ### Validation Framework
 * - **Value Validation**     : Check if values exist in standard vocabularies
 * - **Group Validation**     : Verify group membership and hierarchical relationships
 * - **Position Validation**  : Specialized ISO string format validation for positions
 * - **Metadata Tag Creation**: Generate validated metadata tags from codebook values
 *
 * ## Data Flow Architecture:
 *
 * ```
 *       CodebookDto (External Data)
 *                  ↓
 *         Codebook Construction
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │            Codebook                 │
 * ├─────────────────────────────────────┤
 * │ ┌─────────────────────────────────┐ │
 * │ │    CodebookStandardValues       │ │ ← Standard vocabulary lookup
 * │ │  (StringSet for O(1) lookup)    │ │
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │        CodebookGroups           │ │ ← Hierarchical organization
 * │ │  (StringSet for O(1) lookup)    │ │
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │       Raw Data Mapping          │ │ ← Group → Values mapping
 * │ │   (StringMap<vector<string>>)   │ │
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *                  ↓
 *      Validation & Metadata Creation
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │      Validation Operations          │
 * ├─────────────────────────────────────┤
 * │ - O(1) Value lookup validation      │
 * │ - O(1) Group membership testing     │
 * │ - Position format validation        │
 * │ - MetadataTag creation              │
 * │ - ISO string compliance checks      │
 * └─────────────────────────────────────┘
 *```
 *
 * ## Usage Examples:
 *
 * TODO
 *
 * ## Performance Characteristics:
 *
 * - **O(1) Lookups**    : Hash-based containers for constant-time value and group checks
 * - **Zero-Copy Access**: `string_view` interfaces minimize memory allocation
 * - **Memory Efficient**: Optimized containers with minimal overhead
 * - **Thread Safe**     : Immutable design safe for concurrent read access
 * - **Iterator Support**: STL-compatible iterators for standard algorithms
 *
 * ## Validation Levels:
 *
 * ### Standard Value Validation
 * - **Exact Match**      : Direct lookup in standardized vocabulary
 * - **Case Sensitivity** : Enforces exact case matching for consistency
 * - **Numeric Positions**: Special handling for numeric position identifiers
 *
 * ### Position Validation
 * - **Format Validation**  : ISO string format compliance (hyphen-separated)
 * - **Order Validation**   : Correct sequence of position components
 * - **Grouping Validation**: Valid component combinations and relationships
 * - **Custom Extensions**  : Support for custom position extensions
 *
 * ### Group Validation
 * - **Membership Testing**   : Verify values belong to specific groups
 * - **Hierarchical Checking**: Navigate parent-child group relationships
 * - **Group Discovery**      : Find all groups containing specific values
 *
 * ## Design Philosophy:
 *
 * - **Standards Compliance**: Full adherence to VISTA codebook specifications
 * - **Performance Focus**   : Optimized for high-frequency validation operations
 * - **Type Safety**         : Strong typing with comprehensive error handling
 * - **Extensibility**       : Support for custom codebooks and validation rules
 * - **Usability**           : Clear, intuitive API for common validation scenarios
 * - **Immutability**        : Thread-safe design with immutable data structures
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <nfx/Containers.h>

#include "CodebookName.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	struct CodebookDto;
	class MetadataTag;

	//=====================================================================
	// PositionValidationResults class
	//=====================================================================

	/**
	 * @enum PositionValidationResult
	 * @brief Enumerates the possible outcomes of validating a position string against a codebook.
	 */
	enum class PositionValidationResult
	{
		Invalid = 0,
		InvalidOrder,
		InvalidGrouping,

		Valid = 100,

		Custom = 101
	};

	/**
	 * @class PositionValidationResults
	 * @brief Provides utility functions related to `PositionValidationResult`.
	 * @details This class currently offers conversion from string representations.
	 */
	class PositionValidationResults final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor. */
		PositionValidationResults() = delete;

		/** @brief Copy constructor */
		PositionValidationResults( const PositionValidationResults& ) = delete;

		/** @brief Move constructor */
		PositionValidationResults( PositionValidationResults&& ) noexcept = delete;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~PositionValidationResults() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		PositionValidationResults& operator=( const PositionValidationResults& ) = delete;

		/** @brief Move assignment operator */
		PositionValidationResults& operator=( PositionValidationResults&& ) noexcept = delete;

		//----------------------------------------------
		// Static utility methods
		//----------------------------------------------

		/**
		 * @brief Converts a string representation to a PositionValidationResult.
		 * @param[in] name The string name of the validation result (case-insensitive).
		 * @return The corresponding PositionValidationResult enum value.
		 * @throws std::invalid_argument If the name doesn't match any known validation result.
		 *
		 * @details Supported string values:
		 * - "Invalid" -> PositionValidationResult::Invalid
		 * - "InvalidOrder" -> PositionValidationResult::InvalidOrder
		 * - "InvalidGrouping" -> PositionValidationResult::InvalidGrouping
		 * - "Valid" -> PositionValidationResult::Valid
		 * - "Custom" -> PositionValidationResult::Custom
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static PositionValidationResult fromString( std::string_view name );
	};

	//=====================================================================
	// CodebookStandardValues class
	//=====================================================================

	/**
	 * @brief Container for standard values of a codebook
	 */
	class CodebookStandardValues final
	{
	public:
		//----------------------------------------------
		// Public Types
		//----------------------------------------------

		/**
		 * @brief Iterator type for traversing standard values
		 */
		using Iterator = decltype( std::declval<const nfx::containers::FastHashSet<std::string>>().begin() );

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct with name and values
		 * @param name The codebook name
		 * @param standardValues The set of standard values with zero-copy string_view access
		 */
		inline explicit CodebookStandardValues( CodebookName name, nfx::containers::FastHashSet<std::string>&& standardValues ) noexcept;

		/** @brief Default constructor. */
		CodebookStandardValues() = default;

		/**
		 * @brief Copy constructor
		 * @param other The object to copy from
		 */
		CodebookStandardValues( const CodebookStandardValues& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The object to move from
		 */
		CodebookStandardValues( CodebookStandardValues&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~CodebookStandardValues() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The object to copy from
		 * @return Reference to this object
		 */
		CodebookStandardValues& operator=( const CodebookStandardValues& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The object to move from
		 * @return Reference to this object
		 */
		CodebookStandardValues& operator=( CodebookStandardValues&& other ) noexcept = default;

		//----------------------------------------------
		// Public methods
		//----------------------------------------------

		/**
		 * @brief Get the number of standard values
		 * @return The count of standard values
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline size_t count() const noexcept;

		/**
		 * @brief Check if a value is contained in standard values (string_view overload)
		 * @param tagValue The value to check
		 * @return True if the value is in standard values or is a numeric position
		 */
		inline bool contains( std::string_view tagValue ) const noexcept;

		//----------------------------------------------
		// Iteration
		//----------------------------------------------

		/**
		 * @brief Get iterator to the beginning
		 * @return Iterator to the first standard value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Iterator begin() const noexcept;

		/**
		 * @brief Get iterator to the end
		 * @return Iterator past the last standard value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Iterator end() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief The name of the codebook */
		CodebookName m_name;

		/** @brief The set of standard values */
		nfx::containers::FastHashSet<std::string> m_standardValues;
	};

	//=====================================================================
	// CodebookGroups class
	//=====================================================================

	/**
	 * @class CodebookGroups
	 * @brief A container managing the set of group names defined within a codebook.
	 * @details Provides efficient lookup (`contains`) and iteration over the group names.
	 */
	class CodebookGroups final
	{
	public:
		//----------------------------------------------
		// Public Types
		//----------------------------------------------

		/**
		 * @brief Iterator type for traversing groups
		 */
		using Iterator = nfx::containers::FastHashSet<std::string>::const_iterator;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct with groups
		 * @param groups The set of groups with zero-copy string_view access
		 */
		inline explicit CodebookGroups( nfx::containers::FastHashSet<std::string>&& groups ) noexcept;

		/** @brief Default constructor. */
		CodebookGroups() = default;

		/**
		 * @brief Copy constructor
		 * @param other The object to copy from
		 */
		CodebookGroups( const CodebookGroups& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The object to move from
		 */
		CodebookGroups( CodebookGroups&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~CodebookGroups() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The object to copy from
		 * @return Reference to this object
		 */
		CodebookGroups& operator=( const CodebookGroups& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The object to move from
		 * @return Reference to this object
		 */
		CodebookGroups& operator=( CodebookGroups&& other ) noexcept = default;

		//----------------------------------------------
		// Public methods
		//----------------------------------------------

		/**
		 * @brief Get the number of groups
		 * @return The count of groups
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline size_t count() const noexcept;

		/**
		 * @brief Check if a group is contained (string_view overload)
		 * @param group The group to check (as a `std::string_view`)
		 * @return True if the group exists
		 */
		inline bool contains( std::string_view group ) const noexcept;

		//----------------------------------------------
		// Iteration
		//----------------------------------------------

		/**
		 * @brief Get iterator to the beginning
		 * @return Iterator to the first group
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Iterator begin() const noexcept;

		/**
		 * @brief Get iterator to the end
		 * @return Iterator past the last group
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Iterator end() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief The set of groups */
		nfx::containers::FastHashSet<std::string> m_groups;
	};

	//=====================================================================
	// Codebook class
	//=====================================================================

	/**
	 * @class Codebook
	 * @brief Represents a complete VISTA codebook, containing standard values, groups, and validation logic.
	 * @details This class aggregates standard values and group information, providing methods
	 * to validate data, check for the existence of values/groups, and create associated `MetadataTag` objects.
	 * It is typically constructed from a `CodebookDto` object.
	 */
	class Codebook final
	{
		//----------------------------------------------
		// Friend class declarations
		//----------------------------------------------

		friend class Codebooks;
		friend class VIS;

	private:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct from DTO
		 * @param dto The data transfer object
		 * @throws std::invalid_argument If the DTO has an unknown name
		 */
		explicit Codebook( const CodebookDto& dto );

		/** @brief Default constructor. */
		Codebook() = default;

	public:
		/**
		 * @brief Copy constructor
		 * @param other The object to copy from
		 */
		Codebook( const Codebook& other );

		/**
		 * @brief Move constructor
		 * @param other The object to move from
		 */
		Codebook( Codebook&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~Codebook() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The object to copy from
		 * @return Reference to this object
		 */
		Codebook& operator=( const Codebook& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The object to move from
		 * @return Reference to this object
		 */
		Codebook& operator=( Codebook&& other ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the codebook name
		 * @return The codebook name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr CodebookName name() const noexcept;

		/**
		 * @brief Get the groups
		 * @return Reference to the groups container
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const CodebookGroups& groups() const noexcept;

		/**
		 * @brief Get the standard values
		 * @return Reference to the standard values container
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const CodebookStandardValues& standardValues() const noexcept;

		/**
		 * @brief Get the raw data
		 * @return Map of groups to their values with zero-copy string_view access
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const nfx::containers::FastHashMap<std::string, std::vector<std::string>>& rawData() const noexcept;

		//----------------------------------------------
		// State inspection methods
		//----------------------------------------------

		/**
		 * @brief Check if a group exists
		 * @param group The group to check
		 * @return True if the group exists
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool hasGroup( std::string_view group ) const noexcept;

		/**
		 * @brief Check if a value is a standard value
		 * @param value The value to check
		 * @return True if the value is standard
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool hasStandardValue( std::string_view value ) const noexcept;

		//----------------------------------------------
		// Metadata tag creation
		//----------------------------------------------

		/**
		 * @brief Try to create a metadata tag
		 * @param valueView The tag value
		 * @return The metadata tag, or none if invalid
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::optional<MetadataTag> tryCreateTag( std::string_view valueView ) const;

		/**
		 * @brief Create a metadata tag
		 * @param value The tag value
		 * @return The metadata tag
		 * @throws std::invalid_argument If the value is invalid
		 */
		MetadataTag createTag( std::string_view value ) const;

		//----------------------------------------------
		// Position validation
		//----------------------------------------------

		/**
		 * @brief Validates a position string according to the rules defined for position codebooks.
		 * @details This method specifically applies validation logic relevant to position data
		 *          (e.g., ISO string format, hyphen separation, order, grouping).
		 *          It should only be called on `Codebook` instances representing position codebooks
		 *          (i.e., where `name()` returns `CodebookName::Position`).
		 * @param[in] position The position string to validate (as a `std::string_view`).
		 * @return A `PositionValidationResult` indicating the outcome of the validation.
		 * @warning Behavior is undefined if called on a non-position codebook.
		 */
		PositionValidationResult validatePosition( std::string_view position ) const;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief The name of this codebook */
		CodebookName m_name;

		/** @brief Mapping from values to their group names */
		nfx::containers::FastHashMap<std::string, std::string> m_groupMap;

		/** @brief Container for standard values */
		CodebookStandardValues m_standardValues;

		/** @brief Container for groups */
		CodebookGroups m_groups;

		/** @brief Raw mapping of groups to their values */
		nfx::containers::FastHashMap<std::string, std::vector<std::string>> m_rawData;
	};
} // namespace dnv::vista::sdk

#include "detail/Codebook.inl"
