/**
 * @file DataChannel.h
 * @brief Data Channel List Package implementation with comprehensive validation system
 *
 * @details
 * This file implements the **Data Channel List Package** infrastructure as specified in ISO-19848.
 * It provides a complete system for defining, validating, and managing data channel configurations
 * with rich metadata support and comprehensive business rule validation.
 *
 * ## Package Purpose:
 *
 * The Data Channel List Package is used for:
 * - **Configuration Management**: Defining what data channels exist and their properties
 * - **Metadata Storage**        : Rich naming, versioning, and reference information
 * - **Validation Framework**    : Complete ISO-19848 compliance checking
 * - **System Integration**      : Ship identification and configuration references
 *
 * ## Core Classes:
 *
 * ### DataChannelId
 * - **Purpose**   : Rich identifier with LocalId + optional ShortId + optional NameObject
 * - **Structure** : Always contains LocalId, optionally contains ShortId and naming metadata
 * - **Mutability**: Mutable with setters for runtime configuration management
 *
 * ### DataChannel
 * - **Purpose**    : Individual channel definition with validated properties
 * - **Validation** : Comprehensive property validation at construction and modification
 * - **Flexibility**: Full configuration management capabilities
 *
 * ### DataChannelList
 * - **Purpose**   : Collection of data channels with efficient dual indexing
 * - **Indexing**  : Access by index, LocalId, or ShortId with O(1) lookup performance
 * - **Management**: Add, remove, and query operations with automatic index maintenance
 *
 * ### Property System
 * - **Format**      : Data type validation with ISO-19848 format compliance
 * - **Restrictions**: Advanced validation (patterns, ranges, enumerations, length limits)
 * - **Units**       : Comprehensive unit and quantity management
 * - **Ranges**      : Numeric bound validation with business rule checking
 *
 * ## Validation Framework:
 *
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Configuration Example:
 *
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Design Philosophy:
 *
 * This implementation prioritizes:
 * - **Rich Metadata**: Comprehensive configuration and naming support
 * - **Validation**   : Full ISO-19848 compliance with detailed error reporting
 * - **Flexibility**  : Mutable objects for dynamic configuration management
 * - **Type Safety**  : Strong typing with validation at construction and modification
 * - **Performance**  : Efficient indexing and lookup for large channel collections
 *
 * ## Package Structure:
 *
 * ```cpp
 * DataChannelListPackage
 * └── Package
 *     ├── Header (ship info, versioning, author, timestamps, configuration references)
 *     └── DataChannelList
 *         └── DataChannel[] (id + validated properties)
 * ```
 *
 * @note This implementation focuses on configuration management and metadata.
 *       All classes provide comprehensive validation and rich property support.
 */

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include <nfx/containers/HashMap.h>
#include <nfx/containers/StringMap.h>

#include "dnv/vista/sdk/config/config.h"
#include "dnv/vista/sdk/transport/ISO19848.h"
#include "dnv/vista/sdk/transport/ShipId.h"
#include "dnv/vista/sdk/LocalId.h"

//=====================================================================
// Forward declarations
//=====================================================================

namespace nfx::datatypes
{
	class DateTimeOffset;
	class Decimal;
}

namespace dnv::vista::sdk
{
	class ValidateResult;
}

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// WhiteSpace enumeration
	//=====================================================================

	/**
	 * @brief White space handling enumeration for restrictions
	 */
	enum class WhiteSpace : std::uint8_t
	{
		Preserve = 0,
		Replace,
		Collapse
	};

	//=====================================================================
	// VersionInformation class
	//=====================================================================

	/**
	 * @brief Version information for naming rules and schemes
	 * @details Contains static constants and configurable properties
	 */
	class VersionInformation final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor with standard values */
		VISTA_SDK_CPP_INLINE VersionInformation();

		/**
		 * @brief Copy constructor
		 * @param other The VersionInformation to copy from
		 */
		VersionInformation( const VersionInformation& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The VersionInformation to move from
		 */
		VersionInformation( VersionInformation&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~VersionInformation() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The VersionInformation to copy from
		 * @return Reference to this VersionInformation
		 */
		VersionInformation& operator=( const VersionInformation& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The VersionInformation to move from
		 * @return Reference to this VersionInformation
		 */
		VersionInformation& operator=( VersionInformation&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get naming rule
		 * @return Naming rule string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::string& namingRule() const noexcept;

		/**
		 * @brief Get naming scheme version
		 * @return Naming scheme version string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::string& namingSchemeVersion() const noexcept;

		/**
		 * @brief Get optional reference URL
		 * @return Optional reference URL
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::string>& referenceUrl() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set naming rule
		 * @param namingRule New naming rule
		 */
		VISTA_SDK_CPP_INLINE void setNamingRule( std::string_view namingRule );

		/**
		 * @brief Set naming scheme version
		 * @param namingSchemeVersion New naming scheme version
		 */
		VISTA_SDK_CPP_INLINE void setNamingSchemeVersion( std::string_view namingSchemeVersion );

		/**
		 * @brief Set reference URL
		 * @param referenceUrl New optional reference URL
		 */
		VISTA_SDK_CPP_INLINE void setReferenceUrl( std::optional<std::string_view> referenceUrl );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		std::string m_namingRule;
		std::string m_namingSchemeVersion;
		std::optional<std::string> m_referenceUrl;
	};

	//=====================================================================
	// ConfigurationReference class
	//=====================================================================

	/**
	 * @brief Configuration reference with ID, version, and timestamp
	 */
	class ConfigurationReference final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		ConfigurationReference() = delete;

		/**
		 * @brief Constructs ConfigurationReference with required parameters
		 * @param id Configuration identifier
		 * @param timeStamp Configuration timestamp
		 * @param version Optional version string
		 */
		VISTA_SDK_CPP_INLINE ConfigurationReference( std::string_view id, nfx::time::DateTimeOffset timeStamp, std::optional<std::string_view> version = std::nullopt );

		/**
		 * @brief Copy constructor
		 * @param other The ConfigurationReference to copy from
		 */
		ConfigurationReference( const ConfigurationReference& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The ConfigurationReference to move from
		 */
		ConfigurationReference( ConfigurationReference&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~ConfigurationReference() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The ConfigurationReference to copy from
		 * @return Reference to this ConfigurationReference
		 */
		ConfigurationReference& operator=( const ConfigurationReference& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The ConfigurationReference to move from
		 * @return Reference to this ConfigurationReference
		 */
		ConfigurationReference& operator=( ConfigurationReference&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get configuration ID
		 * @return Configuration identifier
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::string& id() const noexcept;

		/**
		 * @brief Get optional version
		 * @return Optional version string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::string>& version() const noexcept;

		/**
		 * @brief Get timestamp
		 * @return Configuration timestamp
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const nfx::time::DateTimeOffset& timeStamp() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set configuration ID
		 * @param id New configuration identifier
		 */
		VISTA_SDK_CPP_INLINE void setId( std::string_view id );

		/**
		 * @brief Set version
		 * @param version New optional version string
		 */
		VISTA_SDK_CPP_INLINE void setVersion( std::optional<std::string_view> version );

		/**
		 * @brief Set timestamp
		 * @param timeStamp New configuration timestamp
		 */
		VISTA_SDK_CPP_INLINE void setTimeStamp( nfx::time::DateTimeOffset timeStamp );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		std::string m_id;
		nfx::time::DateTimeOffset m_timeStamp;
		std::optional<std::string> m_version;
	};

	//=====================================================================
	// Restriction class
	//=====================================================================

	/**
	 * @brief Advanced validation restriction system
	 * @details Implements enumeration, pattern, numeric bounds, length validation.
	 */
	class Restriction final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		Restriction() = default;

		/**
		 * @brief Copy constructor
		 * @param other The Restriction to copy from
		 */
		Restriction( const Restriction& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The Restriction to move from
		 */
		Restriction( Restriction&& other ) noexcept = default;

		/** @brief Destructor */
		~Restriction() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The Restriction to copy from
		 * @return Reference to this Restriction
		 */
		Restriction& operator=( const Restriction& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The Restriction to move from
		 * @return Reference to this Restriction
		 */
		Restriction& operator=( Restriction&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get enumeration list
		 * @return Optional list of acceptable values
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::vector<std::string>>& enumeration() const noexcept;

		/**
		 * @brief Get fraction digits
		 * @return Optional maximum decimal places allowed
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE std::optional<uint32_t> fractionDigits() const noexcept;

		/**
		 * @brief Get exact length
		 * @return Optional exact number of characters required
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE std::optional<uint32_t> length() const noexcept;

		/**
		 * @brief Get maximum exclusive bound
		 * @return Optional upper bound (value must be less than this)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE std::optional<double> maxExclusive() const noexcept;

		/**
		 * @brief Get maximum inclusive bound
		 * @return Optional upper bound (value must be less than or equal)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE std::optional<double> maxInclusive() const noexcept;

		/**
		 * @brief Get maximum length
		 * @return Optional maximum number of characters allowed
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE std::optional<uint32_t> maxLength() const noexcept;

		/**
		 * @brief Get minimum exclusive bound
		 * @return Optional lower bound (value must be greater than this)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE std::optional<double> minExclusive() const noexcept;

		/**
		 * @brief Get minimum inclusive bound
		 * @return Optional lower bound (value must be greater than or equal)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE std::optional<double> minInclusive() const noexcept;

		/**
		 * @brief Get minimum length
		 * @return Optional minimum number of characters required
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE std::optional<uint32_t> minLength() const noexcept;

		/**
		 * @brief Get regex pattern
		 * @return Optional regex pattern for validation
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::string>& pattern() const noexcept;

		/**
		 * @brief Get total digits
		 * @return Optional exact number of digits required
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE std::optional<uint32_t> totalDigits() const noexcept;

		/**
		 * @brief Get whitespace handling
		 * @return Optional whitespace handling mode
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE std::optional<WhiteSpace> whiteSpace() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set enumeration list
		 * @param enumeration New optional enumeration values
		 */
		VISTA_SDK_CPP_INLINE void setEnumeration( std::optional<std::vector<std::string>> enumeration );

		/**
		 * @brief Set fraction digits
		 * @param fractionDigits New optional fraction digits limit
		 */
		VISTA_SDK_CPP_INLINE void setFractionDigits( std::optional<uint32_t> fractionDigits );

		/**
		 * @brief Set exact length
		 * @param length New optional exact length requirement
		 */
		VISTA_SDK_CPP_INLINE void setLength( std::optional<uint32_t> length );

		/**
		 * @brief Set maximum exclusive bound
		 * @param maxExclusive New optional maximum exclusive bound
		 */
		VISTA_SDK_CPP_INLINE void setMaxExclusive( std::optional<double> maxExclusive );

		/**
		 * @brief Set maximum inclusive bound
		 * @param maxInclusive New optional maximum inclusive bound
		 */
		VISTA_SDK_CPP_INLINE void setMaxInclusive( std::optional<double> maxInclusive );

		/**
		 * @brief Set maximum length
		 * @param maxLength New optional maximum length
		 */
		VISTA_SDK_CPP_INLINE void setMaxLength( std::optional<uint32_t> maxLength );

		/**
		 * @brief Set minimum exclusive bound
		 * @param minExclusive New optional minimum exclusive bound
		 */
		VISTA_SDK_CPP_INLINE void setMinExclusive( std::optional<double> minExclusive );

		/**
		 * @brief Set minimum inclusive bound
		 * @param minInclusive New optional minimum inclusive bound
		 */
		VISTA_SDK_CPP_INLINE void setMinInclusive( std::optional<double> minInclusive );

		/**
		 * @brief Set minimum length
		 * @param minLength New optional minimum length
		 */
		VISTA_SDK_CPP_INLINE void setMinLength( std::optional<uint32_t> minLength );

		/**
		 * @brief Set regex pattern
		 * @param pattern New optional regex pattern
		 */
		VISTA_SDK_CPP_INLINE void setPattern( std::optional<std::string> pattern );

		/**
		 * @brief Set total digits with validation
		 * @param totalDigits New optional total digits (must be > 0)
		 * @throws std::invalid_argument If totalDigits == 0
		 */
		void setTotalDigits( std::optional<uint32_t> totalDigits );

		/**
		 * @brief Set whitespace handling
		 * @param whiteSpace New optional whitespace handling mode
		 */
		VISTA_SDK_CPP_INLINE void setWhiteSpace( std::optional<WhiteSpace> whiteSpace );

		//----------------------------------------------
		// Validation
		//----------------------------------------------

		/**
		 * @brief Validate value against all restrictions
		 * @param value String value to validate
		 * @param format Format context for validation
		 * @return ValidateResult with success or error messages
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] ValidateResult validateValue( const std::string& value, const class Format& format ) const;

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		std::optional<std::vector<std::string>> m_enumeration;
		std::optional<uint32_t> m_fractionDigits;
		std::optional<uint32_t> m_length;
		std::optional<double> m_maxExclusive;
		std::optional<double> m_maxInclusive;
		std::optional<uint32_t> m_maxLength;
		std::optional<double> m_minExclusive;
		std::optional<double> m_minInclusive;
		std::optional<uint32_t> m_minLength;
		std::optional<std::string> m_pattern;
		std::optional<uint32_t> m_totalDigits;
		std::optional<WhiteSpace> m_whiteSpace;
	};

	//=====================================================================
	// Range class
	//=====================================================================

	/**
	 * @brief Numeric range with validation
	 * @details Validated numeric bounds with property setters that throw on invalid ranges
	 */
	class Range final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor with min/max values*/
		VISTA_SDK_CPP_INLINE Range();

		/**
		 * @brief Constructs Range with validation
		 * @param low Lower bound (must be <= high)
		 * @param high Upper bound (must be >= low)
		 * @throws std::invalid_argument If low > high
		 */
		VISTA_SDK_CPP_INLINE Range( double low, double high );

		/**
		 * @brief Copy constructor
		 * @param other The Range to copy from
		 */
		Range( const Range& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The Range to move from
		 */
		Range( Range&& other ) noexcept = default;

		/** @brief Destructor */
		~Range() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The Range to copy from
		 * @return Reference to this Range
		 */
		Range& operator=( const Range& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The Range to move from
		 * @return Reference to this Range
		 */
		Range& operator=( Range&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get lower bound
		 * @return Lower bound value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE double low() const noexcept;

		/**
		 * @brief Get upper bound
		 * @return Upper bound value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE double high() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set lower bound with validation
		 * @param low New lower bound
		 * @throws std::invalid_argument If low > current high
		 */
		void setLow( double low );

		/**
		 * @brief Set upper bound with validation
		 * @param high New upper bound
		 * @throws std::invalid_argument If high < current low
		 */
		void setHigh( double high );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		double m_low;
		double m_high;
	};

	//=====================================================================
	// Unit class
	//=====================================================================

	/**
	 * @brief Unit information with symbol and optional quantity name
	 */
	class Unit final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		Unit() = delete;

		/**
		 * @brief Constructs Unit with required symbol
		 * @param unitSymbol Unit symbol
		 */
		VISTA_SDK_CPP_INLINE explicit Unit( std::string_view unitSymbol );

		/**
		 * @brief Copy constructor
		 * @param other The Unit to copy from
		 */
		Unit( const Unit& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The Unit to move from
		 */
		Unit( Unit&& other ) noexcept = default;

		/** @brief Destructor */
		~Unit() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The Unit to copy from
		 * @return Reference to this Unit
		 */
		Unit& operator=( const Unit& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The Unit to move from
		 * @return Reference to this Unit
		 */
		Unit& operator=( Unit&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get unit symbol
		 * @return Unit symbol string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::string& unitSymbol() const noexcept;

		/**
		 * @brief Get optional quantity name
		 * @return Optional quantity name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::string>& quantityName() const noexcept;

		/**
		 * @brief Get optional custom elements
		 * @return Optional custom elements dictionary
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<nfx::containers::StringMap<Value>>& customElements() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set unit symbol
		 * @param unitSymbol New unit symbol
		 */
		VISTA_SDK_CPP_INLINE void setUnitSymbol( std::string_view unitSymbol );

		/**
		 * @brief Set quantity name
		 * @param quantityName New optional quantity name
		 */
		VISTA_SDK_CPP_INLINE void setQuantityName( std::optional<std::string> quantityName );

		/**
		 * @brief Set custom elements
		 * @param customElements New optional custom elements
		 */
		VISTA_SDK_CPP_INLINE void setCustomElements( std::optional<nfx::containers::StringMap<Value>> customElements );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		std::string m_unitSymbol;
		std::optional<std::string> m_quantityName;
		std::optional<nfx::containers::StringMap<Value>> m_customElements;
	};

	//=====================================================================
	// Header class
	//=====================================================================

	/**
	 * @brief Package header with metadata
	 */
	class Header final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		Header() = delete;

		/**
		 * @brief Constructs Header with required fields
		 * @param shipId Ship identifier
		 * @param dataChannelListId Configuration reference for data channel list
		 * @param author Author information
		 */
		VISTA_SDK_CPP_INLINE Header(
			ShipId shipId,
			ConfigurationReference dataChannelListId,
			std::optional<std::string> author );

		/**
		 * @brief Constructs Header with all fields
		 * @param shipId Ship identifier
		 * @param dataChannelListId Configuration reference for data channel list
		 * @param versionInformation Version information
		 * @param author Author information
		 * @param dateCreated Creation timestamp
		 * @param customHeaders Custom headers dictionary
		 */
		VISTA_SDK_CPP_INLINE Header(
			ShipId shipId,
			ConfigurationReference dataChannelListId,
			std::optional<VersionInformation> versionInformation,
			std::optional<std::string> author,
			std::optional<nfx::time::DateTimeOffset> dateCreated,
			std::optional<nfx::containers::StringMap<Value>> customHeaders );

		/**
		 * @brief Copy constructor
		 * @param other The Header to copy from
		 */
		Header( const Header& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The Header to move from
		 */
		Header( Header&& other ) noexcept = default;

		/** @brief Destructor */
		~Header() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The Header to copy from
		 * @return Reference to this Header
		 */
		Header& operator=( const Header& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The Header to move from
		 * @return Reference to this Header
		 */
		Header& operator=( Header&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get ship ID
		 * @return Ship identifier
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const ShipId& shipId() const noexcept;

		/**
		 * @brief Get data channel list ID
		 * @return Configuration reference for data channel list
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const ConfigurationReference& dataChannelListId() const noexcept;

		/**
		 * @brief Get version information
		 * @return Optional version information
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<VersionInformation>& versionInformation() const noexcept;

		/**
		 * @brief Get author
		 * @return Optional author string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::string>& author() const noexcept;

		/**
		 * @brief Get date created
		 * @return Optional creation timestamp (UtcNow default)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<nfx::time::DateTimeOffset>& dateCreated() const noexcept;

		/**
		 * @brief Get custom headers
		 * @return Optional custom headers dictionary
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<nfx::containers::StringMap<Value>>& customHeaders() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set ship ID
		 * @param shipId New ship identifier
		 */
		VISTA_SDK_CPP_INLINE void setShipId( ShipId shipId );

		/**
		 * @brief Set data channel list ID
		 * @param dataChannelListId New configuration reference
		 */
		VISTA_SDK_CPP_INLINE void setDataChannelListId( ConfigurationReference dataChannelListId );

		/**
		 * @brief Set version information
		 * @param versionInformation New optional version information
		 */
		VISTA_SDK_CPP_INLINE void setVersionInformation( std::optional<VersionInformation> versionInformation );

		/**
		 * @brief Set author
		 * @param author New optional author string
		 */
		VISTA_SDK_CPP_INLINE void setAuthor( std::optional<std::string> author );

		/**
		 * @brief Set date created
		 * @param dateCreated New optional creation timestamp
		 */
		VISTA_SDK_CPP_INLINE void setDateCreated( std::optional<nfx::time::DateTimeOffset> dateCreated );

		/**
		 * @brief Set custom headers
		 * @param customHeaders New optional custom headers dictionary
		 */
		VISTA_SDK_CPP_INLINE void setCustomHeaders( std::optional<nfx::containers::StringMap<Value>> customHeaders );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		ShipId m_shipId;
		ConfigurationReference m_dataChannelListId;
		std::optional<VersionInformation> m_versionInformation;
		std::optional<std::string> m_author;
		std::optional<nfx::time::DateTimeOffset> m_dateCreated;
		std::optional<nfx::containers::StringMap<Value>> m_customHeaders;
	};

	//=====================================================================
	// NameObject class
	//=====================================================================

	/**
	 * @brief Name object for data channel naming
	 */
	class NameObject final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor with standard naming rule */
		VISTA_SDK_CPP_INLINE NameObject();

		/**
		 * @brief Copy constructor
		 * @param other The NameObject to copy from
		 */
		NameObject( const NameObject& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The NameObject to move from
		 */
		NameObject( NameObject&& other ) noexcept = default;

		/** @brief Destructor */
		~NameObject() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The NameObject to copy from
		 * @return Reference to this NameObject
		 */
		NameObject& operator=( const NameObject& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The NameObject to move from
		 * @return Reference to this NameObject
		 */
		NameObject& operator=( NameObject&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get naming rule
		 * @return Naming rule string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::string& namingRule() const noexcept;

		/**
		 * @brief Get custom name objects
		 * @return Optional custom name objects dictionary
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<nfx::containers::StringMap<Value>>& customNameObjects() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set naming rule
		 * @param namingRule New naming rule string
		 */
		VISTA_SDK_CPP_INLINE void setNamingRule( std::string namingRule );

		/**
		 * @brief Set custom name objects
		 * @param customNameObjects New optional custom name objects
		 */
		VISTA_SDK_CPP_INLINE void setCustomNameObjects( std::optional<nfx::containers::StringMap<Value>> customNameObjects );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		std::string m_namingRule;
		std::optional<nfx::containers::StringMap<Value>> m_customNameObjects;
	};

	//=====================================================================
	// Format class
	//=====================================================================

	/**
	 * @brief Data format with validation and ISO19848 integration
	 */
	class Format final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		Format() = delete;

		/**
		 * @brief Constructs Format with required type
		 * @param type Format type string (validates against ISO19848)
		 */
		explicit Format( std::string_view type );

		/**
		 * @brief Copy constructor
		 * @param other The Format to copy from
		 */
		Format( const Format& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The Format to move from
		 */
		Format( Format&& other ) noexcept = default;

		/** @brief Destructor */
		~Format() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The Format to copy from
		 * @return Reference to this Format
		 */
		Format& operator=( const Format& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The Format to move from
		 * @return Reference to this Format
		 */
		Format& operator=( Format&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get format type
		 * @return Format type string
		 * @throws std::invalid_operation If type not set
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::string& type() const;

		/**
		 * @brief Get restriction
		 * @return Optional validation restriction
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<Restriction>& restriction() const noexcept;

		/**
		 * @brief Check if format is decimal type
		 * @return True if type is "Decimal"
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isDecimal() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set format type with ISO19848 validation
		 * @param type New format type
		 * @throws std::invalid_argument If invalid format type
		 */
		VISTA_SDK_CPP_INLINE void setType( std::string_view type );

		/**
		 * @brief Set restriction
		 * @param restriction New optional validation restriction
		 */
		VISTA_SDK_CPP_INLINE void setRestriction( std::optional<Restriction> restriction );

		//----------------------------------------------
		// Validation
		//----------------------------------------------

		/**
		 * @brief Validate value against format and restrictions
		 * @param value String value to validate
		 * @param parsedValue Output parsed value
		 * @return ValidateResult with success or error messages
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] ValidateResult validateValue( const std::string& value, Value& parsedValue ) const;

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		std::optional<std::string> m_type;
		std::optional<Restriction> m_restriction;
		std::optional<FormatDataType> m_dataType;
	};

	//=====================================================================
	// DataChannelType class
	//=====================================================================

	/**
	 * @brief Data channel type with ISO19848 validation
	 */
	class DataChannelType final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		DataChannelType() = delete;

		/**
		 * @brief Constructs DataChannelType with required type
		 * @param type Channel type string (validates against ISO19848)
		 */
		explicit DataChannelType( std::string_view type );

		/**
		 * @brief Copy constructor
		 * @param other The DataChannelType to copy from
		 */
		DataChannelType( const DataChannelType& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The DataChannelType to move from
		 */
		DataChannelType( DataChannelType&& other ) noexcept = default;

		/** @brief Destructor */
		~DataChannelType() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The DataChannelType to copy from
		 * @return Reference to this DataChannelType
		 */
		DataChannelType& operator=( const DataChannelType& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The DataChannelType to move from
		 * @return Reference to this DataChannelType
		 */
		DataChannelType& operator=( DataChannelType&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get channel type
		 * @return Channel type string
		 * @throws std::invalid_operation If type not set
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] const std::string& type() const;

		/**
		 * @brief Get update cycle
		 * @return Optional update cycle in seconds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE std::optional<double> updateCycle() const noexcept;

		/**
		 * @brief Get calculation period
		 * @return Optional calculation period in seconds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE std::optional<double> calculationPeriod() const noexcept;

		/**
		 * @brief Check if channel type is alert
		 * @return True if type is "Alert"
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE bool isAlert() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set channel type with ISO19848 validation
		 * @param type New channel type
		 * @throws std::invalid_argument If invalid channel type
		 */
		void setType( std::string_view type );

		/**
		 * @brief Set update cycle
		 * @param updateCycle New optional update cycle (must be positive)
		 * @throws std::invalid_argument If negative value
		 */
		void setUpdateCycle( std::optional<double> updateCycle );

		/**
		 * @brief Set calculation period
		 * @param calculationPeriod New optional calculation period (must be positive)
		 * @throws std::invalid_argument If negative value
		 */
		void setCalculationPeriod( std::optional<double> calculationPeriod );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		std::optional<std::string> m_type;
		std::optional<double> m_updateCycle;
		std::optional<double> m_calculationPeriod;
	};

	//=====================================================================
	// Property class
	//=====================================================================

	/**
	 * @brief Data channel property with validation
	 */
	class Property final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		Property() = delete;

		/**
		 * @brief Constructs Property with required fields
		 * @param dataChannelType Channel type
		 * @param format Data format
		 * @param range Numeric range
		 * @param unit Unit information
		 * @param alertPriority Alert priority
		 */
		VISTA_SDK_CPP_INLINE Property(
			DataChannelType dataChannelType,
			Format format,
			std::optional<Range> range,
			std::optional<Unit> unit,
			std::optional<std::string> alertPriority );

		/**
		 * @brief Copy constructor
		 * @param other The Property to copy from
		 */
		Property( const Property& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The Property to move from
		 */
		Property( Property&& other ) noexcept = default;

		/** @brief Destructor */
		~Property() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The Property to copy from
		 * @return Reference to this Property
		 */
		Property& operator=( const Property& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The Property to move from
		 * @return Reference to this Property
		 */
		Property& operator=( Property&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get data channel type
		 * @return Reference to channel type
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const DataChannelType& dataChannelType() const noexcept;

		/**
		 * @brief Get format
		 * @return Reference to data format
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const Format& format() const noexcept;

		/**
		 * @brief Get range
		 * @return Optional numeric range
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<Range>& range() const noexcept;

		/**
		 * @brief Get unit
		 * @return Optional unit information
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<Unit>& unit() const noexcept;

		/**
		 * @brief Get quality coding
		 * @return Optional quality coding string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::string>& qualityCoding() const noexcept;

		/**
		 * @brief Get alert priority
		 * @return Optional alert priority string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::string>& alertPriority() const noexcept;

		/**
		 * @brief Get name
		 * @return Optional property name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::string>& name() const noexcept;

		/**
		 * @brief Get remarks
		 * @return Optional remarks string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::string>& remarks() const noexcept;

		/**
		 * @brief Get custom properties
		 * @return Optional custom properties dictionary
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<nfx::containers::StringMap<Value>>& customProperties() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set data channel type
		 * @param dataChannelType New channel type
		 */
		VISTA_SDK_CPP_INLINE void setDataChannelType( DataChannelType dataChannelType );

		/**
		 * @brief Set format
		 * @param format New data format
		 */
		VISTA_SDK_CPP_INLINE void setFormat( Format format );

		/**
		 * @brief Set range
		 * @param range New optional numeric range
		 */
		VISTA_SDK_CPP_INLINE void setRange( std::optional<Range> range );

		/**
		 * @brief Set unit
		 * @param unit New optional unit information
		 */
		VISTA_SDK_CPP_INLINE void setUnit( std::optional<Unit> unit );

		/**
		 * @brief Set quality coding
		 * @param qualityCoding New optional quality coding
		 */
		VISTA_SDK_CPP_INLINE void setQualityCoding( std::optional<std::string> qualityCoding );

		/**
		 * @brief Set alert priority
		 * @param alertPriority New optional alert priority
		 */
		VISTA_SDK_CPP_INLINE void setAlertPriority( std::optional<std::string> alertPriority );

		/**
		 * @brief Set name
		 * @param name New optional property name
		 */
		VISTA_SDK_CPP_INLINE void setName( std::optional<std::string> name );

		/**
		 * @brief Set remarks
		 * @param remarks New optional remarks
		 */
		VISTA_SDK_CPP_INLINE void setRemarks( std::optional<std::string> remarks );

		/**
		 * @brief Set custom properties
		 * @param customProperties New optional custom properties
		 */
		VISTA_SDK_CPP_INLINE void setCustomProperties( std::optional<nfx::containers::StringMap<Value>> customProperties );

		//----------------------------------------------
		// Validation
		//----------------------------------------------

		/**
		 * @brief Validate property business rules
		 * @return ValidateResult with success or error messages
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] ValidateResult validate() const;

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		DataChannelType m_dataChannelType;
		Format m_format;
		std::optional<Range> m_range;
		std::optional<Unit> m_unit;
		std::optional<std::string> m_qualityCoding;
		std::optional<std::string> m_alertPriority;
		std::optional<std::string> m_name;
		std::optional<std::string> m_remarks;
		std::optional<nfx::containers::StringMap<Value>> m_customProperties;
	};

	//=====================================================================
	// DataChannelId class
	//=====================================================================

	/**
	 * @brief Data channel identifier with LocalId, ShortId, and NameObject
	 */
	class DataChannelId final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		DataChannelId() = delete;

		/**
		 * @brief Constructs DataChannelId with required fields
		 * @param localId Required local identifier
		 * @param shortId Optional short identifier
		 */
		VISTA_SDK_CPP_INLINE DataChannelId( LocalId localId, std::optional<std::string> shortId );

		/**
		 * @brief Copy constructor
		 * @param other The DataChannelId to copy from
		 */
		DataChannelId( const DataChannelId& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The DataChannelId to move from
		 */
		DataChannelId( DataChannelId&& other ) noexcept = default;

		/** @brief Destructor */
		~DataChannelId() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The DataChannelId to copy from
		 * @return Reference to this DataChannelId
		 */
		DataChannelId& operator=( const DataChannelId& other ) = delete;

		/**
		 * @brief Move assignment operator
		 * @param other The DataChannelId to move from
		 * @return Reference to this DataChannelId
		 */
		DataChannelId& operator=( DataChannelId&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get local ID
		 * @return Local identifier
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const LocalId& localId() const noexcept;

		/**
		 * @brief Get short ID
		 * @return Optional short identifier
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::string>& shortId() const noexcept;

		/**
		 * @brief Get name object
		 * @return Optional name object
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<NameObject>& nameObject() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set local ID
		 * @param localId New local identifier
		 */
		VISTA_SDK_CPP_INLINE void setLocalId( LocalId localId );

		/**
		 * @brief Set short ID
		 * @param shortId New optional short identifier
		 */
		VISTA_SDK_CPP_INLINE void setShortId( std::optional<std::string> shortId );

		/**
		 * @brief Set name object
		 * @param nameObject New optional name object
		 */
		VISTA_SDK_CPP_INLINE void setNameObject( std::optional<NameObject> nameObject );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		LocalId m_localId;
		std::optional<std::string> m_shortId;
		std::optional<NameObject> m_nameObject;
	};

	//=====================================================================
	// DataChannel class
	//=====================================================================

	/**
	 * @brief Individual data channel with validated property
	 */
	class DataChannel final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		DataChannel() = delete;

		/**
		 * @brief Constructs DataChannel with required fields
		 * @param dataChannelId Channel identifier
		 * @param property Channel property (must be valid)
		 * @throws std::invalid_argument If property validation fails
		 */
		DataChannel( DataChannelId dataChannelId, Property property );

		/**
		 * @brief Copy constructor
		 * @param other The DataChannel to copy from
		 */
		DataChannel( const DataChannel& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The DataChannel to move from
		 */
		DataChannel( DataChannel&& other ) noexcept = default;

		/** @brief Destructor */
		~DataChannel() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The DataChannel to copy from
		 * @return Reference to this DataChannel
		 */
		DataChannel& operator=( const DataChannel& other ) = delete;

		/**
		 * @brief Move assignment operator
		 * @param other The DataChannel to move from
		 * @return Reference to this DataChannel
		 */
		DataChannel& operator=( DataChannel&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get data channel ID
		 * @return Reference to channel identifier
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const DataChannelId& dataChannelId() const noexcept;

		/**
		 * @brief Get property
		 * @return Reference to channel property
		 * @throws std::invalid_operation If property not set
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] const Property& property() const;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set data channel ID
		 * @param dataChannelId New channel identifier
		 */
		VISTA_SDK_CPP_INLINE void setDataChannelId( DataChannelId dataChannelId );

		/**
		 * @brief Set property with validation
		 * @param property New channel property
		 * @throws std::invalid_argument If property validation fails
		 */
		void setProperty( Property property );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		DataChannelId m_dataChannelId;
		std::optional<Property> m_property;
	};

	//=====================================================================
	// DataChannelList class
	//=====================================================================

	/**
	 * @brief Collection of data channels with dual indexing
	 */
	class DataChannelList final
	{
	public:
		//----------------------------------------------
		// Type aliases
		//----------------------------------------------

		/** @brief Iterator type for mutable access to data channels */
		using iterator = std::vector<DataChannel>::iterator;
		
		/** @brief Iterator type for const access to data channels */
		using const_iterator = std::vector<DataChannel>::const_iterator;
		
		/** @brief Size type for collection operations */
		using size_type = std::vector<DataChannel>::size_type;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		DataChannelList() = default;

		/**
		 * @brief Copy constructor
		 * @param other The DataChannelList to copy from
		 */
		DataChannelList( const DataChannelList& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The DataChannelList to move from
		 */
		DataChannelList( DataChannelList&& other ) noexcept = default;

		/** @brief Destructor */
		~DataChannelList() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The DataChannelList to copy from
		 * @return Reference to this DataChannelList
		 */
		DataChannelList& operator=( const DataChannelList& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The DataChannelList to move from
		 * @return Reference to this DataChannelList
		 */
		DataChannelList& operator=( DataChannelList&& other ) noexcept = default;

		//----------------------------------------------
		// Collection interface
		//----------------------------------------------

		/**
		 * @brief Get number of data channels
		 * @return Count of data channels
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE size_type size() const noexcept;

		/**
		 * @brief Check if collection is empty
		 * @return True if no data channels
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE bool isEmpty() const noexcept;

		/**
		 * @brief Clear all data channels
		 */
		void clear();

		//----------------------------------------------
		// Element access
		//----------------------------------------------

		/**
		 * @brief Access data channel by index
		 * @param index Channel index
		 * @return Reference to data channel
		 * @throws std::out_of_range If index invalid
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const DataChannel& operator[]( size_type index ) const;

		/**
		 * @brief Access data channel by short ID
		 * @param shortId Short identifier
		 * @return Reference to data channel
		 * @throws std::out_of_range If short ID not found
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const DataChannel& operator[]( const std::string& shortId ) const;

		/**
		 * @brief Access data channel by local ID
		 * @param localId Local identifier
		 * @return Reference to data channel
		 * @throws std::out_of_range If local ID not found
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const DataChannel& operator[]( const LocalId& localId ) const;

		//----------------------------------------------
		// Safe lookup methods
		//----------------------------------------------

		/**
		 * @brief Try to get data channel by short ID
		 * @param shortId Short identifier to search for
		 * @return Pointer to data channel if found, nullptr otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] const DataChannel* tryGetByShortId( const std::string& shortId ) const;

		/**
		 * @brief Try to get data channel by short ID (string_view overload)
		 * @param shortId Short identifier to search for
		 * @return Pointer to data channel if found, nullptr otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] const DataChannel* tryGetByShortId( std::string_view shortId ) const;

		/**
		 * @brief Try to get data channel by local ID
		 * @param localId Local identifier to search for
		 * @return Pointer to data channel if found, nullptr otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] const DataChannel* tryGetByLocalId( const LocalId& localId ) const;

		//----------------------------------------------
		// Modification methods
		//----------------------------------------------

		/**
		 * @brief Add single data channel
		 * @param dataChannel Channel to add
		 * @throws std::invalid_argument If LocalId or ShortId already exists
		 */
		void add( DataChannel dataChannel );

		/**
		 * @brief Add multiple data channels
		 * @param dataChannels Channels to add
		 * @throws std::invalid_argument If any LocalId or ShortId already exists
		 */
		void add( const std::vector<DataChannel>& dataChannels );

		/**
		 * @brief Remove data channel
		 * @param dataChannel Channel to remove
		 * @return True if removed, false if not found
		 */
		bool remove( const DataChannel& dataChannel );

		//----------------------------------------------
		// Iterators
		//----------------------------------------------

		/**
		 * @brief Get iterator to beginning
		 * @return Iterator to first data channel
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE iterator begin() noexcept;

		/**
		 * @brief Get const iterator to beginning
		 * @return Const iterator to first data channel
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const_iterator begin() const noexcept;

		/**
		 * @brief Get iterator to end
		 * @return Iterator past last data channel
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE iterator end() noexcept;

		/**
		 * @brief Get const iterator to end
		 * @return Const iterator past last data channel
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const_iterator end() const noexcept;

		//----------------------------------------------
		// Data access
		//----------------------------------------------

		/**
		 * @brief Get read-only data channels
		 * @return Const reference to internal vector
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const std::vector<DataChannel>& dataChannels() const noexcept;

	private:
		//----------------------------------------------
		// Private members (triple storage for dual indexing)
		//----------------------------------------------

		std::vector<DataChannel> m_dataChannels;
		nfx::containers::StringMap<const DataChannel*> m_shortIdMap;
		nfx::containers::HashMap<LocalId, const DataChannel*> m_localIdMap;
	};

	//=====================================================================
	// Package class
	//=====================================================================

	/**
	 * @brief Package containing header and data channel list
	 */
	class Package final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		Package() = delete;

		/**
		 * @brief Constructs Package with required components
		 * @param header Package header with metadata
		 * @param dataChannelList Collection of data channels
		 */
		VISTA_SDK_CPP_INLINE Package( Header header, DataChannelList dataChannelList );

		/**
		 * @brief Copy constructor
		 * @param other The Package to copy from
		 */
		Package( const Package& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The Package to move from
		 */
		Package( Package&& other ) noexcept = default;

		/** @brief Destructor */
		~Package() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The Package to copy from
		 * @return Reference to this Package
		 */
		Package& operator=( const Package& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The Package to move from
		 * @return Reference to this Package
		 */
		Package& operator=( Package&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get header
		 * @return Reference to package header
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const Header& header() const noexcept;

		/**
		 * @brief Get header (mutable)
		 * @return Mutable reference to package header
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE Header& header() noexcept;

		/**
		 * @brief Get data channel list
		 * @return Reference to data channel collection
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const DataChannelList& dataChannelList() const noexcept;

		/**
		 * @brief Get data channel list (mutable)
		 * @return Mutable reference to data channel collection
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE DataChannelList& dataChannelList() noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set header
		 * @param header New package header
		 */
		VISTA_SDK_CPP_INLINE void setHeader( Header header );

		/**
		 * @brief Set data channel list
		 * @param dataChannelList New data channel collection
		 */
		VISTA_SDK_CPP_INLINE void setDataChannelList( DataChannelList dataChannelList );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		Header m_header;
		DataChannelList m_dataChannelList;
	};

	//=====================================================================
	// DataChannelListPackage class
	//=====================================================================

	/**
	 * @brief Main data channel list package container
	 * @details Top-level container
	 */
	class DataChannelListPackage final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		DataChannelListPackage() = delete;

		/**
		 * @brief Constructs DataChannelListPackage with required package
		 * @param package Package containing header and data channel list
		 */
		VISTA_SDK_CPP_INLINE explicit DataChannelListPackage( Package package );

		/**
		 * @brief Copy constructor
		 * @param other The DataChannelListPackage to copy from
		 */
		DataChannelListPackage( const DataChannelListPackage& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The DataChannelListPackage to move from
		 */
		DataChannelListPackage( DataChannelListPackage&& other ) noexcept = default;

		/** @brief Destructor */
		~DataChannelListPackage() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The DataChannelListPackage to copy from
		 * @return Reference to this DataChannelListPackage
		 */
		DataChannelListPackage& operator=( const DataChannelListPackage& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The DataChannelListPackage to move from
		 * @return Reference to this DataChannelListPackage
		 */
		DataChannelListPackage& operator=( DataChannelListPackage&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get package
		 * @return Reference to contained package
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const Package& package() const noexcept;

		/**
		 * @brief Get package (mutable)
		 * @return Mutable reference to contained package
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE Package& package() noexcept;

		/**
		 * @brief Get data channel list
		 * @return Reference to data channel list from package
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE const DataChannelList& dataChannelList() const noexcept;

		/**
		 * @brief Get data channel list (mutable convenience)
		 * @return Mutable reference to data channel list from package
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] VISTA_SDK_CPP_INLINE DataChannelList& dataChannelList() noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set package
		 * @param package New package
		 */
		VISTA_SDK_CPP_INLINE void setPackage( Package package );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		Package m_package;
	};
}

#include "dnv/vista/sdk/detail/transport/datachannel/DataChannel.inl"
