/**
 * @file CodebooksDto.h
 * @brief Data Transfer Objects for Codebooks JSON resources
 * @details Header-only DTOs for deserializing Codebooks data from Vista SDK resources.
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace dnv::vista::sdk
{
    /**
     * @brief DTO for a single Codebook
     * @details Represents a codebook with its name and mapping of codes to value arrays.
     */
    struct CodebookDto
    {
        using Values = std::unordered_map<std::string, std::vector<std::string>>;

        std::string name; ///< Codebook name
        Values values;    ///< Map of code to array of values

        /**
         * @brief Equality comparison operator
         * @param other Codebook to compare with
         * @return true if codebooks are equal
         */
        bool operator==( const CodebookDto& other ) const
        {
            return name == other.name && values == other.values;
        }

        /**
         * @brief Inequality comparison operator
         * @param other Codebook to compare with
         * @return true if codebooks are not equal
         */
        bool operator!=( const CodebookDto& other ) const
        {
            return !( *this == other );
        }
    };

    /**
     * @brief DTO for the complete Codebooks structure
     * @details Contains the VIS version and all codebook items.
     */
    struct CodebooksDto
    {
        using Items = std::vector<CodebookDto>;

        std::string visVersion; ///< VIS version string (maps to "visRelease" in JSON)
        Items items;            ///< All codebooks

        /**
         * @brief Equality comparison operator
         * @param other Codebooks to compare with
         * @return true if codebooks are equal
         */
        bool operator==( const CodebooksDto& other ) const
        {
            return visVersion == other.visVersion && items == other.items;
        }

        /**
         * @brief Inequality comparison operator
         * @param other Codebooks to compare with
         * @return true if codebooks are not equal
         */
        bool operator!=( const CodebooksDto& other ) const
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
     * @brief SerializationTraits for CodebookDto
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::CodebookDto>
    {
        /**
         * @brief High-performance streaming serialization
         * @param obj Object to serialize
         * @param builder Builder to write to
         */
        static void serialize( const dnv::vista::sdk::CodebookDto& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();
            builder.write( "name", obj.name );

            builder.writeKey( "values" );
            builder.writeStartObject();
            for( const auto& pair : obj.values )
            {
                builder.writeKey( pair.first );
                builder.writeStartArray();
                for( const auto& val : pair.second )
                {
                    builder.write( val );
                }
                builder.writeEndArray();
            }
            builder.writeEndObject();

            builder.writeEndObject();
        }

        /**
         * @brief Deserialize CodebookDto from JSON document
         * @param doc Document to deserialize from
         * @param obj Object to deserialize into
         * @throws std::runtime_error if required fields are missing or invalid
         */
        static void fromDocument( const nfx::json::Document& doc, dnv::vista::sdk::CodebookDto& obj )
        {
            auto nameOpt = doc.get<std::string>( "name" );
            if( !nameOpt )
            {
                throw std::runtime_error{ "CodebookDto: Missing required field 'name'" };
            }
            obj.name = std::move( *nameOpt );

            auto valuesObjOpt = doc.get<nfx::json::Object>( "values" );
            if( !valuesObjOpt )
            {
                throw std::runtime_error{ "CodebookDto: Missing required field 'values'" };
            }

            nfx::json::Document valuesDoc{ valuesObjOpt.value() };
            Serializer<dnv::vista::sdk::CodebookDto::Values> valuesSer;
            valuesSer.deserializeValue( valuesDoc, obj.values );
        }
    };

    /**
     * @brief SerializationTraits for CodebooksDto
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::CodebooksDto>
    {
        /**
         * @brief High-performance streaming serialization
         * @param obj Object to serialize
         * @param builder Builder to write to
         */
        static void serialize( const dnv::vista::sdk::CodebooksDto& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();
            builder.write( "visRelease", obj.visVersion );

            builder.writeKey( "items" );
            builder.writeStartArray();
            for( const auto& item : obj.items )
            {
                SerializationTraits<dnv::vista::sdk::CodebookDto>::serialize( item, builder );
            }
            builder.writeEndArray();

            builder.writeEndObject();
        }

        /**
         * @brief Deserialize CodebooksDto from JSON document
         * @param doc Document to deserialize from
         * @param obj Object to deserialize into
         * @throws std::runtime_error if required fields are missing or invalid
         * @note JSON field "visRelease" maps to member visVersion
         */
        static void fromDocument( const nfx::json::Document& doc, dnv::vista::sdk::CodebooksDto& obj )
        {
            auto visReleaseOpt = doc.get<std::string>( "visRelease" );
            if( !visReleaseOpt )
            {
                throw std::runtime_error{ "CodebooksDto: Missing required field 'visRelease'" };
            }
            obj.visVersion = std::move( *visReleaseOpt );

            if( obj.visVersion.empty() )
            {
                throw std::runtime_error{ "CodebooksDto: Field 'visRelease' cannot be empty" };
            }

            auto itemsArrOpt = doc.get<nfx::json::Array>( "items" );
            if( !itemsArrOpt )
            {
                throw std::runtime_error{ "CodebooksDto: Missing required field 'items'" };
            }

            nfx::json::Document itemsDoc{ itemsArrOpt.value() };
            Serializer<dnv::vista::sdk::CodebooksDto::Items> itemsSer;
            itemsSer.deserializeValue( itemsDoc, obj.items );
        }
    };
} // namespace nfx::serialization::json
