/**
 * @file TESTS_HashingConsistencyParsingErrors.cpp
 * @brief Hash consistency validation suite for ParsingErrors class
 * @details This test suite validates hash function behavior, distribution quality, and consistency
 *          across all ParsingErrors operations including copy/move semantics, error entry management,
 *          and STL container integration with maritime parsing error collections.
 */

#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <gtest/gtest.h>

#include <internal/parsing/LocalIdParsingErrorBuilder.h>
#include <internal/parsing/LocationParsingErrorBuilder.h>
#include <dnv/vista/sdk/ParsingErrors.h>

namespace dnv::vista::sdk::tests
{
	//========================================================================
	// ParsingErrors hashing consistency, uniqueness and validation tests
	//========================================================================

	//----------------------------------------------
	// Dataset
	//----------------------------------------------

	namespace
	{
		std::vector<std::pair<std::string, std::string>> errorTestData()
		{
			return {
				{ "ValidationError", "Invalid location code 'X' at position 2" },
				{ "FormatError", "Missing separator in location string" },
				{ "SemanticError", "Inconsistent component combination" },
				{ "NamingRuleError", "Invalid naming rule specification" },
				{ "VisVersionError", "Unsupported VIS version format" },
				{ "PrimaryItemError", "Invalid primary item structure" },
				{ "SecondaryItemError", "Invalid secondary item specification" },
				{ "EmptyStateError", "Empty input provided" },
				{ "FormattingError", "Incorrect format specification" },
				{ "CompletenessError", "Incomplete data structure" },
				{ "NamingEntityError", "Invalid naming entity" },
				{ "IMONumberError", "Invalid IMO number format" },
				{ "InvalidCodeError", "Unrecognized code in input" },
				{ "InvalidOrderError", "Incorrect component sequence" },
				{ "NullOrWhiteSpaceError", "Empty or whitespace-only input" },
				{ "StructuralError", "Invalid structural composition" },
				{ "SyntaxError", "Syntax parsing failure" },
				{ "LexicalError", "Lexical analysis error" },
				{ "PathError", "Invalid path specification" },
				{ "NodeError", "Node structure validation failed" } };
		}
	} // namespace

	//----------------------------------------------
	// Test fixture
	//----------------------------------------------

	class ParsingErrorsHashTests : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			testErrorData = errorTestData();
		}

		std::vector<std::pair<std::string, std::string>> testErrorData;

		// Helper methods to create different types of ParsingErrors for testing
		ParsingErrors createLocalIdErrors( const std::string& message ) const
		{
			auto builder = internal::LocalIdParsingErrorBuilder();
			builder.addError( internal::LocalIdParsingState::PrimaryItem, message );
			return builder.build();
		}

		ParsingErrors createLocationErrors( const std::string& message ) const
		{
			auto builder = internal::LocationParsingErrorBuilder();
			builder.addError( internal::LocationValidationResult::InvalidCode, message );
			return builder.build();
		}

		ParsingErrors createMultipleLocalIdErrors() const
		{
			auto builder = internal::LocalIdParsingErrorBuilder();
			builder.addError( internal::LocalIdParsingState::PrimaryItem, "Invalid primary item" );
			builder.addError( internal::LocalIdParsingState::VisVersion, "Invalid VIS version" );
			builder.addError( internal::LocalIdParsingState::MetaQuantity, "Invalid quantity metadata" );
			return builder.build();
		}

		ParsingErrors createMultipleLocationErrors() const
		{
			auto builder = internal::LocationParsingErrorBuilder();
			builder.addError( internal::LocationValidationResult::InvalidCode, "Invalid location code" );
			builder.addError( internal::LocationValidationResult::InvalidOrder, "Invalid component order" );
			return builder.build();
		}
	};

	//----------------------------------------------
	// Hash uniqueness validation
	//----------------------------------------------

	TEST_F( ParsingErrorsHashTests, Test_Hash_Uniqueness_EmptyVsNonEmpty )
	{
		auto emptyErrors = ParsingErrors::empty();
		auto emptyErrors2 = ParsingErrors::empty();

		// Test that empty ParsingErrors instances have identical hashes
		auto emptyHash1 = emptyErrors.hashCode();
		auto emptyHash2 = emptyErrors2.hashCode();

		EXPECT_EQ( emptyHash1, emptyHash2 ) << "Empty ParsingErrors instances should have identical hashes";
		EXPECT_EQ( emptyErrors, emptyErrors2 ) << "Empty ParsingErrors instances should be equal";

		// Test with non-empty ParsingErrors using available builders
		auto localIdErrorBuilder = internal::LocalIdParsingErrorBuilder();
		localIdErrorBuilder.addError( internal::LocalIdParsingState::PrimaryItem, "Invalid primary item format" );
		localIdErrorBuilder.addError( internal::LocalIdParsingState::VisVersion, "Unsupported VIS version" );
		auto localIdErrors = localIdErrorBuilder.build();

		auto locationErrorBuilder = internal::LocationParsingErrorBuilder();
		locationErrorBuilder.addError( internal::LocationValidationResult::InvalidCode, "Invalid location code" );
		locationErrorBuilder.addError( internal::LocationValidationResult::InvalidOrder, "Invalid component order" );
		auto locationErrors = locationErrorBuilder.build();

		auto localIdHash = localIdErrors.hashCode();
		auto locationHash = locationErrors.hashCode();

		EXPECT_NE( emptyHash1, localIdHash ) << "Non-empty ParsingErrors should have different hash than empty";
		EXPECT_NE( emptyHash1, locationHash ) << "Non-empty ParsingErrors should have different hash than empty";
		EXPECT_NE( localIdHash, locationHash ) << "Different error collections should have different hashes";
	}

	TEST_F( ParsingErrorsHashTests, Test_Hash_Uniqueness_AllErrorTypes )
	{
		std::unordered_set<std::size_t> uniqueHashes;
		std::vector<ParsingErrors> allErrors;

		// Empty errors
		auto emptyErrors = ParsingErrors::empty();
		allErrors.push_back( emptyErrors );
		uniqueHashes.insert( emptyErrors.hashCode() );

		// LocalId parsing errors
		for ( const auto& [errorType, message] : testErrorData )
		{
			auto localIdBuilder = internal::LocalIdParsingErrorBuilder();
			localIdBuilder.addError( internal::LocalIdParsingState::PrimaryItem, message );
			auto errors = localIdBuilder.build();
			allErrors.push_back( errors );
			uniqueHashes.insert( errors.hashCode() );
		}

		// Location parsing errors
		for ( const auto& [errorType, message] : testErrorData )
		{
			auto locationBuilder = internal::LocationParsingErrorBuilder();
			locationBuilder.addError( internal::LocationValidationResult::InvalidCode, message );
			auto errors = locationBuilder.build();
			allErrors.push_back( errors );
			uniqueHashes.insert( errors.hashCode() );
		}

		// Test hash uniqueness across the entire dataset
		std::cout << "\nParsingErrors Hash Uniqueness Statistics:" << std::endl;
		std::cout << "  Total error collections: " << allErrors.size() << std::endl;
		std::cout << "  Unique hashes: " << uniqueHashes.size() << std::endl;
		std::cout << "  Hash collisions: " << ( allErrors.size() - uniqueHashes.size() ) << std::endl;

		double uniquenessRatio = static_cast<double>( uniqueHashes.size() ) / static_cast<double>( allErrors.size() );
		std::cout << "  Uniqueness ratio: " << std::fixed << std::setprecision( 1 ) << ( uniquenessRatio * 100.0 ) << "%" << std::endl;

		EXPECT_EQ( uniqueHashes.size(), allErrors.size() )
			<< "Each unique ParsingErrors collection should have a unique hash. "
			<< "Expected " << allErrors.size() << " unique hashes, got " << uniqueHashes.size();

		EXPECT_GE( uniquenessRatio, 1.0 )
			<< "Hash uniqueness ratio should be 100% for unique dataset. "
			<< "Achieved: " << ( uniquenessRatio * 100.0 ) << "%";
	}

	//----------------------------------------------
	// Hash consistency and immutability tests
	//----------------------------------------------

	TEST_F( ParsingErrorsHashTests, Test_Hash_Consistency_CopyConstructor )
	{
		auto original = createMultipleLocalIdErrors();
		auto originalHash = original.hashCode();
		auto originalCount = original.count();

		ParsingErrors copy( original );
		auto copyHash = copy.hashCode();

		EXPECT_EQ( originalHash, copyHash ) << "Copy constructor should preserve hash code";
		EXPECT_EQ( original, copy ) << "Copy should be equal to original";
		EXPECT_EQ( originalCount, copy.count() ) << "Copy should have same error count";
		EXPECT_TRUE( copy.hasErrors() ) << "Copy should have errors";
	}

	TEST_F( ParsingErrorsHashTests, Test_Hash_Consistency_MoveConstructor )
	{
		auto original = createMultipleLocationErrors();
		auto originalHash = original.hashCode();
		auto originalCount = original.count();

		ParsingErrors moved( std::move( original ) );
		auto movedHash = moved.hashCode();

		EXPECT_EQ( originalHash, movedHash ) << "Move constructor should preserve hash code";
		EXPECT_EQ( originalCount, moved.count() ) << "Move constructor should preserve error count";
		EXPECT_TRUE( moved.hasErrors() ) << "Moved object should have errors";
	}

	TEST_F( ParsingErrorsHashTests, Test_Hash_Consistency_CopyAssignment )
	{
		auto original = createLocalIdErrors( "Test validation error" );
		auto target = createLocationErrors( "Different error initially" );
		auto originalHash = original.hashCode();
		auto originalCount = original.count();

		target = original;
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Copy assignment should preserve hash code";
		EXPECT_EQ( original, target ) << "Copy assignment should make objects equal";
		EXPECT_EQ( originalCount, target.count() ) << "Copy assignment should preserve error count";
		EXPECT_TRUE( target.hasErrors() ) << "Target should have errors after assignment";
	}

	TEST_F( ParsingErrorsHashTests, Test_Hash_Consistency_MoveAssignment )
	{
		auto original = createMultipleLocalIdErrors();
		auto target = createLocationErrors( "Initial target error" );
		auto originalHash = original.hashCode();
		auto originalCount = original.count();

		target = std::move( original );
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Move assignment should preserve hash code";
		EXPECT_EQ( originalCount, target.count() ) << "Move assignment should preserve error count";
		EXPECT_TRUE( target.hasErrors() ) << "Target should have errors after move assignment";
	}

	TEST_F( ParsingErrorsHashTests, Test_Hash_Consistency_RepeatAccess )
	{
		auto parsingErrors = createMultipleLocationErrors();

		// Multiple hash code access should return identical values
		auto hash1 = parsingErrors.hashCode();
		auto hash2 = parsingErrors.hashCode();
		auto hash3 = parsingErrors.hashCode();

		EXPECT_EQ( hash1, hash2 ) << "Repeated hash access should return identical values";
		EXPECT_EQ( hash2, hash3 ) << "Repeated hash access should return identical values";
		EXPECT_EQ( hash1, hash3 ) << "Repeated hash access should return identical values";
		EXPECT_TRUE( parsingErrors.hasErrors() ) << "ParsingErrors should have actual error content";
	}

	//----------------------------------------------
	// Enumerator consistency tests
	//----------------------------------------------

	TEST_F( ParsingErrorsHashTests, Test_Hash_Consistency_EnumeratorUsage )
	{
		auto parsingErrors = createMultipleLocalIdErrors();
		auto originalHash = parsingErrors.hashCode();
		auto expectedCount = parsingErrors.count();

		// Using enumerator should not affect hash
		auto enumerator = parsingErrors.enumerator();
		auto hashAfterEnumerator = parsingErrors.hashCode();

		EXPECT_EQ( originalHash, hashAfterEnumerator ) << "Using enumerator should not affect hash";

		// Test enumerator on non-empty collection
		size_t enumeratedCount = 0;
		while ( enumerator.next() )
		{
			auto current = enumerator.current();
			EXPECT_FALSE( current.type.empty() ) << "Error entry should have non-empty type";
			EXPECT_FALSE( current.message.empty() ) << "Error entry should have non-empty message";
			enumeratedCount++;
		}

		EXPECT_EQ( expectedCount, enumeratedCount ) << "Enumerator should visit all error entries";
		EXPECT_GT( enumeratedCount, 0 ) << "Should have enumerated actual errors";

		auto hashAfterEnumeratorUse = parsingErrors.hashCode();
		EXPECT_EQ( originalHash, hashAfterEnumeratorUse ) << "Hash should remain consistent after enumerator use";
	}

	//----------------------------------------------
	// State inspection consistency tests
	//----------------------------------------------

	TEST_F( ParsingErrorsHashTests, Test_Hash_Consistency_StateInspection )
	{
		auto parsingErrors = createMultipleLocationErrors();
		auto originalHash = parsingErrors.hashCode();

		// State inspection methods should not affect hash
		bool hasErrors = parsingErrors.hasErrors();
		auto hashAfterHasErrors = parsingErrors.hashCode();

		size_t count = parsingErrors.count();
		auto hashAfterCount = parsingErrors.hashCode();

		bool hasType = parsingErrors.hasErrorType( "InvalidCode" );
		auto hashAfterHasType = parsingErrors.hashCode();

		std::string stringRep = parsingErrors.toString();
		auto hashAfterToString = parsingErrors.hashCode();

		EXPECT_EQ( originalHash, hashAfterHasErrors ) << "hasErrors() should not affect hash";
		EXPECT_EQ( originalHash, hashAfterCount ) << "count() should not affect hash";
		EXPECT_EQ( originalHash, hashAfterHasType ) << "hasErrorType() should not affect hash";
		EXPECT_EQ( originalHash, hashAfterToString ) << "toString() should not affect hash";

		// Verify expected values for non-empty ParsingErrors
		EXPECT_TRUE( hasErrors ) << "ParsingErrors with errors should return true for hasErrors()";
		EXPECT_GT( count, 0 ) << "ParsingErrors with errors should have count > 0";
		EXPECT_TRUE( hasType ) << "ParsingErrors should contain InvalidCode error type";
		EXPECT_FALSE( stringRep.empty() ) << "String representation should not be empty";
		EXPECT_NE( stringRep.find( "InvalidCode" ), std::string::npos ) << "String should contain error type";
	}

	//----------------------------------------------
	// STL container integration tests
	//----------------------------------------------

	TEST_F( ParsingErrorsHashTests, Test_StdHash_UnorderedMap_Integration )
	{
		std::unordered_map<ParsingErrors, std::string> errorMap;

		// Create different types of ParsingErrors
		auto localIdErrors1 = createLocalIdErrors( "First validation error" );
		auto localIdErrors2 = createLocalIdErrors( "Second validation error" );
		auto locationErrors = createLocationErrors( "Location parsing error" );
		auto multipleErrors = createMultipleLocalIdErrors();

		// Insert different ParsingErrors instances
		errorMap[localIdErrors1] = "local_id_error_1";
		errorMap[localIdErrors2] = "local_id_error_2";
		errorMap[locationErrors] = "location_error";
		errorMap[multipleErrors] = "multiple_errors";

		// Should have separate entries for different ParsingErrors
		EXPECT_EQ( errorMap.size(), 4 ) << "Different ParsingErrors should map to different keys";
		EXPECT_EQ( errorMap[localIdErrors1], "local_id_error_1" ) << "First LocalId error should be retrievable";
		EXPECT_EQ( errorMap[locationErrors], "location_error" ) << "Location error should be retrievable";

		// Test that identical ParsingErrors map to same key
		auto identicalErrors = createLocalIdErrors( "First validation error" );
		errorMap[identicalErrors] = "overwritten_value";
		EXPECT_EQ( errorMap.size(), 4 ) << "Identical ParsingErrors should not add new entry";
		EXPECT_EQ( errorMap[localIdErrors1], "overwritten_value" ) << "Identical ParsingErrors should overwrite existing";

		// Verify std::hash works correctly
		std::hash<ParsingErrors> hasher;
		auto hashValue1 = hasher( localIdErrors1 );
		auto hashValue2 = hasher( identicalErrors );
		auto hashValue3 = localIdErrors1.hashCode();

		EXPECT_EQ( hashValue1, hashValue2 ) << "std::hash should return same value for equal ParsingErrors";
		EXPECT_EQ( hashValue1, hashValue3 ) << "std::hash should match hashCode() method";
	}

	TEST_F( ParsingErrorsHashTests, Test_StdHash_Specialization_UnorderedSet )
	{
		std::unordered_set<ParsingErrors> errorSet;

		// Add different types of ParsingErrors to set
		for ( const auto& [errorType, message] : testErrorData )
		{
			auto localIdError = createLocalIdErrors( message );
			auto locationError = createLocationErrors( message );
			errorSet.insert( localIdError );
			errorSet.insert( locationError );
		}

		// Add some duplicate ParsingErrors (should be deduplicated)
		auto duplicateError1 = createLocalIdErrors( "Duplicate error message" );
		auto duplicateError2 = createLocalIdErrors( "Duplicate error message" );
		errorSet.insert( duplicateError1 );
		errorSet.insert( duplicateError2 );

		size_t expectedUniqueErrors = testErrorData.size() * 2 + 1; // LocalId + Location errors per message + 1 duplicate
		EXPECT_EQ( errorSet.size(), expectedUniqueErrors ) << "Set should contain all unique ParsingErrors";

		// Test find operations
		auto testError = createLocalIdErrors( testErrorData[0].second );
		auto found = errorSet.find( testError );
		EXPECT_NE( found, errorSet.end() ) << "Should find ParsingErrors in set";

		// Test that duplicates are properly deduplicated
		auto duplicateFound = errorSet.find( duplicateError1 );
		EXPECT_NE( duplicateFound, errorSet.end() ) << "Should find duplicate ParsingErrors in set";

		std::cout << "Successfully stored " << errorSet.size() << " unique ParsingErrors in std::unordered_set" << std::endl;
	}

	//----------------------------------------------
	// Hash validation
	//----------------------------------------------

	TEST_F( ParsingErrorsHashTests, Test_Hash_StdHash_Consistency )
	{
		// Test with different types of ParsingErrors
		std::vector<ParsingErrors> testErrors = {
			ParsingErrors::empty(),
			createLocalIdErrors( "Test validation error" ),
			createLocationErrors( "Test location error" ),
			createMultipleLocalIdErrors(),
			createMultipleLocationErrors() };

		std::hash<ParsingErrors> stdHasher;

		for ( const auto& parsingErrors : testErrors )
		{
			// Test that std::hash and hashCode() return the same value
			auto stdHashValue = stdHasher( parsingErrors );
			auto memberHashValue = parsingErrors.hashCode();

			EXPECT_EQ( stdHashValue, memberHashValue )
				<< "std::hash specialization should return same value as hashCode() method";

			// Test multiple calls consistency
			auto stdHashValue2 = stdHasher( parsingErrors );
			auto memberHashValue2 = parsingErrors.hashCode();

			EXPECT_EQ( stdHashValue, stdHashValue2 ) << "std::hash should be consistent across calls";
			EXPECT_EQ( memberHashValue, memberHashValue2 ) << "hashCode() should be consistent across calls";
		}
	}

	//----------------------------------------------
	// Error entry relationship tests
	//----------------------------------------------

	TEST_F( ParsingErrorsHashTests, Test_Hash_ErrorEntry_Structure )
	{
		// Test ErrorEntry structure and equality operators
		ParsingErrors::ErrorEntry entry1( std::string_view( "ValidationError" ), std::string_view( "Test message 1" ) );
		ParsingErrors::ErrorEntry entry2( std::string_view( "ValidationError" ), std::string_view( "Test message 1" ) );
		ParsingErrors::ErrorEntry entry3( std::string_view( "ValidationError" ), std::string_view( "Test message 2" ) );
		ParsingErrors::ErrorEntry entry4( std::string_view( "FormatError" ), std::string_view( "Test message 1" ) );

		EXPECT_EQ( entry1, entry2 ) << "Identical ErrorEntry instances should be equal";
		EXPECT_NE( entry1, entry3 ) << "ErrorEntry with different messages should not be equal";
		EXPECT_NE( entry1, entry4 ) << "ErrorEntry with different types should not be equal";

		EXPECT_FALSE( entry1 != entry2 ) << "Identical ErrorEntry instances should not be unequal";
		EXPECT_TRUE( entry1 != entry3 ) << "ErrorEntry with different messages should be unequal";
		EXPECT_TRUE( entry1 != entry4 ) << "ErrorEntry with different types should be unequal";
	}
} // namespace dnv::vista::sdk::tests
