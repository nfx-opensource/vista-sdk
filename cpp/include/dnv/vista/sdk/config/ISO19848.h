/**
 * @file ISO19848.h
 * @brief Constants for ISO 19848 Annex C naming conventions
 * @details Compile-time constants for ISO 19848 Annex C naming rules and entity identifiers
 */

#pragma once

#include <string_view>

namespace dnv::vista::sdk::iso19848
{
	namespace version
	{
		/** @brief ISO 19848 version 2018 identifier */
		inline constexpr std::string_view v2018 = "v2018";

		/** @brief ISO 19848 version 2024 identifier */
		inline constexpr std::string_view v2024 = "v2024";
	}

	namespace annex_c
	{
		/** @brief ISO 19848 Annex C naming entity */
		inline constexpr std::string_view NAMING_ENTITY = "data.dnv.com";

		/** @brief Default reference URL for ISO 19848 documentation */
		inline constexpr std::string_view REFERENCE_URL = "https://docs.vista.dnv.com";

		/** @brief ISO 19848 Annex C naming rule identifier */
		inline constexpr std::string_view NAMING_RULE_PREFIX = "dnv";

		/** @brief ISO 19848 Annex C naming scheme version */
		inline constexpr std::string_view NAMING_SCHEME_VERSION = "v2";

		/** @brief ISO 19848 Annex C naming rule */
		inline constexpr std::string_view NAMING_RULE = "dnv-v2";
	}
}
