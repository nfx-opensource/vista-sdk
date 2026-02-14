/**
 * @file Sample_TimeSeriesData.cpp
 * @brief Demonstrates usage of vista-sdk-cpp TimeSeriesData API with ISO 19848 support
 * @details This sample shows how to create TimeSeriesData packages with headers, tabular/event data,
 *          cross-validation with DataChannelList, and JSON serialization per ISO 19848:2024
 */

#include <dnv/vista/sdk/Transport.h>
#include <dnv/vista/sdk/VIS.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    using namespace dnv::vista::sdk;
    using namespace dnv::vista::sdk::transport;

    std::cout << "=== vista-sdk-cpp TimeSeriesData Sample ===\n\n";

    //=====================================================================
    // 1. Header: Creating TimeSeriesData package headers
    //=====================================================================
    {
        std::cout << "1. Header: Creating TimeSeriesData package headers\n";
        std::cout << "-----------------------------------------------------\n";

        // Create ShipId
        auto shipId = ShipId::fromString( "IMO9074729" ).value();

        // Create time span for the data
        auto start = DateTimeOffset{ "2024-12-30T10:00:00Z" };
        auto end = DateTimeOffset{ "2024-12-30T14:00:00Z" };
        auto timeSpan = timeseries::TimeSpan{ start, end };

        // Create timestamps
        auto dateCreated = DateTimeOffset{ "2024-12-30T10:00:00Z" };
        auto dateModified = DateTimeOffset{ "2024-12-30T14:30:00Z" };

        // Create system configuration references
        std::vector<timeseries::ConfigurationReference> systemConfigs{ timeseries::ConfigurationReference{
            "SystemConfiguration.xml", DateTimeOffset{ "2024-12-30T00:00:00Z" } } };

        // Build header
        auto header = timeseries::Header{
            shipId,             // Ship identification
            timeSpan,           // Data time span
            dateCreated,        // Creation timestamp
            dateModified,       // Last modification
            "Vista SDK Sample", // Author name
            systemConfigs       // System config refs
        };

        std::cout << "Created header:\n";
        std::cout << "  Ship ID       : " << header.shipId().toString() << "\n";
        std::cout << "  Time Span     : " << header.timeSpan()->start().toString() << " - "
                  << header.timeSpan()->end().toString() << "\n";
        std::cout << "  Date Created  : " << header.dateCreated()->toString() << "\n";
        std::cout << "  Date Modified : " << header.dateModified()->toString() << "\n";
        std::cout << "  Author        : " << header.author().value() << "\n";
        std::cout << "  System Configs: " << header.systemConfiguration()->size() << " entries\n";

        std::cout << "\n";
    }

    //=====================================================================
    // 2. TabularData: Creating tabular time series data
    //=====================================================================
    {
        std::cout << "2. TabularData: Creating tabular time series data\n";
        std::cout << "----------------------------------------------------\n";

        // Create DataChannelIds (references to DataChannelList)
        auto channelId1 = timeseries::DataChannelId::fromString( "Temperature" ).value();
        auto channelId2 = timeseries::DataChannelId::fromString( "Pressure" ).value();

        std::vector<timeseries::DataChannelId> dataChannelIds{ channelId1, channelId2 };

        // Create data sets with timestamps, values, and quality codes
        auto timestamp1 = DateTimeOffset{ "2024-12-30T10:00:00Z" };
        auto timestamp2 = DateTimeOffset{ "2024-12-30T11:00:00Z" };
        auto timestamp3 = DateTimeOffset{ "2024-12-30T12:00:00Z" };

        std::vector<timeseries::TabularDataSet> dataSets{
            timeseries::TabularDataSet{ timestamp1,                                   // First timestamp
                                        std::vector<std::string>{ "100.5", "200.0" }, // Channel values
                                        std::vector<std::string>{ "0", "0" } },       // Quality codes (good)
            timeseries::TabularDataSet{ timestamp2,                                   // Second timestamp
                                        std::vector<std::string>{ "105.2", "205.5" }, // Channel values
                                        std::vector<std::string>{ "0", "0" } },       // Quality codes (good)
            timeseries::TabularDataSet{ timestamp3,                                   // Third timestamp
                                        std::vector<std::string>{ "110.0", "210.0" }, // Channel values
                                        std::vector<std::string>{ "0", "0" } }
        }; // Quality codes (good)

        // Create TabularData
        auto tabularData = timeseries::TabularData{ dataChannelIds, dataSets };

        std::cout << "Created TabularData:\n";
        std::cout << "  Data Channels: " << tabularData.dataChannelIds().size() << "\n";
        std::cout << "  Data Sets    : " << tabularData.dataSets().size() << "\n";
        std::cout << "  Validation   : " << std::boolalpha << tabularData.validate().isOk() << "\n";

        std::cout << "\n";
    }

    //=====================================================================
    // 3. EventData: Creating event-based time series data
    //=====================================================================
    {
        std::cout << "3. EventData: Creating event-based time series data\n";
        std::cout << "------------------------------------------------------\n";

        // Create EventDataSets
        auto channelId = timeseries::DataChannelId::fromString( "AlarmStatus" ).value();
        auto timestamp1 = DateTimeOffset{ "2024-12-30T10:30:00Z" };
        auto timestamp2 = DateTimeOffset{ "2024-12-30T11:45:00Z" };

        std::vector<timeseries::EventDataSet> eventDataSets{ timeseries::EventDataSet{
                                                                 timestamp1,        // Event time
                                                                 channelId,         // Channel ID
                                                                 "HIGH",            // Alarm value
                                                                 std::string{ "0" } // Quality (good)
                                                             },
                                                             timeseries::EventDataSet{
                                                                 timestamp2,        // Event time
                                                                 channelId,         // Channel ID
                                                                 "NORMAL",          // Alarm value
                                                                 std::string{ "0" } // Quality (good)
                                                             } };

        // Create EventData
        auto eventData = timeseries::EventData{ eventDataSets };

        std::cout << "Created EventData:\n";
        std::cout << "  Event Data Sets: " << eventData.dataSet()->size() << "\n";

        std::cout << "\n";
    }

    //=====================================================================
    // 4. TimeSeriesData: Combining tabular and event data
    //=====================================================================
    {
        std::cout << "4. TimeSeriesData: Combining tabular and event data\n";
        std::cout << "------------------------------------------------------\n";

        // Create ConfigurationReference
        auto dataConfig =
            timeseries::ConfigurationReference{ "DataChannelList.xml", DateTimeOffset{ "2024-12-30T00:00:00Z" } };

        // Create TabularData
        auto channelId1 = timeseries::DataChannelId::fromString( "Temp1" ).value();
        auto channelId2 = timeseries::DataChannelId::fromString( "Press1" ).value();

        std::vector<timeseries::TabularDataSet> dataSets{ timeseries::TabularDataSet{
            DateTimeOffset{ "2024-12-30T10:00:00Z" }, std::vector<std::string>{ "100.0", "200.0" }, std::nullopt } };

        auto tabularData =
            timeseries::TabularData{ std::vector<timeseries::DataChannelId>{ channelId1, channelId2 }, dataSets };

        // Create EventData
        auto eventChannelId = timeseries::DataChannelId::fromString( "Alarm1" ).value();
        std::vector<timeseries::EventDataSet> eventSets{ timeseries::EventDataSet{
            DateTimeOffset{ "2024-12-30T10:30:00Z" }, // Event timestamp
            eventChannelId,                           // Alarm channel
            "HIGH",                                   // Alert level
            "0"                                       // Quality (good)
        } };

        auto eventData = timeseries::EventData{ eventSets };

        // Create TimeSeriesData with both tabular and event data
        auto timeSeriesData = timeseries::TimeSeriesData{
            dataConfig, std::vector<timeseries::TabularData>{ tabularData }, eventData, std::nullopt
        };

        std::cout << "Created TimeSeriesData:\n";
        std::cout << "  Data Configuration: " << timeSeriesData.dataConfiguration()->id() << "\n";
        std::cout << "  Tabular Data      : " << timeSeriesData.tabularData()->size() << " tables\n";
        std::cout << "  Event Data        : " << timeSeriesData.eventData()->dataSet()->size() << " events\n";

        std::cout << "\n";
    }

    //=====================================================================
    // 5. Package: Creating complete TimeSeriesData package
    //=====================================================================
    {
        std::cout << "5. Package: Creating complete TimeSeriesData package\n";
        std::cout << "-------------------------------------------------------\n";

        // Create Header
        auto shipId = ShipId::fromString( "IMO9074729" ).value();
        auto timeSpan =
            timeseries::TimeSpan{ DateTimeOffset{ "2024-12-30T10:00:00Z" }, DateTimeOffset{ "2024-12-30T14:00:00Z" } };

        auto header = timeseries::Header{
            shipId,                                   // Ship ID
            timeSpan,                                 // Time span
            DateTimeOffset{ "2024-12-30T10:00:00Z" }, // Created date
            DateTimeOffset{ "2024-12-30T14:30:00Z" }, // Modified date
            "Vista SDK",                              // Author
            std::nullopt                              // No system configs
        };

        // Create TimeSeriesData
        auto dataConfig =
            timeseries::ConfigurationReference{ "DataChannelList.xml", DateTimeOffset{ "2024-12-30T00:00:00Z" } };

        auto channelId = timeseries::DataChannelId::fromString( "Sensor1" ).value();
        std::vector<timeseries::TabularDataSet> dataSets{ timeseries::TabularDataSet{
            DateTimeOffset{ "2024-12-30T10:00:00Z" }, std::vector<std::string>{ "100.0" }, std::nullopt } };

        auto tabularData = timeseries::TabularData{ std::vector<timeseries::DataChannelId>{ channelId }, dataSets };

        auto timeSeriesData = timeseries::TimeSeriesData{
            dataConfig, std::vector<timeseries::TabularData>{ tabularData }, std::nullopt, std::nullopt
        };

        // Create Package
        auto package = timeseries::Package{ header, std::vector<timeseries::TimeSeriesData>{ timeSeriesData } };

        auto timeSeriesDataPackage = timeseries::TimeSeriesDataPackage{ package };

        std::cout << "Created TimeSeriesDataPackage:\n";
        std::cout << "  Header           : " << std::boolalpha << timeSeriesDataPackage.package().header().has_value()
                  << "\n";
        std::cout << "  TimeSeriesData   : " << timeSeriesDataPackage.package().timeSeriesData().size() << " entries\n";

        std::cout << "\n";
    }

    //=====================================================================
    // 6. JSON Serialization: Round-trip serialization
    //=====================================================================
    {
        std::cout << "6. JSON Serialization: Round-trip serialization\n";
        std::cout << "--------------------------------------------------\n";

        // Create simple package
        auto shipId = ShipId::fromString( "IMO1234567" ).value();
        auto header = timeseries::Header{ shipId,       std::nullopt, DateTimeOffset{ "2024-12-30T10:00:00Z" },
                                          std::nullopt, std::nullopt, std::nullopt };

        auto dataConfig =
            timeseries::ConfigurationReference{ "DataChannelList.xml", DateTimeOffset{ "2024-12-30T00:00:00Z" } };

        auto channelId = timeseries::DataChannelId::fromString( "Ch1" ).value();
        auto tabularData = timeseries::TabularData{
            std::vector<timeseries::DataChannelId>{ channelId },
            std::vector<timeseries::TabularDataSet>{ timeseries::TabularDataSet{
                DateTimeOffset{ "2024-12-30T10:00:00Z" }, std::vector<std::string>{ "42.0" }, std::nullopt } }
        };

        auto timeSeriesData = timeseries::TimeSeriesData{
            dataConfig, std::vector<timeseries::TabularData>{ tabularData }, std::nullopt, std::nullopt
        };

        auto package = timeseries::Package{ header, std::vector<timeseries::TimeSeriesData>{ timeSeriesData } };

        auto originalPackage = timeseries::TimeSeriesDataPackage{ package };

        // Serialize to JSON
        auto options = serialization::json::Serializer<timeseries::TimeSeriesDataPackage>::Options{};
        options.includeNullFields = false;
        options.prettyPrint = false;

        auto json =
            serialization::json::Serializer<timeseries::TimeSeriesDataPackage>::toString( originalPackage, options );

        std::cout << "Serialized to JSON (" << json.length() << " bytes)\n";

        // Deserialize from JSON
        auto docOpt = json::Document::fromString( json );
        if( docOpt.has_value() )
        {
            auto deserializedPackage =
                serialization::json::SerializationTraits<timeseries::TimeSeriesDataPackage>::fromDocument( *docOpt );

            std::cout << "Deserialized successfully:\n";
            std::cout << "  Ship ID        : " << deserializedPackage.package().header()->shipId().toString() << "\n";
            std::cout << "  TimeSeriesData : " << deserializedPackage.package().timeSeriesData().size() << " entries\n";
        }

        std::cout << "\n";
    }

    //=====================================================================
    // 7. Cross-Validation: Validating against DataChannelList
    //=====================================================================
    {
        std::cout << "7. Cross-Validation: Validating against DataChannelList\n";
        std::cout << "----------------------------------------------------------\n";

        const auto& vis = VIS::instance();
        const auto& gmod = vis.gmod( vis.latest() );
        const auto& locations = vis.locations( vis.latest() );
        const auto& codebooks = vis.codebooks( vis.latest() );

        // Create DataChannelList
        auto shipId = ShipId::fromString( "IMO9074729" ).value();
        auto configRef = datachannel::ConfigurationReference{ "DataChannelList.xml",
                                                              DateTimeOffset{ "2024-12-30T00:00:00Z" },
                                                              "v1" };

        auto dclHeader = datachannel::Header{
            shipId,       // Ship ID
            configRef,    // Config reference
            std::nullopt, // No data config
            std::nullopt, // No created date
            std::nullopt, // No modified date
            std::nullopt  // No author
        };

        // Create DataChannel with validation rules
        auto primaryItem = GmodPath::fromShortPath( "411.1/C101.31-2", gmod, locations ).value();
        auto qtyTag = codebooks[CodebookName::Quantity].createTag( "temperature" ).value();

        auto localId =
            LocalIdBuilder::create( vis.latest() ).withPrimaryItem( primaryItem ).withMetadataTag( qtyTag ).build();

        auto dataChannelId = datachannel::DataChannelId{ localId, "TempSensor", std::nullopt };

        auto restriction = datachannel::Restriction{};
        restriction.setFractionDigits( 1 );

        auto format = datachannel::Format{ "Decimal", restriction };
        auto dataChannelType = datachannel::DataChannelType{ "Inst" };

        auto range = datachannel::Range{ 0.0, 200.0 };
        auto unit = datachannel::Unit{ "°C" };

        auto property = datachannel::Property{
            dataChannelType, // Channel type
            format,          // Data format
            range,           // Valid range
            unit,            // Unit of measure
            std::nullopt,    // No name
            std::nullopt,    // No remarks
            std::nullopt,    // No alert priority
            std::nullopt,    // No name object
            std::nullopt     // No calculation period
        };

        auto dataChannel = datachannel::DataChannel{ dataChannelId, property };

        auto dataChannelList = datachannel::DataChannelList{ std::vector<datachannel::DataChannel>{ dataChannel } };

        auto dclPackage = datachannel::Package{ dclHeader, dataChannelList };

        auto dataChannelListPackage = datachannel::DataChannelListPackage{ dclPackage };

        // Create TimeSeriesData
        auto tsDataConfig =
            timeseries::ConfigurationReference{ "DataChannelList.xml", DateTimeOffset{ "2024-12-30T00:00:00Z" } };

        auto tsChannelId = timeseries::DataChannelId::fromString( "TempSensor" ).value();

        auto tabularData = timeseries::TabularData{
            std::vector<timeseries::DataChannelId>{ tsChannelId },
            std::vector<timeseries::TabularDataSet>{ timeseries::TabularDataSet{
                DateTimeOffset{ "2024-12-30T10:00:00Z" }, std::vector<std::string>{ "100.5" }, std::nullopt } }
        };

        // Add EventData to trigger onEventData callback
        auto eventData = timeseries::EventData{ std::vector<timeseries::EventDataSet>{ timeseries::EventDataSet{
            DateTimeOffset{ "2024-12-30T10:15:00Z" }, // Event timestamp
            tsChannelId,                              // Channel reference
            "105.2",                                  // Event value
            std::string{ "0" }                        // Quality code (good)
        } } };

        auto timeSeriesData = timeseries::TimeSeriesData{
            tsDataConfig,                                        // Configuration reference
            std::vector<timeseries::TabularData>{ tabularData }, // Tabular data
            eventData,                                           // Event data
            std::nullopt                                         // No custom data
        };

        // Validate callbacks with detailed channel information
        auto onTabularData = []( const DateTimeOffset& timestamp,
                                 const datachannel::DataChannel& channel,
                                 const transport::Value& value,
                                 const std::optional<std::string>& qualityCode ) {
            std::cout << "  Validating tabular data:\n";
            std::cout << "    Channel ID: " << channel.dataChannelId().shortId().value_or( "[no shortId]" ) << "\n";
            std::cout << "    Timestamp : " << timestamp.toString() << "\n";
            std::cout << "    Value     : " << value.toString() << "\n";
            std::cout << "    Type      : " << channel.property().dataChannelType().type() << "\n";
            std::cout << "    Format    : " << channel.property().format().type() << "\n";
            if( channel.property().range() )
                std::cout << "    Range     : [" << channel.property().range()->low() << ", "
                          << channel.property().range()->high() << "]\n";
            if( channel.property().unit() )
                std::cout << "    Unit      : " << channel.property().unit()->unitSymbol() << "\n";
            if( qualityCode )
                std::cout << "    Quality   : " << *qualityCode << "\n";
            return ValidateResult<>::ok();
        };

        auto onEventData = []( const DateTimeOffset& timestamp,
                               const datachannel::DataChannel& channel,
                               const transport::Value& value,
                               const std::optional<std::string>& qualityCode ) {
            std::cout << "  Validating event data:\n";
            std::cout << "    Channel ID: " << channel.dataChannelId().shortId().value_or( "[no shortId]" ) << "\n";
            std::cout << "    Timestamp : " << timestamp.toString() << "\n";
            std::cout << "    Value     : " << value.toString() << "\n";
            std::cout << "    Type      : " << channel.property().dataChannelType().type() << "\n";
            std::cout << "    Format    : " << channel.property().format().type() << "\n";
            if( channel.property().range() )
                std::cout << "    Range     : [" << channel.property().range()->low() << ", "
                          << channel.property().range()->high() << "]\n";
            if( channel.property().unit() )
                std::cout << "    Unit      : " << channel.property().unit()->unitSymbol() << "\n";
            if( qualityCode )
                std::cout << "    Quality   : " << *qualityCode << "\n";
            return ValidateResult<>::ok();
        };

        // Perform cross-validation
        auto result = timeSeriesData.validate( dataChannelListPackage, onTabularData, onEventData );

        std::cout << "Cross-validation result:\n";
        std::cout << "  Is valid: " << std::boolalpha << result.isOk() << "\n";
        if( !result.isOk() )
        {
            std::cout << "  Errors:\n";
            for( const auto& error : result.errors )
            {
                std::cout << "    - " << error << "\n";
            }
        }

        std::cout << "\n";
    }

    //=====================================================================
    // 8. Custom Validation: Business rule callbacks
    //=====================================================================
    {
        std::cout << "8. Custom Validation: Business rule callbacks\n";
        std::cout << "------------------------------------------------\n";

        const auto& vis = VIS::instance();
        const auto& gmod = vis.gmod( vis.latest() );
        const auto& locations = vis.locations( vis.latest() );
        const auto& codebooks = vis.codebooks( vis.latest() );

        // Create minimal DataChannelList
        auto shipId = ShipId::fromString( "IMO1234567" ).value();
        auto configRef =
            datachannel::ConfigurationReference{ "DataChannelList.xml", DateTimeOffset{ "2024-12-30T00:00:00Z" } };

        auto primaryItem = GmodPath::fromShortPath( "411.1", gmod, locations ).value();
        auto qtyTag = codebooks[CodebookName::Quantity].createTag( "pressure" ).value();

        auto localId =
            LocalIdBuilder::create( vis.latest() ).withPrimaryItem( primaryItem ).withMetadataTag( qtyTag ).build();

        auto dataChannelId = datachannel::DataChannelId{ localId, "Sensor", std::nullopt };

        auto dataChannel = datachannel::DataChannel{ dataChannelId,
                                                     datachannel::Property{
                                                         datachannel::DataChannelType{ "Inst" }, // Instantaneous type
                                                         datachannel::Format{ "Decimal" },       // Decimal format
                                                         datachannel::Range{ 0.0, 1000.0 },      // Pressure range
                                                         datachannel::Unit{ "bar" },
                                                         std::nullopt, // No name
                                                         std::nullopt, // No remarks
                                                         std::nullopt, // No alert priority
                                                         std::nullopt, // No name object
                                                         std::nullopt  // No calculation period
                                                     } };

        auto dataChannelListPackage = datachannel::DataChannelListPackage{ datachannel::Package{
            datachannel::Header{
                shipId,       // Ship ID
                configRef,    // Config reference
                std::nullopt, // No data config
                std::nullopt, // No created date
                std::nullopt, // No modified date
                std::nullopt  // No author
            },
            datachannel::DataChannelList{ std::vector<datachannel::DataChannel>{ dataChannel } } } };

        // Create TimeSeriesData
        auto timeSeriesData = timeseries::TimeSeriesData{
            timeseries::ConfigurationReference{ "DataChannelList.xml", DateTimeOffset{ "2024-12-30T00:00:00Z" } },
            std::vector<timeseries::TabularData>{ timeseries::TabularData{
                std::vector<timeseries::DataChannelId>{ timeseries::DataChannelId::fromString( "Sensor" ).value() },
                std::vector<timeseries::TabularDataSet>{ timeseries::TabularDataSet{
                    DateTimeOffset{ "2024-12-30T10:00:00Z" }, // Timestamp
                    std::vector<std::string>{ "999.1" },      // High pressure value (near limit)
                    std::nullopt                              // No quality codes
                } } } },
            timeseries::EventData{ std::vector<timeseries::EventDataSet>{ timeseries::EventDataSet{
                DateTimeOffset{ "2024-12-30T11:00:00Z" },                  // Event timestamp
                timeseries::DataChannelId::fromString( "Sensor" ).value(), // Channel ID
                "300.5",                                                   // Pressure value
                std::string{ "0" }                                         // Quality code (good)
            } } },
            std::nullopt
        };

        auto onTabularData = []( const DateTimeOffset& timestamp,
                                 const datachannel::DataChannel& channel,
                                 const transport::Value& value,
                                 [[maybe_unused]] const std::optional<std::string>& qualityCode ) {
            std::cout << "  Custom business rule - tabular data:\n";
            std::cout << "    Value: " << value.toString() << " " << channel.property().unit()->unitSymbol() << "\n";
            std::cout << "    Range: [" << channel.property().range()->low() << ", "
                      << channel.property().range()->high() << "]\n";
            std::cout << "    Time : " << timestamp.toString() << "\n";

            // Example: Check if value is near upper limit
            if( auto dec = value.decimal() )
            {
                if( *dec > channel.property().range()->high() * 0.9 )
                    std::cout << "    [!] Warning: Value approaching upper limit!\n";
            }
            return ValidateResult<>::ok();
        };

        auto onEventData = []( const DateTimeOffset& timestamp,
                               const datachannel::DataChannel& channel,
                               const transport::Value& value,
                               [[maybe_unused]] const std::optional<std::string>& qualityCode ) {
            std::cout << "  Custom business rule - event data:\n";
            std::cout << "    Value: " << value.toString() << " " << channel.property().unit()->unitSymbol() << "\n";
            std::cout << "    Range: [" << channel.property().range()->low() << ", "
                      << channel.property().range()->high() << "]\n";
            std::cout << "    Time : " << timestamp.toString() << "\n";
            return ValidateResult<>::ok();
        };

        // Validate with custom rule
        auto result = timeSeriesData.validate( dataChannelListPackage, onTabularData, onEventData );

        std::cout << "Custom validation result:\n";
        std::cout << "  Is valid: " << std::boolalpha << result.isOk() << "\n";
        if( !result.isOk() )
        {
            std::cout << "  Errors:\n";
            for( const auto& error : result.errors )
            {
                std::cout << "    - " << error << "\n";
            }
        }

        std::cout << "\n";
    }

    //=====================================================================
    // 9. CustomHeaders & CustomDataKinds: Extension data support
    //=====================================================================
    {
        std::cout << "9. CustomHeaders & CustomDataKinds: Extension data support\n";
        std::cout << "-------------------------------------------------------------\n";

        // Create CustomHeaders with various Value types
        timeseries::CustomHeaders customHeaders;
        customHeaders.set( "version", std::string( "1.2.3" ) );
        customHeaders.set( "recordCount", 42 );
        customHeaders.set( "isCompressed", true );
        customHeaders.set( "temperature", 98.6 );
        customHeaders.set( "createdAt", std::string( "2024-12-30T10:00:00Z" ) );

        // Create Header with CustomHeaders
        auto header = timeseries::Header{ ShipId::fromString( "IMO1234567" ).value(),
                                          std::nullopt,
                                          DateTimeOffset{ "2024-12-30T10:00:00Z" },
                                          std::nullopt,
                                          "CustomData Demo",
                                          std::nullopt,
                                          customHeaders };

        // Create CustomDataKinds for TimeSeriesData
        timeseries::CustomData customDataKinds;
        customDataKinds.set( "sensorModel", std::string( "TempSensor-XYZ-2000" ) );
        customDataKinds.set( "calibrationDate", std::string( "2024-01-15T00:00:00Z" ) );
        customDataKinds.set( "sampleRate", 1000 );
        customDataKinds.set( "accuracy", 0.001 );
        customDataKinds.set( "validated", true );

        // Create TimeSeriesData with CustomDataKinds
        auto dataConfig =
            timeseries::ConfigurationReference{ "DataChannelList.xml", DateTimeOffset{ "2024-12-30T00:00:00Z" } };

        auto channelId = timeseries::DataChannelId::fromString( "Sensor1" ).value();
        auto tabularData = timeseries::TabularData{
            std::vector<timeseries::DataChannelId>{ channelId },
            std::vector<timeseries::TabularDataSet>{ timeseries::TabularDataSet{
                DateTimeOffset{ "2024-12-30T10:00:00Z" }, std::vector<std::string>{ "100.0" }, std::nullopt } }
        };

        auto timeSeriesData = timeseries::TimeSeriesData{
            dataConfig, std::vector<timeseries::TabularData>{ tabularData }, std::nullopt, customDataKinds
        };

        // Create package
        auto package = timeseries::Package{ header, std::vector<timeseries::TimeSeriesData>{ timeSeriesData } };

        auto tsPackage = timeseries::TimeSeriesDataPackage{ package };

        std::cout << "Created package with custom extension data\n";

        // Serialize to JSON with pretty print
        auto options = serialization::json::Serializer<timeseries::TimeSeriesDataPackage>::Options{};
        options.includeNullFields = false;
        options.prettyPrint = true;

        auto json = serialization::json::Serializer<timeseries::TimeSeriesDataPackage>::toString( tsPackage, options );

        std::cout << "\nSerialized JSON with custom data:\n";
        std::cout << json << "\n";

        std::cout << "\n";
    }

    return 0;
}
