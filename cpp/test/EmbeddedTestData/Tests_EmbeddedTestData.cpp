/**
 * @file Tests_EmbeddedTestData.cpp
 * @brief Tests for embedded testdata functionality
 */

#include <gtest/gtest.h>

#include <EmbeddedTestData.h>

using namespace dnv::vista::sdk;

//=====================================================================
// EmbeddedTestData tests
//=====================================================================

TEST( EmbeddedTestDataTests, ListFiles )
{
    // Should list all embedded testdata files
    const auto files = EmbeddedTestData::listFiles();

    EXPECT_FALSE( files.empty() );
    EXPECT_GT( files.size(), 0 );

    // Should include known test files
    bool hasCodebookJson = false;
    bool hasLocalIdsTxt = false;

    for( const auto& file : files )
    {
        if( file == "Codebook.json" )
            hasCodebookJson = true;
        if( file == "LocalIds.txt" )
            hasLocalIdsTxt = true;
    }

    EXPECT_TRUE( hasCodebookJson ) << "Should have Codebook.json";
    EXPECT_TRUE( hasLocalIdsTxt ) << "Should have LocalIds.txt";
}

TEST( EmbeddedTestDataTests, Load_ReturnsDocument )
{
    // Load returns Document which can be used with nfx::json API
    const auto doc = EmbeddedTestData::load<nfx::json::Document>( "Codebook.json" );

    ASSERT_TRUE( doc.has_value() );
    EXPECT_TRUE( doc->contains( "ValidPosition" ) );
    EXPECT_TRUE( doc->contains( "States" ) );
    EXPECT_TRUE( doc->is<nfx::json::Array>( "ValidPosition" ) );
    EXPECT_TRUE( doc->is<nfx::json::Array>( "States" ) );
}

TEST( EmbeddedTestDataTests, Load_NotFound )
{
    const auto doc = EmbeddedTestData::load<nfx::json::Document>( "NonExistent.json" );
    EXPECT_FALSE( doc.has_value() );
}
