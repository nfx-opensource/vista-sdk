/**
 * @file Tests_LocalIdQuery.cpp
 * @brief Unit tests for LocalIdQuery and LocalIdQueryBuilder functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <fstream>

namespace dnv::vista::sdk::test
{
    using namespace dnv::vista::sdk;

    class LocalIdQueryTests : public ::testing::Test
    {
    };

    // Parameterized test data for Test_Matches
    struct MatchTestData
    {
        std::string localIdStr;
        std::function<LocalIdQuery()> queryFactory;
        bool expectedMatch;
        std::string description;
    };

    class LocalIdQueryMatchTests : public ::testing::TestWithParam<MatchTestData>
    {
    };

    TEST_P( LocalIdQueryMatchTests, MatchCases )
    {
        const auto& data = GetParam();
        auto localId = LocalId::fromString( data.localIdStr );
        ASSERT_TRUE( localId.has_value() ) << "Failed to parse LocalId: " << data.localIdStr;

        auto query = data.queryFactory();
        EXPECT_EQ( data.expectedMatch, query.match( *localId ) ) << data.description;
    }

    INSTANTIATE_TEST_SUITE_P(
        MatchTests,
        LocalIdQueryMatchTests,
        ::testing::Values(
            MatchTestData{ "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage",
                           []() {
                               const auto& vis = VIS::instance();
                               const auto& gmod = vis.gmod( VisVersion::v3_4a );
                               const auto& locations = vis.locations( VisVersion::v3_4a );
                               auto path = GmodPath::fromShortPath( "1021.1i-6P/H123", gmod, locations );
                               return LocalIdQueryBuilder::empty()
                                   .withPrimaryItem(
                                       *path,
                                       []( GmodPathQueryBuilder::Path builder ) {
                                           return builder.withoutLocations().build();
                                       } )
                                   .build();
                           },
                           true,
                           "Match primary item without locations" },
            MatchTestData{ "/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened",
                           []() {
                               const auto& vis = VIS::instance();
                               const auto& codebooks = vis.codebooks( VisVersion::v3_4a );
                               auto tag = codebooks[CodebookName::Content].createTag( "sea.water" );
                               return LocalIdQueryBuilder::empty()
                                   .withTags( [&tag]( MetadataTagsQueryBuilder& tags ) {
                                       return tags.withTag( *tag ).build();
                                   } )
                                   .build();
                           },
                           true,
                           "Match by content tag" },
            MatchTestData{ "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
                           []() {
                               const auto& vis = VIS::instance();
                               const auto& gmod = vis.gmod( VisVersion::v3_4a );
                               const auto& locations = vis.locations( VisVersion::v3_4a );
                               auto path = GmodPath::fromShortPath( "411.1/C101.31-1", gmod, locations );
                               return LocalIdQueryBuilder::empty().withPrimaryItem( *path ).build();
                           },
                           false,
                           "No match with different location" },
            MatchTestData{ "/dnv-v2/vis-3-4a/411.1-1/C101.63/S206/~propulsion.engine/~cooling.system/meta/"
                           "qty-temperature/cnt-exhaust.gas/pos-inlet",
                           []() {
                               const auto& vis = VIS::instance();
                               const auto& gmod = vis.gmod( VisVersion::v3_4a );
                               const auto& locations = vis.locations( VisVersion::v3_4a );
                               auto path = GmodPath::fromShortPath( "411.1-2/C101.63/S206", gmod, locations );
                               return LocalIdQueryBuilder::empty()
                                   .withPrimaryItem(
                                       *path,
                                       []( GmodPathQueryBuilder::Path builder ) {
                                           return builder.withoutLocations().build();
                                       } )
                                   .build();
                           },
                           true,
                           "Match primary with different location (withoutLocations)" },
            MatchTestData{ "/dnv-v2/vis-3-4a/411.1/C101.63/S206/sec/411.1/C101.31-5/~propulsion.engine/~cooling.system/"
                           "~for.propulsion.engine/~cylinder.5/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
                           []() {
                               const auto& vis = VIS::instance();
                               const auto& gmod = vis.gmod( VisVersion::v3_4a );
                               const auto& locations = vis.locations( VisVersion::v3_4a );
                               auto path = GmodPath::fromShortPath( "411.1/C101.31-2", gmod, locations );
                               return LocalIdQueryBuilder::empty()
                                   .withSecondaryItem(
                                       *path,
                                       []( GmodPathQueryBuilder::Path builder ) {
                                           return builder.withoutLocations().build();
                                       } )
                                   .build();
                           },
                           true,
                           "Match secondary item without locations" },
            MatchTestData{ "/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low",
                           []() {
                               const auto& vis = VIS::instance();
                               const auto& gmod = vis.gmod( VisVersion::v3_4a );
                               const auto& locations = vis.locations( VisVersion::v3_4a );
                               auto path = GmodPath::fromShortPath( "411.1", gmod, locations );
                               return LocalIdQueryBuilder::empty()
                                   .withPrimaryItem(
                                       *path,
                                       []( GmodPathQueryBuilder::Path builder ) {
                                           return builder.withoutLocations().build();
                                       } )
                                   .build();
                           },
                           false,
                           "No match with different node" },
            MatchTestData{
                "/dnv-v2/vis-3-7a/433.1-S/C322.91/S205/meta/qty-conductivity/detail-relative",
                []() {
                    auto localId = LocalId::fromString( "/dnv-v2/vis-3-7a/433.1-S/C322.91/S205/meta/qty-conductivity" );
                    const auto& vis = VIS::instance();
                    const auto& gmod = vis.gmod( VisVersion::v3_7a );
                    auto node433 = gmod.node( "433.1" );
                    return LocalIdQueryBuilder::from( *localId )
                        .withPrimaryItem(
                            localId->primaryItem(),
                            [&node433]( GmodPathQueryBuilder::Path builder ) {
                                return builder
                                    .withNode(
                                        [&node433]( const std::unordered_map<std::string, const GmodNode*>& nodes ) {
                                            return *node433;
                                        },
                                        true )
                                    .build();
                            } )
                        .build();
                },
                true,
                "Match with node filter and matchAllLocations" } ) );

    TEST_F( LocalIdQueryTests, EmptyQueryMatchesAll )
    {
        auto localIdStr = "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-power";
        auto localId = LocalId::fromString( localIdStr );
        ASSERT_TRUE( localId.has_value() );

        auto query = LocalIdQueryBuilder::empty().build();
        EXPECT_TRUE( query.match( *localId ) );
    }

    TEST_F( LocalIdQueryTests, FromLocalId )
    {
        auto localIdStr = "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-power";
        auto localId = LocalId::fromString( localIdStr );
        ASSERT_TRUE( localId.has_value() );

        auto builder = LocalIdQueryBuilder::from( *localId );
        auto query = builder.build();
        EXPECT_TRUE( query.match( *localId ) );
    }

    TEST_F( LocalIdQueryTests, MatchGmodPathWithoutLocations )
    {
        const auto& vis = VIS::instance();
        auto version = VisVersion::v3_4a;
        const auto& gmod = vis.gmod( version );
        const auto& locations = vis.locations( version );

        auto path = GmodPath::fromShortPath( "411.1/C101.31", gmod, locations );
        ASSERT_TRUE( path.has_value() );

        auto query =
            LocalIdQueryBuilder::empty()
                .withPrimaryItem(
                    *path,
                    []( GmodPathQueryBuilder::Path pathBuilder ) { return pathBuilder.withoutLocations().build(); } )
                .build();

        auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-power" );
        ASSERT_TRUE( localId1.has_value() );
        EXPECT_TRUE( query.match( *localId1 ) );

        auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1-1/C101.31/meta/qty-power" );
        ASSERT_TRUE( localId2.has_value() );
        EXPECT_TRUE( query.match( *localId2 ) );

        auto localId3 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C102.31/meta/qty-power" );
        ASSERT_TRUE( localId3.has_value() );
        EXPECT_FALSE( query.match( *localId3 ) );
    }

    TEST_F( LocalIdQueryTests, MatchByMetadataTags )
    {
        const auto& vis = VIS::instance();
        auto version = VisVersion::v3_4a;
        const auto& codebooks = vis.codebooks( version );

        auto qtyTag = codebooks[CodebookName::Quantity].createTag( "power" );
        ASSERT_TRUE( qtyTag.has_value() );

        auto query =
            LocalIdQueryBuilder::empty()
                .withTags( [&qtyTag]( MetadataTagsQueryBuilder& tags ) { return tags.withTag( *qtyTag ).build(); } )
                .build();

        auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-power" );
        ASSERT_TRUE( localId1.has_value() );
        EXPECT_TRUE( query.match( *localId1 ) );

        auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature" );
        ASSERT_TRUE( localId2.has_value() );
        EXPECT_FALSE( query.match( *localId2 ) );
    }

    TEST_F( LocalIdQueryTests, WithNodeQuery )
    {
        const auto& vis = VIS::instance();
        auto version = VisVersion::v3_4a;
        const auto& gmod = vis.gmod( version );

        auto nodeC101 = gmod.node( "C101" );
        ASSERT_TRUE( nodeC101.has_value() );

        auto query = LocalIdQueryBuilder::empty()
                         .withPrimaryItem( [&nodeC101]( GmodPathQueryBuilder::Nodes nodes ) {
                             return nodes.withNode( **nodeC101 ).build();
                         } )
                         .build();

        auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-power" );
        ASSERT_TRUE( localId1.has_value() );
        EXPECT_TRUE( query.match( *localId1 ) );

        auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C102.31/meta/qty-power" );
        ASSERT_TRUE( localId2.has_value() );
        EXPECT_FALSE( query.match( *localId2 ) );
    }

    TEST_F( LocalIdQueryTests, WithoutSecondaryItem )
    {
        auto baseLocalId = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/meta/qty-power" );
        ASSERT_TRUE( baseLocalId.has_value() );

        auto queryBuilder = LocalIdQueryBuilder::from( *baseLocalId );
        auto otherLocalId = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/sec/412.3/meta/qty-power" );
        ASSERT_TRUE( otherLocalId.has_value() );

        auto query = queryBuilder.build();
        EXPECT_TRUE( query.match( *baseLocalId ) );
        EXPECT_FALSE( query.match( *otherLocalId ) );

        query = queryBuilder.withoutSecondaryItem().build();
        EXPECT_FALSE( query.match( *otherLocalId ) );

        query = queryBuilder.withAnySecondaryItem().build();
        EXPECT_TRUE( query.match( *otherLocalId ) );
    }

    TEST_F( LocalIdQueryTests, Variations )
    {
        auto localId = LocalId::fromString(
            "/dnv-v2/vis-3-4a/1036.13i-1/C662/sec/411.1-2/C101/meta/qty-pressure/cnt-cargo/state-high.high/pos-stage-3/"
            "detail-discharge" );
        ASSERT_TRUE( localId.has_value() );

        const auto& vis = VIS::instance();
        auto version = VisVersion::v3_4a;
        const auto& gmod = vis.gmod( version );
        const auto& locations = vis.locations( version );

        auto primaryItem = GmodPath::fromShortPath( "1036.13i-2/C662", gmod, locations );
        ASSERT_TRUE( primaryItem.has_value() );

        auto builder = LocalIdQueryBuilder::empty().withPrimaryItem( *primaryItem );
        auto query = builder.build();
        EXPECT_FALSE( query.match( *localId ) );

        builder = builder.withPrimaryItem( *primaryItem, []( GmodPathQueryBuilder::Path pathBuilder ) {
            return pathBuilder.withoutLocations().build();
        } );
        query = builder.build();
        EXPECT_TRUE( query.match( *localId ) );

        auto primaryItemNoLoc = primaryItem->withoutLocations();
        builder = LocalIdQueryBuilder::empty().withPrimaryItem( primaryItemNoLoc );
        query = builder.build();
        EXPECT_FALSE( query.match( *localId ) );

        builder = builder.withPrimaryItem( primaryItemNoLoc, []( GmodPathQueryBuilder::Path pathBuilder ) {
            return pathBuilder.withoutLocations().build();
        } );
        query = builder.build();
        EXPECT_TRUE( query.match( *localId ) );
    }

    TEST_F( LocalIdQueryTests, WithAnyNodeBefore )
    {
        const auto& vis = VIS::instance();
        auto version = VisVersion::v3_9a;
        const auto& codebooks = vis.codebooks( version );
        const auto& gmod = vis.gmod( version );
        const auto& locations = vis.locations( version );

        auto basePath = GmodPath::fromShortPath( "411.1/C101.31", gmod, locations );
        ASSERT_TRUE( basePath.has_value() );

        auto baseLocalId = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/sec/412.3/meta/qty-power" );
        ASSERT_TRUE( baseLocalId.has_value() );

        auto nodeC101 = gmod.node( "C101" );
        ASSERT_TRUE( nodeC101.has_value() );
        const auto* pNodeC101 = *nodeC101;

        auto specificQuery =
            LocalIdQueryBuilder::from( *baseLocalId )
                .withPrimaryItem(
                    baseLocalId->primaryItem(),
                    [pNodeC101]( GmodPathQueryBuilder::Path path ) {
                        return path
                            .withAnyNodeBefore(
                                [pNodeC101]( const std::unordered_map<std::string, const GmodNode*>& nodes ) {
                                    return pNodeC101;
                                } )
                            .build();
                    } )
                .build();

        auto qtyPowerTag = codebooks[CodebookName::Quantity].createTag( "power" );
        ASSERT_TRUE( qtyPowerTag.has_value() );

        auto generalQuery =
            LocalIdQueryBuilder::empty()
                .withPrimaryItem(
                    *basePath,
                    [pNodeC101]( GmodPathQueryBuilder::Path path ) {
                        return path
                            .withAnyNodeBefore(
                                [pNodeC101]( const std::unordered_map<std::string, const GmodNode*>& nodes ) {
                                    return pNodeC101;
                                } )
                            .build();
                    } )
                .withTags(
                    [&qtyPowerTag]( MetadataTagsQueryBuilder& tags ) { return tags.withTag( *qtyPowerTag ).build(); } )
                .build();

        EXPECT_TRUE( generalQuery.match( *baseLocalId ) );

        auto l2 = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/sec/412.3/meta/qty-power" );
        ASSERT_TRUE( l2.has_value() );
        EXPECT_TRUE( specificQuery.match( *l2 ) );
        EXPECT_TRUE( generalQuery.match( *l2 ) );

        auto l3 = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/meta/qty-power" );
        ASSERT_TRUE( l3.has_value() );
        EXPECT_FALSE( specificQuery.match( *l3 ) );
        EXPECT_TRUE( generalQuery.match( *l3 ) );

        auto l4 = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C102.31/meta/qty-power" );
        ASSERT_TRUE( l4.has_value() );
        EXPECT_FALSE( generalQuery.match( *l4 ) );
        EXPECT_FALSE( specificQuery.match( *l4 ) );

        auto l5 = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/sec/412.2/meta/qty-power" );
        ASSERT_TRUE( l5.has_value() );
        EXPECT_FALSE( specificQuery.match( *l5 ) );
        EXPECT_TRUE( generalQuery.match( *l5 ) );

        auto l6 = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/sec/412.2/meta/qty-pressure" );
        ASSERT_TRUE( l6.has_value() );
        EXPECT_FALSE( specificQuery.match( *l6 ) );
        EXPECT_FALSE( generalQuery.match( *l6 ) );
    }

    TEST_F( LocalIdQueryTests, UseCase1 )
    {
        const auto& vis = VIS::instance();
        const auto& locations = vis.locations( vis.latest() );

        auto localId = LocalId::fromString(
            "/dnv-v2/vis-3-7a/433.1-P/C322/meta/qty-linear.vibration.amplitude/pos-driving.end/detail-iso.10816" );
        ASSERT_TRUE( localId.has_value() );

        auto locationP = locations.fromString( "P" );
        auto locationS = locations.fromString( "S" );
        ASSERT_TRUE( locationP.has_value() );
        ASSERT_TRUE( locationS.has_value() );

        const auto& gmod = vis.gmod( localId->version() );
        auto node433 = gmod.node( "433.1" );
        ASSERT_TRUE( node433.has_value() );
        const auto* pNode433 = *node433;

        std::vector<Location> locs = { *locationP, *locationS };

        auto query = LocalIdQueryBuilder::from( *localId )
                         .withPrimaryItem(
                             localId->primaryItem(),
                             [pNode433, &locs]( GmodPathQueryBuilder::Path builder ) {
                                 return builder
                                     .withNode(
                                         [pNode433]( const std::unordered_map<std::string, const GmodNode*>& nodes ) {
                                             return pNode433;
                                         },
                                         locs )
                                     .build();
                             } )
                         .build();

        EXPECT_TRUE( query.match( *localId ) );
        auto localId2 = LocalId::fromString(
            "/dnv-v2/vis-3-7a/433.1-S/C322/meta/qty-linear.vibration.amplitude/pos-driving.end/detail-iso.10816" );
        ASSERT_TRUE( localId2.has_value() );
        EXPECT_TRUE( query.match( *localId2 ) );
    }

    TEST_F( LocalIdQueryTests, UseCase2 )
    {
        const auto& vis = VIS::instance();

        auto localId = LocalId::fromString(
            "/dnv-v2/vis-3-7a/511.31/C121/meta/qty-linear.vibration.amplitude/pos-driving.end/detail-iso.10816" );
        ASSERT_TRUE( localId.has_value() );

        const auto& gmod = vis.gmod( localId->version() );

        auto node5113 = gmod.node( "511.3" );
        ASSERT_TRUE( node5113.has_value() );

        auto query = LocalIdQueryBuilder::from( *localId )
                         .withPrimaryItem( [&node5113]( GmodPathQueryBuilder::Nodes builder ) {
                             return builder.withNode( **node5113, true ).build();
                         } )
                         .build();

        EXPECT_TRUE( query.match( *localId ) );

        auto node5114 = gmod.node( "511.4" );
        ASSERT_TRUE( node5114.has_value() );
        const auto* pNode5114 = *node5114;

        query = LocalIdQueryBuilder::empty()
                    .withPrimaryItem( [pNode5114]( GmodPathQueryBuilder::Nodes builder ) {
                        return builder.withNode( *pNode5114, true ).build();
                    } )
                    .build();

        EXPECT_FALSE( query.match( *localId ) );
    }

    TEST_F( LocalIdQueryTests, UseCase3 )
    {
        const auto& vis = VIS::instance();

        auto localId =
            LocalId::fromString( "/dnv-v2/vis-3-7a/433.1-S/C322.91/S205/meta/qty-conductivity/detail-relative" );
        ASSERT_TRUE( localId.has_value() );

        auto builder = localId->builder();
        auto visVersion = localId->version();

        auto query = LocalIdQueryBuilder::from( *localId )
                         .withTags( [&localId]( MetadataTagsQueryBuilder& tags ) {
                             auto tagsBuilder = tags;
                             for( const auto& tag : localId->metadataTags() )
                             {
                                 tagsBuilder = tagsBuilder.withTag( tag );
                             }
                             return tagsBuilder.withAllowOtherTags( false ).build();
                         } )
                         .build();

        EXPECT_TRUE( query.match( *localId ) );

        const auto& gmod = vis.gmod( visVersion );
        const auto& codebooks = vis.codebooks( visVersion );
        const auto& locations = vis.locations( visVersion );

        auto randomTag = codebooks[CodebookName::Content].createTag( "random" );
        ASSERT_TRUE( randomTag.has_value() );
        auto l1 = builder.withMetadataTag( *randomTag ).build();

        auto pathOpt = GmodPath::fromShortPath( "433.1-1S", gmod, locations );
        ASSERT_TRUE( pathOpt.has_value() );
        auto l2 = builder.withPrimaryItem( *pathOpt ).build();

        EXPECT_FALSE( query.match( l1 ) );
        EXPECT_FALSE( query.match( l2 ) );
    }

    TEST_F( LocalIdQueryTests, UseCase4 )
    {
        auto localIdOpt = LocalId::fromString( "/dnv-v2/vis-3-7a/511.11/C101/meta/qty-pressure/cnt-lubricating.oil" );
        ASSERT_TRUE( localIdOpt.has_value() );

        auto queryOpt = LocalIdQueryBuilder::from( *localIdOpt );

        auto query = queryOpt
                         .withPrimaryItem(
                             localIdOpt->primaryItem(),
                             []( GmodPathQueryBuilder::Path builder ) { return builder.withoutLocations().build(); } )
                         .build();

        auto other = LocalId::fromString( "/dnv-v2/vis-3-7a/511.11-1/C101/meta/qty-pressure/cnt-lubricating.oil" );
        ASSERT_TRUE( other.has_value() );

        EXPECT_TRUE( query.match( *other ) );
    }

    TEST_F( LocalIdQueryTests, HappyPath )
    {
        auto localIdStr =
            "/dnv-v2/vis-3-4a/1036.13i-1/C662/sec/411.1-2/C101/meta/qty-pressure/cnt-cargo/state-high.high/pos-stage-3/"
            "detail-discharge";
        auto localId = LocalId::fromString( localIdStr );
        ASSERT_TRUE( localId.has_value() );

        const auto& primaryItem = localId->primaryItem();
        const auto& secondaryItem = localId->secondaryItem();
        ASSERT_TRUE( secondaryItem.has_value() );

        // Match exact
        auto builder = LocalIdQueryBuilder::from( *localId );
        auto query = builder.build();
        EXPECT_TRUE( query.match( *localId ) );

        // Test both individualized and non-individualized
        for( bool individualized : { true, false } )
        {
            auto primaryQueryBuilder = GmodPathQueryBuilder::from( primaryItem );
            auto secondaryQueryBuilder = GmodPathQueryBuilder::from( *secondaryItem );

            if( !individualized )
            {
                primaryQueryBuilder = std::move( primaryQueryBuilder ).withoutLocations();
                secondaryQueryBuilder = std::move( secondaryQueryBuilder ).withoutLocations();
            }

            auto primaryQuery = primaryQueryBuilder.build();
            auto secondaryQuery = secondaryQueryBuilder.build();

            // Match primary
            builder = LocalIdQueryBuilder::empty().withPrimaryItem( primaryQuery );
            query = builder.build();
            EXPECT_TRUE( query.match( *localId ) )
                << "Failed to match primary (individualized=" << individualized << ")";

            // Match secondary
            builder = LocalIdQueryBuilder::empty().withSecondaryItem( secondaryQuery );
            query = builder.build();
            EXPECT_TRUE( query.match( *localId ) )
                << "Failed to match secondary (individualized=" << individualized << ")";

            // Match tags
            builder = LocalIdQueryBuilder::empty();
            for( const auto& tag : localId->metadataTags() )
            {
                builder = std::move( builder ).withTags(
                    [&tag]( MetadataTagsQueryBuilder& tags ) { return tags.withTag( tag ).build(); } );
            }
            query = builder.build();
            EXPECT_TRUE( query.match( *localId ) ) << "Failed to match tags (individualized=" << individualized << ")";

            // Match primary and secondary
            builder = LocalIdQueryBuilder::empty().withPrimaryItem( primaryQuery ).withSecondaryItem( secondaryQuery );
            query = builder.build();
            EXPECT_TRUE( query.match( *localId ) )
                << "Failed to match primary+secondary (individualized=" << individualized << ")";

            // Match primary and tags
            builder = LocalIdQueryBuilder::empty().withPrimaryItem( primaryQuery );
            for( const auto& tag : localId->metadataTags() )
            {
                builder = std::move( builder ).withTags(
                    [&tag]( MetadataTagsQueryBuilder& tags ) { return tags.withTag( tag ).build(); } );
            }
            query = builder.build();
            EXPECT_TRUE( query.match( *localId ) )
                << "Failed to match primary+tags (individualized=" << individualized << ")";

            // Match secondary and tags
            builder = LocalIdQueryBuilder::empty().withSecondaryItem( secondaryQuery );
            for( const auto& tag : localId->metadataTags() )
            {
                builder = std::move( builder ).withTags(
                    [&tag]( MetadataTagsQueryBuilder& tags ) { return tags.withTag( tag ).build(); } );
            }
            query = builder.build();
            EXPECT_TRUE( query.match( *localId ) )
                << "Failed to match secondary+tags (individualized=" << individualized << ")";

            // Match primary, secondary, and tags
            builder = LocalIdQueryBuilder::empty().withPrimaryItem( primaryQuery ).withSecondaryItem( secondaryQuery );
            for( const auto& tag : localId->metadataTags() )
            {
                builder = std::move( builder ).withTags(
                    [&tag]( MetadataTagsQueryBuilder& tags ) { return tags.withTag( tag ).build(); } );
            }
            query = builder.build();
            EXPECT_TRUE( query.match( *localId ) ) << "Failed to match all (individualized=" << individualized << ")";
        }
    }

    // Parameterized test for sample LocalIds
    class LocalIdQuerySampleTests : public ::testing::TestWithParam<std::string>
    {
    };

    TEST_P( LocalIdQuerySampleTests, SampleLocalIds )
    {
        auto localIdStr = GetParam();
        auto localId = LocalId::fromString( localIdStr );
        ASSERT_TRUE( localId.has_value() ) << "Failed to parse LocalId: " << localIdStr;

        auto builder = LocalIdQueryBuilder::from( *localId );
        auto query = builder.build();
        EXPECT_TRUE( query.match( *localId ) );

        query = LocalIdQueryBuilder::empty().build();
        EXPECT_TRUE( query.match( *localId ) );
    }

    INSTANTIATE_TEST_SUITE_P(
        Samples,
        LocalIdQuerySampleTests,
        ::testing::Values(
            "/dnv-v2/vis-3-4a/623.22i/S110/sec/412.722-F/C542/meta/detail-stand.by.start.or.power.failure",
            "/dnv-v2/vis-3-4a/623.1/sec/412.722-F/C542/meta/qty-level/cnt-lubricating.oil/state-high",
            "/dnv-v2/vis-3-4a/412.723-F/C261/meta/qty-temperature",
            "/dnv-v2/vis-3-4a/412.723-A/C261/meta/qty-temperature",
            "/dnv-v2/vis-3-4a/623.121/H201/sec/412.722-A/C542/meta/qty-level/cnt-lubricating.oil/state-high",
            "/dnv-v2/vis-3-4a/623.121/H201/sec/412.722-A/C542/meta/qty-level/cnt-lubricating.oil/state-low",
            "/dnv-v2/vis-3-4a/412.723-A/CS6d/meta/qty-temperature",
            "/dnv-v2/vis-3-4a/411.1/C101.64i-1/S201.1/C151.2/S110/meta/cnt-hydraulic.oil/state-running" ) );

    // Test nullptr handling
    TEST_F( LocalIdQueryTests, OptionalHandling )
    {
        auto query = LocalIdQueryBuilder::empty().build();

        // Test with empty optional
        std::optional<LocalId> emptyLocalId;
        EXPECT_FALSE( emptyLocalId.has_value() ? query.match( *emptyLocalId ) : false );

        // Test with valid optional
        auto validLocalId = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/meta/qty-power" );
        EXPECT_TRUE( validLocalId.has_value() ? query.match( *validLocalId ) : false );
    }
} // namespace dnv::vista::sdk::test
