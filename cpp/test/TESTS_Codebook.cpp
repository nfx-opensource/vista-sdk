/**
 * @file TESTS_Codebook.cpp
 * @brief Unit tests for the Codebook and related functionalities.
 */

#include <gtest/gtest.h>

#include <algorithm>

#include <nfx/serialization/json/Document.h>

#include "TestDataLoader.h"

#include <dnv/vista/sdk/Codebooks.h>
#include <dnv/vista/sdk/MetadataTag.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::test
{
	namespace
	{
		constexpr const char* TEST_DATA_FILE = "Codebook.json";
	}

	namespace CodebookTestFixture
	{
		class CodebookTest : public ::testing::Test
		{
		protected:
			CodebookTest() : m_jsonData{ loadTestData( TEST_DATA_FILE ) } {}

			const Codebooks& getCodebooks()
			{
				const auto& vis = VIS::instance();
				return vis.codebooks( VisVersion::v3_4a );
			}

			const nfx::serialization::json::Document& m_jsonData;
		};

		TEST_F( CodebookTest, Test_Standard_Values )
		{
			const auto& codebooks = getCodebooks();
			const auto& positions = codebooks.codebook( CodebookName::Position );

			EXPECT_TRUE( positions.hasStandardValue( "upper" ) );

			const auto& rawData = positions.rawData();
			EXPECT_TRUE( rawData.find( "Vertical" ) != nullptr );

			const auto* verticalGroupValuesPtr = rawData.find( "Vertical" );
			ASSERT_NE( verticalGroupValuesPtr, nullptr ) << "Group 'Vertical' not found in raw data.";

			const auto& verticalGroupValues = *verticalGroupValuesPtr;
			EXPECT_NE( std::find( verticalGroupValues.begin(), verticalGroupValues.end(), "upper" ), verticalGroupValues.end() );
		}

		TEST_F( CodebookTest, Test_Get_Groups )
		{
			const auto& codebooks = getCodebooks();
			const auto& groups = codebooks.codebook( CodebookName::Position ).groups();
			EXPECT_GT( groups.count(), 1 );

			EXPECT_TRUE( groups.contains( "Vertical" ) );

			const auto& rawData = codebooks.codebook( CodebookName::Position ).rawData();

			EXPECT_EQ( groups.count(), rawData.size() - 1 );
			EXPECT_TRUE( rawData.find( "Vertical" ) != nullptr );
		}

		TEST_F( CodebookTest, Test_Iterate_Groups )
		{
			const auto& codebooks = getCodebooks();
			const auto& groups = codebooks.codebook( CodebookName::Position ).groups();
			int iterated_count = 0;

			for ( [[maybe_unused]] const auto& group : groups )
			{
				iterated_count++;
			}
			EXPECT_EQ( iterated_count, 11 );
		}

		TEST_F( CodebookTest, Test_Iterate_Values )
		{
			const auto& codebooks = getCodebooks();
			const auto& positionCodebook = codebooks.codebook( CodebookName::Position );
			const auto& values = positionCodebook.standardValues();
			int iterated_count = 0;
			for ( [[maybe_unused]] const auto& value_item : values )
			{
				iterated_count++;
			}
			EXPECT_EQ( iterated_count, 28 );
		}
	} // namespace CodebookTestFixture

	namespace CodebookTestParametrized
	{
		class CodebookBaseTest
		{
		protected:
			const Codebooks& getCodebooks()
			{
				const auto& vis = VIS::instance();
				return vis.codebooks( VisVersion::v3_4a );
			}
		};

		struct PositionValidationParam
		{
			std::string input;
			std::string expectedOutput;
		};

		class PositionValidationTest : public CodebookBaseTest, public ::testing::TestWithParam<PositionValidationParam>
		{
		};

		static std::vector<PositionValidationParam> positionValidationData()
		{
			std::vector<PositionValidationParam> data;
			const nfx::serialization::json::Document& jsonDataFromFile = loadTestData( TEST_DATA_FILE );

			auto validPositionDocOpt = jsonDataFromFile.get<nfx::serialization::json::Document>( "/ValidPosition" );
			if ( validPositionDocOpt.has_value() )
			{
				const auto& validPositionDoc = validPositionDocOpt.value();

				auto arrayOpt = validPositionDoc.get<nfx::serialization::json::Document::Array>( "" );
				if ( arrayOpt )
				{
					for ( const auto& item : arrayOpt.value() )
					{
						if ( item.contains( "/0" ) && item.contains( "/1" ) )
						{
							auto firstStrOpt = item.get<std::string>( "/0" );
							auto secondStrOpt = item.get<std::string>( "/1" );

							if ( firstStrOpt.has_value() && secondStrOpt.has_value() )
							{
								data.push_back( { firstStrOpt.value(), secondStrOpt.value() } );
							}
						}
					}
				}
			}

			return data;
		}

		PositionValidationResult positionValidationResultsFromString( std::string_view name )
		{
			if ( name == "Valid" )
			{
				return PositionValidationResult::Valid;
			}
			if ( name == "Invalid" )
			{
				return PositionValidationResult::Invalid;
			}
			if ( name == "InvalidOrder" )
			{
				return PositionValidationResult::InvalidOrder;
			}
			if ( name == "InvalidGrouping" )
			{
				return PositionValidationResult::InvalidGrouping;
			}
			if ( name == "Custom" )
			{
				return PositionValidationResult::Custom;
			}

			throw std::invalid_argument{ "Unknown position validation result: " + std::string{ name } };
		}

		TEST_P( PositionValidationTest, Test_Position_Validation )
		{
			const auto& param = GetParam();
			const auto& codebooks = getCodebooks();

			const auto& codebookType = codebooks.codebook( CodebookName::Position );
			auto validPosition = codebookType.validatePosition( param.input );
			auto parsedExpectedOutput = positionValidationResultsFromString( param.expectedOutput );

			EXPECT_EQ( parsedExpectedOutput, validPosition );
		}

		INSTANTIATE_TEST_SUITE_P( CodebookPositionValidationSuite, PositionValidationTest, ::testing::ValuesIn( positionValidationData() ) );

		struct PositionsParam
		{
			std::string invalidStandardValue;
			std::string validStandardValue;
		};

		class PositionsTest : public CodebookBaseTest, public ::testing::TestWithParam<PositionsParam>
		{
		};

		static std::vector<PositionsParam> positionsData()
		{
			std::vector<PositionsParam> data;
			const nfx::serialization::json::Document& jsonDataFromFile = loadTestData( TEST_DATA_FILE );

			if ( jsonDataFromFile.contains( "/Positions" ) )
			{
				auto positionsDocOpt = jsonDataFromFile.get<nfx::serialization::json::Document>( "/Positions" );
				if ( !positionsDocOpt.has_value() )
				{
					return data;
				}
				const auto& positionsDoc = positionsDocOpt.value();

				auto arrayOpt = positionsDoc.get<nfx::serialization::json::Document::Array>( "" );
				if ( !arrayOpt )
				{
					return data;
				}

				for ( const auto& item : arrayOpt.value() )
				{
					if ( item.contains( "/0" ) && item.contains( "/1" ) )
					{
						auto firstStrOpt = item.get<std::string>( "/0" );
						auto secondStrOpt = item.get<std::string>( "/1" );

						if ( firstStrOpt.has_value() && secondStrOpt.has_value() )
						{
							data.push_back( { firstStrOpt.value(), secondStrOpt.value() } );
						}
					}
				}
			}
			return data;
		}

		TEST_P( PositionsTest, Test_Positions )
		{
			const auto& param = GetParam();
			const auto& codebooks = getCodebooks();

			const auto& positions = codebooks.codebook( CodebookName::Position );

			EXPECT_FALSE( positions.hasStandardValue( param.invalidStandardValue ) );
			EXPECT_TRUE( positions.hasStandardValue( param.validStandardValue ) );
		}

		INSTANTIATE_TEST_SUITE_P( CodebookPositionsSuite, PositionsTest, ::testing::ValuesIn( positionsData() ) );

		struct StatesParam
		{
			std::string invalidGroup;
			std::string validValue;
			std::string validGroup;
			std::string secondValidValue;
		};

		class StatesTest : public CodebookBaseTest, public ::testing::TestWithParam<StatesParam>
		{
		};

		static std::vector<StatesParam> statesData()
		{
			std::vector<StatesParam> data;
			const nfx::serialization::json::Document& jsonDataFromFile = loadTestData( TEST_DATA_FILE );

			if ( jsonDataFromFile.contains( "/States" ) )
			{
				auto statesDocOpt = jsonDataFromFile.get<nfx::serialization::json::Document>( "/States" );
				if ( !statesDocOpt.has_value() )
				{
					return data;
				}
				const auto& statesDoc = statesDocOpt.value();

				auto statesArrayOpt = statesDoc.get<nfx::serialization::json::Document::Array>( "" );
				if ( !statesArrayOpt.has_value() )
				{
					return data;
				}

				for ( const auto& item : statesArrayOpt.value() )
				{
					if ( item.contains( "/0" ) && item.contains( "/1" ) && item.contains( "/2" ) && item.contains( "/3" ) )
					{
						auto str0Opt = item.get<std::string>( "/0" );
						auto str1Opt = item.get<std::string>( "/1" );
						auto str2Opt = item.get<std::string>( "/2" );
						auto str3Opt = item.get<std::string>( "/3" );

						if ( str0Opt.has_value() && str1Opt.has_value() && str2Opt.has_value() && str3Opt.has_value() )
						{
							data.push_back( { str0Opt.value(), str1Opt.value(), str2Opt.value(), str3Opt.value() } );
						}
					}
				}
			}
			return data;
		}

		TEST_P( StatesTest, Test_States )
		{
			const auto& param = GetParam();
			const auto& codebooks = getCodebooks();

			const auto& states = codebooks.codebook( CodebookName::State );

			EXPECT_FALSE( states.hasGroup( param.invalidGroup ) );
			EXPECT_TRUE( states.hasStandardValue( param.validValue ) );
			EXPECT_TRUE( states.hasGroup( param.validGroup ) );
			EXPECT_TRUE( states.hasStandardValue( param.secondValidValue ) );
		}

		INSTANTIATE_TEST_SUITE_P( CodebookStatesSuite, StatesTest, ::testing::ValuesIn( statesData() ) );

		struct TagParam
		{
			std::string firstTag;
			std::string secondTag;
			std::string thirdTag;
			char thirdTagPrefix;
			std::string customTag;
			char customTagPrefix;
			std::string firstInvalidTag;
			std::string secondInvalidTag;
		};

		class TagTest : public CodebookBaseTest, public ::testing::TestWithParam<TagParam>
		{
		};

		static std::vector<TagParam> tagData()
		{
			std::vector<TagParam> data;
			const nfx::serialization::json::Document& jsonDataFromFile = loadTestData( TEST_DATA_FILE );

			if ( jsonDataFromFile.contains( "/Tag" ) )
			{
				auto tagDocOpt = jsonDataFromFile.get<nfx::serialization::json::Document>( "/Tag" );
				if ( !tagDocOpt.has_value() )
				{
					return data;
				}
				const auto& tagDoc = tagDocOpt.value();

				auto tagArrayOpt = tagDoc.get<nfx::serialization::json::Document::Array>( "" );
				if ( !tagArrayOpt.has_value() )
				{
					return data;
				}

				for ( const auto& item : tagArrayOpt.value() )
				{
					if ( item.contains( "/0" ) && item.contains( "/1" ) && item.contains( "/2" ) &&
						 item.contains( "/3" ) && item.contains( "/4" ) && item.contains( "/5" ) &&
						 item.contains( "/6" ) && item.contains( "/7" ) )
					{
						auto str0Opt = item.get<std::string>( "/0" );
						auto str1Opt = item.get<std::string>( "/1" );
						auto str2Opt = item.get<std::string>( "/2" );
						auto str3Opt = item.get<std::string>( "/3" );
						auto str4Opt = item.get<std::string>( "/4" );
						auto str5Opt = item.get<std::string>( "/5" );
						auto str6Opt = item.get<std::string>( "/6" );
						auto str7Opt = item.get<std::string>( "/7" );

						if ( str0Opt.has_value() && str1Opt.has_value() && str2Opt.has_value() && str3Opt.has_value() && !str3Opt.value().empty() &&
							 str4Opt.has_value() && str5Opt.has_value() && !str5Opt.value().empty() && str6Opt.has_value() && str7Opt.has_value() )
						{
							data.push_back( { str0Opt.value(), str1Opt.value(), str2Opt.value(), ( str3Opt.value() )[0],
								str4Opt.value(), ( str5Opt.value() )[0], str6Opt.value(), str7Opt.value() } );
						}
					}
				}
			}
			return data;
		}

		TEST_P( TagTest, Test_Create_Tag )
		{
			const auto& param = GetParam();
			const auto& codebooks = getCodebooks();

			const auto& codebookType = codebooks.codebook( CodebookName::Position );

			auto metadataTag1 = codebookType.createTag( param.firstTag );
			EXPECT_EQ( param.firstTag, metadataTag1.value() );
			EXPECT_FALSE( metadataTag1.isCustom() );

			auto metadataTag2 = codebookType.createTag( param.secondTag );
			EXPECT_EQ( param.secondTag, metadataTag2.value() );
			EXPECT_FALSE( metadataTag2.isCustom() );

			auto metadataTag3 = codebookType.createTag( param.thirdTag );
			EXPECT_EQ( param.thirdTag, metadataTag3.value() );
			EXPECT_FALSE( metadataTag3.isCustom() );
			EXPECT_EQ( param.thirdTagPrefix, metadataTag3.prefix() );

			auto metadataTag4 = codebookType.createTag( param.customTag );
			EXPECT_EQ( param.customTag, metadataTag4.value() );
			EXPECT_TRUE( metadataTag4.isCustom() );
			EXPECT_EQ( param.customTagPrefix, metadataTag4.prefix() );

			EXPECT_THROW( codebookType.createTag( param.firstInvalidTag ), std::invalid_argument );
			EXPECT_EQ( codebookType.tryCreateTag( param.firstInvalidTag ), std::nullopt );

			EXPECT_THROW( codebookType.createTag( param.secondInvalidTag ), std::invalid_argument );
			EXPECT_EQ( codebookType.tryCreateTag( param.secondInvalidTag ), std::nullopt );
		}

		INSTANTIATE_TEST_SUITE_P( CodebookTagSuite, TagTest, ::testing::ValuesIn( tagData() ) );

		struct DetailTagParam
		{
			std::string validCustomTag;
			std::string firstInvalidCustomTag;
			std::string secondInvalidCustomTag;
		};

		class DetailTagTest : public CodebookBaseTest, public ::testing::TestWithParam<DetailTagParam>
		{
		};

		static std::vector<DetailTagParam> detailTagData()
		{
			std::vector<DetailTagParam> data;
			const nfx::serialization::json::Document& jsonDataFromFile = loadTestData( TEST_DATA_FILE );

			auto detailTagDocOpt = jsonDataFromFile.get<nfx::serialization::json::Document>( "/DetailTag" );
			if ( detailTagDocOpt.has_value() )
			{
				const auto& detailTagDoc = detailTagDocOpt.value();

				auto detailTagArrayOpt = detailTagDoc.get<nfx::serialization::json::Document::Array>( "" );
				if ( detailTagArrayOpt.has_value() )
				{
					for ( const auto& item : detailTagArrayOpt.value() )
					{
						if ( item.get<nfx::serialization::json::Document>( "" ).has_value() )
						{
							auto itemDoc = item.get<nfx::serialization::json::Document>( "" ).value();
							if ( itemDoc.contains( "/0" ) && itemDoc.contains( "/1" ) && itemDoc.contains( "/2" ) )
							{
								auto str0Opt = itemDoc.get<std::string>( "/0" );
								auto str1Opt = itemDoc.get<std::string>( "/1" );
								auto str2Opt = itemDoc.get<std::string>( "/2" );

								if ( str0Opt.has_value() && str1Opt.has_value() && str2Opt.has_value() )
								{
									data.push_back( { str0Opt.value(), str1Opt.value(), str2Opt.value() } );
								}
							}
						}
					}
				}
			}
			return data;
		}

		TEST_P( DetailTagTest, Test_Detail_Tag )
		{
			const auto& param = GetParam();
			const auto& codebooks = getCodebooks();

			const auto& codebook = codebooks.codebook( CodebookName::Detail );

			EXPECT_NE( codebook.tryCreateTag( param.validCustomTag ), std::nullopt );
			EXPECT_EQ( codebook.tryCreateTag( param.firstInvalidCustomTag ), std::nullopt );
			EXPECT_EQ( codebook.tryCreateTag( param.secondInvalidCustomTag ), std::nullopt );

			EXPECT_THROW( codebook.createTag( param.firstInvalidCustomTag ), std::invalid_argument );
			EXPECT_THROW( codebook.createTag( param.secondInvalidCustomTag ), std::invalid_argument );
		}

		INSTANTIATE_TEST_SUITE_P( CodebookDetailTagSuite, DetailTagTest, ::testing::ValuesIn( detailTagData() ) );
	} // namespace CodebookTestParametrized

	namespace CodebooksTests
	{
		TEST( CodebooksTests, Test_CodebookName_Prefix_Conversions )
		{
			const std::map<dnv::vista::sdk::CodebookName, std::string_view> expectedMappings = { { dnv::vista::sdk::CodebookName::Quantity, "qty" },
				{ dnv::vista::sdk::CodebookName::Content, "cnt" }, { dnv::vista::sdk::CodebookName::Calculation, "calc" },
				{ dnv::vista::sdk::CodebookName::State, "state" }, { dnv::vista::sdk::CodebookName::Command, "cmd" },
				{ dnv::vista::sdk::CodebookName::Type, "type" }, { dnv::vista::sdk::CodebookName::FunctionalServices, "funct.svc" },
				{ dnv::vista::sdk::CodebookName::MaintenanceCategory, "maint.cat" }, { dnv::vista::sdk::CodebookName::ActivityType, "act.type" },
				{ dnv::vista::sdk::CodebookName::Position, "pos" }, { dnv::vista::sdk::CodebookName::Detail, "detail" } };

			for ( const auto& pair : expectedMappings )
			{
				const auto cbName = pair.first;
				const auto expectedPrefix = pair.second;

				SCOPED_TRACE( "Testing CodebookName toPrefix: " + std::string{ expectedPrefix } );
				std::string actualPrefix;
				ASSERT_NO_THROW( { actualPrefix = dnv::vista::sdk::CodebookNames::toPrefix( cbName ); } );
				ASSERT_EQ( expectedPrefix, actualPrefix );

				SCOPED_TRACE( "Testing fromPrefix round trip for: " + std::string{ expectedPrefix } );
				dnv::vista::sdk::CodebookName roundTripName;
				ASSERT_NO_THROW( { roundTripName = dnv::vista::sdk::CodebookNames::fromPrefix( actualPrefix ); } );
				ASSERT_EQ( cbName, roundTripName );
			}

			ASSERT_THROW( (void)dnv::vista::sdk::CodebookNames::fromPrefix( "" ), std::invalid_argument );
			ASSERT_THROW( (void)dnv::vista::sdk::CodebookNames::fromPrefix( "invalid_prefix" ), std::invalid_argument );
			ASSERT_THROW( (void)dnv::vista::sdk::CodebookNames::fromPrefix( "po" ), std::invalid_argument );

			ASSERT_THROW( (void)dnv::vista::sdk::CodebookNames::fromPrefix( "QTY" ), std::invalid_argument );
			ASSERT_THROW( (void)dnv::vista::sdk::CodebookNames::fromPrefix( "Pos" ), std::invalid_argument );
			ASSERT_THROW( (void)dnv::vista::sdk::CodebookNames::fromPrefix( "funct.SVC" ), std::invalid_argument );

			const auto invalidCbName = static_cast<dnv::vista::sdk::CodebookName>( 999 );
			ASSERT_THROW( (void)dnv::vista::sdk::CodebookNames::toPrefix( invalidCbName ), std::invalid_argument );
		}
	} // namespace CodebooksTests
} // namespace dnv::vista::sdk::test
