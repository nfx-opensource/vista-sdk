/**
 * @file TimeSeriesData.h
 * @brief Time series data package for ISO 19848 compliance
 */

#pragma once

#include "dnv/vista/sdk/transport/timeseries/DataChannelId.h"
#include "dnv/vista/sdk/transport/ISO19848.h"
#include "dnv/vista/sdk/transport/Results.h"
#include "dnv/vista/sdk/transport/ShipId.h"

#include <nfx/serialization/json/Document.h>

#include <functional>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>

namespace dnv::vista::sdk::transport::datachannel
{
	// Forward declarations from datachannel namespace
	class DataChannelListPackage;
	class DataChannel;
} // namespace dnv::vista::sdk::transport::datachannel

namespace dnv::vista::sdk::transport::timeseries
{
	//=====================================================================
	// Type aliases
	//=====================================================================

	/** @brief Alias for custom headers - supports arbitrary JSON */
	using CustomHeaders = nfx::serialization::json::Document;

	/** @brief Alias for custom headers - supports arbitrary JSON */
	using CustomData = nfx::serialization::json::Document;

	//=====================================================================
	// ConfigurationReference class
	//=====================================================================

	/**
	 * @brief Configuration reference with identifier and timestamp
	 * @details References a configuration (e.g., DataChannelList) with:
	 *          - Id: required identifier
	 *          - TimeStamp: required timestamp
	 */
	class ConfigurationReference final
	{
	public:
		/** @brief Default constructor */
		ConfigurationReference() = delete;

		/**
		 * @brief Construct ConfigurationReference with required fields
		 * @param id Configuration identifier (required)
		 * @param timeStamp Configuration timestamp (required)
		 */
		ConfigurationReference( std::string id, DateTimeOffset timeStamp );

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

		/** @brief Get timestamp */
		[[nodiscard]] inline const DateTimeOffset& timeStamp() const noexcept;

		/** @brief Set configuration identifier */
		inline void setId( std::string id );

		/** @brief Set timestamp */
		inline void setTimeStamp( DateTimeOffset timeStamp );

	private:
		std::string m_id;
		DateTimeOffset m_timeStamp;
	};

	//=====================================================================
	// TimeRange class
	//=====================================================================

	/**
	 * @brief Time range with start and end timestamps
	 * @details Defines a time interval for time series data with explicit start and end points
	 */
	class TimeRange final
	{
	public:
		/** @brief Default constructor */
		TimeRange() = delete;

		/**
		 * @brief Construct TimeRange with start and end
		 * @param start Start timestamp (required)
		 * @param end End timestamp (required)
		 */
		TimeRange( DateTimeOffset start, DateTimeOffset end );

		/** @brief Copy constructor */
		TimeRange( const TimeRange& ) = default;

		/** @brief Move constructor */
		TimeRange( TimeRange&& ) noexcept = default;

		/** @brief Destructor */
		~TimeRange() = default;

		/** @brief Copy assignment operator */
		TimeRange& operator=( const TimeRange& ) = default;

		/** @brief Move assignment operator */
		TimeRange& operator=( TimeRange&& ) noexcept = default;

		/** @brief Get start timestamp */
		[[nodiscard]] inline const DateTimeOffset& start() const noexcept;

		/** @brief Get end timestamp */
		[[nodiscard]] inline const DateTimeOffset& end() const noexcept;

		/** @brief Set start timestamp */
		inline void setStart( DateTimeOffset start );

		/** @brief Set end timestamp */
		inline void setEnd( DateTimeOffset end );

	private:
		DateTimeOffset m_start;
		DateTimeOffset m_end;
	};

	//=====================================================================
	// Header class
	//=====================================================================

	/**
	 * @brief Time series package header with metadata
	 * @details Contains metadata for time series data package:
	 *          - ShipId: required ship identifier
	 *          - TimeRange: optional time range for the data
	 *          - DateCreated: optional creation timestamp
	 *          - DateModified: optional modification timestamp
	 *          - Author: optional author information
	 *          - SystemConfiguration: optional system configuration references
	 *          - CustomHeaders: optional extension data dictionary
	 */
	class Header final
	{
	public:
		/** @brief Default constructor */
		Header() = delete;

		/**
		 * @brief Construct Header with required fields
		 * @param shipId Ship identifier (required)
		 * @param timeRange Optional time range
		 * @param dateCreated Optional creation date
		 * @param dateModified Optional modification date
		 * @param author Optional author
		 * @param systemConfiguration Optional system configuration list
		 * @param customHeaders Optional custom headers dictionary
		 */
		Header(
			ShipId shipId,
			std::optional<TimeRange> timeRange = std::nullopt,
			std::optional<DateTimeOffset> dateCreated = std::nullopt,
			std::optional<DateTimeOffset> dateModified = std::nullopt,
			std::optional<std::string> author = std::nullopt,
			std::optional<std::vector<ConfigurationReference>> systemConfiguration = std::nullopt,
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

		/** @brief Get ship identifier */
		[[nodiscard]] inline const ShipId& shipId() const noexcept;

		/** @brief Get optional time range */
		[[nodiscard]] inline const std::optional<TimeRange>& timeRange() const noexcept;

		/** @brief Get optional creation date */
		[[nodiscard]] inline const std::optional<DateTimeOffset>& dateCreated() const noexcept;

		/** @brief Get optional modification date */
		[[nodiscard]] inline const std::optional<DateTimeOffset>& dateModified() const noexcept;
		/** @brief Get optional author */
		[[nodiscard]] inline const std::optional<std::string>& author() const noexcept;

		/** @brief Get optional system configuration list */
		[[nodiscard]] inline const std::optional<std::vector<ConfigurationReference>>& systemConfiguration() const noexcept;

		/** @brief Get optional custom headers */
		[[nodiscard]] inline const std::optional<CustomHeaders>& customHeaders() const noexcept;

		/** @brief Set ship identifier */
		inline void setShipId( ShipId shipId );

		/** @brief Set optional time range */
		inline void setTimeRange( std::optional<TimeRange> timeRange );

		/** @brief Set optional creation date */
		inline void setDateCreated( std::optional<DateTimeOffset> dateCreated );

		/** @brief Set optional modification date */
		inline void setDateModified( std::optional<DateTimeOffset> dateModified );
		/** @brief Set optional author */
		inline void setAuthor( std::optional<std::string> author );

		/** @brief Set optional system configuration list */
		inline void setSystemConfiguration( std::optional<std::vector<ConfigurationReference>> systemConfiguration );

		/** @brief Set optional custom headers */
		inline void setCustomHeaders( std::optional<CustomHeaders> customHeaders );

	private:
		ShipId m_shipId;
		std::optional<TimeRange> m_timeRange;
		std::optional<DateTimeOffset> m_dateCreated;
		std::optional<DateTimeOffset> m_dateModified;
		std::optional<std::string> m_author;
		std::optional<std::vector<ConfigurationReference>> m_systemConfiguration;
		std::optional<CustomHeaders> m_customHeaders;
	};

	//=====================================================================
	// TabularDataSet class
	//=====================================================================

	/**
	 * @brief Single tabular data set with timestamp and values
	 * @details Contains a time-stamped row of data values with optional quality indicators
	 */
	class TabularDataSet final
	{
	public:
		/** @brief Default constructor */
		TabularDataSet() = delete;

		/**
		 * @brief Construct TabularDataSet with required fields
		 * @param timeStamp Timestamp for this data set (required)
		 * @param value List of string values (required, one per data channel)
		 * @param quality Optional quality indicators (one per data channel)
		 */
		TabularDataSet(
			DateTimeOffset timeStamp,
			std::vector<std::string> value,
			std::optional<std::vector<std::string>> quality = std::nullopt );

		/** @brief Copy constructor */
		TabularDataSet( const TabularDataSet& ) = default;

		/** @brief Move constructor */
		TabularDataSet( TabularDataSet&& ) noexcept = default;

		/** @brief Destructor */
		~TabularDataSet() = default;

		/** @brief Copy assignment operator */
		TabularDataSet& operator=( const TabularDataSet& ) = default;

		/** @brief Move assignment operator */
		TabularDataSet& operator=( TabularDataSet&& ) noexcept = default;

		/** @brief Get timestamp */
		[[nodiscard]] inline const DateTimeOffset& timeStamp() const noexcept;

		/** @brief Get value list */
		[[nodiscard]] inline const std::vector<std::string>& value() const noexcept;

		/** @brief Get optional quality list */
		[[nodiscard]] inline const std::optional<std::vector<std::string>>& quality() const noexcept;

		/** @brief Set timestamp */
		inline void setTimeStamp( DateTimeOffset timeStamp );

		/** @brief Set value list */
		inline void setValue( std::vector<std::string> value );

		/** @brief Set optional quality list */
		inline void setQuality( std::optional<std::vector<std::string>> quality );

	private:
		DateTimeOffset m_timeStamp;
		std::vector<std::string> m_value;
		std::optional<std::vector<std::string>> m_quality;
	};

	//=====================================================================
	// TabularData class
	//=====================================================================

	/**
	 * @brief Tabular time series data with data channel mapping
	 * @details Contains tabular data with:
	 *          - DataChannelIds: required list of data channel identifiers
	 *          - DataSets: required list of time-stamped data sets
	 *          - NumberOfDataSets: computed from DataSets size
	 *          - NumberOfDataChannels: computed from DataChannelIds size
	 */
	class TabularData final
	{
	public:
		/** @brief Default constructor */
		TabularData() = delete;

		/**
		 * @brief Construct TabularData with required fields
		 * @param dataChannelIds List of data channel identifiers (required)
		 * @param dataSets List of data sets (required)
		 */
		TabularData(
			std::vector<DataChannelId> dataChannelIds,
			std::vector<TabularDataSet> dataSets );

		/** @brief Copy constructor */
		TabularData( const TabularData& ) = default;

		/** @brief Move constructor */
		TabularData( TabularData&& ) noexcept = default;

		/** @brief Destructor */
		~TabularData() = default;

		/** @brief Copy assignment operator */
		TabularData& operator=( const TabularData& ) = default;

		/** @brief Move assignment operator */
		TabularData& operator=( TabularData&& ) noexcept = default;

		/** @brief Get data channel identifiers */
		[[nodiscard]] inline const std::vector<DataChannelId>& dataChannelIds() const noexcept;

		/** @brief Get data sets */
		[[nodiscard]] inline const std::vector<TabularDataSet>& dataSets() const noexcept;

		/** @brief Get number of data sets */
		[[nodiscard]] inline std::size_t numberOfDataSets() const noexcept;

		/** @brief Get number of data channels */
		[[nodiscard]] inline std::size_t numberOfDataChannels() const noexcept;

		/** @brief Set data channel identifiers */
		inline void setDataChannelIds( std::vector<DataChannelId> dataChannelIds );

		/** @brief Set data sets */
		inline void setDataSets( std::vector<TabularDataSet> dataSets );

		/**
		 * @brief Validate tabular data structure
		 * @return Validation result indicating success or error messages
		 * @details Validates that:
		 *          - DataChannelIds list is not empty
		 *          - DataSets list is not empty
		 *          - Each DataSet has correct number of values
		 */
		[[nodiscard]] inline ValidateResult<> validate() const;

	private:
		std::vector<DataChannelId> m_dataChannelIds;
		std::vector<TabularDataSet> m_dataSets;
	};

	//=====================================================================
	// EventDataSet class
	//=====================================================================

	/**
	 * @brief Single event data point with timestamp and value
	 * @details Contains a time-stamped event with data channel identifier and optional quality
	 */
	class EventDataSet final
	{
	public:
		/** @brief Default constructor */
		EventDataSet() = delete;

		/**
		 * @brief Construct EventDataSet with required fields
		 * @param timeStamp Event timestamp (required)
		 * @param dataChannelId Data channel identifier (required)
		 * @param value Event value (required)
		 * @param quality Optional quality indicator
		 */
		EventDataSet(
			DateTimeOffset timeStamp,
			DataChannelId dataChannelId,
			std::string value,
			std::optional<std::string> quality = std::nullopt );

		/** @brief Copy constructor */
		EventDataSet( const EventDataSet& ) = default;

		/** @brief Move constructor */
		EventDataSet( EventDataSet&& ) noexcept = default;

		/** @brief Destructor */
		~EventDataSet() = default;

		/** @brief Copy assignment operator */
		EventDataSet& operator=( const EventDataSet& ) = default;

		/** @brief Move assignment operator */
		EventDataSet& operator=( EventDataSet&& ) noexcept = default;

		/** @brief Get timestamp */
		[[nodiscard]] inline const DateTimeOffset& timeStamp() const noexcept;

		/** @brief Get data channel identifier */
		[[nodiscard]] inline const DataChannelId& dataChannelId() const noexcept;

		/** @brief Get value */
		[[nodiscard]] inline const std::string& value() const noexcept;

		/** @brief Get optional quality */
		[[nodiscard]] inline const std::optional<std::string>& quality() const noexcept;

		/** @brief Set timestamp */
		inline void setTimeStamp( DateTimeOffset timeStamp );

		/** @brief Set data channel identifier */
		inline void setDataChannelId( DataChannelId dataChannelId );

		/** @brief Set value */
		inline void setValue( std::string value );

		/** @brief Set optional quality */
		inline void setQuality( std::optional<std::string> quality );

	private:
		DateTimeOffset m_timeStamp;
		DataChannelId m_dataChannelId;
		std::string m_value;
		std::optional<std::string> m_quality;
	};

	//=====================================================================
	// EventData class
	//=====================================================================

	/**
	 * @brief Event-based time series data
	 * @details Contains event data with:
	 *          - DataSet: optional list of event data sets
	 *          - NumberOfDataSet: computed from DataSet size
	 */
	class EventData final
	{
	public:
		/**
		 * @brief Construct EventData with optional data sets
		 * @param dataSet Optional list of event data sets
		 */
		explicit EventData( std::optional<std::vector<EventDataSet>> dataSet = std::nullopt );

		/** @brief Copy constructor */
		EventData( const EventData& ) = default;

		/** @brief Move constructor */
		EventData( EventData&& ) noexcept = default;

		/** @brief Destructor */
		~EventData() = default;

		/** @brief Copy assignment operator */
		EventData& operator=( const EventData& ) = default;

		/** @brief Move assignment operator */
		EventData& operator=( EventData&& ) noexcept = default;

		/** @brief Get optional data set list */
		[[nodiscard]] inline const std::optional<std::vector<EventDataSet>>& dataSet() const noexcept;

		/** @brief Get number of data sets */
		[[nodiscard]] inline std::size_t numberOfDataSet() const noexcept;

		/** @brief Set optional data set list */
		inline void setDataSet( std::optional<std::vector<EventDataSet>> dataSet );

	private:
		std::optional<std::vector<EventDataSet>> m_dataSet;
	};

	//=====================================================================
	// TimeSeriesData class
	//=====================================================================

	/**
	 * @brief Time series data with tabular and event data
	 * @details Contains time series data with:
	 *          - DataConfiguration: optional configuration reference
	 *          - TabularData: optional list of tabular data
	 *          - EventData: optional event data
	 *          - CustomDataKinds: optional extension data dictionary
	 */
	class TimeSeriesData final
	{
	public:
		/** @brief Default constructor */
		TimeSeriesData() = default;

		/**
		 * @brief Construct TimeSeriesData with all fields
		 * @param dataConfiguration Optional data configuration reference
		 * @param tabularData Optional list of tabular data
		 * @param eventData Optional event data
		 * @param customDataKinds Optional custom data kinds dictionary
		 */
		TimeSeriesData(
			std::optional<ConfigurationReference> dataConfiguration = std::nullopt,
			std::optional<std::vector<TabularData>> tabularData = std::nullopt,
			std::optional<EventData> eventData = std::nullopt,
			std::optional<CustomData> customDataKinds = std::nullopt );

		/** @brief Copy constructor */
		TimeSeriesData( const TimeSeriesData& ) = default;

		/** @brief Move constructor */
		TimeSeriesData( TimeSeriesData&& ) noexcept = default;

		/** @brief Destructor */
		~TimeSeriesData() = default;

		/** @brief Copy assignment operator */
		TimeSeriesData& operator=( const TimeSeriesData& ) = default;

		/** @brief Move assignment operator */
		TimeSeriesData& operator=( TimeSeriesData&& ) noexcept = default;

		/** @brief Get optional data configuration */
		[[nodiscard]] inline const std::optional<ConfigurationReference>& dataConfiguration() const noexcept;

		/** @brief Get optional tabular data list */
		[[nodiscard]] inline const std::optional<std::vector<TabularData>>& tabularData() const noexcept;

		/** @brief Get optional event data */
		[[nodiscard]] inline const std::optional<EventData>& eventData() const noexcept;

		/** @brief Get optional custom data kinds */
		[[nodiscard]] inline const std::optional<CustomData>& customDataKinds() const noexcept;

		/** @brief Set optional data configuration */
		inline void setDataConfiguration( std::optional<ConfigurationReference> dataConfiguration );

		/** @brief Set optional tabular data list */
		inline void setTabularData( std::optional<std::vector<TabularData>> tabularData );

		/** @brief Set optional event data */
		inline void setEventData( std::optional<EventData> eventData );

		/** @brief Set optional custom data kinds */
		inline void setCustomDataKinds( std::optional<CustomData> customDataKinds );

		/**
		 * @brief Validate time series data against data channel list
		 * @param dcPackage Data channel list package for validation
		 * @param onTabularData Validation callback for tabular data points
		 * @param onEventData Validation callback for event data points
		 * @return Validation result indicating success or error messages
		 * @details Validates that:
		 *          - DataConfiguration matches DataChannelList if present
		 *          - At least one of TabularData or EventData contains data
		 *          - All data channel identifiers exist in the data channel list
		 *          - All values conform to their data channel format specifications
		 */
		[[nodiscard]] ValidateResult<> validate(
			const datachannel::DataChannelListPackage& dcPackage,
			std::function<ValidateResult<>( const DateTimeOffset&, const datachannel::DataChannel&, const Value&, const std::optional<std::string>& )> onTabularData,
			std::function<ValidateResult<>( const DateTimeOffset&, const datachannel::DataChannel&, const Value&, const std::optional<std::string>& )> onEventData ) const;

	private:
		std::optional<ConfigurationReference> m_dataConfiguration;
		std::optional<std::vector<TabularData>> m_tabularData;
		std::optional<EventData> m_eventData;
		std::optional<CustomData> m_customDataKinds;
	};

	//=====================================================================
	// Package class
	//=====================================================================

	/**
	 * @brief Package containing header and time series data list
	 * @details Combines optional metadata header with time series data collection
	 */
	class Package final
	{
	public:
		/** @brief Default constructor */
		Package() = delete;

		/**
		 * @brief Construct Package with header and time series data
		 * @param header Optional package header
		 * @param timeSeriesData Time series data list (required)
		 */
		Package(
			std::optional<Header> header,
			std::vector<TimeSeriesData> timeSeriesData );

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

		/** @brief Get optional header */
		[[nodiscard]] inline const std::optional<Header>& header() const noexcept;

		/** @brief Get time series data list */
		[[nodiscard]] inline const std::vector<TimeSeriesData>& timeSeriesData() const noexcept;

		/** @brief Set optional header */
		inline void setHeader( std::optional<Header> header );

		/** @brief Set time series data list */
		inline void setTimeSeriesData( std::vector<TimeSeriesData> timeSeriesData );

	private:
		std::optional<Header> m_header;
		std::vector<TimeSeriesData> m_timeSeriesData;
	};

	//=====================================================================
	// TimeSeriesDataPackage class
	//=====================================================================

	/**
	 * @brief Top-level time series data package wrapper
	 * @details Wraps a Package for ISO 19848 time series data transmission
	 */
	class TimeSeriesDataPackage final
	{
	public:
		/** @brief Default constructor */
		TimeSeriesDataPackage() = delete;

		/**
		 * @brief Construct TimeSeriesDataPackage with package
		 * @param package Package containing header and time series data (required)
		 */
		explicit TimeSeriesDataPackage( Package package );

		/** @brief Copy constructor */
		TimeSeriesDataPackage( const TimeSeriesDataPackage& ) = default;

		/** @brief Move constructor */
		TimeSeriesDataPackage( TimeSeriesDataPackage&& ) noexcept = default;

		/** @brief Destructor */
		~TimeSeriesDataPackage() = default;

		/** @brief Copy assignment operator */
		TimeSeriesDataPackage& operator=( const TimeSeriesDataPackage& ) = default;

		/** @brief Move assignment operator */
		TimeSeriesDataPackage& operator=( TimeSeriesDataPackage&& ) noexcept = default;

		/** @brief Get package */
		[[nodiscard]] inline const Package& package() const noexcept;

		/** @brief Set package */
		inline void setPackage( Package package );

	private:
		Package m_package;
	};
} // namespace dnv::vista::sdk::transport::timeseries

#include "dnv/vista/sdk/detail/transport/timeseries/TimeSeriesData.inl"
