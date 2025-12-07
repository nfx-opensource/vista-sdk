/**
 * @file GmodDto.h
 * @brief Data Transfer Objects for Gmod (Generic product model) JSON resources
 * @details Header-only DTOs for deserializing Gmod data from Vista SDK resources.
 */

#pragma once

#include <nfx/Serialization.h>

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace dnv::vista::sdk
{
    /**
     * @brief DTO for a single Gmod node
     * @details Represents a node in the Generic product model hierarchy with its
     *          category, type, code, name, and optional metadata.
     */
    struct GmodNodeDto
    {
        using NormalAssignmentNames = std::unordered_map<std::string, std::string>;

        std::string category;                                       ///< Node category
        std::string type;                                           ///< Node type
        std::string code;                                           ///< Node code identifier
        std::optional<std::string> name;                            ///< Optional specific name
        std::optional<std::string> commonName;                      ///< Optional common/generic name
        std::optional<std::string> definition;                      ///< Optional definition
        std::optional<std::string> commonDefinition;                ///< Optional common definition
        std::optional<bool> installSubstructure;                    ///< Optional install substructure flag
        std::optional<NormalAssignmentNames> normalAssignmentNames; ///< Optional assignment names map

        /**
         * @brief Equality comparison operator
         * @param other Node to compare with
         * @return true if nodes are equal
         */
        bool operator==( const GmodNodeDto& other ) const
        {
            return category == other.category && type == other.type && code == other.code && name == other.name &&
                   commonName == other.commonName && definition == other.definition &&
                   commonDefinition == other.commonDefinition && installSubstructure == other.installSubstructure &&
                   normalAssignmentNames == other.normalAssignmentNames;
        }

        /**
         * @brief Inequality comparison operator
         * @param other Node to compare with
         * @return true if nodes are not equal
         */
        bool operator!=( const GmodNodeDto& other ) const
        {
            return !( *this == other );
        }
    };

    /**
     * @brief DTO for the complete Gmod (Generic product model) structure
     * @details Contains the VIS version, all Gmod nodes (items), and their relationships.
     */
    struct GmodDto
    {
        using Items = std::vector<GmodNodeDto>;
        using Relations = std::vector<std::vector<std::string>>;

        std::string visVersion; ///< VIS version string (maps to "visRelease" in JSON)
        Items items;            ///< All Gmod nodes
        Relations relations;    ///< Node relationships

        /**
         * @brief Equality comparison operator
         * @param other Gmod to compare with
         * @return true if Gmods are equal
         */
        bool operator==( const GmodDto& other ) const
        {
            return visVersion == other.visVersion && items == other.items && relations == other.relations;
        }

        /**
         * @brief Inequality comparison operator
         * @param other Gmod to compare with
         * @return true if Gmods are not equal
         */
        bool operator!=( const GmodDto& other ) const
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
     * @brief Serialization traits for GmodNodeDto
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::GmodNodeDto>
    {
        /**
         * @brief Serialize GmodNodeDto to JSON document
         * @param obj Object to serialize
         * @param doc Document to serialize into
         */
        static void serialize( const dnv::vista::sdk::GmodNodeDto& obj, Document& doc )
        {
            // Direct access to Object variant - much faster than JSON Pointer parsing
            doc["category"] = obj.category;
            doc["type"] = obj.type;
            doc["code"] = obj.code;

            if( obj.name )
            {
                doc["name"] = *obj.name;
            }
            if( obj.commonName )
            {
                doc["commonName"] = *obj.commonName;
            }
            if( obj.definition )
            {
                doc["definition"] = *obj.definition;
            }
            if( obj.commonDefinition )
            {
                doc["commonDefinition"] = *obj.commonDefinition;
            }
            if( obj.installSubstructure )
            {
                doc["installSubstructure"] = *obj.installSubstructure;
            }

            if( obj.normalAssignmentNames )
            {
                Serializer<dnv::vista::sdk::GmodNodeDto::NormalAssignmentNames> namesSerializer;
                doc["normalAssignmentNames"] = namesSerializer.serialize( *obj.normalAssignmentNames );
            }
        }

        /**
         * @brief Deserialize GmodNodeDto from JSON document
         * @param obj Object to deserialize into
         * @param doc Document to deserialize from
         * @throws std::runtime_error if required fields are missing or invalid
         */
        static void deserialize( dnv::vista::sdk::GmodNodeDto& obj, const Document& doc )
        {
            auto categoryOpt = doc["category"].root<std::string>();
            if( !categoryOpt )
            {
                throw std::runtime_error{ "GmodNodeDto: Missing required field 'category'" };
            }
            obj.category = std::move( *categoryOpt );

            auto typeOpt = doc["type"].root<std::string>();
            if( !typeOpt )
            {
                throw std::runtime_error{ "GmodNodeDto: Missing required field 'type'" };
            }
            obj.type = std::move( *typeOpt );

            auto codeOpt = doc["code"].root<std::string>();
            if( !codeOpt )
            {
                throw std::runtime_error{ "GmodNodeDto: Missing required field 'code'" };
            }
            obj.code = std::move( *codeOpt );

            // Optional fields
            // Note: 'name' is optional because some nodes (e.g., installSubstructure nodes) may not have it in the JSON
            if( auto val = doc["name"].root<std::string>() )
            {
                obj.name = std::move( *val );
            }

            if( auto val = doc["commonName"].root<std::string>() )
            {
                obj.commonName = std::move( *val );
            }
            if( auto val = doc["definition"].root<std::string>() )
            {
                obj.definition = std::move( *val );
            }
            if( auto val = doc["commonDefinition"].root<std::string>() )
            {
                obj.commonDefinition = std::move( *val );
            }
            if( auto val = doc["installSubstructure"].root<bool>() )
            {
                obj.installSubstructure = *val;
            }

            if( doc.contains( "normalAssignmentNames" ) )
            {
                Serializer<dnv::vista::sdk::GmodNodeDto::NormalAssignmentNames> namesSerializer;
                obj.normalAssignmentNames = namesSerializer.deserialize( doc["normalAssignmentNames"] );
            }
        }
    };

    /**
     * @brief Serialization traits for GmodDto
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::GmodDto>
    {
        /**
         * @brief Serialize GmodDto to JSON document
         * @param obj Object to serialize
         * @param doc Document to serialize into
         */
        static void serialize( const dnv::vista::sdk::GmodDto& obj, Document& doc )
        {
            // Direct access - faster than JSON Pointer
            doc["visRelease"] = obj.visVersion;

            Serializer<dnv::vista::sdk::GmodDto::Items> itemsSerializer;
            doc["items"] = itemsSerializer.serialize( obj.items );

            Serializer<dnv::vista::sdk::GmodDto::Relations> relationsSerializer;
            doc["relations"] = relationsSerializer.serialize( obj.relations );
        }

        /**
         * @brief Deserialize GmodDto from JSON document
         * @param obj Object to deserialize into
         * @param doc Document to deserialize from
         * @throws std::runtime_error if required fields are missing or invalid
         * @note JSON field "visRelease" maps to member visVersion
         */
        static void deserialize( dnv::vista::sdk::GmodDto& obj, const Document& doc )
        {
            auto visReleaseOpt = doc["visRelease"].root<std::string>();
            if( !visReleaseOpt )
            {
                throw std::runtime_error{ "GmodDto: Missing required field 'visRelease'" };
            }
            obj.visVersion = std::move( *visReleaseOpt );

            if( obj.visVersion.empty() )
            {
                throw std::runtime_error{ "GmodDto: Field 'visRelease' cannot be empty" };
            }

            if( !doc.contains( "items" ) )
            {
                throw std::runtime_error{ "GmodDto: Missing required field 'items'" };
            }

            Serializer<dnv::vista::sdk::GmodDto::Items> itemsSerializer;
            obj.items = itemsSerializer.deserialize( doc["items"] );

            if( !doc.contains( "relations" ) )
            {
                throw std::runtime_error{ "GmodDto: Missing required field 'relations'" };
            }

            Serializer<dnv::vista::sdk::GmodDto::Relations> relationsSerializer;
            obj.relations = relationsSerializer.deserialize( doc["relations"] );
        }
    };
} // namespace nfx::serialization::json
