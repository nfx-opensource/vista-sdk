/**
 * @file Locations.h
 * @brief VISTA Location Management System for Maritime Spatial Data Validation
 *
 * @details
 * This file implements the **VISTA Location System** for managing and validating
 * standardized maritime spatial positioning data. It provides comprehensive parsing,
 * validation, and construction capabilities for location strings according to VIS standards,
 * supporting hierarchical location definitions and component-wise validation.
 *
 * ## System Purpose:
 *
 * The **VISTA Location System** serves as the foundation for:
 * - **Spatial Validation**    : Ensuring maritime location data conforms to VIS spatial standards
 * - **Location Parsing**      : Converting string representations to validated Location objects
 * - **Component Analysis**    : Breaking down complex locations into constituent character parts
 * - **Group Classification**  : Organizing location components by type (Side, Vertical, Longitudinal, etc.)
 * - **Relative Positioning**  : Managing hierarchical relationships between location components
 * - **Character Code Mapping**: Providing human-readable names for location character codes
 * - **Fast Validation**       : O(1) hash-based lookups for high-performance location checking
 *
 * ## Core Architecture:
 *
 * ### Location Classes
 * - **Location**        : Immutable validated location string representation
 * - **RelativeLocation**: Component linking character codes to human-readable names
 * - **Locations**       : Main manager providing parsing and validation capabilities
 * - **LocationGroup**   : Enumeration classifying location component types
 *
 * ### Validation Framework
 * - **String Parsing**       : Convert location strings to validated immutable objects
 * - **Character Validation** : Verify individual location characters using hash-based lookup
 * - **Component Validation** : Verify individual location components and their combinations
 * - **Group Classification** : Determine component types (Side, Vertical, Longitudinal, etc.)
 * - **Hierarchical Checking**: Validate component combinations and sequences for maritime context
 * - **Error Diagnostics**    : Comprehensive error reporting with detailed failure information
 *
 * ## Data Flow Architecture:
 *
 * ```
 *      LocationsDto (External Data)
 *                  ↓
 *        Locations Construction
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │            Locations                │
 * ├─────────────────────────────────────┤
 * │ ┌─────────────────────────────────┐ │
 * │ │     RelativeLocation List       │ │ ← Component definitions
 * │ │  (vector<RelativeLocation>)     │ │
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │     Location Code Set           │ │ ← O(1) validation lookup
 * │ │   (unordered_set<char>)         │ │
 * │ └─────────────────────────────────┘ │
 * │ ┌─────────────────────────────────┐ │
 * │ │     Group Classification        │ │ ← Component → Group mapping
 * │ │   (map<char, LocationGroup>)    │ │
 * │ └─────────────────────────────────┘ │
 * └─────────────────────────────────────┘
 *                  ↓
 *      Location Parsing & Validation
 *                  ↓
 * ┌─────────────────────────────────────┐
 * │    Location Processing Operations   │
 * ├─────────────────────────────────────┤
 * │ - O(1) character code validation    │
 * │ - O(1) group classification lookup  │
 * │ - Component sequence parsing        │
 * │ - Location object construction      │
 * │ - Hierarchical validation checks    │
 * │ - Error reporting and diagnostics   │
 * └─────────────────────────────────────┘
 *```
 *
 * ## Usage Patterns:
 *
 * ### Basic Location Parsing
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Component Analysis
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Performance Characteristics:
 *
 * - **O(1) Character Validation**: Hash-based `std::unordered_set<char>` for constant-time lookups
 * - **O(1) Group Classification**: `std::map<char, LocationGroup>` for fast component type determination
 * - **Zero-Copy Access**         : `string_view` interfaces minimize memory allocation overhead
 * - **Immutable Design**         : Thread-safe operations with immutable location objects
 * - **Fast Parsing**             : Optimized string parsing with minimal temporary allocations
 * - **Efficient Storage**        : Compact representation with minimal memory overhead per location
 * - **Container Optimization**   : STL containers optimized
 *
 * ## Location Component Types:
 *
 * ### LocationGroup Classifications
 * - **Number**      : Numeric identifiers (e.g., 1-9 for deck levels, compartment numbers)
 * - **Side**        : Port/Starboard positioning (e.g., P for Port, S for Starboard)
 * - **Vertical**    : Upper/Middle/Lower positioning (e.g., U for Upper, M for Middle, L for Lower)
 * - **Transverse**  : Cross-ship positioning (e.g., C for Center, I for Inboard, O for Outboard)
 * - **Longitudinal**: Fore/Aft positioning (e.g., F for Forward, A for Aft, M for Midships)
 *
 * ### Validation Levels
 * - **Character Validation**: Verify individual characters are valid location codes
 * - **Sequence Validation** : Check component ordering follows VIS standards
 * - **Group Validation**    : Ensure component combinations are valid
 * - **Semantic Validation** : Verify location makes sense in maritime context
 *
 * ## Design Philosophy:
 *
 * - **Standards Compliance**: Full adherence to VIS location specifications and maritime conventions
 * - **Type Safety**         : Strong typing with LocationGroup enums prevents invalid construction
 * - **Performance First**   : Hash-based lookups optimized for high-frequency maritime data processing
 * - **Immutable Objects**   : Thread-safe design with immutable location objects after construction
 * - **Intuitive API**       : Clear, discoverable interfaces for common location validation operations
 * - **Comprehensive Errors**: Detailed error reporting with precise failure diagnostics
 * - **Memory Efficiency**   : Minimal overhead with efficient container usage and string handling
 */

/**
 * @brief Standard Maritime Location Codes Reference
 *
 * ## Location Code Classifications:
 *
 * ### Longitudinal Positioning (Fore-Aft):
 * - **A** - Aft (stern/rear section of vessel)
 * - **F** - Forward (bow/front section of vessel)
 *
 * ### Transverse Positioning (Port-Starboard):
 * - **P** - Port (left side when facing forward)
 * - **S** - Starboard (right side when facing forward)
 * - **C** - Centre (centerline/midship position)
 *
 * ### Vertical Positioning (Height Levels):
 * - **U** - Upper (higher elevation/deck level)
 * - **M** - Middle (intermediate elevation)
 * - **L** - Lower (base level/lower deck)
 *
 * ### Radial Positioning (Inboard-Outboard):
 * - **I** - Inner (closer to vessel centerline)
 * - **O** - Outer (farther from vessel centerline)
 *
 * ### Orientation & Directional:
 * - **H** - Horizontal (horizontal orientation)
 * - **V** - Vertical-diagonal (vertical or diagonal orientation)
 *
 * ### Numeric Identifiers:
 * - **N** - No. n (numbered locations for sequential identification)
 *
 * ## Usage in Maritime Systems:
 * These codes combine to form precise location identifiers (e.g., "PSU" = Port-Starboard-Upper).
 */

#pragma once

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	enum class VisVersion : std::uint16_t;
	struct LocationsDto;
	class ParsingErrors;

	namespace internal
	{
		class LocationParsingErrorBuilder;
	}

	//=====================================================================
	// Enumerations
	//=====================================================================

	/**
	 * @brief Enumeration of location groups.
	 *
	 * Represents different groups of locations used in the VIS system,
	 * such as side, vertical position, etc.
	 */
	enum class LocationGroup
	{
		Number,
		Side,
		Vertical,
		Transverse,
		Longitudinal
	};

	//=====================================================================
	// Location Class
	//=====================================================================

	/**
	 * @brief Represents a validated location string in the VIS system.
	 *
	 * @details This class encapsulates a location string (e.g., "P", "CL1", "P1U") with
	 *          cached hash for optimal performance in hash-based containers.
	 *          Instances are typically created via parsing methods in the `Locations` class.
	 *          This class is immutable; its value is set at construction.
	 *
	 * ## Memory Layout & Performance:
	 *
	 * ```
	 * Location Structure:
	 * ┌─────────────────────────────────────┐
	 * │              Location               │
	 * ├─────────────────────────────────────┤
	 * │      std::size_t m_hashCode         │ ← Cached hash (8 bytes, O(1) access)
	 * ├─────────────────────────────────────┤
	 * │       std::string m_value           │ ← Location string (e.g., "P", "CL1")
	 * └─────────────────────────────────────┘
	 *                  ↓
	 *        Cached Hash Performance
	 *                  ↓
	 * ┌─────────────────────────────────────┐
	 * │       Hash Container Benefits       │
	 * ├─────────────────────────────────────┤
	 * │ - O(1) hash lookups in containers   │
	 * │ - Pre-computed during construction  │
	 * │ - SSE4.2/FNV-1a optimized hashing   │
	 * │ - Zero recomputation overhead       │
	 * │ - Thread-safe concurrent access     │
	 * └─────────────────────────────────────┘
	 * ```
	 */
	class Location final
	{
		friend class Locations;
		friend class LocationBuilder;

	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor - creates an empty location */
		Location() = default;

	private:
		/**
		 * @brief Constructs a Location object with a specific value.
		 * @param value The location string value.
		 */
		explicit Location( std::string_view value );

	public:
		/**
		 * @brief Copy constructor
		 * @param other The Location instance to copy from
		 */
		Location( const Location& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The Location instance to move from
		 */
		Location( Location&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~Location() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The Location instance to copy from
		 * @return Reference to this Location after assignment
		 */
		Location& operator=( const Location& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The Location instance to move from
		 * @return Reference to this Location after assignment
		 */
		Location& operator=( Location&& other ) noexcept = default;

		//----------------------------------------------
		// Comparison Operators
		//----------------------------------------------

		/**
		 * @brief Equality operator. Compares this Location with another.
		 * @param other The other Location to compare.
		 * @return True if the location values are equal, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator==( const Location& other ) const;

		/**
		 * @brief Inequality operator. Compares this Location with another.
		 * @param other The other Location to compare.
		 * @return True if the location values are not equal, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator!=( const Location& other ) const;

		//----------------------------------------------
		// Conversion Operators
		//----------------------------------------------

		/**
		 * @brief Implicit conversion to std::string.
		 * @return The location value as a string.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline operator std::string() const noexcept;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/*
		 * TODO: DO WE REALLY NEED 3 SIMILAR STRING ACCESS?
		 *
		 * C# has: Value property, ToString() method, implicit operator
		 * C++ has: value(), toString(), operator std::string() - for strict mapping
		 *
		 * All return the same string data. Is this redundancy worth maintaining
		 * for API compatibility, or should we simplify to just one or two methods?
		 */

		/**
		 * @brief Gets the string value of the location.
		 * @return A constant reference to the location string value.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::string& value() const noexcept;

		/**
		 * @brief Gets the string value of the location.
		 * @return A constant reference to the location string value.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::string& toString() const noexcept;

		//----------------------------------------------
		// Hashing
		//----------------------------------------------

		/**
		 * @brief Gets the cached hash value for this Location instance
		 * @details **Purpose**: Enables Location instances to be used efficiently as keys in hash-based
		 *          collections (std::unordered_map, nfx::HashMap, etc.) without performance penalties.
		 *
		 *          **Performance Critical**: Hash is pre-computed during construction using optimized
		 *          SSE4.2/FNV-1a algorithms and cached to avoid expensive recomputation. This transforms
		 *          hash lookups from O(n) string processing to O(1) cached access.
		 *
		 * @return The pre-computed hash value for this Location's string value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::size_t hashCode() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Cached hash value computed during construction for O(1) hash access */
		std::size_t m_hashCode;

		/** @brief The string value representing the location. */
		std::string m_value;
	};

	//=====================================================================
	// RelativeLocation
	//=====================================================================

	/**
	 * @brief Represents a relative location within the VIS system, defined by a code, name, and definition.
	 *
	 * This class links a specific character code (e.g., 'P' for Port) to its human-readable name,
	 * its `Location` object representation, and an optional detailed definition.
	 * This class is immutable after construction.
	 */
	class RelativeLocation final
	{
		friend class Locations;

	private:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs a RelativeLocation object.
		 * @param code The single character code representing this relative location (e.g., 'P', 'C', 'L', '1').
		 * @param name The human-readable name of the relative location (e.g., "Port", "Center", "Lower", "Zone 1").
		 * @param location The `Location` object representing the code (e.g., a `Location` with value "P").
		 * @param definition An optional detailed description of the relative location.
		 */
		RelativeLocation(
			char code, std::string_view name,
			const Location& location,
			const std::optional<std::string> definition = std::nullopt );

		/** @brief Default constructor */
		RelativeLocation() = default;

	public:
		/**
		 * @brief Copy constructor
		 * @param other The RelativeLocation instance to copy from
		 */
		RelativeLocation( const RelativeLocation& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The RelativeLocation instance to move from
		 */
		RelativeLocation( RelativeLocation&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~RelativeLocation() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The RelativeLocation instance to copy from
		 * @return Reference to this RelativeLocation after assignment
		 */
		RelativeLocation& operator=( const RelativeLocation& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The RelativeLocation instance to move from
		 * @return Reference to this RelativeLocation after assignment
		 */
		RelativeLocation& operator=( RelativeLocation&& other ) noexcept = default;

		//----------------------------------------------
		// Comparison Operators
		//----------------------------------------------

		/**
		 * @brief Equality operator. Compares based on the `code` member.
		 * @param other The other RelativeLocation to compare.
		 * @return True if the codes are equal, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator==( const RelativeLocation& other ) const;

		/**
		 * @brief Inequality operator. Compares based on the `code` member.
		 * @param other The other RelativeLocation to compare.
		 * @return True if the codes are not equal, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator!=( const RelativeLocation& other ) const;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Gets the character code of the relative location.
		 * @return The character code.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline char code() const noexcept;

		/**
		 * @brief Gets the name of the relative location.
		 * @return A constant reference to the name string.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::string& name() const noexcept;

		/**
		 * @brief Gets the optional definition of the relative location.
		 * @return A constant reference to an optional string containing the definition.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<std::string>& definition() const noexcept;

		/**
		 * @brief Gets the `Location` object associated with this relative location.
		 * @return A constant reference to the `Location` object.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Location& location() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief The single character code for this relative location (e.g., 'P', '1'). */
		char m_code{};

		/** @brief The human-readable name (e.g., "Port", "Zone 1"). */
		std::string m_name;

		/** @brief The `Location` object corresponding to this code (e.g., Location("P")). */
		Location m_location;

		/** @brief An optional detailed definition. */
		std::optional<std::string> m_definition;
	};

	//=====================================================================
	// Locations
	//=====================================================================

	/**
	 * @brief Manages and provides access to all defined locations for a specific VIS version.
	 *
	 * This class serves as the primary entry point for parsing location strings and
	 * retrieving information about predefined relative locations.
	 * An instance of this class is typically initialized with data loaded from a VIS standard definition.
	 */
	class Locations final
	{
		friend class VIS;

	private:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs a Locations manager for a specific VIS version using data from a DTO.
		 * @param version The VIS version this Locations instance will represent.
		 * @param dto The data transfer object containing the location definitions.
		 */
		explicit Locations( VisVersion version, const LocationsDto& dto );

		/** @brief Default constructor. */
		Locations() = default;

	public:
		/**
		 * @brief Copy constructor
		 * @param other The Locations instance to copy from
		 */
		Locations( const Locations& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The Locations instance to move from
		 */
		Locations( Locations&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~Locations() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The Locations instance to copy from
		 * @return Reference to this Locations after assignment
		 */
		Locations& operator=( const Locations& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The Locations instance to move from
		 * @return Reference to this Locations after assignment
		 */
		Locations& operator=( Locations&& other ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Gets the VIS version this Locations instance pertains to.
		 * @return The `VisVersion` enum value.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VisVersion visVersion() const noexcept;

		/**
		 * @brief Gets a read-only list of all defined relative locations.
		 * @return A constant reference to a vector of `RelativeLocation` objects.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] const std::vector<RelativeLocation>& relativeLocations() const noexcept;

		/**
		 * @brief Gets a read-only map of location groups to their respective relative locations.
		 * @return A constant reference to an unordered_map where keys are `LocationGroup`
		 *         and values are vectors of `RelativeLocation` objects belonging to that group.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] const std::unordered_map<LocationGroup, std::vector<RelativeLocation>>& groups() const noexcept;

		/**
		 * @brief Gets a read-only map from character codes to their corresponding location groups.
		 * @details This map is used for validation purposes to determine which LocationGroup
		 *          a specific character belongs to (e.g., 'P' -> LocationGroup::Side,
		 *          'U' -> LocationGroup::Vertical). Essential for validating location components
		 *          during parsing and building operations.
		 * @return A constant reference to an unordered_map where keys are character codes
		 *         (e.g., 'P', 'C', 'S', 'U', 'M', 'L', 'I', 'O', 'F', 'A') and values
		 *         are their corresponding `LocationGroup` classifications.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] const std::map<char, LocationGroup>& reversedGroups() const noexcept;

		//----------------------------------------------
		// Public methods - Parsing
		//----------------------------------------------

		/**
		 * @brief Parses a location string (represented by a string_view) into a `Location` object.
		 * @param locationStr The location string_view to parse.
		 * @return The parsed `Location` object.
		 * @throws std::invalid_argument If parsing fails.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] Location parse( std::string_view locationStr ) const;

		/**
		 * @brief Tries to parse a location string, providing detailed error information.
		 * @param value An optional string containing the location to parse.
		 * @param location Output parameter: if parsing succeeds, this is set to the parsed `Location`.
		 * @param errors Output parameter: populated with any parsing errors encountered.
		 * @return True if parsing succeeded (even if `errors` object indicates warnings or non-critical issues,
		 *         as long as a valid `Location` could be formed), false if a fundamental parsing error occurred.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool tryParse( const std::optional<std::string>& value, Location& location, ParsingErrors& errors ) const;

		/**
		 * @brief Tries to parse a location string (represented by a string_view).
		 * @param value The location string_view to parse.
		 * @param location Output parameter: if parsing succeeds, this is set to the parsed `Location`.
		 * @return True if parsing succeeded, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool tryParse( std::string_view value, Location& location ) const;

		/**
		 * @brief Tries to parse a location string (represented by a string_view), providing detailed error information.
		 * @param value The location string_view to parse.
		 * @param location Output parameter: if parsing succeeds, this is set to the parsed `Location`.
		 * @param errors Output parameter: populated with any parsing errors encountered.
		 * @return True if parsing succeeded, false otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool tryParse( std::string_view value, Location& location, ParsingErrors& errors ) const;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief A hash set of all valid single character location codes for O(1) lookup. */
		std::unordered_set<char> m_locationCodes;

		/** @brief A list of all defined `RelativeLocation` objects for this VIS version. */
		std::vector<RelativeLocation> m_relativeLocations;

		/** @brief A map from character codes to their `LocationGroup` for quick classification. */
		std::map<char, LocationGroup> m_reversedGroups;

		/** @brief The VIS version this `Locations` instance is configured for. */
		VisVersion m_visVersion;

		/** @brief A map grouping `RelativeLocation` objects by their `LocationGroup`. */
		std::unordered_map<LocationGroup, std::vector<RelativeLocation>> m_groups;
	};
} // namespace dnv::vista::sdk

#include "detail/Locations.inl"

namespace std
{
	/**
	 * @brief Hash specialization for dnv::vista::sdk::Location
	 * @details Enables Location instances to be used as keys in all hash-based STL containers.
	 */
	template <>
	struct hash<dnv::vista::sdk::Location>
	{
		/**
		 * @brief Returns the cached hash value for optimal performance
		 * @param[in] location The Location instance to hash
		 * @return Pre-computed hash value (O(1) access) using hardware-accelerated algorithms
		 */
		std::size_t operator()( const dnv::vista::sdk::Location& location ) const noexcept
		{
			return location.hashCode();
		}
	};
} // namespace std
