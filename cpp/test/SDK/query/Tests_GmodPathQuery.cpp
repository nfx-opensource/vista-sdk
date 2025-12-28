/**
 * @file Tests_GmodPathQuery.cpp
 * @brief Unit tests for GmodPathQuery functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::test
{
    // Test Path builder
    class GmodPathQuery_PathBuilder
        : public ::testing::TestWithParam<
              std::tuple<std::string, VisVersion, std::vector<std::pair<std::string, std::vector<std::string>>>, bool>>
    {
    };

    TEST_P( GmodPathQuery_PathBuilder, PathBuilder )
    {
        const auto& [pathStr, visVersion, parameters, expectedMatch] = GetParam();

        const auto& vis = VIS::instance();
        const auto& gmod = vis.gmod( visVersion );
        const auto& locations = vis.locations( visVersion );

        auto path = GmodPath::fromShortPath( pathStr, visVersion );
        ASSERT_TRUE( path.has_value() );

        auto builder = GmodPathQueryBuilder::from( path.value() );
        auto query = builder.build();

        // For consistency, the query should always match itself
        EXPECT_TRUE( query.match( path.value() ) );

        // Apply parameters
        for( const auto& [nodeCode, locStrs] : parameters )
        {
            if( locStrs.empty() )
            {
                // matchAllLocations = true
                builder = builder.withNode(
                    [nodeCode]( const std::unordered_map<std::string, const GmodNode*>& nodes ) -> const GmodNode* {
                        auto it = nodes.find( nodeCode );
                        return it != nodes.end() ? it->second : nullptr;
                    },
                    true );
            }
            else
            {
                // Parse locations
                std::vector<Location> locs;
                for( const auto& locStr : locStrs )
                {
                    auto loc = locations.fromString( locStr );
                    ASSERT_TRUE( loc.has_value() );
                    locs.push_back( loc.value() );
                }
                builder = builder.withNode(
                    [nodeCode]( const std::unordered_map<std::string, const GmodNode*>& nodes ) -> const GmodNode* {
                        auto it = nodes.find( nodeCode );
                        return it != nodes.end() ? it->second : nullptr;
                    },
                    locs );
            }
        }

        query = builder.build();
        bool match = query.match( path.value() );
        EXPECT_EQ( expectedMatch, match );
    }

    INSTANTIATE_TEST_SUITE_P(
        GmodPathQueryTests,
        GmodPathQuery_PathBuilder,
        ::testing::Values(
            std::make_tuple(
                "411.1-1/C101",
                VisVersion::v3_4a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{},
                true ),
            std::make_tuple(
                "411.1-1/C101",
                VisVersion::v3_4a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "411.1", { "1" } } },
                true ),
            std::make_tuple(
                "411.1-1/C101",
                VisVersion::v3_4a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "411.1", { "A" } } },
                false ),
            std::make_tuple(
                "433.1-P/C322.31/C173",
                VisVersion::v3_4a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "C322.31", {} } },
                true ),
            std::make_tuple(
                "433.1-P/C322.31-2/C173",
                VisVersion::v3_4a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "433.1", { "P" } },
                                                                               { "C322.31", {} } },
                true ),
            std::make_tuple(
                "433.1-P/C322.31-2/C173",
                VisVersion::v3_4a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "433.1", { "A" } },
                                                                               { "C322.31", {} } },
                false ),
            std::make_tuple(
                "433.1-P/C322.31-2/C173",
                VisVersion::v3_4a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "433.1", { "P" } },
                                                                               { "C322.31", { "1" } } },
                false ),
            std::make_tuple(
                "433.1-A/C322.31-2/C173",
                VisVersion::v3_4a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "433.1", { "P" } },
                                                                               { "C322.31", { "1" } } },
                false ),
            std::make_tuple(
                "433.1-A/C322.31-2/C173",
                VisVersion::v3_4a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "433.1", {} }, { "C322.31", {} } },
                true ),
            std::make_tuple(
                "433.1/C322.31-2/C173",
                VisVersion::v3_4a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "433.1", { "A" } } },
                false ),
            std::make_tuple(
                "433.1/C322.31-2/C173",
                VisVersion::v3_4a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "433.1", {} } },
                true ) ) );

    // Test Nodes builder
    class GmodPathQuery_NodesBuilder
        : public ::testing::TestWithParam<
              std::tuple<std::string, VisVersion, std::vector<std::pair<std::string, std::vector<std::string>>>, bool>>
    {
    };

    TEST_P( GmodPathQuery_NodesBuilder, NodesBuilder )
    {
        const auto& [pathStr, visVersion, parameters, expectedMatch] = GetParam();

        const auto& vis = VIS::instance();
        const auto& gmod = vis.gmod( visVersion );
        const auto& locations = vis.locations( visVersion );

        auto path = GmodPath::fromShortPath( pathStr, visVersion );
        ASSERT_TRUE( path.has_value() );

        auto builder = GmodPathQueryBuilder::empty();

        // Apply parameters
        for( const auto& [nodeCode, locStrs] : parameters )
        {
            auto node = gmod.node( nodeCode );
            ASSERT_TRUE( node.has_value() );
            const GmodNode* nodePtr = node.value();

            if( locStrs.empty() )
            {
                // matchAllLocations = true
                builder = builder.withNode( *nodePtr, true );
            }
            else
            {
                // Parse locations
                std::vector<Location> locs;
                for( const auto& locStr : locStrs )
                {
                    auto loc = locations.fromString( locStr );
                    ASSERT_TRUE( loc.has_value() );
                    locs.push_back( loc.value() );
                }
                builder = builder.withNode( *nodePtr, locs );
            }
        }

        auto query = builder.build();
        bool match = query.match( path.value() );
        EXPECT_EQ( expectedMatch, match );
    }

    INSTANTIATE_TEST_SUITE_P(
        GmodPathQueryTests,
        GmodPathQuery_NodesBuilder,
        ::testing::Values(
            std::make_tuple(
                "411.1-1/C101",
                VisVersion::v3_4a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "411.1", { "1" } } },
                true ),
            std::make_tuple(
                "411.1-1/C101.61/S203.3/S110.2/C101",
                VisVersion::v3_7a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "411.1", { "1" } } },
                true ),
            std::make_tuple(
                "411.1/C101.61-1/S203.3/S110.2/C101",
                VisVersion::v3_7a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "C101.61", { "1" } } },
                true ),
            std::make_tuple(
                "511.11/C101.61-1/S203.3/S110.2/C101",
                VisVersion::v3_7a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "C101.61", { "1" } } },
                true ),
            std::make_tuple(
                "411.1/C101.61-1/S203.3/S110.2/C101",
                VisVersion::v3_7a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "C101.61", {} } },
                true ),
            std::make_tuple(
                "511.11/C101.61-1/S203.3/S110.2/C101",
                VisVersion::v3_7a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "C101.61", {} } },
                true ),
            std::make_tuple(
                "221.11/C1141.421/C1051.7/C101.61-2/S203",
                VisVersion::v3_7a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "C101.61", {} } },
                true ),
            std::make_tuple(
                "411.1/C101.61-1/S203.3/S110.2/C101",
                VisVersion::v3_7a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "411.1", {} }, { "C101.61", {} } },
                true ),
            std::make_tuple(
                "511.11/C101.61-1/S203.3/S110.2/C101",
                VisVersion::v3_7a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "411.1", {} }, { "C101.61", {} } },
                false ),
            std::make_tuple(
                "411.1/C101.61/S203.3-1/S110.2/C101",
                VisVersion::v3_7a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "S203.3", { "1" } } },
                true ),
            std::make_tuple(
                "411.1/C101.61/S203.3-1/S110.2/C101",
                VisVersion::v3_7a,
                std::vector<std::pair<std::string, std::vector<std::string>>>{ { "S203.3", { "1" } } },
                true ) ) );

    // Test nullptr handling
    TEST( GmodPathQuery, OptionalHandling )
    {
        const auto& vis = VIS::instance();
        const auto& gmod = vis.gmod( VisVersion::v3_4a );
        const auto& locations = vis.locations( VisVersion::v3_4a );

        auto path = GmodPath::fromShortPath( "411.1/C101", gmod, locations );
        ASSERT_TRUE( path.has_value() );

        auto query = GmodPathQueryBuilder::from( *path ).build();

        // Test with empty optional
        std::optional<GmodPath> emptyPath;
        EXPECT_FALSE( emptyPath.has_value() ? query.match( *emptyPath ) : false );

        // Test with valid optional
        EXPECT_TRUE( path.has_value() ? query.match( *path ) : false );
    }
} // namespace dnv::vista::sdk::test
