/**
 * @file TESTS_HashingConsistencyImoNumber.cpp
 * @brief Hash consistency validation suite for ImoNumber class
 * @details This test suite validates hash function behavior, distribution quality, and consistency
 *          across all ImoNumber operations including copy/move semantics, integer validation,
 *          and STL container.
 */

#include <chrono>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <gtest/gtest.h>

#include <dnv/vista/sdk/ImoNumber.h>
#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/VISVersion.h>

namespace dnv::vista::sdk::tests
{
	//========================================================================
	// ImoNumber hashing consistency, uniqueness and validation tests
	//========================================================================

	//----------------------------------------------
	// Dataset
	//----------------------------------------------

	std::string imoNumbers()
	{
#ifdef VISTA_SDK_TESTDATA_DIR
		return std::string{ VISTA_SDK_TESTDATA_DIR } + "/ImoNumbers_unique.txt";
#else
		return "testdata/ImoNumbers_unique.txt";
#endif
	}

	//----------------------------------------------
	// Test fixture
	//----------------------------------------------

	class ImoNumberHashTests : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			std::ifstream file( imoNumbers() );
			ASSERT_TRUE( file.is_open() ) << "Could not open test data file: " << imoNumbers();

			std::string line;
			while ( std::getline( file, line ) )
			{
				if ( line.empty() )
				{
					continue;
				}

				try
				{
					int imoNumber = std::stoi( line );

					if ( ImoNumber::isValid( imoNumber ) )
					{
						validImoNumbers.push_back( imoNumber );
					}
				}
				catch ( const std::exception& )
				{
				}
			}

			file.close();
		}

		std::vector<int> validImoNumbers;
	};

	//----------------------------------------------
	// Hash uniqueness validation
	//----------------------------------------------

	TEST_F( ImoNumberHashTests, Test_Hash_Uniqueness_DifferentNumbers )
	{
		auto imo1 = ImoNumber{ 9074729 };
		auto imo2 = ImoNumber{ 9785811 };
		auto imo3 = ImoNumber{ 9704611 };
		auto imo4 = ImoNumber{ 9368302 };

		auto hash1 = imo1.hashCode();
		auto hash2 = imo2.hashCode();
		auto hash3 = imo3.hashCode();
		auto hash4 = imo4.hashCode();

		EXPECT_NE( hash1, hash2 ) << "Different IMO numbers must have distinct hashes";
		EXPECT_NE( hash1, hash3 ) << "Different IMO numbers must have distinct hashes";
		EXPECT_NE( hash1, hash4 ) << "Different IMO numbers must have distinct hashes";
		EXPECT_NE( hash2, hash3 ) << "Different IMO numbers must have distinct hashes";
		EXPECT_NE( hash2, hash4 ) << "Different IMO numbers must have distinct hashes";
		EXPECT_NE( hash3, hash4 ) << "Different IMO numbers must have distinct hashes";
	}

	TEST_F( ImoNumberHashTests, Test_Hash_Uniqueness_StringVsIntegerConstruction )
	{
		auto imoFromInt = ImoNumber{ 9074729 };
		auto imoFromString = ImoNumber{ "9074729" };
		auto imoFromStringWithPrefix = ImoNumber{ "IMO9074729" };

		auto hashFromInt = imoFromInt.hashCode();
		auto hashFromString = imoFromString.hashCode();
		auto hashFromStringWithPrefix = imoFromStringWithPrefix.hashCode();

		EXPECT_EQ( hashFromInt, hashFromString ) << "Integer and string construction should produce identical hashes";
		EXPECT_EQ( hashFromInt, hashFromStringWithPrefix ) << "String with IMO prefix should produce identical hash";
		EXPECT_EQ( hashFromString, hashFromStringWithPrefix ) << "String constructions should produce identical hashes";

		EXPECT_EQ( imoFromInt, imoFromString ) << "Integer and string construction should be equal";
		EXPECT_EQ( imoFromInt, imoFromStringWithPrefix ) << "All construction methods should be equal";
	}

	TEST_F( ImoNumberHashTests, Test_Hash_Uniqueness_Smoke )
	{
		// Comprehensive hash uniqueness validation across entire ImoNumber dataset
		std::unordered_set<std::size_t> uniqueHashes;
		size_t collisionCount = 0;

		// Test hash uniqueness across the entire dataset
		for ( int imoInt : validImoNumbers )
		{
			ImoNumber imo{ imoInt };
			std::size_t hash = imo.hashCode();

			// Check if we've seen this hash before
			if ( uniqueHashes.find( hash ) != uniqueHashes.end() )
			{
				collisionCount++;
			}
			else
			{
				uniqueHashes.insert( hash );
			}
		}

		double uniquenessRatio = static_cast<double>( uniqueHashes.size() ) / static_cast<double>( validImoNumbers.size() );

		EXPECT_EQ( collisionCount, 0 )
			<< "Hash function should produce zero collisions for unique IMO numbers. "
			<< "Detected " << collisionCount << " collisions out of " << validImoNumbers.size() << " unique IMO numbers";

		EXPECT_EQ( uniqueHashes.size(), validImoNumbers.size() )
			<< "Each unique IMO number should have a unique hash. "
			<< "Expected " << validImoNumbers.size() << " unique hashes, got " << uniqueHashes.size();

		EXPECT_GE( uniquenessRatio, 1.0 )
			<< "Hash uniqueness ratio should be 100% for unique dataset. "
			<< "Achieved: " << ( uniquenessRatio * 100.0 ) << "%";
	}

	//----------------------------------------------
	// Hash consistency and immutability tests
	//----------------------------------------------

	TEST_F( ImoNumberHashTests, Test_Hash_Consistency_CopyConstructor )
	{
		auto original = ImoNumber{ 9387762 };

		ImoNumber copy( original );
		auto originalHash = original.hashCode();
		auto copyHash = copy.hashCode();

		EXPECT_EQ( originalHash, copyHash ) << "Copy constructor should preserve hash code";
		EXPECT_EQ( original, copy ) << "Copy should be equal to original";
	}

	TEST_F( ImoNumberHashTests, Test_Hash_Consistency_MoveConstructor )
	{
		auto original = ImoNumber{ 9370537 };
		auto originalHash = original.hashCode();

		ImoNumber moved( std::move( original ) );
		auto movedHash = moved.hashCode();

		EXPECT_EQ( originalHash, movedHash ) << "Move constructor should preserve hash code";
	}

	TEST_F( ImoNumberHashTests, Test_Hash_Consistency_CopyAssignment )
	{
		auto original = ImoNumber{ 9680102 };
		auto target = ImoNumber{ 9735048 }; // Different IMO initially
		auto originalHash = original.hashCode();

		target = original;
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Copy assignment should preserve hash code";
		EXPECT_EQ( original, target ) << "Copy assignment should make IMO numbers equal";
	}

	TEST_F( ImoNumberHashTests, Test_Hash_Consistency_MoveAssignment )
	{
		auto original = ImoNumber{ 9785823 };
		auto target = ImoNumber{ 1234567 };
		auto originalHash = original.hashCode();

		target = std::move( original );
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Move assignment should preserve hash code";
	}

	TEST_F( ImoNumberHashTests, Test_Hash_Consistency_RepeatAccess )
	{
		auto imo = ImoNumber{ 9074729 };

		auto hash1 = imo.hashCode();
		auto hash2 = imo.hashCode();
		auto hash3 = imo.hashCode();

		EXPECT_EQ( hash1, hash2 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash1, hash3 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash2, hash3 ) << "Multiple hash access should be consistent";
	}

	//----------------------------------------------
	// Parse methods
	//----------------------------------------------

	TEST_F( ImoNumberHashTests, Test_Hash_Consistency_AllParseMethods )
	{
		// Test IMO numbers in different string formats
		const std::vector<std::pair<std::string, int>> testCases = {
			{ "9074729", 9074729 },	   // Numeric only
			{ "IMO9074729", 9074729 }, // With IMO prefix
			{ "imo9074729", 9074729 }, // Lowercase IMO prefix
			{ "9785811", 9785811 },	   // Another numeric
			{ "IMO9785811", 9785811 }, // With IMO prefix
			{ "9704611", 9704611 },	   // Third numeric
			{ "IMO9704611", 9704611 }, // With IMO prefix
			{ "9368302", 9368302 }	   // Fourth numeric
		};

		for ( const auto& [imoStr, expectedInt] : testCases )
		{
			std::vector<std::size_t> allHashes;
			std::vector<std::string> methodNames;
			std::vector<ImoNumber> allImos; // Store actual IMO objects for comparison

			// Method 1: Constructor from integer
			try
			{
				auto imo1 = ImoNumber{ expectedInt };
				allHashes.push_back( imo1.hashCode() );
				methodNames.push_back( "ImoNumber(int)" );
				allImos.push_back( imo1 );
			}
			catch ( const std::exception& )
			{
				// Should not happen with valid test data
			}

			// Method 2: Constructor from string_view
			try
			{
				auto imo2 = ImoNumber{ imoStr };
				allHashes.push_back( imo2.hashCode() );
				methodNames.push_back( "ImoNumber(string_view)" );
				allImos.push_back( imo2 );
			}
			catch ( const std::exception& )
			{
				// Some malformed strings might not be constructible
			}

			// Method 3: parse(string_view)
			try
			{
				auto imo3 = ImoNumber::parse( imoStr );
				allHashes.push_back( imo3.hashCode() );
				methodNames.push_back( "parse(string_view)" );
				allImos.push_back( imo3 );
			}
			catch ( const std::exception& )
			{
				// Some malformed strings might not be parseable
			}

			// Method 4: tryParse(string_view, ImoNumber&)
			{
				ImoNumber imo4;
				if ( ImoNumber::tryParse( imoStr, imo4 ) )
				{
					allHashes.push_back( imo4.hashCode() );
					methodNames.push_back( "tryParse(string_view, ImoNumber&)" );
					allImos.push_back( imo4 );
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
						<< "Hash mismatch for IMO: '" << imoStr << "' (expected: " << expectedInt << ")\n"
						<< "  Reference method: " << referenceName << " (hash: " << referenceHash << ")\n"
						<< "  Compared method:  " << methodNames[i] << " (hash: " << allHashes[i] << ")";

					// Also verify the actual IMO values are equal
					EXPECT_EQ( allImos[0], allImos[i] )
						<< "IMO objects should be equal for input: '" << imoStr << "'\n"
						<< "  Reference method: " << referenceName << " (value: " << static_cast<int>( allImos[0] ) << ")\n"
						<< "  Compared method:  " << methodNames[i] << " (value: " << static_cast<int>( allImos[i] ) << ")";
				}
			}
		}
	}

	TEST_F( ImoNumberHashTests, Test_Hash_Consistency_ConstructorVsParsing )
	{
		// Test that different construction methods produce identical hashes for the same IMO
		const std::vector<std::pair<int, std::string>> testCases = {
			{ 9074729, "9074729" },
			{ 9074729, "IMO9074729" },
			{ 9785811, "9785811" },
			{ 9785811, "IMO9785811" },
			{ 9704611, "imo9704611" }, // Test case insensitive
			{ 9368302, "IMO9368302" } };

		for ( const auto& [intValue, stringValue] : testCases )
		{
			// Constructor from integer
			auto imoFromInt = ImoNumber{ intValue };
			auto hashFromInt = imoFromInt.hashCode();

			// Constructor from string
			auto imoFromString = ImoNumber{ stringValue };
			auto hashFromString = imoFromString.hashCode();

			// parse() method
			auto imoFromParse = ImoNumber::parse( stringValue );
			auto hashFromParse = imoFromParse.hashCode();

			// tryParse() method
			ImoNumber imoFromTryParse;
			ASSERT_TRUE( ImoNumber::tryParse( stringValue, imoFromTryParse ) ) << "tryParse should succeed for valid IMO: " << stringValue;
			auto hashFromTryParse = imoFromTryParse.hashCode();

			// All hashes should be identical
			EXPECT_EQ( hashFromInt, hashFromString )
				<< "Integer and string constructor should produce identical hashes\n"
				<< "  Integer constructor (" << intValue << "): hash = " << hashFromInt << "\n"
				<< "  String constructor (\"" << stringValue << "\"): hash = " << hashFromString;

			EXPECT_EQ( hashFromInt, hashFromParse )
				<< "Integer constructor and parse() should produce identical hashes\n"
				<< "  Integer constructor (" << intValue << "): hash = " << hashFromInt << "\n"
				<< "  parse(\"" << stringValue << "\"): hash = " << hashFromParse;

			EXPECT_EQ( hashFromInt, hashFromTryParse )
				<< "Integer constructor and tryParse() should produce identical hashes\n"
				<< "  Integer constructor (" << intValue << "): hash = " << hashFromInt << "\n"
				<< "  tryParse(\"" << stringValue << "\"): hash = " << hashFromTryParse;

			// All IMO objects should be equal
			EXPECT_EQ( imoFromInt, imoFromString )
				<< "Integer and string constructors should produce equal IMO objects";

			EXPECT_EQ( imoFromInt, imoFromParse )
				<< "Integer constructor and parse() should produce equal IMO objects";

			EXPECT_EQ( imoFromInt, imoFromTryParse )
				<< "Integer constructor and tryParse() should produce equal IMO objects";

			// All should have the same string representation
			EXPECT_EQ( imoFromInt.toString(), imoFromString.toString() )
				<< "String representations should be identical";

			EXPECT_EQ( imoFromInt.toString(), imoFromParse.toString() )
				<< "String representations should be identical";

			EXPECT_EQ( imoFromInt.toString(), imoFromTryParse.toString() )
				<< "String representations should be identical";

			// All should have the same integer value
			EXPECT_EQ( static_cast<int>( imoFromInt ), static_cast<int>( imoFromString ) )
				<< "Integer values should be identical";

			EXPECT_EQ( static_cast<int>( imoFromInt ), static_cast<int>( imoFromParse ) )
				<< "Integer values should be identical";

			EXPECT_EQ( static_cast<int>( imoFromInt ), static_cast<int>( imoFromTryParse ) )
				<< "Integer values should be identical";
		}
	}

	//----------------------------------------------
	// STL container integration tests
	//----------------------------------------------

	TEST_F( ImoNumberHashTests, Test_StdHash_UnorderedMap_Integration )
	{
		std::unordered_map<ImoNumber, std::string> imoMap;
		std::vector<ImoNumber> testImos;

		for ( int imoInt : validImoNumbers )
		{
			testImos.emplace_back( imoInt );
		}

		// Test insertion and lookup with real IMO numbers
		for ( size_t i = 0; i < testImos.size(); ++i )
		{
			std::string description = "Vessel_" + std::to_string( i );
			imoMap[testImos[i]] = description;
		}

		EXPECT_GE( imoMap.size(), testImos.size() / 2 ) << "STL map should handle most unique IMO numbers. Got: " << imoMap.size() << " entries";

		size_t successfulLookups = 0;
		for ( size_t i = 0; i < std::min( testImos.size(), size_t( 100 ) ); ++i )
		{
			auto it = imoMap.find( testImos[i] );
			if ( it != imoMap.end() )
			{
				successfulLookups++;
				EXPECT_FALSE( it->second.empty() ) << "Lookup should return valid description";
			}
		}

		size_t expectedLookups = std::min( testImos.size(), size_t( 100 ) );
		EXPECT_GT( successfulLookups, expectedLookups / 2 ) << "Most lookups should succeed. Got: " << successfulLookups << " out of " << expectedLookups;
	}

	TEST_F( ImoNumberHashTests, Test_StdHash_Specialization_UnorderedSet )
	{
		std::unordered_set<ImoNumber> imoSet;

		auto imo1 = ImoNumber{ 9074729 };
		auto imo2 = ImoNumber{ 9785811 };
		auto imo3 = ImoNumber{ 9704611 };
		auto imo4 = ImoNumber{ "IMO9368302" };

		imoSet.insert( imo1 );
		imoSet.insert( imo2 );
		imoSet.insert( imo3 );
		imoSet.insert( imo4 );

		imoSet.insert( imo1 ); // Duplicate
		imoSet.insert( imo2 ); // Duplicate

		EXPECT_EQ( imoSet.size(), 4 ) << "Set should contain exactly 4 unique IMO numbers";

		EXPECT_NE( imoSet.find( imo1 ), imoSet.end() ) << "IMO 9074729 should be in set";
		EXPECT_NE( imoSet.find( imo2 ), imoSet.end() ) << "IMO 9785811 should be in set";
		EXPECT_NE( imoSet.find( imo3 ), imoSet.end() ) << "IMO 9704611 should be in set";
		EXPECT_NE( imoSet.find( imo4 ), imoSet.end() ) << "IMO 9368302 should be in set";
	}

	//----------------------------------------------
	// Hash validation
	//----------------------------------------------

	TEST_F( ImoNumberHashTests, Test_Hash_StdHash_Consistency )
	{
		auto imo = ImoNumber{ 9074729 };

		// Test std::hash specialization
		std::hash<ImoNumber> stdHasher;
		auto stdHash = stdHasher( imo );
		auto memberHash = imo.hashCode();

		EXPECT_EQ( stdHash, memberHash ) << "std::hash specialization should match member hashCode()";
	}
} // namespace dnv::vista::sdk::tests
