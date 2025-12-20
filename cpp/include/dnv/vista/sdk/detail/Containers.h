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
	template <typename T, std::size_t N>
	using SmallVector = nfx::containers::SmallVector<T, N>;

	inline constexpr std::uint32_t FNV_OFFSET_BASIS_32{ 0x811C9DC5 };

	template <typename T>
	using HashSet = nfx::containers::FastHashSet<T, std::uint32_t, FNV_OFFSET_BASIS_32>;

	template <typename K, typename V>
	using HashMap = nfx::containers::FastHashMap<K, V, std::uint32_t, FNV_OFFSET_BASIS_32>;

	template <typename K, typename V>
	using ChdDictionary = nfx::containers::PerfectHashMap<K, V, std::uint32_t, FNV_OFFSET_BASIS_32>;
} // namespace dnv::vista::sdk
