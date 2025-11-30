#pragma once

#include <string>
#include <vector>

#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/SerializationTraits.h>

namespace dnv::vista::sdk::internal::transport
{
	struct DataChannelTypeNameDto final
	{
		std::string type;
		std::string description;
	};

	struct DataChannelTypeNamesDto final
	{
		std::vector<DataChannelTypeNameDto> values;

		static DataChannelTypeNamesDto fromJsonString( std::string_view jsonStr )
		{
			nfx::serialization::json::Serializer<DataChannelTypeNamesDto> serializer;
			return serializer.deserializeFromString( jsonStr );
		}
	};

	struct FormatDataTypeDto final
	{
		std::string type;
		std::string description;
	};

	struct FormatDataTypesDto final
	{
		std::vector<FormatDataTypeDto> values;

		static FormatDataTypesDto fromJsonString( std::string_view jsonStr )
		{
			nfx::serialization::json::Serializer<FormatDataTypesDto> serializer;
			return serializer.deserializeFromString( jsonStr );
		}
	};
} // namespace dnv::vista::sdk::internal::transport

//=====================================================================
// nfx SerializationTraits specializations for reflection-based serialization
//=====================================================================

namespace nfx::serialization::json
{
	template <>
	struct SerializationTraits<dnv::vista::sdk::internal::transport::DataChannelTypeNameDto>
	{
		static void serialize( const dnv::vista::sdk::internal::transport::DataChannelTypeNameDto& obj, Document& doc )
		{
			doc.set( "/type", obj.type );
			doc.set( "/description", obj.description );
		}

		static void deserialize( dnv::vista::sdk::internal::transport::DataChannelTypeNameDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/type" ) )
			{
				obj.type = *val;
			}
			if ( auto val = doc.get<std::string>( "/description" ) )
			{
				obj.description = *val;
			}
		}
	};

	template <>
	struct SerializationTraits<dnv::vista::sdk::internal::transport::DataChannelTypeNamesDto>
	{
		static void serialize( const dnv::vista::sdk::internal::transport::DataChannelTypeNamesDto& obj, Document& doc )
		{
			Serializer<std::vector<dnv::vista::sdk::internal::transport::DataChannelTypeNameDto>> vectorSerializer;
			Document valuesDoc = vectorSerializer.serialize( obj.values );
			doc.set( "/values", valuesDoc );
		}

		static void deserialize( dnv::vista::sdk::internal::transport::DataChannelTypeNamesDto& obj, const Document& doc )
		{
			if ( auto valuesDoc = doc.get<Document>( "/values" ) )
			{
				Serializer<std::vector<dnv::vista::sdk::internal::transport::DataChannelTypeNameDto>> vectorSerializer;
				obj.values = vectorSerializer.deserialize( *valuesDoc );
			}
		}
	};

	template <>
	struct SerializationTraits<dnv::vista::sdk::internal::transport::FormatDataTypeDto>
	{
		static void serialize( const dnv::vista::sdk::internal::transport::FormatDataTypeDto& obj, Document& doc )
		{
			doc.set( "/type", obj.type );
			doc.set( "/description", obj.description );
		}

		static void deserialize( dnv::vista::sdk::internal::transport::FormatDataTypeDto& obj, const Document& doc )
		{
			if ( auto val = doc.get<std::string>( "/type" ) )
			{
				obj.type = *val;
			}
			if ( auto val = doc.get<std::string>( "/description" ) )
			{
				obj.description = *val;
			}
		}
	};

	template <>
	struct SerializationTraits<dnv::vista::sdk::internal::transport::FormatDataTypesDto>
	{
		static void serialize( const dnv::vista::sdk::internal::transport::FormatDataTypesDto& obj, Document& doc )
		{
			Serializer<std::vector<dnv::vista::sdk::internal::transport::FormatDataTypeDto>> vectorSerializer;
			Document valuesDoc = vectorSerializer.serialize( obj.values );
			doc.set( "/values", valuesDoc );
		}

		static void deserialize( dnv::vista::sdk::internal::transport::FormatDataTypesDto& obj, const Document& doc )
		{
			if ( auto valuesDoc = doc.get<Document>( "/values" ) )
			{
				Serializer<std::vector<dnv::vista::sdk::internal::transport::FormatDataTypeDto>> vectorSerializer;
				obj.values = vectorSerializer.deserialize( *valuesDoc );
			}
		}
	};
} // namespace nfx::serialization::json
