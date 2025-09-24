/**
 * @file ShipId.h
 * @brief Universal Ship Identifier with Type-Safe Discriminated Union
 *
 * @details
 * This file provides the **ShipId** class, a type-safe ship identifier that can represent
 * either an **IMO Number** (International Maritime Organization) or other string-based
 * ship identifiers using a discriminated union pattern.
 *
 * ## Purpose:
 *
 * **ShipId** serves as a universal ship identifier for:
 * - **Hybrid Construction**     : Public constructor for validated IMO numbers, private for custom IDs
 * - **IMO Number Support**      : Type-safe handling of official IMO numbers with validation
 * - **Alternative Identifiers** : Support for custom or legacy ship identification systems
 * - **Type Safety**             : Compile-time guarantees about identifier type through pattern matching
 * - **Memory Efficiency**       : Optimized storage using discriminated union pattern
 * - **Immutability**            : Value semantics with immutable design for thread safety
 *
 * ## Design Pattern:
 *
 * **ShipId** implements a **hybrid construction pattern**:
 * - **Public IMO Constructor**    : Direct construction from validated `ImoNumber` objects (safe)
 * - **Private String Constructor**: Controlled construction for custom IDs (requires validation)
 * - **Static Factory Method**     : `parse()` with automatic type detection for string input
 * - **Discriminated Union**       : Tag-based dispatch with type-safe access to union members
 * - **Immutable Design**          : Value semantics with thread-safe operations after construction
 * - **Pattern Matching**          : Functional-style access via `matchOn()` and `switchOn()` methods
 *
 * ## Usage Examples:
 *
 * ### Direct Construction from IMO Number (Recommended)
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### String Parsing with Type Detection
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Type Checking and Access
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Type Safety Features:
 *
 * - **Compile-Time Safety**: Pattern matching prevents accessing wrong union member
 * - **Optional Returns**   : Safe accessors return `std::optional` to handle empty cases
 * - **[[nodiscard]]**      : All query methods marked to prevent ignoring return values
 * - **Exception Safety**   : Constructor validation with clear error messages
 * - **Move Semantics**     : Efficient transfers with noexcept move operations
 *
 * ## Performance Characteristics:
 *
 * - **Memory Efficient** : Uses discriminated union to minimize storage overhead
 * - **Copy Optimized**   : Small object with efficient copy/move semantics
 * - **Inline Operations**: Type checking and accessors are fully inlined
 * - **String View**      : Non-owning string access where possible for zero-copy operations
 */

#pragma once

#include <functional>
#include <optional>
#include <string>
#include <string_view>

#include "dnv/vista/sdk/ImoNumber.h"

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// ShipId class
	//=====================================================================

	/**
	 * @brief Ship identifier using discriminated union pattern
	 * @details Immutable structure that can represent either an IMO number
	 *          or other string-based ship identifier. Uses a tag-based
	 *          discriminated union pattern similar to std::variant but with
	 *          explicit member access for performance.
	 */
	class ShipId final
	{
	private:
		//----------------------------------------------
		// Private Construction (requires validation)
		//----------------------------------------------

		/**
		 * @brief Constructs ShipId from string_view identifier (private - custom ID only, not IMO)
		 * @param otherId String-based ship identifier (custom/proprietary ID, NOT IMO number)
		 */
		inline explicit ShipId( std::string_view otherId ) noexcept;

	public:
		//----------------------------------------------
		// Public Construction (pre-validated input)
		//----------------------------------------------

		/**
		 * @brief Constructs ShipId from pre-validated IMO number
		 * @param imoNumber IMO number (already validated by ImoNumber constructor)
		 */
		inline explicit ShipId( ImoNumber imoNumber ) noexcept;

		/** @brief Default constructor */
		ShipId() = delete;

	/**
	 * @brief Copy constructor
	 * @param other The object to copy from
	 */
	ShipId( const ShipId& other ) = default;

	/**
	 * @brief Move constructor
	 * @param other The object to move from
	 */
	ShipId( ShipId&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~ShipId() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

	/**
	 * @brief Copy assignment operator
	 * @param other The object to copy from
	 * @return Reference to this ShipId after assignment
	 */
	ShipId& operator=( const ShipId& other ) = default;

	/**
	 * @brief Move assignment operator
	 * @param other The object to move from
	 * @return Reference to this ShipId after assignment
	 */
	ShipId& operator=( ShipId&& other ) noexcept = default;

		//----------------------------------------------
		// Equality operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison operator
		 * @param other The ShipId to compare with
		 * @return True if both ShipIds represent the same identifier
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator==( const ShipId& other ) const noexcept;

		/**
		 * @brief Inequality comparison operator
		 * @param other The ShipId to compare with
		 * @return True if ShipIds represent different identifiers
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator!=( const ShipId& other ) const noexcept;

		//----------------------------------------------
		// Type checking
		//----------------------------------------------

		/**
		 * @brief Check if this ShipId contains an IMO number
		 * @return True if this contains an IMO number
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isImoNumber() const noexcept;

		/**
		 * @brief Check if this ShipId contains an other identifier
		 * @return True if this contains an other identifier
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isOtherId() const noexcept;

		//----------------------------------------------
		// Value access
		//----------------------------------------------

		/**
		 * @brief Get IMO number if present
		 * @return Optional containing IMO number if present, empty otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::optional<ImoNumber> imoNumber() const noexcept;

		/**
		 * @brief Get other identifier if present
		 * @return Optional containing other identifier if present, empty otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::optional<std::string_view> otherId() const noexcept;

		//----------------------------------------------
		// Pattern matching
		//----------------------------------------------

		/**
		 * @brief Pattern match on ShipId content
		 * @tparam T Return type
		 * @param onImoNumber Function to call if ShipId contains IMO number
		 * @param onOtherId Function to call if ShipId contains other identifier
		 * @return Result of the matched function
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename T>
		[[nodiscard]] inline T matchOn(
			std::function<T( const ImoNumber& imo )> onImoNumber,
			std::function<T( std::string_view otherId )> onOtherId ) const;

		/**
		 * @brief Pattern switch on ShipId content (void version)
		 * @param onImoNumber Function to call if ShipId contains IMO number
		 * @param onOtherId Function to call if ShipId contains other identifier
		 */
		inline void switchOn(
			std::function<void( const ImoNumber& imo )> onImoNumber,
			std::function<void( std::string_view otherId )> onOtherId ) const;

		//----------------------------------------------
		// String conversion
		//----------------------------------------------

		/**
		 * @brief Convert to string representation
		 * @return String representation of the ship identifier
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/**
		 * @brief Parse ShipId from string representation with automatic type detection
		 * @param value String to parse - detects IMO numbers starting with "IMO" prefix, treats others as string identifiers
		 * @return Parsed ShipId containing either IMO number (if detected) or string identifier
		 * @throws std::invalid_argument If value is empty
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 * @details Parsing logic: If value starts with "IMO" and is valid IMO number format, creates IMO-type ShipId.
		 *          Otherwise, creates string identifier type ShipId. Implements ISO-19848 convention.
		 */
		[[nodiscard]] static ShipId parse( std::string_view value );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		/** @brief Tag enumeration for discriminated union */
		enum class Tag : bool
		{
			IMO = 0,
			Other
		};

		/** @brief Tag to identify which union member is active */
		Tag m_tag;

		/** @brief IMO number storage (active when m_tag == Tag::IMO) */
		std::optional<ImoNumber> m_imoNumber;

		/** @brief Other identifier storage (active when m_tag == Tag::Other) */
		std::optional<std::string> m_otherId;
	};
}

#include "dnv/vista/sdk/detail/transport/ShipId.inl"
