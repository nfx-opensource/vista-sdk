/**
 * @file Tests_ShipId.cpp
 * @brief Unit tests for ShipId functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/transport/ShipId.h>

namespace dnv::vista::sdk::test
{
    TEST( ShipId, Construction_FromImoNumber )
    {
        // Valid IMO number
        auto imo = ImoNumber::fromString( "9074729" );
        ASSERT_TRUE( imo.has_value() );

        ShipId shipId{ *imo };

        EXPECT_TRUE( shipId.isImoNumber() );
        EXPECT_FALSE( shipId.isOtherId() );
    }

    TEST( ShipId, Construction_FromAlternativeId )
    {
        std::string altId = "VESSEL-123";
        ShipId shipId{ altId };

        EXPECT_FALSE( shipId.isImoNumber() );
        EXPECT_TRUE( shipId.isOtherId() );
    }

    TEST( ShipId, Construction_EmptyAlternativeId_Throws )
    {
        EXPECT_THROW( ShipId{ std::string( "" ) }, std::invalid_argument );
    }

    TEST( ShipId, ImoNumber_Accessor )
    {
        auto imo = ImoNumber::fromString( "9074729" );
        ASSERT_TRUE( imo.has_value() );

        ShipId shipId{ *imo };

        auto retrievedImo = shipId.imoNumber();
        ASSERT_TRUE( retrievedImo.has_value() );
        EXPECT_EQ( retrievedImo->toString(), "IMO9074729" );
        auto otherId = shipId.otherId();
        EXPECT_FALSE( otherId.has_value() );
    }

    TEST( ShipId, OtherId_Accessor )
    {
        std::string altId = "VESSEL-ABC-456";
        ShipId shipId{ altId };

        auto otherId = shipId.otherId();
        ASSERT_TRUE( otherId.has_value() );
        EXPECT_EQ( *otherId, "VESSEL-ABC-456" );

        auto imo = shipId.imoNumber();
        EXPECT_FALSE( imo.has_value() );
    }

    TEST( ShipId, ToString_ImoNumber )
    {
        auto imo = ImoNumber::fromString( "9074729" );
        ASSERT_TRUE( imo.has_value() );

        ShipId shipId{ *imo };

        // Should include "IMO" prefix per ISO 19848
        EXPECT_EQ( shipId.toString(), "IMO9074729" );
    }

    TEST( ShipId, ToString_AlternativeId )
    {
        std::string altId = "CUSTOM-SHIP-ID";
        ShipId shipId{ altId };

        EXPECT_EQ( shipId.toString(), "CUSTOM-SHIP-ID" );
    }

    TEST( ShipId, FromString_ImoWithPrefix )
    {
        auto shipId = ShipId::fromString( "IMO9074729" );

        ASSERT_TRUE( shipId.has_value() );
        EXPECT_TRUE( shipId->isImoNumber() );

        auto imo = shipId->imoNumber();
        ASSERT_TRUE( imo.has_value() );
        EXPECT_EQ( imo->toString(), "IMO9074729" );
    }

    TEST( ShipId, FromString_ImoWithPrefix_CaseInsensitive )
    {
        // Test lowercase
        auto shipId1 = ShipId::fromString( "imo9074729" );
        ASSERT_TRUE( shipId1.has_value() );
        EXPECT_TRUE( shipId1->isImoNumber() );

        // Test mixed case
        auto shipId2 = ShipId::fromString( "ImO9074729" );
        ASSERT_TRUE( shipId2.has_value() );
        EXPECT_TRUE( shipId2->isImoNumber() );
    }

    TEST( ShipId, FromString_ImoWithPrefix_Invalid )
    {
        // Invalid IMO number after prefix
        auto shipId = ShipId::fromString( "IMO1234568" );

        // Should fall back to alternative ID since IMO validation fails
        ASSERT_TRUE( shipId.has_value() );
        EXPECT_TRUE( shipId->isOtherId() );
        EXPECT_EQ( *shipId->otherId(), "IMO1234568" );
    }

    TEST( ShipId, FromString_AlternativeId )
    {
        auto shipId = ShipId::fromString( "VESSEL-XYZ-789" );

        ASSERT_TRUE( shipId.has_value() );
        EXPECT_TRUE( shipId->isOtherId() );

        auto otherId = shipId->otherId();
        ASSERT_TRUE( otherId.has_value() );
        EXPECT_EQ( *otherId, "VESSEL-XYZ-789" );
    }

    TEST( ShipId, FromString_EmptyString )
    {
        auto shipId = ShipId::fromString( "" );
        EXPECT_FALSE( shipId.has_value() );
    }

    TEST( ShipId, FromString_WhitespaceOnly )
    {
        auto shipId = ShipId::fromString( "   " );
        EXPECT_FALSE( shipId.has_value() );
    }

    TEST( ShipId, Equality_ImoNumbers )
    {
        auto imo1 = ImoNumber::fromString( "9074729" ).value();
        auto imo2 = ImoNumber::fromString( "9074729" ).value();
        auto imo3 = ImoNumber::fromString( "1234567" ).value();

        ShipId shipId1{ imo1 };
        ShipId shipId2{ imo2 };
        ShipId shipId3{ imo3 };

        EXPECT_EQ( shipId1, shipId2 );
        EXPECT_NE( shipId1, shipId3 );
    }

    TEST( ShipId, Equality_AlternativeIds )
    {
        ShipId shipId1{ std::string( "VESSEL-A" ) };
        ShipId shipId2{ std::string( "VESSEL-A" ) };
        ShipId shipId3{ std::string( "VESSEL-B" ) };

        EXPECT_EQ( shipId1, shipId2 );
        EXPECT_NE( shipId1, shipId3 );
    }

    TEST( ShipId, Equality_ImoVsAlternative )
    {
        auto imo = ImoNumber::fromString( "9074729" ).value();
        ShipId shipId1{ imo };
        ShipId shipId2{ std::string( "9074729" ) };

        // Different types should not be equal
        EXPECT_NE( shipId1, shipId2 );
    }

    TEST( ShipId, Match_ImoNumber )
    {
        auto imo = ImoNumber::fromString( "9074729" ).value();
        ShipId shipId{ imo };

        auto result = shipId.match(
            []( const ImoNumber& i ) { return "IMO: " + i.toString(); },
            []( std::string_view s ) { return "Other: " + std::string( s ); } );

        EXPECT_EQ( result, "IMO: IMO9074729" );
    }

    TEST( ShipId, Match_AlternativeId )
    {
        ShipId shipId{ std::string( "CUSTOM-123" ) };

        auto result = shipId.match(
            []( const ImoNumber& i ) { return "IMO: " + i.toString(); },
            []( std::string_view s ) { return "Other: " + std::string( s ); } );

        EXPECT_EQ( result, "Other: CUSTOM-123" );
    }

    TEST( ShipId, Visit_ImoNumber )
    {
        auto imo = ImoNumber::fromString( "9074729" ).value();
        ShipId shipId{ imo };

        std::string result;
        shipId.visit(
            [&result]( const ImoNumber& i ) { result = "IMO: " + i.toString(); },
            [&result]( std::string_view s ) { result = "Other: " + std::string( s ); } );

        EXPECT_EQ( result, "IMO: IMO9074729" );
    }

    TEST( ShipId, Visit_AlternativeId )
    {
        ShipId shipId{ std::string( "VESSEL-456" ) };

        std::string result;
        shipId.visit(
            [&result]( const ImoNumber& i ) { result = "IMO: " + i.toString(); },
            [&result]( std::string_view s ) { result = "Other: " + std::string( s ); } );

        EXPECT_EQ( result, "Other: VESSEL-456" );
    }

    TEST( ShipId, CopyConstruction )
    {
        auto imo = ImoNumber::fromString( "9074729" ).value();
        ShipId original{ imo };
        ShipId copy{ original };

        EXPECT_EQ( original, copy );
        EXPECT_TRUE( copy.isImoNumber() );
    }

    TEST( ShipId, MoveConstruction )
    {
        auto imo = ImoNumber::fromString( "9074729" ).value();
        ShipId original{ imo };
        ShipId moved{ std::move( original ) };

        EXPECT_TRUE( moved.isImoNumber() );
        EXPECT_EQ( moved.imoNumber()->toString(), "IMO9074729" );
    }

    TEST( ShipId, CopyAssignment )
    {
        auto imo1 = ImoNumber::fromString( "9074729" ).value();
        auto imo2 = ImoNumber::fromString( "1234567" ).value();

        ShipId shipId1{ imo1 };
        ShipId shipId2{ imo2 };

        shipId2 = shipId1;

        EXPECT_EQ( shipId1, shipId2 );
        EXPECT_EQ( shipId2.imoNumber()->toString(), "IMO9074729" );
    }

    TEST( ShipId, MoveAssignment )
    {
        auto imo1 = ImoNumber::fromString( "9074729" ).value();
        ShipId shipId1{ imo1 };
        ShipId shipId2{ std::string( "TEMP" ) };

        shipId2 = std::move( shipId1 );

        EXPECT_TRUE( shipId2.isImoNumber() );
        EXPECT_EQ( shipId2.imoNumber()->toString(), "IMO9074729" );
    }

    TEST( ShipId, RoundTrip_ImoNumber )
    {
        auto original = ImoNumber::fromString( "9074729" ).value();
        ShipId shipId1{ original };

        auto str = shipId1.toString();
        auto shipId2 = ShipId::fromString( str );

        ASSERT_TRUE( shipId2.has_value() );
        EXPECT_EQ( shipId1, *shipId2 );
        EXPECT_TRUE( shipId2->isImoNumber() );
    }

    TEST( ShipId, RoundTrip_AlternativeId )
    {
        ShipId shipId1{ std::string( "VESSEL-ROUND-TRIP" ) };

        auto str = shipId1.toString();
        auto shipId2 = ShipId::fromString( str );

        ASSERT_TRUE( shipId2.has_value() );
        EXPECT_EQ( shipId1, *shipId2 );
        EXPECT_TRUE( shipId2->isOtherId() );
    }
} // namespace dnv::vista::sdk::test
