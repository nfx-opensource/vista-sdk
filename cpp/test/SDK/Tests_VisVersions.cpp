/**
 * @file Tests_VisVersions.cpp
 * @brief Unit tests for VisVersion and VisVersions utilities
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VisVersions.h>

namespace dnv::vista::sdk::test
{
    //=====================================================================
    // VisVersions tests
    //=====================================================================

    //----------------------------------------------
    // String conversion
    //----------------------------------------------

    TEST( VisVersionsTests, ToString )
    {
        auto visVersion = VisVersion::v3_9a;
        auto visVersionString = VisVersions::toString( visVersion );

        EXPECT_EQ( "3-9a", visVersionString );
    }

    TEST( VisVersionsTests, FromString_OptionalOverload )
    {
        // Test valid versions
        auto v34a = VisVersions::fromString( "3-4a" );
        ASSERT_TRUE( v34a.has_value() );
        EXPECT_EQ( VisVersion::v3_4a, *v34a );

        auto v39a = VisVersions::fromString( "3-9a" );
        ASSERT_TRUE( v39a.has_value() );
        EXPECT_EQ( VisVersion::v3_9a, *v39a );

        // Test invalid version
        auto invalid = VisVersions::fromString( "invalid" );
        EXPECT_FALSE( invalid.has_value() );
    }

    TEST( VisVersionsTests, FromString_OutParameterOverload )
    {
        // Test valid versions
        VisVersion result;
        bool success = VisVersions::fromString( "3-4a", result );
        EXPECT_TRUE( success );
        EXPECT_EQ( VisVersion::v3_4a, result );

        success = VisVersions::fromString( "3-9a", result );
        EXPECT_TRUE( success );
        EXPECT_EQ( VisVersion::v3_9a, result );

        // Test invalid version
        success = VisVersions::fromString( "invalid", result );
        EXPECT_FALSE( success );
    }

    TEST( VisVersionsTests, ToString_InvalidEnum_ThrowsException )
    {
        auto invalidVersion = static_cast<VisVersion>( 9999 );

        EXPECT_THROW( VisVersions::toString( invalidVersion ), std::invalid_argument );
    }

    TEST( VisVersionsTests, FromString_EmptyString )
    {
        // Test optional overload
        auto result = VisVersions::fromString( "" );
        EXPECT_FALSE( result.has_value() );

        // Test out-parameter overload
        VisVersion version;
        bool success = VisVersions::fromString( "", version );
        EXPECT_FALSE( success );
    }

    TEST( VisVersionsTests, FromString_WhitespaceOnly )
    {
        // Test optional overload
        auto result = VisVersions::fromString( "   " );
        EXPECT_FALSE( result.has_value() );

        result = VisVersions::fromString( "\t" );
        EXPECT_FALSE( result.has_value() );

        result = VisVersions::fromString( "\n" );
        EXPECT_FALSE( result.has_value() );

        // Test out-parameter overload
        VisVersion version;
        bool success = VisVersions::fromString( "  ", version );
        EXPECT_FALSE( success );
    }

    TEST( VisVersionsTests, FromString_CaseSensitivity )
    {
        // Version strings are case-sensitive
        auto result = VisVersions::fromString( "3-9A" );
        EXPECT_FALSE( result.has_value() );

        result = VisVersions::fromString( "3-9a" );
        EXPECT_TRUE( result.has_value() );
    }
} // namespace dnv::vista::sdk::test
