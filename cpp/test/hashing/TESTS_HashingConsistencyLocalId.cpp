/**
 * @file TESTS_HashingConsistencyLocalid.cpp
 * @brief Hash consistency validation suite for LocalId class
 * @details This test suite validates hash function behavior, distribution quality, and consistency
 *          across all LocalId operations including copy/move semantics and STL container.
 */

#include <chrono>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <gtest/gtest.h>

#include <dnv/vista/sdk/LocalId.h>
#include <dnv/vista/sdk/LocalIdBuilder.h>
#include <dnv/vista/sdk/ParsingErrors.h>
#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/VISVersion.h>

namespace dnv::vista::sdk::tests
{
	//========================================================================
	// LocalId hashing consistency, uniqueness and validation tests
	//========================================================================

	//----------------------------------------------
	// Dataset
	//----------------------------------------------

	namespace
	{
		std::vector<std::string> localIdUniqueDataPaths()
		{
			std::vector<std::string> paths;
#ifdef VISTA_SDK_TESTDATA_DIR
			std::string baseDir = std::string{ VISTA_SDK_TESTDATA_DIR };
			paths.push_back( baseDir + "/LocalIds_unique_3-4a.txt" );
			paths.push_back( baseDir + "/LocalIds_unique_3-6a.txt" );
			paths.push_back( baseDir + "/LocalIds_unique_3-7a.txt" );
#else
			paths.push_back( "testdata/LocalIds_unique_3-4a.txt" );
			paths.push_back( "testdata/LocalIds_unique_3-6a.txt" );
			paths.push_back( "testdata/LocalIds_unique_3-7a.txt" );
#endif
			return paths;
		}
	} // namespace

	//----------------------------------------------
	// Test fixture
	//----------------------------------------------

	class LocalIdHashTests : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			testLocalIds = {
				"/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage",
				"/dnv-v2/vis-3-4a/1031/meta/cnt-refrigerant/state-leaking",
				"/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
				"/dnv-v2/vis-3-4a/411.1/C101.63/S206/meta/qty-temperature/cnt-exhaust.gas",
				"/dnv-v2/vis-3-4a/411.1/C101.63/S206/sec/411.1/C101.31-5/~propulsion.engine/~cooling.system/~for.propulsion.engine/~cylinder.5/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
				"/dnv-v2/vis-3-4a/411.1/C101.63/S206/~propulsion.engine/~cooling.system/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
				"/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low",
				"/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" };
		}

		std::vector<std::string> testLocalIds;
	};

	//----------------------------------------------
	// Hash uniqueness validation
	//----------------------------------------------

	TEST_F( LocalIdHashTests, Test_Hash_Uniqueness_DifferentLocalIds )
	{
		auto localId1 = LocalId::parse( "/dnv-v2/vis-3-4a/411.1/C101.63/S206/~propulsion.engine/~cooling.system/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto localId2 = LocalId::parse( "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );
		auto localId3 = LocalId::parse( "/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low" );
		auto localId4 = LocalId::parse( "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" );

		auto hash1 = localId1.hashCode();
		auto hash2 = localId2.hashCode();
		auto hash3 = localId3.hashCode();
		auto hash4 = localId4.hashCode();

		EXPECT_NE( hash1, hash2 ) << "Different LocalIds must have distinct hashes";
		EXPECT_NE( hash1, hash3 ) << "Different LocalIds must have distinct hashes";
		EXPECT_NE( hash1, hash4 ) << "Different LocalIds must have distinct hashes";
		EXPECT_NE( hash2, hash3 ) << "Different LocalIds must have distinct hashes";
		EXPECT_NE( hash2, hash4 ) << "Different LocalIds must have distinct hashes";
		EXPECT_NE( hash3, hash4 ) << "Different LocalIds must have distinct hashes";
	}

	TEST_F( LocalIdHashTests, Test_Hash_Uniqueness_Smoke )
	{
		std::unordered_set<std::size_t> uniqueHashes;
		std::vector<LocalId> allLocalIds;

		std::vector<std::string> localIdStrings;
		auto datasetPaths = localIdUniqueDataPaths();

		for ( const auto& datasetPath : datasetPaths )
		{
			std::ifstream datasetFile( datasetPath );
			if ( datasetFile.is_open() )
			{
				std::string line;
				while ( std::getline( datasetFile, line ) )
				{
					if ( !line.empty() )
					{
						localIdStrings.push_back( line );
					}
				}
				datasetFile.close();
			}
		}

		for ( const auto& localIdStr : localIdStrings )
		{
			try
			{
				auto localId = LocalId::parse( localIdStr );
				if ( !localId.isVerboseMode() )
				{
					allLocalIds.push_back( localId );
				}
			}
			catch ( ... )
			{
			}
		}

		// Test hash uniqueness across entire dataset
		size_t collisionCount = 0;
		std::unordered_map<std::size_t, std::vector<std::string>> hashCollisionMap;

		for ( const auto& localId : allLocalIds )
		{
			std::size_t localIdHash = localId.hashCode();

			if ( uniqueHashes.find( localIdHash ) != uniqueHashes.end() )
			{
				collisionCount++;
				hashCollisionMap[localIdHash].push_back( localId.toString() );
			}
			else
			{
				uniqueHashes.insert( localIdHash );
				hashCollisionMap[localIdHash].push_back( localId.toString() );
			}
		}

		// Analyze uniqueness per VIS version and collision types
		std::unordered_map<std::string, size_t> localIdsByVersion;
		std::unordered_map<std::string, std::unordered_set<std::size_t>> hashesByVersion;
		size_t trueCollisions = 0;
		size_t visVersionCollisions = 0;

		// Count LocalIds and unique hashes per VIS version
		for ( const auto& localId : allLocalIds )
		{
			std::string localIdStr = localId.toString();
			std::string visVersion = "unknown";

			// Extract VIS version
			auto visPos = localIdStr.find( "/vis-3-" );
			if ( visPos != std::string::npos )
			{
				auto afterVis = visPos + 1; // Start from 'v' in "vis-3-"
				auto pathStart = localIdStr.find( '/', afterVis );
				if ( pathStart != std::string::npos )
				{
					visVersion = localIdStr.substr( afterVis, pathStart - afterVis );
				}
			}

			localIdsByVersion[visVersion]++;
			hashesByVersion[visVersion].insert( localId.hashCode() );
		}

		// Analyze collision types
		for ( const auto& pair : hashCollisionMap )
		{
			if ( pair.second.size() > 1 )
			{
				// Check if collisions are between different VIS versions of the same LocalId
				bool isVisVersionCollision = true;
				std::string baseContent;
				std::set<std::string> versionsInCollision;

				for ( const auto& localIdStr : pair.second )
				{
					// Extract VIS version for collision analysis
					std::string currentVersion = "unknown";
					auto visPos = localIdStr.find( "/vis-3-" );
					if ( visPos != std::string::npos )
					{
						auto afterVis = visPos + 1;
						auto pathStart = localIdStr.find( '/', afterVis );
						if ( pathStart != std::string::npos )
						{
							currentVersion = localIdStr.substr( afterVis, pathStart - afterVis );
						}
					}
					versionsInCollision.insert( currentVersion );

					// Extract content after VIS version
					std::string content = localIdStr;
					if ( visPos != std::string::npos )
					{
						auto pathStart = localIdStr.find( '/', visPos + 1 );
						if ( pathStart != std::string::npos )
						{
							content = localIdStr.substr( pathStart );
						}
					}

					if ( baseContent.empty() )
					{
						baseContent = content;
					}
					else if ( baseContent != content )
					{
						isVisVersionCollision = false;
						break;
					}
				}

				if ( isVisVersionCollision && versionsInCollision.size() > 1 )
				{
					visVersionCollisions += pair.second.size() - 1;
				}
				else if ( !isVisVersionCollision )
				{
					trueCollisions += pair.second.size() - 1;
				}
			}
		}

		std::cout << "\nLocalId Hash Uniqueness Statistics:\n";
		std::cout << "  Total LocalIds loaded: " << allLocalIds.size() << "\n";
		std::cout << "  Valid parsed LocalIds: " << allLocalIds.size() << "\n";
		std::cout << "  Unique hashes: " << uniqueHashes.size() << "\n";
		std::cout << "  Hash collisions: " << collisionCount << "\n";
		std::cout << "  True hash collisions (unexpected): " << trueCollisions << "\n";
		std::cout << "  VIS version collisions (expected): " << visVersionCollisions << "\n";

		std::cout << "\nUniqueness Per VIS Version:\n";
		for ( const auto& versionPair : localIdsByVersion )
		{
			const std::string& version = versionPair.first;
			size_t totalForVersion = versionPair.second;
			size_t uniqueHashesForVersion = hashesByVersion[version].size();
			double versionUniqueness = static_cast<double>( uniqueHashesForVersion ) / static_cast<double>( totalForVersion );

			std::cout << "  " << version << ": " << uniqueHashesForVersion << "/" << totalForVersion
					  << " (" << ( versionUniqueness * 100.0 ) << "% unique)\n";

			// Each VIS version should have 100% hash uniqueness (no intra-version collisions)
			EXPECT_EQ( uniqueHashesForVersion, totalForVersion )
				<< "VIS version " << version << " should have perfect hash uniqueness within the version. "
				<< "Expected " << totalForVersion << " unique hashes, got " << uniqueHashesForVersion;
		}

		EXPECT_EQ( trueCollisions, 0 )
			<< "Hash function should produce zero true collisions. "
			<< "Detected " << trueCollisions << " true collisions out of " << allLocalIds.size() << " LocalIds";

		EXPECT_GT( visVersionCollisions, 0 )
			<< "Expected VIS version collisions demonstrate hash consistency across versions. "
			<< "Got " << visVersionCollisions << " cross-version collisions (this is correct behavior)";

		// VIS version collisions are expected - the number of unique hashes should equal
		// total LocalIds minus the expected VIS version collisions
		size_t expectedUniqueHashes = allLocalIds.size() - visVersionCollisions;
		EXPECT_EQ( uniqueHashes.size(), expectedUniqueHashes )
			<< "Unique hash count should account for expected VIS version collisions. "
			<< "Expected " << expectedUniqueHashes << " unique hashes, got " << uniqueHashes.size();
	}

	//----------------------------------------------
	// Hash consistency and immutability tests
	//----------------------------------------------

	TEST_F( LocalIdHashTests, Test_Hash_Consistency_CopyConstructor )
	{
		auto original = LocalId::parse( "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );

		LocalId copy( original );
		auto originalHash = original.hashCode();
		auto copyHash = copy.hashCode();

		EXPECT_EQ( originalHash, copyHash ) << "Copy constructor should preserve hash code";
		EXPECT_EQ( original, copy ) << "Copy should be equal to original";
	}

	TEST_F( LocalIdHashTests, Test_Hash_Consistency_MoveConstructor )
	{
		auto original = LocalId::parse( "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );
		auto originalHash = original.hashCode();

		LocalId moved( std::move( original ) );
		auto movedHash = moved.hashCode();

		EXPECT_EQ( originalHash, movedHash ) << "Move constructor should preserve hash code";
	}

	TEST_F( LocalIdHashTests, Test_Hash_Consistency_CopyAssignment )
	{
		auto original = LocalId::parse( "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );
		auto target = LocalId::parse( "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" ); // Different LocalId initially
		auto originalHash = original.hashCode();

		target = original;
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Copy assignment should preserve hash code";
		EXPECT_EQ( original, target ) << "Copy assignment should make LocalIds equal";
	}

	TEST_F( LocalIdHashTests, Test_Hash_Consistency_MoveAssignment )
	{
		auto original = LocalId::parse( "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );
		auto target = LocalId::parse( "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" );
		auto originalHash = original.hashCode();

		target = std::move( original );
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Move assignment should preserve hash code";
	}

	TEST_F( LocalIdHashTests, Test_Hash_Consistency_RepeatAccess )
	{
		auto localId = LocalId::parse( "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" );

		auto hash1 = localId.hashCode();
		auto hash2 = localId.hashCode();
		auto hash3 = localId.hashCode();

		EXPECT_EQ( hash1, hash2 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash1, hash3 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash2, hash3 ) << "Multiple hash access should be consistent";
	}

	//----------------------------------------------
	// Parse methods
	//----------------------------------------------

	TEST_F( LocalIdHashTests, Test_Hash_Consistency_AllParseMethods )
	{
		for ( const auto& localIdStr : testLocalIds )
		{
			std::vector<std::size_t> allHashes;
			std::vector<std::string> methodNames;
			std::vector<LocalId> allLocalIds; // Store actual LocalId objects for comparison

			// Method 1: parse(string_view)
			try
			{
				auto localId1 = LocalId::parse( localIdStr );
				allHashes.push_back( localId1.hashCode() );
				methodNames.push_back( "parse(string_view)" );
				allLocalIds.push_back( localId1 );
			}
			catch ( const std::exception& )
			{
			}

			// Method 2: tryParse(string_view, ParsingErrors&, optional<LocalId>&)
			{
				ParsingErrors errors;
				std::optional<LocalId> localId2;
				if ( LocalId::tryParse( localIdStr, errors, localId2 ) && localId2.has_value() )
				{
					allHashes.push_back( localId2->hashCode() );
					methodNames.push_back( "tryParse(string_view, ParsingErrors&, optional<LocalId>&)" );
					allLocalIds.push_back( *localId2 );
				}
			}

			// Method 3: tryParse(string_view, optional<LocalId>&)
			{
				std::optional<LocalId> localId3;
				if ( LocalId::tryParse( localIdStr, localId3 ) && localId3.has_value() )
				{
					allHashes.push_back( localId3->hashCode() );
					methodNames.push_back( "tryParse(string_view, optional<LocalId>&)" );
					allLocalIds.push_back( *localId3 );
				}
			}

			// Verify all successful parsing methods produced the same hash
			std::size_t referenceHash = allHashes[0];
			std::string referenceName = methodNames[0];

			for ( size_t i = 1; i < allHashes.size(); ++i )
			{
				EXPECT_EQ( referenceHash, allHashes[i] )
					<< "Hash mismatch for LocalId: '" << localIdStr << "'\n"
					<< "  Reference method: " << referenceName << " (hash: " << referenceHash << ")\n"
					<< "  Compared method:  " << methodNames[i] << " (hash: " << allHashes[i] << ")";

				// Also verify the actual LocalId objects are equal
				EXPECT_EQ( allLocalIds[0], allLocalIds[i] )
					<< "LocalId objects should be equal for input: '" << localIdStr << "'\n"
					<< "  Reference method: " << referenceName << " (toString: " << allLocalIds[0].toString() << ")\n"
					<< "  Compared method:  " << methodNames[i] << " (toString: " << allLocalIds[i].toString() << ")";
			}

			// Verify consistent structure across all parsing methods
			const auto& reference = allLocalIds[0];
			for ( size_t i = 1; i < allLocalIds.size(); ++i )
			{
				const auto& compared = allLocalIds[i];

				EXPECT_EQ( reference.visVersion(), compared.visVersion() )
					<< "VIS version should be consistent across parsing methods for: '" << localIdStr << "'";

				EXPECT_EQ( reference.isVerboseMode(), compared.isVerboseMode() )
					<< "Verbose mode should be consistent across parsing methods for: '" << localIdStr << "'";

				// Check metadata consistency
				EXPECT_EQ( reference.quantity().has_value(), compared.quantity().has_value() )
					<< "Quantity metadata presence should be consistent for: '" << localIdStr << "'";

				if ( reference.quantity().has_value() && compared.quantity().has_value() )
				{
					EXPECT_EQ( reference.quantity()->toString(), compared.quantity()->toString() )
						<< "Quantity metadata should be identical for: '" << localIdStr << "'";
				}

				EXPECT_EQ( reference.content().has_value(), compared.content().has_value() )
					<< "Content metadata presence should be consistent for: '" << localIdStr << "'";

				if ( reference.content().has_value() && compared.content().has_value() )
				{
					EXPECT_EQ( reference.content()->toString(), compared.content()->toString() )
						<< "Content metadata should be identical for: '" << localIdStr << "'";
				}
			}
		}
	}

	TEST_F( LocalIdHashTests, Test_Hash_Consistency_ParsingVsConstruction )
	{
		for ( const auto& localIdStr : testLocalIds )
		{
			// Parse LocalId using standard parse method
			auto parsedLocalId = LocalId::parse( localIdStr );
			auto parsedHash = parsedLocalId.hashCode();

			// Verify toString() consistency
			auto reconstructedStr = parsedLocalId.toString();

			// Parse the reconstructed string and verify hash consistency
			try
			{
				auto reparsedLocalId = LocalId::parse( reconstructedStr );
				auto reparsedHash = reparsedLocalId.hashCode();

				EXPECT_EQ( parsedHash, reparsedHash )
					<< "Hash should be consistent for round-trip parsing:\n"
					<< "  Original: '" << localIdStr << "' (hash: " << parsedHash << ")\n"
					<< "  toString(): '" << reconstructedStr << "'\n"
					<< "  Reparsed hash: " << reparsedHash;

				EXPECT_EQ( parsedLocalId, reparsedLocalId )
					<< "LocalId objects should be equal for round-trip parsing:\n"
					<< "  Original: '" << localIdStr << "'\n"
					<< "  toString(): '" << reconstructedStr << "'";

				// Verify structural consistency
				EXPECT_EQ( parsedLocalId.visVersion(), reparsedLocalId.visVersion() )
					<< "VIS version should be consistent for round-trip parsing";

				EXPECT_EQ( parsedLocalId.metadataTags().size(), reparsedLocalId.metadataTags().size() )
					<< "Metadata tag count should be consistent for round-trip parsing";
			}
			catch ( const std::exception& ex )
			{
				// If reparsing fails, it might indicate toString() format issues
				FAIL() << "Failed to reparse toString() output for LocalId: '" << localIdStr << "'\n"
					   << "  toString() result: '" << reconstructedStr << "'\n"
					   << "  Reparse error: " << ex.what();
			}
		}
	}

	//----------------------------------------------
	// STL container integration tests
	//----------------------------------------------

	TEST_F( LocalIdHashTests, Test_StdHash_UnorderedMap_Integration )
	{
		std::unordered_map<LocalId, std::string> localIdMap;
		std::vector<LocalId> testLocalIdObjects;

		// Parse test LocalIds
		for ( const auto& localIdStr : testLocalIds )
		{
			try
			{
				testLocalIdObjects.push_back( LocalId::parse( localIdStr ) );
			}
			catch ( ... )
			{
			}
		}

		// Test insertion and lookup with LocalIds
		for ( size_t i = 0; i < testLocalIdObjects.size(); ++i )
		{
			std::string description = "Sensor_" + std::to_string( i );
			localIdMap[testLocalIdObjects[i]] = description;
		}

		EXPECT_EQ( localIdMap.size(), testLocalIdObjects.size() ) << "STL map should contain all unique LocalIds";

		// Test lookup functionality
		size_t successfulLookups = 0;
		for ( const auto& localId : testLocalIdObjects )
		{
			auto it = localIdMap.find( localId );
			if ( it != localIdMap.end() )
			{
				successfulLookups++;
				EXPECT_FALSE( it->second.empty() ) << "Lookup should return valid description";
			}
		}

		EXPECT_EQ( successfulLookups, testLocalIdObjects.size() ) << "All lookups should succeed";
	}

	TEST_F( LocalIdHashTests, Test_StdHash_Specialization_UnorderedSet )
	{
		std::unordered_set<LocalId> localIdSet;

		auto localId1 = LocalId::parse( "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );
		auto localId2 = LocalId::parse( "/dnv-v2/vis-3-4a/411.1/C101.63/S206/~propulsion.engine/~cooling.system/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto localId3 = LocalId::parse( "/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low" );
		auto localId4 = LocalId::parse( "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" );

		localIdSet.insert( localId1 );
		localIdSet.insert( localId2 );
		localIdSet.insert( localId3 );
		localIdSet.insert( localId4 );

		localIdSet.insert( localId1 ); // Duplicate
		localIdSet.insert( localId2 ); // Duplicate

		EXPECT_EQ( localIdSet.size(), 4 ) << "Set should contain exactly 4 unique LocalIds";

		EXPECT_NE( localIdSet.find( localId1 ), localIdSet.end() ) << "LocalId 1 should be in set";
		EXPECT_NE( localIdSet.find( localId2 ), localIdSet.end() ) << "LocalId 2 should be in set";
		EXPECT_NE( localIdSet.find( localId3 ), localIdSet.end() ) << "LocalId 3 should be in set";
		EXPECT_NE( localIdSet.find( localId4 ), localIdSet.end() ) << "LocalId 4 should be in set";
	}

	//----------------------------------------------
	// Hash validation
	//----------------------------------------------

	TEST_F( LocalIdHashTests, Test_Hash_StdHash_Consistency )
	{
		auto localId = LocalId::parse( "/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low" );

		// Test std::hash specialization
		std::hash<LocalId> stdHasher;
		auto stdHash = stdHasher( localId );
		auto memberHash = localId.hashCode();

		EXPECT_EQ( stdHash, memberHash ) << "std::hash specialization should match member hashCode()";
	}
} // namespace dnv::vista::sdk::tests
