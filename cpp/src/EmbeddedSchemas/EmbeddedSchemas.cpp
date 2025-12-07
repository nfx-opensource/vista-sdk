/**
 * @file EmbeddedSchemas.cpp
 * @brief Implementation of embedded schema resources
 * @details Provides access to JSON and XSD schema files embedded at compile time
 */

#include "EmbeddedSchemas.h"

#include <algorithm>
#include <ranges>

namespace dnv::vista::sdk::schemas
{
	struct SchemaResource
	{
		std::string_view name;
		const uint8_t* data;
		size_t size;
	};

	const std::vector<SchemaResource>& all();
} // namespace dnv::vista::sdk::schemas

namespace dnv::vista::sdk
{
	namespace
	{
		std::optional<std::string_view> findSchema( std::string_view namePattern )
		{
			const auto& resources = schemas::all();
			auto it = std::ranges::find_if( resources,
				[namePattern]( const auto& res ) { return res.name.find( namePattern ) != std::string_view::npos; } );

			if ( it == resources.end() )
			{
				return std::nullopt;
			}

			return std::string_view{ reinterpret_cast<const char*>( it->data ), it->size };
		}
	} // namespace

	std::optional<std::string_view> EmbeddedSchema::dataChannelListJson()
	{
		return findSchema( "DataChannelList.schema.json" );
	}

	std::optional<std::string_view> EmbeddedSchema::timeSeriesDataJson()
	{
		return findSchema( "TimeSeriesData.schema.json" );
	}

	std::optional<std::string_view> EmbeddedSchema::dataChannelListXsd()
	{
		return findSchema( "DataChannelList.xsd" );
	}

	std::optional<std::string_view> EmbeddedSchema::timeSeriesDataXsd()
	{
		return findSchema( "TimeSeriesData.xsd" );
	}

	std::optional<std::string_view> EmbeddedSchema::get( std::string_view name )
	{
		return findSchema( name );
	}
} // namespace dnv::vista::sdk
