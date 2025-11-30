#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include <nfx/Containers.h>
#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/SerializationTraits.h>
#include <nfx/serialization/json/extensions/ContainersTraits.h>

namespace dnv::vista::sdk
{
	struct GmodVersioningAssignmentChangeDto final
	{
		std::string oldAssignment;
		std::string currentAssignment;
	};

	struct GmodNodeConversionDto final
	{
		using OperationSet = std::unordered_set<std::string>;

		OperationSet operations;
		std::string source;
		std::string target;
		std::string oldAssignment;
		std::string newAssignment;
		bool deleteAssignment;
	};

	struct GmodVersioningDto final
	{
		using ItemsMap = nfx::containers::FastHashMap<std::string, GmodNodeConversionDto>;

		std::string visVersion;
		ItemsMap items;

		// Simple nfx serialization methods
		static GmodVersioningDto fromJsonString( std::string_view jsonStr )
		{
			nfx::serialization::json::Serializer<GmodVersioningDto> serializer;
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
	struct SerializationTraits<dnv::vista::sdk::GmodVersioningAssignmentChangeDto>
	{
		static void serialize( const dnv::vista::sdk::GmodVersioningAssignmentChangeDto& obj, Document& doc )
		{
			doc.set( "/oldAssignment", obj.oldAssignment );
			doc.set( "/currentAssignment", obj.currentAssignment );
		}

		static void deserialize( dnv::vista::sdk::GmodVersioningAssignmentChangeDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/oldAssignment" ) )
			{
				obj.oldAssignment = *val;
			}
			if ( auto val = doc.get<std::string>( "/currentAssignment" ) )
			{
				obj.currentAssignment = *val;
			}
		}
	};

	template <>
	struct SerializationTraits<dnv::vista::sdk::GmodNodeConversionDto>
	{
		static void serialize( const dnv::vista::sdk::GmodNodeConversionDto& obj, Document& doc )
		{
			Serializer<dnv::vista::sdk::GmodNodeConversionDto::OperationSet> operationsSerializer;
			Document operationsDoc = operationsSerializer.serialize( obj.operations );
			doc.set( "/operations", operationsDoc );
			doc.set( "/source", obj.source );
			doc.set( "/target", obj.target );
			doc.set( "/oldAssignment", obj.oldAssignment );
			doc.set( "/newAssignment", obj.newAssignment );
			doc.set( "/deleteAssignment", obj.deleteAssignment );
		}

		static void deserialize( dnv::vista::sdk::GmodNodeConversionDto& obj, const Document& doc )
		{
			if ( auto operationsDoc = doc.get<Document>( "/operations" ) )
			{
				Serializer<dnv::vista::sdk::GmodNodeConversionDto::OperationSet> operationsSerializer;
				obj.operations = operationsSerializer.deserialize( *operationsDoc );
			}

			if ( auto val = doc.get<std::string>( "/source" ) )
			{
				obj.source = *val;
			}
			if ( auto val = doc.get<std::string>( "/target" ) )
			{
				obj.target = *val;
			}
			if ( auto val = doc.get<std::string>( "/oldAssignment" ) )
			{
				obj.oldAssignment = *val;
			}
			if ( auto val = doc.get<std::string>( "/newAssignment" ) )
			{
				obj.newAssignment = *val;
			}
			if ( auto val = doc.get<bool>( "/deleteAssignment" ) )
			{
				obj.deleteAssignment = *val;
			}
		}
	};

	template <>
	struct SerializationTraits<dnv::vista::sdk::GmodVersioningDto>
	{
		static void serialize( const dnv::vista::sdk::GmodVersioningDto& obj, Document& doc )
		{
			doc.set( "/visVersion", obj.visVersion );

			Serializer<dnv::vista::sdk::GmodVersioningDto::ItemsMap> itemsSerializer;
			Document itemsDoc = itemsSerializer.serialize( obj.items );
			doc.set( "/items", itemsDoc );
		}

		static void deserialize( dnv::vista::sdk::GmodVersioningDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/visVersion" ) )
			{
				obj.visVersion = *val;
			}

			if ( auto itemsDoc = doc.get<Document>( "/items" ) )
			{
				Serializer<dnv::vista::sdk::GmodVersioningDto::ItemsMap> itemsSerializer;
				obj.items = itemsSerializer.deserialize( *itemsDoc );
			}
		}
	};
} // namespace nfx::serialization::json
