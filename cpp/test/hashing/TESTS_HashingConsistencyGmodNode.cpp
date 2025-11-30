/**
 * @file TESTS_HashingConsistencyGmodNode.cpp
 * @brief Hash consistency validation suite for GmodNode class
 * @details This test suite validates hash function behavior, distribution quality, and consistency
 *          across all GmodNode operations including copy/move semantics, location modifications,
 *          and STL container integration.
 */

#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <gtest/gtest.h>

#include <dnv/vista/sdk/Gmod.h>
#include <dnv/vista/sdk/GmodNode.h>
#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/VISVersion.h>

namespace dnv::vista::sdk::tests
{
	//========================================================================
	// GmodNode hashing consistency, uniqueness and validation tests
	//========================================================================

	//----------------------------------------------
	// Dataset
	//----------------------------------------------

	namespace
	{
		std::string gmodNodeUniqueDataPath()
		{
#ifdef VISTA_SDK_TESTDATA_DIR
			return std::string{ VISTA_SDK_TESTDATA_DIR } + "/GmodNodes_unique_vis-3-4a.txt";
#else
			return "GmodNodes_unique_vis-3-4a.txt";
#endif
		}
	} // namespace

	//----------------------------------------------
	// Test fixture
	//----------------------------------------------

	class GmodNodeHashTests : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			auto& vis = VIS::instance();
			gmod = &vis.gmod( VisVersion::v3_9a );
		}

		const Gmod* gmod = nullptr;
	};

	//----------------------------------------------
	// Hash uniqueness validation
	//----------------------------------------------

	TEST_F( GmodNodeHashTests, Test_Hash_Uniqueness_SameCodeDifferentLocation )
	{
		const GmodNode& baseNode = ( *gmod )["C101.663"];
		auto nodeWithLocation1 = baseNode.withLocation( "FIPU" );
		auto nodeWithLocation2 = baseNode.withLocation( "S" );
		auto nodeWithoutLocation = baseNode.withoutLocation();

		auto baseHash = baseNode.hashCode();
		auto location1Hash = nodeWithLocation1.hashCode();
		auto location2Hash = nodeWithLocation2.hashCode();
		auto noLocationHash = nodeWithoutLocation.hashCode();

		EXPECT_NE( baseHash, location1Hash ) << "Base node and node with location FIPU should have different hashes";
		EXPECT_NE( baseHash, location2Hash ) << "Base node and node with location S should have different hashes";
		EXPECT_NE( location1Hash, location2Hash ) << "Nodes with different locations should have different hashes";

		if ( baseNode.location().has_value() )
		{
			EXPECT_NE( baseHash, noLocationHash ) << "Base node with location and node without location should have different hashes";
		}
		else
		{
			EXPECT_EQ( baseHash, noLocationHash ) << "Base node without location and withoutLocation() should have same hash";
		}

		EXPECT_NE( location1Hash, noLocationHash ) << "Node with location and node without location should have different hashes";
	}

	TEST_F( GmodNodeHashTests, Test_Hash_Uniqueness_SameCodeDifferentLocationPtr )
	{
		GmodNode* baseNode = nullptr;

		bool success = gmod->tryGetNode( "C101.663", baseNode );
		ASSERT_TRUE( success ) << "Should be able to find node C101.663";
		ASSERT_NE( baseNode, nullptr ) << "Node pointer should not be null after successful lookup";

		auto nodeWithLocation1 = baseNode->withLocation( "FIPU" );
		auto nodeWithLocation2 = baseNode->withLocation( "S" );
		auto nodeWithoutLocation = baseNode->withoutLocation();

		auto baseHash = baseNode->hashCode();
		auto location1Hash = nodeWithLocation1.hashCode();
		auto location2Hash = nodeWithLocation2.hashCode();
		auto noLocationHash = nodeWithoutLocation.hashCode();

		EXPECT_NE( baseHash, location1Hash ) << "Base node and node with location FIPU should have different hashes";
		EXPECT_NE( baseHash, location2Hash ) << "Base node and node with location S should have different hashes";
		EXPECT_NE( location1Hash, location2Hash ) << "Nodes with different locations should have different hashes";

		if ( baseNode->location().has_value() )
		{
			EXPECT_NE( baseHash, noLocationHash ) << "Base node with location and node without location should have different hashes";
		}
		else
		{
			EXPECT_EQ( baseHash, noLocationHash ) << "Base node without location and withoutLocation() should have same hash";
		}

		EXPECT_NE( location1Hash, noLocationHash ) << "Node with location and node without location should have different hashes";
	}

	TEST_F( GmodNodeHashTests, Test_Hash_Uniqueness_DifferentCodes )
	{
		const GmodNode& vesselNode = ( *gmod )["VE"];		   // Vessel level
		const GmodNode& hullNode = ( *gmod )["300"];		   // Hull structure
		const GmodNode& engineNode = ( *gmod )["411"];		   // Main engine
		const GmodNode& componentNode = ( *gmod )["C101.663"]; // Specific component

		auto vesselHash = vesselNode.hashCode();
		auto hullHash = hullNode.hashCode();
		auto engineHash = engineNode.hashCode();
		auto componentHash = componentNode.hashCode();

		EXPECT_NE( vesselHash, hullHash ) << "Vessel and hull nodes must have distinct hashes";
		EXPECT_NE( vesselHash, engineHash ) << "Vessel and engine nodes must have distinct hashes";
		EXPECT_NE( vesselHash, componentHash ) << "Vessel and component nodes must have distinct hashes";
		EXPECT_NE( hullHash, engineHash ) << "Hull and engine nodes must have distinct hashes";
		EXPECT_NE( hullHash, componentHash ) << "Hull and component nodes must have distinct hashes";
		EXPECT_NE( engineHash, componentHash ) << "Engine and component nodes must have distinct hashes";
	}

	TEST_F( GmodNodeHashTests, Test_Hash_Uniqueness_Smoke )
	{
		std::unordered_set<std::size_t> uniqueHashes;
		std::vector<GmodNode> allNodes;

		std::vector<std::string> nodeStrings;
		std::ifstream datasetFile( gmodNodeUniqueDataPath() );
		if ( datasetFile.is_open() )
		{
			std::string line;
			while ( std::getline( datasetFile, line ) )
			{
				if ( !line.empty() )
				{
					nodeStrings.push_back( line );
				}
			}
			datasetFile.close();
		}

		// Parse and validate all unique node codes
		for ( const auto& nodeStr : nodeStrings )
		{
			try
			{
				GmodNode* nodePtr = nullptr;
				if ( gmod->tryGetNode( nodeStr, nodePtr ) && nodePtr != nullptr )
				{
					allNodes.push_back( *nodePtr );
				}
			}
			catch ( ... )
			{
			}
		}

		// Test hash uniqueness across entire dataset
		size_t collisionCount = 0;
		for ( const auto& node : allNodes )
		{
			std::size_t nodeHash = node.hashCode();

			if ( uniqueHashes.find( nodeHash ) != uniqueHashes.end() )
			{
				collisionCount++;
			}
			else
			{
				uniqueHashes.insert( nodeHash );
			}
		}

		double uniquenessRatio = static_cast<double>( uniqueHashes.size() ) / static_cast<double>( allNodes.size() );

		EXPECT_EQ( collisionCount, 0 )
			<< "Hash function should produce zero collisions for unique GmodNodes. "
			<< "Detected " << collisionCount << " collisions out of " << allNodes.size() << " unique nodes";

		EXPECT_EQ( uniqueHashes.size(), allNodes.size() )
			<< "Each unique GmodNode should have a unique hash. "
			<< "Expected " << allNodes.size() << " unique hashes, got " << uniqueHashes.size();

		EXPECT_GE( uniquenessRatio, 1.0 )
			<< "Hash uniqueness ratio should be 100% for unique dataset. "
			<< "Achieved: " << ( uniquenessRatio * 100.0 ) << "%";
	}

	//----------------------------------------------
	// Hash consistency and immutability tests
	//----------------------------------------------

	TEST_F( GmodNodeHashTests, Test_Hash_Consistency_CopyConstructor )
	{
		const GmodNode& original = ( *gmod )["411"];

		GmodNode copy( original );
		auto originalHash = original.hashCode();
		auto copyHash = copy.hashCode();

		EXPECT_EQ( originalHash, copyHash ) << "Copy constructor should preserve hash code";
		EXPECT_EQ( original, copy ) << "Copy should be equal to original";
	}

	TEST_F( GmodNodeHashTests, Test_Hash_Consistency_MoveConstructor )
	{
		GmodNode original = ( *gmod )["924.2"];
		auto originalHash = original.hashCode();

		GmodNode moved( std::move( original ) );
		auto movedHash = moved.hashCode();

		EXPECT_EQ( originalHash, movedHash ) << "Move constructor should preserve hash code";
	}

	TEST_F( GmodNodeHashTests, Test_Hash_Consistency_CopyAssignment )
	{
		const GmodNode& original = ( *gmod )["651.21s"];
		GmodNode target = ( *gmod )["VE"]; // Different node initially
		auto originalHash = original.hashCode();

		target = original;
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Copy assignment should preserve hash code";
		EXPECT_EQ( original, target ) << "Copy assignment should make nodes equal";
	}

	TEST_F( GmodNodeHashTests, Test_Hash_Consistency_MoveAssignment )
	{
		GmodNode original = ( *gmod )["C101"];
		GmodNode target = ( *gmod )["300"];
		auto originalHash = original.hashCode();

		target = std::move( original );
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Move assignment should preserve hash code";
	}

	TEST_F( GmodNodeHashTests, Test_Hash_Consistency_LocationModification )
	{
		const GmodNode& baseNode = ( *gmod )["411.1"];

		auto withLocation = baseNode.withLocation( "1FIPU" );
		auto withSameLocation = baseNode.withLocation( "1FIPU" );
		auto withoutLocation1 = withLocation.withoutLocation();
		auto withoutLocation2 = baseNode.withoutLocation();

		EXPECT_EQ( withLocation.hashCode(), withSameLocation.hashCode() )
			<< "Identical location operations should produce identical hashes";

		// withoutLocation() creates nodes with no location, but the hash depends on the node code
		// So they should be equal only if they started from the same base node
		EXPECT_EQ( withoutLocation2.code(), baseNode.code() ) << "withoutLocation2 should have same code as base";
		if ( !baseNode.location().has_value() )
		{
			EXPECT_EQ( withoutLocation1.hashCode(), withoutLocation2.hashCode() )
				<< "withoutLocation() from different sources should produce same hash when base has no location";
		} // Assert: Different operations should produce different hashes
		EXPECT_NE( baseNode.hashCode(), withLocation.hashCode() )
			<< "Adding location should change hash";
		EXPECT_NE( withLocation.hashCode(), withoutLocation1.hashCode() )
			<< "Removing location should change hash";
	}

	//----------------------------------------------
	// STL container integration tests
	//----------------------------------------------

	TEST_F( GmodNodeHashTests, Test_StdHash_UnorderedMap_Integration )
	{
		std::unordered_map<GmodNode, std::string> nodeMap;

		nodeMap[( *gmod )["VE"]] = "Vessel";
		nodeMap[( *gmod )["300"]] = "Hull";
		nodeMap[( *gmod )["411"]] = "Main Engine";
		nodeMap[( *gmod )["C101.663"].withLocation( "FIPU" )] = "Fuel Injection Pump Unit";

		EXPECT_EQ( nodeMap.size(), 4 ) << "All unique nodes should be inserted";
		EXPECT_EQ( nodeMap[( *gmod )["VE"]], "Vessel" ) << "Lookup should work correctly";
		EXPECT_EQ( nodeMap[( *gmod )["300"]], "Hull" ) << "Lookup should work correctly";
		EXPECT_EQ( nodeMap[( *gmod )["411"]], "Main Engine" ) << "Lookup should work correctly";
		EXPECT_EQ( nodeMap[( *gmod )["C101.663"].withLocation( "FIPU" )], "Fuel Injection Pump Unit" )
			<< "Lookup with location should work correctly";

		nodeMap[( *gmod )["VE"]] = "Updated Vessel";
		EXPECT_EQ( nodeMap.size(), 4 ) << "Duplicate key should not increase size";
		EXPECT_EQ( nodeMap[( *gmod )["VE"]], "Updated Vessel" ) << "Value should be updated";
	}

	TEST_F( GmodNodeHashTests, Test_StdHash_Specialization_UnorderedSet )
	{
		std::unordered_set<GmodNode> nodeSet;

		auto baseNode = ( *gmod )["924.2"];
		nodeSet.insert( baseNode );
		nodeSet.insert( baseNode.withLocation( "1FIPU" ) );
		nodeSet.insert( baseNode.withLocation( "FIPU" ) );
		nodeSet.insert( baseNode.withoutLocation() );
		nodeSet.insert( ( *gmod )["651.21s"] );

		nodeSet.insert( baseNode );							// Duplicate
		nodeSet.insert( baseNode.withLocation( "1FIPU" ) ); // Duplicate

		EXPECT_GE( nodeSet.size(), 4 ) << "Set should contain at least 4 unique nodes";
		EXPECT_LE( nodeSet.size(), 5 ) << "Set should contain at most 5 nodes";

		EXPECT_NE( nodeSet.find( baseNode ), nodeSet.end() ) << "Base node should be in set";
		EXPECT_NE( nodeSet.find( baseNode.withLocation( "1FIPU" ) ), nodeSet.end() )
			<< "Node with 1FIPU location should be in set";
		EXPECT_NE( nodeSet.find( baseNode.withLocation( "FIPU" ) ), nodeSet.end() )
			<< "Node with FIPU location should be in set";
		EXPECT_NE( nodeSet.find( baseNode.withoutLocation() ), nodeSet.end() )
			<< "Node without location should be in set";
		EXPECT_NE( nodeSet.find( ( *gmod )["651.21s"] ), nodeSet.end() )
			<< "Different node should be in set";
	}

	//----------------------------------------------
	// Hash validation
	//----------------------------------------------

	TEST_F( GmodNodeHashTests, Test_Hash_StdHash_Consistency )
	{
		std::vector<GmodNode> testNodes = {
			( *gmod )["VE"],
			( *gmod )["300"],
			( *gmod )["411"].withLocation( "1FIPU" ),
			( *gmod )["924.2"].withoutLocation() };

		std::hash<GmodNode> stdHasher;
		for ( const auto& node : testNodes )
		{
			auto methodHash = node.hashCode();
			auto stdHash = stdHasher( node );
			EXPECT_EQ( methodHash, stdHash )
				<< "std::hash specialization should match hashCode() method for node: " << node.code();
		}
	}
} // namespace dnv::vista::sdk::tests
