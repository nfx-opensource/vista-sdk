/**
 * @file TimeSeriesData.h
 * @brief Time Series Data Package implementation with comprehensive validation system
 *
 * @details
 * This file implements the **Time Series Data Package** infrastructure as specified in ISO-19848.
 * It provides a complete system for transporting, validating, and managing real-time sensor data
 * and events with high-performance characteristics optimized for streaming scenarios.
 *
 * ## Package Purpose:
 *
 * The Time Series Data Package is used for:
 * - **Runtime Data Transmission** : Streaming sensor measurements and events
 * - **Temporal Data Organization**: Structured time-stamped data with validation
 * - **Performance Optimization**  : Efficient data structures for high-frequency updates
 * - **Data Quality Management**   : Quality codes and validation for measurement reliability
 *
 * ## Core Data Structures:
 *
 * ### Tabular Data (Columnar Format)
 * - **TabularData**   : Collection with channel IDs as columns and timestamped rows
 * - **TabularDataSet**: Single row with timestamp, values, and optional quality codes
 * - **Use Case**      : Synchronized measurements from multiple sensors (e.g., temperature readings)
 * - **Performance**   : Efficient for bulk data transmission and analysis
 *
 * ### Event Data (Individual Records)
 * - **EventData**   : Collection of individual timestamped events
 * - **EventDataSet**: Single event with timestamp, channel ID, value, and quality
 * - **Use Case**    : Asynchronous events and alarms (e.g., door open, alarm triggered)
 * - **Flexibility** : Each event can reference different channels
 *
 * ### Temporal Framework
 * - **TimeRange**             : Validated time intervals with start/end boundaries
 * - **Header**                : Package metadata with ship info, versioning, author, timestamps
 * - **ConfigurationReference**: Links to data channel definitions and system configurations
 *
 * ## Data Flow Architecture:
 *
 * ```cpp
 * TimeSeriesDataPackage
 * └── Package
 *     ├── Header (ship ID, time range, system configuration versions, metadata)
 *     └── TimeSeriesData[]
 *         ├── TabularData[] (columnar sensor data)
 *         ├── EventData (individual events)
 *         └── DataConfiguration (reference to channel definitions)
 * ```
 *
 * ## Usage Examples:
 *
 * ### Tabular Data (Synchronized Sensors)
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Event Data (Asynchronous Events)
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ### Complete Package Assembly
 * ```cpp
 *
 * TODO
 *
 * ```
 *
 * ## Validation Framework:
 *
 * - **Structure Validation**   : Ensures data consistency (matching column counts, valid timestamps)
 * - **Channel Validation**     : Verifies channels exist in referenced DataChannelList package
 * - **Data Type Validation**   : Validates values against channel format specifications
 * - **Quality Code Validation**: Ensures quality indicators meet system requirements
 * - **Custom Validation**      : Extensible validation through delegate functions
 *
 * ## Performance Characteristics:
 *
 * This implementation is optimized for:
 * - **High Throughput**  : Efficient data structures for streaming scenarios
 * - **Memory Efficiency**: Minimal overhead for large datasets
 * - **Validation Speed** : Fast validation without blocking data flow
 * - **Scalability**      : Handles large numbers of channels and high update frequencies
 *
 * ## Design Philosophy:
 *
 * - **Streaming Focus**     : Optimized for real-time data transmission
 * - **Type Safety**         : Strong typing with comprehensive validation
 * - **Performance**         : Efficient data structures and minimal copying
 * - **Extensibility**       : Custom data kinds and validation delegates
 * - **Standards Compliance**: Full ISO-19848 conformance
 *
 * @note This implementation focuses on runtime data transmission and streaming.
 *       It uses the lightweight DataChannelId from the timeseries namespace.
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <nfx/Containers.h>
#include <nfx/DateTime.h>

#include "dnv/vista/sdk/config/config.h"
#include "dnv/vista/sdk/transport/datachannel/DataChannel.h"
#include "dnv/vista/sdk/transport/timeseries/DataChannelId.h"
#include "dnv/vista/sdk/transport/ShipId.h"
#include "dnv/vista/sdk/LocalId.h"

//=====================================================================
// Forward declarations
//=====================================================================

namespace dnv::vista::sdk
{
	class ValidateResult;

	namespace transport
	{
		class Value;
	}
} // namespace dnv::vista::sdk

namespace dnv::vista::sdk::transport::timeseries
{
	//=====================================================================
	// TimeRange class
	//=====================================================================

	/**
	 * @brief Time range with start and end timestamps
	 * @details Represents a time window or interval defined by start and end points.
	 *          Used to specify the temporal bounds for time series data collection.
	 *          Validates that start time is always before or equal to end time.
	 */
	class TimeRange final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		TimeRange() = delete;

		/**
		 * @brief Constructs TimeRange with start and end times
		 * @param start Start timestamp
		 * @param end End timestamp
		 * @throws std::invalid_argument If start > end
		 */
		inline TimeRange( nfx::time::DateTimeOffset start, nfx::time::DateTimeOffset end );

		/** @brief Copy constructor */
		TimeRange( const TimeRange& ) = default;

		/** @brief Move constructor */
		TimeRange( TimeRange&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~TimeRange() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The TimeRange to copy from
		 * @return Reference to this TimeRange
		 */
		TimeRange& operator=( const TimeRange& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The TimeRange to move from
		 * @return Reference to this TimeRange
		 */
		TimeRange& operator=( TimeRange&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get start timestamp
		 * @return Start time
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const nfx::time::DateTimeOffset& start() const noexcept;

		/**
		 * @brief Get end timestamp
		 * @return End time
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const nfx::time::DateTimeOffset& end() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set start timestamp with validation
		 * @param start New start time
		 * @throws std::invalid_argument If start > current end
		 */
		void setStart( nfx::time::DateTimeOffset start );

		/**
		 * @brief Set end timestamp with validation
		 * @param end New end time
		 * @throws std::invalid_argument If end < current start
		 */
		void setEnd( nfx::time::DateTimeOffset end );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		nfx::time::DateTimeOffset m_start;
		nfx::time::DateTimeOffset m_end;
	};

	//=====================================================================
	// ConfigurationReference class
	//=====================================================================

	/**
	 * @brief Configuration reference for time series data
	 */
	class ConfigurationReference final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		ConfigurationReference() = default;

		/**
		 * @brief Constructs ConfigurationReference with required parameters
		 * @param id Configuration identifier
		 * @param timeStamp Configuration timestamp
		 */
		inline ConfigurationReference( std::string_view id, nfx::time::DateTimeOffset timeStamp );

		/** @brief Copy constructor */
		ConfigurationReference( const ConfigurationReference& ) = default;

		/** @brief Move constructor */
		ConfigurationReference( ConfigurationReference&& ) noexcept = default;

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
		 * @return The configuration ID
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::string& id() const noexcept;

		/**
		 * @brief Get timestamp
		 * @return The configuration timestamp
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const nfx::time::DateTimeOffset& timeStamp() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set configuration ID
		 * @param id New configuration identifier
		 */
		inline void setId( std::string_view id );

		/**
		 * @brief Set timestamp
		 * @param timeStamp New configuration timestamp
		 */
		inline void setTimeStamp( nfx::time::DateTimeOffset timeStamp );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		std::string m_id;
		nfx::time::DateTimeOffset m_timeStamp;
	};

	//=====================================================================
	// TabularDataSet class
	//=====================================================================

	/**
	 * @brief Single tabular data set with timestamp, values, and quality
	 */
	class TabularDataSet final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		TabularDataSet() = default;

		/**
		 * @brief Constructs TabularDataSet with parameter list
		 * @param timeStamp Data timestamp
		 * @param value Data values
		 * @param quality Optional quality values
		 */
		inline TabularDataSet(
			nfx::time::DateTimeOffset timeStamp,
			std::vector<std::string> value = {},
			std::optional<std::vector<std::string>> quality = std::nullopt );

		/** @brief Copy constructor */
		TabularDataSet( const TabularDataSet& ) = default;

		/** @brief Move constructor */
		TabularDataSet( TabularDataSet&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~TabularDataSet() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The TabularDataSet to copy from
		 * @return Reference to this TabularDataSet
		 */
		TabularDataSet& operator=( const TabularDataSet& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The TabularDataSet to move from
		 * @return Reference to this TabularDataSet
		 */
		TabularDataSet& operator=( TabularDataSet&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get timestamp
		 * @return The data timestamp
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const nfx::time::DateTimeOffset& timeStamp() const noexcept;

		/**
		 * @brief Get values
		 * @return The data values
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::vector<std::string>& value() const noexcept;

		/**
		 * @brief Get quality values
		 * @return The quality values
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::optional<std::vector<std::string>>& quality() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set timestamp
		 * @param timeStamp New timestamp
		 */
		inline void setTimeStamp( nfx::time::DateTimeOffset timeStamp );

		/**
		 * @brief Set values
		 * @param value New data values
		 */
		inline void setValue( std::vector<std::string> value );

		/**
		 * @brief Set quality values
		 * @param quality New optional quality values
		 */
		inline void setQuality( std::optional<std::vector<std::string>> quality );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		nfx::time::DateTimeOffset m_timeStamp;
		std::vector<std::string> m_value;
		std::optional<std::vector<std::string>> m_quality;
	};

	//=====================================================================
	// EventDataSet class
	//=====================================================================

	/**
	 * @brief Single event data set with timestamp, channel ID, value, and quality
	 */
	class EventDataSet final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		EventDataSet() = delete;

		/**
		 * @brief Constructs EventDataSet with required fields
		 * @param timeStamp Event timestamp
		 * @param dataChannelId Channel identifier
		 * @param value Event value
		 * @param quality Optional quality value
		 */
		inline EventDataSet(
			nfx::time::DateTimeOffset timeStamp,
			DataChannelId dataChannelId,
			std::string value,
			std::optional<std::string> quality = std::nullopt );

		/** @brief Copy constructor */
		EventDataSet( const EventDataSet& ) = default;

		/** @brief Move constructor */
		EventDataSet( EventDataSet&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~EventDataSet() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The EventDataSet to copy from
		 * @return Reference to this EventDataSet
		 */
		EventDataSet& operator=( const EventDataSet& other ) = delete;

		/**
		 * @brief Move assignment operator
		 * @param other The EventDataSet to move from
		 * @return Reference to this EventDataSet
		 */
		EventDataSet& operator=( EventDataSet&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get timestamp
		 * @return The event timestamp
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const nfx::time::DateTimeOffset& timeStamp() const noexcept;

		/**
		 * @brief Get data channel ID
		 * @return The data channel ID
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const DataChannelId& dataChannelId() const noexcept;

		/**
		 * @brief Get value
		 * @return The event value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::string& value() const noexcept;

		/**
		 * @brief Get quality
		 * @return The quality value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::optional<std::string>& quality() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set timestamp
		 * @param timeStamp New timestamp
		 */
		inline void setTimeStamp( nfx::time::DateTimeOffset timeStamp );

		/**
		 * @brief Set data channel ID
		 * @param dataChannelId New channel identifier
		 */
		inline void setDataChannelId( DataChannelId dataChannelId );

		/**
		 * @brief Set value
		 * @param value New event value
		 */
		inline void setValue( std::string_view value );

		/**
		 * @brief Set quality
		 * @param quality New optional quality value
		 */
		inline void setQuality( std::optional<std::string_view> quality );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		nfx::time::DateTimeOffset m_timeStamp;
		DataChannelId m_dataChannelId;
		std::string m_value;
		std::optional<std::string> m_quality;
	};

	//=====================================================================
	// TabularData class
	//=====================================================================

	/**
	 * @brief Tabular data with channel IDs and data sets
	 */
	class TabularData final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		TabularData() = default;

		/**
		 * @brief Constructs TabularData with required fields
		 * @param dataChannelIds Channel identifiers
		 * @param dataSets Data sets
		 */
		inline TabularData(
			std::vector<DataChannelId> dataChannelIds = {},
			std::vector<TabularDataSet> dataSets = {} );

		/** @brief Copy constructor */
		TabularData( const TabularData& ) = default;

		/** @brief Move constructor */
		TabularData( TabularData&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~TabularData() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The TabularData to copy from
		 * @return Reference to this TabularData
		 */
		TabularData& operator=( const TabularData& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The TabularData to move from
		 * @return Reference to this TabularData
		 */
		TabularData& operator=( TabularData&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get data channel IDs
		 * @return The data channel IDs
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::vector<DataChannelId>& dataChannelIds() const noexcept;

		/**
		 * @brief Get data sets
		 * @return The data sets
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::vector<TabularDataSet>& dataSets() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set data channel IDs
		 * @param dataChannelIds New channel identifiers
		 */
		inline void setDataChannelIds( std::vector<DataChannelId> dataChannelIds );

		/**
		 * @brief Set data sets
		 * @param dataSets New data sets
		 */
		inline void setDataSets( std::vector<TabularDataSet> dataSets );

		//----------------------------------------------
		// Validation
		//----------------------------------------------

		/**
		 * @brief Static validation method
		 * @param table TabularData instance to validate
		 * @return Validation result
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] static ValidateResult Validate( const TabularData& table );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		std::vector<DataChannelId> m_dataChannelIds;
		std::vector<TabularDataSet> m_dataSets;
	};

	//=====================================================================
	// EventData class
	//=====================================================================

	/**
	 * @brief Event data collection
	 */
	class EventData final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Default constructor with optional capacity
		 * @param capacity Initial capacity for data set vector
		 */
		inline explicit EventData( size_t capacity = 0 );

		/**
		 * @brief Constructs EventData with data sets
		 * @param dataSet Event data sets vector
		 */
		inline explicit EventData( std::vector<EventDataSet> dataSet );

		/** @brief Copy constructor */
		EventData( const EventData& ) = default;

		/** @brief Move constructor */
		EventData( EventData&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~EventData() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The EventData to copy from
		 * @return Reference to this EventData
		 */
		EventData& operator=( const EventData& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The EventData to move from
		 * @return Reference to this EventData
		 */
		EventData& operator=( EventData&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get data set
		 * @return The event data sets
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::vector<EventDataSet>& dataSet() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set data set
		 * @param dataSet New event data sets
		 */
		inline void setDataSet( std::vector<EventDataSet> dataSet );

		//----------------------------------------------
		// Collection interface
		//----------------------------------------------

		/**
		 * @brief Add event data set
		 * @param dataSet Event data set to add
		 */
		inline void add( EventDataSet dataSet );

		/**
		 * @brief Clear all data sets
		 */
		inline void clear();

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		std::vector<EventDataSet> m_dataSet;
	};

	//=====================================================================
	// Header class
	//=====================================================================

	/**
	 * @brief Time series package header with metadata
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
		 * @brief Constructs Header with all fields
		 * @param shipId Ship identifier
		 * @param timeRange Optional time range for the data
		 * @param dateCreated Optional creation timestamp
		 * @param dateModified Optional modification timestamp
		 * @param author Optional author information
		 * @param systemConfiguration Optional system configuration references
		 * @param customHeaders Optional custom headers dictionary
		 */
		inline Header(
			ShipId shipId,
			std::optional<TimeRange> timeRange = std::nullopt,
			std::optional<nfx::time::DateTimeOffset> dateCreated = nfx::time::DateTimeOffset::utcNow(),
			std::optional<nfx::time::DateTimeOffset> dateModified = std::nullopt,
			std::optional<std::string> author = std::nullopt,
			std::optional<std::vector<ConfigurationReference>> systemConfiguration = std::nullopt,
			std::optional<nfx::containers::FastHashMap<std::string, Value>> customHeaders = std::nullopt );

		/** @brief Copy constructor */
		Header( const Header& ) = default;

		/** @brief Move constructor */
		Header( Header&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

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
		 * @return The ship ID
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const ShipId& shipId() const noexcept;

		/**
		 * @brief Get time range
		 * @return The time range
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::optional<TimeRange>& timeRange() const noexcept;

		/**
		 * @brief Get date created
		 * @return The creation date
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::optional<nfx::time::DateTimeOffset>& dateCreated() const noexcept;

		/**
		 * @brief Get date modified
		 * @return The modification date
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::optional<nfx::time::DateTimeOffset>& dateModified() const noexcept;

		/**
		 * @brief Get author
		 * @return The author information
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::optional<std::string>& author() const noexcept;

		/**
		 * @brief Get system configuration
		 * @return The system configuration references
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::optional<std::vector<ConfigurationReference>>& systemConfiguration() const noexcept;

		/**
		 * @brief Get custom headers
		 * @return The custom headers
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::optional<nfx::containers::FastHashMap<std::string, Value>>& customHeaders() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set ship ID
		 * @param shipId New ship identifier
		 */
		inline void setShipId( ShipId shipId );

		/**
		 * @brief Set time range
		 * @param timeRange New optional time range
		 */
		inline void setTimeRange( std::optional<TimeRange> timeRange );

		/**
		 * @brief Set date created
		 * @param dateCreated New optional creation timestamp
		 */
		inline void setDateCreated( std::optional<nfx::time::DateTimeOffset> dateCreated );

		/**
		 * @brief Set date modified
		 * @param dateModified New optional modification timestamp
		 */
		inline void setDateModified( std::optional<nfx::time::DateTimeOffset> dateModified );

		/**
		 * @brief Set author
		 * @param author New optional author string
		 */
		inline void setAuthor( std::optional<std::string_view> author );

		/**
		 * @brief Set system configuration
		 * @param systemConfiguration New optional system configuration
		 */
		inline void setSystemConfiguration( std::optional<std::vector<ConfigurationReference>> systemConfiguration );

		/**
		 * @brief Set custom headers
		 * @param customHeaders New optional custom headers
		 */
		inline void setCustomHeaders( std::optional<nfx::containers::FastHashMap<std::string, Value>> customHeaders );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		ShipId m_shipId;
		std::optional<TimeRange> m_timeRange;
		std::optional<nfx::time::DateTimeOffset> m_dateCreated;
		std::optional<nfx::time::DateTimeOffset> m_dateModified;
		std::optional<std::string> m_author;
		std::optional<std::vector<ConfigurationReference>> m_systemConfiguration;
		std::optional<nfx::containers::FastHashMap<std::string, Value>> m_customHeaders;
	};

	//=====================================================================
	// Validation delegate type
	//=====================================================================

	/**
	 * @brief Validation function type for data validation
	 */
	using ValidateData = std::function<ValidateResult(
		const nfx::time::DateTimeOffset& timeStamp,
		const DataChannel& dataChannel,
		const Value& value,
		const std::optional<std::string>& quality )>;

	//=====================================================================
	// TimeSeriesData class
	//=====================================================================

	/**
	 * @brief Time series data with validation capabilities
	 */
	class TimeSeriesData final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		TimeSeriesData() = delete;

		/**
		 * @brief Constructs TimeSeriesData with optional components
		 * @param dataConfiguration Optional data configuration reference
		 * @param tabularData Optional vector of tabular data sets
		 * @param eventData Optional event data collection
		 * @param customDataKinds Optional custom data kinds dictionary
		 */
		inline TimeSeriesData(
			std::optional<ConfigurationReference> dataConfiguration = std::nullopt,
			std::optional<std::vector<TabularData>> tabularData = std::nullopt,
			std::optional<EventData> eventData = std::nullopt,
			std::optional<nfx::containers::FastHashMap<std::string, Value>> customDataKinds = std::nullopt );

		/** @brief Copy constructor */
		TimeSeriesData( const TimeSeriesData& ) = default;

		/** @brief Move constructor */
		TimeSeriesData( TimeSeriesData&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~TimeSeriesData() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The TimeSeriesData to copy from
		 * @return Reference to this TimeSeriesData
		 */
		TimeSeriesData& operator=( const TimeSeriesData& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The TimeSeriesData to move from
		 * @return Reference to this TimeSeriesData
		 */
		TimeSeriesData& operator=( TimeSeriesData&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get data configuration
		 * @return The data configuration reference
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::optional<ConfigurationReference>& dataConfiguration() const noexcept;

		/**
		 * @brief Get tabular data
		 * @return The tabular data collections
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::optional<std::vector<TabularData>>& tabularData() const noexcept;

		/**
		 * @brief Get event data
		 * @return The event data collection
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::optional<EventData>& eventData() const noexcept;

		/**
		 * @brief Get custom data kinds
		 * @return The custom data kinds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::optional<nfx::containers::FastHashMap<std::string, Value>>& customDataKinds() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set data configuration
		 * @param dataConfiguration New optional data configuration
		 */
		inline void setDataConfiguration( std::optional<ConfigurationReference> dataConfiguration );

		/**
		 * @brief Set tabular data
		 * @param tabularData New optional tabular data
		 */
		inline void setTabularData( std::optional<std::vector<TabularData>> tabularData );

		/**
		 * @brief Set event data
		 * @param eventData New optional event data
		 */
		inline void setEventData( std::optional<EventData> eventData );

		/**
		 * @brief Set custom data kinds
		 * @param customDataKinds New optional custom data kinds
		 */
		inline void setCustomDataKinds( std::optional<nfx::containers::FastHashMap<std::string, Value>> customDataKinds );

		//----------------------------------------------
		// Validation
		//----------------------------------------------

		/**
		 * @brief Validate time series data against data channel package
		 * @param dcPackage Data channel list package for validation
		 * @param onTabularData Validation function for tabular data
		 * @param onEventData Validation function for event data
		 * @return Validation result
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] ValidateResult validate(
			const DataChannelListPackage& dcPackage,
			ValidateData onTabularData,
			ValidateData onEventData ) const;

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		std::optional<ConfigurationReference> m_dataConfiguration;
		std::optional<std::vector<TabularData>> m_tabularData;
		std::optional<EventData> m_eventData;
		std::optional<nfx::containers::FastHashMap<std::string, Value>> m_customDataKinds;
	};

	//=====================================================================
	// Package class
	//=====================================================================

	/**
	 * @brief Time series package containing header and data
	 */
	class Package final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		Package() = default;

		/**
		 * @brief Constructs Package with required components
		 * @param header Package header with metadata
		 * @param timeSeriesData Collection of time series data
		 */
		inline Package(
			std::optional<Header> header,
			std::vector<TimeSeriesData> timeSeriesData );

		/** @brief Copy constructor */
		Package( const Package& ) = default;

		/** @brief Move constructor */
		Package( Package&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

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
		 * @return The package header
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::optional<Header>& header() const noexcept;

		/**
		 * @brief Get time series data
		 * @return The time series data collections
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const std::vector<TimeSeriesData>& timeSeriesData() const noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set header
		 * @param header New optional package header
		 */
		inline void setHeader( std::optional<Header> header );

		/**
		 * @brief Set time series data
		 * @param timeSeriesData New time series data vector
		 */
		inline void setTimeSeriesData( std::vector<TimeSeriesData> timeSeriesData );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		std::optional<Header> m_header;
		std::vector<TimeSeriesData> m_timeSeriesData;
	};

	//=====================================================================
	// TimeSeriesDataPackage class
	//=====================================================================

	/**
	 * @brief Main time series data package container
	 * @details Top-level container for time series data transport
	 */
	class TimeSeriesDataPackage final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor */
		TimeSeriesDataPackage() = default;

		/**
		 * @brief Constructs TimeSeriesDataPackage with required package
		 * @param package Package containing header and time series data
		 */
		inline explicit TimeSeriesDataPackage( Package package );

		/** @brief Copy constructor */
		TimeSeriesDataPackage( const TimeSeriesDataPackage& ) = default;

		/** @brief Move constructor */
		TimeSeriesDataPackage( TimeSeriesDataPackage&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~TimeSeriesDataPackage() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The TimeSeriesDataPackage to copy from
		 * @return Reference to this TimeSeriesDataPackage
		 */
		TimeSeriesDataPackage& operator=( const TimeSeriesDataPackage& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param other The TimeSeriesDataPackage to move from
		 * @return Reference to this TimeSeriesDataPackage
		 */
		TimeSeriesDataPackage& operator=( TimeSeriesDataPackage&& other ) noexcept = default;

		//----------------------------------------------
		// Property access
		//----------------------------------------------

		/**
		 * @brief Get package
		 * @return The time series data package
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline const Package& package() const noexcept;

		/**
		 * @brief Get package (mutable)
		 * @return The time series data package
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] [[nodiscard]] inline Package& package() noexcept;

		//----------------------------------------------
		// Setters
		//----------------------------------------------

		/**
		 * @brief Set package
		 * @param package New package
		 */
		inline void setPackage( Package package );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		Package m_package;
	};
} // namespace dnv::vista::sdk::transport::timeseries

#include "dnv/vista/sdk/detail/transport/timeseries/TimeSeriesData.inl"
