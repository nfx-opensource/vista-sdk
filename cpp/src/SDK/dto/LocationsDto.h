/**
 * @file LocationsDto.h
 * @brief Data Transfer Objects for Locations JSON resources
 * @details Header-only DTOs for deserializing Locations data from Vista SDK resources.
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>

#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace dnv::vista::sdk
{
    /**
     * @brief DTO for a single relative location
     * @details Represents a relative location with its code, name, and optional definition.
     */
    struct RelativeLocationsDto
    {
        char code;                             ///< Location code (single character)
        std::string name;                      ///< Location name
        std::optional<std::string> definition; ///< Optional definition

        /**
         * @brief Equality comparison operator
         * @param other Relative location to compare with
         * @return true if relative locations are equal
         */
        bool operator==( const RelativeLocationsDto& other ) const
        {
            return code == other.code && name == other.name && definition == other.definition;
        }

        /**
         * @brief Inequality comparison operator
         * @param other Relative location to compare with
         * @return true if relative locations are not equal
         */
        bool operator!=( const RelativeLocationsDto& other ) const
        {
            return !( *this == other );
        }
    };

    /**
     * @brief DTO for the complete Locations structure
     * @details Contains the VIS version and all relative location items.
     */
    struct LocationsDto
    {
        using Items = std::vector<RelativeLocationsDto>;

        std::string visVersion; ///< VIS version string (maps to "visRelease" in JSON)
        Items items;            ///< All relative locations

        /**
         * @brief Equality comparison operator
         * @param other Locations to compare with
         * @return true if locations are equal
         */
        bool operator==( const LocationsDto& other ) const
        {
            return visVersion == other.visVersion && items == other.items;
        }

        /**
         * @brief Inequality comparison operator
         * @param other Locations to compare with
         * @return true if locations are not equal
         */
        bool operator!=( const LocationsDto& other ) const
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
     * @brief SerializationTraits for RelativeLocationsDto
     * @details High-performance streaming serialization with bidirectional support
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::RelativeLocationsDto>
    {
        /**
         * @brief High-performance streaming serialization
         * @param obj Object to serialize
         * @param builder Builder to write to
         */
        static void serialize( const dnv::vista::sdk::RelativeLocationsDto& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();
            builder.write( "code", std::string( 1, obj.code ) );
            builder.write( "name", obj.name );
            if( obj.definition )
                builder.write( "definition", *obj.definition );
            builder.writeEndObject();
        }

        /**
         * @brief Deserialize RelativeLocationsDto from JSON document
         * @param doc Document to deserialize from
         * @param obj Object to deserialize into
         * @throws std::runtime_error if required fields are missing or invalid
         */
        static void fromDocument( const nfx::json::Document& doc, dnv::vista::sdk::RelativeLocationsDto& obj )
        {
            auto codeOpt = doc.get<std::string>( "code" );
            if( !codeOpt || codeOpt->empty() )
            {
                throw std::runtime_error{ "RelativeLocationsDto: Missing required field 'code'" };
            }
            obj.code = ( *codeOpt )[0];

            auto nameOpt = doc.get<std::string>( "name" );
            if( !nameOpt )
            {
                throw std::runtime_error{ "RelativeLocationsDto: Missing required field 'name'" };
            }
            obj.name = std::move( *nameOpt );

            if( auto val = doc.get<std::string>( "definition" ) )
            {
                obj.definition = std::move( *val );
            }
        }
    };

    /**
     * @brief SerializationTraits for LocationsDto
     * @details High-performance streaming serialization with bidirectional support
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::LocationsDto>
    {
        /**
         * @brief High-performance streaming serialization
         * @param obj Object to serialize
         * @param builder Builder to write to
         */
        static void serialize( const dnv::vista::sdk::LocationsDto& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();
            builder.write( "visRelease", obj.visVersion );

            builder.writeKey( "items" );
            builder.writeStartArray();
            for( const auto& item : obj.items )
            {
                SerializationTraits<dnv::vista::sdk::RelativeLocationsDto>::serialize( item, builder );
            }
            builder.writeEndArray();

            builder.writeEndObject();
        }

        /**
         * @brief Deserialize LocationsDto from JSON document
         * @param doc Document to deserialize from
         * @param obj Object to deserialize into
         * @throws std::runtime_error if required fields are missing or invalid
         * @note JSON field "visRelease" maps to member visVersion
         */
        static void fromDocument( const nfx::json::Document& doc, dnv::vista::sdk::LocationsDto& obj )
        {
            auto visReleaseOpt = doc.get<std::string>( "visRelease" );
            if( !visReleaseOpt )
            {
                throw std::runtime_error{ "LocationsDto: Missing required field 'visRelease'" };
            }
            obj.visVersion = std::move( *visReleaseOpt );

            if( obj.visVersion.empty() )
            {
                throw std::runtime_error{ "LocationsDto: Field 'visRelease' cannot be empty" };
            }

            auto itemsArrOpt = doc.get<nfx::json::Array>( "items" );
            if( !itemsArrOpt )
            {
                throw std::runtime_error{ "LocationsDto: Missing required field 'items'" };
            }
            nfx::json::Document itemsDoc{ itemsArrOpt.value() };
            Serializer<dnv::vista::sdk::LocationsDto::Items> itemsSer;
            itemsSer.deserializeValue( itemsDoc, obj.items );
        }
    };
} // namespace nfx::serialization::json
