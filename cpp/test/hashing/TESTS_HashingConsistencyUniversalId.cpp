/**
 * @file TESTS_HashingConsistencyUniversalId.cpp
 * @brief Hash consistency validation suite for UniversalId class
 * @details This test suite validates hash function behavior, distribution quality, and consistency
 *          across all UniversalId operations including copy/move semantics, parsing methods,
 *          and STL container integration with maritime UniversalId instances.
 */

#include <fstream>
#include <iomanip>

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
	// UniversalId hashing consistency, uniqueness and validation tests
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

		std::vector<std::string> imoNumbersFromFile( size_t maxCount = 500 )
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
	} // namespace

	//----------------------------------------------
	// Test fixture
	//----------------------------------------------

	class UniversalIdHashTests : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			if ( s_imoNumbers.empty() )
			{
				s_imoNumbers = imoNumbersFromFile( 1000 );
			}
		}

		UniversalId createUniversalId( const std::string& imoStr, const std::string& localIdStr )
		{
			auto imoNumber = ImoNumber::parse( imoStr );
			auto localId = LocalId::parse( localIdStr );
			auto visVersion = localId.visVersion();
			return UniversalIdBuilder::create( visVersion )
				.withImoNumber( imoNumber )
				.withLocalId( LocalIdBuilder::parse( localIdStr ) )
				.build();
		}

		static std::vector<std::string> s_imoNumbers;
	};

	std::vector<std::string> UniversalIdHashTests::s_imoNumbers;

	//----------------------------------------------
	// Hash uniqueness validation
	//----------------------------------------------

	TEST_F( UniversalIdHashTests, Test_Hash_Uniqueness_DifferentUniversalIds )
	{
		// Test different UniversalId combinations have unique hashes using IMO file data
		// Use first 4 IMO numbers from static cache

		auto universalId1 = createUniversalId( s_imoNumbers[0], "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto universalId2 = createUniversalId( s_imoNumbers[1], "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto universalId3 = createUniversalId( s_imoNumbers[2], "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );
		auto universalId4 = createUniversalId( s_imoNumbers[3], "/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low" );

		auto hash1 = universalId1.hashCode();
		auto hash2 = universalId2.hashCode();
		auto hash3 = universalId3.hashCode();
		auto hash4 = universalId4.hashCode();

		EXPECT_NE( hash1, hash2 ) << "Different IMO numbers with same LocalId should have different hashes";
		EXPECT_NE( hash1, hash3 ) << "Same IMO number with different LocalIds should have different hashes";
		EXPECT_NE( hash1, hash4 ) << "Different UniversalIds should have different hashes";
		EXPECT_NE( hash2, hash3 ) << "Different UniversalIds should have different hashes";
		EXPECT_NE( hash2, hash4 ) << "Different UniversalIds should have different hashes";
		EXPECT_NE( hash3, hash4 ) << "Different UniversalIds should have different hashes";
	}

	TEST_F( UniversalIdHashTests, Test_Hash_Uniqueness_Smoke )
	{
		std::unordered_set<std::size_t> uniqueHashes;
		std::vector<UniversalId> allUniversalIds;
		size_t collisionCount = 0;

		// Use first 200 IMO numbers from static cache
		size_t imoCount = std::min( static_cast<size_t>( 200 ), s_imoNumbers.size() );
		const std::vector<std::string> localIdStrings = {
			"/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
			"/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened",
			"/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low",
			"/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" };

		// Create UniversalIds from IMO numbers file and LocalId combinations
		for ( size_t i = 0; i < imoCount; ++i )
		{
			const auto& imoStr = s_imoNumbers[i];
			for ( const auto& localIdStr : localIdStrings )
			{
				try
				{
					auto universalId = createUniversalId( imoStr, localIdStr );
					allUniversalIds.push_back( universalId );
				}
				catch ( const std::exception& )
				{
				}
			}
		}

		// Test hash uniqueness across the entire dataset
		for ( const auto& universalId : allUniversalIds )
		{
			std::size_t universalIdHash = universalId.hashCode();

			if ( uniqueHashes.find( universalIdHash ) != uniqueHashes.end() )
			{
				collisionCount++;
			}
			else
			{
				uniqueHashes.insert( universalIdHash );
			}
		}

		// Calculate and report statistics
		double uniquenessRatio = static_cast<double>( uniqueHashes.size() ) / static_cast<double>( allUniversalIds.size() );

		std::cout << "\nUniversalId Hash Uniqueness Statistics:\n";
		std::cout << "  Total UniversalIds: " << allUniversalIds.size() << "\n";
		std::cout << "  Unique hashes: " << uniqueHashes.size() << "\n";
		std::cout << "  Hash collisions: " << collisionCount << "\n";
		std::cout << "  Uniqueness ratio: " << std::fixed << std::setprecision( 1 ) << ( uniquenessRatio * 100.0 ) << "%\n";

		// Assert hash quality requirements
		EXPECT_EQ( collisionCount, 0 )
			<< "Hash function should produce zero collisions for unique UniversalIds. "
			<< "Detected " << collisionCount << " collisions out of " << allUniversalIds.size() << " unique UniversalIds";

		EXPECT_EQ( uniqueHashes.size(), allUniversalIds.size() )
			<< "Each unique UniversalId should have a unique hash. "
			<< "Expected " << allUniversalIds.size() << " unique hashes, got " << uniqueHashes.size();

		EXPECT_GE( uniquenessRatio, 1.0 )
			<< "Hash uniqueness ratio should be 100% for unique dataset. "
			<< "Achieved: " << ( uniquenessRatio * 100.0 ) << "%";
	}

	//----------------------------------------------
	// Hash consistency and immutability tests
	//----------------------------------------------

	TEST_F( UniversalIdHashTests, Test_Hash_Consistency_CopyConstructor )
	{
		auto imoNumbers = imoNumbersFromFile( 1 );
		auto original = createUniversalId( imoNumbers[0], "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );

		UniversalId copy( original );
		auto originalHash = original.hashCode();
		auto copyHash = copy.hashCode();

		EXPECT_EQ( originalHash, copyHash ) << "Copy constructor should preserve hash code";
		EXPECT_EQ( original, copy ) << "Copy should be equal to original";
	}

	TEST_F( UniversalIdHashTests, Test_Hash_Consistency_MoveConstructor )
	{
		auto imoNumbers = imoNumbersFromFile( 1 );
		auto original = createUniversalId( imoNumbers[0], "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto originalHash = original.hashCode();

		UniversalId moved( std::move( original ) );
		auto movedHash = moved.hashCode();

		EXPECT_EQ( originalHash, movedHash ) << "Move constructor should preserve hash code";
	}

	TEST_F( UniversalIdHashTests, Test_Hash_Consistency_RepeatAccess )
	{
		auto imoNumbers = imoNumbersFromFile( 1 );
		auto universalId = createUniversalId( imoNumbers[0], "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );

		auto hash1 = universalId.hashCode();
		auto hash2 = universalId.hashCode();
		auto hash3 = universalId.hashCode();

		EXPECT_EQ( hash1, hash2 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash1, hash3 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash2, hash3 ) << "Multiple hash access should be consistent";
	}

	TEST_F( UniversalIdHashTests, Test_Hash_Consistency_ComponentAccess )
	{
		auto imoNumbers = imoNumbersFromFile( 1 );
		auto universalId = createUniversalId( imoNumbers[0], "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto universalIdHash = universalId.hashCode();

		// Access components and verify hash remains stable
		auto imoNumber = universalId.imoNumber();
		auto localId = universalId.localId();
		auto toString = universalId.toString();

		auto hashAfterAccess = universalId.hashCode();
		EXPECT_EQ( universalIdHash, hashAfterAccess ) << "Hash should remain stable after component access";

		// Verify components are valid
		EXPECT_EQ( imoNumber.toString(), imoNumbers[0] ) << "IMO number should be accessible";
		EXPECT_EQ( localId.toString(), "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" ) << "LocalId should be accessible";
		EXPECT_FALSE( toString.empty() ) << "ToString should produce valid output";
	}

	//----------------------------------------------
	// Parse methods and construction consistency
	//----------------------------------------------

	TEST_F( UniversalIdHashTests, Test_Hash_Consistency_ParseVsConstruction )
	{
		// Test parsing vs construction consistency using IMO file data
		auto imoNumbers = imoNumbersFromFile( 3 );
		const std::vector<std::pair<std::string, std::string>> testCases = {
			{ imoNumbers[0], "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" },
			{ imoNumbers[1], "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" },
			{ imoNumbers[2], "/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low" } };

		for ( const auto& [imoStr, localIdStr] : testCases )
		{
			// Method 1: Construct from components
			auto constructedUniversalId = createUniversalId( imoStr, localIdStr );
			auto constructedHash = constructedUniversalId.hashCode();

			// Method 2: Parse from string representation
			auto universalIdString = constructedUniversalId.toString();

			try
			{
				auto parsedUniversalId = UniversalId::parse( universalIdString );
				auto parsedHash = parsedUniversalId.hashCode();

				EXPECT_EQ( constructedHash, parsedHash )
					<< "Hash should be consistent between construction and parsing:\n"
					<< "  Constructed (hash: " << constructedHash << "): " << universalIdString << "\n"
					<< "  Parsed (hash: " << parsedHash << "): " << parsedUniversalId.toString();

				EXPECT_EQ( constructedUniversalId, parsedUniversalId )
					<< "UniversalId objects should be equal for construction vs parsing:\n"
					<< "  Constructed: " << universalIdString << "\n"
					<< "  Parsed: " << parsedUniversalId.toString();
			}
			catch ( const std::exception& ex )
			{
				FAIL() << "Failed to parse constructed UniversalId string: '" << universalIdString << "'\n"
					   << "  Parse error: " << ex.what();
			}
		}
	}

	TEST_F( UniversalIdHashTests, Test_Hash_Consistency_TryParseMethod )
	{
		// Test tryParse method consistency using IMO file data
		auto imoNumbers = imoNumbersFromFile( 1 );
		auto constructedUniversalId = createUniversalId( imoNumbers[0], "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto constructedHash = constructedUniversalId.hashCode();
		auto universalIdString = constructedUniversalId.toString();

		ParsingErrors errors;
		std::optional<UniversalId> parsedUniversalId;

		bool parseSuccess = UniversalId::tryParse( universalIdString, errors, parsedUniversalId );

		EXPECT_TRUE( parseSuccess ) << "tryParse should succeed for valid UniversalId string";
		ASSERT_TRUE( parsedUniversalId.has_value() ) << "tryParse should return valid UniversalId";

		auto parsedHash = parsedUniversalId->hashCode();
		EXPECT_EQ( constructedHash, parsedHash ) << "tryParse should produce same hash as construction";
		EXPECT_EQ( constructedUniversalId, *parsedUniversalId ) << "tryParse should produce equal UniversalId";
	}

	//----------------------------------------------
	// STL container integration tests
	//----------------------------------------------

	TEST_F( UniversalIdHashTests, Test_StdHash_UnorderedMap_Integration )
	{
		std::unordered_map<UniversalId, std::string> universalIdMap;
		std::vector<UniversalId> testUniversalIds;

		auto imoNumbers = imoNumbersFromFile( 100 );
		const std::vector<std::string> localIdStrings = {
			"/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
			"/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" };

		for ( const auto& imoStr : imoNumbers )
		{
			for ( const auto& localIdStr : localIdStrings )
			{
				try
				{
					testUniversalIds.push_back( createUniversalId( imoStr, localIdStr ) );
				}
				catch ( ... )
				{
				}
			}
		}

		// Test insertion and lookup with UniversalIds
		for ( size_t i = 0; i < testUniversalIds.size(); ++i )
		{
			std::string description = "Vessel_" + std::to_string( i );
			universalIdMap[testUniversalIds[i]] = description;
		}

		EXPECT_EQ( universalIdMap.size(), testUniversalIds.size() ) << "STL map should contain all unique UniversalIds";

		// Test lookup functionality
		size_t successfulLookups = 0;
		for ( const auto& universalId : testUniversalIds )
		{
			auto it = universalIdMap.find( universalId );
			if ( it != universalIdMap.end() )
			{
				successfulLookups++;
				EXPECT_FALSE( it->second.empty() ) << "Lookup should return valid description";
			}
		}

		EXPECT_EQ( successfulLookups, testUniversalIds.size() ) << "All lookups should succeed";
	}

	TEST_F( UniversalIdHashTests, Test_StdHash_Specialization_UnorderedSet )
	{
		std::unordered_set<UniversalId> universalIdSet;

		auto imoNumbers = imoNumbersFromFile( 4 );
		ASSERT_GE( imoNumbers.size(), 4 ) << "Need at least 4 IMO numbers from file for this test";
		auto universalId1 = createUniversalId( imoNumbers[0], "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto universalId2 = createUniversalId( imoNumbers[1], "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto universalId3 = createUniversalId( imoNumbers[2], "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );
		auto universalId4 = createUniversalId( imoNumbers[3], "/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low" );

		universalIdSet.insert( universalId1 );
		universalIdSet.insert( universalId2 );
		universalIdSet.insert( universalId3 );
		universalIdSet.insert( universalId4 );

		universalIdSet.insert( universalId1 ); // Duplicate
		universalIdSet.insert( universalId2 ); // Duplicate

		EXPECT_EQ( universalIdSet.size(), 4 ) << "Set should contain exactly 4 unique UniversalIds";

		EXPECT_NE( universalIdSet.find( universalId1 ), universalIdSet.end() ) << "UniversalId 1 should be in set";
		EXPECT_NE( universalIdSet.find( universalId2 ), universalIdSet.end() ) << "UniversalId 2 should be in set";
		EXPECT_NE( universalIdSet.find( universalId3 ), universalIdSet.end() ) << "UniversalId 3 should be in set";
		EXPECT_NE( universalIdSet.find( universalId4 ), universalIdSet.end() ) << "UniversalId 4 should be in set";
	}

	//----------------------------------------------
	// Hash validation
	//----------------------------------------------

	TEST_F( UniversalIdHashTests, Test_Hash_StdHash_Consistency )
	{
		auto imoNumbers = imoNumbersFromFile( 1 );
		auto universalId = createUniversalId( imoNumbers[0], "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );

		// Test std::hash specialization
		std::hash<UniversalId> stdHasher;
		auto stdHash = stdHasher( universalId );
		auto memberHash = universalId.hashCode();

		EXPECT_EQ( stdHash, memberHash ) << "std::hash specialization should match member hashCode()";
	}

	//----------------------------------------------
	// Component hash relationship tests
	//----------------------------------------------

	TEST_F( UniversalIdHashTests, Test_Hash_Component_Relationship )
	{
		// Test that UniversalId hash is properly derived from its components using IMO file data
		auto imoNumbers = imoNumbersFromFile( 3 );
		auto universalId1 = createUniversalId( imoNumbers[0], "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto universalId2 = createUniversalId( imoNumbers[1], "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );   // Different IMO
		auto universalId3 = createUniversalId( imoNumbers[2], "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" ); // Different LocalId

		auto hash1 = universalId1.hashCode();
		auto hash2 = universalId2.hashCode();
		auto hash3 = universalId3.hashCode();

		// UniversalIds with different components should have different hashes
		EXPECT_NE( hash1, hash2 ) << "Different IMO numbers should result in different UniversalId hashes";
		EXPECT_NE( hash1, hash3 ) << "Different LocalIds should result in different UniversalId hashes";
		EXPECT_NE( hash2, hash3 ) << "Different combinations should result in different UniversalId hashes";

		// Get component hashes
		auto imo1Hash = universalId1.imoNumber().hashCode();
		auto imo2Hash = universalId2.imoNumber().hashCode();
		auto localId1Hash = universalId1.localId().hashCode();
		auto localId3Hash = universalId3.localId().hashCode();

		// Component hashes should be different when components are different
		EXPECT_NE( imo1Hash, imo2Hash ) << "Different IMO numbers should have different hashes";
		EXPECT_NE( localId1Hash, localId3Hash ) << "Different LocalIds should have different hashes";

		// UniversalId hash should not equal component hashes (proper composition)
		EXPECT_NE( hash1, imo1Hash ) << "UniversalId hash should not equal IMO hash";
		EXPECT_NE( hash1, localId1Hash ) << "UniversalId hash should not equal LocalId hash";
	}
} // namespace dnv::vista::sdk::tests
