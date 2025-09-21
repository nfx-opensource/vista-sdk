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
 * - **Dual Path Storage**    : Primary and optional secondary GMOD path management
 * - **Immutable Design**     : Thread-safe storage that cannot be modified after construction
 * - **Builder Integration**  : Exclusive access through LocalIdBuilder friend class
 * - **Move Semantics**       : Efficient path ownership transfer with zero-copy construction
 * - **String Representation**: Unified path serialization with verbose mode support
 *
 * ## Core Architecture:
 *
 * ### Storage Model
 * - **Primary Path**   : Required GMOD path representing the main component or data point
 * - **Secondary Path** : Optional GMOD path for additional context or related components
 * - **Move-Only**      : GmodPath non-copyable nature enforces move semantics throughout
 * - **Immutable**      : No modification allowed after construction for thread safety
 *
 * ## Memory Layout & Performance:
 *
 * ```
 * LocalIdItems Container Structure:
 * ┌─────────────────────────────────────┐
 * │          LocalIdItems               │
 * ├─────────────────────────────────────┤
 * │ ┌─────────────────────────────────┐ │
 * │ │  std::optional<GmodPath>        │ │ ← Primary path storage
 * │ │      m_primaryItem              │ │   (required component)
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │  std::optional<GmodPath>        │ │ ← Secondary path storage
 * │ │     m_secondaryItem             │ │   (optional context)
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 * ```
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
 * ## Design Philosophy:
 *
 * - **Type Safety**         : Private constructors prevent invalid state creation
 * - **Performance Focus**   : Move semantics for efficient path ownership transfer
 * - **Immutability**        : Thread-safe design with no post-construction changes
 * - **Builder Integration** : Seamless integration with fluent builder interface
 * - **Standards Compliance**: Consistent with VIS maritime data identification standards
 *
 * ## Performance Characteristics:
 *
 * - **Construction**: O(1) move operations for path ownership transfer
 * - **Access**      : O(1) direct member access through friend interface
 * - **Memory**      : Minimal overhead with optional<GmodPath> storage
 * - **Thread Safety**: Read-only operations safe for concurrent access
 * - **Move Semantics**: Zero-copy construction and manipulation
 */

#pragma once

#include <optional>

#include <nfx/string/StringBuilderPool.h>

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
	 * immutable after construction. Due to GmodPath being non-copyable, this class
	 * also enforces move-only semantics for construction involving GmodPath objects.
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
		 * @param primaryItem The primary GmodPath (moved into `m_primaryItem`).
		 * @param secondaryItem The optional secondary GmodPath (moved into `m_secondaryItem`).
		 */
		inline LocalIdItems( GmodPath&& primaryItem, std::optional<GmodPath>&& secondaryItem );

		/**
		 * @brief Constructor to create a new instance by replacing the primary item.
		 *
		 * Creates a new instance by moving the secondary item from `other` and
		 * moving the provided `newPrimaryItem`.
		 * @param other The existing LocalIdItems instance (moved from).
		 * @param newPrimaryItem The new primary GmodPath (moved).
		 */
		inline LocalIdItems( LocalIdItems&& other, GmodPath&& newPrimaryItem );

		/**
		 * @brief Constructor to create a new instance by replacing the secondary item.
		 *
		 * Creates a new instance by moving the primary item from `other` and
		 * moving the provided `newSecondaryItem`.
		 * @param other The existing LocalIdItems instance (moved from).
		 * @param newSecondaryItem The new optional secondary GmodPath (moved).
		 */
		inline LocalIdItems( LocalIdItems&& other, std::optional<GmodPath>&& newSecondaryItem );

		/** @brief Default constructor */
		LocalIdItems() = default;

		/** @brief Copy constructor */
		LocalIdItems( const LocalIdItems& ) = default;

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
		LocalIdItems& operator=( const LocalIdItems& ) = delete;

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
}

#include "detail/LocalIdItems.inl"
