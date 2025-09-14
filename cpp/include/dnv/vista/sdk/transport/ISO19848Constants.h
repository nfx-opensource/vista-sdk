/**
 * @file ISO19848Constants.h
 * @brief ISO-19848 Standard Constants for Maritime Data Exchange
 *
 * @details
 * This file provides compile-time constants for the **ISO-19848 Standard** implementation.
 * It contains all standardized identifiers, version information, and naming conventions
 * required for compliant maritime data exchange systems.
 *
 * ## Purpose:
 *
 * **ISO-19848 Constants** provide:
 * - **Version Identifiers**: Standard version strings for ISO-19848:2018 and ISO-19848:2024
 * - **Naming Conventions** : Annex C naming rules and entity identifiers
 * - **Reference URLs**     : Official documentation and specification links
 * - **Compile-Time Safety**: All constants are `constexpr` for zero runtime overhead
 *
 * ## Annex C Naming System:
 *
 * **ISO-19848 Annex C** defines standardized naming conventions for maritime data:
 * - **Naming Entity**: Official authority for naming rules (`data.dnv.com`)
 * - **Naming Rule**  : Structured identifier format (`dnv-v2`)
 * - **Versioning**   : Scheme version tracking for backward compatibility
 * - **Documentation**: Reference URLs for specification details
 *
 * ## Standard Compliance:
 *
 * All constants are derived from the official ISO-19848 specification:
 * - **ISO-19848:2018**: Original maritime data exchange standard
 * - **ISO-19848:2024**: Latest revision with enhanced naming conventions
 * - **Annex C**       : Standardized naming rules for data channel identification
 * - **DNV Authority** : Official naming entity for maritime industry standards
 *
 * @note These constants provide the foundation for all naming and versioning
 *       operations in the Vista SDK's ISO-19848 implementation. They ensure
 *       consistent, standards-compliant maritime data exchange.
 */
#pragma once

#include <string>
#include <string_view>

namespace dnv::vista::sdk::transport
{
	inline constexpr std::string_view ISO19848_v2018 = "v2018";
	inline constexpr std::string_view ISO19848_v2024 = "v2024";

	inline constexpr std::string_view ISO19848_ANNEX_C_NAMING_ENTITY = "data.dnv.com";
	inline constexpr std::string_view ISO19848_ANNEX_C_REFERENCE_URL = "https://docs.vista.dnv.com";
	inline constexpr std::string_view ISO19848_ANNEX_C_NAMING_RULE_PREFIX = "dnv";
	inline constexpr std::string_view ISO19848_ANNEX_C_NAMING_SCHEME_VERSION = "v2";
	inline constexpr std::string_view ISO19848_ANNEX_C_NAMING_RULE = "dnv-v2";

	// TODO
	// static_assert( ISO19848_ANNEX_C_NAMING_RULE ==
	//			   std::string{ ISO19848_ANNEX_C_NAMING_RULE_PREFIX } + "-" + std::string{ ISO19848_ANNEX_C_NAMING_SCHEME_VERSION } );
}
