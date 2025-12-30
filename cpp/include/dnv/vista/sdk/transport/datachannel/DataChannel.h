/**
 * @file DataChannel.h
 * @brief Data Channel domain model for ISO 19848 compliance
 */

#pragma once

#include "dnv/vista/sdk/detail/Containers.h"
#include "dnv/vista/sdk/transport/ISO19848.h"
#include "dnv/vista/sdk/transport/Results.h"
#include "dnv/vista/sdk/transport/ShipId.h"
#include "dnv/vista/sdk/LocalId.h"

#include <nfx/serialization/json/Document.h>

#include <functional>
#include <optional>
#include <string>
#include <algorithm>

namespace dnv::vista::sdk::transport::datachannel
{
	//=====================================================================
	// Type aliases
	//=====================================================================

	/** @brief Alias for custom headers - supports arbitrary JSON */
	using CustomHeaders = nfx::serialization::json::Document;

	/** @brief Alias for custom headers - supports arbitrary JSON */
	using CustomNameObjects = nfx::serialization::json::Document;

	/** @brief Alias for custom headers - supports arbitrary JSON */
	using CustomElements = nfx::serialization::json::Document;

	/** @brief Alias for custom headers - supports arbitrary JSON */
	using CustomProperties = nfx::serialization::json::Document;

	//=====================================================================
	// ConfigurationReference class
	//=====================================================================

	/**
	 * @brief Configuration reference with version and timestamp
	 * @details References a data channel list configuration with:
	 *          - Id: required identifier
	 *          - Version: optional version string
	 *          - TimeStamp: required timestamp
	 */
	class ConfigurationReference final
	{
	public:
		/** @brief Default constructor */
		ConfigurationReference() = default;

		/**
		 * @brief Construct ConfigurationReference with all fields
		 * @param id Configuration identifier (required)
		 * @param timeStamp Configuration timestamp (required)
		 * @param version Optional version string
		 */
		explicit ConfigurationReference(
			std::string id,
			DateTimeOffset timeStamp,
			std::optional<std::string> version = std::nullopt );

		/** @brief Copy constructor */
		ConfigurationReference( const ConfigurationReference& ) = default;

		/** @brief Move constructor */
		ConfigurationReference( ConfigurationReference&& ) noexcept = default;

		/** @brief Destructor */
		~ConfigurationReference() = default;

		/** @brief Copy assignment operator */
		ConfigurationReference& operator=( const ConfigurationReference& ) = default;

		/** @brief Move assignment operator */
		ConfigurationReference& operator=( ConfigurationReference&& ) noexcept = default;

		/** @brief Get configuration identifier */
		[[nodiscard]] inline const std::string& id() const noexcept;

		/** @brief Get optional version */
		[[nodiscard]] inline const std::optional<std::string>& version() const noexcept;

		/** @brief Get timestamp */
		[[nodiscard]] inline const DateTimeOffset& timeStamp() const noexcept;

		/** @brief Set configuration identifier */
		inline void setId( std::string_view id );

		/** @brief Set optional version */
		inline void setVersion( std::optional<std::string> version );

		/** @brief Set timestamp */
		inline void setTimeStamp( DateTimeOffset timeStamp );

	private:
		std::string m_id;
		std::optional<std::string> m_version;
		DateTimeOffset m_timeStamp;
	};

	//=====================================================================
	// VersionInformation class
	//=====================================================================

	/**
	 * @brief Version information for naming rules
	 * @details Contains naming rule version information with defaults from ISO 19848 Annex C
	 */
	class VersionInformation final
	{
	public:
		/** @brief Default constructor with ISO 19848 Annex C defaults */
		VersionInformation();

		/**
		 * @brief Construct VersionInformation with all fields
		 * @param namingRule Naming rule identifier
		 * @param namingSchemeVersion Naming scheme version
		 * @param referenceUrl Optional reference URL
		 */
		VersionInformation(
			std::string namingRule,
			std::string namingSchemeVersion,
			std::optional<std::string> referenceUrl = std::nullopt );

		/** @brief Copy constructor */
		VersionInformation( const VersionInformation& ) = default;

		/** @brief Move constructor */
		VersionInformation( VersionInformation&& ) noexcept = default;

		/** @brief Destructor */
		~VersionInformation() = default;

		/** @brief Copy assignment operator */
		VersionInformation& operator=( const VersionInformation& ) = default;

		/** @brief Move assignment operator */
		VersionInformation& operator=( VersionInformation&& ) noexcept = default;

		/** @brief Get naming rule */
		[[nodiscard]] inline const std::string& namingRule() const noexcept;

		/** @brief Get naming scheme version */
		[[nodiscard]] inline const std::string& namingSchemeVersion() const noexcept;

		/** @brief Get optional reference URL */
		[[nodiscard]] inline const std::optional<std::string>& referenceUrl() const noexcept;

		/** @brief Set naming rule */
		inline void setNamingRule( std::string_view namingRule );

		/** @brief Set naming scheme version */
		inline void setNamingSchemeVersion( std::string_view namingSchemeVersion );

		/** @brief Set optional reference URL */
		inline void setReferenceUrl( std::optional<std::string> referenceUrl );

	private:
		std::string m_namingRule;
		std::string m_namingSchemeVersion;
		std::optional<std::string> m_referenceUrl;
	};

	//=====================================================================
	// WhiteSpace enumeration
	//=====================================================================

	/**
	 * @brief White space handling modes
	 * @details Specifies how white space (line feeds, tabs, spaces, carriage returns) is handled
	 */
	enum class WhiteSpace : std::uint8_t
	{
		Preserve = 0, ///< Preserve all white space
		Replace,	  ///< Replace sequences of white space with single space
		Collapse	  ///< Collapse sequences of white space and trim leading/trailing
	};

	//=====================================================================
	// Range class
	//=====================================================================

	/**
	 * @brief Numeric range with low and high bounds
	 * @details Validates that low value is less than high value
	 */
	class Range final
	{
	public:
		/** @brief Default constructor with min/max double values */
		Range() = default;

		/**
		 * @brief Construct Range with low and high bounds
		 * @param low Lower bound (required)
		 * @param high Upper bound (required)
		 */
		Range( double low, double high );

		/** @brief Copy constructor */
		Range( const Range& ) = default;

		/** @brief Move constructor */
		Range( Range&& ) noexcept = default;

		/** @brief Destructor */
		~Range() = default;

		/** @brief Copy assignment operator */
		Range& operator=( const Range& ) = default;

		/** @brief Move assignment operator */
		Range& operator=( Range&& ) noexcept = default;

		/** @brief Get lower bound */
		[[nodiscard]] inline double low() const noexcept;

		/** @brief Get upper bound */
		[[nodiscard]] inline double high() const noexcept;

		/** @brief Set lower bound (must be less than high) */
		inline void setLow( double low );

		/** @brief Set upper bound (must be greater than low) */
		inline void setHigh( double high );

	private:
		double m_low = std::numeric_limits<double>::lowest();
		double m_high = std::numeric_limits<double>::max();
	};

	//=====================================================================
	// Restriction class
	//=====================================================================

	class Format;

	/**
	 * @brief Value restriction constraints
	 * @details Defines constraints for format validation including enumeration, length, pattern, numeric bounds
	 */
	class Restriction final
	{
	public:
		/** @brief Default constructor */
		Restriction() = default;

		/** @brief Copy constructor */
		Restriction( const Restriction& ) = default;

		/** @brief Move constructor */
		Restriction( Restriction&& ) noexcept = default;

		/** @brief Destructor */
		~Restriction() = default;

		/** @brief Copy assignment operator */
		Restriction& operator=( const Restriction& ) = default;

		/** @brief Move assignment operator */
		Restriction& operator=( Restriction&& ) noexcept = default;

		/** @brief Get enumeration of acceptable values */
		[[nodiscard]] inline const std::optional<std::vector<std::string>>& enumeration() const noexcept;

		/** @brief Get maximum decimal places allowed */
		[[nodiscard]] inline const std::optional<std::uint32_t>& fractionDigits() const noexcept;

		/** @brief Get exact number of characters or list items */
		[[nodiscard]] inline const std::optional<std::uint32_t>& length() const noexcept;

		/** @brief Get upper bounds (exclusive) */
		[[nodiscard]] inline const std::optional<double>& maxExclusive() const noexcept;

		/** @brief Get upper bounds (inclusive) */
		[[nodiscard]] inline const std::optional<double>& maxInclusive() const noexcept;

		/** @brief Get maximum number of characters or list items */
		[[nodiscard]] inline const std::optional<std::uint32_t>& maxLength() const noexcept;

		/** @brief Get lower bounds (exclusive) */
		[[nodiscard]] inline const std::optional<double>& minExclusive() const noexcept;

		/** @brief Get lower bounds (inclusive) */
		[[nodiscard]] inline const std::optional<double>& minInclusive() const noexcept;

		/** @brief Get minimum number of characters or list items */
		[[nodiscard]] inline const std::optional<std::uint32_t>& minLength() const noexcept;

		/** @brief Get regex pattern for acceptable character sequences */
		[[nodiscard]] inline const std::optional<std::string>& pattern() const noexcept;

		/** @brief Get exact number of digits (must be > 0) */
		[[nodiscard]] inline const std::optional<std::uint32_t>& totalDigits() const noexcept;

		/** @brief Get white space handling mode */
		[[nodiscard]] inline const std::optional<WhiteSpace>& whiteSpace() const noexcept;

		/** @brief Set enumeration */
		inline void setEnumeration( std::optional<std::vector<std::string>> enumeration );

		/** @brief Set fraction digits */
		inline void setFractionDigits( std::optional<std::uint32_t> fractionDigits );

		/** @brief Set length */
		inline void setLength( std::optional<std::uint32_t> length );

		/** @brief Set max exclusive */
		inline void setMaxExclusive( std::optional<double> maxExclusive );

		/** @brief Set max inclusive */
		inline void setMaxInclusive( std::optional<double> maxInclusive );

		/** @brief Set max length */
		inline void setMaxLength( std::optional<std::uint32_t> maxLength );

		/** @brief Set min exclusive */
		inline void setMinExclusive( std::optional<double> minExclusive );

		/** @brief Set min inclusive */
		inline void setMinInclusive( std::optional<double> minInclusive );

		/** @brief Set min length */
		inline void setMinLength( std::optional<std::uint32_t> minLength );

		/** @brief Set pattern */
		inline void setPattern( std::optional<std::string> pattern );

		/** @brief Set total digits (must be > 0) */
		inline void setTotalDigits( std::optional<std::uint32_t> totalDigits );

		/** @brief Set white space handling */
		inline void setWhiteSpace( std::optional<WhiteSpace> whiteSpace );

		/**
		 * @brief Validate a string value against restriction constraints
		 * @param value The string value to validate
		 * @param format The format containing the data type
		 * @return Validation result indicating success or error messages
		 * @details Validates the value against enumeration, numeric bounds, length, pattern, etc.
		 */
		[[nodiscard]] ValidateResult<> validateValue( std::string_view value, const Format& format ) const;

	private:
		/**
		 * @brief Validate numeric value against bounds
		 * @param number The number to validate
		 * @return Validation result indicating success or error messages
		 */
		[[nodiscard]] inline ValidateResult<> validateNumber( double number ) const;

		std::optional<std::vector<std::string>> m_enumeration;
		std::optional<std::uint32_t> m_fractionDigits;
		std::optional<std::uint32_t> m_length;
		std::optional<double> m_maxExclusive;
		std::optional<double> m_maxInclusive;
		std::optional<std::uint32_t> m_maxLength;
		std::optional<double> m_minExclusive;
		std::optional<double> m_minInclusive;
		std::optional<std::uint32_t> m_minLength;
		std::optional<std::string> m_pattern;
		std::optional<std::uint32_t> m_totalDigits;
		std::optional<WhiteSpace> m_whiteSpace;
	};

	//=====================================================================
	// Format class
	//=====================================================================

	/**
	 * @brief Data format type with optional restriction
	 * @details Validates format type against ISO 19848 format data types
	 */
	class Format final
	{
	public:
		/** @brief Default constructor */
		Format() = default;

		/**
		 * @brief Construct Format with all fields
		 * @param type Format type (required)
		 * @param restriction Optional restriction constraints
		 */
		Format( std::string type, std::optional<Restriction> restriction = std::nullopt );

		/** @brief Copy constructor */
		Format( const Format& ) = default;

		/** @brief Move constructor */
		Format( Format&& ) noexcept = default;

		/** @brief Destructor */
		~Format() = default;

		/** @brief Copy assignment operator */
		Format& operator=( const Format& ) = default;

		/** @brief Move assignment operator */
		Format& operator=( Format&& ) noexcept = default;

		/** @brief Get format type */
		[[nodiscard]] inline const std::string& type() const noexcept;

		/** @brief Get optional restriction */
		[[nodiscard]] inline const std::optional<Restriction>& restriction() const noexcept;

		/** @brief Check if format type is Decimal */
		[[nodiscard]] inline bool isDecimal() const noexcept;

		/** @brief Set format type */
		inline void setType( std::string_view type );

		/** @brief Set optional restriction */
		inline void setRestriction( std::optional<Restriction> restriction );

		/**
		 * @brief Validate a string value against this format
		 * @param value The string value to validate
		 * @param parsedValue Output parameter for the parsed value
		 * @return Validation result indicating success or error messages
		 * @details Validates the value against the format type and optional restriction
		 */
		[[nodiscard]] ValidateResult<> validateValue( std::string_view value, Value& parsedValue ) const;

	private:
		std::string m_type;
		std::optional<Restriction> m_restriction;
	};

	//=====================================================================
	// DataChannelType class
	//=====================================================================

	/**
	 * @brief Data channel type with update cycle and calculation period
	 * @details Validates type against ISO 19848 data channel type names
	 */
	class DataChannelType final
	{
	public:
		/** @brief Default constructor */
		DataChannelType() = default;

		/**
		 * @brief Construct DataChannelType with all fields
		 * @param type Data channel type (required, validated against ISO 19848)
		 * @param updateCycle Optional update cycle in seconds (must be >= 0)
		 * @param calculationPeriod Optional calculation period in seconds (must be >= 0)
		 */
		DataChannelType(
			std::string type,
			std::optional<double> updateCycle = std::nullopt,
			std::optional<double> calculationPeriod = std::nullopt );

		/** @brief Copy constructor */
		DataChannelType( const DataChannelType& ) = default;

		/** @brief Move constructor */
		DataChannelType( DataChannelType&& ) noexcept = default;

		/** @brief Destructor */
		~DataChannelType() = default;

		/** @brief Copy assignment operator */
		DataChannelType& operator=( const DataChannelType& ) = default;

		/** @brief Move assignment operator */
		DataChannelType& operator=( DataChannelType&& ) noexcept = default;

		/** @brief Get data channel type */
		[[nodiscard]] inline const std::string& type() const noexcept;

		/** @brief Get optional update cycle */
		[[nodiscard]] inline const std::optional<double>& updateCycle() const noexcept;

		/** @brief Get optional calculation period */
		[[nodiscard]] inline const std::optional<double>& calculationPeriod() const noexcept;

		/** @brief Check if type is Alert */
		[[nodiscard]] inline bool isAlert() const noexcept;

		/** @brief Set data channel type (validated against ISO 19848) */
		inline void setType( std::string_view type );

		/** @brief Set optional update cycle (must be >= 0) */
		inline void setUpdateCycle( std::optional<double> updateCycle );

		/** @brief Set optional calculation period (must be >= 0) */
		inline void setCalculationPeriod( std::optional<double> calculationPeriod );

	private:
		std::string m_type;
		std::optional<double> m_updateCycle;
		std::optional<double> m_calculationPeriod;
	};

	//=====================================================================
	// NameObject class
	//=====================================================================

	/**
	 * @brief Name object with naming rule and custom name objects
	 * @details Contains naming rule identifier with optional custom extensions
	 */
	class NameObject final
	{
	public:
		/** @brief Default constructor with ISO 19848 Annex C naming rule */
		NameObject();

		/**
		 * @brief Construct NameObject with all fields
		 * @param namingRule Naming rule identifier
		 * @param customNameObjects Optional custom name objects
		 */
		NameObject(
			std::string namingRule,
			std::optional<CustomNameObjects> customNameObjects = std::nullopt );

		/** @brief Copy constructor */
		NameObject( const NameObject& ) = default;

		/** @brief Move constructor */
		NameObject( NameObject&& ) noexcept = default;

		/** @brief Destructor */
		~NameObject() = default;

		/** @brief Copy assignment operator */
		NameObject& operator=( const NameObject& ) = default;

		/** @brief Move assignment operator */
		NameObject& operator=( NameObject&& ) noexcept = default;

		/** @brief Get naming rule */
		[[nodiscard]] inline const std::string& namingRule() const noexcept;

		/** @brief Get optional custom name objects */
		[[nodiscard]] inline const std::optional<CustomNameObjects>& customNameObjects() const noexcept;

		/** @brief Set naming rule */
		inline void setNamingRule( std::string_view namingRule );

		/** @brief Set optional custom name objects */
		inline void setCustomNameObjects( std::optional<CustomNameObjects> customNameObjects );

	private:
		std::string m_namingRule;
		std::optional<CustomNameObjects> m_customNameObjects;
	};

	//=====================================================================
	// Unit class
	//=====================================================================

	/**
	 * @brief Unit of measurement with symbol and quantity name
	 * @details Unit according to ISO 80000, IEC 80000 Table 4
	 */
	class Unit final
	{
	public:
		/** @brief Default constructor */
		Unit() = default;

		/**
		 * @brief Construct Unit with all fields
		 * @param unitSymbol Unit symbol (required)
		 * @param quantityName Optional quantity name
		 * @param customElements Optional custom elements dictionary
		 */
		Unit(
			std::string unitSymbol,
			std::optional<std::string> quantityName = std::nullopt,
			std::optional<CustomElements> customElements = std::nullopt );

		/** @brief Copy constructor */
		Unit( const Unit& ) = default;

		/** @brief Move constructor */
		Unit( Unit&& ) noexcept = default;

		/** @brief Destructor */
		~Unit() = default;

		/** @brief Copy assignment operator */
		Unit& operator=( const Unit& ) = default;

		/** @brief Move assignment operator */
		Unit& operator=( Unit&& ) noexcept = default;

		/** @brief Get unit symbol */
		[[nodiscard]] inline const std::string& unitSymbol() const noexcept;

		/** @brief Get optional quantity name */
		[[nodiscard]] inline const std::optional<std::string>& quantityName() const noexcept;

		/** @brief Get optional custom elements */
		[[nodiscard]] inline const std::optional<CustomElements>& customElements() const noexcept;

		/** @brief Set unit symbol */
		inline void setUnitSymbol( std::string_view unitSymbol );

		/** @brief Set optional quantity name */
		inline void setQuantityName( std::optional<std::string> quantityName );

		/** @brief Set optional custom elements */
		inline void setCustomElements( std::optional<CustomElements> customElements );

	private:
		std::string m_unitSymbol;
		std::optional<std::string> m_quantityName;
		std::optional<CustomElements> m_customElements;
	};

	//=====================================================================
	// Header class
	//=====================================================================

	/**
	 * @brief Package header with metadata
	 * @details Contains ship identification, configuration reference, and optional metadata:
	 *          - ShipId: required
	 *          - DataChannelListId: required configuration reference
	 *          - VersionInformation: optional version information
	 *          - Author: optional author information
	 *          - DateCreated: optional creation timestamp
	 *          - CustomHeaders: optional extension data dictionary
	 */
	class Header final
	{
	public:
		/** @brief Default constructor  */
		Header() = delete;

		/**
		 * @brief Construct Header with all fields
		 * @param shipId Ship identifier (required)
		 * @param dataChannelListId Configuration reference (required)
		 * @param versionInformation Optional version information
		 * @param author Optional author string (nullable)
		 * @param dateCreated Optional creation timestamp (defaults to UtcNow)
		 * @param customHeaders Optional custom headers dictionary
		 */
		Header(
			ShipId shipId,
			ConfigurationReference dataChannelListId,
			std::optional<VersionInformation> versionInformation = std::nullopt,
			std::optional<std::string> author = std::nullopt,
			std::optional<DateTimeOffset> dateCreated = std::nullopt,
			std::optional<CustomHeaders> customHeaders = std::nullopt );

		/** @brief Copy constructor */
		Header( const Header& ) = default;

		/** @brief Move constructor */
		Header( Header&& ) noexcept = default;

		/** @brief Destructor */
		~Header() = default;

		/** @brief Copy assignment operator */
		Header& operator=( const Header& ) = default;

		/** @brief Move assignment operator */
		Header& operator=( Header&& ) noexcept = default;

		/** @brief Get optional author */
		[[nodiscard]] inline const std::optional<std::string>& author() const noexcept;

		/** @brief Get ship identifier */
		[[nodiscard]] inline const ShipId& shipId() const noexcept;

		/** @brief Get optional creation date */
		[[nodiscard]] inline const std::optional<DateTimeOffset>& dateCreated() const noexcept;

		/** @brief Get data channel list configuration reference */
		[[nodiscard]] inline const ConfigurationReference& dataChannelListId() const noexcept;

		/**
		 * @brief Get version information
		 * @return Optional version information
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::optional<VersionInformation>& versionInformation() const noexcept;

		/** @brief Get optional custom headers dictionary */
		[[nodiscard]] inline const std::optional<CustomHeaders>& customHeaders() const noexcept;

		/** @brief Set optional author */
		inline void setAuthor( std::optional<std::string> author );

		/** @brief Set optional creation date */
		inline void setDateCreated( std::optional<DateTimeOffset> dateCreated );

		/** @brief Set ship identifier */
		inline void setShipId( ShipId shipId );

		/** @brief Set data channel list configuration reference */
		inline void setDataChannelListId( ConfigurationReference dataChannelListId );

		/** @brief Set optional version information */
		inline void setVersionInformation( std::optional<VersionInformation> versionInformation );

		/** @brief Set optional custom headers dictionary */
		inline void setCustomHeaders( std::optional<CustomHeaders> customHeaders );

	private:
		ShipId m_shipId;
		ConfigurationReference m_dataChannelListId;
		std::optional<VersionInformation> m_versionInformation;
		std::optional<std::string> m_author;
		std::optional<DateTimeOffset> m_dateCreated;
		std::optional<CustomHeaders> m_customHeaders;
	};

	//=====================================================================
	// Property class
	//=====================================================================

	/**
	 * @brief Data channel property with type, format, range, and unit
	 * @details Contains property information for a data channel including:
	 *          - DataChannelType: required type information
	 *          - Format: required format specification
	 *          - Range: optional range (required for decimal formats)
	 *          - Unit: optional unit (required for decimal formats)
	 *          - QualityCoding: optional quality coding (e.g., IEC 61162-STATUS)
	 *          - AlertPriority: optional alert priority (required for alert types)
	 *          - Name: optional human-readable name
	 *          - Remarks: optional remarks
	 *          - CustomProperties: optional extension data dictionary
	 */
	class Property final
	{
	public:
		/** @brief Default constructor */
		Property() = default;

		/**
		 * @brief Construct Property with all fields
		 * @param dataChannelType Data channel type (required)
		 * @param format Format specification (required)
		 * @param range Optional range (required for decimal formats)
		 * @param unit Optional unit (required for decimal formats)
		 * @param qualityCoding Optional quality coding
		 * @param alertPriority Optional alert priority (nullable, required for alert types)
		 * @param name Optional human-readable name
		 * @param remarks Optional remarks
		 * @param customProperties Optional custom properties dictionary
		 */
		Property(
			DataChannelType dataChannelType,
			Format format,
			std::optional<Range> range = std::nullopt,
			std::optional<Unit> unit = std::nullopt,
			std::optional<std::string> qualityCoding = std::nullopt,
			std::optional<std::string> alertPriority = std::nullopt,
			std::optional<std::string> name = std::nullopt,
			std::optional<std::string> remarks = std::nullopt,
			std::optional<CustomProperties> customProperties = std::nullopt );

		/** @brief Copy constructor */
		Property( const Property& ) = default;

		/** @brief Move constructor */
		Property( Property&& ) noexcept = default;

		/** @brief Destructor */
		~Property() = default;

		/** @brief Copy assignment operator */
		Property& operator=( const Property& ) = default;

		/** @brief Move assignment operator */
		Property& operator=( Property&& ) noexcept = default;

		/** @brief Get data channel type */
		[[nodiscard]] inline const DataChannelType& dataChannelType() const noexcept;

		/** @brief Get format specification */
		[[nodiscard]] inline const Format& format() const noexcept;

		/** @brief Get optional range */
		[[nodiscard]] inline const std::optional<Range>& range() const noexcept;

		/** @brief Get optional unit */
		[[nodiscard]] inline const std::optional<Unit>& unit() const noexcept;

		/** @brief Get optional quality coding */
		[[nodiscard]] inline const std::optional<std::string>& qualityCoding() const noexcept;

		/** @brief Get optional alert priority */
		[[nodiscard]] inline const std::optional<std::string>& alertPriority() const noexcept;

		/** @brief Get optional name */
		[[nodiscard]] inline const std::optional<std::string>& name() const noexcept;

		/** @brief Get optional remarks */
		[[nodiscard]] inline const std::optional<std::string>& remarks() const noexcept;

		/** @brief Get optional custom properties */
		[[nodiscard]] inline const std::optional<CustomProperties>& customProperties() const noexcept;

		/** @brief Set data channel type */
		inline void setDataChannelType( DataChannelType dataChannelType );

		/** @brief Set format specification */
		inline void setFormat( Format format );

		/** @brief Set optional range */
		inline void setRange( std::optional<Range> range );

		/** @brief Set optional unit */
		inline void setUnit( std::optional<Unit> unit );

		/** @brief Set optional quality coding */
		inline void setQualityCoding( std::optional<std::string> qualityCoding );

		/** @brief Set optional alert priority */
		inline void setAlertPriority( std::optional<std::string> alertPriority );

		/** @brief Set optional name */
		inline void setName( std::optional<std::string> name );

		/** @brief Set optional remarks */
		inline void setRemarks( std::optional<std::string> remarks );

		/** @brief Set optional custom properties */
		inline void setCustomProperties( std::optional<CustomProperties> customProperties );

		/**
		 * @brief Validate property consistency
		 * @return Validation result indicating success or error messages
		 * @details Validates that:
		 *          - Range is present for decimal formats
		 *          - Unit is present for decimal formats
		 *          - AlertPriority is present for alert types
		 */
		[[nodiscard]] inline ValidateResult<> validate() const;

	private:
		DataChannelType m_dataChannelType;
		Format m_format;
		std::optional<Range> m_range;
		std::optional<Unit> m_unit;
		std::optional<std::string> m_qualityCoding;
		std::optional<std::string> m_alertPriority;
		std::optional<std::string> m_name;
		std::optional<std::string> m_remarks;
		std::optional<CustomProperties> m_customProperties;
	};

	//=====================================================================
	// DataChannelId class
	//=====================================================================

	/**
	 * @brief Data channel identifier with LocalId, ShortId, and NameObject
	 * @details Contains multiple identifier forms for a data channel:
	 *          - LocalId: required local identifier
	 *          - ShortId: optional short identifier (nullable)
	 *          - NameObject: optional name object with naming rule
	 */
	class DataChannelId final
	{
	public:
		/** @brief Default constructor */
		DataChannelId() = delete;

		/**
		 * @brief Construct DataChannelId with all fields
		 * @param localId Local identifier (required)
		 * @param shortId Optional short identifier (nullable)
		 * @param nameObject Optional name object
		 */
		DataChannelId(
			LocalId localId,
			std::optional<std::string> shortId = std::nullopt,
			std::optional<NameObject> nameObject = std::nullopt );

		/** @brief Copy constructor */
		DataChannelId( const DataChannelId& ) = default;

		/** @brief Move constructor */
		DataChannelId( DataChannelId&& ) noexcept = default;

		/** @brief Destructor */
		~DataChannelId() = default;

		/** @brief Copy assignment operator */
		DataChannelId& operator=( const DataChannelId& ) = default;

		/** @brief Move assignment operator */
		DataChannelId& operator=( DataChannelId&& ) noexcept = default;

		/** @brief Get local identifier */
		[[nodiscard]] inline const LocalId& localId() const noexcept;

		/** @brief Get optional short identifier */
		[[nodiscard]] inline const std::optional<std::string>& shortId() const noexcept;

		/** @brief Get optional name object */
		[[nodiscard]] inline const std::optional<NameObject>& nameObject() const noexcept;

		/** @brief Set local identifier */
		inline void setLocalId( LocalId localId );

		/** @brief Set optional short identifier */
		inline void setShortId( std::optional<std::string> shortId );

		/** @brief Set optional name object */
		inline void setNameObject( std::optional<NameObject> nameObject );

	private:
		LocalId m_localId;
		std::optional<std::string> m_shortId;
		std::optional<NameObject> m_nameObject;
	};

	//=====================================================================
	// DataChannel class
	//=====================================================================

	/**
	 * @brief Data channel with identifier and property
	 * @details Contains a data channel with:
	 *          - DataChannelId: required identifier
	 *          - Property: required property (validated on construction)
	 */
	class DataChannel final
	{
	public:
		/** @brief Default constructor */
		DataChannel() = delete;

		/**
		 * @brief Construct DataChannel with all fields
		 * @param dataChannelId Data channel identifier (required)
		 * @param property Property specification (required, validated)
		 * @throws std::invalid_argument if property validation fails
		 */
		DataChannel(
			DataChannelId dataChannelId,
			Property property );

		/** @brief Copy constructor */
		DataChannel( const DataChannel& ) = default;

		/** @brief Move constructor */
		DataChannel( DataChannel&& ) noexcept = default;

		/** @brief Destructor */
		~DataChannel() = default;

		/** @brief Copy assignment operator */
		DataChannel& operator=( const DataChannel& ) = default;

		/** @brief Move assignment operator */
		DataChannel& operator=( DataChannel&& ) noexcept = default;

		/** @brief Get data channel identifier */
		[[nodiscard]] inline const DataChannelId& dataChannelId() const noexcept;

		/** @brief Get property specification */
		[[nodiscard]] inline const Property& property() const noexcept;

		/** @brief Set data channel identifier */
		inline void setDataChannelId( DataChannelId dataChannelId );

		/**
		 * @brief Set property specification with validation
		 * @param property Property to set
		 * @throws std::invalid_argument if property validation fails
		 */
		inline void setProperty( Property property );

	private:
		DataChannelId m_dataChannelId;
		Property m_property;
	};

	//=====================================================================
	// DataChannelList class
	//=====================================================================

	/**
	 * @brief Collection of data channels with lookup by LocalId, ShortId, and index
	 * @details Implements a collection that maintains three lookup structures:
	 *          - List by insertion order
	 *          - Map by LocalId (required, unique)
	 *          - Map by ShortId (optional, unique if present)
	 */
	class DataChannelList final
	{
	public:
		using Iterator = std::vector<DataChannel>::iterator;
		using ConstIterator = std::vector<DataChannel>::const_iterator;

		/** @brief Default constructor */
		DataChannelList() = default;

		/**
		 * @brief Construct DataChannelList with initial data channels
		 * @param dataChannels Vector of data channels to add
		 */
		DataChannelList( std::vector<DataChannel> dataChannels );

		/** @brief Copy constructor */
		DataChannelList( const DataChannelList& ) = default;

		/** @brief Move constructor */
		DataChannelList( DataChannelList&& ) noexcept = default;

		/** @brief Destructor */
		~DataChannelList() = default;

		/** @brief Copy assignment operator */
		DataChannelList& operator=( const DataChannelList& ) = default;

		/** @brief Move assignment operator */
		DataChannelList& operator=( DataChannelList&& ) noexcept = default;

		/** @brief Get read-only vector of all data channels */
		[[nodiscard]] inline const std::vector<DataChannel>& dataChannels() const noexcept;

		/** @brief Get number of data channels */
		[[nodiscard]] inline std::size_t size() const noexcept;

		/**
		 * @brief Get data channel by short ID
		 * @param shortId Short identifier to lookup
		 * @return Optional reference to data channel if found
		 */
		[[nodiscard]] inline std::optional<std::reference_wrapper<const DataChannel>> from( std::string_view shortId ) const noexcept;

		/**
		 * @brief Get data channel by local ID
		 * @param localId Local identifier to lookup
		 * @return Optional reference to data channel if found
		 */
		[[nodiscard]] inline std::optional<std::reference_wrapper<const DataChannel>> from( const LocalId& localId ) const noexcept;

		/**
		 * @brief Add a data channel to the collection
		 * @param dataChannel Data channel to add
		 * @throws std::invalid_argument if LocalId already exists or ShortId conflicts
		 */
		inline void add( DataChannel dataChannel );

		/**
		 * @brief Add multiple data channels to the collection
		 * @param dataChannels Vector of data channels to add
		 * @throws std::invalid_argument if any LocalId already exists or ShortId conflicts
		 */
		inline void add( const std::vector<DataChannel>& dataChannels );

		/**
		 * @brief Remove a data channel from the collection
		 * @param item Data channel to remove
		 * @return true if removed, false if not found
		 */
		inline bool remove( const DataChannel& item );

		/** @brief Clear all data channels from the collection */
		inline void clear();

		/**
		 * @brief Get data channel by short ID
		 * @param shortId Short identifier
		 * @return Reference to data channel
		 * @throws std::out_of_range if not found
		 */
		[[nodiscard]] inline const DataChannel& operator[]( std::string_view shortId ) const;

		/**
		 * @brief Get data channel by index
		 * @param index Index in list
		 * @return Reference to data channel
		 * @throws std::out_of_range if index invalid
		 */
		[[nodiscard]] inline const DataChannel& operator[]( std::size_t index ) const;

		/**
		 * @brief Get data channel by local ID
		 * @param localId Local identifier
		 * @return Reference to data channel
		 * @throws std::out_of_range if not found
		 */
		[[nodiscard]] inline const DataChannel& operator[]( const LocalId& localId ) const;

		/**
		 * @brief Get iterator to beginning
		 * @return Iterator to first data channel
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Iterator begin() noexcept;

		/**
		 * @brief Get const iterator to beginning
		 * @return Const iterator to first data channel
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline ConstIterator begin() const noexcept;

		/**
		 * @brief Get iterator to end
		 * @return Iterator past last data channel
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Iterator end() noexcept;

		/**
		 * @brief Get const iterator to end
		 * @return Const iterator past last data channel
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline ConstIterator end() const noexcept;

	private:
		std::vector<DataChannel> m_dataChannels;
	};

	//=====================================================================
	// Package class
	//=====================================================================

	/**
	 * @brief Package containing header and data channel list
	 * @details Combines metadata header with the data channel collection
	 */
	class Package final
	{
	public:
		/** @brief Default constructor  */
		Package() = delete;

		/**
		 * @brief Construct Package with all fields
		 * @param header Package header (required)
		 * @param dataChannelList Data channel list (required)
		 */
		Package(
			Header header,
			DataChannelList dataChannelList );

		/** @brief Copy constructor */
		Package( const Package& ) = default;

		/** @brief Move constructor */
		Package( Package&& ) noexcept = default;

		/** @brief Destructor */
		~Package() = default;

		/** @brief Copy assignment operator */
		Package& operator=( const Package& ) = default;

		/** @brief Move assignment operator */
		Package& operator=( Package&& ) noexcept = default;

		/** @brief Get package header */
		[[nodiscard]] inline const Header& header() const noexcept;

		/** @brief Get data channel list */
		[[nodiscard]] inline const DataChannelList& dataChannelList() const noexcept;

		/** @brief Set package header */
		inline void setHeader( Header header );

		/** @brief Set data channel list */
		inline void setDataChannelList( DataChannelList dataChannelList );

	private:
		Header m_header;
		DataChannelList m_dataChannelList;
	};

	//=====================================================================
	// DataChannelListPackage class
	//=====================================================================

	/**
	 * @brief Top-level package wrapper
	 * @details Wraps a Package with convenient access to DataChannelList
	 */
	class DataChannelListPackage final
	{
	public:
		/** @brief Default constructor  */
		DataChannelListPackage() = delete;

		/**
		 * @brief Construct DataChannelListPackage with package
		 * @param package Package to wrap (required)
		 */
		DataChannelListPackage( Package package );

		/** @brief Copy constructor */
		DataChannelListPackage( const DataChannelListPackage& ) = default;

		/** @brief Move constructor */
		DataChannelListPackage( DataChannelListPackage&& ) noexcept = default;

		/** @brief Destructor */
		~DataChannelListPackage() = default;

		/** @brief Copy assignment operator */
		DataChannelListPackage& operator=( const DataChannelListPackage& ) = default;

		/** @brief Move assignment operator */
		DataChannelListPackage& operator=( DataChannelListPackage&& ) noexcept = default;

		/** @brief Get package */
		[[nodiscard]] inline const Package& package() const noexcept;

		/** @brief Get data channel list (convenience accessor) */
		[[nodiscard]] inline const DataChannelList& dataChannelList() const noexcept;

		/** @brief Set package */
		inline void setPackage( Package package );

	private:
		Package m_package;
	};
} // namespace dnv::vista::sdk::transport::datachannel

#include "dnv/vista/sdk/detail/transport/datachannel/DataChannel.inl"
