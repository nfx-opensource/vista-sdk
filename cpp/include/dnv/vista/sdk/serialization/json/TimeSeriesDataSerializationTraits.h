/**
 * @file TimeSeriesDataSerializationTraits.h
 * @brief JSON serialization specialization for TimeSeriesData domain objects
 * @details Provides SerializationTraits specializations for Vista SDK TimeSeriesData
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/extensions/ContainersTraits.h>
#include <nfx/serialization/json/extensions/DatatypesTraits.h>
#include <nfx/serialization/json/extensions/DateTimeTraits.h>

#include "dnv/vista/sdk/transport/timeseries/TimeSeriesData.h"
#include "ISO19848Validation.h"

namespace nfx::serialization::json
{
    using namespace dnv::vista::sdk;
    using namespace dnv::vista::sdk::transport;

    /**
     * @brief SerializationTraits specialization for TimeSeriesDataPackage
     * @details Provides unified bidirectional JSON serialization for Vista SDK TimeSeriesData domain objects.
     *          - serialize(): High-performance streaming serialization using Builder API
     *          - fromDocument(): DOM-based deserialization
     */
    template <>
    struct SerializationTraits<timeseries::TimeSeriesDataPackage>
    {
        /**
         * @brief Serialize TimeSeriesDataPackage to JSON using Builder
         * @param pkg The package to serialize
         * @param builder The JSON Builder to write to
         */
        static void serialize( const timeseries::TimeSeriesDataPackage& pkg, nfx::json::Builder& builder )
        {
            const auto& package = pkg.package();

            builder.writeStartObject(); // Package root
            builder.writeKey( "Package" );
            builder.writeStartObject();

            // Header (optional)
            if( auto header = package.header() )
            {
                builder.writeKey( "Header" );
                serializeHeader( *header, builder );
            }

            // TimeSeriesData
            builder.writeKey( "TimeSeriesData" );
            serializeTimeSeriesDataList( package.timeSeriesData(), builder );

            builder.writeEndObject();
            builder.writeEndObject();
        }

        /**
         * @brief Factory deserialization: Construct TimeSeriesDataPackage directly from JSON document
         * @param doc The JSON document to deserialize from
         * @return Newly constructed TimeSeriesDataPackage
         */
        static timeseries::TimeSeriesDataPackage fromDocument( const nfx::json::Document& doc )
        {
            auto& packageObj = doc["Package"];

            // Header
            std::optional<timeseries::Header> header;

            auto& headerDocRef = packageObj["Header"];
            if( headerDocRef.isValid() )
            {
                auto& headerDoc = headerDocRef; // ShipId (required)
                auto shipIdStr = headerDoc["ShipID"].get<std::string>( "" );
                if( !shipIdStr.has_value() )
                {
                    throw std::runtime_error{ "Missing required ShipID in TimeSeriesData Header" };
                }

                auto shipIdOpt = ShipId::fromString( *shipIdStr );
                if( !shipIdOpt.has_value() )
                {
                    throw std::runtime_error{ "Invalid ShipID format: " + *shipIdStr };
                }

                // TimeSpan (optional)
                std::optional<timeseries::TimeSpan> timeSpan;
                auto& timeSpanRef = headerDoc["TimeSpan"];
                if( timeSpanRef.isValid() )
                {
                    auto& timeSpanObj = timeSpanRef;
                    auto startStr = timeSpanObj["Start"].get<std::string>( "" );
                    auto endStr = timeSpanObj["End"].get<std::string>( "" );
                    if( startStr.has_value() && endStr.has_value() )
                    {
                        if( !isValidIso19848DateTimeOffset( *startStr ) )
                        {
                            throw std::runtime_error{ "Invalid ISO 19848 DateTimeOffset format in TimeSpan/Start: " +
                                                      *startStr };
                        }
                        if( !isValidIso19848DateTimeOffset( *endStr ) )
                        {
                            throw std::runtime_error{ "Invalid ISO 19848 DateTimeOffset format in TimeSpan/End: " +
                                                      *endStr };
                        }
                        DateTimeOffset start;
                        DateTimeOffset end;
                        if( DateTimeOffset::fromString( *startStr, start ) &&
                            DateTimeOffset::fromString( *endStr, end ) )
                        {
                            timeSpan = timeseries::TimeSpan{ start, end };
                        }
                    }
                }

                // DateCreated (optional)
                std::optional<DateTimeOffset> dateCreated;
                if( auto dateCreatedStr = headerDoc["DateCreated"].get<std::string>( "" ) )
                {
                    if( !isValidIso19848DateTimeOffset( *dateCreatedStr ) )
                    {
                        throw std::runtime_error{ "Invalid ISO 19848 DateTimeOffset format in DateCreated: " +
                                                  *dateCreatedStr };
                    }
                    DateTimeOffset dt;
                    if( DateTimeOffset::fromString( *dateCreatedStr, dt ) )
                    {
                        dateCreated = dt;
                    }
                }

                // DateModified (optional)
                std::optional<DateTimeOffset> dateModified;
                if( auto dateModifiedStr = headerDoc["DateModified"].get<std::string>( "" ) )
                {
                    if( !isValidIso19848DateTimeOffset( *dateModifiedStr ) )
                    {
                        throw std::runtime_error{ "Invalid ISO 19848 DateTimeOffset format in DateModified: " +
                                                  *dateModifiedStr };
                    }
                    DateTimeOffset dt;
                    if( DateTimeOffset::fromString( *dateModifiedStr, dt ) )
                    {
                        dateModified = dt;
                    }
                }

                // Author (optional)
                std::optional<std::string> author;
                if( auto authorStr = headerDoc["Author"].get<std::string>( "" ) )
                {
                    author = *authorStr;
                }

                // SystemConfiguration (optional)
                std::optional<std::vector<timeseries::ConfigurationReference>> systemConfiguration;
                if( auto sysConfigArrayOpt = headerDoc["SystemConfiguration"].get<Array>( "" ) )
                {
                    const auto& sysConfigArray = *sysConfigArrayOpt;
                    std::vector<timeseries::ConfigurationReference> sysConfigs;
                    sysConfigs.reserve( sysConfigArray.size() );

                    for( const auto& configDoc : sysConfigArray )
                    {
                        auto idOpt = configDoc["ID"].get<std::string>( "" );
                        auto tsOpt = configDoc["TimeStamp"].get<std::string>( "" );

                        if( idOpt.has_value() && tsOpt.has_value() )
                        {
                            if( !isValidIso19848DateTimeOffset( *tsOpt ) )
                            {
                                throw std::runtime_error{
                                    "Invalid ISO 19848 format for SystemConfiguration/TimeStamp: '" + *tsOpt + "'"
                                };
                            }
                            DateTimeOffset timestamp;
                            if( DateTimeOffset::fromString( *tsOpt, timestamp ) )
                            {
                                sysConfigs.emplace_back( timeseries::ConfigurationReference{ *idOpt, timestamp } );
                            }
                        }
                    }
                    if( !sysConfigs.empty() )
                    {
                        systemConfiguration = std::move( sysConfigs );
                    }
                }

                // CustomHeaders (optional)
                std::optional<timeseries::CustomHeaders> customHeaders;
                if( auto headerObjOpt = headerDoc.getRef<Object>( "" ) )
                {
                    static const std::unordered_set<std::string> knownKeys = { "ShipID",      "TimeSpan",
                                                                               "DateCreated", "DateModified",
                                                                               "Author",      "SystemConfiguration" };

                    nfx::json::Document customHdrsDoc = nfx::json::Document::object();
                    bool hasCustomHeaders = false;

                    // Custom headers object
                    if( auto customHdrsRef = customHdrsDoc.rootRef<Object>() )
                    {
                        auto& customHdrsObj = customHdrsRef->get();
                        for( const auto& [key, valueDoc] : headerObjOpt->get() )
                        {
                            // Skip known properties
                            if( knownKeys.find( key ) != knownKeys.end() )
                            {
                                continue;
                            }

                            // Add custom header
                            customHdrsObj.emplace_back( key, valueDoc );
                            hasCustomHeaders = true;
                        }
                    }

                    if( hasCustomHeaders )
                    {
                        timeseries::CustomHeaders customHdrs;
                        customHdrs.document() = std::move( customHdrsDoc );
                        customHeaders = std::move( customHdrs );
                    }
                }

                header = timeseries::Header{ *shipIdOpt,          timeSpan,     dateCreated, dateModified, author,
                                             systemConfiguration, customHeaders };
            }

            // TimeSeriesData array
            std::vector<timeseries::TimeSeriesData> timeSeriesDataList;

            if( auto tsArrayOpt = packageObj["TimeSeriesData"].get<Array>( "" ) )
            {
                const auto& tsArray = *tsArrayOpt;
                timeSeriesDataList.reserve( tsArray.size() );

                for( const auto& tsDoc : tsArray )
                {
                    // DataConfiguration (optional)
                    std::optional<timeseries::ConfigurationReference> dataConfiguration;
                    auto& dataConfigRef = tsDoc["DataConfiguration"];
                    if( dataConfigRef.isValid() )
                    {
                        auto& dataConfigObj = dataConfigRef;
                        auto idOpt = dataConfigObj["ID"].get<std::string>( "" );
                        auto tsOpt = dataConfigObj["TimeStamp"].get<std::string>( "" );
                        if( idOpt.has_value() && tsOpt.has_value() )
                        {
                            if( !isValidIso19848DateTimeOffset( *tsOpt ) )
                            {
                                throw std::runtime_error{
                                    "Invalid ISO 19848 format for DataConfiguration/TimeStamp: '" + *tsOpt + "'"
                                };
                            }
                            DateTimeOffset timestamp;
                            if( DateTimeOffset::fromString( *tsOpt, timestamp ) )
                            {
                                dataConfiguration = timeseries::ConfigurationReference{ *idOpt, timestamp };
                            }
                        }
                    }

                    // TabularData (optional)
                    std::optional<std::vector<timeseries::TabularData>> tabularDataList;
                    if( auto tabularArrayOpt = tsDoc["TabularData"].get<Array>( "" ) )
                    {
                        const auto& tabularArray = *tabularArrayOpt;
                        std::vector<timeseries::TabularData> tabList;
                        tabList.reserve( tabularArray.size() );

                        for( const auto& tabDoc : tabularArray )
                        {
                            // DataChannelIDs
                            std::vector<timeseries::DataChannelId> channelIds;
                            if( auto channelIdArrayOpt = tabDoc["DataChannelID"].get<Array>( "" ) )
                            {
                                const auto& channelIdArray = *channelIdArrayOpt;
                                channelIds.reserve( channelIdArray.size() );

                                for( const auto& cidDoc : channelIdArray )
                                {
                                    auto cidStr = cidDoc.get<std::string>( "" );
                                    if( cidStr.has_value() )
                                    {
                                        auto cidOpt = timeseries::DataChannelId::fromString( *cidStr );
                                        if( cidOpt.has_value() )
                                        {
                                            channelIds.push_back( *cidOpt );
                                        }
                                    }
                                }
                            }

                            // Validate NumberOfDataChannel
                            if( auto numberOfDataChannelOpt = tabDoc["NumberOfDataChannel"].get<int64_t>( "" ) )
                            {
                                if( *numberOfDataChannelOpt != static_cast<int64_t>( channelIds.size() ) )
                                {
                                    throw std::runtime_error{
                                        "Number of data channels does not match the expected count"
                                    };
                                }
                            }

                            // DataSets
                            std::vector<timeseries::TabularDataSet> dataSets;
                            if( auto dataSetArrayOpt = tabDoc["DataSet"].get<Array>( "" ) )
                            {
                                const auto& dataSetArray = *dataSetArrayOpt;
                                dataSets.reserve( dataSetArray.size() );

                                for( const auto& dsDoc : dataSetArray )
                                {
                                    auto timestampStr = dsDoc["TimeStamp"].get<std::string>( "" );
                                    if( !timestampStr.has_value() )
                                    {
                                        continue;
                                    }

                                    if( !isValidIso19848DateTimeOffset( *timestampStr ) )
                                    {
                                        throw std::runtime_error{
                                            "Invalid ISO 19848 DateTimeOffset format in TabularDataSet TimeStamp: " +
                                            *timestampStr
                                        };
                                    }

                                    // Values
                                    std::vector<std::string> values;
                                    if( auto valueArrayOpt = dsDoc["Value"].get<Array>( "" ) )
                                    {
                                        const auto& valueArray = *valueArrayOpt;
                                        values.reserve( valueArray.size() );

                                        for( const auto& valueDoc : valueArray )
                                        {
                                            if( auto valueOpt = valueDoc.get<std::string>( "" ) )
                                            {
                                                values.push_back( *valueOpt );
                                            }
                                        }
                                    }

                                    // Quality (optional)
                                    std::optional<std::vector<std::string>> quality;
                                    if( auto qualityArrayOpt = dsDoc["Quality"].get<Array>( "" ) )
                                    {
                                        const auto& qualityArray = *qualityArrayOpt;
                                        std::vector<std::string> qualities;
                                        qualities.reserve( qualityArray.size() );

                                        for( const auto& qualityDoc : qualityArray )
                                        {
                                            if( auto qualityOpt = qualityDoc.get<std::string>( "" ) )
                                            {
                                                qualities.push_back( *qualityOpt );
                                            }
                                        }
                                        if( !qualities.empty() )
                                        {
                                            quality = std::move( qualities );
                                        }
                                    }

                                    DateTimeOffset timestamp;
                                    if( DateTimeOffset::fromString( *timestampStr, timestamp ) )
                                    {
                                        dataSets.emplace_back(
                                            timeseries::TabularDataSet{ timestamp, values, quality } );
                                    }
                                }
                            }

                            // Validate NumberOfDataSet
                            if( auto numberOfDataSetOpt = tabDoc["NumberOfDataSet"].get<int64_t>( "" ) )
                            {
                                if( *numberOfDataSetOpt != static_cast<int64_t>( dataSets.size() ) )
                                {
                                    throw std::runtime_error{ "Number of data sets does not match the expected count" };
                                }
                            }

                            if( !channelIds.empty() && !dataSets.empty() )
                            {
                                tabList.emplace_back( timeseries::TabularData{ channelIds, dataSets } );
                            }
                        }

                        if( !tabList.empty() )
                        {
                            tabularDataList = std::move( tabList );
                        }
                    }

                    // EventData (optional)
                    std::optional<timeseries::EventData> eventData;
                    auto& eventDataRef = tsDoc["EventData"];
                    if( eventDataRef.isValid() )
                    {
                        auto& eventDataObj = eventDataRef;
                        if( auto eventDataArrayOpt = eventDataObj["DataSet"].get<Array>( "" ) )
                        {
                            const auto& eventDataArray = *eventDataArrayOpt;
                            std::vector<timeseries::EventDataSet> eventDataSets;
                            eventDataSets.reserve( eventDataArray.size() );

                            for( const auto& edDoc : eventDataArray )
                            {
                                auto timestampStr = edDoc["TimeStamp"].get<std::string>( "" );
                                auto dataChannelIdStr = edDoc["DataChannelID"].get<std::string>( "" );
                                auto valueStr = edDoc["Value"].get<std::string>( "" );

                                if( timestampStr.has_value() && dataChannelIdStr.has_value() && valueStr.has_value() )
                                {
                                    if( !isValidIso19848DateTimeOffset( *timestampStr ) )
                                    {
                                        throw std::runtime_error{
                                            "Invalid ISO 19848 DateTimeOffset format in EventData TimeStamp: " +
                                            *timestampStr
                                        };
                                    }

                                    DateTimeOffset timestamp;
                                    auto dataChannelIdOpt = timeseries::DataChannelId::fromString( *dataChannelIdStr );

                                    if( DateTimeOffset::fromString( *timestampStr, timestamp ) &&
                                        dataChannelIdOpt.has_value() )
                                    {
                                        std::optional<std::string> quality;
                                        if( auto qualityOpt = edDoc["Quality"].get<std::string>( "" ) )
                                        {
                                            quality = *qualityOpt;
                                        }

                                        eventDataSets.emplace_back( timeseries::EventDataSet{
                                            timestamp, *dataChannelIdOpt, *valueStr, quality } );
                                    }
                                }
                            }

                            if( !eventDataSets.empty() )
                            {
                                eventData = timeseries::EventData{ eventDataSets };
                            }
                        }
                    }

                    // CustomDataKinds (optional)
                    std::optional<timeseries::CustomData> customDataKinds;
                    if( auto tsObjOpt = tsDoc.getRef<Object>( "" ) )
                    {
                        static const std::unordered_set<std::string> knownKeys = { "DataConfiguration",
                                                                                   "TabularData",
                                                                                   "EventData" };

                        nfx::json::Document customDataDoc = nfx::json::Document::object();
                        bool hasCustomData = false;

                        // Custom data object
                        if( auto customDataRef = customDataDoc.rootRef<Object>() )
                        {
                            auto& customDataObj = customDataRef->get();
                            for( const auto& [key, valueDoc] : tsObjOpt->get() )
                            {
                                // Skip known properties
                                if( knownKeys.find( key ) != knownKeys.end() )
                                {
                                    continue;
                                }

                                // Add custom property
                                customDataObj.emplace_back( key, valueDoc );
                                hasCustomData = true;
                            }
                        }

                        if( hasCustomData )
                        {
                            timeseries::CustomData customData;
                            customData.document() = std::move( customDataDoc );
                            customDataKinds = std::move( customData );
                        }
                    } // TimeSeriesData
                    timeSeriesDataList.emplace_back(
                        timeseries::TimeSeriesData{ dataConfiguration, tabularDataList, eventData, customDataKinds } );
                }
            }

            // Create Package
            timeseries::Package package{ header, std::move( timeSeriesDataList ) };

            return timeseries::TimeSeriesDataPackage{ std::move( package ) };
        }

    private:
        // Helper methods for streaming serialization
        static void serializeHeader( const timeseries::Header& header, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            // ShipID
            builder.writeKey( "ShipID" );
            builder.write( header.shipId().toString() );

            // TimeSpan (optional)
            if( auto timeSpan = header.timeSpan() )
            {
                builder.writeKey( "TimeSpan" );
                serializeTimeSpan( *timeSpan, builder );
            }

            // DateCreated (optional)
            if( auto dateCreated = header.dateCreated() )
            {
                builder.writeKey( "DateCreated" );
                builder.write( dateCreated->toString( DateTime::Format::Iso8601Precise ) );
            }

            // DateModified (optional)
            if( auto dateModified = header.dateModified() )
            {
                builder.writeKey( "DateModified" );
                builder.write( dateModified->toString( DateTime::Format::Iso8601Precise ) );
            }

            // Author (optional)
            if( auto author = header.author() )
            {
                builder.writeKey( "Author" );
                builder.write( *author );
            }

            // SystemConfiguration (optional)
            if( auto sysConfig = header.systemConfiguration() )
            {
                builder.writeKey( "SystemConfiguration" );
                builder.writeStartArray();
                for( const auto& config : *sysConfig )
                {
                    serializeConfigRef( config, builder );
                }
                builder.writeEndArray();
            }

            // CustomHeaders
            if( auto customHeaders = header.customHeaders() )
            {
                if( auto objRef = customHeaders->document().rootRef<Object>() )
                {
                    for( const auto& [key, valueDoc] : objRef->get() )
                    {
                        builder.write( key, valueDoc );
                    }
                }
            }

            builder.writeEndObject();
        }

        static void serializeTimeSpan( const timeseries::TimeSpan& timeSpan, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            builder.writeKey( "Start" );
            builder.write( timeSpan.start().toString( DateTime::Format::Iso8601Precise ) );

            builder.writeKey( "End" );
            builder.write( timeSpan.end().toString( DateTime::Format::Iso8601Precise ) );

            builder.writeEndObject();
        }

        static void serializeConfigRef( const timeseries::ConfigurationReference& ref, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            builder.writeKey( "ID" );
            builder.write( ref.id() );

            builder.writeKey( "TimeStamp" );
            builder.write( ref.timeStamp().toString( DateTime::Format::Iso8601Precise ) );

            builder.writeEndObject();
        }

        static void serializeTimeSeriesDataList(
            const std::vector<timeseries::TimeSeriesData>& dataList, nfx::json::Builder& builder )
        {
            builder.writeStartArray();
            for( const auto& data : dataList )
            {
                serializeTimeSeriesData( data, builder );
            }
            builder.writeEndArray();
        }

        static void serializeTimeSeriesData( const timeseries::TimeSeriesData& data, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            // DataConfiguration (optional)
            if( auto dataConfig = data.dataConfiguration() )
            {
                builder.writeKey( "DataConfiguration" );
                serializeConfigRef( *dataConfig, builder );
            }

            // TabularData (optional)
            if( auto tabularData = data.tabularData() )
            {
                builder.writeKey( "TabularData" );
                serializeTabularDataList( *tabularData, builder );
            }

            // EventData (optional)
            if( auto eventData = data.eventData() )
            {
                builder.writeKey( "EventData" );
                serializeEventData( *eventData, builder );
            }

            // CustomDataKinds
            if( auto customDataKinds = data.customDataKinds() )
            {
                if( auto objRef = customDataKinds->document().rootRef<Object>() )
                {
                    for( const auto& [key, valueDoc] : objRef->get() )
                    {
                        builder.write( key, valueDoc );
                    }
                }
            }

            builder.writeEndObject();
        }

        static void serializeTabularDataList(
            const std::vector<timeseries::TabularData>& tabularList, nfx::json::Builder& builder )
        {
            builder.writeStartArray();
            for( const auto& tabular : tabularList )
            {
                serializeTabularData( tabular, builder );
            }
            builder.writeEndArray();
        }

        static void serializeTabularData( const timeseries::TabularData& tabular, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            builder.writeKey( "NumberOfDataSet" );
            builder.write( static_cast<uint64_t>( tabular.numberOfDataSets() ) );

            builder.writeKey( "NumberOfDataChannel" );
            builder.write( static_cast<uint64_t>( tabular.numberOfDataChannels() ) );

            // DataChannelID
            builder.writeKey( "DataChannelID" );
            builder.writeStartArray();
            for( const auto& channelId : tabular.dataChannelIds() )
            {
                builder.write( channelId.toString() );
            }
            builder.writeEndArray();

            // DataSet
            builder.writeKey( "DataSet" );
            builder.writeStartArray();
            for( const auto& dataSet : tabular.dataSets() )
            {
                serializeTabularDataSet( dataSet, builder );
            }
            builder.writeEndArray();

            builder.writeEndObject();
        }

        static void serializeTabularDataSet( const timeseries::TabularDataSet& dataSet, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            // TimeStamp
            builder.writeKey( "TimeStamp" );
            builder.write( dataSet.timeStamp().toString( DateTime::Format::Iso8601Precise ) );

            // Value
            builder.writeKey( "Value" );
            builder.writeStartArray();
            for( const auto& val : dataSet.value() )
            {
                builder.write( val );
            }
            builder.writeEndArray();

            // Quality (optional)
            if( auto quality = dataSet.quality() )
            {
                builder.writeKey( "Quality" );
                builder.writeStartArray();
                for( const auto& qual : *quality )
                {
                    builder.write( qual );
                }
                builder.writeEndArray();
            }

            builder.writeEndObject();
        }

        static void serializeEventData( const timeseries::EventData& eventData, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            // NumberOfDataSet
            builder.writeKey( "NumberOfDataSet" );
            builder.write( static_cast<uint64_t>( eventData.numberOfDataSet() ) );

            // DataSet (optional)
            if( auto dataSet = eventData.dataSet() )
            {
                builder.writeKey( "DataSet" );
                builder.writeStartArray();
                for( const auto& event : *dataSet )
                {
                    serializeEventDataSet( event, builder );
                }
                builder.writeEndArray();
            }

            builder.writeEndObject();
        }

        static void serializeEventDataSet( const timeseries::EventDataSet& eventSet, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            // TimeStamp
            builder.writeKey( "TimeStamp" );
            builder.write( eventSet.timeStamp().toString( DateTime::Format::Iso8601Precise ) );

            // DataChannelID
            builder.writeKey( "DataChannelID" );
            builder.write( eventSet.dataChannelId().toString() );

            // Value
            builder.writeKey( "Value" );
            builder.write( eventSet.value() );

            // Quality (optional)
            if( auto quality = eventSet.quality() )
            {
                builder.writeKey( "Quality" );
                builder.write( *quality );
            }

            builder.writeEndObject();
        }
    };
} // namespace nfx::serialization::json
