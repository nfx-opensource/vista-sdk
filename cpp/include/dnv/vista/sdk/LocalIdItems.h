/**
 * @file LocalIdItems.h
 * @brief VISTA Local ID Items Container for Primary and Secondary GMOD Path Storage
 *
 * @details
 * This file implements the **VISTA LocalIdItems System** for internal storage and management
 * of primary and secondary GMOD paths within LocalId instances. It provides a private container
 * design ensuring controlled access through the LocalIdBuilder fluent interface.
 *
 * ## System Purpose:
 *
 * The **VISTA LocalIdItems Container** serves as the internal foundation for:
 * - **Dual Path Storage**     : Primary and optional secondary GMOD path management
 * - **Immutable Design**      : Thread-safe storage with no post-construction modification
 * - **Controlled Access**     : Exclusive access through LocalIdBuilder friend class interface
 * - **Move-Only Semantics**   : Efficient path ownership transfer with zero-copy construction
 * - **String Serialization**  : Unified path-to-string conversion with verbose mode support
 * - **Type-Safe Construction**: Private constructors prevent invalid state creation
 * - **Memory Efficient**      : Minimal overhead with optional<GmodPath> storage model
 *
 * ## Core Architecture:
 *
 * ### Storage Model
 * - **Primary Path**    : Required GMOD path representing the main component or data point
 * - **Secondary Path**  : Optional GMOD path for additional context or related components
 * - **Optional Wrapper**: Both paths use std::optional for uniform handling and null safety
 * - **Immutable State** : No modification allowed after construction ensuring thread safety
 * - **Friend Access**   : Private interface accessible only through LocalIdBuilder
 *
 * ## Memory Layout & Performance:
 *
 * ```
 * LocalIdItems Container Structure:
 * ┌─────────────────────────────────────┐
 * │          LocalIdItems               │
 * ├─────────────────────────────────────┤
 * │ ┌─────────────────────────────────┐ │
 * │ │  std::optional<GmodPath>        │ │ ← Primary path storage (required component)
 * │ │      m_primaryItem              │ │
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │  std::optional<GmodPath>        │ │ ← Secondary path storage (optional context)
 * │ │     m_secondaryItem             │ │
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *                  ↓
 * Path Access & String Operations
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │    Container Access Operations      │
 * ├─────────────────────────────────────┤
 * │ - O(1) primary/secondary access     │
 * │ - Move-only construction patterns   │
 * │ - Friend class controlled access    │
 * │ - StringBuilder serialization       │
 * │ - Immutable read-only operations    │
 * └─────────────────────────────────────┘
 *```
 *
 * ## Usage Patterns:
 *
 * ### Builder-Mediated Construction
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Performance Characteristics:
 *
 * - **Construction**     : O(1) move operations for efficient path ownership transfer
 * - **Access Operations**: O(1) direct member access through const reference returns
 * - **Memory Footprint** : Minimal overhead with std::optional<GmodPath> storage model
 * - **Thread Safety**    : Immutable read-only operations safe for concurrent access
 * - **Move Semantics**   : Zero-copy construction and manipulation throughout lifecycle
 * - **String Building**  : O(n) serialization where n is total path string length
 * - **Comparison Ops**   : O(n) equality comparison delegated to GmodPath operators Secondary GMOD Path Storage
 *
 * ## Design Philosophy:
 *
 * - **Encapsulation First** : Private constructors with friend class access ensure controlled creation
 * - **Move-Only Efficiency**: Leverages GmodPath move semantics for optimal performance
 * - **Immutable Container** : Thread-safe design with guaranteed no post-construction changes
 * - **Builder Integration** : Seamless integration with LocalIdBuilder fluent interface
 * - **Type-Safe Storage**   : std::optional wrapper provides null safety and uniform handling
 * - **Standards Compliance**: Consistent with VIS maritime data identification standards
 * - **Minimal Interface**   : Simple, focused API with essential operations only
 *
 * ## Performance Characteristics:
 *
 * - **Construction**  : O(1) move operations for path ownership transfer
 * - **Access**        : O(1) direct member access through friend interface
 * - **Memory**        : Minimal overhead with optional<GmodPath> storage
 * - **Thread Safety** : Read-only operations safe for concurrent access
 * - **Move Semantics**: Zero-copy construction and manipulation
 */

#pragma once

#include <optional>

#include <nfx/string/StringBuilder.h>

#include "GmodPath.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// LocalIdItems class
	//=====================================================================

	/**
	 * @brief Immutable structure representing primary and optional secondary GMOD items for a LocalId.
	 *
	 * This class stores primary and secondary GmodPath items. It is designed to be
	 * immutable after construction.
	 */
	class LocalIdItems final
	{
		friend class LocalIdBuilder;

	private:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Base move constructor.
		 *
		 * Constructs LocalIdItems by taking ownership of the provided GmodPath objects.
		 * This is the primary way to create a populated instance from scratch.
		 *
		 * @param primaryItem The primary GmodPath.
		 * @param secondaryItem The optional secondary GmodPath.
		 */
		inline LocalIdItems( GmodPath&& primaryItem, std::optional<GmodPath>&& secondaryItem );

		/**
		 * @brief Constructor to create a new instance by replacing the primary item.
		 *
		 * Creates a new instance by moving the secondary item from `other` and
		 * moving the provided `newPrimaryItem`.
		 * @param other The existing LocalIdItems instance.
		 * @param newPrimaryItem The new primary GmodPath.
		 */
		inline LocalIdItems( LocalIdItems&& other, GmodPath&& newPrimaryItem );

		/**
		 * @brief Constructor to create a new instance by replacing the secondary item.
		 *
		 * Creates a new instance by moving the primary item from `other` and
		 * moving the provided `newSecondaryItem`.
		 * @param other The existing LocalIdItems instance.
		 * @param newSecondaryItem The new optional secondary GmodPath.
		 */
		inline LocalIdItems( LocalIdItems&& other, std::optional<GmodPath>&& newSecondaryItem );

		/** @brief Default constructor */
		LocalIdItems() = default;

		/** @brief Copy constructor */
		LocalIdItems( const LocalIdItems& other ) = default;

		/** @brief Move constructor */
		LocalIdItems( LocalIdItems&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~LocalIdItems() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		LocalIdItems& operator=( const LocalIdItems& ) = default;

		/** @brief Move assignment operator */
		LocalIdItems& operator=( LocalIdItems&& other ) noexcept = default;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison operator.
		 *        Compares the `m_primaryItem` and `m_secondaryItem` members for equality.
		 * @param other The other LocalIdItems instance to compare with.
		 * @return true if both primary and secondary items are equal, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator==( const LocalIdItems& other ) const noexcept;

		/**
		 * @brief Inequality comparison operator.
		 * @param other The other LocalIdItems instance to compare with.
		 * @return true if the instances are not equal, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator!=( const LocalIdItems& other ) const noexcept;

		void append( nfx::string::StringBuilder& builder, bool verboseMode ) const;

	public:
		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Gets the primary item.
		 * @return A const reference to the optional primary GmodPath.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<GmodPath>& primaryItem() const noexcept;

		/**
		 * @brief Gets the optional secondary item.
		 * @return A const reference to the optional secondary GmodPath.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<GmodPath>& secondaryItem() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief The optional primary item path. */
		std::optional<GmodPath> m_primaryItem;

		/** @brief The optional secondary item path. */
		std::optional<GmodPath> m_secondaryItem;
	};
} // namespace dnv::vista::sdk

#include "detail/LocalIdItems.inl"
