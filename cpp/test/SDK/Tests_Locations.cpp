/**
 * @file Tests_Locations.cpp
 * @brief Unit tests for Locations functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <EmbeddedTestData.h>

#include <string>
#include <vector>

namespace dnv::vista::sdk::test
{
    using nfx::json::Document;
    using nfx::json::Array;
    using nfx::json::Object;

    //=====================================================================
    // Test data loading utilities
    //=====================================================================

    /**
     * @brief Test data structure for location parsing tests
     */
    struct LocationTestCase
    {
        std::string value;
        bool success;
        std::string output;
        std::vector<std::string> expectedErrorMessages;
    };

    /**
     * @brief Loads Location test cases from embedded test data
     * @return Vector of location test cases
     */
    static std::vector<LocationTestCase> LoadLocationTestData()
    {
        auto doc = EmbeddedTestData::load<Document>( "Locations.json" ).value();
        const auto& array = doc["locations"].rootRef<Array>().value().get();

        std::vector<LocationTestCase> result;
        result.reserve( array.size() );

        for( const auto& elem : array )
        {
            if( elem.type() != nfx::json::Type::Object )
                continue;

            const auto& item = elem.rootRef<Object>().value().get();

            LocationTestCase testCase;

            // Extract value (required)
            bool hasValue = false;
            for( const auto& [key, value] : item )
            {
                if( key == "value" && value.type() == nfx::json::Type::String )
                {
                    if( auto val = value.root<std::string>() )
                    {
                        testCase.value = *val;
                        hasValue = true;
                        break;
                    }
                }
            }
            if( !hasValue )
                continue;

            // Extract success (required)
            bool hasSuccess = false;
            for( const auto& [key, value] : item )
            {
                if( key == "success" && value.type() == nfx::json::Type::Boolean )
                {
                    if( auto val = value.root<bool>() )
                    {
                        testCase.success = *val;
                        hasSuccess = true;
                        break;
                    }
                }
            }
            if( !hasSuccess )
                continue;

            // Extract optional fields
            for( const auto& [key, value] : item )
            {
                if( key == "output" && value.type() == nfx::json::Type::String )
                {
                    if( auto val = value.root<std::string>() )
                        testCase.output = *val;
                }
                else if( key == "expectedErrorMessages" && value.type() == nfx::json::Type::Array )
                {
                    const auto& errArray = value.rootRef<Array>().value().get();
                    for( const auto& errElem : errArray )
                    {
                        if( errElem.type() == nfx::json::Type::String )
                        {
                            if( auto val = errElem.root<std::string>() )
                                testCase.expectedErrorMessages.push_back( *val );
                        }
                    }
                }
            }
            result.push_back( std::move( testCase ) );
        }
        return result;
    }

    //=====================================================================
    // Locations tests
    //=====================================================================

    TEST( LocationsTests, LoadsForAllVersions )
    {
        const auto& vis = VIS::instance();
        const auto& allVersions = vis.versions();

        for( const auto& visVersion : allVersions )
        {
            SCOPED_TRACE( "Testing VIS version: " + std::string( VisVersions::toString( visVersion ) ) );

            const auto& locations = vis.locations( visVersion );
            EXPECT_FALSE( locations.groups().empty() );
            EXPECT_FALSE( locations.relativeLocations().empty() );
        }
    }

    TEST( LocationsTests, EnumProperties )
    {
        std::vector<int> values = { static_cast<int>( LocationGroup::Number ),
                                    static_cast<int>( LocationGroup::Side ),
                                    static_cast<int>( LocationGroup::Vertical ),
                                    static_cast<int>( LocationGroup::Transverse ),
                                    static_cast<int>( LocationGroup::Longitudinal ) };

        std::set<int> uniqueValues( values.begin(), values.end() );
        EXPECT_EQ( values.size(), uniqueValues.size() );
        EXPECT_EQ( 5, values.size() );
        EXPECT_EQ( 0, static_cast<int>( LocationGroup::Number ) );

        for( size_t i = 0; i < values.size() - 1; i++ )
        {
            EXPECT_EQ( static_cast<int>( i ), values[i + 1] - 1 );
        }
    }

    TEST( LocationsTests, Locations )
    {
        const auto& locations = VIS::instance().locations( VisVersion::v3_4a );
        auto testCases = LoadLocationTestData();

        ASSERT_FALSE( testCases.empty() ) << "Failed to load test cases";

        for( const auto& testCase : testCases )
        {
            SCOPED_TRACE( "Testing location: '" + testCase.value + "'" );

            auto result = locations.fromString( testCase.value );

            ParsingErrors errors;
            auto resultWithErrors = locations.fromString( testCase.value, errors );

            if( !testCase.success )
            {
                EXPECT_FALSE( result.has_value() );
                EXPECT_FALSE( resultWithErrors.has_value() );
                EXPECT_TRUE( errors.hasErrors() );

                if( !testCase.expectedErrorMessages.empty() && errors.hasErrors() )
                {
                    std::vector<std::string> actualErrors;
                    actualErrors.reserve( errors.count() );

                    for( const auto& error : errors )
                    {
                        actualErrors.push_back( error.message );
                    }

                    EXPECT_EQ( testCase.expectedErrorMessages.size(), actualErrors.size() );

                    for( size_t i = 0; i < std::min( testCase.expectedErrorMessages.size(), actualErrors.size() ); ++i )
                    {
                        EXPECT_EQ( testCase.expectedErrorMessages[i], actualErrors[i] );
                    }
                }
            }
            else
            {
                ASSERT_TRUE( result.has_value() );
                ASSERT_TRUE( resultWithErrors.has_value() );
                EXPECT_FALSE( errors.hasErrors() );

                EXPECT_EQ( testCase.output, result->value() );
                EXPECT_EQ( testCase.output, resultWithErrors->value() );
            }
        }
    }

    TEST( LocationsTests, ParseThrowsOnNullOrEmpty )
    {
        const auto& locations = VIS::instance().locations( VisVersion::v3_4a );

        auto result1 = locations.fromString( "" );
        EXPECT_FALSE( result1.has_value() );

        auto result2 = locations.fromString( std::string_view{} );
        EXPECT_FALSE( result2.has_value() );
    }
} // namespace dnv::vista::sdk::test
