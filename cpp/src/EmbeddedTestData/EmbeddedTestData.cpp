/**
 * @file EmbeddedTestData.cpp
 * @brief Implementation of test data resource loading
 */

#include "EmbeddedTestData.h"

#include <testdata.h>
#include <testdata_additional.h>

#include <algorithm>
#include <ranges>
#include <sstream>
#include <stdexcept>

namespace dnv::vista::sdk
{
    std::vector<uint8_t> EmbeddedTestData::getBytes( std::string_view filename )
    {
        // Try main testdata first
        const auto* resource = testdata::find( filename );

        // Fall back to additional testdata
        if( !resource || resource->empty() )
        {
            resource = testdata::additional::find( filename );
        }

        if( !resource || resource->empty() )
        {
            return {};
        }

        return std::vector<uint8_t>( resource->data, resource->data + resource->size );
    }

    std::string EmbeddedTestData::getText( std::string_view filename )
    {
        // Try main testdata first
        const auto* resource = testdata::find( filename );

        // Fall back to additional testdata
        if( !resource || resource->empty() )
        {
            resource = testdata::additional::find( filename );
        }

        if( !resource || resource->empty() )
        {
            return {};
        }

        return std::string( resource->str() );
    }

    std::vector<std::string> EmbeddedTestData::listFiles()
    {
        const auto& resources = testdata::all();
        const auto& additionalResources = testdata::additional::all();

        std::vector<std::string> filenames;
        filenames.reserve( resources.size() + additionalResources.size() );

        for( const auto& r : resources )
        {
            filenames.emplace_back( r.name );
        }

        for( const auto& r : additionalResources )
        {
            filenames.emplace_back( r.name );
        }

        return filenames;
    }
} // namespace dnv::vista::sdk
