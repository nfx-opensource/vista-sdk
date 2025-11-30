/**
 * @file Codebook.h
 * @brief Codebook-specific constants for Vista SDK codebook operations
 * @details Contains codebook names, prefixes, validation groups, and result constants
 *          for efficient codebook processing and position validation
 */

#pragma once

#include <cstdint>
#include <string_view>

namespace dnv::vista::sdk::internal
{
	namespace codebook
	{
		//----------------------------------------------
		// Magic numbers
		//----------------------------------------------

		/*
		 * TODO: Future enhancement: Consider making these limits configurable through a CodebookConfig
		 *       structure to allow users to tune performance/memory trade-offs for their specific use cases.
		 */

		/**
		 * @brief Stack allocation limit for position parsing arrays to avoid heap allocation during position validation.
		 * @details This constant defines the maximum number of position components that can be parsed from a
		 *          hyphen-separated position string (e.g., "port-forward-upper"). Using stack allocation for
		 *          small arrays provides significant performance benefits over dynamic allocation, especially
		 *          for high-frequency validation operations.
		 *
		 *          Typical VISTA position strings contain 1-4 components, making 16 a generous upper bound
		 *          that covers edge cases while maintaining stack safety. If a position string exceeds this
		 *          limit, validation returns Invalid to prevent buffer overflows.
		 *
		 * @note Value chosen based on analysis of VISTA position vocabulary patterns and performance testing.
		 */
		static constexpr size_t MAX_POSITIONS = 16;

		/**
		 * @brief Stack allocation limit for group tracking arrays to avoid heap allocation during position grouping validation.
		 * @details During position validation, the system tracks which groups each position component belongs to
		 *          (e.g., "Side (ship)", "Longitudinal", "Vertical"). This constant limits the number of unique
		 *          groups that can be tracked on the stack.
		 *
		 *          VISTA position codebook contains approximately 13 distinct groups, making 16 a safe upper bound
		 *          that accommodates all current groups plus potential future extensions. Stack allocation here
		 *          avoids memory fragmentation during frequent group validation operations.
		 *
		 * @note Corresponds to the number of position groups in VISTA codebook specification.
		 */
		static constexpr size_t MAX_GROUPS = 16;

		/**
		 * @brief Stack allocation limit for non-numeric position tracking to avoid heap allocation during order validation.
		 * @details Position validation includes checking the alphabetical ordering of non-numeric components
		 *          (numeric components like "1", "2", "3" must appear at the end). This constant limits how many
		 *          non-numeric position components can be tracked for sorting validation.
		 *
		 *          Most VISTA position strings contain 2-3 non-numeric components, making 8 a conservative
		 *          upper bound that handles complex position hierarchies while maintaining stack efficiency.
		 *          Exceeding this limit degrades to less strict validation but remains functional.
		 *
		 * @note Chosen to handle complex multi-level position hierarchies without excessive stack usage.
		 */
		static constexpr size_t MAX_NON_NUMERIC = 8;

		//----------------------------------------------
		// Codebooks names
		//----------------------------------------------

		/** @brief Standard position codebook name identifier. */
		inline constexpr std::string_view NAME_POSITION = "positions";

		/** @brief Standard quantity codebook name identifier. */
		inline constexpr std::string_view NAME_QUANTITY = "quantities";

		/** @brief Standard calculation codebook name identifier. */
		inline constexpr std::string_view NAME_CALCULATION = "calculations";

		/** @brief Standard state codebook name identifier. */
		inline constexpr std::string_view NAME_STATE = "states";

		/** @brief Standard content codebook name identifier. */
		inline constexpr std::string_view NAME_CONTENT = "contents";

		/** @brief Standard command codebook name identifier. */
		inline constexpr std::string_view NAME_COMMAND = "commands";

		/** @brief Standard type codebook name identifier. */
		inline constexpr std::string_view NAME_TYPE = "types";

		/** @brief Functional services codebook name identifier. */
		inline constexpr std::string_view NAME_FUNCTIONAL_SERVICES = "functional_services";

		/** @brief Maintenance category codebook name identifier. */
		inline constexpr std::string_view NAME_MAINTENANCE_CATEGORY = "maintenance_category";

		/** @brief Activity type codebook name identifier. */
		inline constexpr std::string_view NAME_ACTIVITY_TYPE = "activity_type";

		/** @brief Detail codebook name identifier. */
		inline constexpr std::string_view NAME_DETAIL = "detail";

		//----------------------------------------------
		// Codebook enum names (for error messages)
		//----------------------------------------------

		/** @brief Enum name for Position codebook. */
		inline constexpr std::string_view ENUM_POSITION = "Position";

		/** @brief Enum name for Quantity codebook. */
		inline constexpr std::string_view ENUM_QUANTITY = "Quantity";

		/** @brief Enum name for Calculation codebook. */
		inline constexpr std::string_view ENUM_CALCULATION = "Calculation";

		/** @brief Enum name for State codebook. */
		inline constexpr std::string_view ENUM_STATE = "State";

		/** @brief Enum name for Content codebook. */
		inline constexpr std::string_view ENUM_CONTENT = "Content";

		/** @brief Enum name for Command codebook. */
		inline constexpr std::string_view ENUM_COMMAND = "Command";

		/** @brief Enum name for Type codebook. */
		inline constexpr std::string_view ENUM_TYPE = "Type";

		/** @brief Enum name for FunctionalServices codebook. */
		inline constexpr std::string_view ENUM_FUNCTIONAL_SERVICES = "FunctionalServices";

		/** @brief Enum name for MaintenanceCategory codebook. */
		inline constexpr std::string_view ENUM_MAINTENANCE_CATEGORY = "MaintenanceCategory";

		/** @brief Enum name for ActivityType codebook. */
		inline constexpr std::string_view ENUM_ACTIVITY_TYPE = "ActivityType";

		/** @brief Enum name for Detail codebook. */
		inline constexpr std::string_view ENUM_DETAIL = "Detail";

		//----------------------------------------------
		// Codebooks prefix constants
		//----------------------------------------------

		/** @brief Prefix identifier for position-related codebook entries. */
		inline constexpr std::string_view PREFIX_POSITION = "pos";

		/** @brief Prefix identifier for quantity-related codebook entries. */
		inline constexpr std::string_view PREFIX_QUANTITY = "qty";

		/** @brief Prefix identifier for calculation-related codebook entries. */
		inline constexpr std::string_view PREFIX_CALCULATION = "calc";

		/** @brief Prefix identifier for state-related codebook entries. */
		inline constexpr std::string_view PREFIX_STATE = "state";

		/** @brief Prefix identifier for content-related codebook entries. */
		inline constexpr std::string_view PREFIX_CONTENT = "cnt";

		/** @brief Prefix identifier for command-related codebook entries. */
		inline constexpr std::string_view PREFIX_COMMAND = "cmd";

		/** @brief Prefix identifier for type-related codebook entries. */
		inline constexpr std::string_view PREFIX_TYPE = "type";

		/** @brief Prefix identifier for functional services codebook entries. */
		inline constexpr std::string_view PREFIX_FUNCTIONAL_SERVICES = "funct.svc";

		/** @brief Prefix identifier for maintenance category codebook entries. */
		inline constexpr std::string_view PREFIX_MAINTENANCE_CATEGORY = "maint.cat";

		/** @brief Prefix identifier for activity type codebook entries. */
		inline constexpr std::string_view PREFIX_ACTIVITY_TYPE = "act.type";

		/** @brief Prefix identifier for detail-related codebook entries. */
		inline constexpr std::string_view PREFIX_DETAIL = "detail";

		//----------------------------------------------
		// Position validation group constants
		//----------------------------------------------

		/** @brief Special group identifier for numeric-only values in VISTA codebooks. */
		inline constexpr std::string_view GROUP_NUMBER = "<number>";

		/** @brief Default group name for ungrouped position components. */
		inline constexpr std::string_view GROUP_DEFAULT = "DEFAULT_GROUP";

		/** @brief Fallback group identifier for unrecognized position components. */
		inline constexpr std::string_view GROUP_UNKNOWN = "UNKNOWN";

		//----------------------------------------------
		// Position validation results constants
		//----------------------------------------------

		/** @brief Result constant indicating invalid position validation. */
		inline constexpr std::string_view POSITION_VALIDATION_INVALID = "invalid";

		/** @brief Result constant indicating invalid position component ordering. */
		inline constexpr std::string_view POSITION_VALIDATION_INVALID_ORDER = "invalidorder";

		/** @brief Result constant indicating invalid position component grouping. */
		inline constexpr std::string_view POSITION_VALIDATION_INVALID_GROUPING = "invalidgrouping";

		/** @brief Result constant indicating valid position validation. */
		inline constexpr std::string_view POSITION_VALIDATION_VALID = "valid";

		/** @brief Result constant indicating custom position validation rules applied. */
		inline constexpr std::string_view POSITION_VALIDATION_CUSTOM = "custom";
	} // namespace codebook
} // namespace dnv::vista::sdk::internal
