/**
 * @file Tests_DataChannelId.cpp
 * @brief Unit tests for DataChannelId class
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/transport/timeseries/DataChannelId.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::tests
{
    //=====================================================================
    // DataChannelId Construction Tests
    //=====================================================================

    TEST( DataChannelId, ConstructFromLocalId )
    {
        std::string validLocalIdString =
            "/dnv-v2/vis-3-4a/411.1/C101.62/S205/meta/qty-temperature/cnt-lubricating.oil/pos-inlet";
        auto localIdOpt = LocalIdBuilder::fromString( validLocalIdString );
        ASSERT_TRUE( localIdOpt.has_value() );

        auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString( localIdOpt->toString() );
        ASSERT_TRUE( dataChannelIdOpt.has_value() );
        auto& dataChannelId = *dataChannelIdOpt;
        EXPECT_TRUE( dataChannelId.isLocalId() );
        EXPECT_FALSE( dataChannelId.isShortId() );

        auto retrievedLocalId = dataChannelId.localId();
        ASSERT_TRUE( retrievedLocalId.has_value() );
        EXPECT_EQ( retrievedLocalId.value().get(), *localIdOpt );
        auto shortId = dataChannelId.shortId();
        EXPECT_FALSE( shortId.has_value() );
    }

    TEST( DataChannelId, ConstructFromStringView )
    {
        std::string_view testId = "CH001";
        auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString( testId );
        ASSERT_TRUE( dataChannelIdOpt.has_value() );
        auto dataChannelId = *dataChannelIdOpt;
        EXPECT_FALSE( dataChannelId.isLocalId() );
        EXPECT_TRUE( dataChannelId.isShortId() );

        auto retrievedShortId = dataChannelId.shortId();
        ASSERT_TRUE( retrievedShortId.has_value() );
        EXPECT_EQ( retrievedShortId.value(), testId );

        auto localId = dataChannelId.localId();
        EXPECT_FALSE( localId.has_value() );
    }

    //=====================================================================
    // DataChannelId Equality Tests
    //=====================================================================

    TEST( DataChannelId, EqualityLocalIds )
    {
        std::string validLocalIdString =
            "/dnv-v2/vis-3-4a/411.1/C101.62/S205/meta/qty-temperature/cnt-lubricating.oil/pos-inlet";
        auto localId1Opt = LocalIdBuilder::fromString( validLocalIdString );
        auto localId2Opt = LocalIdBuilder::fromString( validLocalIdString );

        ASSERT_TRUE( localId1Opt.has_value() );
        ASSERT_TRUE( localId2Opt.has_value() );

        auto dataChannelId1 = *transport::timeseries::DataChannelId::fromString( localId1Opt->toString() );
        auto dataChannelId2 = *transport::timeseries::DataChannelId::fromString( localId2Opt->toString() );

        EXPECT_EQ( dataChannelId1, dataChannelId2 );
        EXPECT_FALSE( dataChannelId1 != dataChannelId2 );
    }

    TEST( DataChannelId, EqualityShortIds )
    {
        auto dataChannelId1 = *transport::timeseries::DataChannelId::fromString( "CH001" );
        auto dataChannelId2 = *transport::timeseries::DataChannelId::fromString( "CH001" );

        EXPECT_EQ( dataChannelId1, dataChannelId2 );
        EXPECT_FALSE( dataChannelId1 != dataChannelId2 );
    }

    TEST( DataChannelId, InequalityDifferentTypes )
    {
        std::string validLocalIdString = "/dnv-v2/vis-3-4a/411.1/C101.44i-5A/C261/meta/qty-temperature";
        auto localIdOpt = LocalIdBuilder::fromString( validLocalIdString );

        ASSERT_TRUE( localIdOpt.has_value() );

        auto localIdDataChannelId = *transport::timeseries::DataChannelId::fromString( localIdOpt->toString() );
        auto shortIdDataChannelId = *transport::timeseries::DataChannelId::fromString( "CH001" );
        EXPECT_NE( localIdDataChannelId, shortIdDataChannelId );
        EXPECT_TRUE( localIdDataChannelId != shortIdDataChannelId );
    }

    TEST( DataChannelId, InequalityDifferentValues )
    {
        auto dataChannelId1 = *transport::timeseries::DataChannelId::fromString( "CH001" );
        auto dataChannelId2 = *transport::timeseries::DataChannelId::fromString( "CH002" );

        EXPECT_NE( dataChannelId1, dataChannelId2 );
        EXPECT_TRUE( dataChannelId1 != dataChannelId2 );
    }

    //=====================================================================
    // DataChannelId ToString Tests
    //=====================================================================

    TEST( DataChannelId, ToStringLocalId )
    {
        std::string validLocalIdString = "/dnv-v2/vis-3-4a/411.1/C101.44i-6A/C261/meta/qty-temperature";
        auto localIdOpt = LocalIdBuilder::fromString( validLocalIdString );

        ASSERT_TRUE( localIdOpt.has_value() );

        auto dataChannelId = *transport::timeseries::DataChannelId::fromString( localIdOpt->toString() );

        std::string result = dataChannelId.toString();
        EXPECT_EQ( result, localIdOpt->toString() );
    }

    TEST( DataChannelId, ToStringShortId )
    {
        auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString( "CH001" );
        ASSERT_TRUE( dataChannelIdOpt.has_value() );

        std::string result = dataChannelIdOpt->toString();
        EXPECT_EQ( result, "CH001" );
    }

    //=====================================================================
    // DataChannelId Parse Tests
    //=====================================================================

    TEST( DataChannelId, ParseValidLocalId )
    {
        std::string validLocalIdString = "/dnv-v2/vis-3-4a/511.15-1/E32/meta/qty-frequency";
        auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString( validLocalIdString );
        ASSERT_TRUE( dataChannelIdOpt.has_value() );
        auto dataChannelId = *dataChannelIdOpt;
        EXPECT_TRUE( dataChannelId.isLocalId() );
        EXPECT_FALSE( dataChannelId.isShortId() );

        auto localId = dataChannelId.localId();
        ASSERT_TRUE( localId.has_value() );
        EXPECT_EQ( localId.value().get().toString(), validLocalIdString );
    }

    TEST( DataChannelId, ParseShortId )
    {
        auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString( "CH001" );
        ASSERT_TRUE( dataChannelIdOpt.has_value() );
        auto dataChannelId = *dataChannelIdOpt;

        EXPECT_FALSE( dataChannelId.isLocalId() );
        EXPECT_TRUE( dataChannelId.isShortId() );

        auto shortId = dataChannelId.shortId();
        ASSERT_TRUE( shortId.has_value() );
        EXPECT_EQ( shortId.value(), "CH001" );
    }

    TEST( DataChannelId, ParseInvalidLocalIdBecomesShortId )
    {
        auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString( "/invalid/local/id" );
        ASSERT_TRUE( dataChannelIdOpt.has_value() );
        auto dataChannelId = *dataChannelIdOpt;

        EXPECT_FALSE( dataChannelId.isLocalId() );
        EXPECT_TRUE( dataChannelId.isShortId() );

        auto shortId = dataChannelId.shortId();
        ASSERT_TRUE( shortId.has_value() );
        EXPECT_EQ( shortId.value(), "/invalid/local/id" );
    }

    TEST( DataChannelId, ParseEmptyStringReturnsNullopt )
    {
        auto result1 = transport::timeseries::DataChannelId::fromString( "" );
        EXPECT_FALSE( result1.has_value() );

        auto result2 = transport::timeseries::DataChannelId::fromString( std::string_view{} );
        EXPECT_FALSE( result2.has_value() );
    }

    //=====================================================================
    // DataChannelId Pattern Matching Tests
    //=====================================================================

    TEST( DataChannelId, MatchOnLocalId )
    {
        std::string validLocalIdString = "/dnv-v2/vis-3-4a/511.15-1/E32/meta/qty-power";
        auto localIdOpt = LocalIdBuilder::fromString( validLocalIdString );

        ASSERT_TRUE( localIdOpt.has_value() );

        auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString( localIdOpt->toString() );
        ASSERT_TRUE( dataChannelIdOpt.has_value() );

        std::string result = dataChannelIdOpt->match(
            []( const LocalId& id ) { return "LocalId: " + id.toString(); },
            []( std::string_view shortId ) { return "ShortId: " + std::string{ shortId }; } );

        EXPECT_EQ( result, "LocalId: " + localIdOpt->toString() );
    }

    TEST( DataChannelId, MatchOnShortId )
    {
        auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString( "CH001" );
        ASSERT_TRUE( dataChannelIdOpt.has_value() );

        std::string result = dataChannelIdOpt->match(
            []( const LocalId& localId ) { return "LocalId: " + localId.toString(); },
            []( std::string_view shortId ) { return "ShortId: " + std::string{ shortId }; } );

        EXPECT_EQ( result, "ShortId: CH001" );
    }

    TEST( DataChannelId, SwitchOnLocalId )
    {
        std::string validLocalIdString = "/dnv-v2/vis-3-4a/511.15-1/E32/meta/qty-energy";
        auto localIdOpt = LocalIdBuilder::fromString( validLocalIdString );

        ASSERT_TRUE( localIdOpt.has_value() );

        auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString( localIdOpt->toString() );
        ASSERT_TRUE( dataChannelIdOpt.has_value() );

        std::string result;
        dataChannelIdOpt->visit(
            [&result]( const LocalId& id ) { result = "LocalId: " + id.toString(); },
            [&result]( std::string_view shortId ) { result = "ShortId: " + std::string{ shortId }; } );

        EXPECT_EQ( result, "LocalId: " + localIdOpt->toString() );
    }

    TEST( DataChannelId, SwitchOnShortId )
    {
        auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString( "CH001" );
        ASSERT_TRUE( dataChannelIdOpt.has_value() );

        std::string result;
        dataChannelIdOpt->visit(
            [&result]( const LocalId& localId ) { result = "LocalId: " + localId.toString(); },
            [&result]( std::string_view shortId ) { result = "ShortId: " + std::string{ shortId }; } );

        EXPECT_EQ( result, "ShortId: CH001" );
    }

    //=====================================================================
    // DataChannelId Copy/Move Tests
    //=====================================================================

    TEST( DataChannelId, CopyConstructor )
    {
        auto originalOpt = transport::timeseries::DataChannelId::fromString( "CH001" );
        ASSERT_TRUE( originalOpt.has_value() );

        transport::timeseries::DataChannelId copy{ *originalOpt };

        EXPECT_EQ( *originalOpt, copy );
        EXPECT_TRUE( copy.isShortId() );
    }

    TEST( DataChannelId, MoveConstructor )
    {
        auto originalOpt = transport::timeseries::DataChannelId::fromString( "CH001" );
        ASSERT_TRUE( originalOpt.has_value() );

        transport::timeseries::DataChannelId moved{ std::move( *originalOpt ) };

        EXPECT_TRUE( moved.isShortId() );
        auto shortId = moved.shortId();
        ASSERT_TRUE( shortId.has_value() );
        EXPECT_EQ( shortId.value(), "CH001" );
    }

    TEST( DataChannelId, MoveAssignment )
    {
        auto originalOpt = transport::timeseries::DataChannelId::fromString( "CH001" );
        ASSERT_TRUE( originalOpt.has_value() );

        std::string validLocalIdString = "/dnv-v2/vis-3-4a/511.11-2/C101/meta/qty-rotational.frequency";
        auto localIdOpt = LocalIdBuilder::fromString( validLocalIdString );

        ASSERT_TRUE( localIdOpt.has_value() );

        auto targetOpt = transport::timeseries::DataChannelId::fromString( localIdOpt->toString() );
        ASSERT_TRUE( targetOpt.has_value() );

        *targetOpt = std::move( *originalOpt );

        EXPECT_TRUE( targetOpt->isShortId() );
        auto shortId = targetOpt->shortId();
        ASSERT_TRUE( shortId.has_value() );
        EXPECT_EQ( shortId.value(), "CH001" );
    }

    //=====================================================================
    // DataChannelId Implicit Conversion Tests
    //=====================================================================

    TEST( DataChannelId, ImplicitConversionFromLocalId )
    {
        std::string validLocalIdString = "/dnv-v2/vis-3-4a/754/I101/meta/state-alarm";
        auto localIdOpt = LocalIdBuilder::fromString( validLocalIdString );

        ASSERT_TRUE( localIdOpt.has_value() );

        auto dataChannelIdOpt = transport::timeseries::DataChannelId::fromString( localIdOpt->toString() );
        ASSERT_TRUE( dataChannelIdOpt.has_value() );

        EXPECT_TRUE( dataChannelIdOpt->isLocalId() );
        auto retrievedLocalId = dataChannelIdOpt->localId();
        ASSERT_TRUE( retrievedLocalId.has_value() );
        EXPECT_EQ( retrievedLocalId.value().get(), *localIdOpt );
    }

    TEST( DataChannelId, CopyAssignment )
    {
        auto originalOpt = transport::timeseries::DataChannelId::fromString( "CH001" );
        ASSERT_TRUE( originalOpt.has_value() );

        std::string validLocalIdString = "/dnv-v2/vis-3-4a/511.11-2/C101/meta/qty-rotational.frequency";
        auto localIdOpt = LocalIdBuilder::fromString( validLocalIdString );
        ASSERT_TRUE( localIdOpt.has_value() );

        auto targetOpt = transport::timeseries::DataChannelId::fromString( localIdOpt->toString() );
        ASSERT_TRUE( targetOpt.has_value() );

        *targetOpt = *originalOpt; // Copy assignment

        EXPECT_EQ( *targetOpt, *originalOpt );
        EXPECT_TRUE( targetOpt->isShortId() );
        auto shortId = targetOpt->shortId();
        ASSERT_TRUE( shortId.has_value() );
        EXPECT_EQ( shortId.value(), "CH001" );
    }
} // namespace dnv::vista::sdk::tests
