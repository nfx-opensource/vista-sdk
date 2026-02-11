/**
 * @file Json.h
 * @brief JSON type aliases for the Vista SDK
 * @details Provides convenient aliases to nfx::json types within the dnv::vista::sdk namespace
 */

#pragma once

#include <nfx/Json.h>

namespace dnv::vista::sdk::json
{
    /** @brief JSON document builder for creating JSON structures */
    using Builder = nfx::json::Builder;

    /** @brief JSON document representation */
    using Document = nfx::json::Document;

    /** @brief JSON type enumeration */
    using Type = nfx::json::Type;

    /** @brief JSON array type */
    using Array = nfx::json::Array;

    /** @brief JSON object type */
    using Object = nfx::json::Object;
} // namespace dnv::vista::sdk::json
