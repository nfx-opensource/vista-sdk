/**
 * @file Tests_DataChannelISOmessages.cpp
 * @brief Unit tests for DataChannel infrastructure
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/transport/datachannel/DataChannel.h>
#include <dnv/vista/sdk/serialization/json/DataChannelListSerializationTraits.h>
#include <nfx/serialization/json/extensions/DatatypesTraits.h>

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

        // Add custom headers using Document
        transport::datachannel::CustomHeaders customHeaders;
        customHeaders.set( "nr:CustomHeaderElement", std::string{ "Vendor specific headers" } );
        customHeaders.set( "sampleRate", 1000 );
        customHeaders.set( "isValidated", true );
        customHeaders.set( "tolerance", std::string{ "0.001" } );
        customHeaders.set( "certificationDate", std::string{ "2024-01-15T00:00:00Z" } );

        transport::datachannel::Header header{ dnv::vista::sdk::ShipId::fromString( "IMO1234567" ).value(),
                                               dataChannelListId,
                                               versionInfo,
                                               std::string{ "Author1" },
                                               DateTimeOffset{ "2015-12-01T00:00:00Z" },
                                               std::move( customHeaders ) };

        // Create DataChannelList
        transport::datachannel::DataChannelList dataChannelList;

        // First DataChannel - Temperature sensor
        {
            auto localIdOpt = dnv::vista::sdk::LocalId::fromString(
                "/dnv-v2/vis-3-4a/411.1-1/C101.63/S206/meta/qty-temperature/cnt-cooling.air" );
            EXPECT_TRUE( localIdOpt.has_value() );
            auto localId = localIdOpt.value();

            transport::datachannel::DataChannelId dataChannelId{ localId, "0010" };

            // Add custom name objects - all 5 Value types using Document
            transport::datachannel::CustomNameObjects customNameObjs;
            customNameObjs.set( "nr:CustomNameObject", std::string( "Vendor specific NameObject" ) );
            customNameObjs.set( "priority", static_cast<int64_t>( 10 ) );
            customNameObjs.set( "isActive", false );
            customNameObjs.set( "offset", Decimal{ 2.5 } );
            customNameObjs.set( "lastModified", std::string( "2024-01-10T12:30:00Z" ) );

            transport::datachannel::NameObject nameObject;
            nameObject.setNamingRule( "Naming_Rule" );
            nameObject.setCustomNameObjects( std::move( customNameObjs ) );

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

            // Custom elements for unit
            transport::datachannel::CustomElements customElements;
            customElements.set( "nr:CustomUnitElement", std::string( "Vendor specific unit element" ) );
            customElements.set( "conversionFactor", static_cast<int64_t>( 100 ) );
            customElements.set( "isMetric", false );
            customElements.set( "multiplier", Decimal::fromString( "1.8" ).value() );
            customElements.set( "standardizedDate", std::string( "2020-05-01T00:00:00.0000000Z" ) );

            transport::datachannel::Unit unit{ "°C", "Temperature", customElements };

            // Custom properties
            transport::datachannel::CustomProperties customProperties;
            customProperties.set( "nr:CustomPropertyElement", std::string( "Vendor specific property element" ) );
            customProperties.set( "maxSamples", static_cast<int64_t>( 500 ) );
            customProperties.set( "requiresCalibration", false );
            customProperties.set( "accuracy", Decimal::fromString( "0.05" ).value() );
            customProperties.set( "installationDate", std::string( "2023-06-15T08:00:00.0000000Z" ) );

            transport::datachannel::Property property{
                dataChannelType,
                format,
                range,
                unit,
                std::string{ "OPC_QUALITY" },                                              // qualityCoding
                std::nullopt,                                                              // alertPriority
                std::string{ "M/E #1 Air Cooler CFW OUT Temp" },                           // name
                std::string{ "Location: ECR, Manufacturer: AAA Company, Type: TYPE-AAA" }, // remarks
                customProperties                                                           // customProperties
            };

            transport::datachannel::DataChannel dataChannel{ dataChannelId, property };
            dataChannelList.add( dataChannel );
        }

        // Second DataChannel - Alert type
        {
            auto localIdOpt = dnv::vista::sdk::LocalId::fromString( "/dnv-v2/vis-3-4a/511.15-1/E32/meta/qty-power" );
            EXPECT_TRUE( localIdOpt.has_value() );
            auto localId = localIdOpt.value();

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
     * @brief Creates ValidDataChannelList
     */
    transport::datachannel::DataChannelListPackage createValidDataChannelList()
    {
        // Create ConfigurationReference for DataChannelListId
        transport::datachannel::ConfigurationReference dataChannelListId{ "some-id",
                                                                          DateTimeOffset{ "2016-01-01T00:00:00Z" } };

        // Create Header
        transport::datachannel::Header header{ dnv::vista::sdk::ShipId::fromString( "IMO1234567" ).value(), // ShipId
                                               dataChannelListId,            // Configuration reference
                                               std::nullopt,                 // versionInformation
                                               std::string{ "some-author" }, // Author
                                               DateTimeOffset::utcNow(),     // Creation date
                                               std::nullopt };               // customHeaders

        // Create DataChannelList
        transport::datachannel::DataChannelList dataChannelList;

        // Single DataChannel
        {
            auto localIdOpt = dnv::vista::sdk::LocalId::fromString( "/dnv-v2/vis-3-4a/511.15-1/E32/meta/qty-power" );
            EXPECT_TRUE( localIdOpt.has_value() );
            auto localId = localIdOpt.value();

            transport::datachannel::DataChannelId dataChannelId{ localId, "0010" };

            transport::datachannel::DataChannelType dataChannelType{ "Inst" };
            transport::datachannel::Format format{ "String" };

            transport::datachannel::Property property{
                dataChannelType, format,
                std::nullopt, // range
                std::nullopt, // unit
                std::nullopt, // qualityCoding
                std::nullopt, // alertPriority
                std::nullopt, // name
                std::nullopt, // remarks
                std::nullopt  // customProperties
            };

            transport::datachannel::DataChannel dataChannel{ dataChannelId, property };
            dataChannelList.add( dataChannel );
        }

        transport::datachannel::Package package{ header, std::move( dataChannelList ) };

        return transport::datachannel::DataChannelListPackage{ std::move( package ) };
    }

    //=====================================================================
    // Datachannel list tests
    //=====================================================================

    TEST( IsoMessageTests, DataChannelList )
    {
        auto message = createValidDataChannelList();
        EXPECT_FALSE( message.package().dataChannelList().size() == 0 );

        auto& first = message.package().dataChannelList().dataChannels().front();
        EXPECT_FALSE( first.dataChannelId().localId().toString().empty() );

        EXPECT_NO_THROW( (void)first.property() );
    }

    TEST( IsoMessageTests, LocalId_Lookup )
    {
        auto message = createValidDataChannelList();
        const auto& dataChannel = message.package().dataChannelList()[0];
        const auto& localId = dataChannel.dataChannelId().localId();
        const auto& lookup = message.package().dataChannelList()[localId];
        auto lookup2Opt = message.package().dataChannelList().from( localId );
        EXPECT_TRUE( lookup2Opt.has_value() );
        EXPECT_EQ( &dataChannel, &lookup );
        EXPECT_EQ( &dataChannel, &lookup2Opt->get() );
    }

    TEST( IsoMessageTests, ShortId_Lookup )
    {
        auto message = createValidDataChannelList();
        const auto& dataChannel = message.package().dataChannelList()[0];
        const auto& shortId = dataChannel.dataChannelId().shortId();
        EXPECT_TRUE( shortId.has_value() );
        const auto& lookup = message.package().dataChannelList()[*shortId];
        auto lookup2Opt = message.package().dataChannelList().from( *shortId );
        EXPECT_TRUE( lookup2Opt.has_value() );
        EXPECT_EQ( &dataChannel, &lookup );
        EXPECT_EQ( &dataChannel, &lookup2Opt->get() );
    }

    TEST( IsoMessageTests, DataChannelList_Enumerator )
    {
        auto message = createValidDataChannelList();
        const auto expectedLength = message.package().dataChannelList().dataChannels().size();
        const auto actualLength = message.package().dataChannelList().size();
        size_t counter = 0;
        EXPECT_EQ( expectedLength, actualLength );

        // Test range-based for loop iteration
        for( const auto& dc : message.package().dataChannelList() )
        {
            EXPECT_NE( &dc, nullptr );
            counter++;
        }
        EXPECT_EQ( expectedLength, counter );

        // Test iterator-based iteration
        counter = 0;
        for( auto it = message.package().dataChannelList().begin(); it != message.package().dataChannelList().end();
             ++it )
        {
            EXPECT_NE( &( *it ), nullptr );
            counter++;
        }
        EXPECT_EQ( expectedLength, counter );
    }

    /**
     * @brief Test fully populated DataChannel with all properties
     * @details Tests complex DataChannel with all optional fields populated
     */
    TEST( DataChannelTests, FullyCustomDataChannel )
    {
        auto dataChannelList = createValidFullyCustomDataChannelList();

        EXPECT_EQ( dataChannelList.package().dataChannelList().size(), 2U );

        // Test custom headers
        const auto& header = dataChannelList.package().header();
        EXPECT_TRUE( header.customHeaders().has_value() );
        const auto& customHeaders = *header.customHeaders();

        auto headerValueOpt = customHeaders.get<std::string>( "nr:CustomHeaderElement" );
        EXPECT_TRUE( headerValueOpt.has_value() );
        ASSERT_TRUE( headerValueOpt.has_value() );
        EXPECT_EQ( headerValueOpt.value(), "Vendor specific headers" );

        // Test first DataChannel (temperature sensor)
        const auto& tempChannel = dataChannelList.package().dataChannelList()[0];
        const auto& tempProperty = tempChannel.property();

        EXPECT_EQ( tempProperty.dataChannelType().type(), "Inst" );
        EXPECT_TRUE( tempProperty.dataChannelType().updateCycle().has_value() );
        EXPECT_EQ( *tempProperty.dataChannelType().updateCycle(), 1.0 );

        EXPECT_EQ( tempProperty.format().type(), "Decimal" );
        EXPECT_TRUE( tempProperty.format().restriction().has_value() );
        const auto& restriction = *tempProperty.format().restriction();
        EXPECT_TRUE( restriction.fractionDigits().has_value() );
        EXPECT_EQ( *restriction.fractionDigits(), 1U );
        EXPECT_TRUE( restriction.maxInclusive().has_value() );
        EXPECT_EQ( *restriction.maxInclusive(), 200.0 );
        EXPECT_TRUE( restriction.minInclusive().has_value() );
        EXPECT_EQ( *restriction.minInclusive(), -150.0 );

        EXPECT_TRUE( tempProperty.range().has_value() );
        const auto& range = *tempProperty.range();
        EXPECT_EQ( range.low(), 0.0 );
        EXPECT_EQ( range.high(), 150.0 );

        EXPECT_TRUE( tempProperty.unit().has_value() );
        const auto& unit = *tempProperty.unit();
        EXPECT_EQ( unit.unitSymbol(), "°C" );
        EXPECT_TRUE( unit.quantityName().has_value() );
        EXPECT_EQ( *unit.quantityName(), "Temperature" );

        EXPECT_TRUE( tempProperty.qualityCoding().has_value() );
        EXPECT_EQ( *tempProperty.qualityCoding(), "OPC_QUALITY" );

        EXPECT_TRUE( tempProperty.name().has_value() );
        EXPECT_EQ( *tempProperty.name(), "M/E #1 Air Cooler CFW OUT Temp" );

        EXPECT_TRUE( tempProperty.remarks().has_value() );
        EXPECT_EQ( *tempProperty.remarks(), "Location: ECR, Manufacturer: AAA Company, Type: TYPE-AAA" );

        // Test second DataChannel (alert type)
        const auto& alertChannel = dataChannelList.package().dataChannelList()[1];
        const auto& alertProperty = alertChannel.property();

        EXPECT_EQ( alertProperty.dataChannelType().type(), "Alert" );
        EXPECT_EQ( alertProperty.format().type(), "String" );
        EXPECT_TRUE( alertProperty.alertPriority().has_value() );
        EXPECT_EQ( *alertProperty.alertPriority(), "Warning" );

        EXPECT_TRUE( alertProperty.format().restriction().has_value() );
        const auto& stringRestriction = *alertProperty.format().restriction();
        EXPECT_TRUE( stringRestriction.maxLength().has_value() );
        EXPECT_EQ( *stringRestriction.maxLength(), 100U );
        EXPECT_TRUE( stringRestriction.minLength().has_value() );
        EXPECT_EQ( *stringRestriction.minLength(), 0U );
    }

    /**
     * @brief Test Property validation rules
     * @details Tests business rule validation in Property class
     */
    TEST( DataChannelTests, Property_Validation )
    {
        // Test valid decimal property
        {
            transport::datachannel::DataChannelType dataChannelType{ "Inst" };
            transport::datachannel::Format format{ "Decimal" };
            transport::datachannel::Range range{ 0.0, 100.0 };
            transport::datachannel::Unit unit{ "°C" };

            transport::datachannel::Property property{
                dataChannelType, format, range, unit,
                std::nullopt, // qualityCoding
                std::nullopt, // alertPriority
                std::nullopt, // name
                std::nullopt, // remarks
                std::nullopt  // customProperties
            };
            auto result = property.validate();
            EXPECT_TRUE( result.isOk() );
        }

        // Test alert property with priority
        {
            transport::datachannel::DataChannelType dataChannelType{ "Alert" };
            transport::datachannel::Format format{ "String" };

            transport::datachannel::Property property{
                dataChannelType,
                format,
                std::nullopt,              // range
                std::nullopt,              // unit
                std::nullopt,              // qualityCoding
                std::string{ "Critical" }, // alertPriority
                std::nullopt,              // name
                std::nullopt,              // remarks
                std::nullopt               // customProperties
            };
            auto result = property.validate();
            EXPECT_TRUE( result.isOk() );
        }
    }

    /**
     * @brief Test Restriction validation functionality
     * @details Tests various restriction validation scenarios
     */
    TEST( DataChannelTests, Restriction_Validation )
    {
        transport::datachannel::Restriction restriction;
        transport::datachannel::Format format{ "String" };

        // Test length validation
        restriction.setLength( 5 );
        auto result = restriction.validateValue( "hello", format );
        EXPECT_TRUE( result.isOk() );

        result = restriction.validateValue( "hi", format );
        EXPECT_FALSE( result.isOk() );

        result = restriction.validateValue( "toolong", format );
        EXPECT_FALSE( result.isOk() );

        // Test min/max length validation
        restriction = transport::datachannel::Restriction{};
        restriction.setMinLength( 2 );
        restriction.setMaxLength( 10 );

        result = restriction.validateValue( "ok", format );
        EXPECT_TRUE( result.isOk() );

        result = restriction.validateValue( "x", format );
        EXPECT_FALSE( result.isOk() );

        result = restriction.validateValue( "thisstringistoolong", format );
        EXPECT_FALSE( result.isOk() );
    }

    /**
     * @brief Test Range validation and construction
     * @details Tests Range class validation rules
     */
    TEST( RangeTest, RangeCreationAndValidation )
    {
        // Test valid range construction
        transport::datachannel::Range range{ 0.0, 100.0 };
        EXPECT_EQ( range.low(), 0.0 );
        EXPECT_EQ( range.high(), 100.0 );

        // Test invalid range construction should throw
        EXPECT_THROW( transport::datachannel::Range( 100.0, 0.0 ), std::invalid_argument );

        // Test setting range values
        range.setLow( 10.0 );
        EXPECT_EQ( range.low(), 10.0 );

        range.setHigh( 90.0 );
        EXPECT_EQ( range.high(), 90.0 );

        // low > high
        EXPECT_THROW( range.setLow( 200.0 ), std::invalid_argument );
        /*high < low*/
        EXPECT_THROW( range.setHigh( -10.0 ), std::invalid_argument );
    }

    /**
     * @brief Test DataChannelList collection operations
     * @details Tests add, remove, and lookup operations
     */
    TEST( DataChannelTests, DataChannelList_Operations )
    {
        transport::datachannel::DataChannelList dataChannelList;
        EXPECT_EQ( dataChannelList.size(), 0U );
        EXPECT_EQ( dataChannelList.size(), 0U );

        // Add single DataChannel
        auto singleList = createValidDataChannelList();
        const auto& originalChannel = singleList.package().dataChannelList()[0];

        // Create a copy for adding
        transport::datachannel::DataChannel channelCopy = originalChannel;
        dataChannelList.add( channelCopy );

        EXPECT_EQ( dataChannelList.size(), 1U );
        EXPECT_EQ( dataChannelList.size(), 1U );

        // Test lookups work
        const auto& localId = originalChannel.dataChannelId().localId();
        auto foundChannelOpt = dataChannelList.from( localId );
        EXPECT_TRUE( foundChannelOpt.has_value() );

        // Test clear
        dataChannelList.clear();
        EXPECT_EQ( dataChannelList.size(), 0U );
        EXPECT_EQ( dataChannelList.size(), 0U );
    }

    /**
     * @brief Test error scenarios and exception handling
     * @details Tests various error conditions and proper error reporting
     */
    TEST( DataChannelTests, Error_Handling )
    {
        // Test invalid DataChannelType construction
        EXPECT_THROW( transport::datachannel::DataChannelType{ "InvalidType" }, std::invalid_argument );

        // Test invalid Format type
        EXPECT_THROW( transport::datachannel::Format{ "InvalidFormat" }, std::invalid_argument );

        // Test duplicate LocalId addition
        transport::datachannel::DataChannelList dataChannelList;
        auto validList = createValidDataChannelList();
        const auto& originalChannel = validList.package().dataChannelList()[0];

        // Add first channel
        transport::datachannel::DataChannel firstChannel = originalChannel;
        dataChannelList.add( firstChannel );

        // Try to add duplicate LocalId - should throw
        transport::datachannel::DataChannel duplicateChannel = originalChannel;
        EXPECT_THROW( dataChannelList.add( duplicateChannel ), std::invalid_argument );
    }

    /**
     * @brief Test string optimization in error messages
     */
    TEST( RestrictionTest, StringLengthRestrictionValidation )
    {
        transport::datachannel::Restriction restriction;
        restriction.setLength( 5 );

        transport::datachannel::Format format{ "String" };

        auto result = restriction.validateValue( "toolongstring", format );
        EXPECT_FALSE( result.isOk() );

        if( !result.isOk() )
        {
            const auto& errors = result.errors;
            EXPECT_FALSE( errors.empty() );
            if( !errors.empty() )
            {
                EXPECT_TRUE( errors[0].find( "should be 5" ) != std::string::npos );
            }
        }
    }

    //=====================================================================
    // JSON Serialization Tests
    //=====================================================================

    /**
     * @brief Complete JSON serialization round-trip test
     * @details Verifies that ALL fields survive JSON serialization/deserialization including:
     *          - All custom dictionaries (customHeaders, customNameObjects, customElements, customProperties)
     *          - All optional fields
     *          - All restriction details
     *          - ConfigurationReference.version
     *          - Both DataChannels (temperature and alert)
     */
    TEST( IsoMessageTests, DataChannelListJsonSerializationComplete )
    {
        // Create a fully populated DataChannelList
        auto originalPackage = createValidFullyCustomDataChannelList();

        // Serialize domain model directly to JSON
        auto options = nfx::serialization::json::Serializer<transport::datachannel::DataChannelListPackage>::Options{};
        options.includeNullFields = false;
        options.prettyPrint = true;
        std::string json =
            nfx::serialization::json::Serializer<transport::datachannel::DataChannelListPackage>::toString(
                originalPackage, options );

        // Deserialize JSON directly to domain model
        auto docOpt = nfx::json::Document::fromString( json );
        ASSERT_TRUE( docOpt.has_value() );

        auto deserializedPackage =
            nfx::serialization::json::SerializationTraits<transport::datachannel::DataChannelListPackage>::fromDocument(
                *docOpt );

        // Header
        const auto& originalHeader = originalPackage.package().header();
        const auto& deserializedHeader = deserializedPackage.package().header();

        // ShipId
        EXPECT_EQ( originalHeader.shipId().toString(), deserializedHeader.shipId().toString() );

        // ConfigurationReference
        EXPECT_EQ( originalHeader.dataChannelListId().id(), deserializedHeader.dataChannelListId().id() );
        EXPECT_EQ(
            originalHeader.dataChannelListId().timeStamp().toString(),
            deserializedHeader.dataChannelListId().timeStamp().toString() );
        EXPECT_TRUE( originalHeader.dataChannelListId().version().has_value() );
        EXPECT_TRUE( deserializedHeader.dataChannelListId().version().has_value() );
        EXPECT_EQ( *originalHeader.dataChannelListId().version(), *deserializedHeader.dataChannelListId().version() );

        // VersionInformation
        EXPECT_TRUE( originalHeader.versionInformation().has_value() );
        EXPECT_TRUE( deserializedHeader.versionInformation().has_value() );
        EXPECT_EQ(
            originalHeader.versionInformation()->namingRule(), deserializedHeader.versionInformation()->namingRule() );
        EXPECT_EQ(
            originalHeader.versionInformation()->namingSchemeVersion(),
            deserializedHeader.versionInformation()->namingSchemeVersion() );
        EXPECT_EQ(
            originalHeader.versionInformation()->referenceUrl(),
            deserializedHeader.versionInformation()->referenceUrl() );

        // Author
        EXPECT_TRUE( originalHeader.author().has_value() );
        EXPECT_TRUE( deserializedHeader.author().has_value() );
        EXPECT_EQ( *originalHeader.author(), *deserializedHeader.author() );

        // DateCreated
        EXPECT_TRUE( originalHeader.dateCreated().has_value() );
        EXPECT_TRUE( deserializedHeader.dateCreated().has_value() );
        EXPECT_EQ( originalHeader.dateCreated()->toString(), deserializedHeader.dateCreated()->toString() );

        // CustomHeaders
        EXPECT_TRUE( deserializedHeader.customHeaders().has_value() );
        const auto& deserializedCustomHeaders = *deserializedHeader.customHeaders();

        // Verify String type
        auto headerValueOpt = deserializedCustomHeaders.get<std::string>( "nr:CustomHeaderElement" );
        ASSERT_TRUE( headerValueOpt.has_value() );
        EXPECT_EQ( headerValueOpt.value(), "Vendor specific headers" );

        // Verify Integer type
        auto sampleRateOpt = deserializedCustomHeaders.get<int64_t>( "sampleRate" );
        ASSERT_TRUE( sampleRateOpt.has_value() );
        EXPECT_EQ( sampleRateOpt.value(), 1000 );

        // Verify Boolean type
        auto isValidatedOpt = deserializedCustomHeaders.get<bool>( "isValidated" );
        ASSERT_TRUE( isValidatedOpt.has_value() );
        EXPECT_EQ( isValidatedOpt.value(), true );

        // Verify String (decimal as string) type
        auto toleranceOpt = deserializedCustomHeaders.get<std::string>( "tolerance" );
        ASSERT_TRUE( toleranceOpt.has_value() );
        EXPECT_EQ( toleranceOpt.value(), "0.001" );

        // Verify String (DateTime as string) type
        auto certDateOpt = deserializedCustomHeaders.get<std::string>( "certificationDate" );
        ASSERT_TRUE( certDateOpt.has_value() );
        EXPECT_EQ( certDateOpt.value(), "2024-01-15T00:00:00Z" );

        // DataChannelList
        const auto& originalList = originalPackage.package().dataChannelList();
        const auto& deserializedList = deserializedPackage.package().dataChannelList();

        EXPECT_EQ( originalList.size(), deserializedList.size() );
        EXPECT_EQ( deserializedList.size(), 2U );

        // First DataChannel (Temperature Sensor)
        const auto& originalChannel1 = originalList[0];
        const auto& deserializedChannel1 = deserializedList[0];

        // DataChannelId
        EXPECT_EQ(
            originalChannel1.dataChannelId().localId().toString(),
            deserializedChannel1.dataChannelId().localId().toString() );
        EXPECT_TRUE( originalChannel1.dataChannelId().shortId().has_value() );
        EXPECT_TRUE( deserializedChannel1.dataChannelId().shortId().has_value() );
        EXPECT_EQ( *originalChannel1.dataChannelId().shortId(), *deserializedChannel1.dataChannelId().shortId() );

        // NameObject
        EXPECT_TRUE( originalChannel1.dataChannelId().nameObject().has_value() );
        EXPECT_TRUE( deserializedChannel1.dataChannelId().nameObject().has_value() );
        const auto& originalNameObject = *originalChannel1.dataChannelId().nameObject();
        const auto& deserializedNameObject = *deserializedChannel1.dataChannelId().nameObject();
        EXPECT_EQ( originalNameObject.namingRule(), deserializedNameObject.namingRule() );

        // CustomNameObjects - verify all types using Document API
        EXPECT_TRUE( originalNameObject.customNameObjects().has_value() );
        EXPECT_TRUE( deserializedNameObject.customNameObjects().has_value() );
        const auto& originalCustomNameObjects = *originalNameObject.customNameObjects();
        const auto& deserializedCustomNameObjects = *deserializedNameObject.customNameObjects();

        // Get root object for iteration
        auto deserializedObjOpt = deserializedCustomNameObjects.document().get<nfx::serialization::json::Object>( "" );
        ASSERT_TRUE( deserializedObjOpt.has_value() );
        const auto& deserializedObj = deserializedObjOpt.value();

        // Verify we have 5 custom properties
        size_t count = 0;
        for( const auto& [key, doc] : deserializedObj )
        {
            ++count;
        }
        EXPECT_EQ( count, 5U );

        // Verify string value
        auto nameObjStrOpt = deserializedCustomNameObjects.get<std::string>( "nr:CustomNameObject" );
        ASSERT_TRUE( nameObjStrOpt.has_value() );
        EXPECT_EQ( nameObjStrOpt.value(), "Vendor specific NameObject" );

        // Verify integer value
        auto priorityOpt = deserializedCustomNameObjects.get<int64_t>( "priority" );
        ASSERT_TRUE( priorityOpt.has_value() );
        EXPECT_EQ( priorityOpt.value(), 10 );

        // Verify boolean value
        auto isActiveOpt = deserializedCustomNameObjects.get<bool>( "isActive" );
        ASSERT_TRUE( isActiveOpt.has_value() );
        EXPECT_EQ( isActiveOpt.value(), false );

        // Verify decimal value
        auto offsetOpt = deserializedCustomNameObjects.get<Decimal>( "offset" );
        ASSERT_TRUE( offsetOpt.has_value() );
        EXPECT_EQ( offsetOpt.value().toString(), "2.5" );

        // Verify datetime value
        auto lastModifiedOpt = deserializedCustomNameObjects.get<std::string>( "lastModified" );
        ASSERT_TRUE( lastModifiedOpt.has_value() );
        EXPECT_EQ( lastModifiedOpt.value(), "2024-01-10T12:30:00Z" );
        // Property
        const auto& originalProperty1 = originalChannel1.property();
        const auto& deserializedProperty1 = deserializedChannel1.property();

        // DataChannelType
        EXPECT_EQ( originalProperty1.dataChannelType().type(), deserializedProperty1.dataChannelType().type() );
        EXPECT_TRUE( originalProperty1.dataChannelType().updateCycle().has_value() );
        EXPECT_TRUE( deserializedProperty1.dataChannelType().updateCycle().has_value() );
        EXPECT_EQ(
            *originalProperty1.dataChannelType().updateCycle(),
            *deserializedProperty1.dataChannelType().updateCycle() );

        // Format
        EXPECT_EQ( originalProperty1.format().type(), deserializedProperty1.format().type() );

        // Restriction
        EXPECT_TRUE( originalProperty1.format().restriction().has_value() );
        EXPECT_TRUE( deserializedProperty1.format().restriction().has_value() );
        const auto& originalRestriction = *originalProperty1.format().restriction();
        const auto& deserializedRestriction = *deserializedProperty1.format().restriction();

        EXPECT_TRUE( originalRestriction.fractionDigits().has_value() );
        EXPECT_TRUE( deserializedRestriction.fractionDigits().has_value() );
        EXPECT_EQ( *originalRestriction.fractionDigits(), *deserializedRestriction.fractionDigits() );

        EXPECT_TRUE( originalRestriction.maxInclusive().has_value() );
        EXPECT_TRUE( deserializedRestriction.maxInclusive().has_value() );
        EXPECT_EQ( *originalRestriction.maxInclusive(), *deserializedRestriction.maxInclusive() );

        EXPECT_TRUE( originalRestriction.minInclusive().has_value() );
        EXPECT_TRUE( deserializedRestriction.minInclusive().has_value() );
        EXPECT_EQ( *originalRestriction.minInclusive(), *deserializedRestriction.minInclusive() );

        // Range
        EXPECT_TRUE( originalProperty1.range().has_value() );
        EXPECT_TRUE( deserializedProperty1.range().has_value() );
        const auto& originalRange = *originalProperty1.range();
        const auto& deserializedRange = *deserializedProperty1.range();
        EXPECT_EQ( originalRange.low(), deserializedRange.low() );
        EXPECT_EQ( originalRange.high(), deserializedRange.high() );

        // Unit
        EXPECT_TRUE( originalProperty1.unit().has_value() );
        EXPECT_TRUE( deserializedProperty1.unit().has_value() );
        const auto& originalUnit = *originalProperty1.unit();
        const auto& deserializedUnit = *deserializedProperty1.unit();
        EXPECT_EQ( originalUnit.unitSymbol(), deserializedUnit.unitSymbol() );
        EXPECT_TRUE( originalUnit.quantityName().has_value() );
        EXPECT_TRUE( deserializedUnit.quantityName().has_value() );
        EXPECT_EQ( *originalUnit.quantityName(), *deserializedUnit.quantityName() );

        // CustomElements - verify all 5 Value types
        EXPECT_TRUE( originalUnit.customElements().has_value() );
        EXPECT_TRUE( deserializedUnit.customElements().has_value() );
        const auto& deserializedCustomElements = *deserializedUnit.customElements();

        // Verify values using Document API
        EXPECT_EQ(
            deserializedCustomElements.get<std::string>( "nr:CustomUnitElement" ), "Vendor specific unit element" );
        EXPECT_EQ( deserializedCustomElements.get<int64_t>( "conversionFactor" ), 100 );
        EXPECT_EQ( deserializedCustomElements.get<bool>( "isMetric" ), false );
        EXPECT_EQ( deserializedCustomElements.get<Decimal>( "multiplier" )->toString(), "1.8" );
        EXPECT_EQ( deserializedCustomElements.get<std::string>( "standardizedDate" ), "2020-05-01T00:00:00.0000000Z" );

        // QualityCoding
        EXPECT_TRUE( originalProperty1.qualityCoding().has_value() );
        EXPECT_TRUE( deserializedProperty1.qualityCoding().has_value() );
        EXPECT_EQ( *originalProperty1.qualityCoding(), *deserializedProperty1.qualityCoding() );

        // Name
        EXPECT_TRUE( originalProperty1.name().has_value() );
        EXPECT_TRUE( deserializedProperty1.name().has_value() );
        EXPECT_EQ( *originalProperty1.name(), *deserializedProperty1.name() );

        // Remarks
        EXPECT_TRUE( originalProperty1.remarks().has_value() );
        EXPECT_TRUE( deserializedProperty1.remarks().has_value() );
        EXPECT_EQ( *originalProperty1.remarks(), *deserializedProperty1.remarks() );

        // CustomProperties - verify all 5 values using Document API
        EXPECT_TRUE( originalProperty1.customProperties().has_value() );
        EXPECT_TRUE( deserializedProperty1.customProperties().has_value() );
        const auto& deserializedCustomProperties = *deserializedProperty1.customProperties();

        EXPECT_EQ(
            deserializedCustomProperties.get<std::string>( "nr:CustomPropertyElement" ),
            "Vendor specific property element" );
        EXPECT_EQ( deserializedCustomProperties.get<int64_t>( "maxSamples" ), 500 );
        EXPECT_EQ( deserializedCustomProperties.get<bool>( "requiresCalibration" ), false );
        EXPECT_EQ( deserializedCustomProperties.get<Decimal>( "accuracy" )->toString(), "0.05" );
        EXPECT_EQ(
            deserializedCustomProperties.get<std::string>( "installationDate" ), "2023-06-15T08:00:00.0000000Z" );

        // Second DataChannel (Alert Type)
        const auto& originalChannel2 = originalList[1];
        const auto& deserializedChannel2 = deserializedList[1];

        // DataChannelId
        EXPECT_EQ(
            originalChannel2.dataChannelId().localId().toString(),
            deserializedChannel2.dataChannelId().localId().toString() );
        EXPECT_TRUE( originalChannel2.dataChannelId().shortId().has_value() );
        EXPECT_TRUE( deserializedChannel2.dataChannelId().shortId().has_value() );
        EXPECT_EQ( *originalChannel2.dataChannelId().shortId(), *deserializedChannel2.dataChannelId().shortId() );

        // Property
        const auto& originalProperty2 = originalChannel2.property();
        const auto& deserializedProperty2 = deserializedChannel2.property();

        // DataChannelType
        EXPECT_EQ( originalProperty2.dataChannelType().type(), deserializedProperty2.dataChannelType().type() );
        EXPECT_EQ( originalProperty2.dataChannelType().type(), "Alert" );

        // Format
        EXPECT_EQ( originalProperty2.format().type(), deserializedProperty2.format().type() );
        EXPECT_EQ( originalProperty2.format().type(), "String" );

        // Restriction for String
        EXPECT_TRUE( originalProperty2.format().restriction().has_value() );
        EXPECT_TRUE( deserializedProperty2.format().restriction().has_value() );
        const auto& originalRestriction2 = *originalProperty2.format().restriction();
        const auto& deserializedRestriction2 = *deserializedProperty2.format().restriction();

        EXPECT_TRUE( originalRestriction2.maxLength().has_value() );
        EXPECT_TRUE( deserializedRestriction2.maxLength().has_value() );
        EXPECT_EQ( *originalRestriction2.maxLength(), *deserializedRestriction2.maxLength() );

        EXPECT_TRUE( originalRestriction2.minLength().has_value() );
        EXPECT_TRUE( deserializedRestriction2.minLength().has_value() );
        EXPECT_EQ( *originalRestriction2.minLength(), *deserializedRestriction2.minLength() );

        // AlertPriority
        EXPECT_TRUE( originalProperty2.alertPriority().has_value() );
        EXPECT_TRUE( deserializedProperty2.alertPriority().has_value() );
        EXPECT_EQ( *originalProperty2.alertPriority(), *deserializedProperty2.alertPriority() );

        // Verify optional fields are NOT present (should be nullopt)
        EXPECT_FALSE( originalProperty2.range().has_value() );
        EXPECT_FALSE( deserializedProperty2.range().has_value() );
        EXPECT_FALSE( originalProperty2.unit().has_value() );
        EXPECT_FALSE( deserializedProperty2.unit().has_value() );
        EXPECT_FALSE( originalProperty2.qualityCoding().has_value() );
        EXPECT_FALSE( deserializedProperty2.qualityCoding().has_value() );
        EXPECT_FALSE( originalProperty2.name().has_value() );
        EXPECT_FALSE( deserializedProperty2.name().has_value() );
        EXPECT_FALSE( originalProperty2.remarks().has_value() );
        EXPECT_FALSE( deserializedProperty2.remarks().has_value() );
        EXPECT_FALSE( originalProperty2.customProperties().has_value() );
        EXPECT_FALSE( deserializedProperty2.customProperties().has_value() );
    }
} // namespace dnv::vista::sdk::tests
