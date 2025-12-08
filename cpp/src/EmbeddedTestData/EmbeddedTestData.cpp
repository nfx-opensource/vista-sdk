/**
 * @file EmbeddedTestData.cpp
 * @brief Implementation of test data resource loading
 */

#include "EmbeddedTestData.h"

#include <nfx/serialization/json/Serializer.h>
#include <algorithm>
#include <ranges>
#include <sstream>
#include <stdexcept>

namespace dnv::vista::sdk::testdata
{
	struct TestResource
	{
		std::string_view name;
		const uint8_t* data;
		size_t size;
	};

	const std::vector<TestResource>& all();
} // namespace dnv::vista::sdk::testdata

namespace dnv::vista::sdk
{
	std::vector<uint8_t> EmbeddedTestData::getBytes( std::string_view filename )
	{
		const auto& resources = testdata::all();
		auto it = std::ranges::find_if( resources, [filename]( const auto& r ) {
			return r.name == filename;
		} );

		if ( it == resources.end() )
		{
			return {};
		}

		return std::vector<uint8_t>( it->data, it->data + it->size );
	}

	std::string EmbeddedTestData::getText( std::string_view filename )
	{
		const auto bytes = getBytes( filename );
		if ( bytes.empty() )
		{
			return {};
		}

		return std::string( reinterpret_cast<const char*>( bytes.data() ), bytes.size() );
	}

	std::vector<std::string> EmbeddedTestData::listFiles()
	{
		const auto& resources = testdata::all();
		std::vector<std::string> filenames;
		filenames.reserve( resources.size() );

		for ( const auto& r : resources )
		{
			filenames.emplace_back( r.name );
		}

		return filenames;
	}
} // namespace dnv::vista::sdk
