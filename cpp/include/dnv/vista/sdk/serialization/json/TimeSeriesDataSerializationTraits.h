/**
 * @file TimeSeriesDataSerializationTraits.h
 * @brief JSON serialization specialization for TimeSeriesData domain objects
 * @details Provides SerializationTraits specializations for Vista SDK TimeSeriesData
 */

#pragma once

#include <nfx/serialization/json/SerializationTraits.h>
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
     * @details Converts Vista SDK TimeSeriesData domain objects directly to/from JSON
     */
    template <>
    struct SerializationTraits<timeseries::TimeSeriesDataPackage>
    {
    public:
        /**
         * @brief Serialize TimeSeriesDataPackage to JSON document
         * @param obj The TimeSeriesDataPackage to serialize
         * @param doc The JSON document to serialize into
         */
        static void serialize( const timeseries::TimeSeriesDataPackage& obj, SerializableDocument& doc )
        {
            auto& jsonDoc = doc.document();
            const auto& package = obj.package();

            // Serialize Header
            nfx::json::Document headerDoc;
            if( package.header().has_value() )
            {
                const auto& header = *package.header();

                // Build Header object
                headerDoc = nfx::json::Document::object();
                if( auto headerRef = headerDoc.rootRef<Object>() )
                {
                    auto& headerObj = headerRef->get();
                    // ShipID, TimeSpan, DateCreated, DateModified, Author, SystemConfiguration, CustomHeaders
                    headerObj.reserve( 7 );

                    // ShipID (required)
                    headerObj.emplace_back( "ShipID", nfx::json::Document{ header.shipId().toString() } );

                    // TimeSpan (optional)
                    if( auto timeSpanOpt = header.timeSpan() )
                    {
                        const auto& timeSpan = *timeSpanOpt;
                        nfx::json::Document timeSpanDoc = nfx::json::Document::object();
                        if( auto tsRef = timeSpanDoc.rootRef<Object>() )
                        {
                            auto& tsObj = tsRef->get();
                            tsObj.reserve( 2 );
                            tsObj.emplace_back(
                                "Start",
                                nfx::json::Document{
                                    timeSpan.start().toString( DateTime::Format::Iso8601PreciseTrimmed ) } );
                            tsObj.emplace_back(
                                "End",
                                nfx::json::Document{
                                    timeSpan.end().toString( DateTime::Format::Iso8601PreciseTrimmed ) } );
                        }
                        headerObj.emplace_back( "TimeSpan", std::move( timeSpanDoc ) );
                    }

                    // DateCreated (optional)
                    if( auto dateCreated = header.dateCreated() )
                    {
                        headerObj.emplace_back(
                            "DateCreated",
                            nfx::json::Document{ dateCreated->toString( DateTime::Format::Iso8601PreciseTrimmed ) } );
                    }

                    // DateModified (optional)
                    if( auto dateModified = header.dateModified() )
                    {
                        headerObj.emplace_back(
                            "DateModified",
                            nfx::json::Document{ dateModified->toString( DateTime::Format::Iso8601PreciseTrimmed ) } );
                    }

                    // Author (optional)
                    if( auto author = header.author() )
                    {
                        headerObj.emplace_back( "Author", nfx::json::Document{ author.value() } );
                    }

                    // SystemConfiguration (optional)
                    if( auto sysConfigsOpt = header.systemConfiguration() )
                    {
                        const auto& sysConfigs = *sysConfigsOpt;
                        Array sysConfigArray;
                        sysConfigArray.reserve( sysConfigs.size() );

                        for( const auto& config : sysConfigs )
                        {
                            nfx::json::Document configDoc = nfx::json::Document::object();
                            if( auto configRef = configDoc.rootRef<Object>() )
                            {
                                auto& cfgObj = configRef->get();
                                cfgObj.reserve( 2 );
                                cfgObj.emplace_back( "ID", nfx::json::Document{ config.id() } );
                                cfgObj.emplace_back(
                                    "TimeStamp",
                                    nfx::json::Document{
                                        config.timeStamp().toString( DateTime::Format::Iso8601PreciseTrimmed ) } );
                            }
                            sysConfigArray.push_back( std::move( configDoc ) );
                        }
                        headerObj.emplace_back(
                            "SystemConfiguration", nfx::json::Document{ std::move( sysConfigArray ) } );
                    }

                    // CustomHeaders
                    if( auto customHeadersOpt = header.customHeaders() )
                    {
                        const auto& customHdrs = *customHeadersOpt;
                        if( auto customHdrsObj = customHdrs.get<Object>( "" ) )
                        {
                            for( auto&& [key, valueDoc] : customHdrsObj.value() )
                            {
                                headerObj.emplace_back( std::move( key ), std::move( valueDoc ) );
                            }
                        }
                    }
                }
            }

            // TimeSeriesData
            const auto& timeSeriesData = package.timeSeriesData();
            Array timeSeriesArray;
            timeSeriesArray.reserve( timeSeriesData.size() );

            for( const auto& tsData : timeSeriesData )
            {
                // DataConfiguration (optional)
                nfx::json::Document dataConfigDoc;
                bool hasDataConfig = false;
                if( tsData.dataConfiguration().has_value() )
                {
                    hasDataConfig = true;
                    const auto& dataConfig = *tsData.dataConfiguration();
                    dataConfigDoc = nfx::json::Document::object();
                    if( auto dcRef = dataConfigDoc.rootRef<Object>() )
                    {
                        auto& dcObj = dcRef->get();
                        dcObj.reserve( 2 );
                        dcObj.emplace_back( "ID", nfx::json::Document{ dataConfig.id() } );
                        dcObj.emplace_back(
                            "TimeStamp",
                            nfx::json::Document{
                                dataConfig.timeStamp().toString( DateTime::Format::Iso8601PreciseTrimmed ) } );
                    }
                }

                // TabularData (optional)
                nfx::json::Document tabularDataArrayDoc;
                bool hasTabularData = false;
                if( auto tabularDataListOpt = tsData.tabularData() )
                {
                    hasTabularData = true;
                    const auto& tabularDataList = *tabularDataListOpt;
                    Array tabularArray;
                    tabularArray.reserve( tabularDataList.size() );

                    for( const auto& tabularData : tabularDataList )
                    {
                        const auto& dataSets = tabularData.dataSets();
                        const auto& channelIds = tabularData.dataChannelIds();

                        // TabularData object
                        nfx::json::Document tabDoc = nfx::json::Document::object();
                        if( auto tabRef = tabDoc.rootRef<Object>() )
                        {
                            auto& tabObj = tabRef->get();
                            tabObj.reserve( 4 ); // NumberOfDataSet, NumberOfDataChannel, DataChannelID, DataSet

                            tabObj.emplace_back(
                                "NumberOfDataSet", nfx::json::Document{ static_cast<uint64_t>( dataSets.size() ) } );
                            tabObj.emplace_back(
                                "NumberOfDataChannel",
                                nfx::json::Document{ static_cast<uint64_t>( channelIds.size() ) } );

                            // DataChannelID
                            Array channelIdArray;
                            channelIdArray.reserve( channelIds.size() );
                            for( const auto& channelId : channelIds )
                            {
                                channelIdArray.push_back( nfx::json::Document{ channelId.toString() } );
                            }
                            tabObj.emplace_back( "DataChannelID", nfx::json::Document{ std::move( channelIdArray ) } );

                            // DataSet
                            Array dataSetArray;
                            dataSetArray.reserve( dataSets.size() );
                            for( const auto& dataSet : dataSets )
                            {
                                // DataSet
                                nfx::json::Document dsDoc = nfx::json::Document::object();
                                if( auto dsRef = dsDoc.rootRef<Object>() )
                                {
                                    auto& dsObj = dsRef->get();
                                    dsObj.reserve( 3 ); // TimeStamp, Value, Quality (optional)

                                    dsObj.emplace_back(
                                        "TimeStamp",
                                        nfx::json::Document{
                                            dataSet.timeStamp().toString( DateTime::Format::Iso8601PreciseTrimmed ) } );

                                    // Values
                                    const auto& values = dataSet.value();
                                    Array valueArray;
                                    valueArray.reserve( values.size() );
                                    for( const auto& value : values )
                                    {
                                        valueArray.push_back( nfx::json::Document{ value } );
                                    }
                                    dsObj.emplace_back( "Value", nfx::json::Document{ std::move( valueArray ) } );

                                    // Quality (optional)
                                    if( dataSet.quality().has_value() )
                                    {
                                        const auto& qualities = *dataSet.quality();
                                        Array qualityArray;
                                        qualityArray.reserve( qualities.size() );
                                        for( const auto& quality : qualities )
                                        {
                                            qualityArray.push_back( nfx::json::Document{ quality } );
                                        }
                                        dsObj.emplace_back(
                                            "Quality", nfx::json::Document{ std::move( qualityArray ) } );
                                    }
                                }
                                dataSetArray.push_back( std::move( dsDoc ) );
                            }
                            tabObj.emplace_back( "DataSet", nfx::json::Document{ std::move( dataSetArray ) } );
                        }

                        tabularArray.push_back( std::move( tabDoc ) );
                    }
                    tabularDataArrayDoc = nfx::json::Document{ std::move( tabularArray ) };
                }

                // EventData (optional)
                nfx::json::Document eventDataDoc;
                bool hasEventData = false;
                if( tsData.eventData().has_value() )
                {
                    hasEventData = true;
                    const auto& eventData = *tsData.eventData();
                    if( eventData.dataSet().has_value() )
                    {
                        const auto& eventDataSets = *eventData.dataSet();

                        Array eventDataArray;
                        eventDataArray.reserve( eventDataSets.size() );

                        for( const auto& eventDataSet : eventDataSets )
                        {
                            nfx::json::Document edDoc = nfx::json::Document::object();
                            if( auto ref = edDoc.rootRef<Object>() )
                            {
                                auto& edObj = ref->get();
                                const bool hasQuality = eventDataSet.quality().has_value();
                                edObj.reserve( hasQuality ? 4 : 3 );

                                edObj.emplace_back(
                                    "TimeStamp",
                                    nfx::json::Document{ eventDataSet.timeStamp().toString(
                                        DateTime::Format::Iso8601PreciseTrimmed ) } );
                                edObj.emplace_back(
                                    "DataChannelID", nfx::json::Document{ eventDataSet.dataChannelId().toString() } );
                                edObj.emplace_back( "Value", nfx::json::Document{ eventDataSet.value() } );

                                // Quality (optional)
                                if( hasQuality )
                                {
                                    edObj.emplace_back( "Quality", nfx::json::Document{ *eventDataSet.quality() } );
                                }
                            }

                            eventDataArray.push_back( std::move( edDoc ) );
                        }

                        eventDataDoc = nfx::json::Document::object();
                        if( auto ref = eventDataDoc.rootRef<Object>() )
                        {
                            auto& eventDataObj = ref->get();
                            eventDataObj.reserve( 2 );
                            eventDataObj.emplace_back(
                                "NumberOfDataSet",
                                nfx::json::Document{ static_cast<uint64_t>( eventDataSets.size() ) } );
                            eventDataObj.emplace_back( "DataSet", nfx::json::Document{ std::move( eventDataArray ) } );
                        }
                    }
                }

                // TimeSeriesData
                nfx::json::Document tsDoc = nfx::json::Document::object();
                if( auto tsRef = tsDoc.rootRef<Object>() )
                {
                    auto& tsObj = tsRef->get();
                    // Reserve space: DataConfiguration, TabularData, EventData, + CustomData fields
                    tsObj.reserve( 3 + ( tsData.customDataKinds().has_value() ? 5 : 0 ) );

                    if( hasDataConfig )
                    {
                        tsObj.emplace_back( "DataConfiguration", std::move( dataConfigDoc ) );
                    }

                    if( hasTabularData )
                    {
                        tsObj.emplace_back( "TabularData", std::move( tabularDataArrayDoc ) );
                    }

                    if( hasEventData )
                    {
                        tsObj.emplace_back( "EventData", std::move( eventDataDoc ) );
                    }

                    // CustomData (JsonExtensionData - flattened at TimeSeriesData level)
                    if( auto customDataKindsOpt = tsData.customDataKinds() )
                    {
                        const auto& customDataKinds = *customDataKindsOpt;
                        if( auto customDataKindsObj = customDataKinds.get<Object>( "" ) )
                        {
                            for( auto&& [key, valueDoc] : customDataKindsObj.value() )
                            {
                                tsObj.emplace_back( std::move( key ), std::move( valueDoc ) );
                            }
                        }
                    }
                }

                timeSeriesArray.push_back( std::move( tsDoc ) );
            }

            // Package
            nfx::json::Document packageDoc = nfx::json::Document::object();
            if( auto packageRef = packageDoc.rootRef<Object>() )
            {
                auto& packageObj = packageRef->get();
                packageObj.reserve( headerDoc.isValid() ? 2 : 1 );

                if( headerDoc.isValid() )
                {
                    packageObj.emplace_back( "Header", std::move( headerDoc ) );
                }
                packageObj.emplace_back( "TimeSeriesData", nfx::json::Document{ std::move( timeSeriesArray ) } );
            }

            jsonDoc["Package"] = std::move( packageDoc );
        }

        /**
         * @brief Deserialize TimeSeriesDataPackage from JSON document
         * @param doc The JSON document to deserialize from
         * @param obj The TimeSeriesDataPackage to populate
         */
        static void deserialize( const SerializableDocument& doc, timeseries::TimeSeriesDataPackage& obj )
        {
            auto& jsonDoc = doc.document();

            auto& packageObj = jsonDoc["Package"];

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
                if( auto headerObjOpt = headerDoc.get<Object>( "" ) )
                {
                    static const std::unordered_set<std::string> knownKeys = { "ShipID",      "TimeSpan",
                                                                               "DateCreated", "DateModified",
                                                                               "Author",      "SystemConfiguration" };

                    timeseries::CustomHeaders customHdrs;
                    auto& customHdrsDoc = customHdrs.document();
                    bool hasCustomHeaders = false;

                    // Custom headers object
                    if( auto customHdrsRef = customHdrsDoc.rootRef<Object>() )
                    {
                        auto& customHdrsObj = customHdrsRef->get();
                        for( const auto& [key, valueDoc] : headerObjOpt.value() )
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
                    if( auto tsObjOpt = tsDoc.get<Object>( "" ) )
                    {
                        static const std::unordered_set<std::string> knownKeys = { "DataConfiguration",
                                                                                   "TabularData",
                                                                                   "EventData" };

                        timeseries::CustomData customData;
                        auto& customDataDoc = customData.document();
                        bool hasCustomData = false;

                        // Custom data object
                        if( auto customDataRef = customDataDoc.rootRef<Object>() )
                        {
                            auto& customDataObj = customDataRef->get();
                            for( const auto& [key, valueDoc] : tsObjOpt.value() )
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
                            customDataKinds = std::move( customData );
                        }
                    } // TimeSeriesData
                    timeSeriesDataList.emplace_back(
                        timeseries::TimeSeriesData{ dataConfiguration, tabularDataList, eventData, customDataKinds } );
                }
            }

            // Create Package
            timeseries::Package package{ header, std::move( timeSeriesDataList ) };

            // Assign to obj
            obj = timeseries::TimeSeriesDataPackage{ std::move( package ) };
        }
    };
} // namespace nfx::serialization::json
