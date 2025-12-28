/**
 * @file Tests_MetadataTagsQuery.cpp
 * @brief Unit tests for MetadataTagsQuery functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::test
{
    // Test basic empty query with allowOtherTags=true (default)
    TEST( MetadataTagsQuery, EmptyQuery_AllowOthers )
    {
        auto query = MetadataTagsQueryBuilder::empty().build();

        // Should match any LocalId
        auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-temperature" );
        ASSERT_TRUE( localId1.has_value() );
        EXPECT_TRUE( query.match( localId1.value() ) );

        auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-temperature/cnt-exhaust.gas" );
        ASSERT_TRUE( localId2.has_value() );
        EXPECT_TRUE( query.match( localId2.value() ) );
    }

    // Test single tag match - subset mode (allowOtherTags=true)
    TEST( MetadataTagsQuery, SingleTag_AllowOthers )
    {
        auto query = MetadataTagsQueryBuilder::empty()
                         .withTag( CodebookName::Content, "sea.water" )
                         .withAllowOtherTags( true )
                         .build();

        // Should match LocalId with cnt-sea.water (can have other tags)
        auto localId1 =
            LocalId::fromString( "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened" );
        ASSERT_TRUE( localId1.has_value() );
        EXPECT_TRUE( query.match( localId1.value() ) );

        // Should not match LocalId with different content
        auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-temperature/cnt-exhaust.gas" );
        ASSERT_TRUE( localId2.has_value() );
        EXPECT_FALSE( query.match( localId2.value() ) );

        // Should not match LocalId with no cnt tag
        auto localId3 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-temperature" );
        ASSERT_TRUE( localId3.has_value() );
        EXPECT_FALSE( query.match( localId3.value() ) );
    }

    // Test single tag match - exact mode (allowOtherTags=false)
    TEST( MetadataTagsQuery, SingleTag_NoAllowOthers )
    {
        auto query = MetadataTagsQueryBuilder::empty()
                         .withTag( CodebookName::Content, "exhaust.gas" )
                         .withAllowOtherTags( false )
                         .build();

        // Should match LocalId with ONLY cnt-exhaust.gas
        auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/cnt-exhaust.gas" );
        ASSERT_TRUE( localId1.has_value() );
        EXPECT_TRUE( query.match( localId1.value() ) );

        // Should not match LocalId with cnt-exhaust.gas + other tags
        auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-temperature/cnt-exhaust.gas" );
        ASSERT_TRUE( localId2.has_value() );
        EXPECT_FALSE( query.match( localId2.value() ) );
    }

    // Test multiple tags - subset mode
    TEST( MetadataTagsQuery, MultipleTags_AllowOthers )
    {
        auto query = MetadataTagsQueryBuilder::empty()
                         .withTag( CodebookName::Quantity, "temperature" )
                         .withTag( CodebookName::Content, "exhaust.gas" )
                         .withAllowOtherTags( true )
                         .build();

        // Should match LocalId with both tags (can have more)
        auto localId1 =
            LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
        ASSERT_TRUE( localId1.has_value() );
        EXPECT_TRUE( query.match( localId1.value() ) );

        // Should not match LocalId missing one tag
        auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-temperature" );
        ASSERT_TRUE( localId2.has_value() );
        EXPECT_FALSE( query.match( localId2.value() ) );

        // Should not match LocalId with different cnt value
        auto localId3 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-temperature/cnt-sea.water" );
        ASSERT_TRUE( localId3.has_value() );
        EXPECT_FALSE( query.match( localId3.value() ) );
    }

    // Test multiple tags - exact mode
    TEST( MetadataTagsQuery, MultipleTags_NoAllowOthers )
    {
        auto query = MetadataTagsQueryBuilder::empty()
                         .withTag( CodebookName::Quantity, "temperature" )
                         .withTag( CodebookName::Content, "exhaust.gas" )
                         .withAllowOtherTags( false )
                         .build();

        // Should match LocalId with EXACTLY these two tags
        auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-temperature/cnt-exhaust.gas" );
        ASSERT_TRUE( localId1.has_value() );
        EXPECT_TRUE( query.match( localId1.value() ) );

        // Should not match LocalId with additional tag
        auto localId2 =
            LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
        ASSERT_TRUE( localId2.has_value() );
        EXPECT_FALSE( query.match( localId2.value() ) );
    }

    // Test from() method - creates query from LocalId
    TEST( MetadataTagsQuery, FromLocalId_AllowOthers )
    {
        auto sourceLocalId = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-temperature/cnt-exhaust.gas" );
        ASSERT_TRUE( sourceLocalId.has_value() );

        auto query = MetadataTagsQueryBuilder::from( sourceLocalId.value(), true ).build();

        // Should match LocalId with same tags (can have more)
        EXPECT_TRUE( query.match( sourceLocalId.value() ) );

        auto localId2 =
            LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
        ASSERT_TRUE( localId2.has_value() );
        EXPECT_TRUE( query.match( localId2.value() ) );
    }

    // Test from() method - exact match
    TEST( MetadataTagsQuery, FromLocalId_NoAllowOthers )
    {
        auto sourceLocalId = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-temperature/cnt-exhaust.gas" );
        ASSERT_TRUE( sourceLocalId.has_value() );

        auto query = MetadataTagsQueryBuilder::from( sourceLocalId.value(), false ).build();

        // Should match LocalId with EXACTLY the same tags
        EXPECT_TRUE( query.match( sourceLocalId.value() ) );

        // Should not match with additional tags
        auto localId2 =
            LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
        ASSERT_TRUE( localId2.has_value() );
        EXPECT_FALSE( query.match( localId2.value() ) );
    }

    // Test optional handling pattern
    TEST( MetadataTagsQuery, OptionalHandling )
    {
        auto query = MetadataTagsQueryBuilder::empty().build();

        // Test with empty optional
        std::optional<LocalId> emptyLocalId;
        EXPECT_FALSE( emptyLocalId.has_value() ? query.match( *emptyLocalId ) : false );

        // Test with valid optional
        auto validLocalId = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-power" );
        EXPECT_TRUE( validLocalId.has_value() ? query.match( *validLocalId ) : false );
    }

    // Test move semantics - build chained queries efficiently
    TEST( MetadataTagsQuery, MoveSemantics )
    {
        // Test that move semantics work for temporary builders
        auto query = MetadataTagsQueryBuilder::empty()
                         .withTag( CodebookName::Quantity, "temperature" )
                         .withTag( CodebookName::Content, "exhaust.gas" )
                         .withTag( CodebookName::Position, "inlet" )
                         .withAllowOtherTags( false )
                         .build();

        auto localId = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
        ASSERT_TRUE( localId.has_value() );
        EXPECT_TRUE( query.match( localId.value() ) );
    }

    // Test immutability - withTag returns new builder
    TEST( MetadataTagsQuery, Immutability )
    {
        auto builder1 = MetadataTagsQueryBuilder::empty();
        auto builder2 = builder1.withTag( CodebookName::Quantity, "temperature" );

        auto query1 = builder1.build();
        auto query2 = builder2.build();

        auto localId = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-temperature" );
        ASSERT_TRUE( localId.has_value() );

        // builder1 should still match any (no tags specified)
        EXPECT_TRUE( query1.match( localId.value() ) );

        // builder2 should match LocalIds with qty-temperature
        EXPECT_TRUE( query2.match( localId.value() ) );
    }
} // namespace dnv::vista::sdk::test
