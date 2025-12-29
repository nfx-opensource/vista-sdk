/**
 * @file Tests_ISO19848.Versions.cpp
 * @brief Unit tests for ISO19848Version and ISO19848Versions utilities
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/transport/ISO19848Versions.h>

using namespace dnv::vista::sdk::transport;

namespace dnv::vista::sdk::test
{
    //=====================================================================
    // ISO19848Versions tests
    //=====================================================================

    //----------------------------------------------
    // String conversion
    //----------------------------------------------

    TEST( ISO19848VersionsTests, ToString )
    {
        auto version = ISO19848Version::v2024;
        auto versionString = ISO19848Versions::toString( version );

        EXPECT_EQ( "v2024", versionString );
    }

    TEST( ISO19848VersionsTests, FromString_OptionalOverload )
    {
        // Test valid versions
        auto v2018 = ISO19848Versions::fromString( "v2018" );
        ASSERT_TRUE( v2018.has_value() );
        EXPECT_EQ( ISO19848Version::v2018, *v2018 );

        auto v2024 = ISO19848Versions::fromString( "v2024" );
        ASSERT_TRUE( v2024.has_value() );
        EXPECT_EQ( ISO19848Version::v2024, *v2024 );

        // Test invalid version
        auto invalid = ISO19848Versions::fromString( "invalid" );
        EXPECT_FALSE( invalid.has_value() );
    }

    TEST( ISO19848VersionsTests, ToString_InvalidEnum_ThrowsException )
    {
        auto invalidVersion = static_cast<ISO19848Version>( 9999 );

        EXPECT_THROW( ISO19848Versions::toString( invalidVersion ), std::invalid_argument );
    }

    TEST( ISO19848VersionsTests, FromString_EmptyString )
    {
        auto result = ISO19848Versions::fromString( "" );
        EXPECT_FALSE( result.has_value() );
    }

    TEST( ISO19848VersionsTests, FromString_WhitespaceOnly )
    {
        auto result = ISO19848Versions::fromString( "   " );
        EXPECT_FALSE( result.has_value() );

        result = ISO19848Versions::fromString( "\t" );
        EXPECT_FALSE( result.has_value() );

        result = ISO19848Versions::fromString( "\n" );
        EXPECT_FALSE( result.has_value() );
    }

    TEST( ISO19848VersionsTests, FromString_CaseSensitivity )
    {
        // Version strings are case-sensitive
        auto result = ISO19848Versions::fromString( "V2024" );
        EXPECT_FALSE( result.has_value() );

        result = ISO19848Versions::fromString( "v2024" );
        EXPECT_TRUE( result.has_value() );

        result = ISO19848Versions::fromString( "2024" );
        EXPECT_FALSE( result.has_value() );
    }
} // namespace dnv::vista::sdk::test
