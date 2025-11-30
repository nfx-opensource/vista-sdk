/**
 * @file TESTS_HashingConsistencyLocations.cpp
 * @brief Hash consistency validation suite for Location class
 * @details This test suite validates hash function behavior, distribution quality, and consistency
 *          across all Location operations including copy/move semantics, Location parsing,
 *          and STL container integration with maritime location codes.
 */

#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <gtest/gtest.h>

#include <dnv/vista/sdk/Locations.h>
#include <dnv/vista/sdk/ParsingErrors.h>
#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/VISVersion.h>

namespace dnv::vista::sdk::tests
{
	//========================================================================
	// Location hashing consistency, uniqueness and validation tests
	//========================================================================

	//----------------------------------------------
	// Dataset
	//----------------------------------------------

	namespace
	{
		std::vector<std::string> locationTestData()
		{
			return {
				// Valid single character locations
				"S", "P", "C", "U", "M", "L", "F", "A", "I", "O", "H", "V",

				// Valid numeric locations
				"1", "2", "3", "4", "5", "6", "7", "8", "9",
				"34", "77",

				// Valid combined locations
				"7C", "77C",
				"1A", "1C", "1F", "1I", "1L", "1M", "1O", "1P", "1S", "1U",
				"2A", "2C", "2F", "2I", "2L", "2M", "2O", "2P", "2S", "2U",

				// Valid maritime combinations from Locations.json test data
				"FIPU",	 // Forward Inner Port Upper
				"1FIPU", // Zone 1 Forward Inner Port Upper
				"2FIPU", // Zone 2 Forward Inner Port Upper
				"FIPS",	 // Forward Inner Port Starboard
				"1FIPS", // Zone 1 Forward Inner Port Starboard
				"2FIPS", // Zone 2 Forward Inner Port Starboard
				"AIPU",	 // After Inner Port Upper
				"AIPS",	 // After Inner Port Starboard

				// Additional single-group valid combinations (no conflicts)
				"FI", "FL", "FM", "FO", "FP", "FS", "FU",
				"AI", "AL", "AM", "AO", "AP", "AS", "AU",
				"CI", "CL", "CM", "CO", "CP", "CS", "CU",
				"IL", "IM", "IO", "IP", "IS", "IU",
				"LO", "LP", "LS", "LU",
				"MO", "MP", "MS", "MU",
				"OP", "OS", "OU",
				"PU", "SU" };
		}
	} // namespace

	//----------------------------------------------
	// Test fixture
	//----------------------------------------------

	class LocationHashTests : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			locations = &VIS::instance().locations( VisVersion::v3_9a );
			testLocationStrings = locationTestData();
		}

		const Locations* locations = nullptr;
		std::vector<std::string> testLocationStrings;
	};

	//----------------------------------------------
	// Hash uniqueness validation
	//----------------------------------------------

	TEST_F( LocationHashTests, Test_Hash_Uniqueness_DifferentLocations )
	{
		auto location1 = locations->parse( "P" );
		auto location2 = locations->parse( "S" );
		auto location3 = locations->parse( "1P" );
		auto location4 = locations->parse( "FIPU" );

		auto hash1 = location1.hashCode();
		auto hash2 = location2.hashCode();
		auto hash3 = location3.hashCode();
		auto hash4 = location4.hashCode();

		EXPECT_NE( hash1, hash2 ) << "Different Locations must have distinct hashes";
		EXPECT_NE( hash1, hash3 ) << "Different Locations must have distinct hashes";
		EXPECT_NE( hash1, hash4 ) << "Different Locations must have distinct hashes";
		EXPECT_NE( hash2, hash3 ) << "Different Locations must have distinct hashes";
		EXPECT_NE( hash2, hash4 ) << "Different Locations must have distinct hashes";
		EXPECT_NE( hash3, hash4 ) << "Different Locations must have distinct hashes";
	}

	TEST_F( LocationHashTests, Test_Hash_Uniqueness_SingleVsCompoundLocations )
	{
		auto singleP = locations->parse( "P" );
		auto singleS = locations->parse( "S" );
		auto compound1P = locations->parse( "1P" );
		auto compoundPU = locations->parse( "PU" );

		auto hashSingleP = singleP.hashCode();
		auto hashSingleS = singleS.hashCode();
		auto hashCompound1P = compound1P.hashCode();
		auto hashCompoundPU = compoundPU.hashCode();

		EXPECT_NE( hashSingleP, hashSingleS ) << "Single character locations should have different hashes";
		EXPECT_NE( hashSingleP, hashCompound1P ) << "Single and compound locations should have different hashes";
		EXPECT_NE( hashSingleP, hashCompoundPU ) << "Single and compound locations should have different hashes";
		EXPECT_NE( hashCompound1P, hashCompoundPU ) << "Different compound locations should have different hashes";
	}

	//----------------------------------------------
	// Hash consistency and immutability tests
	//----------------------------------------------

	TEST_F( LocationHashTests, Test_Hash_Consistency_CopyConstructor )
	{
		auto original = locations->parse( "FIPU" );

		Location copy( original );
		auto originalHash = original.hashCode();
		auto copyHash = copy.hashCode();

		EXPECT_EQ( originalHash, copyHash ) << "Copy constructor should preserve hash code";
		EXPECT_EQ( original, copy ) << "Copy should be equal to original";
	}

	TEST_F( LocationHashTests, Test_Hash_Consistency_MoveConstructor )
	{
		auto original = locations->parse( "1FIPU" );
		auto originalHash = original.hashCode();

		Location moved( std::move( original ) );
		auto movedHash = moved.hashCode();

		EXPECT_EQ( originalHash, movedHash ) << "Move constructor should preserve hash code";
	}

	TEST_F( LocationHashTests, Test_Hash_Consistency_CopyAssignment )
	{
		auto original = locations->parse( "FIPU" );
		auto target = locations->parse( "C" ); // Different location initially
		auto originalHash = original.hashCode();

		target = original;
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Copy assignment should preserve hash code";
		EXPECT_EQ( original, target ) << "Copy assignment should make locations equal";
	}

	TEST_F( LocationHashTests, Test_Hash_Consistency_MoveAssignment )
	{
		auto original = locations->parse( "2C" );
		auto target = locations->parse( "1P" );
		auto originalHash = original.hashCode();

		target = std::move( original );
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Move assignment should preserve hash code";
	}

	TEST_F( LocationHashTests, Test_Hash_Consistency_RepeatAccess )
	{
		auto location = locations->parse( "FIPU" );

		auto hash1 = location.hashCode();
		auto hash2 = location.hashCode();
		auto hash3 = location.hashCode();

		EXPECT_EQ( hash1, hash2 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash1, hash3 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash2, hash3 ) << "Multiple hash access should be consistent";
	}

	//----------------------------------------------
	// Parse methods
	//----------------------------------------------

	TEST_F( LocationHashTests, Test_Hash_Consistency_AllParseMethods )
	{
		const std::string locationStr = "1FIPU";

		// Test parse() method
		auto parsedLocation = locations->parse( locationStr );
		auto parsedHash = parsedLocation.hashCode();

		// Test tryParse() method with string
		Location tryParseLocation;
		bool parseSuccess = locations->tryParse( std::string{ locationStr }, tryParseLocation );
		ASSERT_TRUE( parseSuccess ) << "tryParse should succeed for valid location";
		auto tryParseHash = tryParseLocation.hashCode();

		// Test tryParse() method with string_view
		Location tryParseViewLocation;
		std::string_view locationView( locationStr );
		bool parseViewSuccess = locations->tryParse( locationView, tryParseViewLocation );
		ASSERT_TRUE( parseViewSuccess ) << "tryParse with string_view should succeed";
		auto tryParseViewHash = tryParseViewLocation.hashCode();

		// Test tryParse() method with errors
		Location tryParseErrorLocation;
		ParsingErrors errors;
		bool parseErrorSuccess = locations->tryParse( std::string_view( locationStr ), tryParseErrorLocation, errors );
		ASSERT_TRUE( parseErrorSuccess ) << "tryParse with errors should succeed";
		auto tryParseErrorHash = tryParseErrorLocation.hashCode();

		// All parsing methods should produce identical hashes for the same location
		EXPECT_EQ( parsedHash, tryParseHash ) << "parse() and tryParse() should produce identical hashes";
		EXPECT_EQ( parsedHash, tryParseViewHash ) << "parse() and tryParse(string_view) should produce identical hashes";
		EXPECT_EQ( parsedHash, tryParseErrorHash ) << "parse() and tryParse(with errors) should produce identical hashes";

		// All locations should be equal
		EXPECT_EQ( parsedLocation, tryParseLocation ) << "Locations from different parse methods should be equal";
		EXPECT_EQ( parsedLocation, tryParseViewLocation ) << "Locations from different parse methods should be equal";
		EXPECT_EQ( parsedLocation, tryParseErrorLocation ) << "Locations from different parse methods should be equal";

		// String representations should be identical
		EXPECT_EQ( parsedLocation.toString(), tryParseLocation.toString() ) << "String representations should be identical";
		EXPECT_EQ( parsedLocation.toString(), tryParseViewLocation.toString() ) << "String representations should be identical";
		EXPECT_EQ( parsedLocation.toString(), tryParseErrorLocation.toString() ) << "String representations should be identical";
	}

	TEST_F( LocationHashTests, Test_Hash_Consistency_ParseErrorHandling )
	{
		// Test that parsing errors don't affect hash consistency for valid locations
		const std::vector<std::string> invalidLocations = {
			"",		// Empty
			"Z",	// Invalid character
			"1Z",	// Invalid combination
			"ZP",	// Invalid start character
			"PA",	// Wrong order (should be AP)
			"ACFL", // Multiple from longitudinal group (A, F conflicting with L)
			"P1",	// Wrong order
		};

		for ( const auto& invalidStr : invalidLocations )
		{
			Location location;
			ParsingErrors errors;
			bool success = locations->tryParse( std::string_view( invalidStr ), location, errors );
			EXPECT_FALSE( success ) << "Parsing should fail for invalid location: " << invalidStr;
		}

		// Verify that valid locations still parse correctly after error cases
		auto validLocation = locations->parse( "1FIPU" );
		auto validHash = validLocation.hashCode();

		Location retestLocation;
		bool retestSuccess = locations->tryParse( std::string{ "1FIPU" }, retestLocation );
		ASSERT_TRUE( retestSuccess ) << "Valid location should still parse after error cases";
		auto retestHash = retestLocation.hashCode();

		EXPECT_EQ( validHash, retestHash ) << "Hash should be consistent after parsing errors";
	}

	//----------------------------------------------
	// STL container integration tests
	//----------------------------------------------

	TEST_F( LocationHashTests, Test_StdHash_UnorderedMap_Integration )
	{
		std::unordered_map<Location, std::string> locationMap;
		std::unordered_map<std::size_t, Location> hashMap;

		// Test insertion and lookup with various location types
		const std::vector<std::pair<std::string, std::string>> testPairs = {
			{ "P", "Port" },
			{ "S", "Starboard" },
			{ "1P", "Port Zone 1" },
			{ "FIPU", "Forward Inner Port Upper" },
			{ "2FIPU", "Zone 2 Forward Inner Port Upper" } };

		for ( const auto& [locationStr, description] : testPairs )
		{
			auto location = locations->parse( locationStr );
			auto hash = location.hashCode();

			// Test that std::hash produces the same result as hashCode()
			std::hash<Location> stdHasher;
			auto stdHash = stdHasher( location );
			EXPECT_EQ( hash, stdHash ) << "std::hash should match hashCode() for location: " << locationStr;

			// Insert into containers
			locationMap[location] = description;
			hashMap[hash] = location;

			// Verify retrieval
			EXPECT_EQ( locationMap[location], description ) << "Should retrieve correct description for location: " << locationStr;
			EXPECT_EQ( hashMap[hash], location ) << "Should retrieve correct location for hash";
		}

		EXPECT_EQ( locationMap.size(), testPairs.size() ) << "All locations should be stored in unordered_map";
		EXPECT_EQ( hashMap.size(), testPairs.size() ) << "All hashes should be unique";
	}

	TEST_F( LocationHashTests, Test_StdHash_Specialization_UnorderedSet )
	{
		std::unordered_set<Location> locationSet;

		// Insert various locations
		for ( const auto& locationStr : testLocationStrings )
		{
			Location location;
			if ( locations->tryParse( std::string{ locationStr }, location ) )
			{
				auto insertResult = locationSet.insert( location );
				EXPECT_TRUE( insertResult.second ) << "Each unique location should be inserted successfully: " << locationStr;

				// Verify the location can be found
				auto findResult = locationSet.find( location );
				EXPECT_NE( findResult, locationSet.end() ) << "Inserted location should be findable: " << locationStr;
				EXPECT_EQ( *findResult, location ) << "Found location should match inserted location";
			}
		}

		// Test that duplicate insertions are handled correctly
		auto duplicateLocation = locations->parse( "P" );
		auto duplicateInsertResult = locationSet.insert( duplicateLocation );
		EXPECT_FALSE( duplicateInsertResult.second ) << "Duplicate location insertion should fail";

		std::cout << "Successfully stored " << locationSet.size() << " unique locations in std::unordered_set\n";
	}

	//----------------------------------------------
	// Hash validation
	//----------------------------------------------

	TEST_F( LocationHashTests, Test_Hash_StdHash_Consistency )
	{
		// Verify that std::hash<Location> produces identical results to Location::hashCode()
		std::hash<Location> stdHasher;

		for ( const auto& locationStr : testLocationStrings )
		{
			Location location;
			if ( locations->tryParse( std::string{ locationStr }, location ) )
			{
				auto locationHash = location.hashCode();
				auto stdHash = stdHasher( location );

				EXPECT_EQ( locationHash, stdHash )
					<< "std::hash<Location> should match Location::hashCode() for location: " << locationStr
					<< " (Location hash: " << locationHash << ", std::hash: " << stdHash << ")";
			}
		}
	}
} // namespace dnv::vista::sdk::tests
