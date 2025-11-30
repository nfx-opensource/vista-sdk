/**
 * @file GmodNode.h
 * @brief GMOD node classification and categorization constants
 * @details Category, type, and composite classification constants for
 *          GMOD node identification and traversal operations
 */

#pragma once

#include <cstdint>
#include <string_view>

namespace dnv::vista::sdk::internal
{
	namespace gmodnode
	{
		//=====================================================================
		// GMOD node classification constants
		//=====================================================================

		//----------------------------------------------
		// Primary node categories
		//----------------------------------------------

		/** @brief Asset category identifier for GMOD nodes. */
		inline constexpr std::string_view CATEGORY_ASSET = "ASSET";

		/** @brief Function category identifier for GMOD nodes. */
		inline constexpr std::string_view CATEGORY_FUNCTION = "FUNCTION";

		/** @brief Product category identifier for GMOD nodes. */
		inline constexpr std::string_view CATEGORY_PRODUCT = "PRODUCT";

		//----------------------------------------------
		// Composite node categories
		//----------------------------------------------

		/** @brief Asset function composite category for GMOD nodes. */
		inline constexpr std::string_view ASSET_FUNCTION = "ASSET FUNCTION";

		/** @brief Product function composite category for GMOD nodes. */
		inline constexpr std::string_view CATEGORY_PRODUCT_FUNCTION = "PRODUCT FUNCTION";

		//----------------------------------------------
		// Node Type Classifications
		//----------------------------------------------

		/** @brief Selection type identifier for GMOD nodes. */
		inline constexpr std::string_view TYPE_SELECTION = "SELECTION";

		/** @brief Group type identifier for GMOD nodes. */
		inline constexpr std::string_view TYPE_GROUP = "GROUP";

		/** @brief Leaf type identifier for GMOD nodes. */
		inline constexpr std::string_view TYPE_LEAF = "LEAF";

		/** @brief Type type identifier for GMOD nodes. */
		inline constexpr std::string_view TYPE_TYPE = "TYPE";

		/** @brief Composition type identifier for GMOD nodes. */
		inline constexpr std::string_view TYPE_COMPOSITION = "COMPOSITION";

		//----------------------------------------------
		// Full Type Specifications
		//----------------------------------------------

		/** @brief Complete asset function leaf type specification. */
		inline constexpr std::string_view FULLTYPE_ASSET_FUNCTION_LEAF = "ASSET FUNCTION LEAF";

		/** @brief Complete product function leaf type specification. */
		inline constexpr std::string_view FULLTYPE_PRODUCT_FUNCTION_LEAF = "PRODUCT FUNCTION LEAF";
	} // namespace gmodnode
} // namespace dnv::vista::sdk::internal
