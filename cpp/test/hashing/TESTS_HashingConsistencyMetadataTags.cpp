/**
 * @file TESTS_HashingConsistencyMetadataTags.cpp
 * @brief Hash consistency validation suite for MetadataTag class
 * @details This test suite validates hash function behavior, distribution quality, and consistency
 *          across all MetadataTag operations including copy/move semantics, tag creation,
 *          and STL container integration with maritime metadata tags.
 */

#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <gtest/gtest.h>

#include <dnv/vista/sdk/MetadataTag.h>
#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/VISVersion.h>

namespace dnv::vista::sdk::tests
{
	//========================================================================
	// MetadataTag hashing consistency, uniqueness and validation tests
	//========================================================================

	//----------------------------------------------
	// Dataset
	//----------------------------------------------

	namespace
	{
		std::vector<std::pair<CodebookName, std::string>> metadataTestData()
		{
			return {
				// Common quantity types
				{ CodebookName::Quantity, "temperature" },
				{ CodebookName::Quantity, "pressure" },
				{ CodebookName::Quantity, "flow" },
				{ CodebookName::Quantity, "level" },
				{ CodebookName::Quantity, "speed" },
				{ CodebookName::Quantity, "power" },
				{ CodebookName::Quantity, "voltage" },
				{ CodebookName::Quantity, "current" },

				// Position types
				{ CodebookName::Position, "inlet" },
				{ CodebookName::Position, "outlet" },
				{ CodebookName::Position, "centre" },
				{ CodebookName::Position, "top" },
				{ CodebookName::Position, "bottom" },

				// Content types
				{ CodebookName::Content, "fuel.oil" },
				{ CodebookName::Content, "cooling.water" },
				{ CodebookName::Content, "exhaust.gas" },
				{ CodebookName::Content, "lubricating.oil" },
				{ CodebookName::Content, "starting.air" },
				{ CodebookName::Content, "fresh.water" },

				// Detail types
				{ CodebookName::Detail, "setpoint" },
				{ CodebookName::Detail, "actual" },
				{ CodebookName::Detail, "maximum" },
				{ CodebookName::Detail, "minimum" },
				{ CodebookName::Detail, "average" },

				// State types
				{ CodebookName::State, "running" },
				{ CodebookName::State, "stopped" },
				{ CodebookName::State, "standby" },
				{ CodebookName::State, "alarm" },
				{ CodebookName::State, "fault" } };
		}
	} // namespace

	//----------------------------------------------
	// Test fixture
	//----------------------------------------------

	class MetadataTagHashTests : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			const auto& vis = VIS::instance();
			codebooks = &vis.codebooks( VisVersion::v3_9a );
			testTagData = metadataTestData();
		}

		const Codebooks* codebooks;
		std::vector<std::pair<CodebookName, std::string>> testTagData;

		const Codebook* getCodebookForName( CodebookName name ) const
		{
			return &codebooks->codebook( name );
		}
	};

	//----------------------------------------------
	// Hash uniqueness validation
	//----------------------------------------------

	TEST_F( MetadataTagHashTests, Test_Hash_Uniqueness_DifferentTags )
	{
		auto tag1 = getCodebookForName( CodebookName::Quantity )->createTag( "temperature" );
		auto tag2 = getCodebookForName( CodebookName::Quantity )->createTag( "pressure" );
		auto tag3 = getCodebookForName( CodebookName::Position )->createTag( "inlet" );

		auto hash1 = tag1.hashCode();
		auto hash2 = tag2.hashCode();
		auto hash3 = tag3.hashCode();

		EXPECT_NE( hash1, hash2 ) << "Different quantity tags should have different hashes";
		EXPECT_NE( hash1, hash3 ) << "Different tag types should have different hashes";
		EXPECT_NE( hash2, hash3 ) << "Different tag types should have different hashes";
	}

	TEST_F( MetadataTagHashTests, Test_Hash_Uniqueness_SameNameDifferentValues )
	{
		auto tag1 = getCodebookForName( CodebookName::Quantity )->createTag( "temperature" );
		auto tag2 = getCodebookForName( CodebookName::Quantity )->createTag( "pressure" );
		auto tag3 = getCodebookForName( CodebookName::Quantity )->createTag( "flow" );

		auto hash1 = tag1.hashCode();
		auto hash2 = tag2.hashCode();
		auto hash3 = tag3.hashCode();

		EXPECT_NE( hash1, hash2 ) << "Same tag name with different values should have different hashes";
		EXPECT_NE( hash1, hash3 ) << "Same tag name with different values should have different hashes";
		EXPECT_NE( hash2, hash3 ) << "Same tag name with different values should have different hashes";
	}

	TEST_F( MetadataTagHashTests, Test_Hash_Uniqueness_AllTagTypes )
	{
		std::unordered_set<std::size_t> uniqueHashes;
		std::vector<MetadataTag> allTags;

		// Create all test tags
		for ( const auto& [name, value] : testTagData )
		{
			const Codebook* cb = getCodebookForName( name );
			auto tag = cb->createTag( value );
			allTags.push_back( tag );

			auto hash = tag.hashCode();
			uniqueHashes.insert( hash );
		}

		// Test hash uniqueness across the entire dataset
		std::cout << "\nMetadataTag Hash Uniqueness Statistics:" << std::endl;
		std::cout << "  Total tags: " << allTags.size() << std::endl;
		std::cout << "  Unique hashes: " << uniqueHashes.size() << std::endl;
		std::cout << "  Hash collisions: " << ( allTags.size() - uniqueHashes.size() ) << std::endl;

		double uniquenessRatio = static_cast<double>( uniqueHashes.size() ) / static_cast<double>( allTags.size() );
		std::cout << "  Uniqueness ratio: " << std::fixed << std::setprecision( 1 ) << ( uniquenessRatio * 100.0 ) << "%" << std::endl;

		EXPECT_EQ( uniqueHashes.size(), allTags.size() )
			<< "Each unique MetadataTag should have a unique hash. "
			<< "Expected " << allTags.size() << " unique hashes, got " << uniqueHashes.size();

		EXPECT_GE( uniquenessRatio, 1.0 )
			<< "Hash uniqueness ratio should be 100% for unique dataset. "
			<< "Achieved: " << ( uniquenessRatio * 100.0 ) << "%";
	}

	//----------------------------------------------
	// Hash consistency and immutability tests
	//----------------------------------------------

	TEST_F( MetadataTagHashTests, Test_Hash_Consistency_CopyConstructor )
	{
		auto original = getCodebookForName( CodebookName::Quantity )->createTag( "temperature" );

		MetadataTag copy( original );
		auto originalHash = original.hashCode();
		auto copyHash = copy.hashCode();

		EXPECT_EQ( originalHash, copyHash ) << "Copy constructor should preserve hash code";
		EXPECT_EQ( original.name(), copy.name() ) << "Copy constructor should preserve name";
		EXPECT_EQ( original.value(), copy.value() ) << "Copy constructor should preserve value";
		EXPECT_EQ( original.isCustom(), copy.isCustom() ) << "Copy constructor should preserve custom flag";
	}

	TEST_F( MetadataTagHashTests, Test_Hash_Consistency_MoveConstructor )
	{
		auto original = getCodebookForName( CodebookName::Position )->createTag( "inlet" );
		auto originalHash = original.hashCode();
		auto originalName = original.name();
		auto originalValue = std::string{ original.value() };
		auto originalCustom = original.isCustom();

		MetadataTag moved( std::move( original ) );
		auto movedHash = moved.hashCode();

		EXPECT_EQ( originalHash, movedHash ) << "Move constructor should preserve hash code";
		EXPECT_EQ( originalName, moved.name() ) << "Move constructor should preserve name";
		EXPECT_EQ( originalValue, moved.value() ) << "Move constructor should preserve value";
		EXPECT_EQ( originalCustom, moved.isCustom() ) << "Move constructor should preserve custom flag";
	}

	TEST_F( MetadataTagHashTests, Test_Hash_Consistency_CopyAssignment )
	{
		auto original = getCodebookForName( CodebookName::Content )->createTag( "fuel.oil" );
		auto target = getCodebookForName( CodebookName::Detail )->createTag( "setpoint" ); // Different tag initially
		auto originalHash = original.hashCode();

		target = original;
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Copy assignment should preserve hash code";
		EXPECT_EQ( original.name(), target.name() ) << "Copy assignment should preserve name";
		EXPECT_EQ( original.value(), target.value() ) << "Copy assignment should preserve value";
		EXPECT_EQ( original.isCustom(), target.isCustom() ) << "Copy assignment should preserve custom flag";
	}

	TEST_F( MetadataTagHashTests, Test_Hash_Consistency_MoveAssignment )
	{
		auto original = getCodebookForName( CodebookName::State )->createTag( "running" );
		auto target = getCodebookForName( CodebookName::Quantity )->createTag( "pressure" ); // Different tag initially
		auto originalHash = original.hashCode();
		auto originalName = original.name();
		auto originalValue = std::string{ original.value() };
		auto originalCustom = original.isCustom();

		target = std::move( original );
		auto targetHash = target.hashCode();

		EXPECT_EQ( originalHash, targetHash ) << "Move assignment should preserve hash code";
		EXPECT_EQ( originalName, target.name() ) << "Move assignment should preserve name";
		EXPECT_EQ( originalValue, target.value() ) << "Move assignment should preserve value";
		EXPECT_EQ( originalCustom, target.isCustom() ) << "Move assignment should preserve custom flag";
	}

	TEST_F( MetadataTagHashTests, Test_Hash_Consistency_RepeatAccess )
	{
		auto tag = getCodebookForName( CodebookName::Quantity )->createTag( "temperature" );

		auto hash1 = tag.hashCode();
		auto hash2 = tag.hashCode();
		auto hash3 = tag.hashCode();

		EXPECT_EQ( hash1, hash2 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash2, hash3 ) << "Multiple hash access should be consistent";
		EXPECT_EQ( hash1, hash3 ) << "Multiple hash access should be consistent";
	}

	TEST_F( MetadataTagHashTests, Test_Hash_Consistency_CustomVsStandard )
	{
		// Test hash differences between standard and custom tags
		auto standardTag = getCodebookForName( CodebookName::Quantity )->createTag( "temperature" );
		auto customTag = getCodebookForName( CodebookName::Quantity )->createTag( "~custom_temperature" );

		auto standardHash = standardTag.hashCode();
		auto customHash = customTag.hashCode();

		EXPECT_NE( standardHash, customHash ) << "Standard and custom tags with same name should have different hashes";
		EXPECT_FALSE( standardTag.isCustom() ) << "Standard tag should not be marked as custom";
		EXPECT_TRUE( customTag.isCustom() ) << "Custom tag should be marked as custom";
	}

	//----------------------------------------------
	// STL container integration tests
	//----------------------------------------------

	TEST_F( MetadataTagHashTests, Test_StdHash_UnorderedMap_Integration )
	{
		std::unordered_map<MetadataTag, std::string> tagMap;
		std::unordered_map<std::size_t, MetadataTag> hashMap;

		// Test insertion and lookup with various tag types
		const std::vector<std::tuple<CodebookName, std::string, std::string>> testPairs = {
			{ CodebookName::Quantity, "temperature", "Temperature Sensor" },
			{ CodebookName::Position, "inlet", "Inlet Position" },
			{ CodebookName::Content, "fuel.oil", "Fuel Oil Content" },
			{ CodebookName::Detail, "setpoint", "Setpoint Detail" },
			{ CodebookName::State, "running", "Running State" } };

		for ( const auto& [name, value, description] : testPairs )
		{
			const Codebook* cb = getCodebookForName( name );
			auto tag = cb->createTag( value );
			auto hash = tag.hashCode();

			// Test std::unordered_map with MetadataTag as key
			tagMap[tag] = description;
			EXPECT_EQ( tagMap[tag], description ) << "Tag should be retrievable from unordered_map";

			// Test std::unordered_map with hash as key
			hashMap.emplace( hash, tag );
			auto it = hashMap.find( hash );
			ASSERT_NE( it, hashMap.end() ) << "Hash should be findable in unordered_map";
			EXPECT_EQ( it->second.name(), tag.name() ) << "Hash should map back to correct tag";
			EXPECT_EQ( it->second.value(), tag.value() ) << "Hash should map back to correct tag value";
		}

		EXPECT_EQ( tagMap.size(), testPairs.size() ) << "All tags should be stored uniquely";
		EXPECT_EQ( hashMap.size(), testPairs.size() ) << "All hashes should be stored uniquely";
	}

	TEST_F( MetadataTagHashTests, Test_StdHash_Specialization_UnorderedSet )
	{
		std::unordered_set<MetadataTag> tagSet;

		// Add all test tags to unordered_set
		for ( const auto& [name, value] : testTagData )
		{
			const Codebook* cb = getCodebookForName( name );
			auto tag = cb->createTag( value );
			tagSet.insert( tag );
		}

		std::cout << "Successfully stored " << tagSet.size() << " unique metadata tags in std::unordered_set" << std::endl;

		EXPECT_EQ( tagSet.size(), testTagData.size() ) << "All tags should be stored uniquely in unordered_set";

		// Verify all tags can be found
		for ( const auto& [name, value] : testTagData )
		{
			const Codebook* cb = getCodebookForName( name );
			auto tag = cb->createTag( value );
			EXPECT_NE( tagSet.find( tag ), tagSet.end() ) << "Tag should be findable in unordered_set";
		}
	}

	TEST_F( MetadataTagHashTests, Test_Hash_StdHash_Consistency )
	{
		auto tag = getCodebookForName( CodebookName::Quantity )->createTag( "temperature" );

		auto internalHash = tag.hashCode();
		auto stdHash = std::hash<MetadataTag>{}( tag );

		EXPECT_EQ( internalHash, stdHash ) << "std::hash should return same value as hashCode()";
	}
} // namespace dnv::vista::sdk::tests
