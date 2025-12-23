/**
 * @file ISO19848AnnexC.h
 * @brief ISO 19848 Annex C DNV constants
 */

#pragma once

#include <string_view>

namespace dnv::vista::sdk::iso19848::annexC
{
    /**
     * @brief ISO 19848 Annex C naming entity
     * @details Domain name for universal IDs as per ISO 19848 Annex C
     */
    constexpr std::string_view NamingEntity = "data.dnv.com";

    /**
     * @brief ISO 19848 Annex C naming rule identifier
     * @details Identifier component of the naming rule as per ISO 19848 Annex C
     */
    constexpr std::string_view NamingRule = "dnv";

    /**
     * @brief ISO 19848 Annex C naming scheme version
     * @details Version component of the naming rule as per ISO 19848 Annex C
     */
    constexpr std::string_view NamingSchemeVersion = "v2";

    /**
     * @brief ISO 19848 Annex C full naming rule
     * @details Complete naming rule (NamingRule-NamingSchemeVersion) as per ISO 19848 Annex C
     */
    constexpr std::string_view VersionedNamingRule = "dnv-v2";

    /**
     * @brief ISO 19848 Annex C reference URL
     * @details Reference documentation URL as per ISO 19848 Annex C
     */
    constexpr std::string_view ReferenceUrl = "https://docs.vista.dnv.com";
} // namespace dnv::vista::sdk::iso19848::annexC
