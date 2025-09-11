/**
 * @file DtoKeys.h
 * @brief JSON DTO parsing key constants for all Vista SDK data structures
 * @details String constants for parsing Codebooks, GMOD, Locations,
 *          and Versioning JSON data with zero-allocation string_view usage
 */

#pragma once

#include <string_view>

namespace dnv::vista::sdk::internal::constants::dto
{
	//=====================================================================
	// VIS Release Version Keys
	//=====================================================================

	/** @brief VIS release version key for JSON parsing. */
	inline constexpr std::string_view KEY_VIS_RELEASE = "visRelease";

	//=====================================================================
	// Domain-Specific DTO Parsing Keys
	//=====================================================================

	namespace codebook
	{
		//----------------------------------------------
		// Codebook DTO parsing keys
		//----------------------------------------------

		/** @brief Codebook name key for JSON parsing. */
		inline constexpr std::string_view KEY_NAME = "name";

		/** @brief Items array key for codebook JSON parsing. */
		inline constexpr std::string_view KEY_ITEMS = "items";

		/** @brief Values array key for codebook JSON parsing. */
		inline constexpr std::string_view KEY_VALUES = "values";
	}

	namespace gmod
	{
		//----------------------------------------------
		// GMOD DTO parsing keys
		//----------------------------------------------

		/** @brief GMOD name key for JSON parsing. */
		inline constexpr std::string_view KEY_NAME = "name";

		/** @brief Items array key for GMOD JSON parsing. */
		inline constexpr std::string_view KEY_ITEMS = "items";

		//----------------------------------------------
		// GMOD DTO classification keys
		//----------------------------------------------

		/** @brief Category classification key for GMOD JSON parsing. */
		inline constexpr std::string_view KEY_CATEGORY = "category";

		/** @brief Type classification key for GMOD JSON parsing. */
		inline constexpr std::string_view KEY_TYPE = "type";

		/** @brief Code identifier key for GMOD JSON parsing. */
		inline constexpr std::string_view KEY_CODE = "code";

		/** @brief Common name key for GMOD JSON parsing. */
		inline constexpr std::string_view KEY_COMMON_NAME = "commonName";

		/** @brief Definition key for GMOD JSON parsing. */
		inline constexpr std::string_view KEY_DEFINITION = "definition";

		/** @brief Common definition key for GMOD JSON parsing. */
		inline constexpr std::string_view KEY_COMMON_DEFINITION = "commonDefinition";

		/** @brief Install substructure key for GMOD JSON parsing. */
		inline constexpr std::string_view KEY_INSTALL_SUBSTRUCTURE = "installSubstructure";

		/** @brief Normal assignment names key for GMOD JSON parsing. */
		inline constexpr std::string_view KEY_NORMAL_ASSIGNMENT_NAMES = "normalAssignmentNames";

		/** @brief Relations key for GMOD JSON parsing. */
		inline constexpr std::string_view KEY_RELATIONS = "relations";
	}

	namespace gmodversioning
	{
		//----------------------------------------------
		// GMOD versioning DTO parsing keys
		//----------------------------------------------

		/** @brief Items array key for GMOD versioning JSON parsing. */
		inline constexpr std::string_view KEY_ITEMS = "items";

		/** @brief Old assignment key for GMOD versioning JSON parsing. */
		inline constexpr std::string_view KEY_OLD_ASSIGNMENT = "oldAssignment";

		/** @brief Current assignment key for GMOD versioning JSON parsing. */
		inline constexpr std::string_view KEY_CURRENT_ASSIGNMENT = "currentAssignment";

		/** @brief New assignment key for GMOD versioning JSON parsing. */
		inline constexpr std::string_view KEY_NEW_ASSIGNMENT = "newAssignment";

		/** @brief Delete assignment key for GMOD versioning JSON parsing. */
		inline constexpr std::string_view KEY_DELETE_ASSIGNMENT = "deleteAssignment";

		/** @brief Operations key for GMOD versioning JSON parsing. */
		inline constexpr std::string_view KEY_OPERATIONS = "operations";

		/** @brief Source key for GMOD versioning JSON parsing. */
		inline constexpr std::string_view KEY_SOURCE = "source";

		/** @brief Target key for GMOD versioning JSON parsing. */
		inline constexpr std::string_view KEY_TARGET = "target";
	}

	namespace iso19848
	{
		//----------------------------------------------
		// ISO19848 DTO parsing keys
		//----------------------------------------------

		/** @brief Values key for ISO19848 JSON parsing. */
		inline constexpr std::string_view KEY_VALUES = "values";

		/** @brief Type key for ISO19848 JSON parsing. */
		inline constexpr std::string_view KEY_TYPE = "type";

		/** @brief Description key for ISO19848 JSON parsing. */
		inline constexpr std::string_view KEY_DESCRIPTION = "description";
	}

	namespace locations
	{
		//----------------------------------------------
		// Locations DTO parsing keys
		//----------------------------------------------

		/** @brief Code key for locations JSON parsing. */
		inline constexpr std::string_view KEY_CODE = "code";

		/** @brief Name key for locations JSON parsing. */
		inline constexpr std::string_view KEY_NAME = "name";

		/** @brief Definition key for locations JSON parsing. */
		inline constexpr std::string_view KEY_DEFINITION = "definition";

		/** @brief Items array key for locations JSON parsing. */
		inline constexpr std::string_view KEY_ITEMS = "items";
	}

	namespace error
	{
		/** @brief Unknown codebook name fallback constant. */
		inline constexpr std::string_view UNKNOWN_NAME = "[unknown name]";

		/** @brief Unknown GMOD code fallback constant. */
		inline constexpr std::string_view UNKNOWN_CODE = "[unknown code]";

		/** @brief Unknown GMOD version fallback constant. */
		inline constexpr std::string_view UNKNOWN_VERSION = "[unknown version]";

		/** @brief Unknown old assignment fallback constant. */
		inline constexpr std::string_view UNKNOWN_OLD_ASSIGNMENT = "[unknown oldAssignment]";

		/** @brief Unknown source fallback constant. */
		inline constexpr std::string_view UNKNOWN_SOURCE = "[unknown source]";

		/** @brief Unknown ISO19848 type fallback constant. */
		inline constexpr std::string_view UNKNOWN_TYPE = "[unknown type]";
	}
}
