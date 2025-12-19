/**
 * @file Tests_ImoNumber.cpp
 * @brief Unit tests for ImoNumber functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <EmbeddedTestData.h>

namespace dnv::vista::sdk::test
{
    using nfx::json::Document;
    using nfx::json::Array;
    using nfx::json::Object;

    //=====================================================================
    // Test data loading utilities
    //=====================================================================

    /**
     * @brief Test data item from ImoNumbers.json
     */
    struct ImoNumberTestData
    {
        std::string value;
        bool success;
        std::optional<std::string> output;
    };

    /**
     * @brief Loads IMO number test cases from embedded test data
     * @return Vector of test data items
     */
    static std::vector<ImoNumberTestData> LoadImoNumberData()
    {
        auto doc = EmbeddedTestData::load<Document>( "ImoNumbers.json" ).value();
        const auto& arrayDoc = doc["imoNumbers"];

        if( arrayDoc.type() != nfx::json::Type::Array )
            return {};

        const auto& array = arrayDoc.rootRef<Array>().value().get();

        std::vector<ImoNumberTestData> result;
        result.reserve( array.size() );

        for( const auto& elem : array )
        {
            if( elem.type() != nfx::json::Type::Object )
                continue;

            const auto& obj = elem.rootRef<Object>().value().get();

            ImoNumberTestData data;

            for( const auto& [key, value] : obj )
            {
                if( key == "value" && value.type() == nfx::json::Type::String )
                {
                    if( auto val = value.root<std::string>() )
                        data.value = *val;
                }
                else if( key == "success" && value.type() == nfx::json::Type::Boolean )
                {
                    if( auto val = value.root<bool>() )
                        data.success = *val;
                }
                else if( key == "output" && value.type() == nfx::json::Type::String )
                {
                    if( auto val = value.root<std::string>() )
                        data.output = *val;
                }
            }
            result.push_back( data );
        }
        return result;
    }

    //=====================================================================
    // ImoNumber tests
    //=====================================================================

    class ImoNumberTest : public ::testing::TestWithParam<ImoNumberTestData>
    {
    };

    TEST_P( ImoNumberTest, Validation_TestCases )
    {
        auto testData = GetParam();

        auto parsedImo = ImoNumber::fromString( testData.value );

        if( testData.success )
        {
            ASSERT_TRUE( parsedImo.has_value() ) << "Expected successful parse for: " << testData.value;

            if( testData.output.has_value() )
            {
                EXPECT_EQ( parsedImo->toString(), testData.output.value() ) << "Input: \"" << testData.value << "\"";
            }
        }
        else
        {
            EXPECT_FALSE( parsedImo.has_value() ) << "Expected parse to fail for: " << testData.value;
        }
    }

    INSTANTIATE_TEST_SUITE_P( ImoNumberTests, ImoNumberTest, ::testing::ValuesIn( LoadImoNumberData() ) );

    //----------------------------------------------
    // isValid() tests
    //----------------------------------------------

    TEST( ImoNumberTests, IsValid_ValidNumbers )
    {
        EXPECT_TRUE( ImoNumber::isValid( 9074729 ) );
        EXPECT_TRUE( ImoNumber::isValid( 9785811 ) );
        EXPECT_TRUE( ImoNumber::isValid( 9704611 ) );
        EXPECT_TRUE( ImoNumber::isValid( 9074729 ) );
    }

    TEST( ImoNumberTests, IsValid_InvalidNumbers )
    {
        EXPECT_FALSE( ImoNumber::isValid( -1 ) );
        EXPECT_FALSE( ImoNumber::isValid( 0 ) );
        EXPECT_FALSE( ImoNumber::isValid( 1 ) );
        EXPECT_FALSE( ImoNumber::isValid( 123412034 ) ); // Too many digits
        EXPECT_FALSE( ImoNumber::isValid( 1234507 ) );   // Invalid checksum
    }

    //----------------------------------------------
    // Constructor tests
    //----------------------------------------------

    TEST( ImoNumberTests, Constructor_ValidInt )
    {
        EXPECT_NO_THROW( ImoNumber{ 9074729 } );
        auto imo = ImoNumber{ 9074729 };
        EXPECT_EQ( imo.toString(), "IMO9074729" );
    }

    TEST( ImoNumberTests, Constructor_InvalidInt_Throws )
    {
        EXPECT_THROW( ImoNumber{ 1234507 }, std::invalid_argument );
        EXPECT_THROW( ImoNumber{ 0 }, std::invalid_argument );
    }

    TEST( ImoNumberTests, Constructor_ValidString )
    {
        EXPECT_NO_THROW( ImoNumber{ "9074729" } );
        EXPECT_NO_THROW( ImoNumber{ "IMO9074729" } );

        auto imo1 = ImoNumber{ "9074729" };
        auto imo2 = ImoNumber{ "IMO9074729" };

        EXPECT_EQ( imo1.toString(), "IMO9074729" );
        EXPECT_EQ( imo2.toString(), "IMO9074729" );
        EXPECT_EQ( imo1, imo2 );
    }

    TEST( ImoNumberTests, Constructor_InvalidString_Throws )
    {
        EXPECT_THROW( ImoNumber{ "1234507" }, std::invalid_argument );
        EXPECT_THROW( ImoNumber{ "IM9074729" }, std::invalid_argument );
        EXPECT_THROW( ImoNumber{ "" }, std::invalid_argument );
        EXPECT_THROW( ImoNumber{ "abc" }, std::invalid_argument );
    }

    //----------------------------------------------
    // Comparison tests
    //----------------------------------------------

    TEST( ImoNumberTests, Equality_SameValues )
    {
        auto imo1 = ImoNumber{ 9074729 };
        auto imo2 = ImoNumber{ "IMO9074729" };
        auto imo3 = ImoNumber{ "9074729" };

        EXPECT_EQ( imo1, imo2 );
        EXPECT_EQ( imo2, imo3 );
        EXPECT_EQ( imo1, imo3 );
    }

    TEST( ImoNumberTests, Inequality_DifferentValues )
    {
        auto imo1 = ImoNumber{ 9074729 };
        auto imo2 = ImoNumber{ 9785811 };

        EXPECT_NE( imo1, imo2 );
    }

    //----------------------------------------------
    // toString() tests
    //----------------------------------------------

    TEST( ImoNumberTests, ToString_FormatWithPrefix )
    {
        auto imo = ImoNumber{ 9074729 };
        EXPECT_EQ( imo.toString(), "IMO9074729" );
    }
} // namespace dnv::vista::sdk::test
