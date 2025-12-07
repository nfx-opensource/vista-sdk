/**
 * @file LocationsDto.h
 * @brief Data Transfer Objects for Locations JSON resources
 * @details Header-only DTOs for deserializing Locations data from Vista SDK resources.
 */

#pragma once

#include <nfx/Serialization.h>

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
     * @brief Serialization traits for RelativeLocationsDto
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::RelativeLocationsDto>
    {
        /**
         * @brief Serialize RelativeLocationsDto to JSON document
         * @param obj Object to serialize
         * @param doc Document to serialize into
         */
        static void serialize( const dnv::vista::sdk::RelativeLocationsDto& obj, Document& doc )
        {
            doc["code"] = std::string( 1, obj.code );
            doc["name"] = obj.name;

            if( obj.definition )
            {
                doc["definition"] = *obj.definition;
            }
        }

        /**
         * @brief Deserialize RelativeLocationsDto from JSON document
         * @param obj Object to deserialize into
         * @param doc Document to deserialize from
         * @throws std::runtime_error if required fields are missing or invalid
         */
        static void deserialize( dnv::vista::sdk::RelativeLocationsDto& obj, const Document& doc )
        {
            auto codeOpt = doc["code"].root<std::string>();
            if( !codeOpt || codeOpt->empty() )
            {
                throw std::runtime_error{ "RelativeLocationsDto: Missing required field 'code'" };
            }
            obj.code = ( *codeOpt )[0];

            auto nameOpt = doc["name"].root<std::string>();
            if( !nameOpt )
            {
                throw std::runtime_error{ "RelativeLocationsDto: Missing required field 'name'" };
            }
            obj.name = std::move( *nameOpt );

            if( auto val = doc["definition"].root<std::string>() )
            {
                obj.definition = std::move( *val );
            }
        }
    };

    /**
     * @brief Serialization traits for LocationsDto
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::LocationsDto>
    {
        /**
         * @brief Serialize LocationsDto to JSON document
         * @param obj Object to serialize
         * @param doc Document to serialize into
         */
        static void serialize( const dnv::vista::sdk::LocationsDto& obj, Document& doc )
        {
            doc["visRelease"] = obj.visVersion;

            Serializer<dnv::vista::sdk::LocationsDto::Items> itemsSerializer;
            doc["items"] = itemsSerializer.serialize( obj.items );
        }

        /**
         * @brief Deserialize LocationsDto from JSON document
         * @param obj Object to deserialize into
         * @param doc Document to deserialize from
         * @throws std::runtime_error if required fields are missing or invalid
         * @note JSON field "visRelease" maps to member visVersion
         */
        static void deserialize( dnv::vista::sdk::LocationsDto& obj, const Document& doc )
        {
            auto visReleaseOpt = doc["visRelease"].root<std::string>();
            if( !visReleaseOpt )
            {
                throw std::runtime_error{ "LocationsDto: Missing required field 'visRelease'" };
            }
            obj.visVersion = std::move( *visReleaseOpt );

            if( obj.visVersion.empty() )
            {
                throw std::runtime_error{ "LocationsDto: Field 'visRelease' cannot be empty" };
            }

            if( !doc.contains( "items" ) )
            {
                throw std::runtime_error{ "LocationsDto: Missing required field 'items'" };
            }

            Serializer<dnv::vista::sdk::LocationsDto::Items> itemsSerializer;
            obj.items = itemsSerializer.deserialize( doc["items"] );
        }
    };
} // namespace nfx::serialization::json
