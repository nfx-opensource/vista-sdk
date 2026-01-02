/**
 * @file TimeSeriesData.h
 * @brief Time series data package for ISO 19848 compliance
 */

#pragma once

#include "dnv/vista/sdk/transport/timeseries/DataChannelId.h"
#include "dnv/vista/sdk/transport/ISO19848.h"
#include "dnv/vista/sdk/transport/Results.h"
#include "dnv/vista/sdk/transport/ShipId.h"

#include <nfx/Serialization.h>

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
    using CustomHeaders = nfx::serialization::json::SerializableDocument;

    /** @brief Alias for custom headers - supports arbitrary JSON */
    using CustomData = nfx::serialization::json::SerializableDocument;

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

        /**
         * @brief Copy assignment operator
         * @return Reference to this ConfigurationReference
         */
        ConfigurationReference& operator=( const ConfigurationReference& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this ConfigurationReference
         */
        ConfigurationReference& operator=( ConfigurationReference&& ) noexcept = default;

        /**
         * @brief Get configuration identifier
         * @return Configuration identifier string
         */
        [[nodiscard]] inline const std::string& id() const noexcept;

        /**
         * @brief Get timestamp
         * @return Configuration timestamp
         */
        [[nodiscard]] inline const DateTimeOffset& timeStamp() const noexcept;

        /**
         * @brief Set configuration identifier
         * @param id Configuration identifier string
         */
        inline void setId( std::string id );

        /**
         * @brief Set timestamp
         * @param timeStamp Configuration timestamp
         */
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

        /**
         * @brief Copy assignment operator
         * @return Reference to this TimeRange
         */
        TimeRange& operator=( const TimeRange& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this TimeRange
         */
        TimeRange& operator=( TimeRange&& ) noexcept = default;

        /**
         * @brief Get start timestamp
         * @return Start timestamp
         */
        [[nodiscard]] inline const DateTimeOffset& start() const noexcept;

        /**
         * @brief Get end timestamp
         * @return End timestamp
         */
        [[nodiscard]] inline const DateTimeOffset& end() const noexcept;

        /**
         * @brief Set start timestamp
         * @param start Start timestamp
         */
        inline void setStart( DateTimeOffset start );

        /**
         * @brief Set end timestamp
         * @param end End timestamp
         */
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

        /**
         * @brief Copy assignment operator
         * @return Reference to this Header
         */
        Header& operator=( const Header& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this Header
         */
        Header& operator=( Header&& ) noexcept = default;

        /**
         * @brief Get ship identifier
         * @return Ship identifier
         */
        [[nodiscard]] inline const ShipId& shipId() const noexcept;

        /**
         * @brief Get optional time range
         * @return Optional time range
         */
        [[nodiscard]] inline const std::optional<TimeRange>& timeRange() const noexcept;

        /**
         * @brief Get optional creation date
         * @return Optional creation timestamp
         */
        [[nodiscard]] inline const std::optional<DateTimeOffset>& dateCreated() const noexcept;

        /**
         * @brief Get optional modification date
         * @return Optional modification timestamp
         */
        [[nodiscard]] inline const std::optional<DateTimeOffset>& dateModified() const noexcept;

        /**
         * @brief Get optional author
         * @return Optional author string
         */
        [[nodiscard]] inline const std::optional<std::string>& author() const noexcept;

        /**
         * @brief Get optional system configuration list
         * @return Optional system configuration list
         */
        [[nodiscard]] inline const std::optional<std::vector<ConfigurationReference>>& systemConfiguration()
            const noexcept;

        /**
         * @brief Get optional custom headers
         * @return Optional custom headers dictionary
         */
        [[nodiscard]] inline const std::optional<CustomHeaders>& customHeaders() const noexcept;

        /**
         * @brief Set ship identifier
         * @param shipId Ship identifier
         */
        inline void setShipId( ShipId shipId );

        /**
         * @brief Set optional time range
         * @param timeRange Optional time range
         */
        inline void setTimeRange( std::optional<TimeRange> timeRange );

        /**
         * @brief Set optional creation date
         * @param dateCreated Optional creation timestamp
         */
        inline void setDateCreated( std::optional<DateTimeOffset> dateCreated );

        /**
         * @brief Set optional modification date
         * @param dateModified Optional modification timestamp
         */
        inline void setDateModified( std::optional<DateTimeOffset> dateModified );

        /**
         * @brief Set optional author
         * @param author Optional author string
         */
        inline void setAuthor( std::optional<std::string> author );

        /**
         * @brief Set optional system configuration list
         * @param systemConfiguration Optional system configuration list
         */
        inline void setSystemConfiguration( std::optional<std::vector<ConfigurationReference>> systemConfiguration );

        /**
         * @brief Set optional custom headers
         * @param customHeaders Optional custom headers dictionary
         */
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

        /**
         * @brief Copy assignment operator
         * @return Reference to this TabularDataSet
         */
        TabularDataSet& operator=( const TabularDataSet& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this TabularDataSet
         */
        TabularDataSet& operator=( TabularDataSet&& ) noexcept = default;

        /**
         * @brief Get timestamp
         * @return Timestamp for this data set
         */
        [[nodiscard]] inline const DateTimeOffset& timeStamp() const noexcept;

        /**
         * @brief Get value list
         * @return List of string values
         */
        [[nodiscard]] inline const std::vector<std::string>& value() const noexcept;

        /**
         * @brief Get optional quality list
         * @return Optional quality indicators
         */
        [[nodiscard]] inline const std::optional<std::vector<std::string>>& quality() const noexcept;

        /**
         * @brief Set timestamp
         * @param timeStamp Timestamp for this data set
         */
        inline void setTimeStamp( DateTimeOffset timeStamp );

        /**
         * @brief Set value list
         * @param value List of string values
         */
        inline void setValue( std::vector<std::string> value );

        /**
         * @brief Set optional quality list
         * @param quality Optional quality indicators
         */
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
        TabularData( std::vector<DataChannelId> dataChannelIds, std::vector<TabularDataSet> dataSets );

        /** @brief Copy constructor */
        TabularData( const TabularData& ) = default;

        /** @brief Move constructor */
        TabularData( TabularData&& ) noexcept = default;

        /** @brief Destructor */
        ~TabularData() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this TabularData
         */
        TabularData& operator=( const TabularData& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this TabularData
         */
        TabularData& operator=( TabularData&& ) noexcept = default;

        /**
         * @brief Get data channel identifiers
         * @return List of data channel identifiers
         */
        [[nodiscard]] inline const std::vector<DataChannelId>& dataChannelIds() const noexcept;

        /**
         * @brief Get data sets
         * @return List of tabular data sets
         */
        [[nodiscard]] inline const std::vector<TabularDataSet>& dataSets() const noexcept;

        /**
         * @brief Get number of data sets
         * @return Number of data sets in the list
         */
        [[nodiscard]] inline std::size_t numberOfDataSets() const noexcept;

        /**
         * @brief Get number of data channels
         * @return Number of data channels in the list
         */
        [[nodiscard]] inline std::size_t numberOfDataChannels() const noexcept;

        /**
         * @brief Set data channel identifiers
         * @param dataChannelIds List of data channel identifiers
         */
        inline void setDataChannelIds( std::vector<DataChannelId> dataChannelIds );

        /**
         * @brief Set data sets
         * @param dataSets List of tabular data sets
         */
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

        /**
         * @brief Copy assignment operator
         * @return Reference to this EventDataSet
         */
        EventDataSet& operator=( const EventDataSet& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this EventDataSet
         */
        EventDataSet& operator=( EventDataSet&& ) noexcept = default;

        /**
         * @brief Get timestamp
         * @return Event timestamp
         */
        [[nodiscard]] inline const DateTimeOffset& timeStamp() const noexcept;

        /**
         * @brief Get data channel identifier
         * @return Data channel identifier
         */
        [[nodiscard]] inline const DataChannelId& dataChannelId() const noexcept;

        /**
         * @brief Get value
         * @return Event value string
         */
        [[nodiscard]] inline const std::string& value() const noexcept;

        /**
         * @brief Get optional quality
         * @return Optional quality indicator
         */
        [[nodiscard]] inline const std::optional<std::string>& quality() const noexcept;

        /**
         * @brief Set timestamp
         * @param timeStamp Event timestamp
         */
        inline void setTimeStamp( DateTimeOffset timeStamp );

        /**
         * @brief Set data channel identifier
         * @param dataChannelId Data channel identifier
         */
        inline void setDataChannelId( DataChannelId dataChannelId );

        /**
         * @brief Set value
         * @param value Event value string
         */
        inline void setValue( std::string value );

        /**
         * @brief Set optional quality
         * @param quality Optional quality indicator
         */
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

        /**
         * @brief Copy assignment operator
         * @return Reference to this EventData
         */
        EventData& operator=( const EventData& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this EventData
         */
        EventData& operator=( EventData&& ) noexcept = default;

        /**
         * @brief Get optional data set list
         * @return Optional list of event data sets
         */
        [[nodiscard]] inline const std::optional<std::vector<EventDataSet>>& dataSet() const noexcept;

        /**
         * @brief Get number of data sets
         * @return Number of event data sets
         */
        [[nodiscard]] inline std::size_t numberOfDataSet() const noexcept;

        /**
         * @brief Set optional data set list
         * @param dataSet Optional list of event data sets
         */
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

        /**
         * @brief Copy assignment operator
         * @return Reference to this TimeSeriesData
         */
        TimeSeriesData& operator=( const TimeSeriesData& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this TimeSeriesData
         */
        TimeSeriesData& operator=( TimeSeriesData&& ) noexcept = default;

        /**
         * @brief Get optional data configuration
         * @return Optional data configuration reference
         */
        [[nodiscard]] inline const std::optional<ConfigurationReference>& dataConfiguration() const noexcept;

        /**
         * @brief Get optional tabular data list
         * @return Optional list of tabular data
         */
        [[nodiscard]] inline const std::optional<std::vector<TabularData>>& tabularData() const noexcept;

        /**
         * @brief Get optional event data
         * @return Optional event data
         */
        [[nodiscard]] inline const std::optional<EventData>& eventData() const noexcept;

        /**
         * @brief Get optional custom data kinds
         * @return Optional custom data kinds dictionary
         */
        [[nodiscard]] inline const std::optional<CustomData>& customDataKinds() const noexcept;

        /**
         * @brief Set optional data configuration
         * @param dataConfiguration Optional data configuration reference
         */
        inline void setDataConfiguration( std::optional<ConfigurationReference> dataConfiguration );

        /**
         * @brief Set optional tabular data list
         * @param tabularData Optional list of tabular data
         */
        inline void setTabularData( std::optional<std::vector<TabularData>> tabularData );

        /**
         * @brief Set optional event data
         * @param eventData Optional event data
         */
        inline void setEventData( std::optional<EventData> eventData );

        /**
         * @brief Set optional custom data kinds
         * @param customDataKinds Optional custom data kinds dictionary
         */
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
            std::function<ValidateResult<>(
                const DateTimeOffset&,
                const datachannel::DataChannel&,
                const Value&,
                const std::optional<std::string>& )> onTabularData,
            std::function<ValidateResult<>(
                const DateTimeOffset&,
                const datachannel::DataChannel&,
                const Value&,
                const std::optional<std::string>& )> onEventData ) const;

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
        Package( std::optional<Header> header, std::vector<TimeSeriesData> timeSeriesData );

        /** @brief Copy constructor */
        Package( const Package& ) = default;

        /** @brief Move constructor */
        Package( Package&& ) noexcept = default;

        /** @brief Destructor */
        ~Package() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this Package
         */
        Package& operator=( const Package& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this Package
         */
        Package& operator=( Package&& ) noexcept = default;

        /**
         * @brief Get optional header
         * @return Optional package header
         */
        [[nodiscard]] inline const std::optional<Header>& header() const noexcept;

        /**
         * @brief Get time series data list
         * @return List of time series data
         */
        [[nodiscard]] inline const std::vector<TimeSeriesData>& timeSeriesData() const noexcept;

        /**
         * @brief Set optional header
         * @param header Optional package header
         */
        inline void setHeader( std::optional<Header> header );

        /**
         * @brief Set time series data list
         * @param timeSeriesData List of time series data
         */
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

        /**
         * @brief Copy assignment operator
         * @return Reference to this TimeSeriesDataPackage
         */
        TimeSeriesDataPackage& operator=( const TimeSeriesDataPackage& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this TimeSeriesDataPackage
         */
        TimeSeriesDataPackage& operator=( TimeSeriesDataPackage&& ) noexcept = default;

        /**
         * @brief Get package
         * @return Package containing header and time series data
         */
        [[nodiscard]] inline const Package& package() const noexcept;

        /**
         * @brief Set package
         * @param package Package containing header and time series data
         */
        inline void setPackage( Package package );

    private:
        Package m_package;
    };
} // namespace dnv::vista::sdk::transport::timeseries

#include "dnv/vista/sdk/detail/transport/timeseries/TimeSeriesData.inl"
