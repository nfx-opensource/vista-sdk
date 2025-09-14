/**
 * @file ParsingErrors.h
 * @brief VISTA Parsing Error Management System for Comprehensive Diagnostic Reporting
 *
 * @details
 * This file implements the **VISTA Parsing Error System** for collecting, managing, and
 * reporting parsing errors encountered during maritime data validation and processing operations.
 * It provides comprehensive error collection, classification, and diagnostic reporting
 * capabilities with specialized builder patterns for domain-specific parsing scenarios.
 *
 * ## System Purpose:
 *
 * The **VISTA Parsing Error System** serves as the foundation for:
 * - **Error Collection**    : Gathering parsing errors during data validation operations
 * - **Error Classification**: Categorizing errors by type and parsing stage for targeted handling
 * - **Diagnostic Reporting**: Providing detailed error messages for debugging and analysis
 * - **Validation Feedback** : Supporting comprehensive validation result reporting
 * - **Builder Integration** : Specialized error builders for LocalId and Location parsing
 *
 * ## Core Architecture:
 *
 * ### Error Management Components
 * - **ParsingErrors**              : Main immutable container managing error collections
 * - **ErrorEntry**                 : Individual error with type and message information
 * - **Enumerator**                 : Safe iterator for traversing error collections
 * - **LocalIdParsingErrorBuilder** : Specialized builder for LocalId parsing errors
 * - **LocationParsingErrorBuilder**: Specialized builder for Location parsing errors
 *
 * ### Builder Pattern Integration
 * - **State-Based Classification**: Enum-driven error categorization for precise diagnostics
 * - **Fluent Interface**          : Chainable error accumulation with method chaining
 * - **Domain Specialization**     : Tailored builders for specific parsing scenarios
 * - **Controlled Construction**   : Friend class pattern prevents invalid error creation
 *
 * ## Data Flow Architecture:
 *
 * ```
 * Specialized Parsing Operation
 *         ↓
 * Domain-Specific Error Builder
 *         ↓
 * ┌─────────────────────────────────────┐
 * │    LocalId/LocationErrorBuilder     │
 * ├─────────────────────────────────────┤
 * │ ┌─────────────────────────────────┐ │
 * │ │   ParsingState/ValidationResult │ │ ← Enum-based classification
 * │ │        + Error Messages         │ │
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *         ↓ build()
 * ┌─────────────────────────────────────┐
 * │          ParsingErrors              │
 * ├─────────────────────────────────────┤
 * │ ┌─────────────────────────────────┐ │
 * │ │    vector<ErrorEntry>           │ │ ← Immutable error storage
 * │ │      m_errors                   │ │   (move semantics)
 * │ └─────────────────────────────────┘ │
 * │               ↓                     │
 * │ ┌─────────────────────────────────┐ │
 * │ │      ErrorEntry                 │ │ ← Type-safe error pairs
 * │ │  ┌───────────────────────────┐  │ │
 * │ │  │  string type              │  │ │ ← Error category
 * │ │  │  string message           │  │ │ ← Error description
 * │ │  └───────────────────────────┘  │ │
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *         ↓
 * Error Analysis & Reporting
 * ```
 *
 * ## Usage Patterns:
 *
 * ### LocalId Parsing with State-Based Errors
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Location Parsing with Validation Results
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Error Analysis and Reporting
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Performance Characteristics:
 *
 * - **Move Semantics**    : Zero-copy construction with `std::vector<ErrorEntry>&&`
 * - **Memory Efficiency** : Optimized storage with move constructors and string optimization
 * - **Fast Enumeration**  : Exception-safe iterator with `noexcept` traversal
 * - **Static Empty**      : Shared empty instance for zero-error scenarios
 * - **String View Access**: `string_view` interfaces minimize allocation overhead
 *
 * ## Error Classification Systems:
 *
 * ### LocalId Parsing States (Hierarchical)
 * - **Sequential Stages (0-99)**   : `NamingRule`, `VisVersion`, `PrimaryItem`, `SecondaryItem`
 * - **Structural Errors (100-199)**: `EmptyState`, `Formatting`, `Completeness`
 * - **Validation Errors (200+)**   : `NamingEntity`, `IMONumber`
 *
 * ### Location Validation Results
 * - **`Invalid`**         : General validation failure
 * - **`InvalidCode`**     : Unrecognized location code
 * - **`InvalidOrder`**    : Incorrect component sequence
 * - **`NullOrWhiteSpace`**:  Empty or whitespace-only input
 * - **`Valid`**           : Successful validation
 *
 * ## String Representation Format:
 *
 * ### Formatted Output Examples
 * ```
 * Success                           (when no errors)
 *
 * Parsing errors:                   (when errors exist)
 * 	ValidationError - Invalid location code 'X' at position 2
 * 	FormatError - Missing separator in location string
 * 	SemanticError - Inconsistent component combination
 * ```
 *
 * ## Design Philosophy:
 *
 * - **Comprehensive Diagnostics**: Detailed error information for effective debugging and analysis
 * - **Type Safety**              : Strong typing with enum-based classification prevents error loss
 * - **Performance Focus**        : Move semantics and zero-copy patterns for parsing-intensive operations
 * - **Immutability**             : Thread-safe design with immutable error collections after construction
 * - **Builder Pattern**          : Specialized builders provide domain-specific error accumulation
 */

#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace dnv::vista::sdk
{
	namespace internal
	{
		class LocalIdParsingErrorBuilder;
		class LocationParsingErrorBuilder;
	}

	//=====================================================================
	// ParsingErrors class
	//=====================================================================

	/**
	 * @brief Represents a collection of parsing errors.
	 *
	 * This class is used to store and manage errors encountered during parsing operations.
	 */
	class ParsingErrors final
	{
		friend class internal::LocalIdParsingErrorBuilder;
		friend class internal::LocationParsingErrorBuilder;

	public:
		//----------------------------------------------
		// Forward declarations
		//----------------------------------------------

		class Enumerator;
		struct ErrorEntry;

		//----------------------------------------------
		// Construction
		//----------------------------------------------
	private:
		/**
		 * @brief Internal constructor for creating ParsingErrors with error entries (copy).
		 * @param errors A vector of error entries to copy.
		 */
		explicit ParsingErrors( const std::vector<ErrorEntry>& errors );

		/**
		 * @brief Internal constructor for creating ParsingErrors with error entries (move).
		 * @param errors A vector of error entries to move from.
		 * @note This constructor takes ownership of the provided vector for optimal performance.
		 */
		explicit ParsingErrors( std::vector<ErrorEntry>&& errors ) noexcept;

	public:
		/** @brief Default constructor */
		ParsingErrors();

		/** @brief Copy constructor */
		ParsingErrors( const ParsingErrors& ) = default;

		/** @brief Move constructor */
		ParsingErrors( ParsingErrors&& errors ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~ParsingErrors() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		ParsingErrors& operator=( const ParsingErrors& ) = default;

		/** @brief Move assignment operator */
		ParsingErrors& operator=( ParsingErrors&& ) noexcept = default;

		//----------------------------------------------
		// Operators
		//----------------------------------------------

		/**
		 * @brief Equality operator.
		 * @param other The other ParsingErrors object to compare.
		 * @return True if the two ParsingErrors objects are equal, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator==( const ParsingErrors& other ) const noexcept;

		/**
		 * @brief Inequality operator.
		 * @param other The other ParsingErrors object to compare.
		 * @return True if the two ParsingErrors objects are not equal, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator!=( const ParsingErrors& other ) const noexcept;

		/**
		 * @brief Checks if this ParsingErrors object is equal to another.
		 * @param other The other ParsingErrors object to compare.
		 * @return True if the two ParsingErrors objects are equal, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool equals( const ParsingErrors& other ) const noexcept;

		//----------------------------------------------
		// Public static members
		//----------------------------------------------

		/** @brief Gets an empty set of parsing errors. */
		inline static const ParsingErrors& empty();

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Gets the number of error entries.
		 * @return The count of error entries.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline size_t count() const noexcept;

		//----------------------------------------------
		// State inspection methods
		//----------------------------------------------

		/**
		 * @brief Checks if there are any errors.
		 * @return True if there are errors, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool hasErrors() const noexcept;

		/**
		 * @brief Checks if there is an error of a specific type.
		 * @param type The type of error to check for.
		 * @return True if an error of the specified type exists, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool hasErrorType( std::string_view type ) const noexcept;

		//----------------------------------------------
		// String conversion methods
		//----------------------------------------------

		/**
		 * @brief Converts the parsing errors to a string representation.
		 * @return A string representation of the parsing errors.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		//----------------------------------------------
		// Enumeration
		//----------------------------------------------

		/**
		 * @brief Gets an enumerator for the error entries.
		 * @return An enumerator for iterating through the errors.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] Enumerator enumerator() const;

		//----------------------------------------------
		// ParsingErrors::Enumerator class
		//----------------------------------------------

		/**
		 * @brief Enumerator for traversing parsing errors.
		 *
		 * Provides enumeration functionality for iterating through error entries.
		 * The enumerator starts positioned before the first element.
		 */
		class Enumerator final
		{
		public:
			//----------------------------
			// Construction
			//----------------------------

			/**
			 * @brief Constructs an enumerator for the given error data.
			 * @param data Pointer to the vector of error entries to enumerate.
			 */
			Enumerator( const std::vector<ErrorEntry>* data );

			/** @brief Default constructor */
			Enumerator() = delete;

			/** @brief Copy constructor */
			Enumerator( const Enumerator& ) = default;

			/** @brief Move constructor */
			Enumerator( Enumerator&& ) noexcept = default;

			//----------------------------------------------
			// Destruction
			//----------------------------------------------

			/** @brief Destructor */
			~Enumerator() = default;

			//----------------------------
			// Assignment operators
			//----------------------------

			/** @brief Copy assignment operator */
			Enumerator& operator=( const Enumerator& ) = default;

			/** @brief Move assignment operator */
			Enumerator& operator=( Enumerator&& ) noexcept = default;

			//----------------------------
			// Enumeration methods
			//----------------------------

			/**
			 * @brief Advances the enumerator to the next element.
			 * @return True if the enumerator successfully moved to the next element;
			 *         false if the enumerator has passed the end of the collection.
			 */
			[[nodiscard]] inline bool next() noexcept;

			/**
			 * @brief Gets the current element.
			 * @return The current error entry.
			 * @throws std::out_of_range if enumerator is not positioned on a valid element
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] const ErrorEntry& current() const;

			/** @brief Resets the enumerator to its initial position. */
			void inline reset() noexcept;

		private:
			//----------------------------
			// Private member variables
			//----------------------------

			const std::vector<ErrorEntry>* m_data;
			size_t m_index;
		};

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		std::vector<ErrorEntry> m_errors;

	public:
		//----------------------------------------------
		// ParsingErrors::ErrorEntry struct
		//----------------------------------------------
		struct ErrorEntry
		{
			std::string type;
			std::string message;

			ErrorEntry() = default;
			ErrorEntry( std::string_view errorType, std::string_view errorMessage );
			ErrorEntry( std::string&& errorType, std::string&& errorMessage );

			//----------------------------
			// Operators
			//----------------------------

			/**
			 * @brief Equality operator for error entries
			 * @param other The other ErrorEntry to compare with
			 * @return True if both type and message are equal, false otherwise
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool operator==( const ErrorEntry& other ) const noexcept;

			/**
			 * @brief Inequality operator for error entries
			 * @param other The other ErrorEntry to compare with
			 * @return True if type or message differ, false otherwise
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool operator!=( const ErrorEntry& other ) const noexcept;
		};
	};
}

#include "ParsingErrors.inl"
