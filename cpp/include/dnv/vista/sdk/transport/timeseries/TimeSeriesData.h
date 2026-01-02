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
    // TimeSpan class - Table 25
    //=====================================================================

    /**
     * @brief Time span with start and end timestamps (ISO 19848 TimeSpan)
     * @details Defines a time interval for time series data with explicit start and end points.
     *          ISO 19848:2024 Table 25: TimeSpan contains Start (time stamp of the oldest Data Set)
     *          and End (time stamp of the newest Data Set), both Mandatory.
     */
    class TimeSpan final
    {
    public:
        /** @brief Default constructor */
        TimeSpan() = delete;

        /**
         * @brief Construct TimeSpan with start and end timestamps
         * @param start Time stamp of the oldest Data Set (Mandatory, ISO 19848 Table 25, Start element)
         * @param end Time stamp of the newest Data Set (Mandatory, ISO 19848 Table 25, End element)
         */
        TimeSpan( DateTimeOffset start, DateTimeOffset end );

        /** @brief Copy constructor */
        TimeSpan( const TimeSpan& ) = default;

        /** @brief Move constructor */
        TimeSpan( TimeSpan&& ) noexcept = default;

        /** @brief Destructor */
        ~TimeSpan() = default;

        /**
         * @brief Copy assignment operator
         * @return Reference to this TimeSpan
         */
        TimeSpan& operator=( const TimeSpan& ) = default;

        /**
         * @brief Move assignment operator
         * @return Reference to this TimeSpan
         */
        TimeSpan& operator=( TimeSpan&& ) noexcept = default;

        /**
         * @brief Get start timestamp (time stamp of the oldest Data Set)
         * @return Start timestamp (ISO 19848 Table 25, Start element)
         */
        [[nodiscard]] inline const DateTimeOffset& start() const noexcept;

        /**
         * @brief Get end timestamp (time stamp of the newest Data Set)
         * @return End timestamp (ISO 19848 Table 25, End element)
         */
        [[nodiscard]] inline const DateTimeOffset& end() const noexcept;

        /**
         * @brief Set start timestamp (time stamp of the oldest Data Set)
         * @param start Start timestamp (ISO 19848 Table 25, Start element)
         */
        inline void setStart( DateTimeOffset start );

        /**
         * @brief Set end timestamp (time stamp of the newest Data Set)
         * @param end End timestamp (ISO 19848 Table 25, End element)
         */
        inline void setEnd( DateTimeOffset end );

    private:
        DateTimeOffset m_start;
        DateTimeOffset m_end;
    };

    //=====================================================================
    // ConfigurationReference class - Table 26
    //=====================================================================

    /**
     * @brief Configuration reference with identifier and timestamp (ISO 19848 ConfigurationReference)
     * @details ISO 19848:2024 Table 26: ConfigurationReference structure
     *          - ID: Mandatory, String - Identifier of the configuration
     *          - TimeStamp: Mandatory, DateTime - Modified date and time of the configuration
     */
    class ConfigurationReference final
    {
    public:
        /** @brief Default constructor */
        ConfigurationReference() = delete;

        /**
         * @brief Construct ConfigurationReference with required fields
         * @param id Identifier of the configuration (Mandatory, ISO 19848 Table 26)
         * @param timeStamp Modified date and time of the configuration (Mandatory, ISO 19848 Table 26)
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
         * @return Identifier of the configuration (ISO 19848 Table 26, ID element)
         */
        [[nodiscard]] inline const std::string& id() const noexcept;

        /**
         * @brief Get timestamp
         * @return Modified date and time of the configuration (ISO 19848 Table 26, TimeStamp element)
         */
        [[nodiscard]] inline const DateTimeOffset& timeStamp() const noexcept;

        /**
         * @brief Set configuration identifier
         * @param id Identifier of the configuration (ISO 19848 Table 26, ID element)
         */
        inline void setId( std::string id );

        /**
         * @brief Set timestamp
         * @param timeStamp Modified date and time of the configuration (ISO 19848 Table 26, TimeStamp element)
         */
        inline void setTimeStamp( DateTimeOffset timeStamp );

    private:
        std::string m_id;
        DateTimeOffset m_timeStamp;
    };

    //=====================================================================
    // Header class - Table 24
    //=====================================================================

    /**
     * @brief Time series package header with metadata (ISO 19848 Header)
     * @details ISO 19848:2024 Table 24: Header structure
     *          - ShipID: Mandatory, String - IMO number, HIN, etc.
     *          - TimeSpan: Optional, TimeSpan (Table 25)
     *          - DateCreated: Optional, DateTime - Date when Package is created
     *          - DateModified: Optional, DateTime - Date when Package is modified
     *          - Author: Optional, String - Author of data
     *          - SystemConfiguration: Optional, ConfigurationReference[] (Table 26) - Reference to the configuration of
     * systems, such as OS, RDBMS, etc.
     */
    class Header final
    {
    public:
        /** @brief Default constructor */
        Header() = delete;

        /**
         * @brief Construct Header with required fields
         * @param shipId Ship identifier (Mandatory, ISO 19848 Table 24, ShipID element - IMO number, HIN, etc.)
         * @param timeSpan Optional time span (ISO 19848 Table 24, TimeSpan element)
         * @param dateCreated Optional creation date (ISO 19848 Table 24, DateCreated element - Date when Package is
         * created)
         * @param dateModified Optional modification date (ISO 19848 Table 24, DateModified element - Date when Package
         * is modified)
         * @param author Optional author (ISO 19848 Table 24, Author element - Author of data)
         * @param systemConfiguration Optional system configuration list (ISO 19848 Table 24, SystemConfiguration
         * element)
         * @param customHeaders Optional custom headers dictionary (extension)
         */
        Header(
            ShipId shipId,
            std::optional<TimeSpan> timeSpan = std::nullopt,
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
         * @return Ship identifier (ISO 19848 Table 24, ShipID element - IMO number, HIN, etc.)
         */
        [[nodiscard]] inline const ShipId& shipId() const noexcept;

        /**
         * @brief Get optional time span
         * @return Optional time span (ISO 19848 Table 24, TimeSpan element)
         */
        [[nodiscard]] inline const std::optional<TimeSpan>& timeSpan() const noexcept;

        /**
         * @brief Get optional creation date
         * @return Optional creation timestamp (ISO 19848 Table 24, DateCreated element - Date when Package is created)
         */
        [[nodiscard]] inline const std::optional<DateTimeOffset>& dateCreated() const noexcept;

        /**
         * @brief Get optional modification date
         * @return Optional modification timestamp (ISO 19848 Table 24, DateModified element - Date when Package is
         * modified)
         */
        [[nodiscard]] inline const std::optional<DateTimeOffset>& dateModified() const noexcept;

        /**
         * @brief Get optional author
         * @return Optional author string (ISO 19848 Table 24, Author element - Author of data)
         */
        [[nodiscard]] inline const std::optional<std::string>& author() const noexcept;

        /**
         * @brief Get optional system configuration list
         * @return Optional system configuration list (ISO 19848 Table 24, SystemConfiguration element)
         */
        [[nodiscard]] inline const std::optional<std::vector<ConfigurationReference>>& systemConfiguration()
            const noexcept;

        /**
         * @brief Get optional custom headers
         * @return Optional custom headers dictionary (extension)
         */
        [[nodiscard]] inline const std::optional<CustomHeaders>& customHeaders() const noexcept;

        /**
         * @brief Set ship identifier
         * @param shipId Ship identifier (ISO 19848 Table 24, ShipID element)
         */
        inline void setShipId( ShipId shipId );

        /**
         * @brief Set optional time span
         * @param timeSpan Optional time span (ISO 19848 Table 24, TimeSpan element)
         */
        inline void setTimeSpan( std::optional<TimeSpan> timeSpan );

        /**
         * @brief Set optional creation date
         * @param dateCreated Optional creation timestamp (ISO 19848 Table 24, DateCreated element)
         */
        inline void setDateCreated( std::optional<DateTimeOffset> dateCreated );

        /**
         * @brief Set optional modification date
         * @param dateModified Optional modification timestamp (ISO 19848 Table 24, DateModified element)
         */
        inline void setDateModified( std::optional<DateTimeOffset> dateModified );

        /**
         * @brief Set optional author
         * @param author Optional author string (ISO 19848 Table 24, Author element)
         */
        inline void setAuthor( std::optional<std::string> author );

        /**
         * @brief Set optional system configuration list
         * @param systemConfiguration Optional system configuration list (ISO 19848 Table 24, SystemConfiguration
         * element)
         */
        inline void setSystemConfiguration( std::optional<std::vector<ConfigurationReference>> systemConfiguration );

        /**
         * @brief Set optional custom headers
         * @param customHeaders Optional custom headers dictionary (extension)
         */
        inline void setCustomHeaders( std::optional<CustomHeaders> customHeaders );

    private:
        ShipId m_shipId;
        std::optional<TimeSpan> m_timeSpan;
        std::optional<DateTimeOffset> m_dateCreated;
        std::optional<DateTimeOffset> m_dateModified;
        std::optional<std::string> m_author;
        std::optional<std::vector<ConfigurationReference>> m_systemConfiguration;
        std::optional<CustomHeaders> m_customHeaders;
    };

    //=====================================================================
    // TabularDataSet class - Table 30
    //=====================================================================

    /**
     * @brief Single tabular data set with timestamp and values (ISO 19848 DataSet_Tabular)
     * @details ISO 19848:2024 Table 30: DataSet_Tabular structure
     *          - TimeStamp: Mandatory, DateTime - Time of measurement
     *          - Value: Mandatory, String[] - Array of measurement value of each data channel at TimeStamp.
     *            Order of Value elements shall be same as Table 28, TabularData/DataChannelID.
     *            If the order of Value elements cannot be specified, reference number shall be added.
     *          - Quality: Optional, String - Data quality of the Value above
     */
    class TabularDataSet final
    {
    public:
        /** @brief Default constructor */
        TabularDataSet() = delete;

        /**
         * @brief Construct TabularDataSet with required fields
         * @param timeStamp Timestamp for this data set (Mandatory, ISO 19848 Table 30, TimeStamp element - Time of
         * measurement)
         * @param value List of string values (Mandatory, ISO 19848 Table 30, Value element - Array of measurement value
         * of each data channel at TimeStamp)
         * @param quality Optional quality indicators (ISO 19848 Table 30, Quality element - Data quality of the Value
         * above)
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
         * @return Timestamp for this data set (ISO 19848 Table 30, TimeStamp element - Time of measurement)
         */
        [[nodiscard]] inline const DateTimeOffset& timeStamp() const noexcept;

        /**
         * @brief Get value list
         * @return List of string values (ISO 19848 Table 30, Value element - Array of measurement values)
         */
        [[nodiscard]] inline const std::vector<std::string>& value() const noexcept;

        /**
         * @brief Get optional quality list
         * @return Optional quality indicators (ISO 19848 Table 30, Quality element - Data quality of the Value above)
         */
        [[nodiscard]] inline const std::optional<std::vector<std::string>>& quality() const noexcept;

        /**
         * @brief Set timestamp
         * @param timeStamp Timestamp for this data set (ISO 19848 Table 30, TimeStamp element)
         */
        inline void setTimeStamp( DateTimeOffset timeStamp );

        /**
         * @brief Set value list
         * @param value List of string values (ISO 19848 Table 30, Value element)
         */
        inline void setValue( std::vector<std::string> value );

        /**
         * @brief Set optional quality list
         * @param quality Optional quality indicators (ISO 19848 Table 30, Quality element)
         */
        inline void setQuality( std::optional<std::vector<std::string>> quality );

    private:
        DateTimeOffset m_timeStamp;
        std::vector<std::string> m_value;
        std::optional<std::vector<std::string>> m_quality;
    };

    //=====================================================================
    // TabularData class - Table 28
    //=====================================================================

    /**
     * @brief Tabular time series data with data channel mapping (ISO 19848 TabularData)
     * @details ISO 19848:2024 Table 28: TabularData structure
     *          - NumberOfDataSet: Optional, NonNegativeInteger - Number of DataSet in the TabularData element
     *          - NumberOfDataChannel: Optional, NonNegativeInteger - Number of DataChannel in the TabularData element
     *          - DataChannelID: Optional, String[] - Array of one of the Data Channel ID in 5.1.
     *            Order of DataChannelID elements shall be same as Table 28 TabularData/Value.
     *            If the order of DataChannelID elements cannot be specified, reference number shall be added.
     *            Further, if the order of DataChannelID elements can be fixed and never changed, these elements may be
     * omitted.
     *          - DataSet: Optional, DataSet_Tabular[] (Table 30) - See Clause 6
     */
    class TabularData final
    {
    public:
        /** @brief Default constructor */
        TabularData() = delete;

        /**
         * @brief Construct TabularData with required fields
         * @param dataChannelIds List of data channel identifiers (ISO 19848 Table 28, DataChannelID element)
         * @param dataSets List of data sets (ISO 19848 Table 28, DataSet element)
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
         * @return List of data channel identifiers (ISO 19848 Table 28, DataChannelID element)
         */
        [[nodiscard]] inline const std::vector<DataChannelId>& dataChannelIds() const noexcept;

        /**
         * @brief Get data sets
         * @return List of tabular data sets (ISO 19848 Table 28, DataSet element)
         */
        [[nodiscard]] inline const std::vector<TabularDataSet>& dataSets() const noexcept;

        /**
         * @brief Get number of data sets
         * @return Number of data sets in the list (ISO 19848 Table 28, NumberOfDataSet element)
         */
        [[nodiscard]] inline std::size_t numberOfDataSets() const noexcept;

        /**
         * @brief Get number of data channels
         * @return Number of data channels in the list (ISO 19848 Table 28, NumberOfDataChannel element)
         */
        [[nodiscard]] inline std::size_t numberOfDataChannels() const noexcept;

        /**
         * @brief Set data channel identifiers
         * @param dataChannelIds List of data channel identifiers (ISO 19848 Table 28, DataChannelID element)
         */
        inline void setDataChannelIds( std::vector<DataChannelId> dataChannelIds );

        /**
         * @brief Set data sets
         * @param dataSets List of tabular data sets (ISO 19848 Table 28, DataSet element)
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
    // EventDataSet class - Table 31
    //=====================================================================

    /**
     * @brief Single event data point with timestamp and value (ISO 19848 DataSet_Event)
     * @details ISO 19848:2024 Table 31: DataSet_Event structure
     *          - TimeStamp: Mandatory, DateTime - Time of measurement
     *          - DataChannelID: Mandatory, String - One of the Data Channel ID in 5.1
     *          - Value: Mandatory, String - Measurement value
     *          - Quality: Optional, String - Data quality of the Value above
     */
    class EventDataSet final
    {
    public:
        /** @brief Default constructor */
        EventDataSet() = delete;

        /**
         * @brief Construct EventDataSet with required fields
         * @param timeStamp Event timestamp (Mandatory, ISO 19848 Table 31, TimeStamp element - Time of measurement)
         * @param dataChannelId Data channel identifier (Mandatory, ISO 19848 Table 31, DataChannelID element - One of
         * the Data Channel ID in 5.1)
         * @param value Event value (Mandatory, ISO 19848 Table 31, Value element - Measurement value)
         * @param quality Optional quality indicator (ISO 19848 Table 31, Quality element - Data quality of the Value
         * above)
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
         * @return Event timestamp (ISO 19848 Table 31, TimeStamp element - Time of measurement)
         */
        [[nodiscard]] inline const DateTimeOffset& timeStamp() const noexcept;

        /**
         * @brief Get data channel identifier
         * @return Data channel identifier (ISO 19848 Table 31, DataChannelID element)
         */
        [[nodiscard]] inline const DataChannelId& dataChannelId() const noexcept;

        /**
         * @brief Get value
         * @return Event value string (ISO 19848 Table 31, Value element - Measurement value)
         */
        [[nodiscard]] inline const std::string& value() const noexcept;

        /**
         * @brief Get optional quality
         * @return Optional quality indicator (ISO 19848 Table 31, Quality element - Data quality of the Value above)
         */
        [[nodiscard]] inline const std::optional<std::string>& quality() const noexcept;

        /**
         * @brief Set timestamp
         * @param timeStamp Event timestamp (ISO 19848 Table 31, TimeStamp element)
         */
        inline void setTimeStamp( DateTimeOffset timeStamp );

        /**
         * @brief Set data channel identifier
         * @param dataChannelId Data channel identifier (ISO 19848 Table 31, DataChannelID element)
         */
        inline void setDataChannelId( DataChannelId dataChannelId );

        /**
         * @brief Set value
         * @param value Event value string (ISO 19848 Table 31, Value element)
         */
        inline void setValue( std::string value );

        /**
         * @brief Set optional quality
         * @param quality Optional quality indicator (ISO 19848 Table 31, Quality element)
         */
        inline void setQuality( std::optional<std::string> quality );

    private:
        DateTimeOffset m_timeStamp;
        DataChannelId m_dataChannelId;
        std::string m_value;
        std::optional<std::string> m_quality;
    };

    //=====================================================================
    // EventData class - Table 29
    //=====================================================================

    /**
     * @brief Event-based time series data (ISO 19848 EventData)
     * @details ISO 19848:2024 Table 29: EventData structure
     *          - NumberOfDataSet: Optional, NonNegativeInteger - Number of DataSet in the EventData element
     *          - DataSet: Optional, DataSet_Event[] (Table 31) - See 6.3 b)
     */
    class EventData final
    {
    public:
        /**
         * @brief Construct EventData with optional data sets
         * @param dataSet Optional list of event data sets (ISO 19848 Table 29, DataSet element)
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
         * @return Optional list of event data sets (ISO 19848 Table 29, DataSet element)
         */
        [[nodiscard]] inline const std::optional<std::vector<EventDataSet>>& dataSet() const noexcept;

        /**
         * @brief Get number of data sets
         * @return Number of event data sets (ISO 19848 Table 29, NumberOfDataSet element)
         */
        [[nodiscard]] inline std::size_t numberOfDataSet() const noexcept;

        /**
         * @brief Set optional data set list
         * @param dataSet Optional list of event data sets (ISO 19848 Table 29, DataSet element)
         */
        inline void setDataSet( std::optional<std::vector<EventDataSet>> dataSet );

    private:
        std::optional<std::vector<EventDataSet>> m_dataSet;
    };

    //=====================================================================
    // TimeSeriesData class - Table 27
    //=====================================================================

    /**
     * @brief Time series data with tabular and event data (ISO 19848 TimeSeriesData)
     * @details ISO 19848:2024 Table 27: TimeSeriesData structure
     *          - DataConfiguration: Optional, ConfigurationReference (Table 26) - Reference to the DataChannelList
     *          - TabularData: Optional, TabularData[] (Table 28) - See Table 28 and Clause 6
     *          - EventData: Optional, EventData (Table 29) - See Table 29 and Clause 6
     */
    class TimeSeriesData final
    {
    public:
        /** @brief Default constructor */
        TimeSeriesData() = default;

        /**
         * @brief Construct TimeSeriesData with all fields
         * @param dataConfiguration Optional data configuration reference (ISO 19848 Table 27, DataConfiguration
         * element)
         * @param tabularData Optional list of tabular data (ISO 19848 Table 27, TabularData element)
         * @param eventData Optional event data (ISO 19848 Table 27, EventData element)
         * @param customDataKinds Optional custom data kinds dictionary (extension)
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
         * @return Optional data configuration reference (ISO 19848 Table 27, DataConfiguration element)
         */
        [[nodiscard]] inline const std::optional<ConfigurationReference>& dataConfiguration() const noexcept;

        /**
         * @brief Get optional tabular data list
         * @return Optional list of tabular data (ISO 19848 Table 27, TabularData element)
         */
        [[nodiscard]] inline const std::optional<std::vector<TabularData>>& tabularData() const noexcept;

        /**
         * @brief Get optional event data
         * @return Optional event data (ISO 19848 Table 27, EventData element)
         */
        [[nodiscard]] inline const std::optional<EventData>& eventData() const noexcept;

        /**
         * @brief Get optional custom data kinds
         * @return Optional custom data kinds dictionary (extension)
         */
        [[nodiscard]] inline const std::optional<CustomData>& customDataKinds() const noexcept;

        /**
         * @brief Set optional data configuration
         * @param dataConfiguration Optional data configuration reference (ISO 19848 Table 27, DataConfiguration
         * element)
         */
        inline void setDataConfiguration( std::optional<ConfigurationReference> dataConfiguration );

        /**
         * @brief Set optional tabular data list
         * @param tabularData Optional list of tabular data (ISO 19848 Table 27, TabularData element)
         */
        inline void setTabularData( std::optional<std::vector<TabularData>> tabularData );

        /**
         * @brief Set optional event data
         * @param eventData Optional event data (ISO 19848 Table 27, EventData element)
         */
        inline void setEventData( std::optional<EventData> eventData );

        /**
         * @brief Set optional custom data kinds
         * @param customDataKinds Optional custom data kinds dictionary (extension)
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
    // Package class - Table 23
    //=====================================================================

    /**
     * @brief Package containing header and time series data list (ISO 19848 Package)
     * @details ISO 19848:2024 Table 23: Package structure
     *          - Header: Optional, Header (Table 24) - For periodic data exchange, this element can be omitted to
     * reduce data size
     *          - TimeSeriesData: Mandatory, TimeSeriesData[] (Table 27) - TimeSeriesData are grouped by DataChannelList
     * editions
     */
    class Package final
    {
    public:
        /** @brief Default constructor */
        Package() = delete;

        /**
         * @brief Construct Package with header and time series data
         * @param header Optional package header (ISO 19848 Table 23, Header element)
         * @param timeSeriesData Time series data list (Mandatory, ISO 19848 Table 23, TimeSeriesData element)
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
         * @return Optional package header (ISO 19848 Table 23, Header element)
         */
        [[nodiscard]] inline const std::optional<Header>& header() const noexcept;

        /**
         * @brief Get time series data list
         * @return List of time series data (ISO 19848 Table 23, TimeSeriesData element)
         */
        [[nodiscard]] inline const std::vector<TimeSeriesData>& timeSeriesData() const noexcept;

        /**
         * @brief Set optional header
         * @param header Optional package header (ISO 19848 Table 23, Header element)
         */
        inline void setHeader( std::optional<Header> header );

        /**
         * @brief Set time series data list
         * @param timeSeriesData List of time series data (ISO 19848 Table 23, TimeSeriesData element)
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
     * @brief Top-level time series data package wrapper (ISO 19848 TimeSeriesDataPackage)
     * @details Wraps a Package (Table 23) for ISO 19848 time series data transmission
     */
    class TimeSeriesDataPackage final
    {
    public:
        /** @brief Default constructor */
        TimeSeriesDataPackage() = delete;

        /**
         * @brief Construct TimeSeriesDataPackage with package
         * @param package Package containing header and time series data (ISO 19848 Table 23)
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
         * @return Package containing header and time series data (ISO 19848 Table 23)
         */
        [[nodiscard]] inline const Package& package() const noexcept;

        /**
         * @brief Set package
         * @param package Package containing header and time series data (ISO 19848 Table 23)
         */
        inline void setPackage( Package package );

    private:
        Package m_package;
    };
} // namespace dnv::vista::sdk::transport::timeseries

#include "dnv/vista/sdk/detail/transport/timeseries/TimeSeriesData.inl"
