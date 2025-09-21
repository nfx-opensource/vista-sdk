/**
 * @file UniversalIdBuilder.h
 * @brief VISTA Universal ID Builder System for Fluent Global Maritime ID Construction
 *
 * @details
 * This file implements the **VISTA Universal ID Builder System** for constructing and validating
 * Universal ID instances through an immutable fluent interface pattern. It provides comprehensive
 * builder capabilities combining IMO numbers with Local IDs to create globally unique maritime
 * data identifiers that prevent conflicts across worldwide vessel operations.
 *
 * ## System Purpose:
 *
 * The **VISTA Universal ID Builder System** serves as the foundation for:
 * - **Global ID Construction**: Step-by-step Universal ID building with method chaining
 * - **Conflict Prevention**   : Ensuring unique identification across worldwide maritime fleet
 * - **Validation Integration**: Built-in validation at each construction step
 * - **Immutable Pattern**     : Each builder operation returns a new instance
 * - **Parsing Support**       : Complete Universal ID string-to-builder conversion
 * - **Standards Compliance**  : Full adherence to VIS Universal ID specifications
 *
 * ## Core Architecture:
 *
 * ### Builder Pattern Implementation
 * - **UniversalIdBuilder**: Main fluent interface for constructing Universal IDs
 * - **Immutable Design**  : Each method returns new builder instance (no mutation)
 * - **Validation Gates**  : State validation at construction boundaries
 * - **Factory Pattern**   : Private constructors enforce controlled creation
 *
 * ### Construction Flow
 * - **Static Factory**    : `create(VisVersion)` initializes builder with Local ID foundation
 * - **Fluent Interface**  : Method chaining for readable construction
 * - **Component Assembly**: Combining IMO number with Local ID builder
 * - **Build Validation**  : Final validation before Universal ID creation
 *
 * ## Memory Layout & Performance:
 *
 * ```
 * UniversalIdBuilder Structure:
 * ┌─────────────────────────────────────┐
 * │        UniversalIdBuilder           │
 * ├─────────────────────────────────────┤
 * │ std::optional<ImoNumber>            │ ← 8 bytes (7-digit + validity flag)
 * │ ┌─────────────────────────────────┐ │
 * │ │   std::optional<LocalIdBuilder> │ │ ← Local ID construction state
 * │ │  ┌───────────────────────────┐  │ │
 * │ │  │    LocalIdItems           │  │ │ ← GMOD path components
 * │ │  │    Metadata Tags          │  │ │
 * │ │  │    VIS Version            │  │ │
 * │ │  │    Verbose Mode           │  │ │
 * │ │  └───────────────────────────┘  │ │
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *
 * Key Performance Features:
 * - Immutable copy-on-write semantics
 * - Optional wrapping minimizes memory overhead
 * - Move semantics for expensive Local ID operations
 * - Stack allocation for builder instances
 * ```
 *
 * ## Usage Patterns:
 *
 * ### Basic Universal ID Construction
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Advanced Construction with Validation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Parsing and Error Handling
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Component Access and Analysis
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Performance Characteristics:
 *
 * - **Builder Creation** : O(1) static factory method with LocalIdBuilder initialization
 * - **Method Chaining**  : O(1) per operation with immutable copy semantics
 * - **Component Setting**: O(1) for IMO number, O(n) for Local ID operations
 * - **Validation**       : O(1) state checks with early failure detection
 * - **Build Operation**  : O(1) construction of final Universal ID
 * - **String Parsing**   : O(n) where n is input string length
 * - **Memory Efficiency**: Optional wrapping minimizes overhead for unset components
 *
 * ## Validation and State Management:
 *
 * ### Builder State Validation
 * - **IMO Number Required**  : Valid 7-digit IMO number must be set
 * - **Local ID Required**    : Valid LocalIdBuilder with primary item must be present
 * - **Component Consistency**: VIS version compatibility between Universal and Local components
 * - **Early Validation**     : State checks prevent invalid Universal ID construction
 *
 * ### Error Handling Patterns
 * - **Exception Methods**: `withImoNumber()`, `withLocalId()` throw on invalid input
 * - **Try Methods**      : `tryWithImoNumber()`, `tryWithLocalId()` return success status
 * - **Parsing Errors**   : Detailed parsing error reporting through ParsingErrors
 * - **State Validation** : `isValid()` checks completeness before build()
 *
 * ## Integration with Maritime Systems:
 *
 * ### Global Fleet Management
 * - **Vessel Identification**: IMO number provides globally unique vessel identity
 * - **Data Point Resolution**: Local ID resolves to specific vessel systems and sensors
 * - **Cross-System Mapping** : Universal IDs enable seamless multi-vessel data correlation
 * - **Regulatory Compliance**: Full traceability for maritime regulatory requirements
 *
 * ### Builder Pattern Benefits
 * - **Incremental Construction**: Step-by-step building with validation at each stage
 * - **Immutable Safety**        : Thread-safe operations with no shared mutable state
 * - **Parsing Integration**     : Direct conversion from string representations
 * - **Fluent Interface**        : Readable, self-documenting construction code
 *
 * ## Design Philosophy:
 *
 * - **Global Uniqueness**   : IMO + Local ID combination ensures worldwide uniqueness
 * - **Immutability First**  : No builder mutation, always return new instances
 * - **Controlled Creation** : Private constructors enforce factory pattern usage
 * - **Standards Compliance**: Full adherence to VIS Universal ID specifications
 * - **Type Safety**         : Strong typing prevents invalid Universal ID construction
 * - **Maritime Domain**     : Tailored for real-world vessel operations and data flows
 * - **Performance Focus**   : Optimized for high-frequency maritime data processing
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "ImoNumber.h"
#include "LocalIdBuilder.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	class ParsingErrors;
	class UniversalId;
	enum class VisVersion : std::uint16_t;

	namespace internal
	{
		class LocalIdParsingErrorBuilder;
		enum class LocalIdParsingState : std::uint8_t;
	}

	//=====================================================================
	// UniversalIdBuilder class
	//=====================================================================

	/**
	 * @class UniversalIdBuilder
	 * @brief Fluent builder for UniversalId objects.
	 *
	 * @details Concrete implementation using immutable fluent pattern with direct value storage.
	 */
	class UniversalIdBuilder final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

	private:
		/** @brief Default constructor */
		UniversalIdBuilder() = default;

	public:
		/** @brief Copy constructor */
		UniversalIdBuilder( const UniversalIdBuilder& ) = default;

		/** @brief Move constructor */
		UniversalIdBuilder( UniversalIdBuilder&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~UniversalIdBuilder() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		UniversalIdBuilder& operator=( const UniversalIdBuilder& ) = delete;

		/** @brief Move assignment operator */
		UniversalIdBuilder& operator=( UniversalIdBuilder&& other ) noexcept;

		//----------------------------------------------
		// Operators
		//----------------------------------------------

		/**
		 * @brief Equality operator.
		 * @param[in] other The other builder to compare.
		 * @return True if equal, false otherwise.
		 */
		inline bool operator==( const UniversalIdBuilder& other ) const;

		/**
		 * @brief Inequality operator.
		 * @param[in] other The other builder to compare.
		 * @return True if not equal, false otherwise.
		 */
		inline bool operator!=( const UniversalIdBuilder& other ) const;

		/**
		 * @brief Deep equality comparison with another builder.
		 * @param[in] other The other builder to compare.
		 * @return True if all components are equal.
		 */
		inline bool equals( const UniversalIdBuilder& other ) const;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Gets the IMO number currently set in builder.
		 * @return Constant reference to optional IMO number.
		 * @note Zero-copy access via const reference.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<ImoNumber>& imoNumber() const noexcept;

		/**
		 * @brief Gets the Local ID builder currently set.
		 * @return Constant reference to optional LocalIdBuilder.
		 * @note Zero-copy access via const reference.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<LocalIdBuilder>& localId() const noexcept;

		//----------------------------------------------
		// State inspection methods
		//----------------------------------------------

		/**
		 * @brief Checks if builder state is valid for building UniversalId.
		 * @details Requires both IMO number and valid LocalIdBuilder.
		 * @return True if build() will succeed.
		 * @note This method is guaranteed not to throw.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isValid() const noexcept;

		//----------------------------------------------
		// String conversion
		//----------------------------------------------

		/**
		 * @brief Generates string representation of current builder state.
		 * @return String in Universal ID format.
		 * @throws std::invalid_argument If builder state is invalid.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/**
		 * @brief Creates new UniversalIdBuilder for specified VIS version.
		 * @param[in] version VIS version for the Local ID component.
		 * @return New UniversalIdBuilder instance.
		 */
		static UniversalIdBuilder create( VisVersion version );

		//----------------------------------------------
		// Build methods (Immutable fluent interface)
		//----------------------------------------------

		/**
		 * @brief Builds UniversalId from current builder state.
		 * @return Constructed UniversalId.
		 * @throws std::invalid_argument If builder state is invalid.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] UniversalId build() const;

		//----------------------------
		// Local Id
		//----------------------------

		/**
		 * @brief Returns new builder with Local ID builder set.
		 * @param[in] localId LocalIdBuilder to set.
		 * @return New UniversalIdBuilder instance.
		 * @throws std::invalid_argument If localId is invalid.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] UniversalIdBuilder withLocalId( const LocalIdBuilder& localId ) const;

		/**
		 * @brief Returns new builder with optional Local ID builder.
		 * @param[in] localId Optional LocalIdBuilder to set.
		 * @return New UniversalIdBuilder instance.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] UniversalIdBuilder tryWithLocalId( const std::optional<LocalIdBuilder>& localId ) const;

		/**
		 * @brief Returns new builder with optional Local ID builder, reporting success.
		 * @param[in] localId Optional LocalIdBuilder to set.
		 * @param[out] succeeded True if LocalIdBuilder was set successfully.
		 * @return New UniversalIdBuilder instance.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] UniversalIdBuilder tryWithLocalId( const std::optional<LocalIdBuilder>& localId, bool& succeeded ) const;

		/**
		 * @brief Returns new builder without Local ID builder.
		 * @return New UniversalIdBuilder instance.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] UniversalIdBuilder withoutLocalId() const;

		//----------------------------
		// IMO number
		//----------------------------

		/**
		 * @brief Returns new builder with IMO number set.
		 * @param[in] imoNumber IMO number to set.
		 * @return New UniversalIdBuilder instance.
		 * @throws std::invalid_argument If imoNumber is invalid.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] UniversalIdBuilder withImoNumber( const ImoNumber& imoNumber ) const;

		/**
		 * @brief Returns new builder with optional IMO number.
		 * @param[in] imoNumber Optional IMO number to set.
		 * @return New UniversalIdBuilder instance.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] UniversalIdBuilder tryWithImoNumber( const std::optional<ImoNumber>& imoNumber ) const;

		/**
		 * @brief Returns new builder with optional IMO number, reporting success.
		 * @param[in] imoNumber Optional IMO number to set.
		 * @param[out] succeeded True if IMO number was set successfully.
		 * @return New UniversalIdBuilder instance.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] UniversalIdBuilder tryWithImoNumber( const std::optional<ImoNumber>& imoNumber, bool& succeeded ) const;

		/**
		 * @brief Returns new builder without IMO number.
		 * @return New UniversalIdBuilder instance.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] UniversalIdBuilder withoutImoNumber() const;

		//----------------------------------------------
		// Static parsing methods
		//----------------------------------------------

		/**
		 * @brief Parses UniversalIdBuilder from string representation.
		 * @param[in] universalIdStr String to parse.
		 * @return Parsed UniversalIdBuilder.
		 * @throws std::invalid_argument If parsing fails.
		 */
		static UniversalIdBuilder parse( std::string_view universalIdStr );

		/**
		 * @brief Attempts to parse UniversalIdBuilder from string.
		 * @param[in] universalIdStr String to parse.
		 * @param[out] universalIdBuilder Parsed result if successful.
		 * @return True if parsing succeeded.
		 */
		static bool tryParse( std::string_view universalIdStr, std::optional<UniversalIdBuilder>& universalIdBuilder );

		/**
		 * @brief Attempts to parse UniversalIdBuilder from string with error reporting.
		 * @param[in] universalIdStr String to parse.
		 * @param[out] errors Parsing errors if unsuccessful.
		 * @param[out] universalIdBuilder Parsed result if successful.
		 * @return True if parsing succeeded.
		 */
		static bool tryParse(
			std::string_view universalIdStr,
			ParsingErrors& errors,
			std::optional<UniversalIdBuilder>& universalIdBuilder );

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		std::optional<LocalIdBuilder> m_localIdBuilder;
		std::optional<ImoNumber> m_imoNumber;
	};
}

#include "detail/UniversalIdBuilder.inl"
