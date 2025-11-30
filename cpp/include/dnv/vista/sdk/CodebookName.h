/**
 * @file CodebookName.h
 * @brief Enumeration and utility functions for ISO 19848 codebook naming
 *
 * This file defines the standard codebook types as specified in the ISO 19848 standard
 * for vessel information structure, along with utility functions to convert between
 * enum values and their string prefix representations.
 *
 * @see ISO 19848 - Ships and marine technology — Standard data for shipboard machinery and equipment
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace dnv::vista::sdk
{
	//=====================================================================
	// Codebook Type Definitions
	//=====================================================================

	/**
	 * @brief Enumeration of codebook types used in the VIS system
	 *
	 * Each value represents a different category of data that can be stored
	 * in the vessel information structure according to ISO 19848.
	 */
	enum class CodebookName : std::uint8_t
	{
		Quantity = 1,		 ///< qty
		Content,			 ///< cnt
		Calculation,		 ///< calc
		State,				 ///< state
		Command,			 ///< cmd
		Type,				 ///< type
		FunctionalServices,	 ///< funct.svc
		MaintenanceCategory, ///< maint.cat
		ActivityType,		 ///< act.type
		Position,			 ///< pos
		Detail				 ///< detail
	};

	//=====================================================================
	// CodebookNames class
	//=====================================================================

	/**
	 * @brief Utility class for working with CodebookName values
	 *
	 * Provides conversion methods between CodebookName enum values
	 * and their string prefix representations as defined in the ISO 19848 standard.
	 */
	class CodebookNames final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor. */
		CodebookNames() = delete;

		/** @brief Copy constructor */
		CodebookNames( const CodebookNames& ) = delete;

		/** @brief Move constructor */
		CodebookNames( CodebookNames&& ) noexcept = delete;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~CodebookNames() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		CodebookNames& operator=( const CodebookNames& ) = delete;

		/** @brief Move assignment operator */
		CodebookNames& operator=( CodebookNames&& ) noexcept = delete;

		//----------------------------------------------
		// Public static methods
		//----------------------------------------------

		/**
		 * @brief Convert a string prefix to a CodebookName
		 * @param prefix The string prefix to convert (e.g., "pos", "qty", "calc"). Must not be empty.
		 * @return The corresponding CodebookName.
		 * @throws std::invalid_argument If the prefix is empty or unknown.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static CodebookName fromPrefix( std::string_view prefix );

		/**
		 * @brief Convert a CodebookName to its string prefix
		 * @param name The CodebookName to convert
		 * @return The corresponding string prefix (e.g., "pos" for Position)
		 * @throws std::invalid_argument If the name is unknown
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static std::string toPrefix( CodebookName name );

		/**
		 * @brief Convert a string name to a CodebookName
		 * @param name The string name to convert
		 * @return The corresponding CodebookName
		 * @throws std::invalid_argument If the name is unknown
		 */
		[[nodiscard]] static CodebookName fromString( std::string_view name );

		/**
		 * @brief Converts a CodebookName enum to its string representation for error messages.
		 * @param[in] name The CodebookName enum value to convert.
		 * @return A string containing the codebook name.
		 * @throws std::invalid_argument If the codebook name is unknown or invalid.
		 */
		[[nodiscard]] static std::string toString( CodebookName name );
	};
} // namespace dnv::vista::sdk
