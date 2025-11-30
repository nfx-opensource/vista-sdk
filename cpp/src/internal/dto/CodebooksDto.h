#pragma once

#include <string>
#include <vector>

#include <nfx/Containers.h>
#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/SerializationTraits.h>
#include <nfx/serialization/json/extensions/ContainersTraits.h>

namespace dnv::vista::sdk
{
	struct CodebookDto final
	{
		using ValueGroup = std::vector<std::string>;
		using ValuesMap = nfx::containers::FastHashMap<std::string, ValueGroup>;

		std::string name;
		ValuesMap values;
	};

	struct CodebooksDto final
	{
		using Items = std::vector<CodebookDto>;

		std::string visVersion;
		Items items;

		static CodebooksDto fromJsonString( std::string_view jsonStr )
		{
			nfx::serialization::json::Serializer<CodebooksDto> serializer;
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
	struct SerializationTraits<dnv::vista::sdk::CodebookDto>
	{
		static void serialize( const dnv::vista::sdk::CodebookDto& obj, Document& doc )
		{
			doc.set( "/name", obj.name );

			Serializer<dnv::vista::sdk::CodebookDto::ValuesMap> valuesSerializer;
			Document valuesDoc = valuesSerializer.serialize( obj.values );
			doc.set( "/values", valuesDoc );
		}

		static void deserialize( dnv::vista::sdk::CodebookDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/name" ) )
			{
				obj.name = *val;
			}

			if ( auto valuesDoc = doc.get<Document>( "/values" ) )
			{
				Serializer<dnv::vista::sdk::CodebookDto::ValuesMap> valuesSerializer;
				obj.values = valuesSerializer.deserialize( *valuesDoc );
			}
		}
	};

	template <>
	struct SerializationTraits<dnv::vista::sdk::CodebooksDto>
	{
		static void serialize( const dnv::vista::sdk::CodebooksDto& obj, Document& doc )
		{
			doc.set( "/visVersion", obj.visVersion );

			Serializer<dnv::vista::sdk::CodebooksDto::Items> itemsSerializer;
			Document itemsDoc = itemsSerializer.serialize( obj.items );
			doc.set( "/items", itemsDoc );
		}

		static void deserialize( dnv::vista::sdk::CodebooksDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/visVersion" ) )
			{
				obj.visVersion = *val;
			}

			if ( auto itemsDoc = doc.get<Document>( "/items" ) )
			{
				Serializer<dnv::vista::sdk::CodebooksDto::Items> itemsSerializer;
				obj.items = itemsSerializer.deserialize( *itemsDoc );
			}
		}
	};
} // namespace nfx::serialization::json
