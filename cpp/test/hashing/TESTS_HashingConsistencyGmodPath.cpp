/**
 * @file TESTS_HashingConsistencyGmodPath.cpp
 * @brief Hash consistency validation suite for GmodPath class
 * @details This test suite validates hash function behavior, distribution quality, and consistency
 *          across all GmodPath operations including copy/move semantics, path parsing,
 *          and STL container integration.
 */

#include <fstream>

#include <gtest/gtest.h>

#include <dnv/vista/sdk/GmodPath.h>
#include <dnv/vista/sdk/GmodNode.h>
#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/VISVersion.h>

namespace dnv::vista::sdk::tests
{
	//========================================================================
	// GmodPath hashing consistency, uniqueness and validation tests
	//========================================================================

	//----------------------------------------------
	// Dataset
	//----------------------------------------------

	namespace
	{
		std::string gmodPathUniqueDataPath()
		{
#ifdef VISTA_SDK_TESTDATA_DIR
			return std::string{ VISTA_SDK_TESTDATA_DIR } + "/GmodPaths_unique_vis-3-4a.txt";
#else
			return "testdata/GmodPaths_unique_vis-3-4a.txt";
#endif
		}
	} // namespace

	//----------------------------------------------
	// Test fixture
	//----------------------------------------------

	class GmodPathHashTests : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			auto& vis = VIS::instance();
			gmod = &vis.gmod( VisVersion::v3_9a );
			locations = &vis.locations( VisVersion::v3_9a );
		}

		const Gmod* gmod = nullptr;
		const Locations* locations = nullptr;
	};

	//----------------------------------------------
	// Hash uniqueness validation
	//----------------------------------------------

	TEST_F( GmodPathHashTests, Test_Hash_Uniqueness_DifferentPaths )
	{
		auto path1 = GmodPath::parse( "VE/000a", VisVersion::v3_9a );
		auto path2 = GmodPath::parse( "VE/000a/000", VisVersion::v3_9a );
		auto path3 = GmodPath::parse( "VE/000a/000/001", VisVersion::v3_9a );
		auto path4 = GmodPath::parse( "VE/000a/000/002/002.1", VisVersion::v3_9a );

		auto hash1 = path1.hashCode();
		auto hash2 = path2.hashCode();
		auto hash3 = path3.hashCode();
		auto hash4 = path4.hashCode();

		EXPECT_NE( hash1, hash2 ) << "Different paths must have distinct hashes";
		EXPECT_NE( hash1, hash3 ) << "Different paths must have distinct hashes";
		EXPECT_NE( hash1, hash4 ) << "Different paths must have distinct hashes";
		EXPECT_NE( hash2, hash3 ) << "Different paths must have distinct hashes";
		EXPECT_NE( hash2, hash4 ) << "Different paths must have distinct hashes";
		EXPECT_NE( hash3, hash4 ) << "Different paths must have distinct hashes";
	}

	TEST_F( GmodPathHashTests, Test_Hash_Uniqueness_Smoke )
	{
		std::unordered_set<std::size_t> uniqueHashes;
		std::vector<GmodPath> allPaths;

		std::ifstream datasetFile( gmodPathUniqueDataPath() );
		ASSERT_TRUE( datasetFile.is_open() ) << "Could not open test data file: " << gmodPathUniqueDataPath();

		std::string line;
		while ( std::getline( datasetFile, line ) )
		{
			if ( !line.empty() )
			{
				try
				{
					auto path = GmodPath::parse( line, VisVersion::v3_4a );
					allPaths.push_back( path );
				}
				catch ( ... )
				{
				}
			}
		}
		datasetFile.close();

		// Test hash uniqueness across entire dataset
		size_t collisionCount = 0;
		for ( const auto& path : allPaths )
		{
			std::size_t pathHash = path.hashCode();

			if ( uniqueHashes.find( pathHash ) != uniqueHashes.end() )
			{
				collisionCount++;
			}
			else
			{
				uniqueHashes.insert( pathHash );
			}
		}
		double uniquenessRatio = static_cast<double>( uniqueHashes.size() ) / static_cast<double>( allPaths.size() );

		EXPECT_EQ( collisionCount, 0 )
			<< "Hash function should produce zero collisions for unique GmodPaths. "
			<< "Detected " << collisionCount << " collisions out of " << allPaths.size() << " unique nodes";

		EXPECT_EQ( uniqueHashes.size(), allPaths.size() )
			<< "Each unique GmodPath should have a unique hash. "
			<< "Expected " << allPaths.size() << " unique hashes, got " << uniqueHashes.size();

		EXPECT_GE( uniquenessRatio, 1.0 )
			<< "Hash uniqueness ratio should be 100% for unique dataset. "
			<< "Achieved: " << ( uniquenessRatio * 100.0 ) << "%";
	}

	//----------------------------------------------
	// Hash consistency and immutability tests
	//----------------------------------------------

	TEST_F( GmodPathHashTests, Test_Hash_Consistency_CopyConstructor )
	{
		auto original = GmodPath::parse( "VE/000a/000/001", VisVersion::v3_9a );

		GmodPath copy( original );
		auto originalHash = original.hashCode();
		auto copyHash = copy.hashCode();

		EXPECT_EQ( originalHash, copyHash ) << "Copy constructor should preserve hash code";
		EXPECT_EQ( original.toString(), copy.toString() ) << "Copy should be equal to original";
	}

	TEST_F( GmodPathHashTests, Test_Hash_Consistency_MoveConstructor )
	{
		auto original = GmodPath::parse( "VE/000a/000/002", VisVersion::v3_9a );
		auto originalHash = original.hashCode();

		GmodPath moved( std::move( original ) );
		auto movedHash = moved.hashCode();

		EXPECT_EQ( originalHash, movedHash ) << "Move constructor should preserve hash code";
	}

	TEST_F( GmodPathHashTests, Test_Hash_Consistency_CopyAssignment )
	{
		auto original = GmodPath::parse( "VE/000a/000/003", VisVersion::v3_9a );
		auto target = GmodPath::parse( "VE/000a", VisVersion::v3_9a ); // Different path initially
		auto originalHash = original.hashCode();

		target = original;
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Copy assignment should preserve hash code";
		EXPECT_EQ( original.toString(), target.toString() ) << "Copy assignment should make paths equal";
	}

	TEST_F( GmodPathHashTests, Test_Hash_Consistency_MoveAssignment )
	{
		auto original = GmodPath::parse( "VE/000a/000/004", VisVersion::v3_9a );
		auto target = GmodPath::parse( "VE/000a/010", VisVersion::v3_9a );
		auto originalHash = original.hashCode();

		target = std::move( original );
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Move assignment should preserve hash code";
	}

	TEST_F( GmodPathHashTests, Test_Hash_Consistency_LocationModification )
	{
		// Test with path that has location information
		auto pathWithLocation = GmodPath::parse( "411.1/C101.31-5", VisVersion::v3_4a );
		auto withoutLocations = pathWithLocation.withoutLocations();

		auto pathWithLocationHash = pathWithLocation.hashCode();
		auto withoutLocationsHash = withoutLocations.hashCode();

		// Test consistency of withoutLocations() operation
		auto withoutLocations2 = pathWithLocation.withoutLocations();
		EXPECT_EQ( withoutLocationsHash, withoutLocations2.hashCode() )
			<< "Multiple withoutLocations() calls should produce identical hashes";

		EXPECT_NE( pathWithLocationHash, withoutLocationsHash )
			<< "Path with location and withoutLocations() should have different hashes";

		// Test with path without location information
		auto pathWithoutLocation = GmodPath::parse( "411.1/C101.31", VisVersion::v3_4a );
		auto pathNoLocationCopy = pathWithoutLocation.withoutLocations();

		auto baseHash = pathWithoutLocation.hashCode();
		auto copyHash = pathNoLocationCopy.hashCode();

		EXPECT_EQ( baseHash, copyHash )
			<< "Path without location and withoutLocations() should have same hash";
	}

	//----------------------------------------------
	// Parse methods
	//----------------------------------------------

	TEST_F( GmodPathHashTests, Test_Hash_Consistency_AllParseMethods )
	{
		// Test paths that work with both regular parse and parseFullPath
		const std::vector<std::string> testPaths = {
			"VE",			   // Root level
			"VE/000a",		   // Two levels
			"VE/000a/000",	   // Three levels
			"VE/000a/000/001", // Four levels
			"411.1/C101.31",   // Different branch
			"621.21/S90",	   // Another branch
			"612.42"		   // Single component
		};

		for ( const auto& pathStr : testPaths )
		{
			std::vector<std::size_t> allHashes;
			std::vector<std::string> methodNames;
			std::vector<GmodPath> allPaths; // Store actual paths for structure comparison

			// Method 1: parse(string_view, VisVersion)
			try
			{
				auto path1 = GmodPath::parse( pathStr, VisVersion::v3_9a );
				allHashes.push_back( path1.hashCode() );
				methodNames.push_back( "parse(string_view, VisVersion)" );
				allPaths.push_back( path1 );
			}
			catch ( const std::exception& )
			{
				// Some paths might not be parseable with this method
			}

			// Method 2: parse(string_view, Gmod&, Locations&)
			try
			{
				auto path2 = GmodPath::parse( pathStr, *gmod, *locations );
				allHashes.push_back( path2.hashCode() );
				methodNames.push_back( "parse(string_view, Gmod&, Locations&)" );
				allPaths.push_back( path2 );
			}
			catch ( const std::exception& )
			{
				// Some paths might not be parseable with this method
			}

			// Method 3: tryParse(string_view, VisVersion, optional&)
			{
				std::optional<GmodPath> path3;
				if ( GmodPath::tryParse( pathStr, VisVersion::v3_9a, path3 ) && path3.has_value() )
				{
					allHashes.push_back( path3->hashCode() );
					methodNames.push_back( "tryParse(string_view, VisVersion, optional&)" );
					allPaths.push_back( *path3 );
				}
			}

			// Method 4: tryParse(string_view, Gmod&, Locations&, optional&)
			{
				std::optional<GmodPath> path4;
				if ( GmodPath::tryParse( pathStr, *gmod, *locations, path4 ) && path4.has_value() )
				{
					allHashes.push_back( path4->hashCode() );
					methodNames.push_back( "tryParse(string_view, Gmod&, Locations&, optional&)" );
					allPaths.push_back( *path4 );
				}
			}

			// Skip full path methods for short paths that aren't full paths
			bool isFullPath = pathStr.find( "VE/" ) == 0 || pathStr == "VE";

			if ( isFullPath )
			{
				// Method 5: parseFullPath(string_view, VisVersion)
				try
				{
					auto path5 = GmodPath::parseFullPath( pathStr, VisVersion::v3_9a );
					allHashes.push_back( path5.hashCode() );
					methodNames.push_back( "parseFullPath(string_view, VisVersion)" );
					allPaths.push_back( path5 );
				}
				catch ( const std::exception& )
				{
					// Some paths might not be parseable as full paths
				}

				// Method 6: tryParseFullPath(string_view, VisVersion, optional&)
				{
					std::optional<GmodPath> path6;
					if ( GmodPath::tryParseFullPath( pathStr, VisVersion::v3_9a, path6 ) && path6.has_value() )
					{
						allHashes.push_back( path6->hashCode() );
						methodNames.push_back( "tryParseFullPath(string_view, VisVersion, optional&)" );
						allPaths.push_back( *path6 );
					}
				}

				// Method 7: tryParseFullPath(string_view, Gmod&, Locations&, optional&)
				{
					std::optional<GmodPath> path7;
					if ( GmodPath::tryParseFullPath( pathStr, *gmod, *locations, path7 ) && path7.has_value() )
					{
						allHashes.push_back( path7->hashCode() );
						methodNames.push_back( "tryParseFullPath(string_view, Gmod&, Locations&, optional&)" );
						allPaths.push_back( *path7 );
					}
				}
			}

			// Verify all successful parsing methods produced the same hash
			if ( allHashes.size() >= 2 )
			{
				std::size_t referenceHash = allHashes[0];
				std::string referenceName = methodNames[0];

				for ( size_t i = 1; i < allHashes.size(); ++i )
				{
					EXPECT_EQ( referenceHash, allHashes[i] )
						<< "Hash mismatch for path: '" << pathStr << "'\n"
						<< "  Reference method: " << referenceName << " (hash: " << referenceHash << ")\n"
						<< "  Compared method:  " << methodNames[i] << " (hash: " << allHashes[i] << ")";
				}
			}
		}
	}

	TEST_F( GmodPathHashTests, Test_Hash_Consistency_ConstructorVsParsing )
	{
		// Test that manually constructed GmodPath has same hash as parsed equivalent
		const std::string testPathStr = "VE/000a/000/001";

		// Parse the path to get the structure
		auto parsedPath = GmodPath::parse( testPathStr, VisVersion::v3_9a );
		auto parsedHash = parsedPath.hashCode();

		// Now manually construct the same path using constructor
		std::vector<GmodNode> parents;

		// Get the nodes that make up this path
		for ( size_t i = 0; i < parsedPath.length() - 1; ++i )
		{
			parents.push_back( parsedPath[i] );
		}

		GmodNode targetNode = parsedPath[parsedPath.length() - 1];

		// Construct using the constructor
		GmodPath constructedPath( parents, targetNode );
		auto constructedHash = constructedPath.hashCode();

		EXPECT_EQ( parsedHash, constructedHash )
			<< "Constructor and parsing methods must produce identical hashes for path: " << testPathStr
			<< "\n  Parsed hash:      " << parsedHash
			<< "\n  Constructed hash: " << constructedHash;

		EXPECT_EQ( parsedPath.toString(), constructedPath.toString() )
			<< "Constructor and parsing methods must produce identical string representations";

		EXPECT_EQ( parsedPath.length(), constructedPath.length() )
			<< "Constructor and parsing methods must produce identical path lengths";
	}

	//----------------------------------------------
	// STL container integration tests
	//----------------------------------------------

	TEST_F( GmodPathHashTests, Test_StdHash_UnorderedMap_Integration )
	{
		std::unordered_map<GmodPath, std::string> pathMap;
		std::vector<GmodPath> testPaths;

		std::ifstream datasetFile( gmodPathUniqueDataPath() );
		if ( datasetFile.is_open() )
		{
			std::string line;
			while ( std::getline( datasetFile, line ) )
			{
				if ( !line.empty() )
				{
					try
					{
						auto path = GmodPath::parse( line, VisVersion::v3_4a );
						testPaths.push_back( path );
					}
					catch ( ... )
					{
					}
				}
			}
			datasetFile.close();
		}

		for ( size_t i = 0; i < testPaths.size(); ++i )
		{
			pathMap[testPaths[i]] = "Component_" + std::to_string( i );
		}

		EXPECT_GT( pathMap.size(), testPaths.size() / 2 ) << "STL map should handle most unique paths. Got: " << pathMap.size() << " entries";

		size_t successfulLookups = 0;
		for ( size_t i = 0; i < std::min( testPaths.size(), size_t( 100 ) ); ++i )
		{
			auto it = pathMap.find( testPaths[i] );
			if ( it != pathMap.end() )
			{
				successfulLookups++;
			}
		}

		EXPECT_GT( successfulLookups, 50 ) << "Most lookups should succeed. Got: " << successfulLookups << " out of 100";
	}

	TEST_F( GmodPathHashTests, Test_StdHash_Specialization_UnorderedSet )
	{
		std::unordered_set<GmodPath> pathSet;

		auto basePath = GmodPath::parse( "411.1/C101.31-5", VisVersion::v3_4a );
		auto basePathWithoutLocations = basePath.withoutLocations();
		auto differentPath = GmodPath::parse( "441.1/E202", VisVersion::v3_4a );
		auto anotherPath = GmodPath::parse( "621.21/S90", VisVersion::v3_4a );

		pathSet.insert( basePath );
		pathSet.insert( basePathWithoutLocations );
		pathSet.insert( differentPath );
		pathSet.insert( anotherPath );

		pathSet.insert( basePath );					// Duplicate
		pathSet.insert( basePathWithoutLocations ); // Duplicate

		EXPECT_GE( pathSet.size(), 3 ) << "Set should contain at least 3 unique paths";
		EXPECT_LE( pathSet.size(), 4 ) << "Set should contain at most 4 paths";

		EXPECT_NE( pathSet.find( basePath ), pathSet.end() ) << "Base path should be in set";
		EXPECT_NE( pathSet.find( basePathWithoutLocations ), pathSet.end() )
			<< "Path without locations should be in set";
		EXPECT_NE( pathSet.find( differentPath ), pathSet.end() )
			<< "Different path should be in set";
		EXPECT_NE( pathSet.find( anotherPath ), pathSet.end() )
			<< "Another path should be in set";
	}

	//----------------------------------------------
	// Hash validation
	//----------------------------------------------

	TEST_F( GmodPathHashTests, Test_Hash_StdHash_Consistency )
	{
		// Ensures that the STL hash specialization produces identical hash values to the instance method hashCode().
		std::vector<GmodPath> testPaths = {
			GmodPath::parse( "411.1", VisVersion::v3_9a ),
			GmodPath::parse( "411.1/C101.31", VisVersion::v3_9a ),
			GmodPath::parse( "612.21", VisVersion::v3_9a ),
			GmodPath::parse( "924.2", VisVersion::v3_9a ).withoutLocations() };

		std::hash<GmodPath> stdHasher;
		for ( const auto& path : testPaths )
		{
			auto methodHash = path.hashCode();
			auto stdHash = stdHasher( path );
			EXPECT_EQ( methodHash, stdHash )
				<< "std::hash specialization should match hashCode() method for path: " << path.toString();
		}
	}
} // namespace dnv::vista::sdk::tests
