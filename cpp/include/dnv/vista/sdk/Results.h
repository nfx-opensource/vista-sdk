/**
 * @file Results.h
 * @brief VISTA Validation Result System for Type-Safe Operation Outcomes
 *
 * @details
 * This file implements the **VISTA Result System** for representing validation and operation
 * outcomes in a type-safe, discriminated union pattern. It provides comprehensive result
 * handling capabilities with explicit success/failure states and detailed error reporting,
 * following functional programming principles for robust error handling in maritime data validation.
 *
 * ## System Purpose:
 *
 * The **VISTA Result System** serves as the foundation for:
 * - **Type-Safe Results**      : Discriminated unions preventing invalid state access
 * - **Explicit Error Handling**: Compile-time enforcement of error checking
 * - **Validation Outcomes**    : Clear success/failure representation for validation operations
 * - **Error Aggregation**      : Collection and reporting of multiple validation errors
 * - **Functional Patterns**    : Result types enabling functional composition and error propagation
 *
 * ## Core Architecture:
 *
 * ### Result Components
 * - **ValidateResult**         : Discriminated union representing validation outcomes
 * - **ValidateResult::Ok**     : Success state with no errors or additional data
 * - **ValidateResult::Invalid**: Failure state containing validation error messages
 * - **std::variant Backend**   : Type-safe union implementation with compile-time guarantees
 *
 * ### Result Pattern Benefits
 * - **Explicit State**     : Clear distinction between success and failure states
 * - **Compile-Time Safety**: Type system prevents accessing invalid result states
 * - **Error Propagation**  : Structured error passing through validation chains
 * - **No Exceptions**      : Exception-free error handling for performance-critical code
 *
 * ## Data Flow Architecture:
 *
 * ```
 *         Validation Operation
 *                   ↓
 *        Result Classification
 *                   ↓
 * ┌─────────────────────────────────────┐
 * │            ValidateResult           │
 * ├─────────────────────────────────────┤
 * │             std::variant            │
 * │ ┌─────────────────────────────────┐ │
 * │ │                Ok               │ │ ← Success state
 * │ │       (no additional data)      │ │   (empty class)
 * │ └─────────────────────────────────┘ │
 * │                  OR                 │
 * │ ┌─────────────────────────────────┐ │
 * │ │             Invalid             │ │ ← Failure state
 * │ │  ┌────────────────────────────┐ │ │
 * │ │  │  vector<string> m_errors   │ │ │ ← Error messages
 * │ │  └────────────────────────────┘ │ │
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *                   ↓
 *       Type-Safe Result Handling
 *                   ↓
 * ┌─────────────────────────────────────┐
 * │    Application Response System      │
 * ├─────────────────────────────────────┤
 * │ - Success Path Execution            │
 * │ - Error Message Processing          │
 * │ - Validation Feedback Generation    │
 * │ - Functional Composition Chains     │
 * └─────────────────────────────────────┘
 *```
 *
 * ## Usage Patterns:
 *
 * ### Basic Validation Result Creation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Type-Safe Result Handling
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Functional Composition Patterns
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Error Aggregation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Performance Characteristics:
 *
 * - **Zero-Cost Abstractions**: std::variant implementation with no runtime overhead
 * - **Move Semantics**        : Efficient error vector transfer with move construction
 * - **Stack Allocation**      : Result objects allocated on stack for performance
 * - **No Exceptions**         : Exception-free error handling reduces overhead
 * - **Compile-Time Safety**   : Type checking prevents runtime state access errors
 *
 * ## Type Safety Guarantees:
 *
 * ### Compile-Time Enforcement
 * - **State Verification**: `isOk()` and `isInvalid()` must be checked before access
 * - **Type System**       : std::variant prevents invalid state combinations
 * - **[[nodiscard]]**     : Return values must be explicitly handled
 * - **const Correctness** : Immutable access to result contents
 *
 * ### Runtime Safety
 * - **Exception on Invalid Access**: Attempting to access wrong state throws exception
 * - **Move-Only Semantics**        : Error vectors can be efficiently moved
 * - **Memory Safety**              : RAII and automatic cleanup of error collections
 *
 * ## Integration with Vista SDK:
 *
 * ### Validation Operation Patterns
 * - **LocalId Validation** : Returns ValidateResult for ID parsing and validation
 * - **Location Validation**: Returns ValidateResult for location string parsing
 * - **Metadata Validation**: Returns ValidateResult for metadata tag validation
 * - **Codebook Validation**: Returns ValidateResult for codebook value checking
 *
 * ## Design Philosophy:
 *
 * - **Explicit Error Handling**: Compile-time enforcement prevents ignored errors
 * - **Functional Composition** : Result types enable functional programming patterns
 * - **Performance Focus**      : Zero-cost abstractions with efficient error propagation
 * - **Type Safety**            : Strong typing prevents invalid state access
 * - **Maritime Domain Focus**  : Tailored for maritime data validation scenarios
 */

#pragma once

#include <string>
#include <variant>
#include <vector>

namespace dnv::vista::sdk
{
	//=====================================================================
	// ValidateResult class
	//=====================================================================

	/**
	 * @brief Discriminated union representing a validation result that can be either Ok or Invalid
	 */
	class ValidateResult
	{
	public:
		/**
		 * @brief Represents a successful validation result with no errors
		 */
		class Ok final
		{
		public:
			/**
			 * @brief Default constructor for successful validation result
			 */
			Ok() = default;
		};

		/**
		 * @brief Represents a failed validation result containing error messages.
		 *
		 * @details
		 * The Invalid class encapsulates the failure state of a validation operation in the VISTA Result System.
		 * It stores a vector of error messages describing the reasons for validation failure. Instances of Invalid
		 * are used in the ValidateResult discriminated union to provide detailed, type-safe error reporting and
		 * aggregation for robust error handling in maritime data validation workflows.
		 */
		class Invalid final
		{
		public:
			/**
			 * @brief Constructs an invalid result with a list of validation errors
			 * @param errors The list of validation error messages
			 */
			inline explicit Invalid( std::vector<std::string> errors ) noexcept;

			/**
			 * @brief Gets the list of validation errors
			 * @return Constant reference to the vector of validation error messages
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const std::vector<std::string>& errors() const noexcept;

		private:
			std::vector<std::string> m_errors;
		};

		//-----------------------------
		// Construction from result
		//-----------------------------

		/**
		 * @brief Constructs a ValidateResult from a successful Ok result
		 * @param ok The Ok result state
		 */
		inline ValidateResult( Ok ok ) noexcept;

		/**
		 * @brief Constructs a ValidateResult from an Invalid result containing errors
		 * @param invalid The Invalid result state
		 */
		inline ValidateResult( Invalid invalid ) noexcept;

		//-----------------------------
		// Type checking
		//-----------------------------

		/**
		 * @brief Checks if this result represents a successful validation
		 * @return True if the result is Ok, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isOk() const noexcept;

		/**
		 * @brief Checks if this result represents a failed validation with errors
		 * @return True if the result is Invalid, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isInvalid() const noexcept;

		//-----------------------------
		// Value access
		//-----------------------------

		/**
		 * @brief Gets the Ok result value, throws if this result is Invalid
		 * @return Constant reference to the Ok result state
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Ok& ok() const;

		/**
		 * @brief Gets the Invalid result value, throws if this result is Ok
		 * @return Constant reference to the Invalid result state
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Invalid& invalid() const;

	private:
		std::variant<Ok, Invalid> m_value;
	};
} // namespace dnv::vista::sdk

#include "detail/Results.inl"
