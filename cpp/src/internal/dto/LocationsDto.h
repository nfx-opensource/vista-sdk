#pragma once

#include <optional>
#include <string>
#include <vector>

#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/SerializationTraits.h>

namespace dnv::vista::sdk
{
	struct RelativeLocationsDto final
	{
		char code;
		std::string name;
		std::optional<std::string> definition;
	};

	struct LocationsDto final
	{
		std::string visVersion;
		std::vector<RelativeLocationsDto> items;

		static LocationsDto fromJsonString( std::string_view jsonStr )
		{
			nfx::serialization::json::Serializer<LocationsDto> serializer;
			return serializer.deserializeFromString( jsonStr );
		}
	};
} // namespace dnv::vista::sdk

//=====================================================================
// nfx SerializationTraits specializations - required for custom types
//=====================================================================

namespace nfx::serialization::json
{
	template <>
	struct SerializationTraits<dnv::vista::sdk::RelativeLocationsDto>
	{
		static void serialize( const dnv::vista::sdk::RelativeLocationsDto& obj, Document& doc )
		{
			doc.set( "/code", obj.code );
			doc.set( "/name", obj.name );

			if ( obj.definition )
			{
				doc.set( "/definition", *obj.definition );
			}
		}

		static void deserialize( dnv::vista::sdk::RelativeLocationsDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<char>( "/code" ) )
			{
				obj.code = *val;
			}
			if ( auto val = doc.get<std::string>( "/name" ) )
			{
				obj.name = *val;
			}
			if ( auto val = doc.get<std::string>( "/definition" ) )
			{
				obj.definition = *val;
			}
		}
	};

	template <>
	struct SerializationTraits<dnv::vista::sdk::LocationsDto>
	{
		static void serialize( const dnv::vista::sdk::LocationsDto& obj, Document& doc )
		{
			doc.set( "/visVersion", obj.visVersion );

			Serializer<std::vector<dnv::vista::sdk::RelativeLocationsDto>> itemsSerializer;
			Document itemsDoc = itemsSerializer.serialize( obj.items );
			doc.set( "/items", itemsDoc );
		}

		static void deserialize( dnv::vista::sdk::LocationsDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/visVersion" ) )
			{
				obj.visVersion = *val;
			}

			if ( auto itemsDoc = doc.get<Document>( "/items" ) )
			{
				Serializer<std::vector<dnv::vista::sdk::RelativeLocationsDto>> itemsSerializer;
				obj.items = itemsSerializer.deserialize( *itemsDoc );
			}
		}
	};
} // namespace nfx::serialization::json
