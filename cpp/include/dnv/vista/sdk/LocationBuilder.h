/**
 * @file LocationBuilder.h
 * @brief VISTA Location Builder System for Fluent VIS Location Construction with Validation
 *
 * @details
 * This file implements the **VISTA Location Builder System** for constructing and validating
 * Location instances through an immutable fluent interface pattern. It provides comprehensive
 * builder capabilities with component-wise validation against VIS standard location specifications,
 * ensuring maritime equipment location identifiers comply with ISO 19848 requirements for
 * vessel coordinate systems and spatial positioning.
 *
 * ## System Purpose:
 *
 * The **VISTA Location Builder System** serves as the foundation for:
 * - **VIS Location Construction**: Step-by-step location building with component validation
 * - **Spatial Coordinate System**: Maritime vessel coordinate system compliance (ISO 19848)
 * - **Component Validation**     : Real-time validation against VIS location group specifications
 * - **Immutable Pattern**        : Each builder operation returns a new validated instance
 * - **Equipment Positioning**    : Precise maritime equipment location identification
 *
 * ## Core Architecture:
 *
 * ### Location Component System
 * - **LocationBuilder**     : Main fluent interface for constructing VIS-compliant locations
 * - **Component Validation**: Real-time validation against LocationGroup mappings
 * - **Immutable Design**    : Each method returns new builder instance (no mutation)
 * - **VIS Version Support** : Version-specific validation rules and component sets
 * - **Factory Pattern**     : Controlled creation through Locations context
 *
 * ### VIS Location Components
 * - **Number Component**     : Numeric identifier (1-based, e.g., 1, 2, 100)
 * - **Side Component**       : Port/Center/Starboard positioning ('P', 'C', 'S')
 * - **Vertical Component**   : Upper/Middle/Lower positioning ('U', 'M', 'L')
 * - **Transverse Component** : Inner/Outer positioning ('I', 'O')
 * - **Longitudinal Component**: Forward/Aft positioning ('F', 'A')
 *
 * ## Data Flow Architecture:
 *
 * ```
 *       Location Construction Request
 *                  ↓
 *        VIS Version & Context Setup
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │          LocationBuilder            │
 * ├─────────────────────────────────────┤
 * │    std::optional<int> m_number      │ ← Numeric component (1+)
 * │    std::optional<char> m_side       │ ← Side: P/C/S
 * │    std::optional<char> m_vertical   │ ← Vertical: U/M/L
 * │    std::optional<char> m_transverse │ ← Transverse: I/O
 * │    std::optional<char> longitudinal │ ← Longitudinal: F/A
 * │    VisVersion m_visVersion          │ ← VIS standard version
 * │    const std::map* m_reversedGroups │ ← Validation mapping
 * └─────────────────────────────────────┘
 *                  ↓
 *       Component-wise Validation
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │      Fluent Builder Operations      │
 * ├─────────────────────────────────────┤
 * │ - withNumber(int) validation        │
 * │ - withSide(char) group checking     │
 * │ - withVertical(char) validation     │
 * │ - withTransverse(char) checking     │
 * │ - withLongitudinal(char) validation │
 * │ - Immutable copy-on-write pattern   │
 * └─────────────────────────────────────┘
 *                  ↓
 *               build()
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │        VIS Location Output          │
 * ├─────────────────────────────────────┤
 * │ - VIS Standard Compliance           │
 * │ - Maritime Equipment Positioning    │
 * │ - Thread-Safe Immutable Object      │
 * └─────────────────────────────────────┘
 *```
 *
 * ## Usage Patterns:
 *
 * ### Basic Location Construction
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Advanced Component Validation
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Location Parsing and Analysis
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Maritime Equipment Positioning
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Performance Characteristics:
 *
 * - **Builder Creation** : O(1) factory method with VIS context initialization
 * - **Component Setting**: O(1) per operation with immediate validation
 * - **Validation Lookup**: O(1) hash map lookup for component group checking
 * - **String Generation**: O(n) where n is total character count in components
 * - **Memory Efficiency**: Optional wrapping minimizes overhead for unset components
 * - **Copy Operations**  : Efficient due to small builder footprint and optional optimization
 * - **Immutable Safety** : Thread-safe operations with no shared mutable state
 *
 * ## VIS Standard Compliance:
 *
 * ### Component Validation Rules
 * - **Number Range**         : Must be >= 1 (VIS specification requirement)
 * - **Character Groups**     : Each component validated against LocationGroup mappings
 * - **Version Compatibility**: Component sets vary by VIS version (3-4a, 3-5a, etc.)
 *
 * ### Maritime Coordinate System
 * - **Side Components**        : Port (P), Center (C), Starboard (S) - vessel width axis
 * - **Vertical Components**    : Upper (U), Middle (M), Lower (L) - vessel height axis
 * - **Transverse Components**  : Inner (I), Outer (O) - inboard/outboard positioning
 * - **Longitudinal Components**: Forward (F), Aft (A) - vessel length axis
 *
 * ## Integration with VIS Ecosystem:
 *
 * ### Location System Integration
 * - **Locations Context**    : Builder factory requires Locations instance for validation
 * - **GMOD Path Integration**: Locations used within Local ID GMOD path structures
 * - **Equipment Mapping**    : Precise positioning for maritime equipment and sensors
 * - **Standards Compliance** : Full adherence to ISO 19848 vessel coordinate specifications
 *
 * ### Builder Pattern Benefits
 * - **Incremental Construction**: Step-by-step building with validation at each stage
 * - **Immutable Safety**        : Thread-safe operations with no builder mutation
 * - **Component Flexibility**   : Optional components support various location specificity levels
 * - **Fluent Interface**        : Readable, self-documenting construction code
 *
 * ## Design Philosophy:
 *
 * - **VIS Standard Fidelity**   : Exact compliance with ISO 19848 location specifications
 * - **Immutability First**      : No builder mutation, always return new instances
 * - **Component Validation**    : Real-time validation prevents invalid location construction
 * - **Maritime Domain Focus**   : Tailored for vessel coordinate systems and equipment positioning
 * - **Performance Optimization**: Efficient optional storage and validation lookup patterns
 * - **Type Safety**             : Strong typing with enum-based validation prevents invalid combinations
 * - **Thread Safety**           : Immutable design enables safe concurrent usage
 */

#pragma once

#include <map>
#include <optional>
#include <string>

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	class Location;
	class Locations;
	enum class LocationGroup;
	enum class VisVersion : std::uint16_t;

	//=====================================================================
	// LocationBuilder class
	//=====================================================================

	/**
	 * @brief A fluent builder for constructing Location objects with validation.
	 * @details Provides a type-safe way to build locations by setting individual components
	 *          (number, side, vertical, transverse, longitudinal) with validation against
	 *          the VIS standard for a specific version.
	 */
	class LocationBuilder final
	{
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Private constructor for internal use.
		 * @param visVersion The VIS version.
		 * @param reversedGroups Pointer to map from characters to their location groups.
		 */
		explicit LocationBuilder( VisVersion visVersion, const std::map<char, LocationGroup>* reversedGroups );

	protected:
		/** @brief Default constructor. */
		LocationBuilder() = default;

	public:
		/**
		 * @brief Copy constructor
		 * @param other The object to copy from
		 */
		LocationBuilder( const LocationBuilder& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The object to move from
		 */
		LocationBuilder( LocationBuilder&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~LocationBuilder() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The LocationBuilder to copy from
		 * @return Reference to this LocationBuilder after assignment
		 */
		LocationBuilder& operator=( const LocationBuilder& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The LocationBuilder to move from
		 * @return Reference to this LocationBuilder after assignment
		 */
		LocationBuilder& operator=( LocationBuilder&& other ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Gets the VIS version this builder is configured for.
		 * @return The VisVersion enum value.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline VisVersion visVersion() const noexcept;

		/**
		 * @brief Gets the numeric component of the location.
		 * @return Optional integer representing the number component.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<int>& number() const noexcept;

		/**
		 * @brief Gets the side component of the location.
		 * @return Optional character representing the side (e.g., 'P', 'C', 'S').
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<char>& side() const noexcept;

		/**
		 * @brief Gets the vertical component of the location.
		 * @return Optional character representing vertical position (e.g., 'U', 'M', 'L').
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<char>& vertical() const noexcept;

		/**
		 * @brief Gets the transverse component of the location.
		 * @return Optional character representing transverse position (e.g., 'I', 'O').
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<char>& transverse() const noexcept;

		/**
		 * @brief Gets the longitudinal component of the location.
		 * @return Optional character representing longitudinal position (e.g., 'F', 'A').
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<char>& longitudinal() const noexcept;

		//----------------------------------------------
		// Conversion and comparison
		//----------------------------------------------

		/**
		 * @brief Generates the string representation of the location.
		 * @details Components are alphabetically sorted as per VIS standard.
		 * @return A string representing the current builder state.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/**
		 * @brief Creates a new LocationBuilder for the specified Locations context.
		 * @param locations The Locations object providing validation context.
		 * @return A new LocationBuilder instance.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static LocationBuilder create( const Locations& locations );

		//----------------------------------------------
		// Build methods (Immutable fluent interface)
		//----------------------------------------------

		//----------------------------
		// Build
		//----------------------------

		/**
		 * @brief Builds a Location from the current builder state.
		 * @return The constructed Location object.
		 * @details The resulting location string is alphabetically sorted according to VIS standards.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] Location build() const;

		//----------------------------
		// Location
		//----------------------------

		/**
		 * @brief Sets all components by parsing an existing Location.
		 * @param location The Location to parse and apply to this builder.
		 * @return A new LocationBuilder instance with the parsed components.
		 * @throws std::invalid_argument If the location contains invalid components.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withLocation( const Location& location ) const;

		//----------------------------
		// Number
		//----------------------------

		/**
		 * @brief Sets the numeric component.
		 * @param number The number component (must be >= 1).
		 * @return A new LocationBuilder instance with the updated number.
		 * @throws std::invalid_argument If number is less than 1.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withNumber( int number ) const;

		/**
		 * @brief Removes the numeric component.
		 * @return A new LocationBuilder instance without the number component.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withoutNumber() const;

		//----------------------------
		// Side
		//----------------------------

		/**
		 * @brief Sets the side component.
		 * @param side The side character (e.g., 'P', 'C', 'S').
		 * @return A new LocationBuilder instance with the updated side.
		 * @throws std::runtime_error If the character is not a valid side value.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withSide( char side ) const;

		/**
		 * @brief Removes the side component.
		 * @return A new LocationBuilder instance without the side component.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withoutSide() const;

		//----------------------------
		// Vertical
		//----------------------------

		/**
		 * @brief Sets the vertical component.
		 * @param vertical The vertical character (e.g., 'U', 'M', 'L').
		 * @return A new LocationBuilder instance with the updated vertical.
		 * @throws std::runtime_error If the character is not a valid vertical value.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withVertical( char vertical ) const;

		/**
		 * @brief Removes the vertical component.
		 * @return A new LocationBuilder instance without the vertical component.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withoutVertical() const;

		//----------------------------
		// Transverse
		//----------------------------

		/**
		 * @brief Sets the transverse component.
		 * @param transverse The transverse character (e.g., 'I', 'O').
		 * @return A new LocationBuilder instance with the updated transverse.
		 * @throws std::runtime_error If the character is not a valid transverse value.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withTransverse( char transverse ) const;

		/**
		 * @brief Removes the transverse component.
		 * @return A new LocationBuilder instance without the transverse component.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withoutTransverse() const;

		//----------------------------
		// Longitudinal
		//----------------------------

		/**
		 * @brief Sets the longitudinal component.
		 * @param longitudinal The longitudinal character (e.g., 'F', 'A').
		 * @return A new LocationBuilder instance with the updated longitudinal.
		 * @throws std::runtime_error If the character is not a valid longitudinal value.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withLongitudinal( char longitudinal ) const;

		/**
		 * @brief Removes the longitudinal component.
		 * @return A new LocationBuilder instance without the longitudinal component.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withoutLongitudinal() const;

		//----------------------------
		// Value
		//----------------------------

		/**
		 * @brief Sets a numeric value (equivalent to withNumber).
		 * @param value The numeric value to set.
		 * @return A new LocationBuilder instance with the updated number.
		 * @throws std::invalid_argument If value is less than 1.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withValue( int value ) const;

		/**
		 * @brief Sets a character value to the appropriate component based on validation.
		 * @param value The character value to set.
		 * @return A new LocationBuilder instance with the updated component.
		 * @throws std::invalid_argument If the character is not valid for any component.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withValue( char value ) const;

		/**
		 * @brief Removes a component by group type.
		 * @param group The LocationGroup to remove.
		 * @return A new LocationBuilder instance with the specified component removed.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] LocationBuilder withoutValue( LocationGroup group ) const;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief The numeric component (1-based). */
		std::optional<int> m_number;

		/** @brief The side component ('P', 'C', 'S'). */
		std::optional<char> m_side;

		/** @brief The vertical component ('U', 'M', 'L'). */
		std::optional<char> m_vertical;

		/** @brief The transverse component ('I', 'O'). */
		std::optional<char> m_transverse;

		/** @brief The longitudinal component ('F', 'A'). */
		std::optional<char> m_longitudinal;

		/** @brief The VIS version this builder is configured for. */
		VisVersion m_visVersion;

		/** @brief Pointer to map from character codes to their LocationGroup for validation. */
		const std::map<char, LocationGroup>* m_reversedGroups;
	};
} // namespace dnv::vista::sdk

#include "detail/LocationBuilder.inl"
