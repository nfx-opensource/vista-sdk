/**
 * @file Serialization.h
 * @brief Serialization type aliases for the Vista SDK
 * @details Provides convenient aliases to nfx::serialization types within the dnv::vista::sdk namespace
 */

#pragma once

#include "json/Json.h"

#include <nfx/Serialization.h>

namespace dnv::vista::sdk::serialization::json
{
    /** @brief JSON serializer for converting objects to/from JSON */
    template <typename T>
    using Serializer = nfx::serialization::json::Serializer<T>;

    /** @brief Serialization traits for custom type serialization */
    template <typename T>
    using SerializationTraits = nfx::serialization::json::SerializationTraits<T>;
} // namespace dnv::vista::sdk::serialization::json
