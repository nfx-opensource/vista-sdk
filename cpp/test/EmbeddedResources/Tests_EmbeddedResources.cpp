/**
 * @file Tests_EmbeddedResources.cpp
 * @brief Tests for embedded VIS resources functionality
 *
 */

#include <gtest/gtest.h>

#include <EmbeddedResources.h>

#include <CodebooksDto.h>
#include <GmodDto.h>
#include <LocationsDto.h>
#include <GmodVersioningDto.h>
#include <ISO19848Dtos.h>

#include <algorithm>

namespace dnv::vista::sdk::test
{
    //=====================================================================
    // EmbeddedResources tests
    //=====================================================================

    //----------------------------------------------
    // VisVersions
    //----------------------------------------------

    TEST( EmbeddedResourcesTest, VisVersions_ContainsExpectedVersions )
    {
        auto versions = EmbeddedResources::visVersions();

        std::vector<std::string> expectedVersions = { "3-4a", "3-5a", "3-6a", "3-7a", "3-8a", "3-9a", "3-10a" };

        for( const auto& expected : expectedVersions )
        {
            bool found = std::find( versions.begin(), versions.end(), expected ) != versions.end();
            EXPECT_TRUE( found ) << "Expected version not found: " << expected;
        }
    }

    TEST( EmbeddedResourcesTest, Ordered )
    {
        const auto versions = EmbeddedResources::visVersions();

        auto it34 = std::find( versions.begin(), versions.end(), "3-4a" );
        auto it310 = std::find( versions.begin(), versions.end(), "3-10a" );

        ASSERT_NE( it34, versions.end() );
        ASSERT_NE( it310, versions.end() );

        EXPECT_LT( std::distance( versions.begin(), it34 ), std::distance( versions.begin(), it310 ) );
    }

    //----------------------------------------------
    // Gmod
    //----------------------------------------------

    TEST( EmbeddedResourcesTest, Gmod_LoadValidVersion )
    {
        auto gmod = EmbeddedResources::gmod( "3-10a" );

        ASSERT_TRUE( gmod.has_value() );
        EXPECT_EQ( "3-10a", gmod->visVersion );
        EXPECT_FALSE( gmod->items.empty() );
        EXPECT_FALSE( gmod->relations.empty() );
    }

    TEST( EmbeddedResourcesTest, Gmod_InvalidVersion_ReturnsNullopt )
    {
        auto gmod = EmbeddedResources::gmod( "invalid-version" );

        EXPECT_FALSE( gmod.has_value() );
    }

    TEST( EmbeddedResourcesTest, Gmod_AllVersionsLoad )
    {
        auto allVersions = EmbeddedResources::visVersions();

        for( const auto& version : allVersions )
        {
            auto gmod = EmbeddedResources::gmod( version );

            ASSERT_TRUE( gmod.has_value() ) << "Failed to load Gmod for version: " << version;
            EXPECT_EQ( version, gmod->visVersion );
            EXPECT_FALSE( gmod->items.empty() ) << "Gmod items empty for version: " << version;
            EXPECT_FALSE( gmod->relations.empty() ) << "Gmod relations empty for version: " << version;
        }
    }

    //----------------------------------------------
    // Codebooks
    //----------------------------------------------

    TEST( EmbeddedResourcesTest, Codebooks_LoadValidVersion )
    {
        auto codebooks = EmbeddedResources::codebooks( "3-10a" );

        ASSERT_TRUE( codebooks.has_value() );
        EXPECT_EQ( "3-10a", codebooks->visVersion );
        EXPECT_FALSE( codebooks->items.empty() );

        // Check first codebook has expected structure
        if( !codebooks->items.empty() )
        {
            const auto& firstCodebook = codebooks->items[0];
            EXPECT_FALSE( firstCodebook.name.empty() );
            EXPECT_FALSE( firstCodebook.values.empty() );
        }
    }

    TEST( EmbeddedResourcesTest, Codebooks_InvalidVersion_ReturnsNullopt )
    {
        auto codebooks = EmbeddedResources::codebooks( "invalid-version" );

        EXPECT_FALSE( codebooks.has_value() );
    }

    TEST( EmbeddedResourcesTest, Codebooks_AllVersionsLoad )
    {
        auto allVersions = EmbeddedResources::visVersions();

        for( const auto& version : allVersions )
        {
            auto codebooks = EmbeddedResources::codebooks( version );

            ASSERT_TRUE( codebooks.has_value() ) << "Failed to load Codebooks for version: " << version;
            EXPECT_EQ( version, codebooks->visVersion );
            EXPECT_FALSE( codebooks->items.empty() ) << "Codebooks items empty for version: " << version;

            // Validate first codebook structure
            const auto& firstCodebook = codebooks->items[0];
            EXPECT_FALSE( firstCodebook.name.empty() ) << "Codebook name empty for version: " << version;
            EXPECT_FALSE( firstCodebook.values.empty() ) << "Codebook values empty for version: " << version;
        }
    }

    TEST( EmbeddedResourcesTest, Codebooks_EqualityOperators )
    {
        // Load same version twice
        auto codebooks1 = EmbeddedResources::codebooks( "3-10a" );
        auto codebooks2 = EmbeddedResources::codebooks( "3-10a" );

        ASSERT_TRUE( codebooks1.has_value() );
        ASSERT_TRUE( codebooks2.has_value() );

        // Test CodebooksDto equality
        EXPECT_EQ( *codebooks1, *codebooks2 );
        EXPECT_FALSE( *codebooks1 != *codebooks2 );

        // Test individual CodebookDto equality
        if( !codebooks1->items.empty() && !codebooks2->items.empty() )
        {
            EXPECT_EQ( codebooks1->items[0], codebooks2->items[0] );
            EXPECT_FALSE( codebooks1->items[0] != codebooks2->items[0] );
        }

        // Load different version
        auto codebooks3 = EmbeddedResources::codebooks( "3-4a" );
        ASSERT_TRUE( codebooks3.has_value() );

        EXPECT_NE( *codebooks1, *codebooks3 );
        EXPECT_FALSE( *codebooks1 == *codebooks3 );

        // Test inequality
        EXPECT_NE( codebooks1->visVersion, codebooks3->visVersion );
    }

    //----------------------------------------------
    // Locations
    //----------------------------------------------

    TEST( EmbeddedResourcesTest, Locations_LoadValidVersion )
    {
        auto locations = EmbeddedResources::locations( "3-10a" );

        ASSERT_TRUE( locations.has_value() );
        EXPECT_EQ( "3-10a", locations->visVersion );
        EXPECT_FALSE( locations->items.empty() );

        // Check first location has expected structure
        if( !locations->items.empty() )
        {
            const auto& firstLocation = locations->items[0];
            EXPECT_NE( '\0', firstLocation.code );
            EXPECT_FALSE( firstLocation.name.empty() );
        }
    }

    TEST( EmbeddedResourcesTest, Locations_InvalidVersion_ReturnsNullopt )
    {
        auto locations = EmbeddedResources::locations( "invalid-version" );

        EXPECT_FALSE( locations.has_value() );
    }

    TEST( EmbeddedResourcesTest, Locations_AllVersionsLoad )
    {
        auto allVersions = EmbeddedResources::visVersions();

        for( const auto& version : allVersions )
        {
            auto locations = EmbeddedResources::locations( version );

            ASSERT_TRUE( locations.has_value() ) << "Failed to load Locations for version: " << version;
            EXPECT_EQ( version, locations->visVersion );
            EXPECT_FALSE( locations->items.empty() ) << "Locations items empty for version: " << version;

            // Validate first location structure
            const auto& firstLocation = locations->items[0];
            EXPECT_NE( '\0', firstLocation.code ) << "Location code empty for version: " << version;
            EXPECT_FALSE( firstLocation.name.empty() ) << "Location name empty for version: " << version;
        }
    }

    TEST( EmbeddedResourcesTest, Locations_EqualityOperators )
    {
        // Load same version twice
        auto locations1 = EmbeddedResources::locations( "3-10a" );
        auto locations2 = EmbeddedResources::locations( "3-10a" );

        ASSERT_TRUE( locations1.has_value() );
        ASSERT_TRUE( locations2.has_value() );

        // Test LocationsDto equality
        EXPECT_EQ( *locations1, *locations2 );
        EXPECT_FALSE( *locations1 != *locations2 );

        // Test individual RelativeLocationsDto equality
        if( !locations1->items.empty() && !locations2->items.empty() )
        {
            EXPECT_EQ( locations1->items[0], locations2->items[0] );
            EXPECT_FALSE( locations1->items[0] != locations2->items[0] );
        }

        // Load different version
        auto locations3 = EmbeddedResources::locations( "3-4a" );
        ASSERT_TRUE( locations3.has_value() );

        EXPECT_NE( *locations1, *locations3 );
        EXPECT_FALSE( *locations1 == *locations3 );
    }

    //----------------------------------------------
    // GmodVersioning
    //----------------------------------------------

    TEST( EmbeddedResourcesTest, GmodVersioning_LoadsAllVersions )
    {
        auto versioningMap = EmbeddedResources::gmodVersioning();

        ASSERT_TRUE( versioningMap.has_value() );
        EXPECT_FALSE( versioningMap->empty() );

        // Check if at least one version has non-empty items
        bool hasNonEmptyItems = false;
        for( const auto& [visVersion, versioningDto] : *versioningMap )
        {
            EXPECT_FALSE( visVersion.empty() );
            EXPECT_EQ( visVersion, versioningDto.visVersion );

            if( !versioningDto.items.empty() )
            {
                hasNonEmptyItems = true;
            }
        }

        // At least one version should have conversion data
        EXPECT_TRUE( hasNonEmptyItems ) << "All versioning files have empty items - check deserialization";
    }

    TEST( EmbeddedResourcesTest, GmodVersioning_EqualityOperators )
    {
        auto versioningMap = EmbeddedResources::gmodVersioning();

        ASSERT_TRUE( versioningMap.has_value() );
        ASSERT_FALSE( versioningMap->empty() );

        // Get two versions to compare
        auto it35 = versioningMap->find( "3-5a" );
        auto it35_2 = versioningMap->find( "3-5a" );
        auto it310 = versioningMap->find( "3-10a" );

        ASSERT_TRUE( it35 != versioningMap->end() );
        ASSERT_TRUE( it310 != versioningMap->end() );

        // Test GmodVersioningDto equality
        EXPECT_EQ( it35->second, it35_2->second );
        EXPECT_FALSE( it35->second != it35_2->second );

        // Test GmodVersioningDto inequality
        EXPECT_NE( it35->second, it310->second );
        EXPECT_FALSE( it35->second == it310->second );

        // Test individual GmodNodeConversionDto equality if items exist
        if( !it35->second.items.empty() )
        {
            const auto& firstItem = it35->second.items.begin()->second;
            EXPECT_EQ( firstItem, firstItem );
            EXPECT_FALSE( firstItem != firstItem );
        }
    }

    //----------------------------------------------
    // DataChannelTypeNames (ISO19848)
    //----------------------------------------------

    TEST( EmbeddedResourcesTest, DataChannelTypeNames_LoadValidVersion )
    {
        auto dataChannelTypeNames = EmbeddedResources::dataChannelTypeNames( "v2024" );

        ASSERT_TRUE( dataChannelTypeNames.has_value() );
        EXPECT_FALSE( dataChannelTypeNames->values.empty() );

        // Check first data channel type name has expected structure
        if( !dataChannelTypeNames->values.empty() )
        {
            const auto& firstType = dataChannelTypeNames->values[0];
            EXPECT_FALSE( firstType.type.empty() );
            EXPECT_FALSE( firstType.description.empty() );
        }
    }

    TEST( EmbeddedResourcesTest, DataChannelTypeNames_InvalidVersion_ReturnsNullopt )
    {
        auto dataChannelTypeNames = EmbeddedResources::dataChannelTypeNames( "invalid-version" );

        EXPECT_FALSE( dataChannelTypeNames.has_value() );
    }

    TEST( EmbeddedResourcesTest, DataChannelTypeNames_EqualityOperators )
    {
        // Load same version twice
        auto dataChannelTypeNames1 = EmbeddedResources::dataChannelTypeNames( "v2024" );
        auto dataChannelTypeNames2 = EmbeddedResources::dataChannelTypeNames( "v2024" );

        ASSERT_TRUE( dataChannelTypeNames1.has_value() );
        ASSERT_TRUE( dataChannelTypeNames2.has_value() );

        // Test DataChannelTypeNamesDto equality
        EXPECT_EQ( *dataChannelTypeNames1, *dataChannelTypeNames2 );
        EXPECT_FALSE( *dataChannelTypeNames1 != *dataChannelTypeNames2 );

        // Test individual DataChannelTypeNameDto equality
        if( !dataChannelTypeNames1->values.empty() && !dataChannelTypeNames2->values.empty() )
        {
            EXPECT_EQ( dataChannelTypeNames1->values[0], dataChannelTypeNames2->values[0] );
            EXPECT_FALSE( dataChannelTypeNames1->values[0] != dataChannelTypeNames2->values[0] );
        }

        // Load different version
        auto dataChannelTypeNames3 = EmbeddedResources::dataChannelTypeNames( "v2018" );
        ASSERT_TRUE( dataChannelTypeNames3.has_value() );

        EXPECT_NE( *dataChannelTypeNames1, *dataChannelTypeNames3 );
        EXPECT_FALSE( *dataChannelTypeNames1 == *dataChannelTypeNames3 );
    }

    //----------------------------------------------
    // FormatDataTypes (ISO19848)
    //----------------------------------------------

    TEST( EmbeddedResourcesTest, FormatDataTypes_LoadValidVersion )
    {
        auto formatDataTypes = EmbeddedResources::formatDataTypes( "v2024" );

        ASSERT_TRUE( formatDataTypes.has_value() );
        EXPECT_FALSE( formatDataTypes->values.empty() );

        // Check first format data type has expected structure
        if( !formatDataTypes->values.empty() )
        {
            const auto& firstType = formatDataTypes->values[0];
            EXPECT_FALSE( firstType.type.empty() );
            EXPECT_FALSE( firstType.description.empty() );
        }
    }

    TEST( EmbeddedResourcesTest, FormatDataTypes_InvalidVersion_ReturnsNullopt )
    {
        auto formatDataTypes = EmbeddedResources::formatDataTypes( "invalid-version" );

        EXPECT_FALSE( formatDataTypes.has_value() );
    }

    TEST( EmbeddedResourcesTest, FormatDataTypes_EqualityOperators )
    {
        // Load same version twice
        auto formatDataTypes1 = EmbeddedResources::formatDataTypes( "v2024" );
        auto formatDataTypes2 = EmbeddedResources::formatDataTypes( "v2024" );

        ASSERT_TRUE( formatDataTypes1.has_value() );
        ASSERT_TRUE( formatDataTypes2.has_value() );

        // Test FormatDataTypesDto equality
        EXPECT_EQ( *formatDataTypes1, *formatDataTypes2 );
        EXPECT_FALSE( *formatDataTypes1 != *formatDataTypes2 );

        // Test individual FormatDataTypeDto equality
        if( !formatDataTypes1->values.empty() && !formatDataTypes2->values.empty() )
        {
            EXPECT_EQ( formatDataTypes1->values[0], formatDataTypes2->values[0] );
            EXPECT_FALSE( formatDataTypes1->values[0] != formatDataTypes2->values[0] );
        }

        // Load different version
        auto formatDataTypes3 = EmbeddedResources::formatDataTypes( "v2018" );
        ASSERT_TRUE( formatDataTypes3.has_value() );

        // iso19848-v2018-format-data-types.json.gz and iso19848-v2024-format-data-types.json.gz are identical
        EXPECT_EQ( *formatDataTypes1, *formatDataTypes3 );
        EXPECT_TRUE( *formatDataTypes1 == *formatDataTypes3 );
    }
} // namespace dnv::vista::sdk::test
