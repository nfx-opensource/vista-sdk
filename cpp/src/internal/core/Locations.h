/**
 * @file Locations.h
 * @brief Location parsing and validation constants
 * @details Contains character codes, group names, and error message templates
 *          for location parsing and validation operations
 */

#pragma once

#include <cstdint>
#include <string_view>

namespace dnv::vista::sdk::internal
{
	namespace locations
	{
		//----------------------------------------------
		// Location character constants
		//----------------------------------------------

		/**
		 * @name Special location character codes
		 * @brief Special characters used in location parsing
		 */
		/// Horizontal location character
		inline constexpr char CHAR_HORIZONTAL = 'H';
		/// Vertical location character
		inline constexpr char CHAR_VERTICAL = 'V';
		/// Number location character
		inline constexpr char CHAR_NUMBER = 'N';

		/**
		 * @name Side group character codes
		 * @brief Characters representing port, center, and starboard sides
		 */
		/// Port side character
		inline constexpr char CHAR_PORT = 'P';
		/// Center side character
		inline constexpr char CHAR_CENTER = 'C';
		/// Starboard side character
		inline constexpr char CHAR_STARBOARD = 'S';

		/**
		 * @name Vertical group character codes
		 * @brief Characters representing upper, middle, and lower vertical positions
		 */
		/// Upper vertical character
		inline constexpr char CHAR_UPPER = 'U';
		/// Middle vertical character
		inline constexpr char CHAR_MIDDLE = 'M';
		/// Lower vertical character
		inline constexpr char CHAR_LOWER = 'L';

		/**
		 * @name Transverse group character codes
		 * @brief Characters representing inboard and outboard positions
		 */
		/// Inboard transverse character
		inline constexpr char CHAR_INBOARD = 'I';
		/// Outboard transverse character
		inline constexpr char CHAR_OUTBOARD = 'O';

		/**
		 * @name Longitudinal group character codes
		 * @brief Characters representing forward and aft positions
		 */
		/// Forward longitudinal character
		inline constexpr char CHAR_FORWARD = 'F';
		/// Aft longitudinal character
		inline constexpr char CHAR_AFT = 'A';

		//----------------------------------------------
		// Group name constants
		//----------------------------------------------

		inline constexpr std::string_view GROUP_NAME_NUMBER = "Number";
		inline constexpr std::string_view GROUP_NAME_SIDE = "Side";
		inline constexpr std::string_view GROUP_NAME_VERTICAL = "Vertical";
		inline constexpr std::string_view GROUP_NAME_TRANSVERSE = "Transverse";
		inline constexpr std::string_view GROUP_NAME_LONGITUDINAL = "Longitudinal";
		inline constexpr std::string_view GROUP_NAME_UNKNOWN = "Unknown";
	} // namespace locations
} // namespace dnv::vista::sdk::internal
