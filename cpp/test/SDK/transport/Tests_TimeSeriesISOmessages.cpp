/**
 * @file Tests_TimeSeriesISOmessages .cpp
 * @brief Unit tests for TimeSeries infrastructure
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/Transport.h>

namespace dnv::vista::sdk::tests
{
    //=====================================================================
    // Test data
    //=====================================================================

    /**
     * @brief Creates ValidFullyCustomDataChannelList
     */
    transport::datachannel::DataChannelListPackage createValidFullyCustomDataChannelList()
    {
        // Create ConfigurationReference for DataChannelListId
        auto timeStamp = DateTimeOffset{ "2016-01-01T00:00:00Z" };
        transport::datachannel::ConfigurationReference dataChannelListId{ "DataChannelList.xml", timeStamp, "1.0" };

        // Create Header
        transport::datachannel::VersionInformation versionInfo{ "some_naming_rule", "2.0", "http://somewhere.net" };

        transport::datachannel::Header header{
            ShipId::fromString( "IMO1234567" ).value(), // ShipId (required)
            dataChannelListId,                          // ConfigurationReference (required)
            versionInfo,                                // VersionInformation (optional)
            "Author1",                                  // Author (optional)
            DateTimeOffset{ "2015-12-01T00:00:00Z" },   // Creation date (optional)
            std::nullopt                                // customHeaders
        };

        // Create DataChannelList
        transport::datachannel::DataChannelList dataChannelList;

        // First DataChannel - Temperature sensor
        {
            auto localIdOpt = LocalIdBuilder::fromString(
                "/dnv-v2/vis-3-4a/411.1-1/C101.63/S206/meta/qty-temperature/cnt-cooling.air" );
            EXPECT_TRUE( localIdOpt.has_value() );
            auto localId = *localIdOpt;

            transport::datachannel::DataChannelId dataChannelId{ localId, "0010" };

            transport::datachannel::NameObject nameObject;
            nameObject.setNamingRule( "Naming_Rule" );
            dataChannelId.setNameObject( nameObject );

            transport::datachannel::DataChannelType dataChannelType{ "Inst" };
            dataChannelType.setUpdateCycle( 1.0 );

            transport::datachannel::Format format{ "Decimal" };
            transport::datachannel::Restriction restriction;
            restriction.setFractionDigits( 1 );
            restriction.setMaxInclusive( 200.0 );
            restriction.setMinInclusive( -150.0 );
            format.setRestriction( restriction );

            transport::datachannel::Range range{ 0.0, 150.0 };

            transport::datachannel::Unit unit{ "°C", "Temperature" };

            transport::datachannel::Property property{
                dataChannelType,
                format,
                range,
                unit,
                std::string{ "OPC_QUALITY" },
                std::nullopt, // alertPriority
                std::string{ "M/E #1 Air Cooler CFW OUT Temp" },
                std::string{ " Location: ECR, Manufacturer: AAA Company, Type: TYPE-AAA " },
                std::nullopt // customProperties
            };

            transport::datachannel::DataChannel dataChannel{ dataChannelId, property };
            dataChannelList.add( dataChannel );
        }

        // Second DataChannel - Alert type
        {
            auto localIdOpt = LocalIdBuilder::fromString( "/dnv-v2/vis-3-4a/511.15-1/E32/meta/qty-power" );
            EXPECT_TRUE( localIdOpt.has_value() );
            auto localId = *localIdOpt;

            transport::datachannel::DataChannelId dataChannelId{ localId, "0020" };

            transport::datachannel::DataChannelType dataChannelType{ "Alert" };

            transport::datachannel::Format format{ "String" };
            transport::datachannel::Restriction restriction;
            restriction.setMaxLength( 100 );
            restriction.setMinLength( 0 );
            format.setRestriction( restriction );

            transport::datachannel::Property property{
                dataChannelType,
                format,
                std::nullopt,             // range
                std::nullopt,             // unit
                std::nullopt,             // qualityCoding
                std::string{ "Warning" }, // alertPriority
                std::nullopt,             // name
                std::nullopt,             // remarks
                std::nullopt              // customProperties
            };

            transport::datachannel::DataChannel dataChannel{ dataChannelId, property };
            dataChannelList.add( dataChannel );
        }

        transport::datachannel::Package package{ header, std::move( dataChannelList ) };

        return transport::datachannel::DataChannelListPackage{ std::move( package ) };
    }

    /**
     * @brief Creates TestTimeSeriesDataPackage
     */
    transport::timeseries::TimeSeriesDataPackage createTestTimeSeriesDataPackage()
    {
        auto testDataChannelListPackage = createValidFullyCustomDataChannelList();

        // Create data channel IDs from test package
        std::vector<transport::timeseries::DataChannelId> allDataChannelIds;
        for( const auto& dc : testDataChannelListPackage.dataChannelList().dataChannels() )
        {
            auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString(
                dc.dataChannelId().shortId().has_value() ? *dc.dataChannelId().shortId()
                                                         : dc.dataChannelId().localId().toString() );
            if( dataChannelIdOpt.has_value() )
            {
                allDataChannelIds.push_back( *dataChannelIdOpt );
            }
        }
        std::vector<transport::timeseries::DataChannelId> firstDataChannelId;
        firstDataChannelId.push_back( allDataChannelIds[0] );

        // Create timestamps
        auto start = DateTimeOffset{ "2016-01-01T12:00:00Z" };
        auto end = DateTimeOffset{ "2016-01-03T12:00:00Z" };
        auto timestamp1 = DateTimeOffset{ "2016-01-01T12:00:00Z" };
        auto timestamp2 = DateTimeOffset{ "2016-01-02T12:00:00Z" };
        auto timestamp3 = DateTimeOffset{ "2016-01-03T12:00:00Z" };
        auto eventTimestamp = DateTimeOffset{ "2016-01-01T12:00:01Z" };
        auto createdModified = DateTimeOffset{ "2016-01-03T12:00:00Z" };
        auto configTimestamp = DateTimeOffset{ "2016-01-03T00:00:00Z" };

        // Create first tabular data
        std::vector<transport::timeseries::TabularDataSet> dataSets1 = {
            transport::timeseries::TabularDataSet(
                timestamp1, std::vector<std::string>{ "100.0", "200.0" }, std::vector<std::string>{ "0", "0" } ),
            transport::timeseries::TabularDataSet(
                timestamp2, std::vector<std::string>{ "105.0", "210.0" }, std::vector<std::string>{ "0", "0" } )
        };

        transport::timeseries::TabularData tabularData1( allDataChannelIds, dataSets1 );

        // Create second tabular data
        std::vector<transport::timeseries::TabularDataSet> dataSets2 = {
            transport::timeseries::TabularDataSet( timestamp1, std::vector<std::string>{ "100.0" }, std::nullopt ),
            transport::timeseries::TabularDataSet( timestamp2, std::vector<std::string>{ "100.1" }, std::nullopt ),
            transport::timeseries::TabularDataSet( timestamp3, std::vector<std::string>{ "100.2" }, std::nullopt )
        };

        transport::timeseries::TabularData tabularData2( firstDataChannelId, dataSets2 );

        // Create event data
        std::vector<transport::timeseries::EventDataSet> eventDataSets{ transport::timeseries::EventDataSet{
            eventTimestamp, allDataChannelIds[1], "HIGH", std::string{ "0" } } };

        transport::timeseries::EventDataSet{ eventTimestamp, allDataChannelIds[1], "LOW", std::string{ "0" } };

        transport::timeseries::EventDataSet{ eventTimestamp, allDataChannelIds[1], "AVERAGE", std::string{ "0" } };

        transport::timeseries::EventData eventData{ eventDataSets };

        // Create configuration reference
        transport::timeseries::ConfigurationReference dataConfig{ "DataChannelList.xml",
                                                                  DateTimeOffset{ "2016-01-01T00:00:00Z" } };

        // Create system configuration references
        std::vector<transport::timeseries::ConfigurationReference> systemConfigs{
            transport::timeseries::ConfigurationReference( "SystemConfiguration.xml", configTimestamp ),
            transport::timeseries::ConfigurationReference( "SystemConfiguration.xml", configTimestamp )
        };

        // Create time span
        transport::timeseries::TimeSpan timeSpan( start, end );

        // Create CustomDataKinds with various types
        transport::timeseries::CustomData customDataKinds1;
        customDataKinds1.set( "dataQuality", std::string( "high" ) );
        customDataKinds1.set( "recordCount", 42 );
        customDataKinds1.set( "validated", true );
        customDataKinds1.set( "accuracy", 0.01 );
        customDataKinds1.set( "processingTime", std::string( "2024-01-15T10:30:00Z" ) );

        // Create first time series data
        transport::timeseries::TimeSeriesData timeSeriesData1(
            dataConfig,
            std::vector<transport::timeseries::TabularData>{ tabularData1, tabularData2 },
            eventData,
            customDataKinds1 );

        // Create CustomDataKinds for second time series data
        transport::timeseries::CustomData customDataKinds2;
        customDataKinds2.set( "source", std::string( "sensor_array_1" ) );
        customDataKinds2.set( "version", 2 );

        // Create second time series data
        transport::timeseries::TimeSeriesData timeSeriesData2(
            dataConfig,
            std::vector<transport::timeseries::TabularData>{ tabularData1, tabularData2 },
            eventData,
            customDataKinds2 );

        // Create CustomHeaders with various types
        transport::timeseries::CustomHeaders customHeaders;
        customHeaders.set( "temperatureUnit", std::string( "Celsius" ) );
        customHeaders.set( "sampleRate", 1000 );
        customHeaders.set( "isCompressed", true );
        customHeaders.set( "precision", 0.001 );
        customHeaders.set( "calibrationDate", std::string( "2024-01-15T00:00:00Z" ) );

        // Create header
        transport::timeseries::Header header{ ShipId::fromString( "IMO1234567" ).value(),
                                              timeSpan,
                                              createdModified,
                                              createdModified,
                                              "Shipboard data server",
                                              systemConfigs,
                                              customHeaders };

        // Create package
        transport::timeseries::Package package{
            header, std::vector<transport::timeseries::TimeSeriesData>{ timeSeriesData1, timeSeriesData2 }
        };

        return transport::timeseries::TimeSeriesDataPackage{ package };
    }

    //=====================================================================
    // Time series tests
    //=====================================================================

    TEST( IsoMessageTests, Test_TimeSeriesData )
    {
        auto message = createTestTimeSeriesDataPackage();
        EXPECT_FALSE( message.package().timeSeriesData().empty() );
    }

    TEST( IsoMessageTests, Test_TimeSeriesData_Validation )
    {
        auto message = createTestTimeSeriesDataPackage();

        for( const auto& tsData : message.package().timeSeriesData() )
        {
            EXPECT_TRUE( tsData.tabularData().has_value() );
            if( tsData.tabularData().has_value() )
            {
                const auto& tabularDataList = *tsData.tabularData();
                EXPECT_FALSE( tabularDataList.empty() );

                for( const auto& tabularData : tabularDataList )
                {
                    EXPECT_FALSE( tabularData.dataChannelIds().empty() );
                    EXPECT_FALSE( tabularData.dataSets().empty() );
                }
            }

            EXPECT_TRUE( tsData.eventData().has_value() );
            if( tsData.eventData().has_value() )
            {
                const auto& eventData = *tsData.eventData();
                EXPECT_TRUE( eventData.dataSet().has_value() );
                EXPECT_FALSE( eventData.dataSet()->empty() );
            }
        }
    }

    /**
     * @brief Test TimeSpan validation and construction
     * @details Tests TimeSpan class validation rules similar to DataChannel Range tests
     */
    TEST( TimeSeriesTests, Test_TimeSpan_Validation )
    {
        auto start = DateTimeOffset{ "2016-01-01T12:00:00Z" };
        auto end = DateTimeOffset{ "2016-01-03T12:00:00Z" };

        // Test valid range construction
        transport::timeseries::TimeSpan timeSpan( start, end );
        EXPECT_EQ( timeSpan.start().toString(), start.toString() );
        EXPECT_EQ( timeSpan.end().toString(), end.toString() );

        // Test setting range values
        auto newStart = DateTimeOffset{ "2016-01-02T12:00:00Z" };
        timeSpan.setStart( newStart );
        EXPECT_EQ( timeSpan.start().toString(), newStart.toString() );

        auto newEnd = DateTimeOffset{ "2016-01-04T12:00:00Z" };
        timeSpan.setEnd( newEnd );
        EXPECT_EQ( timeSpan.end().toString(), newEnd.toString() );

        // Test invalid range: start > end should throw
        auto invalidEnd = DateTimeOffset{ "2016-01-01T12:00:00Z" };
        EXPECT_THROW( timeSpan.setEnd( invalidEnd ), std::invalid_argument );

        auto invalidStart = DateTimeOffset{ "2016-01-05T12:00:00Z" };
        EXPECT_THROW( timeSpan.setStart( invalidStart ), std::invalid_argument );
    }

    /**
     * @brief Test TabularData validation functionality
     * @details Tests TabularData validation similar to DataChannel Property validation
     */
    TEST( TimeSeriesTests, Test_TabularData_Validation )
    {
        // Create valid DataChannelIds
        std::vector<transport::timeseries::DataChannelId> dataChannelIds;
        auto id1 = transport::timeseries::DataChannelId::fromString( "0010" );
        auto id2 = transport::timeseries::DataChannelId::fromString( "0020" );
        if( id1.has_value() )
            dataChannelIds.push_back( *id1 );
        if( id2.has_value() )
            dataChannelIds.push_back( *id2 );

        // Create valid TabularDataSets
        auto timestamp = DateTimeOffset{ "2016-01-01T12:00:00Z" };
        std::vector<transport::timeseries::TabularDataSet> dataSets = { transport::timeseries::TabularDataSet(
            timestamp, std::vector<std::string>{ "100.0", "200.0" }, std::vector<std::string>{ "0", "0" } ) };

        transport::timeseries::TabularData tabularData( dataChannelIds, dataSets );

        // Test validation
        auto instanceResult = tabularData.validate();
        EXPECT_TRUE( instanceResult.isOk() );
    }

    /**
     * @brief Test EventData collection operations
     * @details Tests EventData construction and numberOfDataSet
     */
    TEST( TimeSeriesTests, Test_EventData_Operations )
    {
        // Test default constructor
        transport::timeseries::EventData eventData;
        EXPECT_FALSE( eventData.dataSet().has_value() );
        EXPECT_EQ( eventData.numberOfDataSet(), 0U );

        // Create event data sets
        auto timestamp = DateTimeOffset{ "2016-01-01T12:00:01Z" };
        auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString( "0010" );
        EXPECT_TRUE( dataChannelIdOpt.has_value() );

        std::vector<transport::timeseries::EventDataSet> dataSets;
        dataSets.push_back(
            transport::timeseries::EventDataSet{ timestamp, *dataChannelIdOpt, "HIGH", std::string{ "0" } } );

        eventData.setDataSet( dataSets );
        EXPECT_TRUE( eventData.dataSet().has_value() );
        EXPECT_EQ( eventData.numberOfDataSet(), 1U );
        EXPECT_FALSE( eventData.dataSet()->empty() );

        // Add another event
        dataSets.push_back(
            transport::timeseries::EventDataSet{ timestamp, *dataChannelIdOpt, "LOW", std::string{ "0" } } );

        eventData.setDataSet( dataSets );
        EXPECT_EQ( eventData.numberOfDataSet(), 2U );

        // Test clear
        eventData.setDataSet( std::nullopt );
        EXPECT_FALSE( eventData.dataSet().has_value() );
        EXPECT_EQ( eventData.numberOfDataSet(), 0U );
    }

    /**
     * @brief Test ConfigurationReference validation
     * @details Tests ConfigurationReference construction and property access
     */
    TEST( ValidationTests, Test_ConfigurationReference_Validation )
    {
        auto timestamp = DateTimeOffset{ "2016-01-01T00:00:00Z" };

        transport::timeseries::ConfigurationReference configRef( "TestConfig.xml", timestamp );

        EXPECT_EQ( configRef.id(), "TestConfig.xml" );
        EXPECT_EQ( configRef.timeStamp().toString(), timestamp.toString() );

        // Test setters
        configRef.setId( "NewConfig.xml" );
        EXPECT_EQ( configRef.id(), "NewConfig.xml" );

        auto newTimestamp = DateTimeOffset{ "2016-01-02T00:00:00Z" };
        configRef.setTimeStamp( newTimestamp );
        EXPECT_EQ( configRef.timeStamp().toString(), newTimestamp.toString() );
    }

    /**
     * @brief Test TabularDataSet validation
     * @details Tests TabularDataSet construction and property validation
     */
    TEST( ValidationTests, Test_TabularDataSet_Validation )
    {
        auto timestamp = DateTimeOffset{ "2016-01-01T12:00:00Z" };
        std::vector<std::string> values = { "100.0", "200.0" };
        std::vector<std::string> quality = { "0", "0" };

        transport::timeseries::TabularDataSet dataSet( timestamp, values, quality );

        EXPECT_EQ( dataSet.timeStamp().toString(), timestamp.toString() );
        EXPECT_EQ( dataSet.value().size(), 2U );
        EXPECT_EQ( dataSet.value()[0], "100.0" );
        EXPECT_EQ( dataSet.value()[1], "200.0" );
        EXPECT_TRUE( dataSet.quality().has_value() );
        EXPECT_EQ( dataSet.quality()->size(), 2U );
        EXPECT_EQ( ( *dataSet.quality() )[0], "0" );
        EXPECT_EQ( ( *dataSet.quality() )[1], "0" );

        // Test without quality
        transport::timeseries::TabularDataSet dataSetNoQuality( timestamp, values );
        EXPECT_FALSE( dataSetNoQuality.quality().has_value() );
    }

    /**
     * @brief Test EventDataSet validation
     * @details Tests EventDataSet construction and property validation
     */
    TEST( ValidationTests, Test_EventDataSet_Validation )
    {
        auto timestamp = DateTimeOffset{ "2016-01-01T12:00:01Z" };
        auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString( "0010" );
        EXPECT_TRUE( dataChannelIdOpt.has_value() );

        transport::timeseries::EventDataSet eventDataSet( timestamp, *dataChannelIdOpt, "HIGH", std::string{ "0" } );

        EXPECT_EQ( eventDataSet.timeStamp().toString(), timestamp.toString() );
        EXPECT_EQ( eventDataSet.dataChannelId().toString(), dataChannelIdOpt->toString() );
        EXPECT_EQ( eventDataSet.value(), "HIGH" );
        EXPECT_TRUE( eventDataSet.quality().has_value() );
        EXPECT_EQ( *eventDataSet.quality(), "0" );

        // Test without quality
        transport::timeseries::EventDataSet eventDataSetNoQuality( timestamp, *dataChannelIdOpt, "LOW" );
        EXPECT_FALSE( eventDataSetNoQuality.quality().has_value() );
    }

    /**
     * @brief Test TimeSpan error scenarios
     * @details Tests TimeSpan error conditions similar to DataChannel Range error tests
     */
    TEST( ErrorHandlingTests, Test_TimeSpan_Error_Handling )
    {
        auto start = DateTimeOffset{ "2016-01-03T12:00:00Z" };
        auto end = DateTimeOffset{ "2016-01-01T12:00:00Z" };

        // Test invalid range construction: start > end should throw
        EXPECT_THROW( transport::timeseries::TimeSpan( start, end ), std::invalid_argument );

        // Test valid construction first
        auto validStart = DateTimeOffset{ "2016-01-01T12:00:00Z" };
        auto validEnd = DateTimeOffset{ "2016-01-03T12:00:00Z" };
        transport::timeseries::TimeSpan timeSpan( validStart, validEnd );

        // Test invalid setters
        auto invalidStart = DateTimeOffset{ "2016-01-04T12:00:00Z" };
        EXPECT_THROW( timeSpan.setStart( invalidStart ), std::invalid_argument );

        auto invalidEnd = DateTimeOffset{ "2015-12-31T12:00:00Z" };
        EXPECT_THROW( timeSpan.setEnd( invalidEnd ), std::invalid_argument );
    }

    /**
     * @brief Test TabularData mismatched dimensions
     * @details Tests TabularData with mismatched channel IDs and data columns
     */
    TEST( ErrorHandlingTests, Test_TabularData_Dimension_Mismatch )
    {
        // Create 2 DataChannelIds but data with 3 columns - should fail validation
        std::vector<transport::timeseries::DataChannelId> dataChannelIds;
        auto id1 = transport::timeseries::DataChannelId::fromString( "0010" );
        auto id2 = transport::timeseries::DataChannelId::fromString( "0020" );
        if( id1.has_value() )
            dataChannelIds.push_back( *id1 );
        if( id2.has_value() )
            dataChannelIds.push_back( *id2 );

        // 3 values but only 2 channels
        auto timestamp = DateTimeOffset{ "2016-01-01T12:00:00Z" };
        std::vector<transport::timeseries::TabularDataSet> dataSets = { transport::timeseries::TabularDataSet(
            timestamp,
            std::vector<std::string>{ "100.0", "200.0", "300.0" },
            std::vector<std::string>{ "0", "0", "0" } ) };

        transport::timeseries::TabularData tabularData( dataChannelIds, dataSets );

        // Test validation
        auto instanceResult = tabularData.validate();
        EXPECT_FALSE( instanceResult.isOk() );
    }

    /**
     * @brief Test Package with invalid TimeSeriesData
     * @details Tests Package error handling with malformed data
     */
    TEST( ErrorHandlingTests, Test_Package_Error_Handling )
    {
        // Test Package with empty TimeSeriesData vector
        std::vector<transport::timeseries::TimeSeriesData> emptyTimeSeriesData;

        // Create minimal header
        transport::timeseries::Header header{
            ShipId::fromString( "IMO1234567" ).value(),
            std::nullopt, // no time span
            std::nullopt, // no date created
            std::nullopt, // no date modified
            std::nullopt, // no author
            std::nullopt, // no system configuration
            std::nullopt  // no custom headers
        };

        // This should be allowed but worth testing
        transport::timeseries::Package package( header, emptyTimeSeriesData );
        EXPECT_TRUE( package.timeSeriesData().empty() );
        EXPECT_TRUE( package.header().has_value() );
    }

    /**
     * @brief Test valid cross-validation between TimeSeriesData and DataChannelList
     * @details Verifies that TimeSeriesData with valid DataChannelIds and values passes validation
     */
    TEST( CrossValidationTests, Valid_TimeSeriesData_Against_DataChannelList )
    {
        auto dataChannelListPackage = createValidFullyCustomDataChannelList();
        auto timeSeriesDataPackage = createTestTimeSeriesDataPackage();

        // Get first TimeSeriesData
        const auto& tsData = timeSeriesDataPackage.package().timeSeriesData()[0];

        // Simple validation callbacks that accept everything
        auto onTabularData = []( const auto&, const auto&, const auto&, const auto& ) {
            return transport::ValidateResult<>::ok();
        };
        auto onEventData = []( const auto&, const auto&, const auto&, const auto& ) {
            return transport::ValidateResult<>::ok();
        };

        // Validate - should succeed
        auto result = tsData.validate( dataChannelListPackage, onTabularData, onEventData );

        EXPECT_TRUE( result.isOk() );
    }

    /**
     * @brief Test validation fails with non-existent DataChannelId
     * @details Verifies that referencing a DataChannelId that doesn't exist in DataChannelList fails
     */
    TEST( CrossValidationTests, Invalid_DataChannelId_Not_Found )
    {
        auto dataChannelListPackage = createValidFullyCustomDataChannelList();

        // Create TimeSeriesData with non-existent DataChannelId
        auto timestamp = DateTimeOffset{ "2016-01-01T12:00:00Z" };
        auto invalidChannelIdOpt = transport::timeseries::DataChannelId::fromString( "9999" );
        ASSERT_TRUE( invalidChannelIdOpt.has_value() );

        std::vector<transport::timeseries::DataChannelId> channelIds{ *invalidChannelIdOpt };
        std::vector<transport::timeseries::TabularDataSet> dataSets{ transport::timeseries::TabularDataSet(
            timestamp, std::vector<std::string>{ "100.0" }, std::nullopt ) };

        transport::timeseries::TabularData tabularData{ channelIds, dataSets };

        transport::timeseries::ConfigurationReference dataConfig{ "DataChannelList.xml",
                                                                  DateTimeOffset{ "2016-01-01T00:00:00Z" } };

        transport::timeseries::TimeSeriesData tsData{
            dataConfig, std::vector<transport::timeseries::TabularData>{ tabularData }, std::nullopt, std::nullopt
        };

        auto onTabularData = []( const auto&, const auto&, const auto&, const auto& ) {
            return transport::ValidateResult<>::ok();
        };
        auto onEventData = []( const auto&, const auto&, const auto&, const auto& ) {
            return transport::ValidateResult<>::ok();
        };

        // Validate - should fail
        auto result = tsData.validate( dataChannelListPackage, onTabularData, onEventData );
        EXPECT_FALSE( result.isOk() );
        EXPECT_FALSE( result.errors.empty() );
        EXPECT_TRUE( result.errors[0].find( "not found" ) != std::string::npos );
    }

    /**
     * @brief Test validation fails with invalid value format
     * @details Verifies that values not matching the declared Format fail validation
     */
    TEST( CrossValidationTests, Invalid_Value_Format_Mismatch )
    {
        auto dataChannelListPackage = createValidFullyCustomDataChannelList();

        // Get the first DataChannel (Decimal format, expects numeric values)
        const auto& firstChannel = dataChannelListPackage.dataChannelList().dataChannels()[0];
        auto channelIdOpt = transport::timeseries::DataChannelId::fromString(
            firstChannel.dataChannelId().shortId().has_value() ? *firstChannel.dataChannelId().shortId()
                                                               : firstChannel.dataChannelId().localId().toString() );
        ASSERT_TRUE( channelIdOpt.has_value() );

        // Create TimeSeriesData with invalid value (string for decimal format)
        auto timestamp = DateTimeOffset{ "2016-01-01T12:00:00Z" };
        std::vector<transport::timeseries::DataChannelId> channelIds{ *channelIdOpt };
        std::vector<transport::timeseries::TabularDataSet> dataSets{ transport::timeseries::TabularDataSet(
            timestamp, std::vector<std::string>{ "invalid_not_a_number" }, std::nullopt ) };

        transport::timeseries::TabularData tabularData{ channelIds, dataSets };

        transport::timeseries::ConfigurationReference dataConfig{ "DataChannelList.xml",
                                                                  DateTimeOffset{ "2016-01-01T00:00:00Z" } };

        transport::timeseries::TimeSeriesData tsData{
            dataConfig, std::vector<transport::timeseries::TabularData>{ tabularData }, std::nullopt, std::nullopt
        };

        auto onTabularData = []( const auto&, const auto&, const auto&, const auto& ) {
            return transport::ValidateResult<>::ok();
        };
        auto onEventData = []( const auto&, const auto&, const auto&, const auto& ) {
            return transport::ValidateResult<>::ok();
        };

        // Validate - should fail
        auto result = tsData.validate( dataChannelListPackage, onTabularData, onEventData );
        EXPECT_FALSE( result.isOk() );
        EXPECT_FALSE( result.errors.empty() );
        EXPECT_TRUE( result.errors[0].find( "invalid value" ) != std::string::npos );
    }

    /**
     * @brief Test validation fails with mismatched DataConfiguration ID
     * @details Verifies that DataConfiguration.id must match DataChannelList.id
     */
    TEST( CrossValidationTests, Invalid_DataConfiguration_Id_Mismatch )
    {
        auto dataChannelListPackage = createValidFullyCustomDataChannelList();
        auto timeSeriesDataPackage = createTestTimeSeriesDataPackage();

        // Create TimeSeriesData with wrong DataConfiguration ID
        const auto& originalTsData = timeSeriesDataPackage.package().timeSeriesData()[0];

        transport::timeseries::ConfigurationReference wrongDataConfig{ "WrongDataChannelList.xml",
                                                                       DateTimeOffset{ "2016-01-01T00:00:00Z" } };

        transport::timeseries::TimeSeriesData tsData{
            wrongDataConfig, originalTsData.tabularData(), originalTsData.eventData(), std::nullopt
        };

        auto onTabularData = []( const auto&, const auto&, const auto&, const auto& ) {
            return transport::ValidateResult<>::ok();
        };
        auto onEventData = []( const auto&, const auto&, const auto&, const auto& ) {
            return transport::ValidateResult<>::ok();
        };

        // Validate - should fail
        auto result = tsData.validate( dataChannelListPackage, onTabularData, onEventData );
        EXPECT_FALSE( result.isOk() );
        EXPECT_FALSE( result.errors.empty() );
        EXPECT_TRUE( result.errors[0].find( "does not match" ) != std::string::npos );
    }

    /**
     * @brief Test validation fails with empty TimeSeriesData
     * @details Verifies that TimeSeriesData must contain either TabularData or EventData
     */
    TEST( CrossValidationTests, Invalid_Empty_TimeSeriesData )
    {
        auto dataChannelListPackage = createValidFullyCustomDataChannelList();

        // Create empty TimeSeriesData (no TabularData, no EventData)
        transport::timeseries::ConfigurationReference dataConfig{ "DataChannelList.xml",
                                                                  DateTimeOffset{ "2016-01-01T00:00:00Z" } };

        transport::timeseries::TimeSeriesData tsData{ dataConfig, std::nullopt, std::nullopt, std::nullopt };

        auto onTabularData = []( const auto&, const auto&, const auto&, const auto& ) {
            return transport::ValidateResult<>::ok();
        };
        auto onEventData = []( const auto&, const auto&, const auto&, const auto& ) {
            return transport::ValidateResult<>::ok();
        };

        // Validate - should fail
        auto result = tsData.validate( dataChannelListPackage, onTabularData, onEventData );
        EXPECT_FALSE( result.isOk() );
        EXPECT_FALSE( result.errors.empty() );
        EXPECT_TRUE( result.errors[0].find( "without data" ) != std::string::npos );
    }

    /**
     * @brief Test custom validation callback rejection
     * @details Verifies that custom callbacks can reject values based on business rules
     */
    TEST( CrossValidationTests, Custom_Callback_Rejection )
    {
        auto dataChannelListPackage = createValidFullyCustomDataChannelList();
        auto timeSeriesDataPackage = createTestTimeSeriesDataPackage();

        const auto& tsData = timeSeriesDataPackage.package().timeSeriesData()[0];

        // Callback that rejects all values
        auto onTabularData = []( const auto&, const auto&, const auto&, const auto& ) {
            return transport::ValidateResult<>::invalid( { "Custom business rule violation" } );
        };
        auto onEventData = []( const auto&, const auto&, const auto&, const auto& ) {
            return transport::ValidateResult<>::ok();
        };

        // Validate - should fail due to callback rejection
        auto result = tsData.validate( dataChannelListPackage, onTabularData, onEventData );
        EXPECT_FALSE( result.isOk() );
        EXPECT_FALSE( result.errors.empty() );
        EXPECT_TRUE( result.errors[0].find( "Custom business rule violation" ) != std::string::npos );
    }

    //=====================================================================
    // JSON Serialization Tests
    //=====================================================================

    /**
     * @brief Test TimeSeriesData JSON serialization
     * @details Tests JSON round-trip serialization
     */
    TEST( IsoMessageTests, TimeSeriesDataJson )
    {
        // Create original package
        auto originalPackage = createTestTimeSeriesDataPackage();

        // Serialize domain model to JSON
        auto options = serialization::json::Serializer<transport::timeseries::TimeSeriesDataPackage>::Options{};
        options.includeNullFields = false;
        options.prettyPrint = true;
        std::string json = serialization::json::Serializer<transport::timeseries::TimeSeriesDataPackage>::toString(
            originalPackage, options );

        // Deserialize JSON to domain model
        auto docOpt = json::Document::fromString( json );
        ASSERT_TRUE( docOpt.has_value() );

        auto deserializedPackage =
            serialization::json::SerializationTraits<transport::timeseries::TimeSeriesDataPackage>::fromDocument(
                *docOpt );

        // Header
        const auto& originalHeader = originalPackage.package().header();
        const auto& deserializedHeader = deserializedPackage.package().header();

        ASSERT_TRUE( originalHeader.has_value() );
        ASSERT_TRUE( deserializedHeader.has_value() );

        // ShipId
        EXPECT_EQ( originalHeader->shipId().toString(), deserializedHeader->shipId().toString() );

        // TimeSpan
        ASSERT_TRUE( originalHeader->timeSpan().has_value() );
        ASSERT_TRUE( deserializedHeader->timeSpan().has_value() );
        EXPECT_EQ( originalHeader->timeSpan()->start().toString(), deserializedHeader->timeSpan()->start().toString() );
        EXPECT_EQ( originalHeader->timeSpan()->end().toString(), deserializedHeader->timeSpan()->end().toString() );

        // DateCreated
        ASSERT_TRUE( originalHeader->dateCreated().has_value() );
        ASSERT_TRUE( deserializedHeader->dateCreated().has_value() );
        EXPECT_EQ( originalHeader->dateCreated()->toString(), deserializedHeader->dateCreated()->toString() );

        // DateModified
        ASSERT_TRUE( originalHeader->dateModified().has_value() );
        ASSERT_TRUE( deserializedHeader->dateModified().has_value() );
        EXPECT_EQ( originalHeader->dateModified()->toString(), deserializedHeader->dateModified()->toString() );

        // Author
        ASSERT_TRUE( originalHeader->author().has_value() );
        ASSERT_TRUE( deserializedHeader->author().has_value() );
        EXPECT_EQ( *originalHeader->author(), *deserializedHeader->author() );

        // SystemConfiguration
        ASSERT_TRUE( originalHeader->systemConfiguration().has_value() );
        ASSERT_TRUE( deserializedHeader->systemConfiguration().has_value() );
        const auto& originalSysConfigs = *originalHeader->systemConfiguration();
        const auto& deserializedSysConfigs = *deserializedHeader->systemConfiguration();
        EXPECT_EQ( originalSysConfigs.size(), deserializedSysConfigs.size() );
        for( std::size_t i = 0; i < originalSysConfigs.size(); ++i )
        {
            EXPECT_EQ( originalSysConfigs[i].id(), deserializedSysConfigs[i].id() );
            EXPECT_EQ( originalSysConfigs[i].timeStamp().toString(), deserializedSysConfigs[i].timeStamp().toString() );
        }

        // CustomHeaders (verify JSON serialization)
        ASSERT_TRUE( originalHeader->customHeaders().has_value() );
        ASSERT_TRUE( deserializedHeader->customHeaders().has_value() );
        const auto& deserializedCustomHeaders = *deserializedHeader->customHeaders();

        // Verify String type
        auto tempUnit = deserializedCustomHeaders.get<std::string>( "temperatureUnit" );
        ASSERT_TRUE( tempUnit.has_value() );
        EXPECT_EQ( *tempUnit, "Celsius" );

        // Verify Integer type
        auto sampleRate = deserializedCustomHeaders.get<int64_t>( "sampleRate" );
        ASSERT_TRUE( sampleRate.has_value() );
        EXPECT_EQ( *sampleRate, 1000 );

        // Verify Boolean type
        auto isCompressed = deserializedCustomHeaders.get<bool>( "isCompressed" );
        ASSERT_TRUE( isCompressed.has_value() );
        EXPECT_TRUE( *isCompressed );

        // Verify Decimal type
        auto precision = deserializedCustomHeaders.get<double>( "precision" );
        ASSERT_TRUE( precision.has_value() );
        EXPECT_DOUBLE_EQ( *precision, 0.001 );

        // Verify DateTime type (stored as string)
        auto calDate = deserializedCustomHeaders.get<std::string>( "calibrationDate" );
        ASSERT_TRUE( calDate.has_value() );
        EXPECT_EQ( *calDate, "2024-01-15T00:00:00Z" );

        // TimeSeriesData
        const auto& originalTimeSeriesData = originalPackage.package().timeSeriesData();
        const auto& deserializedTimeSeriesData = deserializedPackage.package().timeSeriesData();

        EXPECT_EQ( originalTimeSeriesData.size(), deserializedTimeSeriesData.size() );
        ASSERT_EQ( deserializedTimeSeriesData.size(), 2U );

        for( std::size_t tsIdx = 0; tsIdx < originalTimeSeriesData.size(); ++tsIdx )
        {
            const auto& originalTsData = originalTimeSeriesData[tsIdx];
            const auto& deserializedTsData = deserializedTimeSeriesData[tsIdx];

            // DataConfiguration
            ASSERT_TRUE( originalTsData.dataConfiguration().has_value() );
            ASSERT_TRUE( deserializedTsData.dataConfiguration().has_value() );
            EXPECT_EQ( originalTsData.dataConfiguration()->id(), deserializedTsData.dataConfiguration()->id() );
            EXPECT_EQ(
                originalTsData.dataConfiguration()->timeStamp().toString(),
                deserializedTsData.dataConfiguration()->timeStamp().toString() );

            // TabularData
            ASSERT_TRUE( originalTsData.tabularData().has_value() );
            ASSERT_TRUE( deserializedTsData.tabularData().has_value() );
            const auto& originalTabularDataList = *originalTsData.tabularData();
            const auto& deserializedTabularDataList = *deserializedTsData.tabularData();
            EXPECT_EQ( originalTabularDataList.size(), deserializedTabularDataList.size() );

            for( std::size_t tabIdx = 0; tabIdx < originalTabularDataList.size(); ++tabIdx )
            {
                const auto& originalTabData = originalTabularDataList[tabIdx];
                const auto& deserializedTabData = deserializedTabularDataList[tabIdx];

                // DataChannelIds
                const auto& originalChannelIds = originalTabData.dataChannelIds();
                const auto& deserializedChannelIds = deserializedTabData.dataChannelIds();
                EXPECT_EQ( originalChannelIds.size(), deserializedChannelIds.size() );
                for( std::size_t cidIdx = 0; cidIdx < originalChannelIds.size(); ++cidIdx )
                {
                    EXPECT_EQ( originalChannelIds[cidIdx].toString(), deserializedChannelIds[cidIdx].toString() );
                }

                // DataSets
                const auto& originalDataSets = originalTabData.dataSets();
                const auto& deserializedDataSets = deserializedTabData.dataSets();
                EXPECT_EQ( originalDataSets.size(), deserializedDataSets.size() );

                for( std::size_t dsIdx = 0; dsIdx < originalDataSets.size(); ++dsIdx )
                {
                    const auto& originalDataSet = originalDataSets[dsIdx];
                    const auto& deserializedDataSet = deserializedDataSets[dsIdx];

                    // TimeStamp
                    EXPECT_EQ( originalDataSet.timeStamp().toString(), deserializedDataSet.timeStamp().toString() );

                    // Values
                    const auto& originalValues = originalDataSet.value();
                    const auto& deserializedValues = deserializedDataSet.value();
                    EXPECT_EQ( originalValues.size(), deserializedValues.size() );
                    for( std::size_t vIdx = 0; vIdx < originalValues.size(); ++vIdx )
                    {
                        EXPECT_EQ( originalValues[vIdx], deserializedValues[vIdx] );
                    }

                    // Quality
                    if( originalDataSet.quality().has_value() )
                    {
                        ASSERT_TRUE( deserializedDataSet.quality().has_value() );
                        const auto& originalQuality = *originalDataSet.quality();
                        const auto& deserializedQuality = *deserializedDataSet.quality();
                        EXPECT_EQ( originalQuality.size(), deserializedQuality.size() );
                        for( std::size_t qIdx = 0; qIdx < originalQuality.size(); ++qIdx )
                        {
                            EXPECT_EQ( originalQuality[qIdx], deserializedQuality[qIdx] );
                        }
                    }
                    else
                    {
                        EXPECT_FALSE( deserializedDataSet.quality().has_value() );
                    }
                }
            }

            // EventData
            ASSERT_TRUE( originalTsData.eventData().has_value() );
            ASSERT_TRUE( deserializedTsData.eventData().has_value() );
            const auto& originalEventData = *originalTsData.eventData();
            const auto& deserializedEventData = *deserializedTsData.eventData();

            if( originalEventData.dataSet().has_value() )
            {
                ASSERT_TRUE( deserializedEventData.dataSet().has_value() );
                const auto& originalEventDataSets = *originalEventData.dataSet();
                const auto& deserializedEventDataSets = *deserializedEventData.dataSet();
                EXPECT_EQ( originalEventDataSets.size(), deserializedEventDataSets.size() );

                for( std::size_t edIdx = 0; edIdx < originalEventDataSets.size(); ++edIdx )
                {
                    const auto& originalEventDataSet = originalEventDataSets[edIdx];
                    const auto& deserializedEventDataSet = deserializedEventDataSets[edIdx];

                    // TimeStamp
                    EXPECT_EQ(
                        originalEventDataSet.timeStamp().toString(), deserializedEventDataSet.timeStamp().toString() );

                    // DataChannelId
                    EXPECT_EQ(
                        originalEventDataSet.dataChannelId().toString(),
                        deserializedEventDataSet.dataChannelId().toString() );

                    // Value
                    EXPECT_EQ( originalEventDataSet.value(), deserializedEventDataSet.value() );

                    // Quality
                    if( originalEventDataSet.quality().has_value() )
                    {
                        ASSERT_TRUE( deserializedEventDataSet.quality().has_value() );
                        EXPECT_EQ( *originalEventDataSet.quality(), *deserializedEventDataSet.quality() );
                    }
                    else
                    {
                        EXPECT_FALSE( deserializedEventDataSet.quality().has_value() );
                    }
                }
            }
            else
            {
                EXPECT_FALSE( deserializedEventData.dataSet().has_value() );
            }

            // CustomDataKinds (verify JSON serialization)
            ASSERT_TRUE( originalTsData.customDataKinds().has_value() );
            ASSERT_TRUE( deserializedTsData.customDataKinds().has_value() );
            const auto& deserializedCustomData = *deserializedTsData.customDataKinds();

            if( tsIdx == 0 )
            {
                // First TimeSeriesData has 5 custom data entries
                // Verify String type
                auto quality = deserializedCustomData.get<std::string>( "dataQuality" );
                ASSERT_TRUE( quality.has_value() );
                EXPECT_EQ( *quality, "high" );

                // Verify Integer type
                auto recordCount = deserializedCustomData.get<int64_t>( "recordCount" );
                ASSERT_TRUE( recordCount.has_value() );
                EXPECT_EQ( *recordCount, 42 );

                // Verify Boolean type
                auto validated = deserializedCustomData.get<bool>( "validated" );
                ASSERT_TRUE( validated.has_value() );
                EXPECT_TRUE( *validated );

                // Verify Decimal type
                auto accuracy = deserializedCustomData.get<double>( "accuracy" );
                ASSERT_TRUE( accuracy.has_value() );
                EXPECT_DOUBLE_EQ( *accuracy, 0.01 );

                // Verify DateTime type (stored as string)
                auto procTime = deserializedCustomData.get<std::string>( "processingTime" );
                ASSERT_TRUE( procTime.has_value() );
                EXPECT_EQ( *procTime, "2024-01-15T10:30:00Z" );
            }
            else if( tsIdx == 1 )
            {
                // Second TimeSeriesData has 2 custom data entries
                auto source = deserializedCustomData.get<std::string>( "source" );
                ASSERT_TRUE( source.has_value() );
                EXPECT_EQ( *source, "sensor_array_1" );

                auto version = deserializedCustomData.get<int64_t>( "version" );
                ASSERT_TRUE( version.has_value() );
                EXPECT_EQ( *version, 2 );
            }
        }
    }
} // namespace dnv::vista::sdk::tests
