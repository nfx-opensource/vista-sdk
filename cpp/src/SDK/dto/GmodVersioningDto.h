/**
 * @file GmodVersioningDto.h
 * @brief Data Transfer Objects for Gmod Versioning JSON resources
 * @details Header-only DTOs for deserializing Gmod versioning data from Vista SDK resources.
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace dnv::vista::sdk
{
    /**
     * @brief DTO for Gmod node assignment changes
     * @details Represents changes in assignment names between versions.
     */
    struct GmodVersioningAssignmentChangeDto
    {
        std::string oldAssignment;     ///< Old assignment name
        std::string currentAssignment; ///< Current assignment name

        /**
         * @brief Equality comparison operator
         * @param other Assignment change to compare with
         * @return true if assignment changes are equal
         */
        bool operator==( const GmodVersioningAssignmentChangeDto& other ) const
        {
            return oldAssignment == other.oldAssignment && currentAssignment == other.currentAssignment;
        }

        /**
         * @brief Inequality comparison operator
         * @param other Assignment change to compare with
         * @return true if assignment changes are not equal
         */
        bool operator!=( const GmodVersioningAssignmentChangeDto& other ) const
        {
            return !( *this == other );
        }
    };

    /**
     * @brief DTO for Gmod node conversion information
     * @details Contains conversion operations and mappings between versions.
     */
    struct GmodNodeConversionDto
    {
        using Operations = std::unordered_set<std::string>;

        Operations operations;                        ///< Set of operations (e.g., "changeCode", "merge")
        std::optional<std::string> source;            ///< Source node identifier
        std::optional<std::string> target;            ///< Target node identifier
        std::optional<std::string> oldAssignment;     ///< Old assignment name
        std::optional<std::string> newAssignment;     ///< New assignment name
        std::optional<bool> deleteAssignment = false; ///< Whether assignment should be deleted

        /**
         * @brief Equality comparison operator
         * @param other Conversion to compare with
         * @return true if conversions are equal
         */
        bool operator==( const GmodNodeConversionDto& other ) const
        {
            return operations == other.operations && source == other.source && target == other.target &&
                   oldAssignment == other.oldAssignment && newAssignment == other.newAssignment &&
                   deleteAssignment == other.deleteAssignment;
        }

        /**
         * @brief Inequality comparison operator
         * @param other Conversion to compare with
         * @return true if conversions are not equal
         */
        bool operator!=( const GmodNodeConversionDto& other ) const
        {
            return !( *this == other );
        }
    };

    /**
     * @brief DTO for the complete Gmod versioning structure
     * @details Contains the VIS version and all node conversion mappings.
     */
    struct GmodVersioningDto
    {
        using Items = std::unordered_map<std::string, GmodNodeConversionDto>;

        std::string visVersion; ///< VIS version string (maps to "visRelease" in JSON)
        Items items;            ///< Map of node code to conversion information

        /**
         * @brief Equality comparison operator
         * @param other Versioning to compare with
         * @return true if versioning data are equal
         */
        bool operator==( const GmodVersioningDto& other ) const
        {
            return visVersion == other.visVersion && items == other.items;
        }

        /**
         * @brief Inequality comparison operator
         * @param other Versioning to compare with
         * @return true if versioning data are not equal
         */
        bool operator!=( const GmodVersioningDto& other ) const
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
     * @brief SerializationTraits for GmodVersioningAssignmentChangeDto
     * @details High-performance streaming serialization with bidirectional support
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::GmodVersioningAssignmentChangeDto>
    {
        /**
         * @brief High-performance streaming serialization
         * @param obj Object to serialize
         * @param builder Builder to write to
         */
        static void serialize(
            const dnv::vista::sdk::GmodVersioningAssignmentChangeDto& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();
            builder.write( "oldAssignment", obj.oldAssignment );
            builder.write( "currentAssignment", obj.currentAssignment );
            builder.writeEndObject();
        }

        /**
         * @brief Deserialize GmodVersioningAssignmentChangeDto from JSON document
         * @param doc Document to deserialize from
         * @param obj Object to deserialize into
         * @throws std::runtime_error if required fields are missing or invalid
         */
        static void fromDocument(
            const nfx::json::Document& doc, dnv::vista::sdk::GmodVersioningAssignmentChangeDto& obj )
        {
            auto oldAssignmentOpt = doc.get<std::string>( "oldAssignment" );
            if( !oldAssignmentOpt )
            {
                throw std::runtime_error{ "GmodVersioningAssignmentChangeDto: Missing required field 'oldAssignment'" };
            }
            obj.oldAssignment = std::move( *oldAssignmentOpt );

            auto currentAssignmentOpt = doc.get<std::string>( "currentAssignment" );
            if( !currentAssignmentOpt )
            {
                throw std::runtime_error{
                    "GmodVersioningAssignmentChangeDto: Missing required field 'currentAssignment'"
                };
            }
            obj.currentAssignment = std::move( *currentAssignmentOpt );
        }
    };

    /**
     * @brief SerializationTraits for GmodNodeConversionDto
     * @details High-performance streaming serialization with bidirectional support
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::GmodNodeConversionDto>
    {
        /**
         * @brief High-performance streaming serialization
         * @param obj Object to serialize
         * @param builder Builder to write to
         */
        static void serialize( const dnv::vista::sdk::GmodNodeConversionDto& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            builder.writeKey( "operations" );
            builder.writeStartArray();
            for( const auto& op : obj.operations )
            {
                builder.write( op );
            }
            builder.writeEndArray();

            // Optional fields
            if( obj.source )
                builder.write( "source", *obj.source );
            if( obj.target )
                builder.write( "target", *obj.target );
            if( obj.oldAssignment )
                builder.write( "oldAssignment", *obj.oldAssignment );
            if( obj.newAssignment )
                builder.write( "newAssignment", *obj.newAssignment );
            if( obj.deleteAssignment )
                builder.write( "deleteAssignment", *obj.deleteAssignment );

            builder.writeEndObject();
        }

        /**
         * @brief Deserialize GmodNodeConversionDto from JSON document
         * @param doc Document to deserialize from
         * @param obj Object to deserialize into
         * @throws std::runtime_error if required fields are missing or invalid
         */
        static void fromDocument( const nfx::json::Document& doc, dnv::vista::sdk::GmodNodeConversionDto& obj )
        {
            auto operationsArrOpt = doc.get<nfx::json::Array>( "operations" );
            if( !operationsArrOpt )
            {
                throw std::runtime_error{ "GmodNodeConversionDto: Missing required field 'operations'" };
            }
            nfx::json::Document operationsDoc{ operationsArrOpt.value() };
            Serializer<dnv::vista::sdk::GmodNodeConversionDto::Operations> operationsSer;
            operationsSer.deserializeValue( operationsDoc, obj.operations );

            if( auto val = doc.get<std::string>( "source" ) )
                obj.source = std::move( *val );
            if( auto val = doc.get<std::string>( "target" ) )
                obj.target = std::move( *val );
            if( auto val = doc.get<std::string>( "oldAssignment" ) )
                obj.oldAssignment = std::move( *val );
            if( auto val = doc.get<std::string>( "newAssignment" ) )
                obj.newAssignment = std::move( *val );
            if( auto val = doc.get<bool>( "deleteAssignment" ) )
                obj.deleteAssignment = *val;
        }
    };

    /**
     * @brief SerializationTraits for GmodVersioningDto
     * @details High-performance streaming serialization with bidirectional support
     */
    template <>
    struct SerializationTraits<dnv::vista::sdk::GmodVersioningDto>
    {
        /**
         * @brief High-performance streaming serialization
         * @param obj Object to serialize
         * @param builder Builder to write to
         */
        static void serialize( const dnv::vista::sdk::GmodVersioningDto& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();
            builder.write( "visRelease", obj.visVersion );

            builder.writeKey( "items" );
            builder.writeStartObject();
            for( const auto& pair : obj.items )
            {
                builder.writeKey( pair.first );
                SerializationTraits<dnv::vista::sdk::GmodNodeConversionDto>::serialize( pair.second, builder );
            }
            builder.writeEndObject();

            builder.writeEndObject();
        }

        /**
         * @brief Deserialize GmodVersioningDto from JSON document
         * @param doc Document to deserialize from
         * @param obj Object to deserialize into
         * @throws std::runtime_error if required fields are missing or invalid
         * @note JSON field "visRelease" maps to member visVersion
         */
        static void fromDocument( const nfx::json::Document& doc, dnv::vista::sdk::GmodVersioningDto& obj )
        {
            auto visReleaseOpt = doc.get<std::string>( "visRelease" );
            if( !visReleaseOpt )
            {
                throw std::runtime_error{ "GmodVersioningDto: Missing required field 'visRelease'" };
            }
            obj.visVersion = std::move( *visReleaseOpt );

            if( obj.visVersion.empty() )
            {
                throw std::runtime_error{ "GmodVersioningDto: Field 'visRelease' cannot be empty" };
            }

            auto itemsObjOpt = doc.get<nfx::json::Object>( "items" );
            if( !itemsObjOpt )
            {
                throw std::runtime_error{ "GmodVersioningDto: Missing required field 'items'" };
            }
            nfx::json::Document itemsDoc{ itemsObjOpt.value() };
            Serializer<dnv::vista::sdk::GmodVersioningDto::Items> itemsSer;
            itemsSer.deserializeValue( itemsDoc, obj.items );
        }
    };
} // namespace nfx::serialization::json
