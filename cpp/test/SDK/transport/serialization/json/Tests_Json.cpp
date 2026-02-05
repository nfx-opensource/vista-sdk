/**
 * @file Tests_Json.cpp
 * @brief Unit tests for DataChannelList JSON serialization
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/serialization/json/DataChannelListSerializationTraits.h>
#include <dnv/vista/sdk/serialization/json/TimeSeriesDataSerializationTraits.h>

#include <dnv/vista/sdk/transport/datachannel/DataChannel.h>

#include <nfx/Json.h>
#include <nfx/Serialization.h>

#include <EmbeddedTestData.h>
#include <EmbeddedSchemas/EmbeddedSchemas.h>

using namespace dnv::vista::sdk;
using namespace dnv::vista::sdk::transport;
using namespace dnv::vista::sdk::transport::datachannel;
using namespace nfx::serialization::json;

namespace dnv::vista::sdk::tests
{
    //=====================================================================
    // Helper functions
    //=====================================================================

    /**
     * @brief Deserialize DataChannelListPackage from JSON string
     */
    DataChannelListPackage deserializeDataChannelList( const std::string& jsonStr )
    {
        auto docOpt = nfx::json::Document::fromString( jsonStr );
        if( !docOpt.has_value() )
        {
            throw std::runtime_error{ "Failed to parse JSON" };
        }

        const auto& doc = docOpt.value();

        // Factory pattern - direct construction, no minimal package needed!
        return nfx::serialization::json::SerializationTraits<DataChannelListPackage>::fromDocument( doc );
    }

    /**
     * @brief Serialize DataChannelListPackage to JSON string
     */
    std::string serializeDataChannelList( const DataChannelListPackage& package )
    {
        auto options = nfx::serialization::json::Serializer<DataChannelListPackage>::Options{};
        options.includeNullFields = false;
        options.prettyPrint = true;
        return nfx::serialization::json::Serializer<DataChannelListPackage>::toString( package, options );
    }

    /**
     * @brief Deserialize TimeSeriesDataPackage from JSON string
     */
    transport::timeseries::TimeSeriesDataPackage deserializeTimeSeriesData( const std::string& jsonStr )
    {
        using namespace transport::timeseries;

        auto docOpt = nfx::json::Document::fromString( jsonStr );
        if( !docOpt.has_value() )
        {
            throw std::runtime_error{ "Failed to parse JSON" };
        }

        const auto& doc = docOpt.value();

        return nfx::serialization::json::SerializationTraits<TimeSeriesDataPackage>::fromDocument( doc );
    }

    /**
     * @brief Serialize TimeSeriesDataPackage to JSON string
     */
    std::string serializeTimeSeriesData( const transport::timeseries::TimeSeriesDataPackage& package )
    {
        auto options = nfx::serialization::json::Serializer<transport::timeseries::TimeSeriesDataPackage>::Options{};
        options.includeNullFields = false;
        options.prettyPrint = true;
        return nfx::serialization::json::Serializer<transport::timeseries::TimeSeriesDataPackage>::toString(
            package, options );
    }

    //=====================================================================
    // Tests
    //=====================================================================

    //----------------------------------------------
    // ISO8601 DateTimeOffset format validation
    //----------------------------------------------

    TEST( DataChannelListJsonTests, ISO8601_DateTimeOffset )
    {
        // Helper to create minimal DataChannelList JSON with specific TimeStamp
        auto createTestJson = []( const std::string& timestamp ) -> std::string {
            return R"({
                "Package": {
                    "Header": {
                        "ShipID": "IMO1234567",
                        "DataChannelListID": {
                            "ID": "test-config",
                            "TimeStamp": ")" +
                   timestamp + R"(",
                            "Version": "1.0"
                        }
                    },
                    "DataChannelList": {
                        "DataChannel": []
                    }
                }
            })";
        };

        // Test valid formats - should deserialize successfully
        std::vector<std::string> validFormats = { "2022-04-04T20:44:31Z",
                                                  "2022-04-04T20:44:31.1234567Z",
                                                  "2022-04-04T20:44:31.123456789Z",
                                                  "2022-04-04T20:44:31.1234567+00:00",
                                                  "2022-04-04T20:44:31.1234567-00:00",
                                                  "2022-04-04T20:44:31.1234567+01:00",
                                                  "2022-04-04T20:44:31.1234567-01:00" };

        for( const auto& format : validFormats )
        {
            auto json = createTestJson( format );
            EXPECT_NO_THROW( {
                auto package = deserializeDataChannelList( json );
                // Verify roundtrip
                auto serialized = serializeDataChannelList( package );
                auto roundtripped = deserializeDataChannelList( serialized );
                EXPECT_EQ(
                    package.package().header().dataChannelListId().timeStamp().ticks(),
                    roundtripped.package().header().dataChannelListId().timeStamp().ticks() )
                    << "Roundtrip should preserve ticks for: " << format;
            } ) << "Should parse valid format: "
                << format;
        }

        // Test invalid formats - should throw during deserialization
        // NOTE: ISO 8601 allows abbreviated timezone notation "±hh" (without minutes)
        // Vista SDK enforces stricter validation requiring "±hh:mm"
        std::vector<std::string> invalidFormats = {
            "2022-04-04T20:44:31.1234567-01", // Missing :00 for timezone
            "20-11-1994T20:44:31Z"            // Invalid date format (2-digit year)
        };

        for( const auto& format : invalidFormats )
        {
            auto json = createTestJson( format );
            EXPECT_THROW(
                { auto package = deserializeDataChannelList( json ); }, std::runtime_error )
                << "Should reject invalid format: " << format;
        }
    }

    TEST( TimeSeriesDataTests, ISO8601_DateTimeOffset )
    {
        using namespace dnv::vista::sdk::transport::timeseries;

        // Helper to create minimal TimeSeriesData JSON with specific TimeStamp
        auto createTestJson = []( const std::string& timestamp ) -> std::string {
            return R"({
                "Package": {
                    "Header": {
                        "ShipID": "IMO1234567",
                        "DateCreated": "2024-12-30T10:00:00.0000000Z"
                    },
                    "TimeSeriesData": [
                    {
                        "DataConfiguration": {
                                "ID": "test.xml",
                                "TimeStamp": ")" +
                   timestamp + R"("
                            }
                        }
                    ]
                }
            })";
        };

        // Test valid formats - should deserialize successfully
        std::vector<std::string> validFormats = { "2022-04-04T20:44:31Z",
                                                  "2022-04-04T20:44:31.1234567Z",
                                                  "2022-04-04T20:44:31.123456789Z",
                                                  "2022-04-04T20:44:31.1234567+00:00",
                                                  "2022-04-04T20:44:31.1234567-00:00",
                                                  "2022-04-04T20:44:31.1234567+01:00",
                                                  "2022-04-04T20:44:31.1234567-01:00" };

        for( const auto& format : validFormats )
        {
            auto json = createTestJson( format );
            EXPECT_NO_THROW( {
                auto package = deserializeTimeSeriesData( json );
                // Verify roundtrip
                auto serialized = serializeTimeSeriesData( package );
                auto roundtripped = deserializeTimeSeriesData( serialized );
                EXPECT_EQ(
                    package.package().timeSeriesData()[0].dataConfiguration()->timeStamp().ticks(),
                    roundtripped.package().timeSeriesData()[0].dataConfiguration()->timeStamp().ticks() )
                    << "Roundtrip should preserve ticks for: " << format;
            } ) << "Should parse valid format: "
                << format;
        }

        // Test invalid formats - should throw during deserialization
        std::vector<std::string> invalidFormats = {
            "2022-04-04T20:44:31.1234567-01", // Missing :00 for timezone
            "20-11-1994T20:44:31Z"            // Invalid date format (2-digit year)
        };

        for( const auto& format : invalidFormats )
        {
            auto json = createTestJson( format );
            EXPECT_THROW(
                { auto package = deserializeTimeSeriesData( json ); }, std::runtime_error )
                << "Should reject invalid format: " << format;
        }
    }

    //----------------------------------------------
    // JSONSchema validation
    //----------------------------------------------

    TEST( DataChannelListJsonTests, DataChannelList_JSONSchema_Validation )
    {
        // Schema files to validate against
        std::vector<std::string> testFiles = {
            "DataChannelList.sample.json",
            // "DataChannelList.sample.compact.json" // FIXME: Compact sample has duplicate LocalIds
            "DataChannelList.sample.json"
        };

        // Load schema
        auto schemaDataOpt = EmbeddedSchema::dataChannelListJson();
        ASSERT_TRUE( schemaDataOpt.has_value() ) << "Failed to load embedded schema";

        auto schemaDocOpt = nfx::json::Document::fromString( *schemaDataOpt );
        ASSERT_TRUE( schemaDocOpt.has_value() ) << "Failed to parse schema";

        SchemaValidator validator;
        ASSERT_TRUE( validator.load( *schemaDocOpt ) ) << "Failed to load schema into validator";

        // Validate each test file
        for( const auto& fileName : testFiles )
        {
            auto jsonDataOpt = EmbeddedSchema::get( fileName );
            ASSERT_TRUE( jsonDataOpt.has_value() ) << "Failed to load " << fileName;

            auto docOpt = nfx::json::Document::fromString( *jsonDataOpt );
            ASSERT_TRUE( docOpt.has_value() ) << "Failed to parse " << fileName;

            // Validate against schema
            auto result = validator.validate( *docOpt );
            EXPECT_TRUE( result.isValid() ) << "Schema validation failed for " << fileName;

            if( !result.isValid() )
            {
                for( size_t i = 0; i < result.errorCount(); ++i )
                {
                    const auto& error = result.error( i );
                    std::cerr << "  [" << ( i + 1 ) << "] " << error.path() << ": " << error.message() << "\n";
                }
            }

            // Also verify we can deserialize it
            auto package = deserializeDataChannelList( std::string{ *jsonDataOpt } );
            EXPECT_TRUE( package.package().dataChannelList().size() > 0 ) << "Should have channels in " << fileName;
        }
    }

    TEST( TimeSeriesDataTests, TimeSeriesData_JSONSchema_Validation )
    {
        // Schema file to validate against
        std::vector<std::string> testFiles = { "TimeSeriesData.sample.json" };

        // Load schema
        auto schemaDataOpt = EmbeddedSchema::timeSeriesDataJson();
        ASSERT_TRUE( schemaDataOpt.has_value() ) << "Failed to load embedded schema";

        auto schemaDocOpt = nfx::json::Document::fromString( *schemaDataOpt );
        ASSERT_TRUE( schemaDocOpt.has_value() ) << "Failed to parse schema";

        SchemaValidator validator;
        ASSERT_TRUE( validator.load( *schemaDocOpt ) ) << "Failed to load schema into validator";

        // Validate each test file
        for( const auto& fileName : testFiles )
        {
            auto jsonDataOpt = EmbeddedSchema::get( fileName );
            ASSERT_TRUE( jsonDataOpt.has_value() ) << "Failed to load " << fileName;

            auto docOpt = nfx::json::Document::fromString( *jsonDataOpt );
            ASSERT_TRUE( docOpt.has_value() ) << "Failed to parse " << fileName;

            // Validate against schema
            auto result = validator.validate( *docOpt );
            EXPECT_TRUE( result.isValid() ) << "Schema validation failed for " << fileName;

            if( !result.isValid() )
            {
                for( size_t i = 0; i < result.errorCount(); ++i )
                {
                    const auto& error = result.error( i );
                    std::cerr << "  [" << ( i + 1 ) << "] " << error.path() << ": " << error.message() << "\n";
                }
            }

            // Also verify we can deserialize it
            auto package = deserializeTimeSeriesData( std::string{ *jsonDataOpt } );
            EXPECT_GT( package.package().timeSeriesData().size(), 0U )
                << "Should have time series data in " << fileName;
        }
    }

    //----------------------------------------------
    // File serialization
    //----------------------------------------------

    TEST( DataChannelListJsonTests, DataChannelList_File_Serialization )
    {
        std::vector<std::string> testFiles = {
            "DataChannelList.json",
            "DataChannelList.sample.json",
            // "DataChannelList.sample.compact.json" // FIXME: Compact sample has duplicate LocalIds
            "DataChannelList.sample.json"
        };

        for( const auto& fileName : testFiles )
        {
            // Load file
            std::string jsonData;

            if( fileName == "DataChannelList.json" )
            {
                auto dataOpt = EmbeddedTestData::load<std::string>( fileName );
                ASSERT_TRUE( dataOpt.has_value() ) << "Failed to load " << fileName;
                jsonData = dataOpt.value();
            }
            else
            {
                auto dataOpt = EmbeddedSchema::get( fileName );
                ASSERT_TRUE( dataOpt.has_value() ) << "Failed to load " << fileName;
                jsonData = std::string{ *dataOpt };
            }

            // Deserialize
            auto package = deserializeDataChannelList( jsonData );
            EXPECT_TRUE( package.package().dataChannelList().size() > 0 ) << "Should have channels in " << fileName;

            // Serialize (this effectively creates the .out file equivalent)
            auto serialized = serializeDataChannelList( package );
            EXPECT_FALSE( serialized.empty() ) << "Serialization should produce output for " << fileName;

            // Verify serialized JSON can be deserialized again
            auto roundtrippedPackage = deserializeDataChannelList( serialized );
            EXPECT_EQ(
                package.package().dataChannelList().size(), roundtrippedPackage.package().dataChannelList().size() )
                << "Roundtrip should preserve channel count for " << fileName;
        }
    }

    TEST( TimeSeriesDataTests, TimeSeriesData_File_Serialization )
    {
        auto jsonStr = EmbeddedTestData::load<std::string>( "TimeSeriesData.json" );

        // Deserialize
        auto package = deserializeTimeSeriesData( jsonStr.value() );
        EXPECT_GT( package.package().timeSeriesData().size(), 0U )
            << "Should have time series data in TimeSeriesData.json";

        // Serialize (this effectively creates the .out file equivalent)
        auto serialized = serializeTimeSeriesData( package );
        EXPECT_FALSE( serialized.empty() ) << "Serialization should produce output for TimeSeriesData.json";

        // Verify serialized JSON can be deserialized again
        auto roundtrippedPackage = deserializeTimeSeriesData( serialized );
        EXPECT_EQ( package.package().timeSeriesData().size(), roundtrippedPackage.package().timeSeriesData().size() )
            << "Roundtrip should preserve time series data count for TimeSeriesData.json";
    }

    //----------------------------------------------
    // Basic deserialization
    //----------------------------------------------

    TEST( DataChannelListJsonTests, DataChannelList_Deserialization )
    {
        auto jsonStr = EmbeddedTestData::load<std::string>( "DataChannelList.json" ).value();

        ASSERT_NO_THROW( {
            auto package = deserializeDataChannelList( jsonStr );

            // Verify basic structure
            EXPECT_EQ( package.package().header().shipId().toString(), "IMO1234567" );
            EXPECT_EQ( package.package().header().dataChannelListId().id(), "DataChannelList.xml" );
            EXPECT_EQ( package.package().dataChannelList().size(), 4 );
        } );
    }

    TEST( TimeSeriesDataTests, TimeSeriesData_Deserialization )
    {
        auto jsonStr = EmbeddedTestData::load<std::string>( "TimeSeriesData.json" );

        ASSERT_NO_THROW( {
            auto package = deserializeTimeSeriesData( jsonStr.value() );

            // Verify basic structure
            EXPECT_EQ( package.package().header()->shipId().toString(), "IMO1234567" );
            EXPECT_GT( package.package().timeSeriesData().size(), 0U );
        } );
    }

    //----------------------------------------------
    // Serialization roundtrip
    //----------------------------------------------

    TEST( DataChannelListJsonTests, DataChannelList_Serialization_Roundtrip )
    {
        // Load original JSON
        auto originalJsonStr = EmbeddedTestData::load<std::string>( "DataChannelList.json" ).value();

        // Deserialize
        auto package = deserializeDataChannelList( originalJsonStr );

        // Serialize
        auto serializedJsonStr = serializeDataChannelList( package );

        // Deserialize again
        auto deserializedPackage = deserializeDataChannelList( serializedJsonStr );

        // Bit-to-bit comparison: serialize both and compare JSON strings
        auto json1 = serializeDataChannelList( package );
        auto json2 = serializeDataChannelList( deserializedPackage );
        EXPECT_EQ( json1, json2 ) << "JSON should be identical after roundtrip (bit-to-bit)";

        // Verify fields are preserved
        EXPECT_EQ(
            package.package().header().shipId().toString(),
            deserializedPackage.package().header().shipId().toString() );
        EXPECT_EQ(
            package.package().header().dataChannelListId().id(),
            deserializedPackage.package().header().dataChannelListId().id() );
        EXPECT_EQ(
            package.package().header().dataChannelListId().timeStamp().ticks(),
            deserializedPackage.package().header().dataChannelListId().timeStamp().ticks() );
        EXPECT_EQ( package.package().dataChannelList().size(), deserializedPackage.package().dataChannelList().size() );

        // Verify first channel
        const auto& channel1 = package.package().dataChannelList()[0];
        const auto& deserializedChannel1 = deserializedPackage.package().dataChannelList()[0];

        EXPECT_EQ(
            channel1.dataChannelId().localId().toString(), deserializedChannel1.dataChannelId().localId().toString() );
        EXPECT_EQ( channel1.dataChannelId().shortId(), deserializedChannel1.dataChannelId().shortId() );
        EXPECT_EQ(
            channel1.property().dataChannelType().type(), deserializedChannel1.property().dataChannelType().type() );
    }

    TEST( TimeSeriesDataTests, TimeSeriesData_Serialization_Roundtrip )
    {
        auto jsonStr = EmbeddedTestData::load<std::string>( "TimeSeriesData.json" );

        // Load original JSON
        auto originalJsonStr = jsonStr.value();

        // Deserialize
        auto package = deserializeTimeSeriesData( originalJsonStr );

        // Serialize
        auto serializedJsonStr = serializeTimeSeriesData( package );

        // Deserialize again
        auto deserializedPackage = deserializeTimeSeriesData( serializedJsonStr );

        // Bit-to-bit comparison: serialize both and compare JSON strings
        auto json1 = serializeTimeSeriesData( package );
        auto json2 = serializeTimeSeriesData( deserializedPackage );
        EXPECT_EQ( json1, json2 ) << "JSON should be identical after roundtrip (bit-to-bit)";

        // Verify fields are preserved
        if( package.package().header().has_value() )
        {
            EXPECT_TRUE( deserializedPackage.package().header().has_value() );
            EXPECT_EQ(
                package.package().header()->shipId().toString(),
                deserializedPackage.package().header()->shipId().toString() );
        }
        EXPECT_EQ( package.package().timeSeriesData().size(), deserializedPackage.package().timeSeriesData().size() );
    }

    //----------------------------------------------
    // DateTimeOffset consistency
    //----------------------------------------------

    TEST( DataChannelListJsonTests, DataChannelListId_DateTimeOffset_Consistency )
    {
        // Load original JSON
        auto originalJsonStr = EmbeddedTestData::load<std::string>( "DataChannelList.json" ).value();

        // Deserialize original package
        auto package = deserializeDataChannelList( originalJsonStr );

        // Serialize and deserialize (roundtrip)
        auto serializedJsonStr = serializeDataChannelList( package );
        auto deserializedPackage = deserializeDataChannelList( serializedJsonStr );

        // Get timestamps
        auto packageTimeStamp = package.package().header().dataChannelListId().timeStamp();
        auto deserializedTimeStamp = deserializedPackage.package().header().dataChannelListId().timeStamp();

        // Verify ALL timestamps are the same
        EXPECT_EQ( packageTimeStamp.ticks(), deserializedTimeStamp.ticks() )
            << "Package and Deserialized timestamps must be identical";
        EXPECT_EQ( packageTimeStamp.offset().ticks(), deserializedTimeStamp.offset().ticks() )
            << "Package and Deserialized offsets must be identical";

        // Also verify DateCreated
        if( auto dateCreated = package.package().header().dateCreated() )
        {
            auto deserializedDateCreated = deserializedPackage.package().header().dateCreated();
            ASSERT_TRUE( deserializedDateCreated.has_value() ) << "DateCreated should be preserved";

            EXPECT_EQ( dateCreated->ticks(), deserializedDateCreated->ticks() )
                << "DateCreated ticks must be identical";
            EXPECT_EQ( dateCreated->offset().ticks(), deserializedDateCreated->offset().ticks() )
                << "DateCreated offset must be identical";
        }
    }

    TEST( TimeSeriesDataTests, TimeSeriesData_DateTimeOffset_Consistency )
    {
        auto jsonStr = EmbeddedTestData::load<std::string>( "TimeSeriesData.json" );

        // Load original JSON
        auto originalJsonStr = jsonStr.value();

        // Deserialize original package
        auto package = deserializeTimeSeriesData( originalJsonStr );

        // Serialize and deserialize (roundtrip)
        auto serializedJsonStr = serializeTimeSeriesData( package );
        auto deserializedPackage = deserializeTimeSeriesData( serializedJsonStr );

        // Verify DateTimeOffset consistency for DataConfiguration if present
        if( package.package().timeSeriesData().size() > 0 &&
            package.package().timeSeriesData()[0].dataConfiguration().has_value() )
        {
            ASSERT_TRUE( deserializedPackage.package().timeSeriesData().size() > 0 );
            ASSERT_TRUE( deserializedPackage.package().timeSeriesData()[0].dataConfiguration().has_value() );

            auto packageTimeStamp = package.package().timeSeriesData()[0].dataConfiguration()->timeStamp();
            auto deserializedTimeStamp =
                deserializedPackage.package().timeSeriesData()[0].dataConfiguration()->timeStamp();

            // Verify ALL timestamps are the same
            EXPECT_EQ( packageTimeStamp.ticks(), deserializedTimeStamp.ticks() )
                << "Package and Deserialized timestamps must be identical";
            EXPECT_EQ( packageTimeStamp.offset().ticks(), deserializedTimeStamp.offset().ticks() )
                << "Package and Deserialized offsets must be identical";
        }

        // Also verify DateCreated
        if( package.package().header().has_value() && package.package().header()->dateCreated().has_value() )
        {
            ASSERT_TRUE( deserializedPackage.package().header().has_value() );
            auto dateCreated = package.package().header()->dateCreated();
            auto deserializedDateCreated = deserializedPackage.package().header()->dateCreated();
            ASSERT_TRUE( deserializedDateCreated.has_value() ) << "DateCreated should be preserved";

            EXPECT_EQ( dateCreated->ticks(), deserializedDateCreated->ticks() )
                << "DateCreated ticks must be identical";
            EXPECT_EQ( dateCreated->offset().ticks(), deserializedDateCreated->offset().ticks() )
                << "DateCreated offset must be identical";
        }
    }
} // namespace dnv::vista::sdk::tests
