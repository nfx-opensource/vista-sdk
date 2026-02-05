/**
 * @file ISO19848Dtos.h
 * @brief Data Transfer Objects for ISO 19848 JSON resources
 * @details Header-only DTOs for deserializing ISO 19848 data channel type names and format data types from Vista SDK
 * resources.
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>

#include <stdexcept>
#include <string>
#include <vector>

namespace dnv::vista::sdk
{
    /**
     * @brief DTO for a single data channel type name
     * @details Represents a data channel type with its type identifier and description.
     */
    struct DataChannelTypeNameDto
    {
        std::string type;        ///< Data channel type identifier
        std::string description; ///< Data channel type description

        /**
         * @brief Equality comparison operator
         * @param other Data channel type name to compare with
         * @return true if data channel type names are equal
         */
        bool operator==( const DataChannelTypeNameDto& other ) const
        {
            return type == other.type && description == other.description;
        }

        /**
         * @brief Inequality comparison operator
         * @param other Data channel type name to compare with
         * @return true if data channel type names are not equal
         */
        bool operator!=( const DataChannelTypeNameDto& other ) const
        {
            return !( *this == other );
        }
    };

    /**
     * @brief DTO for the complete data channel type names structure
     * @details Contains all data channel type name entries for ISO 19848.
     */
    struct DataChannelTypeNamesDto
    {
        using Values = std::vector<DataChannelTypeNameDto>;

        Values values; ///< All data channel type names

        /**
         * @brief Equality comparison operator
         * @param other Data channel type names to compare with
         * @return true if data channel type names are equal
         */
        bool operator==( const DataChannelTypeNamesDto& other ) const
        {
            return values == other.values;
        }

        /**
         * @brief Inequality comparison operator
         * @param other Data channel type names to compare with
         * @return true if data channel type names are not equal
         */
        bool operator!=( const DataChannelTypeNamesDto& other ) const
        {
            return !( *this == other );
        }
    };

    /**
     * @brief DTO for a single format data type
     * @details Represents a format data type with its type identifier and description.
     */
    struct FormatDataTypeDto
    {
        std::string type;        ///< Format data type identifier
        std::string description; ///< Format data type description

        /**
         * @brief Equality comparison operator
         * @param other Format data type to compare with
         * @return true if format data types are equal
         */
        bool operator==( const FormatDataTypeDto& other ) const
        {
            return type == other.type && description == other.description;
        }

        /**
         * @brief Inequality comparison operator
         * @param other Format data type to compare with
         * @return true if format data types are not equal
         */
        bool operator!=( const FormatDataTypeDto& other ) const
        {
            return !( *this == other );
        }
    };

    /**
     * @brief DTO for the complete format data types structure
     * @details Contains all format data type entries for ISO 19848.
     */
    struct FormatDataTypesDto
    {
        using Values = std::vector<FormatDataTypeDto>;

        Values values; ///< All format data types

        /**
         * @brief Equality comparison operator
         * @param other Format data types to compare with
         * @return true if format data types are equal
         */
        bool operator==( const FormatDataTypesDto& other ) const
        {
            return values == other.values;
        }

        /**
         * @brief Inequality comparison operator
         * @param other Format data types to compare with
         * @return true if format data types are not equal
         */
        bool operator!=( const FormatDataTypesDto& other ) const
        {
            return !( *this == other );
        }
    };
} // namespace dnv::vista::sdk

//=====================================================================
// nfx SerializationTraits specializations
//=====================================================================

namespace nfx::serialization::json
{
    /**
     * @brief SerializationTraits for DataChannelTypeNameDto
     * @details High-performance streaming serialization with bidirectional support
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::DataChannelTypeNameDto>
    {
        static void serialize( const dnv::vista::sdk::DataChannelTypeNameDto& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();
            builder.write( "type", obj.type );
            builder.write( "description", obj.description );
            builder.writeEndObject();
        }

        static void fromDocument( const nfx::json::Document& doc, dnv::vista::sdk::DataChannelTypeNameDto& obj )
        {
            auto typeOpt = doc.get<std::string>( "type" );
            if( !typeOpt )
            {
                throw std::runtime_error{ "DataChannelTypeNameDto: Missing required field 'type'" };
            }
            obj.type = std::move( *typeOpt );

            auto descriptionOpt = doc.get<std::string>( "description" );
            if( !descriptionOpt )
            {
                throw std::runtime_error{ "DataChannelTypeNameDto: Missing required field 'description'" };
            }
            obj.description = std::move( *descriptionOpt );
        }
    };

    /**
     * @brief SerializationTraits for DataChannelTypeNamesDto
     * @details High-performance streaming serialization with bidirectional support
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::DataChannelTypeNamesDto>
    {
        static void serialize( const dnv::vista::sdk::DataChannelTypeNamesDto& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            builder.writeKey( "values" );
            builder.writeStartArray();
            for( const auto& item : obj.values )
            {
                SerializationTraits<dnv::vista::sdk::DataChannelTypeNameDto>::serialize( item, builder );
            }
            builder.writeEndArray();

            builder.writeEndObject();
        }

        static void fromDocument( const nfx::json::Document& doc, dnv::vista::sdk::DataChannelTypeNamesDto& obj )
        {
            auto valuesArrOpt = doc.get<nfx::json::Array>( "values" );
            if( !valuesArrOpt )
            {
                throw std::runtime_error{ "DataChannelTypeNamesDto: Missing required field 'values'" };
            }
            nfx::json::Document valuesDoc{ valuesArrOpt.value() };
            Serializer<dnv::vista::sdk::DataChannelTypeNamesDto::Values> valuesSer;
            valuesSer.deserializeValue( valuesDoc, obj.values );
        }
    };

    /**
     * @brief SerializationTraits for FormatDataTypeDto
     * @details High-performance streaming serialization with bidirectional support
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::FormatDataTypeDto>
    {
        static void serialize( const dnv::vista::sdk::FormatDataTypeDto& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();
            builder.write( "type", obj.type );
            builder.write( "description", obj.description );
            builder.writeEndObject();
        }

        static void fromDocument( const nfx::json::Document& doc, dnv::vista::sdk::FormatDataTypeDto& obj )
        {
            auto typeOpt = doc.get<std::string>( "type" );
            if( !typeOpt )
            {
                throw std::runtime_error{ "FormatDataTypeDto: Missing required field 'type'" };
            }
            obj.type = std::move( *typeOpt );

            auto descriptionOpt = doc.get<std::string>( "description" );
            if( !descriptionOpt )
            {
                throw std::runtime_error{ "FormatDataTypeDto: Missing required field 'description'" };
            }
            obj.description = std::move( *descriptionOpt );
        }
    };

    /**
     * @brief SerializationTraits for FormatDataTypesDto
     * @details High-performance streaming serialization with bidirectional support
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::FormatDataTypesDto>
    {
        static void serialize( const dnv::vista::sdk::FormatDataTypesDto& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            builder.writeKey( "values" );
            builder.writeStartArray();
            for( const auto& item : obj.values )
            {
                SerializationTraits<dnv::vista::sdk::FormatDataTypeDto>::serialize( item, builder );
            }
            builder.writeEndArray();

            builder.writeEndObject();
        }

        static void fromDocument( const nfx::json::Document& doc, dnv::vista::sdk::FormatDataTypesDto& obj )
        {
            auto valuesArrOpt = doc.get<nfx::json::Array>( "values" );
            if( !valuesArrOpt )
            {
                throw std::runtime_error{ "FormatDataTypesDto: Missing required field 'values'" };
            }
            nfx::json::Document valuesDoc{ valuesArrOpt.value() };
            Serializer<dnv::vista::sdk::FormatDataTypesDto::Values> valuesSer;
            valuesSer.deserializeValue( valuesDoc, obj.values );
        }
    };
} // namespace nfx::serialization::json
