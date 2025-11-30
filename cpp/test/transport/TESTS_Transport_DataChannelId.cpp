/**
 * @file TESTS_Transport_DataChannelId.cpp
 * @brief Comprehensive tests for DataChannelId class
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/transport/timeseries/DataChannelId.h>
#include <dnv/vista/sdk/GmodPath.h>
#include <dnv/vista/sdk/LocalId.h>
#include <dnv/vista/sdk/LocalIdBuilder.h>
#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/VISVersion.h>

namespace dnv::vista::sdk::tests
{
	//=====================================================================
	// DataChannelId Construction Tests
	//=====================================================================

	TEST( DataChannelId, ConstructFromLocalId )
	{
		std::string validLocalIdString = "/dnv-v2/vis-3-4a/411.1/C101.62/S205/meta/qty-temperature/cnt-lubricating.oil/pos-inlet";
		std::optional<LocalIdBuilder> localIdBuilderOpt;
		bool parsed = LocalIdBuilder::tryParse( validLocalIdString, localIdBuilderOpt );

		ASSERT_TRUE( parsed );
		ASSERT_TRUE( localIdBuilderOpt.has_value() );

		auto localId = localIdBuilderOpt->build();
		auto dataChannelId = transport::timeseries::DataChannelId::parse( localId.toString() );
		EXPECT_TRUE( dataChannelId.isLocalId() );
		EXPECT_FALSE( dataChannelId.isShortId() );

		auto retrievedLocalId = dataChannelId.localId();
		ASSERT_TRUE( retrievedLocalId.has_value() );
		EXPECT_EQ( retrievedLocalId.value(), localId );

		auto shortId = dataChannelId.shortId();
		EXPECT_FALSE( shortId.has_value() );
	}

	TEST( DataChannelId, ConstructFromStringView )
	{
		std::string_view testId = "CH001";
		auto dataChannelId = transport::timeseries::DataChannelId::parse( testId );

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
		std::string validLocalIdString = "/dnv-v2/vis-3-4a/411.1/C101.62/S205/meta/qty-temperature/cnt-lubricating.oil/pos-inlet";
		std::optional<LocalIdBuilder> localIdBuilder1Opt;
		std::optional<LocalIdBuilder> localIdBuilder2Opt;
		bool parsed1 = LocalIdBuilder::tryParse( validLocalIdString, localIdBuilder1Opt );
		bool parsed2 = LocalIdBuilder::tryParse( validLocalIdString, localIdBuilder2Opt );

		ASSERT_TRUE( parsed1 );
		ASSERT_TRUE( parsed2 );
		ASSERT_TRUE( localIdBuilder1Opt.has_value() );
		ASSERT_TRUE( localIdBuilder2Opt.has_value() );

		auto localId1 = localIdBuilder1Opt->build();
		auto localId2 = localIdBuilder2Opt->build();
		auto dataChannelId1 = transport::timeseries::DataChannelId::parse( localId1.toString() );
		auto dataChannelId2 = transport::timeseries::DataChannelId::parse( localId2.toString() );

		EXPECT_EQ( dataChannelId1, dataChannelId2 );
		EXPECT_FALSE( dataChannelId1 != dataChannelId2 );
	}

	TEST( DataChannelId, EqualityShortIds )
	{
		auto dataChannelId1 = transport::timeseries::DataChannelId::parse( "CH001" );
		auto dataChannelId2 = transport::timeseries::DataChannelId::parse( "CH001" );

		EXPECT_EQ( dataChannelId1, dataChannelId2 );
		EXPECT_FALSE( dataChannelId1 != dataChannelId2 );
	}

	TEST( DataChannelId, InequalityDifferentTypes )
	{
		std::string validLocalIdString = "/dnv-v2/vis-3-4a/411.1/C101.44i-5A/C261/meta/qty-temperature";
		std::optional<LocalIdBuilder> localIdBuilderOpt;
		bool parsed = LocalIdBuilder::tryParse( validLocalIdString, localIdBuilderOpt );

		ASSERT_TRUE( parsed );
		ASSERT_TRUE( localIdBuilderOpt.has_value() );

		auto localId = localIdBuilderOpt->build();

		auto localIdDataChannelId = transport::timeseries::DataChannelId::parse( localId.toString() );
		auto shortIdDataChannelId = transport::timeseries::DataChannelId::parse( "CH001" );

		EXPECT_NE( localIdDataChannelId, shortIdDataChannelId );
		EXPECT_TRUE( localIdDataChannelId != shortIdDataChannelId );
	}

	TEST( DataChannelId, InequalityDifferentValues )
	{
		auto dataChannelId1 = transport::timeseries::DataChannelId::parse( "CH001" );
		auto dataChannelId2 = transport::timeseries::DataChannelId::parse( "CH002" );

		EXPECT_NE( dataChannelId1, dataChannelId2 );
		EXPECT_TRUE( dataChannelId1 != dataChannelId2 );
	}

	//=====================================================================
	// DataChannelId ToString Tests
	//=====================================================================

	TEST( DataChannelId, ToStringLocalId )
	{
		std::string validLocalIdString = "/dnv-v2/vis-3-4a/411.1/C101.44i-6A/C261/meta/qty-temperature";
		std::optional<LocalIdBuilder> localIdBuilderOpt;
		bool parsed = LocalIdBuilder::tryParse( validLocalIdString, localIdBuilderOpt );

		ASSERT_TRUE( parsed );
		ASSERT_TRUE( localIdBuilderOpt.has_value() );

		auto localId = localIdBuilderOpt->build();
		auto dataChannelId = transport::timeseries::DataChannelId::parse( localId.toString() );

		std::string result = dataChannelId.toString();
		EXPECT_EQ( result, localId.toString() );
	}

	TEST( DataChannelId, ToStringShortId )
	{
		auto dataChannelId = transport::timeseries::DataChannelId::parse( "CH001" );

		std::string result = dataChannelId.toString();
		EXPECT_EQ( result, "CH001" );
	}

	//=====================================================================
	// DataChannelId Parse Tests
	//=====================================================================

	TEST( DataChannelId, ParseValidLocalId )
	{
		std::string validLocalIdString = "/dnv-v2/vis-3-4a/511.15-1/E32/meta/qty-frequency";
		auto dataChannelId = transport::timeseries::DataChannelId::parse( validLocalIdString );

		EXPECT_TRUE( dataChannelId.isLocalId() );
		EXPECT_FALSE( dataChannelId.isShortId() );

		auto localId = dataChannelId.localId();
		ASSERT_TRUE( localId.has_value() );
		EXPECT_EQ( localId.value().toString(), validLocalIdString );
	}

	TEST( DataChannelId, ParseShortId )
	{
		auto dataChannelId = transport::timeseries::DataChannelId::parse( "CH001" );

		EXPECT_FALSE( dataChannelId.isLocalId() );
		EXPECT_TRUE( dataChannelId.isShortId() );

		auto shortId = dataChannelId.shortId();
		ASSERT_TRUE( shortId.has_value() );
		EXPECT_EQ( shortId.value(), "CH001" );
	}

	TEST( DataChannelId, ParseInvalidLocalIdBecomesShortId )
	{
		auto dataChannelId = transport::timeseries::DataChannelId::parse( "/invalid/local/id" );

		EXPECT_FALSE( dataChannelId.isLocalId() );
		EXPECT_TRUE( dataChannelId.isShortId() );

		auto shortId = dataChannelId.shortId();
		ASSERT_TRUE( shortId.has_value() );
		EXPECT_EQ( shortId.value(), "/invalid/local/id" );
	}

	TEST( DataChannelId, ParseEmptyStringThrows )
	{
		EXPECT_THROW( (void)transport::timeseries::DataChannelId::parse( "" ), std::invalid_argument );
		EXPECT_THROW( (void)transport::timeseries::DataChannelId::parse( std::string_view{} ), std::invalid_argument );
	}

	//=====================================================================
	// DataChannelId Pattern Matching Tests
	//=====================================================================

	TEST( DataChannelId, MatchOnLocalId )
	{
		std::string validLocalIdString = "/dnv-v2/vis-3-4a/511.15-1/E32/meta/qty-power";
		std::optional<LocalIdBuilder> localIdBuilderOpt;
		bool parsed = LocalIdBuilder::tryParse( validLocalIdString, localIdBuilderOpt );

		ASSERT_TRUE( parsed );
		ASSERT_TRUE( localIdBuilderOpt.has_value() );

		auto localId = localIdBuilderOpt->build();

		auto dataChannelId = transport::timeseries::DataChannelId::parse( localId.toString() );

		std::string result = dataChannelId.matchOn<std::string>(
			[]( const LocalId& id ) { return "LocalId: " + id.toString(); },
			[]( std::string_view shortId ) { return "ShortId: " + std::string{ shortId }; } );

		EXPECT_EQ( result, "LocalId: " + localId.toString() );
	}

	TEST( DataChannelId, MatchOnShortId )
	{
		auto dataChannelId = transport::timeseries::DataChannelId::parse( "CH001" );

		std::string result = dataChannelId.matchOn<std::string>(
			[]( const LocalId& localId ) { return "LocalId: " + localId.toString(); },
			[]( std::string_view shortId ) { return "ShortId: " + std::string{ shortId }; } );

		EXPECT_EQ( result, "ShortId: CH001" );
	}

	TEST( DataChannelId, SwitchOnLocalId )
	{
		std::string validLocalIdString = "/dnv-v2/vis-3-4a/511.15-1/E32/meta/qty-energy";
		std::optional<LocalIdBuilder> localIdBuilderOpt;
		bool parsed = LocalIdBuilder::tryParse( validLocalIdString, localIdBuilderOpt );

		ASSERT_TRUE( parsed );
		ASSERT_TRUE( localIdBuilderOpt.has_value() );

		auto localId = localIdBuilderOpt->build();

		auto dataChannelId = transport::timeseries::DataChannelId::parse( localId.toString() );

		std::string result;
		dataChannelId.switchOn(
			[&result]( const LocalId& id ) { result = "LocalId: " + id.toString(); },
			[&result]( std::string_view shortId ) { result = "ShortId: " + std::string{ shortId }; } );

		EXPECT_EQ( result, "LocalId: " + localId.toString() );
	}

	TEST( DataChannelId, SwitchOnShortId )
	{
		auto dataChannelId = transport::timeseries::DataChannelId::parse( "CH001" );

		std::string result;
		dataChannelId.switchOn(
			[&result]( const LocalId& localId ) { result = "LocalId: " + localId.toString(); },
			[&result]( std::string_view shortId ) { result = "ShortId: " + std::string{ shortId }; } );

		EXPECT_EQ( result, "ShortId: CH001" );
	}

	//=====================================================================
	// DataChannelId Copy/Move Tests
	//=====================================================================

	TEST( DataChannelId, CopyConstructor )
	{
		auto original = transport::timeseries::DataChannelId::parse( "CH001" );

		transport::timeseries::DataChannelId copy{ original };

		EXPECT_EQ( original, copy );
		EXPECT_TRUE( copy.isShortId() );
	}

	TEST( DataChannelId, MoveConstructor )
	{
		auto original = transport::timeseries::DataChannelId::parse( "CH001" );

		transport::timeseries::DataChannelId moved{ std::move( original ) };

		EXPECT_TRUE( moved.isShortId() );
		auto shortId = moved.shortId();
		ASSERT_TRUE( shortId.has_value() );
		EXPECT_EQ( shortId.value(), "CH001" );
	}

	TEST( DataChannelId, MoveAssignment )
	{
		auto original = transport::timeseries::DataChannelId::parse( "CH001" );

		std::string validLocalIdString = "/dnv-v2/vis-3-4a/511.11-2/C101/meta/qty-rotational.frequency";
		std::optional<LocalIdBuilder> localIdBuilderOpt;
		bool parsed = LocalIdBuilder::tryParse( validLocalIdString, localIdBuilderOpt );

		ASSERT_TRUE( parsed );
		ASSERT_TRUE( localIdBuilderOpt.has_value() );

		auto localId = localIdBuilderOpt->build();

		auto target = transport::timeseries::DataChannelId::parse( localId.toString() );

		target = std::move( original );

		EXPECT_TRUE( target.isShortId() );
		auto shortId = target.shortId();
		ASSERT_TRUE( shortId.has_value() );
		EXPECT_EQ( shortId.value(), "CH001" );
	}

	//=====================================================================
	// DataChannelId Implicit Conversion Tests
	//=====================================================================

	TEST( DataChannelId, ImplicitConversionFromLocalId )
	{
		std::string validLocalIdString = "/dnv-v2/vis-3-4a/754/I101/meta/state-alarm";
		std::optional<LocalIdBuilder> localIdBuilderOpt;
		bool parsed = LocalIdBuilder::tryParse( validLocalIdString, localIdBuilderOpt );

		ASSERT_TRUE( parsed );
		ASSERT_TRUE( localIdBuilderOpt.has_value() );

		auto localId = localIdBuilderOpt->build();

		auto dataChannelId = transport::timeseries::DataChannelId::parse( localId.toString() );

		EXPECT_TRUE( dataChannelId.isLocalId() );
		auto retrievedLocalId = dataChannelId.localId();
		ASSERT_TRUE( retrievedLocalId.has_value() );
		EXPECT_EQ( retrievedLocalId.value(), localId );
	}
} // namespace dnv::vista::sdk::tests
