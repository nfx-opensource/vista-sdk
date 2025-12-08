/**
 * @file Tests_EmbeddedSchemas.cpp
 * @brief Tests for embedded schemas functionality
 *
 */

#include <gtest/gtest.h>

#include <EmbeddedSchemas.h>

using namespace dnv::vista::sdk;

//=====================================================================
// EmbeddedSchema tests
//=====================================================================

//----------------------------------------------
// DataChannelList JSON schema
//----------------------------------------------

TEST( EmbeddedSchemaTests, DataChannelListJson_Returns )
{
    auto schema = EmbeddedSchema::dataChannelListJson();

    ASSERT_TRUE( schema.has_value() );
    EXPECT_FALSE( schema->empty() );
    EXPECT_TRUE( schema->find( "DataChannelListPackage" ) != std::string_view::npos )
        << "Should contain DataChannelListPackage definition";
}

//----------------------------------------------
// TimeSeriesData JSON schema
//----------------------------------------------

TEST( EmbeddedSchemaTests, TimeSeriesDataJson_Returns )
{
    auto schema = EmbeddedSchema::timeSeriesDataJson();

    ASSERT_TRUE( schema.has_value() );
    EXPECT_FALSE( schema->empty() );
    EXPECT_TRUE( schema->find( "TimeSeriesDataPackage" ) != std::string_view::npos )
        << "Should contain TimeSeriesDataPackage definition";
}

//----------------------------------------------
// DataChannelList XSD schema
//----------------------------------------------

TEST( EmbeddedSchemaTests, DataChannelListXsd_Returns )
{
    auto schema = EmbeddedSchema::dataChannelListXsd();

    ASSERT_TRUE( schema.has_value() );
    EXPECT_FALSE( schema->empty() );
    EXPECT_TRUE( schema->find( "<?xml" ) != std::string_view::npos ) << "Should be valid XML";
    EXPECT_TRUE( schema->find( "DataChannelList" ) != std::string_view::npos )
        << "Should contain DataChannelList definition";
}

//----------------------------------------------
// TimeSeriesData XSD schema
//----------------------------------------------

TEST( EmbeddedSchemaTests, TimeSeriesDataXsd_Returns )
{
    auto schema = EmbeddedSchema::timeSeriesDataXsd();

    ASSERT_TRUE( schema.has_value() );
    EXPECT_FALSE( schema->empty() );
    EXPECT_TRUE( schema->find( "<?xml" ) != std::string_view::npos ) << "Should be valid XML";
    EXPECT_TRUE( schema->find( "TimeSeriesData" ) != std::string_view::npos )
        << "Should contain TimeSeriesData definition";
}

//----------------------------------------------
// Generic get() method
//----------------------------------------------

TEST( EmbeddedSchemaTests, Get_DataChannelListSchema )
{
    auto schema = EmbeddedSchema::get( "DataChannelList.schema.json" );

    ASSERT_TRUE( schema.has_value() );
    EXPECT_FALSE( schema->empty() );
    EXPECT_TRUE( schema->find( "DataChannelListPackage" ) != std::string_view::npos );
}

TEST( EmbeddedSchemaTests, Get_TimeSeriesDataSchema )
{
    auto schema = EmbeddedSchema::get( "TimeSeriesData.schema.json" );

    ASSERT_TRUE( schema.has_value() );
    EXPECT_FALSE( schema->empty() );
    EXPECT_TRUE( schema->find( "TimeSeriesDataPackage" ) != std::string_view::npos );
}

TEST( EmbeddedSchemaTests, Get_DataChannelListXsd )
{
    auto schema = EmbeddedSchema::get( "DataChannelList.xsd" );

    ASSERT_TRUE( schema.has_value() );
    EXPECT_FALSE( schema->empty() );
    EXPECT_TRUE( schema->find( "DataChannelList" ) != std::string_view::npos );
}

TEST( EmbeddedSchemaTests, Get_TimeSeriesDataXsd )
{
    auto schema = EmbeddedSchema::get( "TimeSeriesData.xsd" );

    ASSERT_TRUE( schema.has_value() );
    EXPECT_FALSE( schema->empty() );
    EXPECT_TRUE( schema->find( "TimeSeriesData" ) != std::string_view::npos );
}

TEST( EmbeddedSchemaTests, Get_NotFound )
{
    auto schema = EmbeddedSchema::get( "NonExistent.schema.json" );

    EXPECT_FALSE( schema.has_value() );
}

//----------------------------------------------
// Sample files
//----------------------------------------------

TEST( EmbeddedSchemaTests, Get_SampleFiles )
{
    auto sampleCompact = EmbeddedSchema::get( "DataChannelList.sample.compact.json" );
    ASSERT_TRUE( sampleCompact.has_value() );
    EXPECT_FALSE( sampleCompact->empty() );

    auto sample = EmbeddedSchema::get( "DataChannelList.sample.json" );
    ASSERT_TRUE( sample.has_value() );
    EXPECT_FALSE( sample->empty() );
    EXPECT_TRUE( sample->find( "Package" ) != std::string_view::npos ) << "Sample should contain Package";
}

//----------------------------------------------
// Experimental schemas
//----------------------------------------------

TEST( EmbeddedSchemaTests, Get_ExperimentalSchemas )
{
    auto experimentalTimeSeries = EmbeddedSchema::get( "experimental/TimeSeriesData.schema.json" );
    ASSERT_TRUE( experimentalTimeSeries.has_value() );
    EXPECT_FALSE( experimentalTimeSeries->empty() );

    auto experimentalDataList = EmbeddedSchema::get( "experimental/DataList.schema.json" );
    ASSERT_TRUE( experimentalDataList.has_value() );
    EXPECT_FALSE( experimentalDataList->empty() );
}
