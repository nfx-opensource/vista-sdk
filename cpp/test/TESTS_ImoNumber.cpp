/**
 * @file TESTS_ImoNumber.cpp
 * @brief Unit tests for the ImoNumber class.
 */

#include <fstream>

#include "TestDataLoader.h"

#include <gtest/gtest.h>

#include <nfx/serialization/json/Document.h>

#include <dnv/vista/sdk/ImoNumber.h>

namespace dnv::vista::sdk::test
{
	namespace
	{
		constexpr const char* TEST_DATA_FILE = "ImoNumbers.json";
	}

	class ImoNumberTests : public ::testing::Test
	{
	protected:
		struct TestDataItem
		{
			std::string value;
			bool success;
			std::optional<std::string> output;
		};

		std::vector<TestDataItem> testData;

		virtual void SetUp() override
		{
			const nfx::serialization::json::Document& data = loadTestData( TEST_DATA_FILE );

			ASSERT_TRUE( data.is<nfx::serialization::json::Document::Object>( "" ) || data.is<nfx::serialization::json::Document::Array>( "" ) ) << "JSON data is not a valid object or array";

			// Determine the test cases array
			nfx::serialization::json::Document testCases;
			if ( data.is<nfx::serialization::json::Document::Array>( "" ) )
			{
				testCases = data;
			}
			else if ( data.contains( "/imoNumbers" ) )
			{
				auto testCasesOpt = data.get<nfx::serialization::json::Document>( "/imoNumbers" );
				ASSERT_TRUE( testCasesOpt.has_value() ) << "Failed to get 'imoNumbers' field";
				testCases = testCasesOpt.value();
			}
			else
			{
				testCases = data;
			}

			ASSERT_TRUE( testCases.is<nfx::serialization::json::Document::Array>( "" ) ) << "Test cases must be an array";

			// Enumerate the array
			auto arrayOpt = testCases.get<nfx::serialization::json::Document::Array>( "" );
			ASSERT_TRUE( arrayOpt.has_value() );

			for ( const auto& elem : arrayOpt.value() )
			{
				auto valueOpt = elem.get<std::string>( "/value" );
				auto successOpt = elem.get<bool>( "/success" );

				ASSERT_TRUE( valueOpt.has_value() ) << "Item missing 'value' field or not a string";
				ASSERT_TRUE( successOpt.has_value() ) << "Item missing 'success' field or not a boolean";

				std::optional<std::string> output;
				if ( auto outputOpt = elem.get<std::string>( "/output" ) )
				{
					output = outputOpt.value();
				}

				testData.push_back( { valueOpt.value(), successOpt.value(), output } );
			}
		}
	};

	TEST_F( ImoNumberTests, Test_Validation )
	{
		for ( const auto& item : testData )
		{
			ImoNumber parsedImo;
			bool parsedOk = ImoNumber::tryParse( item.value, parsedImo );

			if ( item.success )
			{
				EXPECT_TRUE( parsedOk );
			}
			else
			{
				EXPECT_FALSE( parsedOk );
			}

			if ( item.output.has_value() && parsedOk )
			{
				EXPECT_EQ( parsedImo.toString(), item.output.value() );
			}
		}
	}

	TEST_F( ImoNumberTests, Test_ConstCharInputs )
	{
		const char* validImo = "IMO9074729";
		const char* invalidImo = "IMO123";
		const char* numericOnly = "9074729";

		ImoNumber parsed1;
		EXPECT_TRUE( ImoNumber::tryParse( validImo, parsed1 ) );
		EXPECT_EQ( parsed1.toString(), "IMO9074729" );

		ImoNumber parsed2;
		EXPECT_FALSE( ImoNumber::tryParse( invalidImo, parsed2 ) );

		ImoNumber parsed3;
		EXPECT_TRUE( ImoNumber::tryParse( numericOnly, parsed3 ) );
		EXPECT_EQ( parsed3.toString(), "IMO9074729" );

		EXPECT_NO_THROW( ImoNumber{ validImo } );
		EXPECT_THROW( ImoNumber{ invalidImo }, std::invalid_argument );

		auto parsed4 = ImoNumber::parse( validImo );
		EXPECT_EQ( parsed4.toString(), "IMO9074729" );

		EXPECT_THROW( (void)ImoNumber::parse( invalidImo ), std::invalid_argument );
	}

	TEST_F( ImoNumberTests, Test_StdStringInputs )
	{
		std::string validImo = "IMO9074729";
		std::string invalidImo = "IMO123";
		std::string numericOnly = "9074729";
		std::string caseVariant = "imo9074729";

		ImoNumber parsed1;
		EXPECT_TRUE( ImoNumber::tryParse( validImo, parsed1 ) );
		EXPECT_EQ( parsed1.toString(), "IMO9074729" );

		ImoNumber parsed2;
		EXPECT_FALSE( ImoNumber::tryParse( invalidImo, parsed2 ) );

		ImoNumber parsed3;
		EXPECT_TRUE( ImoNumber::tryParse( numericOnly, parsed3 ) );
		EXPECT_EQ( parsed3.toString(), "IMO9074729" );

		ImoNumber parsed4;
		EXPECT_TRUE( ImoNumber::tryParse( caseVariant, parsed4 ) );
		EXPECT_EQ( parsed4.toString(), "IMO9074729" );

		EXPECT_NO_THROW( ImoNumber{ validImo } );
		EXPECT_THROW( ImoNumber{ invalidImo }, std::invalid_argument );

		auto parsed5 = ImoNumber::parse( validImo );
		EXPECT_EQ( parsed5.toString(), "IMO9074729" );

		EXPECT_THROW( (void)ImoNumber::parse( invalidImo ), std::invalid_argument );
	}

	TEST_F( ImoNumberTests, Test_StringLiteralInputs )
	{
		ImoNumber parsed1;
		EXPECT_TRUE( ImoNumber::tryParse( "IMO9074729", parsed1 ) );
		EXPECT_EQ( parsed1.toString(), "IMO9074729" );

		ImoNumber parsed2;
		EXPECT_TRUE( ImoNumber::tryParse( "9074729", parsed2 ) );
		EXPECT_EQ( parsed2.toString(), "IMO9074729" );

		EXPECT_NO_THROW( ImoNumber{ "IMO9074729" } );
		EXPECT_THROW( ImoNumber{ "invalid" }, std::invalid_argument );

		auto parsed3 = ImoNumber::parse( "IMO9074729" );
		EXPECT_EQ( parsed3.toString(), "IMO9074729" );
	}
} // namespace dnv::vista::sdk::test
