/**
 * @file TESTS_HashingConsistencyUniversalIdBuilder.cpp
 * @brief Hash consistency validation suite for UniversalIdBuilder class
 * @details This test suite validates hash function behavior, distribution quality, and consistency
 *          across all UniversalIdBuilder fluent interface operations including component setting,
 *          parsing methods, and STL container integration with maritime UniversalIdBuilder instances.
 */

#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <gtest/gtest.h>

#include <dnv/vista/sdk/ImoNumber.h>
#include <dnv/vista/sdk/LocalId.h>
#include <dnv/vista/sdk/LocalIdBuilder.h>
#include <dnv/vista/sdk/ParsingErrors.h>
#include <dnv/vista/sdk/UniversalId.h>
#include <dnv/vista/sdk/UniversalIdBuilder.h>
#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/VISVersion.h>

namespace dnv::vista::sdk::tests
{
	//========================================================================
	// UniversalIdBuilder hashing consistency, uniqueness and validation tests
	//========================================================================

	//----------------------------------------------
	// Dataset
	//----------------------------------------------

	namespace
	{
		std::string imoNumbersFilePath()
		{
#ifdef VISTA_SDK_TESTDATA_DIR
			return std::string{ VISTA_SDK_TESTDATA_DIR } + "/ImoNumbers_unique.txt";
#else
			return "testdata/ImoNumbers_unique.txt";
#endif
		}

		std::vector<std::string> loadImoNumbers( size_t maxCount = 100 )
		{
			std::vector<std::string> imoNumbers;
			std::ifstream file( imoNumbersFilePath() );
			if ( !file.is_open() )
			{
				return imoNumbers;
			}

			std::string line;
			while ( std::getline( file, line ) && imoNumbers.size() < maxCount )
			{
				if ( line.empty() )
				{
					continue;
				}

				try
				{
					auto imoNumber = ImoNumber::parse( line );
					imoNumbers.push_back( imoNumber.toString() );
				}
				catch ( const std::exception& )
				{
				}
			}

			file.close();
			return imoNumbers;
		}

		std::vector<std::string> testLocalIdStrings()
		{
			return {
				"/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
				"/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened",
				"/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low",
				"/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage",
				"/dnv-v2/vis-3-5a/411.1/C101.31/meta/qty-temperature/cnt-cooling.water/pos-inlet",
				"/dnv-v2/vis-3-6a/612.42/meta/qty-pressure/cnt-fuel.oil" };
		}
	} // namespace

	//----------------------------------------------
	// Test fixture
	//----------------------------------------------

	class UniversalIdBuilderHashTests : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			if ( s_imoNumbers.empty() )
			{
				s_imoNumbers = loadImoNumbers( 50 );
			}
		}

		// Helper method to create UniversalIdBuilder from IMO and LocalId strings
		UniversalIdBuilder createBuilder( const std::string& imoStr, const std::string& localIdStr )
		{
			auto imoNumber = ImoNumber::parse( imoStr );
			auto localId = LocalId::parse( localIdStr );
			auto localIdBuilder = LocalIdBuilder::parse( localIdStr );
			auto visVersion = localId.visVersion();

			return UniversalIdBuilder::create( visVersion )
				.withImoNumber( imoNumber )
				.withLocalId( localIdBuilder );
		}

		static std::vector<std::string> s_imoNumbers;
	};

	std::vector<std::string> UniversalIdBuilderHashTests::s_imoNumbers;

	//----------------------------------------------
	// Hash uniqueness validation
	//----------------------------------------------

	TEST_F( UniversalIdBuilderHashTests, Test_Hash_Uniqueness_DifferentComponents )
	{
		// Test that different component combinations produce unique hashes
		auto localIdStrings = testLocalIdStrings();

		auto builder1 = createBuilder( s_imoNumbers[0], localIdStrings[0] );
		auto builder2 = createBuilder( s_imoNumbers[1], localIdStrings[0] ); // Different IMO
		auto builder3 = createBuilder( s_imoNumbers[0], localIdStrings[1] ); // Different LocalId
		auto builder4 = createBuilder( s_imoNumbers[1], localIdStrings[1] ); // Both different

		auto hash1 = builder1.hashCode();
		auto hash2 = builder2.hashCode();
		auto hash3 = builder3.hashCode();
		auto hash4 = builder4.hashCode();

		EXPECT_NE( hash1, hash2 ) << "Different IMO numbers should produce different hashes";
		EXPECT_NE( hash1, hash3 ) << "Different LocalIds should produce different hashes";
		EXPECT_NE( hash1, hash4 ) << "Different component combinations should produce different hashes";
		EXPECT_NE( hash2, hash3 ) << "Different component combinations should produce different hashes";
		EXPECT_NE( hash2, hash4 ) << "Different component combinations should produce different hashes";
		EXPECT_NE( hash3, hash4 ) << "Different component combinations should produce different hashes";
	}

	TEST_F( UniversalIdBuilderHashTests, Test_Hash_Uniqueness_ComprehensiveDataset )
	{
		std::unordered_set<std::size_t> uniqueHashes;
		std::vector<UniversalIdBuilder> allBuilders;
		size_t collisionCount = 0;

		auto localIdStrings = testLocalIdStrings();

		// Create builders from all IMO/LocalId combinations
		for ( const auto& imoStr : s_imoNumbers )
		{
			for ( const auto& localIdStr : localIdStrings )
			{
				try
				{
					auto builder = createBuilder( imoStr, localIdStr );
					allBuilders.push_back( builder );
				}
				catch ( const std::exception& )
				{
				}
			}
		}

		// Test hash uniqueness
		for ( const auto& builder : allBuilders )
		{
			std::size_t builderHash = builder.hashCode();

			if ( uniqueHashes.find( builderHash ) != uniqueHashes.end() )
			{
				collisionCount++;
			}
			else
			{
				uniqueHashes.insert( builderHash );
			}
		}

		// Report statistics
		double uniquenessRatio = static_cast<double>( uniqueHashes.size() ) / static_cast<double>( allBuilders.size() );

		std::cout << "\nUniversalIdBuilder Hash Uniqueness Statistics:\n";
		std::cout << "  Total Builders: " << allBuilders.size() << "\n";
		std::cout << "  Unique hashes: " << uniqueHashes.size() << "\n";
		std::cout << "  Hash collisions: " << collisionCount << "\n";
		std::cout << "  Uniqueness ratio: " << std::fixed << std::setprecision( 1 ) << ( uniquenessRatio * 100.0 ) << "%\n";

		EXPECT_EQ( collisionCount, 0 )
			<< "UniversalIdBuilder hash function should produce zero collisions for unique builders. "
			<< "Detected " << collisionCount << " collisions out of " << allBuilders.size() << " unique builders";

		EXPECT_EQ( uniqueHashes.size(), allBuilders.size() )
			<< "Each unique UniversalIdBuilder should have a unique hash";
	}

	//----------------------------------------------
	// Hash consistency and immutability tests
	//----------------------------------------------

	TEST_F( UniversalIdBuilderHashTests, Test_Hash_Consistency_CopyConstructor )
	{
		auto original = createBuilder( s_imoNumbers[0], testLocalIdStrings()[0] );

		UniversalIdBuilder copy( original );
		auto originalHash = original.hashCode();
		auto copyHash = copy.hashCode();

		EXPECT_EQ( originalHash, copyHash ) << "Copy constructor should preserve hash code";
		EXPECT_EQ( original, copy ) << "Copy should be equal to original";
	}

	TEST_F( UniversalIdBuilderHashTests, Test_Hash_Consistency_MoveConstructor )
	{
		auto original = createBuilder( s_imoNumbers[0], testLocalIdStrings()[0] );
		auto originalHash = original.hashCode();

		UniversalIdBuilder moved( std::move( original ) );
		auto movedHash = moved.hashCode();

		EXPECT_EQ( originalHash, movedHash ) << "Move constructor should preserve hash code";
	}

	TEST_F( UniversalIdBuilderHashTests, Test_Hash_Consistency_RepeatAccess )
	{
		auto builder = createBuilder( s_imoNumbers[0], testLocalIdStrings()[0] );

		auto hash1 = builder.hashCode();
		auto hash2 = builder.hashCode();
		auto hash3 = builder.hashCode();

		EXPECT_EQ( hash1, hash2 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash1, hash3 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash2, hash3 ) << "Multiple hash access should be consistent";
	}

	TEST_F( UniversalIdBuilderHashTests, Test_Hash_Consistency_FluentInterface )
	{
		// Test that fluent interface operations produce consistent hashes
		auto imoNumber = ImoNumber::parse( s_imoNumbers[0] );
		auto localIdBuilder = LocalIdBuilder::parse( testLocalIdStrings()[0] );
		auto visVersion = VisVersion::v3_4a;

		// Method 1: Step by step
		auto builder1 = UniversalIdBuilder::create( visVersion )
							.withImoNumber( imoNumber )
							.withLocalId( localIdBuilder );

		// Method 2: Different order
		auto builder2 = UniversalIdBuilder::create( visVersion )
							.withLocalId( localIdBuilder )
							.withImoNumber( imoNumber );

		auto hash1 = builder1.hashCode();
		auto hash2 = builder2.hashCode();

		EXPECT_EQ( hash1, hash2 ) << "Builder construction order should not affect hash";
		EXPECT_EQ( builder1, builder2 ) << "Builders with same components should be equal";
	}

	//----------------------------------------------
	// Fluent interface hash consistency tests
	//----------------------------------------------

	TEST_F( UniversalIdBuilderHashTests, Test_Hash_Consistency_FluentOperations )
	{
		auto imoNumber1 = ImoNumber::parse( s_imoNumbers[0] );
		auto imoNumber2 = ImoNumber::parse( s_imoNumbers[1] );
		auto localIdBuilder = LocalIdBuilder::parse( testLocalIdStrings()[0] );
		auto visVersion = VisVersion::v3_4a;

		// Test withImoNumber consistency
		auto builder1 = UniversalIdBuilder::create( visVersion )
							.withLocalId( localIdBuilder )
							.withImoNumber( imoNumber1 );

		auto builder2 = UniversalIdBuilder::create( visVersion )
							.withLocalId( localIdBuilder )
							.withImoNumber( imoNumber1 );

		EXPECT_EQ( builder1.hashCode(), builder2.hashCode() ) << "Same components should produce same hash";
		EXPECT_EQ( builder1, builder2 ) << "Same components should produce equal builders";

		// Test different components produce different hashes
		auto builder3 = UniversalIdBuilder::create( visVersion )
							.withLocalId( localIdBuilder )
							.withImoNumber( imoNumber2 );

		EXPECT_NE( builder1.hashCode(), builder3.hashCode() ) << "Different IMO numbers should produce different hashes";
		EXPECT_NE( builder1, builder3 ) << "Different components should produce unequal builders";
	}

	TEST_F( UniversalIdBuilderHashTests, Test_Hash_Consistency_TryMethods )
	{
		auto imoNumber = ImoNumber::parse( s_imoNumbers[0] );
		auto localIdBuilder = LocalIdBuilder::parse( testLocalIdStrings()[0] );
		auto visVersion = VisVersion::v3_4a;

		// Test tryWithImoNumber vs withImoNumber
		auto builder1 = UniversalIdBuilder::create( visVersion )
							.withLocalId( localIdBuilder )
							.withImoNumber( imoNumber );

		auto builder2 = UniversalIdBuilder::create( visVersion )
							.withLocalId( localIdBuilder )
							.tryWithImoNumber( std::make_optional( imoNumber ) );

		EXPECT_EQ( builder1.hashCode(), builder2.hashCode() ) << "tryWithImoNumber should produce same hash as withImoNumber";
		EXPECT_EQ( builder1, builder2 ) << "tryWithImoNumber should produce equal builder as withImoNumber";

		// Test tryWithLocalId vs withLocalId
		auto builder3 = UniversalIdBuilder::create( visVersion )
							.withImoNumber( imoNumber )
							.withLocalId( localIdBuilder );

		auto builder4 = UniversalIdBuilder::create( visVersion )
							.withImoNumber( imoNumber )
							.tryWithLocalId( std::make_optional( localIdBuilder ) );

		EXPECT_EQ( builder3.hashCode(), builder4.hashCode() ) << "tryWithLocalId should produce same hash as withLocalId";
		EXPECT_EQ( builder3, builder4 ) << "tryWithLocalId should produce equal builder as withLocalId";
	}

	//----------------------------------------------
	// Parse methods and construction consistency
	//----------------------------------------------

	TEST_F( UniversalIdBuilderHashTests, Test_Hash_Consistency_ParseVsConstruction )
	{
		auto imoNumber = ImoNumber::parse( s_imoNumbers[0] );
		auto localIdBuilder = LocalIdBuilder::parse( testLocalIdStrings()[0] );
		auto visVersion = VisVersion::v3_4a;

		// Method 1: Manual construction
		auto constructedBuilder = UniversalIdBuilder::create( visVersion )
									  .withImoNumber( imoNumber )
									  .withLocalId( localIdBuilder );

		// Method 2: Parse from string
		auto universalId = constructedBuilder.build();
		auto universalIdString = universalId.toString();

		try
		{
			auto parsedBuilder = UniversalIdBuilder::parse( universalIdString );

			EXPECT_EQ( constructedBuilder.hashCode(), parsedBuilder.hashCode() )
				<< "Parsed builder should have same hash as constructed builder";
			EXPECT_EQ( constructedBuilder, parsedBuilder )
				<< "Parsed builder should equal constructed builder";
		}
		catch ( const std::exception& ex )
		{
			FAIL() << "Failed to parse constructed UniversalIdBuilder string: '" << universalIdString << "'\n"
				   << "  Parse error: " << ex.what();
		}
	}

	TEST_F( UniversalIdBuilderHashTests, Test_Hash_Consistency_TryParseMethod )
	{
		auto imoNumber = ImoNumber::parse( s_imoNumbers[0] );
		auto localIdBuilder = LocalIdBuilder::parse( testLocalIdStrings()[0] );
		auto visVersion = VisVersion::v3_4a;

		auto constructedBuilder = UniversalIdBuilder::create( visVersion )
									  .withImoNumber( imoNumber )
									  .withLocalId( localIdBuilder );

		auto universalId = constructedBuilder.build();
		auto universalIdString = universalId.toString();

		std::optional<UniversalIdBuilder> parsedBuilder;
		bool parseSuccess = UniversalIdBuilder::tryParse( universalIdString, parsedBuilder );

		EXPECT_TRUE( parseSuccess ) << "tryParse should succeed for valid UniversalIdBuilder string";
		ASSERT_TRUE( parsedBuilder.has_value() ) << "tryParse should return valid UniversalIdBuilder";

		EXPECT_EQ( constructedBuilder.hashCode(), parsedBuilder->hashCode() )
			<< "tryParse should produce same hash as manual construction";
		EXPECT_EQ( constructedBuilder, *parsedBuilder )
			<< "tryParse should produce equal UniversalIdBuilder";
	}

	//----------------------------------------------
	// STL container integration tests
	//----------------------------------------------

	TEST_F( UniversalIdBuilderHashTests, Test_StdHash_UnorderedMap_Integration )
	{
		std::unordered_map<UniversalIdBuilder, std::string> builderMap;
		std::vector<UniversalIdBuilder> testBuilders;
		auto localIdStrings = testLocalIdStrings();

		// Create test builders
		for ( size_t i = 0; i < std::min( s_imoNumbers.size(), size_t( 10 ) ); ++i )
		{
			for ( size_t j = 0; j < std::min( localIdStrings.size(), size_t( 2 ) ); ++j )
			{
				try
				{
					testBuilders.push_back( createBuilder( s_imoNumbers[i], localIdStrings[j] ) );
				}
				catch ( ... )
				{
				}
			}
		}

		// Test insertion and lookup
		for ( size_t i = 0; i < testBuilders.size(); ++i )
		{
			std::string description = "Builder_" + std::to_string( i );
			builderMap[testBuilders[i]] = description;
		}

		EXPECT_EQ( builderMap.size(), testBuilders.size() ) << "STL map should contain all unique builders";

		// Test lookup functionality
		size_t successfulLookups = 0;
		for ( const auto& builder : testBuilders )
		{
			auto it = builderMap.find( builder );
			if ( it != builderMap.end() )
			{
				successfulLookups++;
				EXPECT_FALSE( it->second.empty() ) << "Lookup should return valid description";
			}
		}

		EXPECT_EQ( successfulLookups, testBuilders.size() ) << "All lookups should succeed";
	}

	TEST_F( UniversalIdBuilderHashTests, Test_StdHash_UnorderedSet_Integration )
	{
		std::unordered_set<UniversalIdBuilder> builderSet;
		auto localIdStrings = testLocalIdStrings();

		auto builder1 = createBuilder( s_imoNumbers[0], localIdStrings[0] );
		auto builder2 = createBuilder( s_imoNumbers[1], localIdStrings[0] );
		auto builder3 = createBuilder( s_imoNumbers[0], localIdStrings[1] );
		auto builder4 = createBuilder( s_imoNumbers[1], localIdStrings[1] );

		builderSet.insert( builder1 );
		builderSet.insert( builder2 );
		builderSet.insert( builder3 );
		builderSet.insert( builder4 );

		builderSet.insert( builder1 ); // Duplicate
		builderSet.insert( builder2 ); // Duplicate

		EXPECT_EQ( builderSet.size(), 4 ) << "Set should contain exactly 4 unique builders";

		EXPECT_NE( builderSet.find( builder1 ), builderSet.end() ) << "Builder 1 should be in set";
		EXPECT_NE( builderSet.find( builder2 ), builderSet.end() ) << "Builder 2 should be in set";
		EXPECT_NE( builderSet.find( builder3 ), builderSet.end() ) << "Builder 3 should be in set";
		EXPECT_NE( builderSet.find( builder4 ), builderSet.end() ) << "Builder 4 should be in set";
	}

	//----------------------------------------------
	// Hash validation
	//----------------------------------------------

	TEST_F( UniversalIdBuilderHashTests, Test_Hash_StdHash_Consistency )
	{
		auto builder = createBuilder( s_imoNumbers[0], testLocalIdStrings()[0] );

		// Test std::hash specialization
		std::hash<UniversalIdBuilder> stdHasher;
		auto stdHash = stdHasher( builder );
		auto memberHash = builder.hashCode();

		EXPECT_EQ( stdHash, memberHash ) << "std::hash specialization should match member hashCode()";
	}

	//----------------------------------------------
	// Component hash relationship tests
	//----------------------------------------------

	TEST_F( UniversalIdBuilderHashTests, Test_Hash_Component_Relationship )
	{
		auto imoNumber1 = ImoNumber::parse( s_imoNumbers[0] );
		auto imoNumber2 = ImoNumber::parse( s_imoNumbers[1] );
		auto localIdBuilder1 = LocalIdBuilder::parse( testLocalIdStrings()[0] );
		auto localIdBuilder2 = LocalIdBuilder::parse( testLocalIdStrings()[1] );
		auto visVersion = VisVersion::v3_4a;

		auto builder1 = UniversalIdBuilder::create( visVersion )
							.withImoNumber( imoNumber1 )
							.withLocalId( localIdBuilder1 );

		auto builder2 = UniversalIdBuilder::create( visVersion )
							.withImoNumber( imoNumber2 )
							.withLocalId( localIdBuilder1 ); // Different IMO

		auto builder3 = UniversalIdBuilder::create( visVersion )
							.withImoNumber( imoNumber1 )
							.withLocalId( localIdBuilder2 ); // Different LocalId

		auto hash1 = builder1.hashCode();
		auto hash2 = builder2.hashCode();
		auto hash3 = builder3.hashCode();

		// Different components should produce different hashes
		EXPECT_NE( hash1, hash2 ) << "Different IMO numbers should result in different builder hashes";
		EXPECT_NE( hash1, hash3 ) << "Different LocalIds should result in different builder hashes";
		EXPECT_NE( hash2, hash3 ) << "Different combinations should result in different builder hashes";

		// Component access should not affect hash
		auto imoComponent = builder1.imoNumber();
		auto localIdComponent = builder1.localId();
		auto hashAfterAccess = builder1.hashCode();

		EXPECT_EQ( hash1, hashAfterAccess ) << "Component access should not affect hash";
		EXPECT_TRUE( imoComponent.has_value() ) << "IMO number should be accessible";
		EXPECT_TRUE( localIdComponent.has_value() ) << "LocalId should be accessible";
	}
} // namespace dnv::vista::sdk::tests
