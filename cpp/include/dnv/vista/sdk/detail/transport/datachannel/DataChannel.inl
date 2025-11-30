/**
 * @file DataChannel.inl
 * @brief Data channel infrastructure inline implementations
 * @details Inline method implementations for data channel classes
 */

#include "dnv/vista/sdk/transport/ISO19848Constants.h"

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// VersionInformation class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline VersionInformation::VersionInformation()
		: m_namingRule{ transport::ISO19848_ANNEX_C_NAMING_RULE },
		  m_namingSchemeVersion{ transport::ISO19848_ANNEX_C_NAMING_SCHEME_VERSION },
		  m_referenceUrl{ transport::ISO19848_ANNEX_C_REFERENCE_URL }
	{
	}

	inline VersionInformation::VersionInformation( const std::string_view namingRule, const std::string_view namingSchemeVersion, const std::optional<std::string>& referenceUrl )
		: m_namingRule{ namingRule },
		  m_namingSchemeVersion{ namingSchemeVersion },
		  m_referenceUrl{ referenceUrl }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const std::string& VersionInformation::namingRule() const noexcept
	{
		return m_namingRule;
	}

	inline const std::string& VersionInformation::namingSchemeVersion() const noexcept
	{
		return m_namingSchemeVersion;
	}

	inline const std::optional<std::string>& VersionInformation::referenceUrl() const noexcept
	{
		return m_referenceUrl;
	}

	//=====================================================================
	// ConfigurationReference class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline ConfigurationReference::ConfigurationReference( std::string_view id, nfx::time::DateTimeOffset timeStamp, std::optional<std::string_view> version )
		: m_id{ id },
		  m_timeStamp{ timeStamp },
		  m_version{ version ? std::optional<std::string>{ *version }
							 : std::nullopt }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const std::string& ConfigurationReference::id() const noexcept
	{
		return m_id;
	}

	inline const std::optional<std::string>& ConfigurationReference::version() const noexcept
	{
		return m_version;
	}

	inline const nfx::time::DateTimeOffset& ConfigurationReference::timeStamp() const noexcept
	{
		return m_timeStamp;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void ConfigurationReference::setId( std::string_view id )
	{
		m_id = id;
	}

	inline void ConfigurationReference::setVersion( std::optional<std::string_view> version )
	{
		if ( version )
		{
			m_version = *version;
		}
		else
		{
			m_version = std::nullopt;
		}
	}

	inline void ConfigurationReference::setTimeStamp( nfx::time::DateTimeOffset timeStamp )
	{
		m_timeStamp = timeStamp;
	}

	//=====================================================================
	// Restriction class
	//=====================================================================

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const std::optional<std::vector<std::string>>& Restriction::enumeration() const noexcept
	{
		return m_enumeration;
	}

	inline std::optional<uint32_t> Restriction::fractionDigits() const noexcept
	{
		return m_fractionDigits;
	}

	inline std::optional<uint32_t> Restriction::length() const noexcept
	{
		return m_length;
	}

	inline std::optional<double> Restriction::maxExclusive() const noexcept
	{
		return m_maxExclusive;
	}

	inline std::optional<double> Restriction::maxInclusive() const noexcept
	{
		return m_maxInclusive;
	}

	inline std::optional<uint32_t> Restriction::maxLength() const noexcept
	{
		return m_maxLength;
	}

	inline std::optional<double> Restriction::minExclusive() const noexcept
	{
		return m_minExclusive;
	}

	inline std::optional<double> Restriction::minInclusive() const noexcept
	{
		return m_minInclusive;
	}

	inline std::optional<uint32_t> Restriction::minLength() const noexcept
	{
		return m_minLength;
	}

	inline const std::optional<std::string>& Restriction::pattern() const noexcept
	{
		return m_pattern;
	}

	inline std::optional<uint32_t> Restriction::totalDigits() const noexcept
	{
		return m_totalDigits;
	}

	inline std::optional<WhiteSpace> Restriction::whiteSpace() const noexcept
	{
		return m_whiteSpace;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void Restriction::setEnumeration( std::optional<std::vector<std::string>> enumeration )
	{
		m_enumeration = std::move( enumeration );
	}

	inline void Restriction::setFractionDigits( std::optional<uint32_t> fractionDigits )
	{
		m_fractionDigits = fractionDigits;
	}

	inline void Restriction::setLength( std::optional<uint32_t> length )
	{
		m_length = length;
	}

	inline void Restriction::setMaxExclusive( std::optional<double> maxExclusive )
	{
		m_maxExclusive = maxExclusive;
	}

	inline void Restriction::setMaxInclusive( std::optional<double> maxInclusive )
	{
		m_maxInclusive = maxInclusive;
	}

	inline void Restriction::setMaxLength( std::optional<uint32_t> maxLength )
	{
		m_maxLength = maxLength;
	}

	inline void Restriction::setMinExclusive( std::optional<double> minExclusive )
	{
		m_minExclusive = minExclusive;
	}

	inline void Restriction::setMinInclusive( std::optional<double> minInclusive )
	{
		m_minInclusive = minInclusive;
	}

	inline void Restriction::setMinLength( std::optional<uint32_t> minLength )
	{
		m_minLength = minLength;
	}

	inline void Restriction::setPattern( std::optional<std::string> pattern )
	{
		m_pattern = std::move( pattern );
	}

	inline void Restriction::setWhiteSpace( std::optional<WhiteSpace> whiteSpace )
	{
		m_whiteSpace = whiteSpace;
	}

	//=====================================================================
	// Range class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline Range::Range( double low, double high )
		: m_low{ low },
		  m_high{ high }
	{
		if ( low > high )
		{
			throw std::invalid_argument{ "Range low bound cannot be greater than high bound" };
		}

		if ( low < std::numeric_limits<double>::lowest() )
		{
			throw std::invalid_argument{ "Range low value is below minimum limit" };
		}

		if ( high > std::numeric_limits<double>::max() )
		{
			throw std::invalid_argument{ "Range high value is above maximum limit" };
		}
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline double Range::low() const noexcept
	{
		return m_low;
	}

	inline double Range::high() const noexcept
	{
		return m_high;
	}

	//=====================================================================
	// Unit class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline Unit::Unit(
		std::string_view unitSymbol,
		std::optional<std::string> quantityName,
		std::optional<nfx::containers::FastHashMap<std::string, Value>> customElements )
		: m_unitSymbol{ unitSymbol },
		  m_quantityName{ std::move( quantityName ) },
		  m_customElements{ std::move( customElements ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const std::string& Unit::unitSymbol() const noexcept
	{
		return m_unitSymbol;
	}

	inline const std::optional<std::string>& Unit::quantityName() const noexcept
	{
		return m_quantityName;
	}

	inline const std::optional<nfx::containers::FastHashMap<std::string, transport::Value>>& Unit::customElements() const noexcept
	{
		return m_customElements;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void Unit::setUnitSymbol( std::string_view unitSymbol )
	{
		m_unitSymbol = unitSymbol;
	}

	inline void Unit::setQuantityName( std::optional<std::string> quantityName )
	{
		m_quantityName = std::move( quantityName );
	}

	inline void Unit::setCustomElements( std::optional<nfx::containers::FastHashMap<std::string, transport::Value>> customElements )
	{
		m_customElements = std::move( customElements );
	}

	//=====================================================================
	// Header class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline Header::Header()
		: m_author{ "Unknown" },
		  m_dateCreated{ nfx::time::DateTimeOffset::utcNow() },
		  m_shipId{ ShipId::parse( "IMO0000000" ) },
		  m_dataChannelListId{ "DefaultChannelList", nfx::time::DateTimeOffset::utcNow() },
		  m_versionInformation{ std::nullopt },
		  m_customHeaders{ std::nullopt }
	{
	}

	inline Header::Header(
		std::string author,
		nfx::time::DateTimeOffset dateCreated,
		ShipId shipId,
		ConfigurationReference dataChannelListId,
		std::optional<VersionInformation> versionInformation,
		std::optional<nfx::containers::FastHashMap<std::string, Value>> customHeaders )
		: m_author{ std::move( author ) },
		  m_dateCreated{ std::move( dateCreated ) },
		  m_shipId{ std::move( shipId ) },
		  m_dataChannelListId{ std::move( dataChannelListId ) },
		  m_versionInformation{ std::move( versionInformation ) },
		  m_customHeaders{ std::move( customHeaders ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const std::string& Header::author() const noexcept
	{
		return m_author;
	}

	inline const nfx::time::DateTimeOffset& Header::dateCreated() const noexcept
	{
		return m_dateCreated;
	}

	inline const ShipId& Header::shipId() const noexcept
	{
		return m_shipId;
	}

	inline const ConfigurationReference& Header::dataChannelListId() const noexcept
	{
		return m_dataChannelListId;
	}

	inline const std::optional<VersionInformation>& Header::versionInformation() const noexcept
	{
		return m_versionInformation;
	}

	inline const std::optional<nfx::containers::FastHashMap<std::string, transport::Value>>& Header::customHeaders() const noexcept
	{
		return m_customHeaders;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void Header::setAuthor( std::string author )
	{
		m_author = std::move( author );
	}

	inline void Header::setDateCreated( nfx::time::DateTimeOffset dateCreated )
	{
		m_dateCreated = std::move( dateCreated );
	}

	inline void Header::setShipId( ShipId shipId )
	{
		m_shipId = std::move( shipId );
	}

	inline void Header::setDataChannelListId( ConfigurationReference dataChannelListId )
	{
		m_dataChannelListId = std::move( dataChannelListId );
	}

	inline void Header::setVersionInformation( std::optional<VersionInformation> versionInformation )
	{
		m_versionInformation = std::move( versionInformation );
	}

	inline void Header::setCustomHeaders( std::optional<nfx::containers::FastHashMap<std::string, Value>> customHeaders )
	{
		m_customHeaders = std::move( customHeaders );
	}

	//=====================================================================
	// NameObject class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline NameObject::NameObject(
		std::optional<std::string_view> namingRule,
		std::optional<nfx::containers::FastHashMap<std::string, Value>> customNameObjects )
		: m_namingRule{ namingRule.has_value() ? std::string{ namingRule.value() } : std::string{ transport::ISO19848_ANNEX_C_NAMING_RULE } },
		  m_customNameObjects{ std::move( customNameObjects ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const std::string& NameObject::namingRule() const noexcept
	{
		return m_namingRule;
	}

	inline const std::optional<nfx::containers::FastHashMap<std::string, transport::Value>>& NameObject::customNameObjects() const noexcept
	{
		return m_customNameObjects;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void NameObject::setNamingRule( std::string namingRule )
	{
		m_namingRule = std::move( namingRule );
	}

	inline void NameObject::setCustomNameObjects( std::optional<nfx::containers::FastHashMap<std::string, transport::Value>> customNameObjects )
	{
		m_customNameObjects = std::move( customNameObjects );
	}

	//=====================================================================
	// Format class
	//=====================================================================

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const std::string& Format::type() const
	{
		if ( !m_type )
		{
			throw std::runtime_error{ "Format type not set" };
		}
		return *m_type;
	}

	inline const std::optional<Restriction>& Format::restriction() const noexcept
	{
		return m_restriction;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void Format::setType( std::string_view type )
	{
		// Validate against ISO19848 format types
		auto formatTypes = ISO19848::instance().formatDataTypes( ISO19848Version::Latest );
		auto result = formatTypes.parse( type );

		if ( !result.isOk() )
		{
			throw std::invalid_argument{ "Invalid format type: " + std::string{ type } };
		}

		m_type = type;
		m_dataType = result.ok().typeName();
	}

	inline void Format::setRestriction( std::optional<Restriction> restriction )
	{
		m_restriction = std::move( restriction );
	}

	//=====================================================================
	// DataChannelType class
	//=====================================================================

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline std::optional<double> DataChannelType::updateCycle() const noexcept
	{
		return m_updateCycle;
	}

	inline std::optional<double> DataChannelType::calculationPeriod() const noexcept
	{
		return m_calculationPeriod;
	}

	inline bool DataChannelType::isAlert() const noexcept
	{
		return m_type && *m_type == "Alert";
	}

	//=====================================================================
	// Property class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline Property::Property(
		DataChannelType dataChannelType,
		Format format,
		std::optional<Range> range,
		std::optional<Unit> unit,
		std::optional<std::string> qualityCoding,
		std::optional<std::string> alertPriority,
		std::optional<std::string> name,
		std::optional<std::string> remarks,
		std::optional<nfx::containers::FastHashMap<std::string, Value>> customProperties )
		: m_dataChannelType{ std::move( dataChannelType ) },
		  m_format{ std::move( format ) },
		  m_range{ std::move( range ) },
		  m_unit{ std::move( unit ) },
		  m_qualityCoding{ std::move( qualityCoding ) },
		  m_alertPriority{ std::move( alertPriority ) },
		  m_name{ std::move( name ) },
		  m_remarks{ std::move( remarks ) },
		  m_customProperties{ std::move( customProperties ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const DataChannelType& Property::dataChannelType() const noexcept
	{
		return m_dataChannelType;
	}

	inline const Format& Property::format() const noexcept
	{
		return m_format;
	}

	inline const std::optional<Range>& Property::range() const noexcept
	{
		return m_range;
	}

	inline const std::optional<Unit>& Property::unit() const noexcept
	{
		return m_unit;
	}

	inline const std::optional<std::string>& Property::qualityCoding() const noexcept
	{
		// TODO: Validate Quality coding 'In the case of “IEC 61162-STATUS”, “A” (Data valid) and “V” (Data invalid) are used for the data quality.'
		return m_qualityCoding;
	}

	inline const std::optional<std::string>& Property::alertPriority() const noexcept
	{
		// TODO: Validate - Priority level and criteria for classification shall be in accordance with IEC 62923-1:2018, 6.2.2.1
		return m_alertPriority;
	}

	inline const std::optional<std::string>& Property::name() const noexcept
	{
		return m_name;
	}

	inline const std::optional<std::string>& Property::remarks() const noexcept
	{
		return m_remarks;
	}

	inline const std::optional<nfx::containers::FastHashMap<std::string, transport::Value>>& Property::customProperties() const noexcept
	{
		return m_customProperties;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void Property::setDataChannelType( DataChannelType dataChannelType )
	{
		m_dataChannelType = std::move( dataChannelType );
	}

	inline void Property::setFormat( Format format )
	{
		m_format = std::move( format );
	}

	inline void Property::setRange( std::optional<Range> range )
	{
		m_range = std::move( range );
	}

	inline void Property::setUnit( std::optional<Unit> unit )
	{
		m_unit = std::move( unit );
	}

	inline void Property::setQualityCoding( std::optional<std::string> qualityCoding )
	{
		m_qualityCoding = std::move( qualityCoding );
	}

	inline void Property::setAlertPriority( std::optional<std::string> alertPriority )
	{
		m_alertPriority = std::move( alertPriority );
	}

	inline void Property::setName( std::optional<std::string> name )
	{
		m_name = std::move( name );
	}

	inline void Property::setRemarks( std::optional<std::string> remarks )
	{
		m_remarks = std::move( remarks );
	}

	inline void Property::setCustomProperties( std::optional<nfx::containers::FastHashMap<std::string, transport::Value>> customProperties )
	{
		m_customProperties = std::move( customProperties );
	}

	//=====================================================================
	// DataChannelId class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline DataChannelId::DataChannelId(
		LocalId localId,
		std::optional<std::string> shortId,
		std::optional<NameObject> nameObject )
		: m_localId{ std::move( localId ) },
		  m_shortId{ std::move( shortId ) },
		  m_nameObject{ std::move( nameObject ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const LocalId& DataChannelId::localId() const noexcept
	{
		return m_localId;
	}

	inline const std::optional<std::string>& DataChannelId::shortId() const noexcept
	{
		return m_shortId;
	}

	inline const std::optional<NameObject>& DataChannelId::nameObject() const noexcept
	{
		return m_nameObject;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void DataChannelId::setLocalId( LocalId localId )
	{
		m_localId = std::move( localId );
	}

	inline void DataChannelId::setShortId( std::optional<std::string> shortId )
	{
		m_shortId = std::move( shortId );
	}

	inline void DataChannelId::setNameObject( std::optional<NameObject> nameObject )
	{
		m_nameObject = std::move( nameObject );
	}

	//=====================================================================
	// DataChannel class
	//=====================================================================

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const DataChannelId& DataChannel::dataChannelId() const noexcept
	{
		return m_dataChannelId;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void DataChannel::setDataChannelId( DataChannelId dataChannelId )
	{
		m_dataChannelId = std::move( dataChannelId );
	}

	//=====================================================================
	// DataChannelList class
	//=====================================================================

	//----------------------------------------------
	// Collection interface
	//----------------------------------------------

	inline DataChannelList::size_type DataChannelList::size() const noexcept
	{
		return m_dataChannels.size();
	}

	inline bool DataChannelList::isEmpty() const noexcept
	{
		return m_dataChannels.empty();
	}

	//----------------------------------------------
	// Element access
	//----------------------------------------------

	inline const DataChannel& DataChannelList::operator[]( size_type index ) const
	{
		return m_dataChannels.at( index );
	}

	inline const DataChannel& DataChannelList::operator[]( const std::string& shortId ) const
	{
		const auto* channel = m_shortIdMap.find( shortId );
		if ( channel == nullptr )
		{
			throw std::out_of_range{ "Short ID not found: " + shortId };
		}

		return **channel;
	}

	inline const DataChannel& DataChannelList::operator[]( const LocalId& localId ) const
	{
		const auto* channel = m_localIdMap.find( localId );
		if ( channel == nullptr )
		{
			throw std::out_of_range{ "Local ID not found" };
		}

		return **channel;
	}

	//----------------------------------------------
	// Iterators
	//----------------------------------------------

	inline DataChannelList::iterator DataChannelList::begin() noexcept
	{
		return m_dataChannels.begin();
	}

	inline DataChannelList::const_iterator DataChannelList::begin() const noexcept
	{
		return m_dataChannels.begin();
	}

	inline DataChannelList::iterator DataChannelList::end() noexcept
	{
		return m_dataChannels.end();
	}

	inline DataChannelList::const_iterator DataChannelList::end() const noexcept
	{
		return m_dataChannels.end();
	}

	//----------------------------------------------
	// Data access
	//----------------------------------------------

	inline const std::vector<DataChannel>& DataChannelList::dataChannels() const noexcept
	{
		return m_dataChannels;
	}

	//=====================================================================
	// Package class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	// inline Package::Package()
	// 	: m_header{},
	// 	  m_dataChannelList{}
	// {
	// }

	inline Package::Package( Header header, DataChannelList dataChannelList )
		: m_header{ std::move( header ) },
		  m_dataChannelList{ std::move( dataChannelList ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const Header& Package::header() const noexcept
	{
		return m_header;
	}

	inline Header& Package::header() noexcept
	{
		return m_header;
	}

	inline const DataChannelList& Package::dataChannelList() const noexcept
	{
		return m_dataChannelList;
	}

	inline DataChannelList& Package::dataChannelList() noexcept
	{
		return m_dataChannelList;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void Package::setHeader( Header header )
	{
		m_header = std::move( header );
	}

	inline void Package::setDataChannelList( DataChannelList dataChannelList )
	{
		m_dataChannelList = std::move( dataChannelList );
	}

	//=====================================================================
	// DataChannelListPackage class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline DataChannelListPackage::DataChannelListPackage( Package package )
		: m_package{ std::move( package ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const Package& DataChannelListPackage::package() const noexcept
	{
		return m_package;
	}

	inline Package& DataChannelListPackage::package() noexcept
	{
		return m_package;
	}

	inline const DataChannelList& DataChannelListPackage::dataChannelList() const noexcept
	{
		return m_package.dataChannelList();
	}

	inline DataChannelList& DataChannelListPackage::dataChannelList() noexcept
	{
		return m_package.dataChannelList();
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void DataChannelListPackage::setPackage( Package package )
	{
		m_package = std::move( package );
	}
} // namespace dnv::vista::sdk::transport
