/**
 * @file TESTS_HashingConsistencyLocalIdBuilder.cpp
 * @brief Hash consistency validation suite for LocalIdBuilder class
 * @details This test suite validates hash function behavior, distribution quality, and consistency
 *          across all LocalIdBuilder operations including copy/move semantics, fluent API operations,
 *          parsing methods, and STL container integration with validation.
 */

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
	// LocalIdBuilder hashing consistency, uniqueness and validation tests
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

	namespace
	{
		std::vector<std::string> localIdBuilderTestData()
		{
			return {
				"/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage",
				"/dnv-v2/vis-3-4a/1031/meta/cnt-refrigerant/state-leaking",
				"/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
				"/dnv-v2/vis-3-4a/411.1/C101.63/S206/meta/qty-temperature/cnt-exhaust.gas",
				"/dnv-v2/vis-3-4a/411.1/C101.63/S206/sec/411.1/C101.31-5/~propulsion.engine/~cooling.system/~for.propulsion.engine/~cylinder.5/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
				"/dnv-v2/vis-3-4a/411.1/C101.63/S206/~propulsion.engine/~cooling.system/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
				"/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low",
				"/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" };
		}
	} // namespace

	//----------------------------------------------
	// Test fixture
	//----------------------------------------------

	class LocalIdBuilderHashTests : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			testLocalIdStrings = localIdBuilderTestData();
		}

		std::vector<std::string> testLocalIdStrings;
	};

	//----------------------------------------------
	// Hash uniqueness validation
	//----------------------------------------------

	TEST_F( LocalIdBuilderHashTests, Test_Hash_Uniqueness_DifferentBuilders )
	{
		// Create different LocalIdBuilder instances and verify unique hashes
		auto builder1 = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/411.1/C101.63/S206/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto builder2 = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );
		auto builder3 = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low" );
		auto builder4 = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" );

		auto hash1 = builder1.hashCode();
		auto hash2 = builder2.hashCode();
		auto hash3 = builder3.hashCode();
		auto hash4 = builder4.hashCode();

		EXPECT_NE( hash1, hash2 ) << "Different LocalIdBuilders must have distinct hashes";
		EXPECT_NE( hash1, hash3 ) << "Different LocalIdBuilders must have distinct hashes";
		EXPECT_NE( hash1, hash4 ) << "Different LocalIdBuilders must have distinct hashes";
		EXPECT_NE( hash2, hash3 ) << "Different LocalIdBuilders must have distinct hashes";
		EXPECT_NE( hash2, hash4 ) << "Different LocalIdBuilders must have distinct hashes";
		EXPECT_NE( hash3, hash4 ) << "Different LocalIdBuilders must have distinct hashes";
	}

	TEST_F( LocalIdBuilderHashTests, Test_Hash_Uniqueness_BuilderVsLocalId )
	{
		// Test that LocalIdBuilder and corresponding LocalId have the same hash
		const std::string testLocalIdStr = "/dnv-v2/vis-3-4a/411.1/C101.63/S206/meta/qty-temperature/cnt-exhaust.gas/pos-inlet";

		auto builder = LocalIdBuilder::parse( testLocalIdStr );
		auto localId = builder.build();

		auto builderHash = builder.hashCode();
		auto localIdHash = localId.hashCode();

		EXPECT_EQ( builderHash, localIdHash ) << "LocalIdBuilder and corresponding LocalId should have identical hashes";
		EXPECT_EQ( builder.toString(), localId.toString() ) << "LocalIdBuilder and LocalId should have identical string representations";
	}

	TEST_F( LocalIdBuilderHashTests, Test_Hash_Uniqueness_Smoke )
	{
		std::unordered_set<std::size_t> uniqueHashes;
		std::vector<LocalIdBuilder> allBuilders;

		std::vector<std::string> localIdStrings;
		auto datasetPaths = localIdUniqueDataPaths();

		// Load LocalId strings from all dataset files
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

		// Parse and validate all LocalIdBuilder entries
		for ( const auto& localIdStr : localIdStrings )
		{
			try
			{
				auto builder = LocalIdBuilder::parse( localIdStr );
				if ( !builder.isVerboseMode() )
				{
					allBuilders.push_back( builder );
				}
			}
			catch ( ... )
			{
			}
		}

		// Test hash uniqueness across entire dataset
		size_t collisionCount = 0;
		std::unordered_map<std::size_t, std::vector<std::string>> hashCollisionMap;

		for ( const auto& builder : allBuilders )
		{
			std::size_t builderHash = builder.hashCode();

			if ( uniqueHashes.find( builderHash ) != uniqueHashes.end() )
			{
				collisionCount++;
				hashCollisionMap[builderHash].push_back( builder.toString() );
			}
			else
			{
				uniqueHashes.insert( builderHash );
				hashCollisionMap[builderHash].push_back( builder.toString() );
			}
		}

		// Analyze uniqueness per VIS version and collision types
		std::unordered_map<std::string, size_t> buildersByVersion;
		std::unordered_map<std::string, std::unordered_set<std::size_t>> hashesByVersion;
		size_t trueCollisions = 0;
		size_t visVersionCollisions = 0;

		// Count LocalIdBuilders and unique hashes per VIS version
		for ( const auto& builder : allBuilders )
		{
			std::string builderStr = builder.toString();
			std::string visVersion = "unknown";

			// Extract VIS version
			auto visPos = builderStr.find( "/vis-3-" );
			if ( visPos != std::string::npos )
			{
				auto afterVis = visPos + 1; // Start from 'v' in "vis-3-"
				auto pathStart = builderStr.find( '/', afterVis );
				if ( pathStart != std::string::npos )
				{
					visVersion = builderStr.substr( afterVis, pathStart - afterVis );
				}
			}

			buildersByVersion[visVersion]++;
			hashesByVersion[visVersion].insert( builder.hashCode() );
		}

		// Analyze collision types
		for ( const auto& pair : hashCollisionMap )
		{
			if ( pair.second.size() > 1 )
			{
				// Check if collisions are between different VIS versions of the same LocalIdBuilder
				bool isVisVersionCollision = true;
				std::string baseContent;
				std::set<std::string> versionsInCollision;

				for ( const auto& builderStr : pair.second )
				{
					// Extract VIS version for collision analysis
					std::string currentVersion = "unknown";
					auto visPos = builderStr.find( "/vis-3-" );
					if ( visPos != std::string::npos )
					{
						auto afterVis = visPos + 1;
						auto pathStart = builderStr.find( '/', afterVis );
						if ( pathStart != std::string::npos )
						{
							currentVersion = builderStr.substr( afterVis, pathStart - afterVis );
						}
					}
					versionsInCollision.insert( currentVersion );

					// Extract content after VIS version
					std::string content = builderStr;
					if ( visPos != std::string::npos )
					{
						auto pathStart = builderStr.find( '/', visPos + 1 );
						if ( pathStart != std::string::npos )
						{
							content = builderStr.substr( pathStart );
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

		std::cout << "\nLocalIdBuilder Hash Uniqueness Statistics:\n";
		std::cout << "  Total LocalIdBuilders loaded: " << allBuilders.size() << "\n";
		std::cout << "  Valid parsed LocalIdBuilders: " << allBuilders.size() << "\n";
		std::cout << "  Unique hashes: " << uniqueHashes.size() << "\n";
		std::cout << "  Hash collisions: " << collisionCount << "\n";
		std::cout << "  True hash collisions (unexpected): " << trueCollisions << "\n";
		std::cout << "  VIS version collisions (expected): " << visVersionCollisions << "\n";

		std::cout << "\nUniqueness Per VIS Version:\n";
		for ( const auto& versionPair : buildersByVersion )
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
			<< "Detected " << trueCollisions << " true collisions out of " << allBuilders.size() << " LocalIdBuilders";

		EXPECT_GT( visVersionCollisions, 0 )
			<< "Expected VIS version collisions demonstrate hash consistency across versions. "
			<< "Got " << visVersionCollisions << " cross-version collisions (this is correct behavior)";

		// VIS version collisions are expected - the number of unique hashes should equal
		// total LocalIdBuilders minus the expected VIS version collisions
		size_t expectedUniqueHashes = allBuilders.size() - visVersionCollisions;
		EXPECT_EQ( uniqueHashes.size(), expectedUniqueHashes )
			<< "Unique hash count should account for expected VIS version collisions. "
			<< "Expected " << expectedUniqueHashes << " unique hashes, got " << uniqueHashes.size();
	}

	//----------------------------------------------
	// Hash consistency and immutability tests
	//----------------------------------------------

	TEST_F( LocalIdBuilderHashTests, Test_Hash_Consistency_CopyConstructor )
	{
		auto original = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );

		LocalIdBuilder copy( original );
		auto originalHash = original.hashCode();
		auto copyHash = copy.hashCode();

		EXPECT_EQ( originalHash, copyHash ) << "Copy constructor should preserve hash code";
		EXPECT_EQ( original, copy ) << "Copy should be equal to original";
	}

	TEST_F( LocalIdBuilderHashTests, Test_Hash_Consistency_MoveConstructor )
	{
		auto original = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );
		auto originalHash = original.hashCode();

		LocalIdBuilder moved( std::move( original ) );
		auto movedHash = moved.hashCode();

		EXPECT_EQ( originalHash, movedHash ) << "Move constructor should preserve hash code";
	}

	TEST_F( LocalIdBuilderHashTests, Test_Hash_Consistency_CopyAssignment )
	{
		auto original = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );
		auto target = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" ); // Different LocalIdBuilder initially
		auto originalHash = original.hashCode();

		target = original;
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Copy assignment should preserve hash code";
		EXPECT_EQ( original, target ) << "Copy assignment should make LocalIdBuilders equal";
	}

	TEST_F( LocalIdBuilderHashTests, Test_Hash_Consistency_MoveAssignment )
	{
		auto original = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );
		auto target = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" );
		auto originalHash = original.hashCode();

		target = std::move( original );
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Move assignment should preserve hash code";
	}

	TEST_F( LocalIdBuilderHashTests, Test_Hash_Consistency_RepeatAccess )
	{
		auto builder = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" );

		auto hash1 = builder.hashCode();
		auto hash2 = builder.hashCode();
		auto hash3 = builder.hashCode();

		EXPECT_EQ( hash1, hash2 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash1, hash3 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash2, hash3 ) << "Multiple hash access should be consistent";
	}

	//----------------------------------------------
	// Parse methods and construction consistency tests
	//----------------------------------------------

	TEST_F( LocalIdBuilderHashTests, Test_Hash_Consistency_AllParseMethods )
	{
		// Test LocalIdBuilder construction via different parsing methods
		for ( const auto& localIdStr : testLocalIdStrings )
		{
			std::vector<std::size_t> allHashes;
			std::vector<std::string> methodNames;
			std::vector<LocalIdBuilder> allBuilders; // Store actual LocalIdBuilder objects for comparison

			// Method 1: parse(string_view)
			try
			{
				auto builder1 = LocalIdBuilder::parse( localIdStr );
				allHashes.push_back( builder1.hashCode() );
				methodNames.push_back( "parse(string_view)" );
				allBuilders.push_back( builder1 );
			}
			catch ( const std::exception& )
			{
			}

			// Method 2: tryParse(string_view, ParsingErrors&, optional<LocalIdBuilder>&)
			{
				ParsingErrors errors;
				std::optional<LocalIdBuilder> builder2;
				if ( LocalIdBuilder::tryParse( localIdStr, errors, builder2 ) && builder2.has_value() )
				{
					allHashes.push_back( builder2->hashCode() );
					methodNames.push_back( "tryParse(string_view, ParsingErrors&, optional<LocalIdBuilder>&)" );
					allBuilders.push_back( *builder2 );
				}
			}

			// Method 3: tryParse(string_view, optional<LocalIdBuilder>&)
			{
				std::optional<LocalIdBuilder> builder3;
				if ( LocalIdBuilder::tryParse( localIdStr, builder3 ) && builder3.has_value() )
				{
					allHashes.push_back( builder3->hashCode() );
					methodNames.push_back( "tryParse(string_view, optional<LocalIdBuilder>&)" );
					allBuilders.push_back( *builder3 );
				}
			}

			// Verify all successful parsing methods produced the same hash
			std::size_t referenceHash = allHashes[0];
			std::string referenceName = methodNames[0];

			for ( size_t i = 1; i < allHashes.size(); ++i )
			{
				EXPECT_EQ( referenceHash, allHashes[i] )
					<< "Hash mismatch for LocalIdBuilder: '" << localIdStr << "'\n"
					<< "  Reference method: " << referenceName << " (hash: " << referenceHash << ")\n"
					<< "  Compared method:  " << methodNames[i] << " (hash: " << allHashes[i] << ")";

				// Also verify the actual LocalIdBuilder objects are equal
				EXPECT_EQ( allBuilders[0], allBuilders[i] )
					<< "LocalIdBuilder objects should be equal for input: '" << localIdStr << "'\n"
					<< "  Reference method: " << referenceName << " (toString: " << allBuilders[0].toString() << ")\n"
					<< "  Compared method:  " << methodNames[i] << " (toString: " << allBuilders[i].toString() << ")";
			}

			// Verify consistent structure across all parsing methods
			const auto& reference = allBuilders[0];
			for ( size_t i = 1; i < allBuilders.size(); ++i )
			{
				const auto& compared = allBuilders[i];

				EXPECT_EQ( reference.visVersion(), compared.visVersion() )
					<< "VIS version should be consistent across parsing methods for: '" << localIdStr << "'";

				EXPECT_EQ( reference.isVerboseMode(), compared.isVerboseMode() )
					<< "Verbose mode should be consistent across parsing methods for: '" << localIdStr << "'";

				EXPECT_EQ( reference.isValid(), compared.isValid() )
					<< "Validation status should be consistent across parsing methods for: '" << localIdStr << "'";
			}
		}
	}

	TEST_F( LocalIdBuilderHashTests, Test_Hash_Consistency_BuilderVsConstruction )
	{
		// Test LocalIdBuilder construction from fluent API vs parsing
		const auto& vis = VIS::instance();

		struct TestCase
		{
			std::string primaryItemStr;
			std::string secondaryItemStr = "";
			std::string quantityStr = "";
			std::string contentStr = "";
			std::string positionStr = "";
			std::string stateStr = "";
			VisVersion visVersion = VisVersion::v3_4a;
			bool verbose = false;
		};

		const std::vector<TestCase> testCases = {
			{ "411.1/C101.31-2", "", "temperature", "exhaust.gas", "inlet" },
			{ "411.1/C101.63/S206", "", "temperature", "exhaust.gas", "inlet", "", VisVersion::v3_4a, true },
			{ "652.31/S90.3/S61", "652.1i-1P", "", "sea.water", "", "opened" },
			{ "1021.1i-6P/H123", "", "volume", "cargo", "~percentage" } };

		for ( const auto& testCase : testCases )
		{
			const auto& gmod = vis.gmod( testCase.visVersion );
			const auto& codebooks = vis.codebooks( testCase.visVersion );

			// Method 1: Construct using fluent API
			auto primaryItem = gmod.parsePath( testCase.primaryItemStr );
			std::optional<GmodPath> secondaryItem;
			if ( !testCase.secondaryItemStr.empty() )
			{
				secondaryItem = gmod.parsePath( testCase.secondaryItemStr );
			}

			auto constructedBuilder = LocalIdBuilder::create( testCase.visVersion )
										  .withPrimaryItem( std::move( primaryItem ) )
										  .tryWithSecondaryItem( std::move( secondaryItem ) )
										  .withVerboseMode( testCase.verbose )
										  .tryWithMetadataTag( codebooks.tryCreateTag( CodebookName::Quantity, testCase.quantityStr ) )
										  .tryWithMetadataTag( codebooks.tryCreateTag( CodebookName::Content, testCase.contentStr ) )
										  .tryWithMetadataTag( codebooks.tryCreateTag( CodebookName::Position, testCase.positionStr ) )
										  .tryWithMetadataTag( codebooks.tryCreateTag( CodebookName::State, testCase.stateStr ) );

			// Method 2: Parse the string representation
			auto constructedStr = constructedBuilder.toString();
			try
			{
				auto parsedBuilder = LocalIdBuilder::parse( constructedStr );

				auto constructedHash = constructedBuilder.hashCode();
				auto parsedHash = parsedBuilder.hashCode();

				EXPECT_EQ( constructedHash, parsedHash )
					<< "Hash should be consistent between construction and parsing:\n"
					<< "  Constructed (hash: " << constructedHash << "): " << constructedStr << "\n"
					<< "  Parsed (hash: " << parsedHash << "): " << parsedBuilder.toString();

				EXPECT_EQ( constructedBuilder, parsedBuilder )
					<< "LocalIdBuilder objects should be equal for construction vs parsing:\n"
					<< "  Constructed: " << constructedStr << "\n"
					<< "  Parsed: " << parsedBuilder.toString();

				// Verify structural consistency
				EXPECT_EQ( constructedBuilder.visVersion(), parsedBuilder.visVersion() )
					<< "VIS version should be consistent between construction and parsing";

				EXPECT_EQ( constructedBuilder.isVerboseMode(), parsedBuilder.isVerboseMode() )
					<< "Verbose mode should be consistent between construction and parsing";
			}
			catch ( const std::exception& ex )
			{
				// If parsing fails, it might indicate toString() format issues
				FAIL() << "Failed to parse constructed LocalIdBuilder string: '" << constructedStr << "'\n"
					   << "  Parse error: " << ex.what();
			}
		}
	}

	//----------------------------------------------
	// Fluent API hash consistency tests
	//----------------------------------------------

	TEST_F( LocalIdBuilderHashTests, Test_Hash_Consistency_FluentModifications )
	{
		// Test hash consistency when using fluent API modifications
		const auto& vis = VIS::instance();
		auto visVersion = VisVersion::v3_4a;
		const auto& gmod = vis.gmod( visVersion );
		const auto& codebooks = vis.codebooks( visVersion );

		auto primaryItem = gmod.parsePath( "411.1/C101.31-2" );
		auto baseBuilder = LocalIdBuilder::create( visVersion )
							   .withPrimaryItem( std::move( primaryItem ) )
							   .tryWithMetadataTag( codebooks.tryCreateTag( CodebookName::Quantity, "temperature" ) );

		auto baseHash = baseBuilder.hashCode();

		// Test adding additional metadata tags
		auto builderWithContent = baseBuilder.tryWithMetadataTag( codebooks.tryCreateTag( CodebookName::Content, "exhaust.gas" ) );
		auto contentHash = builderWithContent.hashCode();

		EXPECT_NE( baseHash, contentHash ) << "Adding additional metadata tag should change hash";

		auto builderWithPosition = builderWithContent.tryWithMetadataTag( codebooks.tryCreateTag( CodebookName::Position, "inlet" ) );
		auto positionHash = builderWithPosition.hashCode();

		EXPECT_NE( contentHash, positionHash ) << "Adding additional metadata tag should change hash";

		// Test removing metadata tags
		auto builderWithoutContent = builderWithPosition.withoutContent();
		auto withoutContentHash = builderWithoutContent.hashCode();

		EXPECT_NE( positionHash, withoutContentHash ) << "Removing content should change hash";
		EXPECT_NE( builderWithPosition, builderWithoutContent ) << "Removing content should make builders different";
	}

	//----------------------------------------------
	// STL container integration tests
	//----------------------------------------------

	TEST_F( LocalIdBuilderHashTests, Test_StdHash_UnorderedMap_Integration )
	{
		std::unordered_map<LocalIdBuilder, std::string> builderMap;
		std::vector<LocalIdBuilder> testBuilders;

		// Parse test LocalIdBuilders - filter to same VIS version to avoid comparison exceptions
		for ( const auto& localIdStr : testLocalIdStrings )
		{
			try
			{
				auto builder = LocalIdBuilder::parse( localIdStr );
				testBuilders.push_back( builder );
			}
			catch ( ... )
			{
			}
		}

		ASSERT_GT( testBuilders.size(), 2 ) << "Should have sufficient LocalIdBuilders from same VIS version for testing";

		// Test insertion and lookup with LocalIdBuilders
		for ( size_t i = 0; i < testBuilders.size(); ++i )
		{
			std::string description = "Builder_" + std::to_string( i );
			builderMap[testBuilders[i]] = description;
		}

		EXPECT_EQ( builderMap.size(), testBuilders.size() ) << "STL map should contain all unique LocalIdBuilders";

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

	TEST_F( LocalIdBuilderHashTests, Test_StdHash_Specialization_UnorderedSet )
	{
		std::unordered_set<LocalIdBuilder> builderSet;

		auto builder1 = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );
		auto builder2 = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/411.1/C101.63/S206/~propulsion.engine/~cooling.system/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto builder3 = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low" );
		auto builder4 = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" );

		builderSet.insert( builder1 );
		builderSet.insert( builder2 );
		builderSet.insert( builder3 );
		builderSet.insert( builder4 );

		builderSet.insert( builder1 ); // Duplicate
		builderSet.insert( builder2 ); // Duplicate

		EXPECT_EQ( builderSet.size(), 4 ) << "Set should contain exactly 4 unique LocalIdBuilders";

		EXPECT_NE( builderSet.find( builder1 ), builderSet.end() ) << "LocalIdBuilder 1 should be in set";
		EXPECT_NE( builderSet.find( builder2 ), builderSet.end() ) << "LocalIdBuilder 2 should be in set";
		EXPECT_NE( builderSet.find( builder3 ), builderSet.end() ) << "LocalIdBuilder 3 should be in set";
		EXPECT_NE( builderSet.find( builder4 ), builderSet.end() ) << "LocalIdBuilder 4 should be in set";
	}

	//----------------------------------------------
	// Hash validation
	//----------------------------------------------

	TEST_F( LocalIdBuilderHashTests, Test_Hash_StdHash_Consistency )
	{
		auto builder = LocalIdBuilder::parse( "/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low" );

		// Test std::hash specialization
		std::hash<LocalIdBuilder> stdHasher;
		auto stdHash = stdHasher( builder );
		auto memberHash = builder.hashCode();

		EXPECT_EQ( stdHash, memberHash ) << "std::hash specialization should match member hashCode()";
	}
} // namespace dnv::vista::sdk::tests
