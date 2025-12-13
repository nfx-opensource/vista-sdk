/**
 * @file Containers.h
 * @brief Vista SDK container hashing constants
 * @details Defines stable hash seed values for Vista SDK containers.
 */

#pragma once

#include <nfx/Containers.h>

#include <cstdint>

namespace dnv::vista::sdk
{
    /** @brief Small vector with stack storage for N elements before heap allocation */
    template <typename T, std::size_t N>
    using SmallVector = nfx::containers::SmallVector<T, N>;

    /** @brief FNV-1a hash offset basis (32-bit) used for all Vista SDK containers */
    constexpr std::uint32_t FNV_OFFSET_BASIS_32{ 0x811C9DC5 };

    /** @brief Fast hash set with FNV-1a hashing */
    template <typename T>
    using HashSet = nfx::containers::FastHashSet<T, std::uint32_t, FNV_OFFSET_BASIS_32>;

    /** @brief Fast hash map with FNV-1a hashing */
    template <typename K, typename V>
    using HashMap = nfx::containers::FastHashMap<K, V, std::uint32_t, FNV_OFFSET_BASIS_32>;
} // namespace dnv::vista::sdk
