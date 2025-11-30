/**
 * @file UniversalId.h
 * @brief VISTA Universal Identification System for Global Maritime Data Identity
 *
 * @details
 * This file implements the **VISTA Universal ID System** for creating globally unique
 * identifiers that combine vessel IMO numbers with Local IDs to provide unambiguous
 * maritime data identification across the global fleet. It serves as the top-level
 * identification mechanism in the VIS standard, ensuring data traceability and
 * preventing identification conflicts in distributed maritime systems.
 *
 * ## System Purpose:
 *
 * The **VISTA Universal ID System** serves as the foundation for:
 * - **Global Identification**: Unique vessel data identification across worldwide maritime operations
 * - **Conflict Prevention**  : Eliminating Local ID conflicts between different vessels
 * - **Data Traceability**    : Complete identification chain from vessel to specific data points
 * - **Standards Compliance** : Full adherence to VIS universal identification specifications
 * - **Distributed Systems**  : Supporting maritime data exchange across multiple organizations
 *
 * ## Core Architecture:
 *
 * ### Universal ID Components
 * - **UniversalId**       : Immutable container combining IMO number and Local ID
 * - **ImoNumber**         : Validated International Maritime Organization vessel identifier
 * - **LocalId**           : Vessel-specific data point identifier with GMOD path and metadata
 * - **UniversalIdBuilder**: Fluent builder for constructing validated Universal IDs
 *
 * ### Identification Hierarchy
 * - **Global Level**   : IMO number uniquely identifies the vessel worldwide
 * - **Vessel Level**   : Local ID identifies specific data point within vessel systems
 * - **Component Level**: GMOD path within Local ID specifies exact data source
 * - **Metadata Level** : Tags within Local ID provide data classification and context
 *
 * ## Data Flow Architecture:
 *
 * ```
 *        Universal ID Construction
 *                  ↓
 *         Component Validation
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │            UniversalId              │
 * ├─────────────────────────────────────┤
 * │ ┌─────────────────────────────────┐ │
 * │ │      UniversalIdBuilder         │ │ ← Centralized data container
 * │ │                                 │ │
 * │ │  ┌───────────────────────────┐  │ │
 * │ │  │       ImoNumber           │  │ │ ← Global vessel identifier
 * │ │  │      (IMO1234567)         │  │ │   (7-digit validated)
 * │ │  └───────────────────────────┘  │ │
 * │ │  ┌───────────────────────────┐  │ │
 * │ │  │    LocalIdBuilder         │  │ │ ← Local ID construction data
 * │ │  │                           │  │ │
 * │ │  │ ┌─────────────────────┐   │  │ │
 * │ │  │ │   LocalIdItems      │   │  │ │ ← GMOD path components
 * │ │  │ │ (Primary+Secondary) │   │  │ │   (Primary + Secondary)
 * │ │  │ └─────────────────────┘   │  │ │
 * │ │  │ ┌─────────────────────┐   │  │ │
 * │ │  │ │   Metadata Tags     │   │  │ │ ← Data classification
 * │ │  │ │(Position,Detail,etc)│   │  │ │   (Position, Detail, etc)
 * │ │  │ └─────────────────────┘   │  │ │
 * │ │  │ ┌─────────────────────┐   │  │ │
 * │ │  │ │   Cached Hash       │   │  │ │ ← Hardware-accelerated hash (O(1) access)
 * │ │  │ └─────────────────────┘   │  │ │
 * │ │  └───────────────────────────┘  │ │
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │       Cached LocalId            │ │ ← Fast accessor cache
 * │ │    (Built from Builder)         │ │   (Immutable instance)
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *                  ↓
 *      Global Unique Identification
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │    Maritime Data Resolution         │
 * ├─────────────────────────────────────┤
 * │ - Fleet-Wide Data Operations        │
 * │ - Cross-System Integration          │
 * │ - Regulatory Compliance Tracking    │
 * │ - Distributed Network Coordination  │
 * │ - STL Container Integration         │
 * │ - Hash-based Operations (O(1))      │
 * └─────────────────────────────────────┘
 *```
 *
 * ## Usage Patterns:
 *
 * ### Universal ID Construction
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Parsing from String Representation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Component Access and Analysis
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Global Maritime Data Operations
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Performance Characteristics:
 *
 * - **Immutable Design**      : Thread-safe operations with no post-construction changes
 * - **Efficient Storage**     : Direct value storage with minimal memory overhead
 * - **Fast Comparison**       : Optimized equality operations for hash map usage
 * - **String Optimization**   : Efficient toString() with string builder pooling
 * - **Validated Construction**: Builder pattern ensures valid state without runtime checks
 *
 * ## String Representation Format:
 *
 * ### Canonical Format
 * ```
 * data.dnv.com/IMO{imo_number}/dnv-v2/vis-{version}/{gmod_path}/{metadata_tags}
 *
 * Examples:
 * - data.dnv.com/IMO1234567/dnv-v2/vis-3-4a/411.1/S90/sec/pos-centre/
 * - data.dnv.com/IMO9876543/dnv-v2/vis-3-5a/621.2/C101/meta-temperature/det-sensor1/
 * ```
 *
 * ### Format Components
 * - **Authority**  : `data.dnv.com` - DNV authority prefix
 * - **Vessel ID**  : `IMO{number}` - International Maritime Organization identifier
 * - **Naming Rule**: `dnv-v2` - DNV naming convention version
 * - **VIS Version**: `vis-{version}` - Vessel Information Structure version
 * - **GMOD Path**  : Hierarchical path to data source component
 * - **Metadata**   : Classification tags for data type and context
 *
 * ## Integration with Maritime Systems:
 *
 * ### Fleet Management Integration
 * - **Vessel Identification**: IMO number links to vessel registry and fleet databases
 * - **Data Point Resolution**: Local ID resolves to specific sensors and data sources
 * - **Cross-System Mapping** : Universal ID enables data correlation across systems
 * - **Compliance Tracking**  : Full traceability for maritime regulatory requirements
 *
 * ### Distributed Maritime Networks
 * - **Data Exchange**    : Universal IDs prevent conflicts in multi-vessel operations
 * - **Shore Integration**: Enables seamless vessel-to-shore data transmission
 * - **Third-Party APIs** : Provides standard identification for maritime service providers
 * - **Analytics Systems**: Supports fleet-wide data aggregation and analysis
 *
 * ## Design Philosophy:
 *
 * - **Global Uniqueness**   : IMO + Local ID combination ensures worldwide uniqueness
 * - **Standards Compliance**: Full adherence to VIS and IMO identification standards
 * - **Immutability**        : Thread-safe design with immutable identification objects
 * - **Builder Pattern**     : Safe construction with validation before object creation
 * - **Performance Focus**   : Optimized for high-frequency maritime data operations
 * - **Maritime Domain**     : Tailored for real-world vessel operations and data flows
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "config/config.h"
#include "ImoNumber.h"
#include "LocalId.h"
#include "UniversalIdBuilder.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	class ParsingErrors;

	//=====================================================================
	// UniversalId class
	//=====================================================================

	/**
	 * @class UniversalId
	 * @brief Universal ID with direct value storage.
	 *
	 * @details Immutable Universal ID combining vessel IMO number with Local ID.
	 */
	class UniversalId final
	{
		friend class UniversalIdBuilder;

	private:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs UniversalId from validated UniversalIdBuilder.
		 * @param[in] builder Valid UniversalIdBuilder instance.
		 * @throws std::invalid_argument If builder is invalid.
		 */
		explicit UniversalId( const UniversalIdBuilder& builder );

		/** @brief Default constructor */
		UniversalId() = delete;

	public:
		/**
		 * @brief Copy constructor
		 * @param other The object to copy from
		 */
		UniversalId( const UniversalId& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The object to move from
		 */
		UniversalId( UniversalId&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~UniversalId() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this UniversalId after assignment
		 */
		UniversalId& operator=( const UniversalId& ) = delete;

		/**
		 * @brief Move assignment operator
		 * @param other The object to move from
		 * @return Reference to this UniversalId after assignment
		 */
		UniversalId& operator=( UniversalId&& other ) noexcept = default;

		//----------------------------------------------
		// Operators
		//----------------------------------------------

		/**
		 * @brief Equality operator.
		 * @param[in] other The other UniversalId to compare.
		 * @return True if equal, false otherwise.
		 */
		inline bool operator==( const UniversalId& other ) const noexcept;

		/**
		 * @brief Inequality operator.
		 * @param[in] other The other UniversalId to compare.
		 * @return True if not equal, false otherwise.
		 */
		inline bool operator!=( const UniversalId& other ) const noexcept;

		/**
		 * @brief Deep equality comparison.
		 * @param[in] other The other UniversalId to compare.
		 * @return True if all components are equal.
		 */
		inline bool equals( const UniversalId& other ) const noexcept;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Gets the IMO number component.
		 * @return Constant reference to the IMO number.
		 * @note Guaranteed to be valid for successfully constructed UniversalId.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const ImoNumber& imoNumber() const noexcept;

		/**
		 * @brief Gets the Local ID component.
		 * @return Constant reference to the Local ID.
		 * @note Guaranteed to be valid for successfully constructed UniversalId.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const LocalId& localId() const noexcept;

		//----------------------------------------------
		// Hashing
		//----------------------------------------------

		/**
		 * @brief Gets the cached hash value for this UniversalId instance.
		 * @return The cached hash value computed using hardware-accelerated algorithms.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::size_t hashCode() const noexcept;

		//----------------------------------------------
		// String conversion
		//----------------------------------------------

		/**
		 * @brief Converts to canonical string representation.
		 * @return String in format "data.dnv.com/IMO1234567/dnv-v2/vis-3-4a/...".
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		//----------------------------------------------
		// Static parsing methods
		//----------------------------------------------

		/**
		 * @brief Parses UniversalId from string representation.
		 * @param[in] universalIdStr String to parse.
		 * @return Parsed UniversalId.
		 * @throws std::invalid_argument If parsing fails.
		 */
		static UniversalId parse( std::string_view universalIdStr );

		/**
		 * @brief Attempts to parse UniversalId from string.
		 * @param[in] universalIdStr String to parse.
		 * @param[out] errors Parsing errors if unsuccessful.
		 * @param[out] universalId Parsed result if successful.
		 * @return True if parsing succeeded.
		 */
		static bool tryParse( std::string_view universalIdStr, ParsingErrors& errors, std::optional<UniversalId>& universalId );

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Builder instance that contains all data */
		UniversalIdBuilder m_builder;

		/** @brief Cached LocalId built from builder for fast access */
		LocalId m_localId;
	};
} // namespace dnv::vista::sdk

#include "detail/UniversalId.inl"

namespace std
{
	/**
	 * @brief Hash specialization for dnv::vista::sdk::UniversalId.
	 * @details Enables UniversalId instances to be used as keys in all hash-based STL containers.
	 */
	template <>
	struct hash<dnv::vista::sdk::UniversalId>
	{
		/**
		 * @brief Returns the cached hash value for optimal performance.
		 * @param[in] universalId The UniversalId instance to hash.
		 * @return Pre-computed hash value (O(1) access) using hardware-accelerated algorithms.
		 */
		std::size_t operator()( const dnv::vista::sdk::UniversalId& universalId ) const noexcept
		{
			return universalId.hashCode();
		}
	};
} // namespace std
