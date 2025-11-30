/**
 * @file TimeSeriesData.inl
 * @brief Inline implementations for TimeSeriesData classes
 */

namespace dnv::vista::sdk::transport::timeseries
{
	//=====================================================================
	// TimeRange class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline TimeRange::TimeRange( nfx::time::DateTimeOffset start, nfx::time::DateTimeOffset end )
		: m_start{ std::move( start ) },
		  m_end{ std::move( end ) }
	{
		if ( m_start > m_end )
		{
			throw std::invalid_argument{ "Start time must be before or equal to end time" };
		}
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const nfx::time::DateTimeOffset& TimeRange::start() const noexcept
	{
		return m_start;
	}

	inline const nfx::time::DateTimeOffset& TimeRange::end() const noexcept
	{
		return m_end;
	}

	//=====================================================================
	// ConfigurationReference class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline ConfigurationReference::ConfigurationReference( std::string_view id, nfx::time::DateTimeOffset timeStamp )
		: m_id{ id },
		  m_timeStamp{ std::move( timeStamp ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const std::string& ConfigurationReference::id() const noexcept
	{
		return m_id;
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

	inline void ConfigurationReference::setTimeStamp( nfx::time::DateTimeOffset timeStamp )
	{
		m_timeStamp = std::move( timeStamp );
	}

	//=====================================================================
	// TabularDataSet class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline TabularDataSet::TabularDataSet(
		nfx::time::DateTimeOffset timeStamp,
		std::vector<std::string> value,
		std::optional<std::vector<std::string>> quality )
		: m_timeStamp{ std::move( timeStamp ) },
		  m_value{ std::move( value ) },
		  m_quality{ std::move( quality ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const nfx::time::DateTimeOffset& TabularDataSet::timeStamp() const noexcept
	{
		return m_timeStamp;
	}

	inline const std::vector<std::string>& TabularDataSet::value() const noexcept
	{
		return m_value;
	}

	inline const std::optional<std::vector<std::string>>& TabularDataSet::quality() const noexcept
	{
		return m_quality;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void TabularDataSet::setTimeStamp( nfx::time::DateTimeOffset timeStamp )
	{
		m_timeStamp = std::move( timeStamp );
	}

	inline void TabularDataSet::setValue( std::vector<std::string> value )
	{
		m_value = std::move( value );
	}

	inline void TabularDataSet::setQuality( std::optional<std::vector<std::string>> quality )
	{
		m_quality = std::move( quality );
	}

	//=====================================================================
	// EventDataSet class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline EventDataSet::EventDataSet(
		nfx::time::DateTimeOffset timeStamp,
		DataChannelId dataChannelId,
		std::string value,
		std::optional<std::string> quality )
		: m_timeStamp{ std::move( timeStamp ) },
		  m_dataChannelId{ std::move( dataChannelId ) },
		  m_value{ std::move( value ) },
		  m_quality{ std::move( quality ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const nfx::time::DateTimeOffset& EventDataSet::timeStamp() const noexcept
	{
		return m_timeStamp;
	}

	inline const DataChannelId& EventDataSet::dataChannelId() const noexcept
	{
		return m_dataChannelId;
	}

	inline const std::string& EventDataSet::value() const noexcept
	{
		return m_value;
	}

	inline const std::optional<std::string>& EventDataSet::quality() const noexcept
	{
		return m_quality;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void EventDataSet::setTimeStamp( nfx::time::DateTimeOffset timeStamp )
	{
		m_timeStamp = std::move( timeStamp );
	}

	inline void EventDataSet::setDataChannelId( DataChannelId dataChannelId )
	{
		m_dataChannelId = std::move( dataChannelId );
	}

	inline void EventDataSet::setValue( std::string_view value )
	{
		m_value = value;
	}

	inline void EventDataSet::setQuality( std::optional<std::string_view> quality )
	{
		if ( quality )
		{
			m_quality = *quality;
		}
		else
		{
			m_quality = std::nullopt;
		}
	}

	//=====================================================================
	// TabularData class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline TabularData::TabularData(
		std::vector<DataChannelId> dataChannelIds,
		std::vector<TabularDataSet> dataSets )
		: m_dataChannelIds{ std::move( dataChannelIds ) },
		  m_dataSets{ std::move( dataSets ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const std::vector<DataChannelId>& TabularData::dataChannelIds() const noexcept
	{
		return m_dataChannelIds;
	}

	inline const std::vector<TabularDataSet>& TabularData::dataSets() const noexcept
	{
		return m_dataSets;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void TabularData::setDataChannelIds( std::vector<DataChannelId> dataChannelIds )
	{
		m_dataChannelIds = std::move( dataChannelIds );
	}

	inline void TabularData::setDataSets( std::vector<TabularDataSet> dataSets )
	{
		m_dataSets = std::move( dataSets );
	}

	//=====================================================================
	// EventData class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline EventData::EventData( size_t capacity )
	{
		m_dataSet.reserve( capacity );
	}

	inline EventData::EventData( std::vector<EventDataSet> dataSet )
		: m_dataSet{ std::move( dataSet ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const std::vector<EventDataSet>& EventData::dataSet() const noexcept
	{
		return m_dataSet;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void EventData::setDataSet( std::vector<EventDataSet> dataSet )
	{
		m_dataSet = std::move( dataSet );
	}

	//----------------------------------------------
	// Collection interface
	//----------------------------------------------

	inline void EventData::add( EventDataSet dataSet )
	{
		m_dataSet.push_back( std::move( dataSet ) );
	}

	inline void EventData::clear()
	{
		m_dataSet.clear();
	}

	//=====================================================================
	// Header class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline Header::Header(
		ShipId shipId,
		std::optional<TimeRange> timeRange,
		std::optional<nfx::time::DateTimeOffset> dateCreated,
		std::optional<nfx::time::DateTimeOffset> dateModified,
		std::optional<std::string> author,
		std::optional<std::vector<ConfigurationReference>> systemConfiguration,
		std::optional<nfx::containers::FastHashMap<std::string, Value>> customHeaders )
		: m_shipId{ std::move( shipId ) },
		  m_timeRange{ std::move( timeRange ) },
		  m_dateCreated{ std::move( dateCreated ) },
		  m_dateModified{ std::move( dateModified ) },
		  m_author{ std::move( author ) },
		  m_systemConfiguration{ std::move( systemConfiguration ) },
		  m_customHeaders{ std::move( customHeaders ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const ShipId& Header::shipId() const noexcept
	{
		return m_shipId;
	}

	inline const std::optional<TimeRange>& Header::timeRange() const noexcept
	{
		return m_timeRange;
	}

	inline const std::optional<nfx::time::DateTimeOffset>& Header::dateCreated() const noexcept
	{
		return m_dateCreated;
	}

	inline const std::optional<nfx::time::DateTimeOffset>& Header::dateModified() const noexcept
	{
		return m_dateModified;
	}

	inline const std::optional<std::string>& Header::author() const noexcept
	{
		return m_author;
	}

	inline const std::optional<std::vector<ConfigurationReference>>& Header::systemConfiguration() const noexcept
	{
		return m_systemConfiguration;
	}

	inline const std::optional<nfx::containers::FastHashMap<std::string, Value>>& Header::customHeaders() const noexcept
	{
		return m_customHeaders;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void Header::setShipId( ShipId shipId )
	{
		m_shipId = std::move( shipId );
	}

	inline void Header::setTimeRange( std::optional<TimeRange> timeRange )
	{
		m_timeRange = std::move( timeRange );
	}

	inline void Header::setDateCreated( std::optional<nfx::time::DateTimeOffset> dateCreated )
	{
		m_dateCreated = std::move( dateCreated );
	}

	inline void Header::setDateModified( std::optional<nfx::time::DateTimeOffset> dateModified )
	{
		m_dateModified = std::move( dateModified );
	}

	inline void Header::setAuthor( std::optional<std::string_view> author )
	{
		if ( author )
		{
			m_author = *author;
		}
		else
		{
			m_author = std::nullopt;
		}
	}

	inline void Header::setSystemConfiguration( std::optional<std::vector<ConfigurationReference>> systemConfiguration )
	{
		m_systemConfiguration = std::move( systemConfiguration );
	}

	inline void Header::setCustomHeaders( std::optional<nfx::containers::FastHashMap<std::string, Value>> customHeaders )
	{
		m_customHeaders = std::move( customHeaders );
	}

	//=====================================================================
	// TimeSeriesData class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline TimeSeriesData::TimeSeriesData(
		std::optional<ConfigurationReference> dataConfiguration,
		std::optional<std::vector<TabularData>> tabularData,
		std::optional<EventData> eventData,
		std::optional<nfx::containers::FastHashMap<std::string, Value>> customDataKinds )
		: m_dataConfiguration{ std::move( dataConfiguration ) },
		  m_tabularData{ std::move( tabularData ) },
		  m_eventData{ std::move( eventData ) },
		  m_customDataKinds{ std::move( customDataKinds ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const std::optional<ConfigurationReference>& TimeSeriesData::dataConfiguration() const noexcept
	{
		return m_dataConfiguration;
	}

	inline const std::optional<std::vector<TabularData>>& TimeSeriesData::tabularData() const noexcept
	{
		return m_tabularData;
	}

	inline const std::optional<EventData>& TimeSeriesData::eventData() const noexcept
	{
		return m_eventData;
	}

	inline const std::optional<nfx::containers::FastHashMap<std::string, Value>>& TimeSeriesData::customDataKinds() const noexcept
	{
		return m_customDataKinds;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void TimeSeriesData::setDataConfiguration( std::optional<ConfigurationReference> dataConfiguration )
	{
		m_dataConfiguration = std::move( dataConfiguration );
	}

	inline void TimeSeriesData::setTabularData( std::optional<std::vector<TabularData>> tabularData )
	{
		m_tabularData = std::move( tabularData );
	}

	inline void TimeSeriesData::setEventData( std::optional<EventData> eventData )
	{
		m_eventData = std::move( eventData );
	}

	inline void TimeSeriesData::setCustomDataKinds( std::optional<nfx::containers::FastHashMap<std::string, Value>> customDataKinds )
	{
		m_customDataKinds = std::move( customDataKinds );
	}

	//=====================================================================
	// Package class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline Package::Package(
		std::optional<Header> header,
		std::vector<TimeSeriesData> timeSeriesData )
		: m_header{ std::move( header ) },
		  m_timeSeriesData{ std::move( timeSeriesData ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const std::optional<Header>& Package::header() const noexcept
	{
		return m_header;
	}

	inline const std::vector<TimeSeriesData>& Package::timeSeriesData() const noexcept
	{
		return m_timeSeriesData;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void Package::setHeader( std::optional<Header> header )
	{
		m_header = std::move( header );
	}

	inline void Package::setTimeSeriesData( std::vector<TimeSeriesData> timeSeriesData )
	{
		m_timeSeriesData = std::move( timeSeriesData );
	}

	//=====================================================================
	// TimeSeriesDataPackage class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline TimeSeriesDataPackage::TimeSeriesDataPackage( Package package )
		: m_package{ std::move( package ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	inline const Package& TimeSeriesDataPackage::package() const noexcept
	{
		return m_package;
	}

	inline Package& TimeSeriesDataPackage::package() noexcept
	{
		return m_package;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	inline void TimeSeriesDataPackage::setPackage( Package package )
	{
		m_package = std::move( package );
	}
} // namespace dnv::vista::sdk::transport::timeseries
