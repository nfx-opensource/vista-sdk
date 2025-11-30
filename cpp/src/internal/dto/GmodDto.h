#pragma once

#include <optional>
#include <string>
#include <vector>

#include <nfx/Containers.h>
#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/SerializationTraits.h>
#include <nfx/serialization/json/extensions/ContainersTraits.h>

namespace dnv::vista::sdk
{
	struct GmodNodeDto final
	{
		using NormalAssignmentNamesMap = nfx::containers::FastHashMap<std::string, std::string>;

		std::string category;
		std::string type;
		std::string code;
		std::string name;
		std::optional<std::string> commonName;
		std::optional<std::string> definition;
		std::optional<std::string> commonDefinition;
		std::optional<bool> installSubstructure;
		std::optional<NormalAssignmentNamesMap> normalAssignmentNames;
	};

	struct GmodDto final
	{
		using Relation = std::vector<std::string>;
		using Relations = std::vector<Relation>;
		using Items = std::vector<GmodNodeDto>;

		std::string visVersion;
		Items items;
		Relations relations;

		static GmodDto fromJsonString( std::string_view jsonStr )
		{
			nfx::serialization::json::Serializer<GmodDto> serializer;
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
	struct SerializationTraits<dnv::vista::sdk::GmodNodeDto>
	{
		static void serialize( const dnv::vista::sdk::GmodNodeDto& obj, Document& doc )
		{
			doc.set( "/category", obj.category );
			doc.set( "/type", obj.type );
			doc.set( "/code", obj.code );
			doc.set( "/name", obj.name );

			if ( obj.commonName )
			{
				doc.set( "/commonName", *obj.commonName );
			}
			if ( obj.definition )
			{
				doc.set( "/definition", *obj.definition );
			}
			if ( obj.commonDefinition )
			{
				doc.set( "/commonDefinition", *obj.commonDefinition );
			}
			if ( obj.installSubstructure )
			{
				doc.set( "/installSubstructure", *obj.installSubstructure );
			}

			if ( obj.normalAssignmentNames )
			{
				Serializer<dnv::vista::sdk::GmodNodeDto::NormalAssignmentNamesMap> serializer;
				Document namesDoc = serializer.serialize( *obj.normalAssignmentNames );
				doc.set( "/normalAssignmentNames", namesDoc );
			}
		}

		static void deserialize( dnv::vista::sdk::GmodNodeDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/category" ) )
			{
				obj.category = *val;
			}
			if ( auto val = doc.get<std::string>( "/type" ) )
			{
				obj.type = *val;
			}
			if ( auto val = doc.get<std::string>( "/code" ) )
			{
				obj.code = *val;
			}
			if ( auto val = doc.get<std::string>( "/name" ) )
			{
				obj.name = *val;
			}
			if ( auto val = doc.get<std::string>( "/commonName" ) )
			{
				obj.commonName = *val;
			}
			if ( auto val = doc.get<std::string>( "/definition" ) )
			{
				obj.definition = *val;
			}
			if ( auto val = doc.get<std::string>( "/commonDefinition" ) )
			{
				obj.commonDefinition = *val;
			}

			if ( auto val = doc.get<bool>( "/installSubstructure" ) )
			{
				obj.installSubstructure = *val;
			}

			if ( auto namesDoc = doc.get<Document>( "/normalAssignmentNames" ) )
			{
				Serializer<dnv::vista::sdk::GmodNodeDto::NormalAssignmentNamesMap> serializer;
				obj.normalAssignmentNames = serializer.deserialize( *namesDoc );
			}
		}
	};

	template <>
	struct SerializationTraits<dnv::vista::sdk::GmodDto>
	{
		static void serialize( const dnv::vista::sdk::GmodDto& obj, Document& doc )
		{
			doc.set( "/visVersion", obj.visVersion );

			Serializer<dnv::vista::sdk::GmodDto::Items> itemsSerializer;
			Document itemsDoc = itemsSerializer.serialize( obj.items );
			doc.set( "/items", itemsDoc );

			Serializer<dnv::vista::sdk::GmodDto::Relations> relationsSerializer;
			Document relationsDoc = relationsSerializer.serialize( obj.relations );
			doc.set( "/relations", relationsDoc );
		}

		static void deserialize( dnv::vista::sdk::GmodDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/visVersion" ) )
			{
				obj.visVersion = *val;
			}

			if ( auto itemsDoc = doc.get<Document>( "/items" ) )
			{
				Serializer<dnv::vista::sdk::GmodDto::Items> itemsSerializer;
				obj.items = itemsSerializer.deserialize( *itemsDoc );
			}

			if ( auto relationsDoc = doc.get<Document>( "/relations" ) )
			{
				Serializer<dnv::vista::sdk::GmodDto::Relations> relationsSerializer;
				obj.relations = relationsSerializer.deserialize( *relationsDoc );
			}
		}
	};
} // namespace nfx::serialization::json
