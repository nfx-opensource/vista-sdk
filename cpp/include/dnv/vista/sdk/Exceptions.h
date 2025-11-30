/**
 * @file Exceptions.h
 * @brief Vista SDK exception classes for error handling and validation failures
 * @details Provides a hierarchy of exception classes for handling errors in the Vista SDK,
 *          with specialized support for validation failures and automatic error message formatting.
 */

#pragma once

#include <stdexcept>
#include <string>

#include "Results.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Base Vista exception
	//=====================================================================

	/**
	 * @brief Base exception class for Vista SDK operations
	 * @details All Vista SDK exceptions inherit from this base class, providing
	 *          consistent error message formatting and integration with standard C++ exception handling.
	 *          Exception messages are automatically prefixed with "VistaException - " for identification.
	 */
	class VistaException : public std::runtime_error
	{
	public:
		/**
		 * @brief Constructs a Vista exception with the specified message
		 * @param message The error message describing the exception
		 * @details The message will be automatically prefixed with "VistaException - "
		 *          when retrieved via what().
		 */
		explicit VistaException( const std::string& message );

		/**
		 * @brief Copy constructor.
		 * @param other The VistaException to copy from.
		 */
		VistaException( const VistaException& other ) = default;

		/**
		 * @brief Move constructor.
		 * @param other The VistaException to move from.
		 */
		VistaException( VistaException&& other ) noexcept = default;

		/**
		 * @brief Copy assignment operator.
		 * @param other The VistaException to copy from.
		 * @return Reference to this VistaException after assignment.
		 */
		VistaException& operator=( const VistaException& other ) = default;

		/**
		 * @brief Move assignment operator.
		 * @param other The VistaException to move from.
		 * @return Reference to this VistaException after assignment.
		 */
		VistaException& operator=( VistaException&& other ) noexcept = default;

		virtual ~VistaException() override = default;
	};

	//=====================================================================
	// Validation exception
	//=====================================================================

	class ValidateResult;

	/**
	 * @brief Exception thrown for validation failures in Vista SDK operations
	 * @details Specialized exception for handling validation errors, with support for both
	 *          simple string messages and structured validation results. Provides automatic
	 *          formatting of multiple validation errors into a single readable message.
	 */
	class ValidationException final : public VistaException
	{
	public:
		/**
		 * @brief Constructs a validation exception with a simple error message
		 * @param message The validation error message
		 * @details The message will be formatted as:
		 *          "VistaException - Validation failed - Message='<message>'"
		 * @code
		 * throw ValidationException("Invalid position format");
		 * // Results in: "VistaException - Validation failed - Message='Invalid position format'"
		 * @endcode
		 */
		explicit ValidationException( const std::string& message );

		/**
		 * @brief Constructs a validation exception from a structured validation result
		 * @param result The ValidateResult::Invalid containing multiple error messages
		 * @details Automatically joins multiple validation errors into a single formatted message.
		 *          Uses high-performance StringBuilder for efficient string concatenation.
		 *          The message will be formatted as:
		 *          "VistaException - Validation failed - Message='[error1, error2, ...]'"
		 * @code
		 * std::vector<std::string> errors = {"Invalid format", "Missing component"};
		 * ValidateResult::Invalid result(std::move(errors));
		 * throw ValidationException(result);
		 * // Results in: "VistaException - Validation failed - Message='[Invalid format, Missing component]'"
		 * @endcode
		 */
		explicit ValidationException( const ValidateResult::Invalid& result );

		/**
		 * @brief Copy constructor.
		 * @param other The ValidationException to copy from.
		 */
		ValidationException( const ValidationException& other ) = default;

		/**
		 * @brief Move constructor.
		 * @param other The ValidationException to move from.
		 */
		ValidationException( ValidationException&& other ) noexcept = default;

		/**
		 * @brief Copy assignment operator.
		 * @param other The ValidationException to copy from.
		 * @return Reference to this ValidationException after assignment.
		 */
		ValidationException& operator=( const ValidationException& other ) = default;

		/**
		 * @brief Move assignment operator.
		 * @param other The ValidationException to move from.
		 * @return Reference to this ValidationException after assignment.
		 */
		ValidationException& operator=( ValidationException&& other ) noexcept = default;

		virtual ~ValidationException() override = default;
	};
} // namespace dnv::vista::sdk
