/**
 * @file CodebooksDto.h
 * @brief Data Transfer Objects for Codebooks JSON resources
 * @details Header-only DTOs for deserializing Codebooks data from Vista SDK resources.
 */

#pragma once

#include <nfx/Serialization.h>

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
     * @brief Serialization traits for CodebookDto
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::CodebookDto>
    {
        /**
         * @brief Serialize CodebookDto to JSON document
         * @param obj Object to serialize
         * @param doc Document to serialize into
         */
        static void serialize( const dnv::vista::sdk::CodebookDto& obj, Document& doc )
        {
            doc["name"] = obj.name;

            Serializer<dnv::vista::sdk::CodebookDto::Values> valuesSerializer;
            doc["values"] = valuesSerializer.serialize( obj.values );
        }

        /**
         * @brief Deserialize CodebookDto from JSON document
         * @param obj Object to deserialize into
         * @param doc Document to deserialize from
         * @throws std::runtime_error if required fields are missing or invalid
         */
        static void deserialize( dnv::vista::sdk::CodebookDto& obj, const Document& doc )
        {
            auto nameOpt = doc["name"].root<std::string>();
            if( !nameOpt )
            {
                throw std::runtime_error{ "CodebookDto: Missing required field 'name'" };
            }
            obj.name = std::move( *nameOpt );

            if( !doc.contains( "values" ) )
            {
                throw std::runtime_error{ "CodebookDto: Missing required field 'values'" };
            }

            Serializer<dnv::vista::sdk::CodebookDto::Values> valuesSerializer;
            obj.values = valuesSerializer.deserialize( doc["values"] );
        }
    };

    /**
     * @brief Serialization traits for CodebooksDto
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::CodebooksDto>
    {
        /**
         * @brief Serialize CodebooksDto to JSON document
         * @param obj Object to serialize
         * @param doc Document to serialize into
         */
        static void serialize( const dnv::vista::sdk::CodebooksDto& obj, Document& doc )
        {
            doc["visRelease"] = obj.visVersion;

            Serializer<dnv::vista::sdk::CodebooksDto::Items> itemsSerializer;
            doc["items"] = itemsSerializer.serialize( obj.items );
        }

        /**
         * @brief Deserialize CodebooksDto from JSON document
         * @param obj Object to deserialize into
         * @param doc Document to deserialize from
         * @throws std::runtime_error if required fields are missing or invalid
         * @note JSON field "visRelease" maps to member visVersion
         */
        static void deserialize( dnv::vista::sdk::CodebooksDto& obj, const Document& doc )
        {
            auto visReleaseOpt = doc["visRelease"].root<std::string>();
            if( !visReleaseOpt )
            {
                throw std::runtime_error{ "CodebooksDto: Missing required field 'visRelease'" };
            }
            obj.visVersion = std::move( *visReleaseOpt );

            if( obj.visVersion.empty() )
            {
                throw std::runtime_error{ "CodebooksDto: Field 'visRelease' cannot be empty" };
            }

            if( !doc.contains( "items" ) )
            {
                throw std::runtime_error{ "CodebooksDto: Missing required field 'items'" };
            }

            Serializer<dnv::vista::sdk::CodebooksDto::Items> itemsSerializer;
            obj.items = itemsSerializer.deserialize( doc["items"] );
        }
    };
} // namespace nfx::serialization::json
