/**
 * @file config.h
 * @brief Cross-platform compiler support and performance optimization macros
 * @details Provides SIMD intrinsics headers and force inline directives for
 *          maximum performance across MSVC, GCC, and Clang compilers
 */

#pragma once

//=====================================================================
// Cross-platform compiler support
//=====================================================================

//----------------------------------------------
// Compiler intrinsics headers
//----------------------------------------------

/** @brief Platform-specific intrinsics headers for SIMD and hardware optimization support */
#ifdef _MSC_VER
#	include <intrin.h>
#elif defined( __GNUC__ ) || defined( __clang__ )
#	include <immintrin.h>
#	include <x86intrin.h>
#endif

//----------------------------------------------
// Hash algorithm constants
//----------------------------------------------

/** @brief FNV-1a 32-bit offset basis constant for hash algorithm initialization */
#define VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS 0x811C9DC5

/** @brief FNV-1a 32-bit prime multiplier constant for hash algorithm computation */
#define VISTA_SDK_CPP_HASH_FNV_PRIME 0x01000193
