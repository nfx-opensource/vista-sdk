/**
 * @file TESTS_UniversalId.cpp
 * @brief Unit tests for LocalId and LocalIdBuilder classes.
 */

#include <gtest/gtest.h>

#include "TestDataLoader.h"

#include <internal/parsing/LocalIdParsingErrorBuilder.h>
#include <dnv/vista/sdk/Gmod.h>
#include <dnv/vista/sdk/GmodPath.h>
#include <dnv/vista/sdk/ImoNumber.h>
#include <dnv/vista/sdk/LocalIdBuilder.h>
#include <dnv/vista/sdk/Locations.h>
#include <dnv/vista/sdk/ParsingErrors.h>
#include <dnv/vista/sdk/UniversalId.h>
#include <dnv/vista/sdk/UniversalIdBuilder.h>
#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/VISVersion.h>

namespace dnv::vista::sdk::tests::test
{
	//----------------------------------------------
	// Test Data
	//----------------------------------------------
	namespace
	{
		static const std::vector<std::string> testData{
			"data.dnv.com/IMO1234567/dnv-v2/vis-3-4a/621.21/S90/sec/411.1/C101/meta/qty-mass/cnt-fuel.oil/pos-inlet",
			"data.dnv.com/IMO1234567/dnv-v2/vis-3-7a/612.21/C701.23/C633/meta/calc~accumulate" };
	}
	//----------------------------------------------
	// Test_TryParsing
	//----------------------------------------------

	TEST( UniversalIdTests, Test_TryParsing_Case0 )
	{
		const std::string testCase = testData[0];

		ParsingErrors errors;
		std::optional<UniversalIdBuilder> uid;
		bool success = UniversalIdBuilder::tryParse( testCase, errors, uid );

		EXPECT_TRUE( success );
	}

	TEST( UniversalIdTests, Test_TryParsing_Case1 )
	{
		const std::string testCase = testData[1];

		ParsingErrors errors;
		std::optional<UniversalIdBuilder> uid;
		bool success = UniversalIdBuilder::tryParse( testCase, errors, uid );

		EXPECT_TRUE( success );
	}

	//----------------------------------------------
	// Test_Parsing
	//----------------------------------------------

	TEST( UniversalIdTests, Test_Parsing_Case0 )
	{
		const std::string testCase = testData[0];

		auto universalIdBuilder = UniversalIdBuilder::parse( testCase );

		EXPECT_TRUE( universalIdBuilder.imoNumber().has_value() );
		EXPECT_TRUE( universalIdBuilder.imoNumber().value() == ImoNumber( 1234567 ) );
	}

	TEST( UniversalIdTests, Test_Parsing_Case1 )
	{
		const std::string testCase = testData[1];

		auto universalIdBuilder = UniversalIdBuilder::parse( testCase );

		EXPECT_TRUE( universalIdBuilder.imoNumber().has_value() );
		EXPECT_TRUE( universalIdBuilder.imoNumber().value() == ImoNumber( 1234567 ) );
	}

	//----------------------------------------------
	// Test_ToString
	//----------------------------------------------

	TEST( UniversalIdTests, Test_ToString_Case0 )
	{
		const std::string testCase = testData[0];

		auto universalId = UniversalIdBuilder::parse( testCase );

		auto universalIdString = universalId.toString();
		EXPECT_EQ( testCase, universalIdString );
	}

	TEST( UniversalIdTests, Test_ToString_Case1 )
	{
		const std::string testCase = testData[1];

		auto universalId = UniversalIdBuilder::parse( testCase );

		auto universalIdString = universalId.toString();
		EXPECT_EQ( testCase, universalIdString );
	}

	//----------------------------------------------
	// Test_UniversalBuilder_Add_And_RemoveAll
	//----------------------------------------------

	TEST( UniversalIdTests, Test_UniversalBuilder_Add_And_RemoveAll_Case0 )
	{
		const std::string testCase = testData[0];

		std::optional<UniversalIdBuilder> universalIdBuilder;
		bool success = UniversalIdBuilder::tryParse( testCase, universalIdBuilder );

		ASSERT_TRUE( success );
		ASSERT_TRUE( universalIdBuilder.has_value() );
		EXPECT_TRUE( universalIdBuilder->localId().has_value() );
		EXPECT_TRUE( universalIdBuilder->imoNumber().has_value() );

		auto id = universalIdBuilder->withoutImoNumber().withoutLocalId();

		EXPECT_FALSE( id.localId().has_value() );
		EXPECT_FALSE( id.imoNumber().has_value() );
	}

	TEST( UniversalIdTests, Test_UniversalBuilder_Add_And_RemoveAll_Case1 )
	{
		const std::string testCase = testData[1];

		std::optional<UniversalIdBuilder> universalIdBuilder;
		bool success = UniversalIdBuilder::tryParse( testCase, universalIdBuilder );

		ASSERT_TRUE( success );
		ASSERT_TRUE( universalIdBuilder.has_value() );
		EXPECT_TRUE( universalIdBuilder->localId().has_value() );
		EXPECT_TRUE( universalIdBuilder->imoNumber().has_value() );

		auto id = universalIdBuilder->withoutImoNumber().withoutLocalId();

		EXPECT_FALSE( id.localId().has_value() );
		EXPECT_FALSE( id.imoNumber().has_value() );
	}

	//----------------------------------------------
	// Test_UniversalBuilder_TryWith
	//----------------------------------------------

	TEST( UniversalIdTests, Test_UniversalBuilder_TryWith_Immutability )
	{
		// Test that original builder remains unchanged when return values are ignored
		auto universalBuilder = UniversalIdBuilder::create( VisVersion::v3_4a ).withoutLocalId().withoutImoNumber();

		(void)universalBuilder.tryWithLocalId( std::nullopt );
		(void)universalBuilder.tryWithImoNumber( std::nullopt );

		EXPECT_FALSE( universalBuilder.localId().has_value() );
		EXPECT_FALSE( universalBuilder.imoNumber().has_value() );
	}

	TEST( UniversalIdTests, Test_UniversalBuilder_TryWith_Comprehensive )
	{
		auto universalBuilder = UniversalIdBuilder::create( VisVersion::v3_4a ).withoutLocalId().withoutImoNumber();

		auto localIdBuilder = LocalIdBuilder::create( VisVersion::v3_4a )
								  .withPrimaryItem( GmodPath::parse( "411.1", VisVersion::v3_4a ) );
		auto imoNumber = ImoNumber( 1234567 );

		auto builder1 = universalBuilder.tryWithLocalId( localIdBuilder );
		auto builder2 = builder1.tryWithImoNumber( imoNumber );

		EXPECT_TRUE( builder2.localId().has_value() );
		EXPECT_TRUE( builder2.imoNumber().has_value() );

		// Test with nullopt (no-op behavior)
		auto builder3 = builder2.tryWithLocalId( std::nullopt );
		auto builder4 = builder2.tryWithImoNumber( std::nullopt );

		EXPECT_TRUE( builder3.localId().has_value() );
		EXPECT_TRUE( builder3.imoNumber().has_value() );
		EXPECT_TRUE( builder4.localId().has_value() );
		EXPECT_TRUE( builder4.imoNumber().has_value() );
	}

	TEST( UniversalIdTests, Test_UniversalBuilder_TryWith_NulloptChaining )
	{
		auto universalBuilder = UniversalIdBuilder::create( VisVersion::v3_4a ).withoutLocalId().withoutImoNumber();

		auto builder1 = universalBuilder.tryWithLocalId( std::nullopt );
		auto builder2 = builder1.tryWithImoNumber( std::nullopt );

		EXPECT_FALSE( builder2.localId().has_value() );
		EXPECT_FALSE( builder2.imoNumber().has_value() );
	}

	TEST( UniversalIdTests, Test_UniversalBuilder_TryWith_NulloptIndependent )
	{
		auto universalBuilder = UniversalIdBuilder::create( VisVersion::v3_4a ).withoutLocalId().withoutImoNumber();

		// Verify that passing nullopt returns the same builder (no-op behavior)
		auto builder1 = universalBuilder.tryWithLocalId( std::nullopt );
		auto builder2 = universalBuilder.tryWithImoNumber( std::nullopt );

		EXPECT_FALSE( builder1.localId().has_value() );
		EXPECT_FALSE( builder1.imoNumber().has_value() );
		EXPECT_FALSE( builder2.localId().has_value() );
		EXPECT_FALSE( builder2.imoNumber().has_value() );
	}
}
