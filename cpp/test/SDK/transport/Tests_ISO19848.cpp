/**
 * @file Tests_ISO19848.cpp
 * @brief ISO 19848 standard tests
 */

#include <algorithm>

#include <gtest/gtest.h>

#include <dnv/vista/sdk/transport/ISO19848.h>

namespace dnv::vista::sdk::tests
{
    //=====================================================================
    // Test fixture for ISO19848 tests
    //=====================================================================

    class ISO19848Tests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
        }

        void TearDown() override
        {
        }

        // Helper functions to reduce lambda verbosity
        static void unexpectedDecimal()
        {
            static_cast<void>( 0 );
            ADD_FAILURE() << "Expected specific type, got decimal";
        }

        static void unexpectedInteger()
        {
            static_cast<void>( 0 );
            ADD_FAILURE() << "Expected specific type, got integer";
        }

        static void unexpectedBoolean()
        {
            static_cast<void>( 0 );
            ADD_FAILURE() << "Expected specific type, got boolean";
        }

        static void unexpectedString()
        {
            static_cast<void>( 0 );
            ADD_FAILURE() << "Expected specific type, got string";
        }

        static void unexpectedDateTime()
        {
            static_cast<void>( 0 );
            ADD_FAILURE() << "Expected specific type, got datetime";
        }

        // Helper function to extract year from DateTime
        static std::string extractYearFromDateTimeOffset( const DateTime& datetime )
        {
            return "datetime:" + std::to_string( datetime.year() );
        }

        // Type-specific suppression helpers
        template <typename T>
        static void suppressUnused( const T& )
        {
            static_cast<void>( 0 );
        }
    };

    //=====================================================================
    // Parameterized test data
    //=====================================================================

    class ISO19848VersionTest : public ::testing::TestWithParam<transport::ISO19848Version>
    {
    };

    class DataChannelTypeNamesParseTest : public ::testing::TestWithParam<std::tuple<std::string, bool>>
    {
    };

    class FormatDataTypesParseTest : public ::testing::TestWithParam<std::tuple<std::string, bool>>
    {
    };

    class FormatDataTypeValidationTest : public ::testing::TestWithParam<std::tuple<std::string, std::string, bool>>
    {
    };

    //=====================================================================
    // Test_Instance
    //=====================================================================

    TEST_F( ISO19848Tests, Instance )
    {
        auto& iso = transport::ISO19848::instance();
        static_cast<void>( iso );
        SUCCEED();
    }

    //=====================================================================
    // Test_EmbeddedResource
    //=====================================================================

    TEST_F( ISO19848Tests, EmbeddedResources )
    {
        // Test that we can find and read ISO19848 embedded resources
        auto& iso = transport::ISO19848::instance();

        // Verify we can load data channel type names (uses embedded resources)
        auto dataChannelTypeNames = iso.dataChannelTypeNames( transport::ISO19848Version::v2024 );
        ASSERT_NE( dataChannelTypeNames.begin(), dataChannelTypeNames.end() );

        // Verify we can load format data types (uses embedded resources)
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );
        ASSERT_NE( formatDataTypes.begin(), formatDataTypes.end() );
    }

    //=====================================================================
    // Test_DataChannelTypeNames_Load
    //=====================================================================

    TEST_P( ISO19848VersionTest, DataChannelTypeNames_Load )
    {
        auto version = GetParam();
        auto& iso = transport::ISO19848::instance();
        auto dataChannelTypeNames = iso.dataChannelTypeNames( version );

        ASSERT_NE( dataChannelTypeNames.begin(), dataChannelTypeNames.end() );
    }

    INSTANTIATE_TEST_SUITE_P(
        ISO19848Tests,
        ISO19848VersionTest,
        ::testing::Values( transport::ISO19848Version::v2018, transport::ISO19848Version::v2024 ) );

    //=====================================================================
    // Test_DataChannelTypeNames_Parse_Self
    //=====================================================================

    TEST_F( ISO19848Tests, DataChannelTypeNames_Parse_Self )
    {
        auto& iso = transport::ISO19848::instance();
        auto dataChannelTypeNames = iso.dataChannelTypeNames( transport::ISO19848Version::v2024 );

        for( const auto& typeName : dataChannelTypeNames )
        {
            auto result = dataChannelTypeNames.fromString( typeName.type );
            ASSERT_TRUE( result.isOk() );
            ASSERT_TRUE( result.value.has_value() );
            ASSERT_EQ( typeName.type, result.value->type );
        }
    }

    //=====================================================================
    // Test_DataChannelTypeNames_Parse
    //=====================================================================

    TEST_P( DataChannelTypeNamesParseTest, DataChannelTypeNames_Parse )
    {
        auto [value, expectedResult] = GetParam();
        auto& iso = transport::ISO19848::instance();
        auto dataChannelTypeNames = iso.dataChannelTypeNames( transport::ISO19848Version::v2024 );
        auto result = dataChannelTypeNames.fromString( value );

        if( expectedResult )
        {
            ASSERT_TRUE( result.isOk() );
            ASSERT_TRUE( result.value.has_value() );
            ASSERT_EQ( value, result.value->type );
        }
        else
        {
            ASSERT_TRUE( result.isInvalid() );
        }
    }

    INSTANTIATE_TEST_SUITE_P(
        ISO19848Tests,
        DataChannelTypeNamesParseTest,
        ::testing::Values(
            std::make_tuple( "Inst", true ),
            std::make_tuple( "Average", true ),
            std::make_tuple( "Max", true ),
            std::make_tuple( "Min", true ),
            std::make_tuple( "Median", true ),
            std::make_tuple( "Mode", true ),
            std::make_tuple( "StandardDeviation", true ),
            std::make_tuple( "Calculated", true ),
            std::make_tuple( "SetPoint", true ),
            std::make_tuple( "Command", true ),
            std::make_tuple( "Alert", true ),
            std::make_tuple( "Status", true ),
            std::make_tuple( "ManualInput", true ),
            std::make_tuple( "manualInput", false ),
            std::make_tuple( "asd", false ),
            std::make_tuple( "some-random", false ),
            std::make_tuple( "InputManual", false ) ) );

    //=====================================================================
    // Test_FormatDataTypes_Load
    //=====================================================================

    TEST_P( ISO19848VersionTest, FormatDataTypes_Load )
    {
        auto version = GetParam();
        auto& iso = transport::ISO19848::instance();
        auto types = iso.formatDataTypes( version );

        ASSERT_NE( types.begin(), types.end() );
    }

    //=====================================================================
    // Test_FormatDataType_Parse_Self
    //=====================================================================

    TEST_F( ISO19848Tests, FormatDataType_Parse_Self )
    {
        auto& iso = transport::ISO19848::instance();
        auto types = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        for( const auto& typeName : types )
        {
            auto result = types.fromString( typeName.type );
            ASSERT_TRUE( result.isOk() );
            ASSERT_TRUE( result.value.has_value() );
            ASSERT_EQ( typeName.type, result.value->type );
        }
    }

    //=====================================================================
    // Test_FormatDataType_Parse
    //=====================================================================

    TEST_P( FormatDataTypesParseTest, FormatDataType_Parse )
    {
        auto [value, expectedResult] = GetParam();
        auto& iso = transport::ISO19848::instance();
        auto types = iso.formatDataTypes( transport::ISO19848Version::v2024 );
        auto result = types.fromString( value );

        if( expectedResult )
        {
            ASSERT_TRUE( result.isOk() );
            ASSERT_TRUE( result.value.has_value() );
            ASSERT_EQ( value, result.value->type );
        }
        else
        {
            ASSERT_TRUE( result.isInvalid() );
        }
    }

    INSTANTIATE_TEST_SUITE_P(
        ISO19848Tests,
        FormatDataTypesParseTest,
        ::testing::Values(
            std::make_tuple( "Decimal", true ),
            std::make_tuple( "Integer", true ),
            std::make_tuple( "Boolean", true ),
            std::make_tuple( "String", true ),
            std::make_tuple( "DateTime", true ),
            std::make_tuple( "decimal", false ),
            std::make_tuple( "string", false ),
            std::make_tuple( "asd", false ),
            std::make_tuple( "some-random", false ),
            std::make_tuple( "TimeDate", false ) ) );

    //=====================================================================
    // Test_FormatDataType_Parse_Valid
    //=====================================================================

    TEST_P( FormatDataTypeValidationTest, FormatDataType_Parse_Valid )
    {
        auto [type, value, expectedResult] = GetParam();
        auto& iso = transport::ISO19848::instance();
        auto types = iso.formatDataTypes( transport::ISO19848Version::v2024 );
        auto result = types.fromString( type );

        ASSERT_TRUE( result.isOk() );
        ASSERT_TRUE( result.value.has_value() );
        ASSERT_EQ( type, result.value->type );

        auto validateResult = result.value->validate( value );

        if( expectedResult )
        {
            ASSERT_TRUE( validateResult.isOk() );
        }
        else
        {
            ASSERT_TRUE( validateResult.isInvalid() );
        }
    }

    INSTANTIATE_TEST_SUITE_P(
        ISO19848Tests,
        FormatDataTypeValidationTest,
        ::testing::Values(
            std::make_tuple( "Decimal", "0.1", true ),
            std::make_tuple( "DateTime", "1994-11-20T10:25:33Z", true ),
            std::make_tuple( "DateTime", "1994-11-20T10", false ) ) );

    //=====================================================================
    // Test_FormatDataType_Match_Method
    //=====================================================================

    TEST_F( ISO19848Tests, FormatDataType_Match_Method )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        // Find the Integer format type for testing
        auto parseResult = formatDataTypes.fromString( "Integer" );
        ASSERT_TRUE( parseResult.isOk() );
        ASSERT_TRUE( parseResult.value.has_value() );
        auto integerType = *parseResult.value;

        // Test match method with valid integer - returns matched value
        int matchedValue = 0;
        ASSERT_NO_THROW( {
            matchedValue = integerType.match(
                "42",
                []( const Decimal& d ) -> int {
                    static_cast<void>( d );
                    ADD_FAILURE() << "Expected integer, got decimal";
                    return 0;
                },
                []( int i ) -> int { return i; },
                []( bool b ) -> int {
                    static_cast<void>( b );
                    ADD_FAILURE() << "Expected integer, got boolean";
                    return 0;
                },
                []( std::string_view s ) -> int {
                    static_cast<void>( s );
                    ADD_FAILURE() << "Expected integer, got string";
                    return 0;
                },
                []( const DateTime& tp ) -> int {
                    static_cast<void>( tp );
                    ADD_FAILURE() << "Expected integer, got datetime";
                    return 0;
                } );
        } );
        ASSERT_EQ( matchedValue, 42 );

        // Test match method with valid integer - returns string representation
        std::string matchResult;
        ASSERT_NO_THROW( {
            matchResult = integerType.match(
                "123",
                []( const Decimal& ) -> std::string {
                    ADD_FAILURE() << "Expected integer, got decimal";
                    return "";
                },
                []( int i ) -> std::string { return "Matched integer: " + std::to_string( i ); },
                []( bool ) -> std::string {
                    ADD_FAILURE() << "Expected integer, got boolean";
                    return "";
                },
                []( std::string_view ) -> std::string {
                    ADD_FAILURE() << "Expected integer, got string";
                    return "";
                },
                []( const DateTime& ) -> std::string {
                    ADD_FAILURE() << "Expected integer, got datetime";
                    return "";
                } );
        } );
        ASSERT_EQ( matchResult, "Matched integer: 123" );

        // Test that invalid values throw exceptions
        ASSERT_THROW(
            {
                static_cast<void>( integerType.match(
                    "not_a_number",
                    []( const Decimal& ) -> int { return 0; },
                    []( int i ) -> int { return i; },
                    []( bool ) -> int { return 0; },
                    []( std::string_view ) -> int { return 0; },
                    []( const DateTime& ) -> int { return 0; } ) );
            },
            std::invalid_argument );
    }

    //=====================================================================
    // Test_Match_All_Types
    //=====================================================================

    TEST_F( ISO19848Tests, Match_Decimal_Type )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        auto parseResult = formatDataTypes.fromString( "Decimal" );
        ASSERT_TRUE( parseResult.isOk() );
        ASSERT_TRUE( parseResult.value.has_value() );
        auto decimalType = *parseResult.value;

        bool decimalCalled = false;
        Decimal decimalValue;
        ASSERT_NO_THROW( {
            decimalType.match(
                "123.456",
                [&]( const Decimal& d ) {
                    decimalCalled = true;
                    decimalValue = d;
                },
                []( int ) { ADD_FAILURE() << "Expected decimal, got integer"; },
                []( bool ) { ADD_FAILURE() << "Expected decimal, got boolean"; },
                []( std::string_view ) { ADD_FAILURE() << "Expected decimal, got string"; },
                []( const DateTime& ) { ADD_FAILURE() << "Expected decimal, got datetime"; } );
        } );
        ASSERT_TRUE( decimalCalled );

        // Test with reasonable tolerance for precision
        Decimal expected( "123.456" );
        Decimal tolerance( "0.000001" );
        Decimal difference = ( decimalValue - expected ).abs();
        ASSERT_TRUE( difference < tolerance );
    }

    TEST_F( ISO19848Tests, Match_Integer_Type )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        auto parseResult = formatDataTypes.fromString( "Integer" );
        ASSERT_TRUE( parseResult.isOk() );
        ASSERT_TRUE( parseResult.value.has_value() );
        auto integerType = *parseResult.value;

        bool integerCalled = false;
        int integerValue = 0;
        ASSERT_NO_THROW( {
            integerType.match(
                "42",
                []( const Decimal& ) { ADD_FAILURE() << "Expected integer, got decimal"; },
                [&]( int i ) {
                    integerCalled = true;
                    integerValue = i;
                },
                []( bool ) { ADD_FAILURE() << "Expected integer, got boolean"; },
                []( std::string_view ) { ADD_FAILURE() << "Expected integer, got string"; },
                []( const DateTime& ) { ADD_FAILURE() << "Expected integer, got datetime"; } );
        } );
        ASSERT_TRUE( integerCalled );
        ASSERT_EQ( integerValue, 42 );
    }

    TEST_F( ISO19848Tests, Match_Boolean_Type )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        auto parseResult = formatDataTypes.fromString( "Boolean" );
        ASSERT_TRUE( parseResult.isOk() );
        ASSERT_TRUE( parseResult.value.has_value() );
        auto booleanType = *parseResult.value;

        bool booleanCalled = false;
        bool booleanValue = false;
        ASSERT_NO_THROW( {
            booleanType.match(
                "true",
                []( const Decimal& ) { ADD_FAILURE() << "Expected boolean, got decimal"; },
                []( int ) { ADD_FAILURE() << "Expected boolean, got integer"; },
                [&]( bool b ) {
                    booleanCalled = true;
                    booleanValue = b;
                },
                []( std::string_view ) { ADD_FAILURE() << "Expected boolean, got string"; },
                []( const DateTime& ) { ADD_FAILURE() << "Expected boolean, got datetime"; } );
        } );
        ASSERT_TRUE( booleanCalled );
        ASSERT_TRUE( booleanValue );
    }

    TEST_F( ISO19848Tests, Match_String_Type )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        auto parseResult = formatDataTypes.fromString( "String" );
        ASSERT_TRUE( parseResult.isOk() );
        ASSERT_TRUE( parseResult.value.has_value() );
        auto stringType = *parseResult.value;

        bool stringCalled = false;
        std::string stringValue;
        ASSERT_NO_THROW( {
            stringType.match(
                "Hello World",
                []( const Decimal& ) { ADD_FAILURE() << "Expected string, got decimal"; },
                []( int ) { ADD_FAILURE() << "Expected string, got integer"; },
                []( bool ) { ADD_FAILURE() << "Expected string, got boolean"; },
                [&]( std::string_view s ) {
                    stringCalled = true;
                    stringValue = std::string{ s };
                },
                []( const DateTime& ) { ADD_FAILURE() << "Expected string, got datetime"; } );
        } );
        ASSERT_TRUE( stringCalled );
        ASSERT_EQ( stringValue, "Hello World" );
    }

    TEST_F( ISO19848Tests, Match_DateTime_Type )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        auto parseResult = formatDataTypes.fromString( "DateTime" );
        ASSERT_TRUE( parseResult.isOk() );
        ASSERT_TRUE( parseResult.value.has_value() );
        auto dateTimeType = *parseResult.value;

        bool dateTimeCalled = false;
        DateTime dateTimeValue;
        ASSERT_NO_THROW( {
            dateTimeType.match(
                "1994-11-20T10:25:33Z",
                []( const Decimal& ) { ADD_FAILURE() << "Expected datetime, got decimal"; },
                []( int ) { ADD_FAILURE() << "Expected datetime, got integer"; },
                []( bool ) { ADD_FAILURE() << "Expected datetime, got boolean"; },
                []( std::string_view ) { ADD_FAILURE() << "Expected datetime, got string"; },
                [&]( const DateTime& datetime ) {
                    dateTimeCalled = true;
                    dateTimeValue = datetime;
                } );
        } );
        ASSERT_TRUE( dateTimeCalled );
        ASSERT_EQ( dateTimeValue.year(), 1994 );
    }

    TEST_F( ISO19848Tests, Match_Invalid_Values_Throw_Exception )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        // Test Decimal with invalid value
        auto decimalResult = formatDataTypes.fromString( "Decimal" );
        ASSERT_TRUE( decimalResult.isOk() );
        ASSERT_TRUE( decimalResult.value.has_value() );

        auto decimalType = *decimalResult.value;
        ASSERT_THROW(
            {
                decimalType.match(
                    "not_a_decimal",
                    []( const Decimal& ) {},
                    []( int ) {},
                    []( bool ) {},
                    []( std::string_view ) {},
                    []( const DateTime& ) {} );
            },
            std::invalid_argument );

        // Test Integer with invalid value
        auto integerResult = formatDataTypes.fromString( "Integer" );
        ASSERT_TRUE( integerResult.isOk() );
        ASSERT_TRUE( integerResult.value.has_value() );

        auto integerType = *integerResult.value;
        ASSERT_THROW(
            {
                integerType.match(
                    "not_an_integer",
                    []( const Decimal& ) {},
                    []( int ) {},
                    []( bool ) {},
                    []( std::string_view ) {},
                    []( const DateTime& ) {} );
            },
            std::invalid_argument );

        // Test Boolean with invalid value
        auto booleanResult = formatDataTypes.fromString( "Boolean" );
        ASSERT_TRUE( booleanResult.isOk() );
        ASSERT_TRUE( booleanResult.value.has_value() );

        auto booleanType = *booleanResult.value;
        ASSERT_THROW(
            {
                booleanType.match(
                    "not_a_boolean",
                    []( const Decimal& ) {},
                    []( int ) {},
                    []( bool ) {},
                    []( std::string_view ) {},
                    []( const DateTime& ) {} );
            },
            std::invalid_argument );

        // Test DateTime with invalid value
        auto dateTimeResult = formatDataTypes.fromString( "DateTime" );
        ASSERT_TRUE( dateTimeResult.isOk() );
        ASSERT_TRUE( dateTimeResult.value.has_value() );

        auto dateTimeType = *dateTimeResult.value;
        ASSERT_THROW(
            {
                dateTimeType.match(
                    "not_a_datetime",
                    []( const Decimal& ) {},
                    []( int ) {},
                    []( bool ) {},
                    []( std::string_view ) {},
                    []( const DateTime& ) {} );
            },
            std::invalid_argument );
    }

    TEST_F( ISO19848Tests, Match_Edge_Cases )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        // Test negative decimal
        auto decimalResult = formatDataTypes.fromString( "Decimal" );
        ASSERT_TRUE( decimalResult.isOk() );
        ASSERT_TRUE( decimalResult.value.has_value() );

        auto decimalType = *decimalResult.value;
        bool negativeCalled = false;
        Decimal negativeValue;
        ASSERT_NO_THROW( {
            decimalType.match(
                "-456.789",
                [&]( const Decimal& d ) {
                    negativeCalled = true;
                    negativeValue = d;
                },
                []( int ) { ADD_FAILURE() << "Expected decimal, got integer"; },
                []( bool ) { ADD_FAILURE() << "Expected decimal, got boolean"; },
                []( std::string_view ) { ADD_FAILURE() << "Expected decimal, got string"; },
                []( const DateTime& ) { ADD_FAILURE() << "Expected decimal, got datetime"; } );
        } );
        ASSERT_TRUE( negativeCalled );

        Decimal expected( "-456.789" );
        Decimal tolerance( "0.000001" );
        Decimal difference = ( negativeValue - expected ).abs();
        ASSERT_TRUE( difference < tolerance );

        // Test negative integer
        auto integerResult = formatDataTypes.fromString( "Integer" );
        ASSERT_TRUE( integerResult.isOk() );
        ASSERT_TRUE( integerResult.value.has_value() );

        auto integerType = *integerResult.value;
        bool negativeIntCalled = false;
        int negativeIntValue = 0;
        ASSERT_NO_THROW( {
            integerType.match(
                "-123",
                []( const Decimal& ) { ADD_FAILURE() << "Expected integer, got decimal"; },
                [&]( int i ) {
                    negativeIntCalled = true;
                    negativeIntValue = i;
                },
                []( bool ) { ADD_FAILURE() << "Expected integer, got boolean"; },
                []( std::string_view ) { ADD_FAILURE() << "Expected integer, got string"; },
                []( const DateTime& ) { ADD_FAILURE() << "Expected integer, got datetime"; } );
        } );
        ASSERT_TRUE( negativeIntCalled );
        ASSERT_EQ( negativeIntValue, -123 );

        // Test false boolean
        auto booleanResult = formatDataTypes.fromString( "Boolean" );
        ASSERT_TRUE( booleanResult.isOk() );
        ASSERT_TRUE( booleanResult.value.has_value() );

        auto booleanType = *booleanResult.value;

        bool falseCalled = false;
        bool falseValue = true;
        ASSERT_NO_THROW( {
            booleanType.match(
                "false",
                []( const Decimal& ) { ADD_FAILURE() << "Expected boolean, got decimal"; },
                []( int ) { ADD_FAILURE() << "Expected boolean, got integer"; },
                [&]( bool b ) {
                    falseCalled = true;
                    falseValue = b;
                },
                []( std::string_view ) { ADD_FAILURE() << "Expected boolean, got string"; },
                []( const DateTime& ) { ADD_FAILURE() << "Expected boolean, got datetime"; } );
        } );
        ASSERT_TRUE( falseCalled );
        ASSERT_FALSE( falseValue );

        // Test empty string
        auto stringResult = formatDataTypes.fromString( "String" );
        ASSERT_TRUE( stringResult.isOk() );
        ASSERT_TRUE( stringResult.value.has_value() );

        auto stringType = *stringResult.value;

        bool emptyCalled = false;
        std::string emptyValue = "not_empty";
        ASSERT_NO_THROW( {
            stringType.match(
                "",
                []( const Decimal& ) { ADD_FAILURE() << "Expected string, got decimal"; },
                []( int ) { ADD_FAILURE() << "Expected string, got integer"; },
                []( bool ) { ADD_FAILURE() << "Expected string, got boolean"; },
                [&]( std::string_view s ) {
                    emptyCalled = true;
                    emptyValue = std::string{ s };
                },
                []( const DateTime& ) { ADD_FAILURE() << "Expected string, got datetime"; } );
        } );
        ASSERT_TRUE( emptyCalled );
        ASSERT_TRUE( emptyValue.empty() );
    }

    //=====================================================================
    // Test_Match_Return_Values
    //=====================================================================

    TEST_F( ISO19848Tests, Match_Decimal_Return_Values )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        auto parseResult = formatDataTypes.fromString( "Decimal" );
        ASSERT_TRUE( parseResult.isOk() );
        ASSERT_TRUE( parseResult.value.has_value() );

        auto decimalType = *parseResult.value;

        // Test match with decimal value - returns string representation
        std::string result;
        ASSERT_NO_THROW( {
            result = decimalType.match(
                "123.456",
                []( const Decimal& d ) -> std::string { return "decimal:" + d.toString(); },
                []( int ) -> std::string {
                    unexpectedInteger();
                    return "";
                },
                []( bool ) -> std::string {
                    unexpectedBoolean();
                    return "";
                },
                []( std::string_view ) -> std::string {
                    unexpectedString();
                    return "";
                },
                []( const DateTime& ) -> std::string {
                    unexpectedDateTime();
                    return "";
                } );
        } );

        // Test with reasonable tolerance for the result comparison
        std::string expected = "decimal:" + Decimal( "123.456" ).toString();
        ASSERT_EQ( result, expected );

        // Test match with decimal value - returns double
        double doubleResult = 0.0;
        ASSERT_NO_THROW( {
            doubleResult = decimalType.match(
                "987.654",
                []( const Decimal& d ) -> double { return d.toDouble(); },
                []( int ) -> double {
                    unexpectedInteger();
                    return 0.0;
                },
                []( bool ) -> double {
                    unexpectedBoolean();
                    return 0.0;
                },
                []( std::string_view ) -> double {
                    unexpectedString();
                    return 0.0;
                },
                []( const DateTime& ) -> double {
                    unexpectedDateTime();
                    return 0.0;
                } );
        } );
        ASSERT_NEAR( doubleResult, 987.654, 0.000001 );
    }

    TEST_F( ISO19848Tests, Match_Integer_Function )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        auto parseResult = formatDataTypes.fromString( "Integer" );
        ASSERT_TRUE( parseResult.isOk() );
        ASSERT_TRUE( parseResult.value.has_value() );

        auto integerType = *parseResult.value;
        std::string result;
        ASSERT_NO_THROW( {
            result = integerType.match(
                "42",
                []( const Decimal& ) -> std::string {
                    unexpectedDecimal();
                    return "";
                },
                []( int i ) -> std::string { return "integer:" + std::to_string( i ); },
                []( bool ) -> std::string {
                    unexpectedBoolean();
                    return "";
                },
                []( std::string_view ) -> std::string {
                    unexpectedString();
                    return "";
                },
                []( const DateTime& ) -> std::string {
                    unexpectedDateTime();
                    return "";
                } );
        } );
        ASSERT_EQ( result, "integer:42" );

        // Test match with integer value - returns squared value
        int squaredResult = 0;
        ASSERT_NO_THROW( {
            squaredResult = integerType.match(
                "7",
                []( const Decimal& ) -> int {
                    unexpectedDecimal();
                    return 0;
                },
                []( int i ) -> int { return i * i; },
                []( bool ) -> int {
                    unexpectedBoolean();
                    return 0;
                },
                []( std::string_view ) -> int {
                    unexpectedString();
                    return 0;
                },
                []( const DateTime& ) -> int {
                    unexpectedDateTime();
                    return 0;
                } );
        } );
        ASSERT_EQ( squaredResult, 49 );
    }

    TEST_F( ISO19848Tests, Match_Boolean_Function )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        auto parseResult = formatDataTypes.fromString( "Boolean" );
        ASSERT_TRUE( parseResult.isOk() );
        ASSERT_TRUE( parseResult.value.has_value() );
        auto booleanType = *parseResult.value;
        std::string result;
        ASSERT_NO_THROW( {
            result = booleanType.match(
                "true",
                []( const Decimal& ) -> std::string {
                    unexpectedDecimal();
                    return "";
                },
                []( int ) -> std::string {
                    unexpectedInteger();
                    return "";
                },
                []( bool b ) -> std::string { return b ? "boolean:true" : "boolean:false"; },
                []( std::string_view ) -> std::string {
                    unexpectedString();
                    return "";
                },
                []( const DateTime& ) -> std::string {
                    unexpectedDateTime();
                    return "";
                } );
        } );
        ASSERT_EQ( result, "boolean:true" );

        // Test match with boolean false - returns inverted value
        bool invertedResult = true;
        ASSERT_NO_THROW( {
            invertedResult = booleanType.match(
                "false",
                []( const Decimal& ) -> bool {
                    unexpectedDecimal();
                    return false;
                },
                []( int ) -> bool {
                    unexpectedInteger();
                    return false;
                },
                []( bool b ) -> bool { return !b; },
                []( std::string_view ) -> bool {
                    unexpectedString();
                    return false;
                },
                []( const DateTime& ) -> bool {
                    unexpectedDateTime();
                    return false;
                } );
        } );
        ASSERT_TRUE( invertedResult );
    }

    TEST_F( ISO19848Tests, Match_String_Function )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        auto parseResult = formatDataTypes.fromString( "String" );
        ASSERT_TRUE( parseResult.isOk() );
        ASSERT_TRUE( parseResult.value.has_value() );

        auto stringType = *parseResult.value;
        size_t lengthResult = 0;
        ASSERT_NO_THROW( {
            lengthResult = stringType.match(
                "Hello World",
                []( const Decimal& ) -> size_t {
                    unexpectedDecimal();
                    return 0;
                },
                []( int ) -> size_t {
                    unexpectedInteger();
                    return 0;
                },
                []( bool ) -> size_t {
                    unexpectedBoolean();
                    return 0;
                },
                []( std::string_view s ) -> size_t { return s.length(); },
                []( const DateTime& ) -> size_t {
                    unexpectedDateTime();
                    return 0;
                } );
        } );
        ASSERT_EQ( lengthResult, 11 );

        // Test match with string value - returns uppercase
        std::string uppercaseResult;
        ASSERT_NO_THROW( {
            uppercaseResult = stringType.match(
                "hello",
                []( const Decimal& ) -> std::string {
                    unexpectedDecimal();
                    return "";
                },
                []( int ) -> std::string {
                    unexpectedInteger();
                    return "";
                },
                []( bool ) -> std::string {
                    unexpectedBoolean();
                    return "";
                },
                []( std::string_view s ) -> std::string {
                    std::string result( s );
                    std::transform( result.begin(), result.end(), result.begin(), []( unsigned char c ) {
                        return static_cast<char>( std::toupper( c ) );
                    } );
                    return "string:" + result;
                },
                []( const DateTime& ) -> std::string {
                    unexpectedDateTime();
                    return "";
                } );
        } );
        ASSERT_EQ( uppercaseResult, "string:HELLO" );
    }

    TEST_F( ISO19848Tests, Match_DateTime_Function )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        auto parseResult = formatDataTypes.fromString( "DateTime" );
        ASSERT_TRUE( parseResult.isOk() );
        ASSERT_TRUE( parseResult.value.has_value() );

        auto dateTimeType = *parseResult.value;
        std::string result;
        ASSERT_NO_THROW( {
            result = dateTimeType.match(
                "1994-11-20T10:25:33Z",
                []( const Decimal& ) -> std::string {
                    unexpectedDecimal();
                    return "";
                },
                []( int ) -> std::string {
                    unexpectedInteger();
                    return "";
                },
                []( bool ) -> std::string {
                    unexpectedBoolean();
                    return "";
                },
                []( std::string_view ) -> std::string {
                    unexpectedString();
                    return "";
                },
                []( const DateTime& datetime ) -> std::string { return extractYearFromDateTimeOffset( datetime ); } );
        } );
        ASSERT_EQ( result, "datetime:1994" );

        // Test match with datetime value - returns time_point duration
        bool durationResult = false;
        ASSERT_NO_THROW( {
            durationResult = dateTimeType.match(
                "1994-11-20T10:25:33Z",
                []( const Decimal& ) -> bool {
                    unexpectedDecimal();
                    return false;
                },
                []( int ) -> bool {
                    unexpectedInteger();
                    return false;
                },
                []( bool ) -> bool {
                    unexpectedBoolean();
                    return false;
                },
                []( std::string_view ) -> bool {
                    unexpectedString();
                    return false;
                },
                []( const DateTime& datetime ) -> bool {
                    // Just verify we got a valid DateTimeOffset with the expected year
                    return datetime.year() == 1994;
                } );
        } );
        ASSERT_TRUE( durationResult );
    }

    TEST_F( ISO19848Tests, Match_Edge_Cases_And_Return_Types )
    {
        auto& iso = transport::ISO19848::instance();
        auto formatDataTypes = iso.formatDataTypes( transport::ISO19848Version::v2024 );

        // Test negative decimal with custom struct return type
        struct DecimalInfo
        {
            Decimal value;
            bool isNegative;

            std::string toString() const
            {
                return isNegative ? "negative:" + value.toString() : "positive:" + value.toString();
            }
        };

        auto decimalResult = formatDataTypes.fromString( "Decimal" );
        ASSERT_TRUE( decimalResult.isOk() );

        ASSERT_TRUE( decimalResult.value.has_value() );

        auto decimalType = *decimalResult.value;
        DecimalInfo info;
        ASSERT_NO_THROW( {
            info = decimalType.match(
                "-456.789",
                []( const Decimal& d ) -> DecimalInfo { return { d, d < Decimal{ 0 } }; },
                []( int ) -> DecimalInfo {
                    unexpectedInteger();
                    return {};
                },
                []( bool ) -> DecimalInfo {
                    unexpectedBoolean();
                    return {};
                },
                []( std::string_view ) -> DecimalInfo {
                    unexpectedString();
                    return {};
                },
                []( const DateTime& ) -> DecimalInfo {
                    unexpectedDateTime();
                    return {};
                } );
        } );

        // Test with reasonable tolerance for IEEE 754 precision limits
        Decimal expected( "-456.789" );
        Decimal tolerance( "0.000001" );
        Decimal difference = ( info.value - expected ).abs();
        ASSERT_TRUE( difference < tolerance );
        ASSERT_TRUE( info.isNegative );
        ASSERT_EQ( info.toString(), "negative:" + expected.toString() );

        // Test integer with complex calculation return
        auto integerResult = formatDataTypes.fromString( "Integer" );
        ASSERT_TRUE( integerResult.isOk() );
        ASSERT_TRUE( integerResult.value.has_value() );

        auto integerType = *integerResult.value;
        std::vector<int> fibResult;
        ASSERT_NO_THROW( {
            fibResult = integerType.match(
                "5",
                []( const Decimal& ) -> std::vector<int> {
                    unexpectedDecimal();
                    return {};
                },
                []( int n ) -> std::vector<int> {
                    // Generate fibonacci sequence up to n
                    std::vector<int> fib;
                    if( n <= 0 )
                    {
                        return fib;
                    }
                    fib.push_back( 0 );
                    if( n > 1 )
                    {
                        fib.push_back( 1 );
                    }
                    for( int i = 2; i < n; ++i )
                    {
                        fib.push_back( fib[static_cast<size_t>( i - 1 )] + fib[static_cast<size_t>( i - 2 )] );
                    }
                    return fib;
                },
                []( bool ) -> std::vector<int> {
                    unexpectedBoolean();
                    return {};
                },
                []( std::string_view ) -> std::vector<int> {
                    unexpectedString();
                    return {};
                },
                []( const DateTime& ) -> std::vector<int> {
                    unexpectedDateTime();
                    return {};
                } );
        } );
        ASSERT_EQ( fibResult.size(), 5 );
        ASSERT_EQ( fibResult[0], 0 );
        ASSERT_EQ( fibResult[1], 1 );
        ASSERT_EQ( fibResult[2], 1 );
        ASSERT_EQ( fibResult[3], 2 );
        ASSERT_EQ( fibResult[4], 3 );

        // Test empty string with optional return type
        auto stringResult = formatDataTypes.fromString( "String" );
        ASSERT_TRUE( stringResult.isOk() );
        ASSERT_TRUE( stringResult.value.has_value() );

        auto stringType = *stringResult.value;

        std::optional<std::string> optionalResult;
        ASSERT_NO_THROW( {
            optionalResult = stringType.match(
                "",
                []( const Decimal& ) -> std::optional<std::string> {
                    unexpectedDecimal();
                    return std::nullopt;
                },
                []( int ) -> std::optional<std::string> {
                    unexpectedInteger();
                    return std::nullopt;
                },
                []( bool ) -> std::optional<std::string> {
                    unexpectedBoolean();
                    return std::nullopt;
                },
                []( std::string_view s ) -> std::optional<std::string> {
                    return s.empty() ? std::nullopt : std::make_optional( std::string{ s } );
                },
                []( const DateTime& ) -> std::optional<std::string> {
                    unexpectedDateTime();
                    return std::nullopt;
                } );
        } );
        ASSERT_FALSE( optionalResult.has_value() );
    }

    //=====================================================================
    // Test_FormatDataType_Value_Types_API
    //=====================================================================

    TEST_F( ISO19848Tests, FormatDataType_Value_Types_API )
    {
        // Test all Value types for complete API coverage

        // Test String Value
        {
            transport::Value value = transport::Value::String{ "test" };
            ASSERT_TRUE( value.string().has_value() );
            ASSERT_FALSE( value.decimal().has_value() );
            ASSERT_FALSE( value.integer().has_value() );
            ASSERT_FALSE( value.boolean().has_value() );
            ASSERT_FALSE( value.dateTime().has_value() );
            ASSERT_TRUE( value.string().has_value() );
            ASSERT_EQ( value.string().value(), "test" );
        }

        // Test Decimal Value
        {
            transport::Value value = transport::Value::Decimal{ 3.14 };
            ASSERT_FALSE( value.string().has_value() );
            ASSERT_TRUE( value.decimal().has_value() );
            ASSERT_FALSE( value.integer().has_value() );
            ASSERT_FALSE( value.boolean().has_value() );
            ASSERT_FALSE( value.dateTime().has_value() );
            ASSERT_TRUE( value.decimal().has_value() );
            ASSERT_EQ( value.decimal().value(), Decimal( 3.14 ) );
        }

        // Test Integer Value
        {
            transport::Value value = transport::Value::Integer{ 42 };
            ASSERT_FALSE( value.string().has_value() );
            ASSERT_FALSE( value.decimal().has_value() );
            ASSERT_TRUE( value.integer().has_value() );
            ASSERT_FALSE( value.boolean().has_value() );
            ASSERT_FALSE( value.dateTime().has_value() );
            ASSERT_TRUE( value.integer().has_value() );
            ASSERT_EQ( value.integer().value(), 42 );
        }

        // Test Boolean Value
        {
            transport::Value value = transport::Value::Boolean{ true };
            ASSERT_FALSE( value.string().has_value() );
            ASSERT_FALSE( value.decimal().has_value() );
            ASSERT_FALSE( value.integer().has_value() );
            ASSERT_TRUE( value.boolean().has_value() );
            ASSERT_FALSE( value.dateTime().has_value() );
            ASSERT_TRUE( value.boolean().has_value() );
            ASSERT_EQ( value.boolean().value(), true );
        }

        // Test DateTime Value
        {
            auto dateTimeOffset = DateTime::now();
            transport::Value value = transport::Value::DateTime{ dateTimeOffset };
            ASSERT_FALSE( value.string().has_value() );
            ASSERT_FALSE( value.decimal().has_value() );
            ASSERT_FALSE( value.integer().has_value() );
            ASSERT_FALSE( value.boolean().has_value() );
            ASSERT_TRUE( value.dateTime().has_value() );
            ASSERT_TRUE( value.dateTime().has_value() );
            ASSERT_EQ( value.dateTime().value(), dateTimeOffset );
        }
    }

    //=====================================================================
    // Test_Value_Type_Accessor
    //=====================================================================

    TEST_F( ISO19848Tests, Value_Type_Accessor )
    {
        // Test that Value::type() returns correct type enum for each wrapper

        // String type
        {
            transport::Value value = transport::Value::String{ "test" };
            ASSERT_EQ( value.type(), transport::Value::Type::String );
        }

        // Decimal type
        {
            transport::Value value = transport::Value::Decimal{ 3.14 };
            ASSERT_EQ( value.type(), transport::Value::Type::Decimal );
        }

        // Integer type
        {
            transport::Value value = transport::Value::Integer{ 42 };
            ASSERT_EQ( value.type(), transport::Value::Type::Integer );
        }

        // Boolean type
        {
            transport::Value value = transport::Value::Boolean{ true };
            ASSERT_EQ( value.type(), transport::Value::Type::Boolean );
        }

        // DateTime type
        {
            transport::Value value = transport::Value::DateTime{ DateTime::now() };
            ASSERT_EQ( value.type(), transport::Value::Type::DateTime );
        }
    }

    //=====================================================================
    // Test_Value_Semantics
    //=====================================================================

    TEST_F( ISO19848Tests, Value_Copy_Semantics )
    {
        // Test copy construction and copy assignment for all Value types

        // String value copy
        {
            transport::Value original = transport::Value::String{ "original" };
            transport::Value copy = original; // copy constructor
            ASSERT_TRUE( copy.string().has_value() );
            ASSERT_EQ( copy.string().value(), "original" );
            ASSERT_EQ( original.string().value(), copy.string().value() );

            transport::Value assigned = transport::Value::Integer{ 0 };
            assigned = original; // copy assignment
            ASSERT_TRUE( assigned.string().has_value() );
            ASSERT_EQ( assigned.string().value(), "original" );
        }

        // Integer value copy
        {
            transport::Value original = transport::Value::Integer{ 42 };
            transport::Value copy = original;
            ASSERT_TRUE( copy.integer().has_value() );
            ASSERT_EQ( copy.integer().value(), 42 );
            ASSERT_EQ( original.integer().value(), copy.integer().value() );
        }

        // Decimal value copy
        {
            transport::Value original = transport::Value::Decimal{ 3.14 };
            transport::Value copy = original;
            ASSERT_TRUE( copy.decimal().has_value() );
            ASSERT_EQ( copy.decimal().value(), Decimal( 3.14 ) );
            ASSERT_EQ( original.decimal().value(), copy.decimal().value() );
        }

        // Boolean value copy
        {
            transport::Value original = transport::Value::Boolean{ true };
            transport::Value copy = original;
            ASSERT_TRUE( copy.boolean().has_value() );
            ASSERT_EQ( copy.boolean().value(), true );
            ASSERT_EQ( original.boolean().value(), copy.boolean().value() );
        }

        // DateTime value copy
        {
            auto datetime = DateTime::now();
            transport::Value original = transport::Value::DateTime{ datetime };
            transport::Value copy = original;
            ASSERT_TRUE( copy.dateTime().has_value() );
            ASSERT_EQ( copy.dateTime().value(), datetime );
            ASSERT_EQ( original.dateTime().value(), copy.dateTime().value() );
        }
    }

    TEST_F( ISO19848Tests, Value_Move_Semantics )
    {
        // Test move construction and move assignment for all Value types

        // String value move
        {
            transport::Value original = transport::Value::String{ "original" };
            transport::Value moved = std::move( original ); // move constructor
            ASSERT_TRUE( moved.string().has_value() );
            ASSERT_EQ( moved.string().value(), "original" );

            transport::Value moveAssigned = transport::Value::Integer{ 0 };
            transport::Value source = transport::Value::String{ "move_assigned" };
            moveAssigned = std::move( source ); // move assignment
            ASSERT_TRUE( moveAssigned.string().has_value() );
            ASSERT_EQ( moveAssigned.string().value(), "move_assigned" );
        }

        // Integer value move
        {
            transport::Value original = transport::Value::Integer{ 42 };
            transport::Value moved = std::move( original );
            ASSERT_TRUE( moved.integer().has_value() );
            ASSERT_EQ( moved.integer().value(), 42 );
        }

        // Decimal value move
        {
            transport::Value original = transport::Value::Decimal{ 3.14 };
            transport::Value moved = std::move( original );
            ASSERT_TRUE( moved.decimal().has_value() );
            ASSERT_EQ( moved.decimal().value(), Decimal( 3.14 ) );
        }

        // Boolean value move
        {
            transport::Value original = transport::Value::Boolean{ false };
            transport::Value moved = std::move( original );
            ASSERT_TRUE( moved.boolean().has_value() );
            ASSERT_EQ( moved.boolean().value(), false );
        }

        // DateTime value move
        {
            auto datetime = DateTime::now();
            transport::Value original = transport::Value::DateTime{ datetime };
            transport::Value moved = std::move( original );
            ASSERT_TRUE( moved.dateTime().has_value() );
            ASSERT_EQ( moved.dateTime().value(), datetime );
        }
    }
} // namespace dnv::vista::sdk::tests
