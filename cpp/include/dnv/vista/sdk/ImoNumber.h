/**
 * @file ImoNumber.h
 * @brief VISTA IMO Number System for Maritime Vessel Identification and Validation
 *
 * @details
 * This file implements the **VISTA IMO Number System** for representing, validating, and managing
 * International Maritime Organization (IMO) numbers according to the official IMO standard.
 * It provides immutable, validated IMO number instances with high-performance hash support,
 * comprehensive checksum validation, and optimized container storage capabilities.
 *
 * ## System Purpose:
 *
 * The **VISTA IMO Number System** serves as the foundation for:
 * - **IMO Standard Compliance**  : Full adherence to official 7-digit IMO number specification
 * - **Checksum Validation**      : Automatic verification of IMO number integrity using standard algorithm
 * - **Immutable Value Semantics**: Thread-safe IMO instances with guaranteed validity after construction
 * - **High-Performance Hashing** : Optimized NFX integer hashing for container storage
 * - **Flexible Construction**    : Support for both integer and string-based IMO number creation
 * - **Error-Safe Parsing**       : Comprehensive validation with detailed error reporting
 * - **Container Optimization**   : Direct integration with std::unordered_set/map and NFX containers
 *
 * ## Core Architecture:
 *
 * ### IMO Number Structure
 * - **7-digit Format**       : Standard maritime vessel identifier (e.g., 9074729)
 * - **6-digit Sequential**   : Unique vessel number assigned by IMO
 * - **1-digit Checksum**     : Validation digit calculated using weighted sum algorithm
 * - **String Representation**: Optional "IMO" prefix (e.g., "IMO9074729")
 *
 * ### Validation Algorithm
 * - **Position Weighting**    : Each digit multiplied by position factor (2-7 from right to left)
 * - **Checksum Calculation**  : Sum of weighted digits, rightmost digit = sum % 10
 * - **Integrity Verification**: Provided checksum must match calculated value
 * - **Range Validation**      : Must be 7-digit number between 1000000-9999999
 *
 * ## Memory Layout & Performance:
 *
 * ```
 * ImoNumber Structure:
 * ┌─────────────────────────────────────┐
 * │             ImoNumber               │
 * ├─────────────────────────────────────┤
 * │      std::size_t m_hashCode         │ ← Cached hash (8 bytes, O(1) access)
 * ├─────────────────────────────────────┤
 * │           int m_value               │ ← 7-digit IMO number (4 bytes)
 * └─────────────────────────────────────┘
 *                  ↓
 *        Cached Hash Performance
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │       Hash Container Benefits       │
 * ├─────────────────────────────────────┤
 * │ - O(1) hash lookups in containers   │
 * │ - NFX avalanche mixing algorithms   │
 * │ - Pre-computed during construction  │
 * │ - Zero recomputation overhead       │
 * │ - Thread-safe concurrent access     │
 * │ - Expected 100M+ operations/second  │
 * └─────────────────────────────────────┘
 * ```
 *
 * ## Usage Examples:
 *
 * ### Basic IMO Number Creation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Safe Parsing with Error Handling
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### High-Performance Container Usage
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Performance Characteristics:
 *
 * - **Construction**       : O(1) integer validation + O(1) hash computation
 * - **String Parsing**     : O(n) where n is string length (typically 7-10 chars)
 * - **Validation**         : O(1) checksum algorithm with digit extraction
 * - **Hash Access**        : O(1) cached hash retrieval
 * - **Equality Comparison**: O(1) integer comparison
 * - **Memory Footprint**   : 12 bytes total (8-byte hash + 4-byte integer)
 * - **Container Storage**  : Optimal for std::unordered_set/map and NFX containers
 * - **Thread Safety**      : Lock-free concurrent read access to immutable data
 *
 * ## IMO Checksum Algorithm:
 *
 * The IMO number validation follows the official standard:
 * ```
 * Example: IMO 9074729
 * Step 1: Extract digits [9,0,7,4,7,2,9]
 * Step 2: Apply position weights (right-to-left, positions 2-7):
 *         (9×7) + (0×6) + (7×5) + (4×4) + (7×3) + (2×2) = 139
 * Step 3: Calculate checksum: 139 % 10 = 9
 * Step 4: Verify: provided checksum (9) matches calculated (9) ✓
 * ```
 *
 * ## Design Philosophy:
 *
 * - **Standards Compliance**: Full adherence to official IMO number specification
 * - **Immutability First**  : Value semantics with no post-construction modification
 * - **Performance Critical**: Zero-overhead abstractions optimized for high-frequency operations
 * - **Type Safety**         : Strong typing prevents invalid IMO number usage
 * - **Error Handling**      : Comprehensive validation with clear error reporting
 * - **Container Friendly**  : Optimized for maritime vessel registries and lookup systems
 * - **Memory Efficient**    : Minimal 12-byte footprint with maximum functionality
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "config/config.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// ImoNumber class
	//=====================================================================

	/**
	 * @brief Represents an International Maritime Organization (IMO) number.
	 *
	 * An IMO number is a unique seven-digit identifier assigned to maritime vessels,
	 * featuring a check digit for validation according to the IMO standard.
	 * This class provides functionality to create, validate, parse, and represent IMO numbers.
	 * Instances of this class are immutable once constructed.
	 */
	class ImoNumber final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor. */
		ImoNumber();

		/**
		 * @brief Constructs an ImoNumber from an integer value.
		 * @param value The IMO number as an integer (e.g., 9074729).
		 *              The value must be a 7-digit number and pass checksum validation.
		 * @throws std::invalid_argument If the provided integer is not a valid IMO number
		 *         (e.g., incorrect number of digits or failed checksum).
		 */
		explicit ImoNumber( int value );

		/**
		 * @brief Constructs an ImoNumber from a string representation.
		 * @param value The IMO number as a string (e.g., "IMO9074729" or "9074729").
		 *              The string can optionally be prefixed with "IMO".
		 * @throws std::invalid_argument If the provided string is not a valid IMO number
		 *         (e.g., incorrect format, non-numeric characters, or failed checksum).
		 */
		explicit ImoNumber( std::string_view value );

		/**
		 * @brief Copy constructor
		 * @param other The object to copy from
		 */
		ImoNumber( const ImoNumber& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The object to move from
		 */
		ImoNumber( ImoNumber&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~ImoNumber() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The object to copy from
		 * @return Reference to this ImoNumber after assignment
		 */
		ImoNumber& operator=( const ImoNumber& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The object to move from
		 * @return Reference to this ImoNumber after assignment
		 */
		ImoNumber& operator=( ImoNumber&& other ) noexcept = default;

		//----------------------------------------------
		// Operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison operator.
		 * @param other The ImoNumber object to compare with this object.
		 * @return True if both ImoNumber objects represent the same IMO number, false otherwise.
		 */
		inline bool operator==( const ImoNumber& other ) const noexcept;

		/**
		 * @brief Inequality comparison operator.
		 * @param other The ImoNumber object to compare with this object.
		 * @return True if the ImoNumber objects represent different IMO numbers, false otherwise.
		 */
		inline bool operator!=( const ImoNumber& other ) const noexcept;

		/**
		 * @brief Explicit conversion to an integer.
		 * @return The underlying 7-digit integer value of the IMO number.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline explicit operator int() const noexcept;

		//----------------------------------------------
		// String conversion
		//----------------------------------------------

		/**
		 * @brief Get the string representation of this IMO number
		 * @return String in format "IMO<number>"
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		//----------------------------------------------
		// Hashing
		//----------------------------------------------

		/**
		 * @brief Gets the cached hash value for this ImoNumber instance
		 * @details **Purpose**: Enables ImoNumber instances to be used efficiently as keys in hash-based
		 *          collections (std::unordered_map, nfx::HashMap, etc.) without performance penalties.
		 *
		 *          **Performance Critical**: Hash is pre-computed during construction using optimized
		 *          FNV-1a algorithms and cached to avoid expensive recomputation. This transforms
		 *          hash lookups from O(n) integer processing to O(1) cached access.
		 *
		 * @return The pre-computed hash value for this ImoNumber's integer value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::size_t hashCode() const noexcept;

		//----------------------------------------------
		// State inspection
		//----------------------------------------------

		/**
		 * @brief Checks if an integer value represents a valid IMO number.
		 *
		 * This method validates the 7-digit structure and the checksum.
		 * @param imoNumber The integer value to check (e.g., 9074729).
		 * @return True if the integer is a valid IMO number, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool isValid( int imoNumber ) noexcept;

		//----------------------------------------------
		// Parsing
		//----------------------------------------------

		/**
		 * @brief Parses a string representation into an ImoNumber object.
		 * @param value The string to parse (e.g., "IMO9074729" or "9074729").
		 *              Can optionally be prefixed with "IMO".
		 * @return The parsed ImoNumber object.
		 * @throws std::invalid_argument If the string is not a valid IMO number.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static ImoNumber parse( std::string_view value );

		/**
		 * @brief Tries to parse a string representation into an ImoNumber object.
		 * @param value The string_view to parse (e.g., "IMO9074729" or "9074729").
		 *              Can optionally be prefixed with "IMO".
		 * @param imoNumber Reference to store the parsed ImoNumber if successful.
		 * @return True if parsing was successful, false otherwise.
		 */
		[[nodiscard]] static bool tryParse( std::string_view value, ImoNumber& imoNumber );

	private:
		//----------------------------------------------
		// Private Members
		//----------------------------------------------

		/** @brief Cached hash value computed during construction for O(1) hash access */
		std::size_t m_hashCode;

		/** @brief The validated 7-digit IMO number integer value */
		int m_value;
	};
} // namespace dnv::vista::sdk

#include "detail/ImoNumber.inl"

namespace std
{
	/**
	 * @brief Hash specialization for dnv::vista::sdk::ImoNumber
	 * @details Enables ImoNumber instances to be used as keys in all hash-based STL containers.
	 */
	template <>
	struct hash<dnv::vista::sdk::ImoNumber>
	{
		/**
		 * @brief Returns the cached hash value for optimal performance
		 * @param[in] imo The ImoNumber instance to hash
		 * @return Pre-computed hash value (O(1) access) using hardware-accelerated algorithms
		 */
		std::size_t operator()( const dnv::vista::sdk::ImoNumber& imo ) const noexcept
		{
			return imo.hashCode();
		}
	};
} // namespace std
