/**
 * @file GmodDto.h
 * @brief Data Transfer Objects for Gmod (Generic product model) JSON resources
 * @details Header-only DTOs for deserializing Gmod data from Vista SDK resources.
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>

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
     * @brief SerializationTraits for GmodNodeDto
     * @details High-performance streaming serialization with bidirectional support
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::GmodNodeDto>
    {
        /**
         * @brief High-performance streaming serialization
         * @param obj Object to serialize
         * @param builder Builder to write to
         */
        static void serialize( const dnv::vista::sdk::GmodNodeDto& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();
            builder.write( "category", obj.category );
            builder.write( "type", obj.type );
            builder.write( "code", obj.code );

            // Optional fields
            if( obj.name )
                builder.write( "name", *obj.name );
            if( obj.commonName )
                builder.write( "commonName", *obj.commonName );
            if( obj.definition )
                builder.write( "definition", *obj.definition );
            if( obj.commonDefinition )
                builder.write( "commonDefinition", *obj.commonDefinition );
            if( obj.installSubstructure )
                builder.write( "installSubstructure", *obj.installSubstructure );

            // Serialize normalAssignmentNames vector
            if( obj.normalAssignmentNames )
            {
                builder.writeKey( "normalAssignmentNames" );
                builder.writeStartObject();
                for( const auto& pair : *obj.normalAssignmentNames )
                {
                    builder.write( pair.first, pair.second );
                }
                builder.writeEndObject();
            }

            builder.writeEndObject();
        }

        /**
         * @brief Deserialize GmodNodeDto from JSON document
         * @param doc Document to deserialize from
         * @param obj Object to deserialize into
         * @throws std::runtime_error if required fields are missing or invalid
         */
        static void fromDocument( const nfx::json::Document& doc, dnv::vista::sdk::GmodNodeDto& obj )
        {
            auto categoryOpt = doc.get<std::string>( "category" );
            if( !categoryOpt )
            {
                throw std::runtime_error{ "GmodNodeDto: Missing required field 'category'" };
            }
            obj.category = std::move( *categoryOpt );

            auto typeOpt = doc.get<std::string>( "type" );
            if( !typeOpt )
            {
                throw std::runtime_error{ "GmodNodeDto: Missing required field 'type'" };
            }
            obj.type = std::move( *typeOpt );

            auto codeOpt = doc.get<std::string>( "code" );
            if( !codeOpt )
            {
                throw std::runtime_error{ "GmodNodeDto: Missing required field 'code'" };
            }
            obj.code = std::move( *codeOpt );

            // Optional fields
            if( auto val = doc.get<std::string>( "name" ) )
                obj.name = std::move( *val );
            if( auto val = doc.get<std::string>( "commonName" ) )
                obj.commonName = std::move( *val );
            if( auto val = doc.get<std::string>( "definition" ) )
                obj.definition = std::move( *val );
            if( auto val = doc.get<std::string>( "commonDefinition" ) )
                obj.commonDefinition = std::move( *val );
            if( auto val = doc.get<bool>( "installSubstructure" ) )
                obj.installSubstructure = *val;

            if( doc.contains( "normalAssignmentNames" ) )
            {
                auto namesObjOpt = doc.get<nfx::json::Object>( "normalAssignmentNames" );
                if( namesObjOpt )
                {
                    nfx::json::Document namesDoc{ namesObjOpt.value() };
                    Serializer<dnv::vista::sdk::GmodNodeDto::NormalAssignmentNames> namesSer;
                    dnv::vista::sdk::GmodNodeDto::NormalAssignmentNames names;
                    namesSer.deserializeValue( namesDoc, names );
                    obj.normalAssignmentNames = std::move( names );
                }
            }
        }
    };

    /**
     * @brief SerializationTraits for GmodDto
     * @details High-performance streaming serialization with bidirectional support
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::GmodDto>
    {
        /**
         * @brief High-performance streaming serialization
         * @param obj Object to serialize
         * @param builder Builder to write to
         */
        static void serialize( const dnv::vista::sdk::GmodDto& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();
            builder.write( "visRelease", obj.visVersion );

            builder.writeKey( "items" );
            builder.writeStartArray();
            for( const auto& item : obj.items )
            {
                SerializationTraits<dnv::vista::sdk::GmodNodeDto>::serialize( item, builder );
            }
            builder.writeEndArray();

            builder.writeKey( "relations" );
            builder.writeStartArray();
            for( const auto& relationArray : obj.relations )
            {
                builder.writeStartArray();
                for( const auto& val : relationArray )
                {
                    builder.write( val );
                }
                builder.writeEndArray();
            }
            builder.writeEndArray();

            builder.writeEndObject();
        }

        /**
         * @brief Deserialize GmodDto from JSON document
         * @param doc Document to deserialize from
         * @param obj Object to deserialize into
         * @throws std::runtime_error if required fields are missing or invalid
         * @note JSON field "visRelease" maps to member visVersion
         */
        static void fromDocument( const nfx::json::Document& doc, dnv::vista::sdk::GmodDto& obj )
        {
            auto visReleaseOpt = doc.get<std::string>( "visRelease" );
            if( !visReleaseOpt )
            {
                throw std::runtime_error{ "GmodDto: Missing required field 'visRelease'" };
            }
            obj.visVersion = std::move( *visReleaseOpt );

            if( obj.visVersion.empty() )
            {
                throw std::runtime_error{ "GmodDto: Field 'visRelease' cannot be empty" };
            }

            auto itemsArrOpt = doc.get<nfx::json::Array>( "items" );
            if( !itemsArrOpt )
            {
                throw std::runtime_error{ "GmodDto: Missing required field 'items'" };
            }
            nfx::json::Document itemsDoc{ itemsArrOpt.value() };
            Serializer<dnv::vista::sdk::GmodDto::Items> itemsSer;
            itemsSer.deserializeValue( itemsDoc, obj.items );

            auto relationsArrOpt = doc.get<nfx::json::Array>( "relations" );
            if( !relationsArrOpt )
            {
                throw std::runtime_error{ "GmodDto: Missing required field 'relations'" };
            }
            nfx::json::Document relationsDoc{ relationsArrOpt.value() };
            Serializer<dnv::vista::sdk::GmodDto::Relations> relationsSer;
            relationsSer.deserializeValue( relationsDoc, obj.relations );
        }
    };
} // namespace nfx::serialization::json
