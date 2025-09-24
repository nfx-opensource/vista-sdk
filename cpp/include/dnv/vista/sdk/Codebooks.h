/**
 * @file Codebooks.h
 * @brief VISTA Codebooks Container for Maritime Data Validation Management
 *
 * @details
 * This file implements the **VISTA Codebooks Container System** for managing
 * collections of standardized maritime data vocabularies. It provides unified
 * access to all codebooks within a specific VIS version, with optimized
 * retrieval, validation, and metadata tag creation capabilities.
 *
 * ## System Purpose:
 *
 * The **VISTA Codebooks Container** serves as the central hub for:
 * - **Multi-Codebook Management**: Unified access to all 11 ISO 19848 codebook types
 * - **Version-Specific Access**  : Codebooks organized by VIS version for compatibility
 * - **High-Performance Lookup**  : O(1) codebook retrieval with compile-time optimization
 * - **Metadata Tag Creation**    : Centralized validation and tag generation across codebooks
 * - **Iterator Support**         : STL-compatible iteration over all codebooks
 *
 * ## Core Architecture:
 *
 * ### Container Design
 * - **Codebooks**       : Main container class holding all codebooks for a VIS version
 * - **Fixed-Size Array**: `std::array` with compile-time size for optimal performance
 * - **Direct Indexing** : Enum-based indexing for zero-overhead codebook access
 * - **Immutable Design**: Thread-safe container that cannot be modified after construction
 *
 * ### Access Patterns
 * - **operator[]**      : Ultra-fast unchecked access for performance-critical code
 * - **codebook()**      : Safe checked access with bounds validation and error handling
 * - **Iterator Support**: Range-based loops and STL algorithm compatibility
 * - **Tag Creation**    : Unified validation and metadata tag generation
 *
 * ## Memory Layout & Performance:
 *
 * ```
 * Codebooks Container Structure:
 * ┌─────────────────────────────────────┐
 * │           Codebooks                 │
 * ├─────────────────────────────────────┤
 * │ VisVersion m_visVersion             │ ← 2 bytes (uint16_t)
 * │ ┌─────────────────────────────────┐ │
 * │ │   std::array<Codebook, 11>      │ │ ← Stack-allocated, contiguous
 * │ │ Codebook instances for:         │ │
 * │ │ - Quantity, Content, Calc, etc. │ │
 * │ │ - All 11 ISO 19848 types        │ │
 * │ │ - Direct enum-based indexing    │ │
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *
 * Key Performance Features:
 * - Stack allocation (no heap overhead)
 * - Contiguous memory layout (cache-friendly)
 * - Direct indexing: codebooks[CodebookName::Position]
 * - Compile-time size determination
 * ```
 *
 * ## Usage Examples:
 *
 * ### Basic Codebook Access
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Metadata Tag Creation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Iteration Over All Codebooks
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Performance Characteristics:
 *
 * - **O(1) Codebook Access** : Direct array indexing with enum values
 * - **Zero Indirection**     : Stack-allocated array, no pointer dereferencing
 * - **Cache-Friendly Layout**: Contiguous memory layout for optimal CPU cache usage
 * - **Compile-Time Sizing**  : Fixed array size known at compile time
 * - **Thread-Safe Reads**    : Immutable design allows concurrent read access
 * - **Memory Efficient**     : No dynamic allocation overhead after construction
 *
 * ## Design Philosophy:
 *
 * - **Performance First**   : Optimized for high-frequency maritime data processing
 * - **Type Safety**         : Strong enum typing prevents invalid codebook access
 * - **Standards Compliance**: Full ISO 19848 codebook specification support
 * - **Version Management**  : Clear separation of different VIS versions
 * - **Ease of Use**         : Intuitive API for both performance and safety needs
 * - **Immutability**        : Thread-safe design with no post-construction changes
 */

#pragma once

#include <array>
#include <optional>
#include <string>
#include <string_view>

#include "Codebook.h"
#include "CodebookName.h"
#include "MetadataTag.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	class CodebooksDto;
	enum class VisVersion : std::uint16_t;

	//=====================================================================
	// Constants
	//=====================================================================

	//=====================================================================
	// Codebooks class
	//=====================================================================

	/**
	 * @class Codebooks
	 * @brief Container for all codebooks in a specific VIS version.
	 * @details Provides access to standard codebooks defined in ISO 19848,
	 *          supports iteration over codebooks and metadata tag creation.
	 *          This container is immutable after construction.
	 */
	class Codebooks final
	{
		//----------------------------------------------
		// Friend class declarations
		//----------------------------------------------

		friend class VIS;

	private:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct codebooks from a Data Transfer Object (DTO).
		 * @details Initializes the container with codebooks based on the provided DTO
		 *          for the specified VIS version. Ensures the 'detail' codebook exists.
		 * @param version The `VisVersion` these codebooks belong to.
		 * @param dto The `CodebooksDto` containing the raw codebook data.
		 */
		explicit Codebooks( VisVersion version, const CodebooksDto& dto );

		/** @brief Default constructor */
		Codebooks() = default;

		/** @brief Copy constructor */
		Codebooks( const Codebooks& other );

	public:
		/**
		 * @brief Move constructor
		 * @param other The object to move from
		 */
		Codebooks( Codebooks&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~Codebooks() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The object to copy from
		 * @return Reference to this object
		 */
		Codebooks& operator=( const Codebooks& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The object to move from
		 * @return Reference to this object
		 */
		Codebooks& operator=( Codebooks&& other ) noexcept = default;

		//----------------------------------------------
		// Lookup operators
		//----------------------------------------------

		/**
		 * @brief Fast access to codebook by name (no bounds checking)
		 * @param name The codebook name (must be valid)
		 * @return Reference to the requested codebook
		 * @note This method does not validate the input for maximum performance.
		 *       Use codebook() if you need bounds checking.
		 * @warning Undefined behavior if name is invalid
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr const Codebook& operator[]( CodebookName name ) const noexcept;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the VIS version associated with these codebooks.
		 * @return The `VisVersion` enum value.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline VisVersion visVersion() const noexcept;

		/**
		 * @brief Safe access to codebook by name (with bounds checking)
		 * @param name The codebook name
		 * @return Reference to the requested codebook
		 * @throws std::invalid_argument If name is invalid
		 * @note This method always validates the input. Use operator[] for maximum performance
		 *       when you know the input is valid.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Codebook& codebook( CodebookName name ) const;

		//----------------------------------------------
		// Range-based iterator
		//----------------------------------------------

		/** @brief Const iterator type for iterating over codebooks. */
		using Iterator = const Codebook*;

		/**
		 * @brief Returns iterator to the first codebook.
		 * @return Iterator to the beginning of the codebook container
		 * @note Zero-overhead pointer-based iteration
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Iterator begin() const noexcept;

		/**
		 * @brief Returns iterator to one past the last codebook.
		 * @return Iterator to the end of the codebook container
		 * @note Zero-overhead pointer-based iteration
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Iterator end() const noexcept;

		//----------------------------------------------
		// Tag creation
		//----------------------------------------------

		/**
		 * @brief Try to create a metadata tag using the appropriate codebook for validation.
		 * @details Attempts to validate the `value` against the codebook specified by `name`.
		 *          If validation succeeds, a `MetadataTag` is created and returned.
		 * @param name The `CodebookName` identifying which codebook to use for validation.
		 * @param value The string value for the tag. Can be empty.
		 * @return An `std::optional<MetadataTag>` containing the created tag if the value is valid
		 *         (or if the codebook allows any value, like 'detail'), otherwise `std::nullopt`.
		 * @throws std::invalid_argument If the `name` enum value is invalid (propagated from `operator[]`).
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::optional<MetadataTag> tryCreateTag( CodebookName name, std::string_view value ) const;

		/**
		 * @brief Create a metadata tag using the appropriate codebook, throwing on failure.
		 * @details Validates the `value` against the codebook specified by `name`.
		 *          If validation succeeds, a `MetadataTag` is created and returned.
		 *          If validation fails, an exception is thrown.
		 * @param name The `CodebookName` identifying which codebook to use for validation.
		 * @param value The string value for the tag. Cannot be empty if validation is required.
		 * @return The created `MetadataTag`.
		 * @throws std::invalid_argument If the `value` is invalid for the specified codebook,
		 *         or if the `name` enum value is invalid.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline MetadataTag createTag( CodebookName name, const std::string& value ) const;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief The VIS version these codebooks belong to. */
		VisVersion m_visVersion;

		/** @brief Fixed-size array holding all codebooks */
		std::array<Codebook, static_cast<size_t>( CodebookName::Detail )> m_codebooks{};
	};
}

#include "detail/Codebooks.inl"
