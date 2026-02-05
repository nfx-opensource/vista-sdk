/**
 * @file DataChannelListSerializationTraits.h
 * @brief JSON serialization specialization for DataChannelList domain objects
 * @details Provides SerializationTraits specializations for Vista SDK DataChannelList
 */

#pragma once

#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/extensions/ContainersTraits.h>
#include <nfx/serialization/json/extensions/DatatypesTraits.h>
#include <nfx/serialization/json/extensions/DateTimeTraits.h>

#include "dnv/vista/sdk/transport/datachannel/DataChannel.h"
#include "ISO19848Validation.h"

namespace nfx::serialization::json
{
    using namespace dnv::vista::sdk;
    using namespace dnv::vista::sdk::transport;

    /**
     * @brief SerializationTraits specialization for DataChannelListPackage
     * @details Provides unified bidirectional JSON serialization for Vista SDK domain objects.
     *          - serialize(): High-performance streaming serialization using Builder API
     *          - fromDocument(): DOM-based deserialization
     */
    template <>
    struct SerializationTraits<datachannel::DataChannelListPackage>
    {
        /**
         * @brief Serialize DataChannelListPackage to JSON using Builder
         * @param pkg The package to serialize
         * @param builder The JSON Builder to write to
         */
        static void serialize( const datachannel::DataChannelListPackage& pkg, nfx::json::Builder& builder )
        {
            const auto& package = pkg.package();

            builder.writeStartObject(); // Package root
            builder.writeKey( "Package" );
            builder.writeStartObject();

            // Header
            builder.writeKey( "Header" );
            serializeHeader( package.header(), builder );

            // DataChannelList
            builder.writeKey( "DataChannelList" );
            serializeDataChannelList( package.dataChannelList(), builder );

            builder.writeEndObject();
            builder.writeEndObject();
        }

        /**
         * @brief Factory deserialization: Construct DataChannelListPackage directly from JSON document
         * @param doc The JSON document to deserialize from
         * @return Newly constructed DataChannelListPackage
         */
        static datachannel::DataChannelListPackage fromDocument( const nfx::json::Document& doc )
        {
            auto& packageObj = doc["Package"];
            auto& headerObj = packageObj["Header"];

            // Header
            auto shipIdStr = headerObj["ShipID"].get<std::string>( "" ).value();
            auto shipId = ShipId::fromString( shipIdStr ).value();

            // DataChannelListID
            auto& configRefObj = headerObj["DataChannelListID"];
            auto configId = configRefObj["ID"].get<std::string>( "" ).value();
            auto configTimeStampStr = configRefObj["TimeStamp"].get<std::string>( "" ).value();
            if( !isValidIso19848DateTimeOffset( configTimeStampStr ) )
            {
                throw std::runtime_error{ "Invalid ISO 19848 DateTimeOffset format in DataChannelListID/TimeStamp: " +
                                          configTimeStampStr };
            }
            DateTimeOffset configTimeStamp;
            if( !DateTimeOffset::fromString( configTimeStampStr, configTimeStamp ) )
            {
                throw std::runtime_error{ "Failed to parse DateTimeOffset in DataChannelListID/TimeStamp: " +
                                          configTimeStampStr };
            }

            std::optional<std::string> configVersion;
            if( auto versionOpt = configRefObj["Version"].get<std::string>( "" ) )
            {
                configVersion = versionOpt.value();
            }

            datachannel::ConfigurationReference dataChannelListId{ configId, configTimeStamp, configVersion };

            // VersionInformation (optional)
            std::optional<datachannel::VersionInformation> versionInfo;
            if( headerObj.contains( "VersionInformation" ) )
            {
                auto& versionInfoObj = headerObj["VersionInformation"];
                if( auto namingRuleOpt = versionInfoObj["NamingRule"].get<std::string>( "" ) )
                {
                    auto namingRule = namingRuleOpt.value();
                    auto namingSchemeVersion = versionInfoObj["NamingSchemeVersion"].get<std::string>( "" ).value();
                    std::optional<std::string> referenceUrl;
                    if( auto refUrlOpt = versionInfoObj["ReferenceURL"].get<std::string>( "" ) )
                    {
                        referenceUrl = refUrlOpt.value();
                    }
                    versionInfo = datachannel::VersionInformation{ namingRule, namingSchemeVersion, referenceUrl };
                }
            }

            // Author
            std::optional<std::string> author;
            if( auto authorOpt = headerObj["Author"].get<std::string>( "" ) )
            {
                author = authorOpt.value();
            }

            // DateCreated
            std::optional<DateTimeOffset> dateCreated;
            if( auto dateCreatedStrOpt = headerObj["DateCreated"].get<std::string>( "" ) )
            {
                if( !isValidIso19848DateTimeOffset( dateCreatedStrOpt.value() ) )
                {
                    throw std::runtime_error{ "Invalid ISO 19848 DateTimeOffset format in DateCreated: " +
                                              dateCreatedStrOpt.value() };
                }
                DateTimeOffset dt;
                if( DateTimeOffset::fromString( dateCreatedStrOpt.value(), dt ) )
                {
                    dateCreated = dt;
                }
            }
            std::optional<datachannel::CustomHeaders> customHeaders;
            if( auto headerObjOpt = headerObj.getRef<Object>( "" ) )
            {
                static const std::unordered_set<std::string> knownKeys = {
                    "ShipID", "Author", "DateCreated", "DataChannelListID", "VersionInformation"
                };

                nfx::json::Document customHdrsDoc = nfx::json::Document::object();
                bool hasCustomHeaders = false;
                for( const auto& [key, valueDoc] : headerObjOpt->get() )
                {
                    // Skip known properties
                    if( knownKeys.find( key ) != knownKeys.end() )
                    {
                        continue;
                    }

                    customHdrsDoc[key] = valueDoc;
                    hasCustomHeaders = true;
                }
                if( hasCustomHeaders )
                {
                    datachannel::CustomHeaders customDict;
                    customDict.document() = std::move( customHdrsDoc );
                    customHeaders = std::move( customDict );
                }
            }

            // Create Header
            datachannel::Header header{ shipId, dataChannelListId, versionInfo, author, dateCreated, customHeaders };

            // Deserialize DataChannelList
            std::vector<datachannel::DataChannel> channels;

            // Iterate through DataChannel array
            if( packageObj.contains( "DataChannelList" ) )
            {
                auto& dataChannelListObj = packageObj["DataChannelList"];
                if( auto dataChannelArrayOpt = dataChannelListObj["DataChannel"].get<Array>( "" ) )
                {
                    const auto& dataChannelArray = *dataChannelArrayOpt;
                    for( size_t channelIndex = 0; channelIndex < dataChannelArray.size(); ++channelIndex )
                    {
                        auto& channelDoc = dataChannelArray[channelIndex];

                        // DataChannelID
                        auto& dataChannelIdObj = channelDoc["DataChannelID"];
                        auto localIdStr = dataChannelIdObj["LocalID"].get<std::string>( "" ).value();
                        auto localId = LocalId::fromString( localIdStr ).value();

                        std::optional<std::string> shortId;
                        if( auto shortIdOpt = dataChannelIdObj["ShortID"].get<std::string>( "" ) )
                        {
                            shortId = shortIdOpt.value();
                        }

                        // NameObject (optional)
                        std::optional<datachannel::NameObject> nameObject;
                        if( auto namingRuleOpt = dataChannelIdObj["NameObject"]["NamingRule"].get<std::string>( "" ) )
                        {
                            auto& nameObjectDoc = dataChannelIdObj["NameObject"];
                            auto namingRule = namingRuleOpt.value();

                            // CustomNameObjects (optional)
                            std::optional<datachannel::CustomNameObjects> customNameObjects;
                            if( auto nameObjOpt = nameObjectDoc.getRef<Object>( "" ) )
                            {
                                nfx::json::Document customNameObjsDoc = nfx::json::Document::object();
                                bool hasCustomNameObjects = false;
                                for( const auto& [key, valueDoc] : nameObjOpt->get() )
                                {
                                    if( key == "NamingRule" )
                                    {
                                        continue;
                                    }
                                    customNameObjsDoc[key] = valueDoc;
                                    hasCustomNameObjects = true;
                                }
                                if( hasCustomNameObjects )
                                {
                                    datachannel::CustomNameObjects customDict;
                                    customDict.document() = std::move( customNameObjsDoc );
                                    customNameObjects = std::move( customDict );
                                }
                            }

                            datachannel::NameObject no{ namingRule, customNameObjects };
                            nameObject = std::move( no );
                        }

                        datachannel::DataChannelId dataChannelId{ localId, shortId, nameObject };

                        // Property
                        auto& propertyObj = channelDoc["Property"];
                        auto& dataChannelTypeObj = propertyObj["DataChannelType"];
                        auto dataChannelTypeStr = dataChannelTypeObj["Type"].get<std::string>( "" ).value();
                        std::optional<double> updateCycle;
                        if( auto intOpt = dataChannelTypeObj["UpdateCycle"].get<int64_t>( "" ) )
                        {
                            updateCycle = static_cast<double>( intOpt.value() );
                        }
                        else if( auto dblOpt = dataChannelTypeObj["UpdateCycle"].get<double>( "" ) )
                        {
                            updateCycle = dblOpt.value();
                        }
                        std::optional<double> calculationPeriod;
                        if( auto intOpt = dataChannelTypeObj["CalculationPeriod"].get<int64_t>( "" ) )
                        {
                            calculationPeriod = static_cast<double>( intOpt.value() );
                        }
                        else if( auto dblOpt = dataChannelTypeObj["CalculationPeriod"].get<double>( "" ) )
                        {
                            calculationPeriod = dblOpt.value();
                        }
                        datachannel::DataChannelType dataChannelType{ dataChannelTypeStr,
                                                                      updateCycle,
                                                                      calculationPeriod };

                        // Format
                        auto& formatObj = propertyObj["Format"];
                        auto formatTypeStr = formatObj["Type"].get<std::string>( "" ).value();

                        // Restriction (optional)
                        std::optional<datachannel::Restriction> restriction;
                        if( formatObj.contains( "Restriction" ) )
                        {
                            datachannel::Restriction r{};
                            auto& restrictionDoc = formatObj["Restriction"];
                            // Numeric bounds
                            if( auto maxIncOpt = restrictionDoc["MaxInclusive"].get<double>( "" ) )
                            {
                                r.setMaxInclusive( maxIncOpt.value() );
                            }
                            else if( auto maxIncOpt = restrictionDoc["MaxInclusive"].get<int64_t>( "" ) )
                            {
                                r.setMaxInclusive( static_cast<double>( maxIncOpt.value() ) );
                            }
                            if( auto minIncOpt = restrictionDoc["MinInclusive"].get<double>( "" ) )
                            {
                                r.setMinInclusive( minIncOpt.value() );
                            }
                            else if( auto minIncOpt = restrictionDoc["MinInclusive"].get<int64_t>( "" ) )
                            {
                                r.setMinInclusive( static_cast<double>( minIncOpt.value() ) );
                            }
                            if( auto maxExcOpt = restrictionDoc["MaxExclusive"].get<double>( "" ) )
                            {
                                r.setMaxExclusive( maxExcOpt.value() );
                            }
                            else if( auto maxExcOpt = restrictionDoc["MaxExclusive"].get<int64_t>( "" ) )
                            {
                                r.setMaxExclusive( static_cast<double>( maxExcOpt.value() ) );
                            }
                            if( auto minExcOpt = restrictionDoc["MinExclusive"].get<double>( "" ) )
                            {
                                r.setMinExclusive( minExcOpt.value() );
                            }
                            else if( auto minExcOpt = restrictionDoc["MinExclusive"].get<int64_t>( "" ) )
                            {
                                r.setMinExclusive( static_cast<double>( minExcOpt.value() ) );
                            }
                            // Length constraints
                            if( auto fracDigitsOpt = restrictionDoc["FractionDigits"].get<int64_t>( "" ) )
                            {
                                r.setFractionDigits( static_cast<uint32_t>( fracDigitsOpt.value() ) );
                            }
                            else if( auto fracDigitsOpt = restrictionDoc["FractionDigits"].get<uint64_t>( "" ) )
                            {
                                r.setFractionDigits( static_cast<uint32_t>( fracDigitsOpt.value() ) );
                            }
                            if( auto maxLengthOpt = restrictionDoc["MaxLength"].get<int64_t>( "" ) )
                            {
                                r.setMaxLength( static_cast<uint32_t>( maxLengthOpt.value() ) );
                            }
                            else if( auto maxLengthOpt = restrictionDoc["MaxLength"].get<uint64_t>( "" ) )
                            {
                                r.setMaxLength( static_cast<uint32_t>( maxLengthOpt.value() ) );
                            }
                            if( auto minLengthOpt = restrictionDoc["MinLength"].get<int64_t>( "" ) )
                            {
                                r.setMinLength( static_cast<uint32_t>( minLengthOpt.value() ) );
                            }
                            else if( auto minLengthOpt = restrictionDoc["MinLength"].get<uint64_t>( "" ) )
                            {
                                r.setMinLength( static_cast<uint32_t>( minLengthOpt.value() ) );
                            }
                            if( auto lengthOpt = restrictionDoc["Length"].get<int64_t>( "" ) )
                            {
                                r.setLength( static_cast<uint32_t>( lengthOpt.value() ) );
                            }
                            else if( auto lengthOpt = restrictionDoc["Length"].get<uint64_t>( "" ) )
                            {
                                r.setLength( static_cast<uint32_t>( lengthOpt.value() ) );
                            }
                            if( auto totalDigitsOpt = restrictionDoc["TotalDigits"].get<int64_t>( "" ) )
                            {
                                r.setTotalDigits( static_cast<uint32_t>( totalDigitsOpt.value() ) );
                            }
                            else if( auto totalDigitsOpt = restrictionDoc["TotalDigits"].get<uint64_t>( "" ) )
                            {
                                r.setTotalDigits( static_cast<uint32_t>( totalDigitsOpt.value() ) );
                            }
                            // Pattern
                            if( auto patternOpt = restrictionDoc["Pattern"].get<std::string>( "" ) )
                            {
                                r.setPattern( patternOpt.value() );
                            }
                            // Enumeration
                            if( restrictionDoc.contains( "Enumeration" ) )
                            {
                                std::vector<std::string> enumVec;
                                size_t enumIdx = 0;
                                auto& enumArray = restrictionDoc["Enumeration"];
                                while( true )
                                {
                                    if( auto enumValOpt = enumArray[std::to_string( enumIdx )].get<std::string>( "" ) )
                                    {
                                        enumVec.push_back( enumValOpt.value() );
                                        ++enumIdx;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                if( !enumVec.empty() )
                                {
                                    r.setEnumeration( std::move( enumVec ) );
                                }
                            }
                            // WhiteSpace
                            if( auto whiteSpaceOpt = restrictionDoc["WhiteSpace"].get<std::string>( "" ) )
                            {
                                auto wsStr = whiteSpaceOpt.value();
                                if( wsStr == "Preserve" )
                                {
                                    r.setWhiteSpace( datachannel::WhiteSpace::Preserve );
                                }
                                else if( wsStr == "Replace" )
                                {
                                    r.setWhiteSpace( datachannel::WhiteSpace::Replace );
                                }
                                else if( wsStr == "Collapse" )
                                {
                                    r.setWhiteSpace( datachannel::WhiteSpace::Collapse );
                                }
                            }
                            restriction = std::move( r );
                        }

                        datachannel::Format format{ formatTypeStr, restriction };

                        // Optional Property fields
                        std::optional<datachannel::Range> range;
                        if( propertyObj.contains( "Range" ) )
                        {
                            auto& rangeObj = propertyObj["Range"];
                            double high = 0.0;
                            if( auto highIntOpt = rangeObj["High"].get<int64_t>( "" ) )
                            {
                                high = static_cast<double>( highIntOpt.value() );
                            }
                            else if( auto highDblOpt = rangeObj["High"].get<double>( "" ) )
                            {
                                high = highDblOpt.value();
                            }
                            double low = 0.0;
                            if( auto lowIntOpt = rangeObj["Low"].get<int64_t>( "" ) )
                            {
                                low = static_cast<double>( lowIntOpt.value() );
                            }
                            else if( auto lowDblOpt = rangeObj["Low"].get<double>( "" ) )
                            {
                                low = lowDblOpt.value();
                            }
                            range = datachannel::Range{ low, high };
                        }

                        std::optional<datachannel::Unit> unit;
                        if( propertyObj.contains( "Unit" ) )
                        {
                            auto& unitObj = propertyObj["Unit"];
                            if( auto unitSymbolOpt = unitObj["UnitSymbol"].get<std::string>( "" ) )
                            {
                                auto unitSymbol = unitSymbolOpt.value();
                                std::optional<std::string> quantityName;
                                if( auto qnOpt = unitObj["QuantityName"].get<std::string>( "" ) )
                                {
                                    quantityName = qnOpt.value();
                                }

                                // CustomElements (optional)
                                std::optional<datachannel::CustomElements> customElements;
                                if( auto unitObjOpt = unitObj.getRef<Object>( "" ) )
                                {
                                    nfx::json::Document customElemsDoc = nfx::json::Document::object();
                                    bool hasCustomElements = false;
                                    for( const auto& [key, valueDoc] : unitObjOpt->get() )
                                    {
                                        if( key == "UnitSymbol" || key == "QuantityName" )
                                        {
                                            continue;
                                        }
                                        customElemsDoc[key] = valueDoc;
                                        hasCustomElements = true;
                                    }
                                    if( hasCustomElements )
                                    {
                                        datachannel::CustomElements customDict;
                                        customDict.document() = std::move( customElemsDoc );
                                        customElements = std::move( customDict );
                                    }
                                }

                                unit = datachannel::Unit{ unitSymbol, quantityName, customElements };
                            }
                        }

                        std::optional<std::string> name;
                        if( auto nameOpt = propertyObj["Name"].get<std::string>( "" ) )
                        {
                            name = nameOpt.value();
                        }

                        std::optional<std::string> qualityCoding;
                        if( auto qcOpt = propertyObj["QualityCoding"].get<std::string>( "" ) )
                        {
                            qualityCoding = qcOpt.value();
                        }

                        std::optional<std::string> alertPriority;
                        if( auto apOpt = propertyObj["AlertPriority"].get<std::string>( "" ) )
                        {
                            alertPriority = apOpt.value();
                        }

                        std::optional<std::string> remarks;
                        if( auto remarksOpt = propertyObj["Remarks"].get<std::string>( "" ) )
                        {
                            remarks = remarksOpt.value();
                        }

                        // CustomProperties (optional)
                        std::optional<datachannel::CustomProperties> customProperties;
                        if( auto propObjOpt = propertyObj.getRef<Object>( "" ) )
                        {
                            static const std::unordered_set<std::string> knownKeys = {
                                "DataChannelType", "Format",        "Range", "Unit",
                                "QualityCoding",   "AlertPriority", "Name",  "Remarks"
                            };
                            nfx::json::Document customPropsDoc = nfx::json::Document::object();
                            bool hasCustomProperties = false;
                            for( const auto& [key, valueDoc] : propObjOpt->get() )
                            {
                                if( knownKeys.find( key ) != knownKeys.end() )
                                {
                                    continue;
                                }
                                customPropsDoc[key] = valueDoc;
                                hasCustomProperties = true;
                            }
                            if( hasCustomProperties )
                            {
                                datachannel::CustomProperties customDict;
                                customDict.document() = std::move( customPropsDoc );
                                customProperties = std::move( customDict );
                            }
                        }

                        datachannel::Property property{ dataChannelType, format,        range, unit,
                                                        qualityCoding,   alertPriority, name,  remarks,
                                                        customProperties };

                        channels.emplace_back(
                            datachannel::DataChannel{ std::move( dataChannelId ), std::move( property ) } );
                    }
                }
            }

            datachannel::DataChannelList dataChannelList{ std::move( channels ) };

            // Create Package
            datachannel::Package package{ header, std::move( dataChannelList ) };

            return datachannel::DataChannelListPackage{ std::move( package ) };
        }

    private:
        // Helper methods for streaming serialization
        static void serializeHeader( const datachannel::Header& header, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            // ShipID
            builder.writeKey( "ShipID" );
            builder.write( header.shipId().toString() );

            // DataChannelListID
            builder.writeKey( "DataChannelListID" );
            serializeConfigRef( header.dataChannelListId(), builder );

            // VersionInformation (optional)
            if( auto versionInfo = header.versionInformation() )
            {
                builder.writeKey( "VersionInformation" );
                serializeVersionInfo( *versionInfo, builder );
            }

            // Author (optional)
            if( auto author = header.author() )
            {
                builder.writeKey( "Author" );
                builder.write( *author );
            }

            // DateCreated (optional)
            if( auto dateCreated = header.dateCreated() )
            {
                builder.writeKey( "DateCreated" );
                builder.write( dateCreated->toString( DateTime::Format::Iso8601Precise ) );
            }

            // CustomHeaders
            if( auto customHeaders = header.customHeaders() )
            {
                if( auto objRef = customHeaders->document().rootRef<Object>() )
                {
                    for( const auto& [key, valueDoc] : objRef->get() )
                    {
                        builder.write( key, valueDoc );
                    }
                }
            }

            builder.writeEndObject();
        }

        static void serializeConfigRef( const datachannel::ConfigurationReference& ref, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            builder.writeKey( "ID" );
            builder.write( ref.id() );

            if( auto version = ref.version() )
            {
                builder.writeKey( "Version" );
                builder.write( version.value() );
            }

            builder.writeKey( "TimeStamp" );
            builder.write( ref.timeStamp().toString( DateTime::Format::Iso8601Precise ) );

            builder.writeEndObject();
        }

        static void serializeVersionInfo( const datachannel::VersionInformation& info, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            builder.writeKey( "NamingRule" );
            builder.write( info.namingRule() );

            builder.writeKey( "NamingSchemeVersion" );
            builder.write( info.namingSchemeVersion() );

            if( auto refUrl = info.referenceUrl() )
            {
                builder.writeKey( "ReferenceURL" );
                builder.write( *refUrl );
            }

            builder.writeEndObject();
        }

        static void serializeDataChannelList( const datachannel::DataChannelList& list, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            builder.writeKey( "DataChannel" );
            builder.writeStartArray();
            for( const auto& channel : list )
            {
                serializeDataChannel( channel, builder );
            }
            builder.writeEndArray();

            builder.writeEndObject();
        }

        static void serializeDataChannel( const datachannel::DataChannel& channel, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            // DataChannelID
            builder.writeKey( "DataChannelID" );
            serializeDataChannelId( channel.dataChannelId(), builder );

            // Property
            builder.writeKey( "Property" );
            serializeProperty( channel.property(), builder );

            builder.writeEndObject();
        }

        static void serializeDataChannelId( const datachannel::DataChannelId& id, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            builder.writeKey( "LocalID" );
            builder.write( id.localId().toString() );

            if( auto shortId = id.shortId() )
            {
                builder.writeKey( "ShortID" );
                builder.write( *shortId );
            }

            if( auto nameObj = id.nameObject() )
            {
                builder.writeKey( "NameObject" );
                serializeNameObject( *nameObj, builder );
            }

            builder.writeEndObject();
        }

        static void serializeNameObject( const datachannel::NameObject& nameObj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            builder.writeKey( "NamingRule" );
            builder.write( nameObj.namingRule() );

            // CustomNameObjects
            if( auto customObjs = nameObj.customNameObjects() )
            {
                if( auto objRef = customObjs->document().rootRef<Object>() )
                {
                    for( const auto& [key, valueDoc] : objRef->get() )
                    {
                        builder.write( key, valueDoc );
                    }
                }
            }

            builder.writeEndObject();
        }

        static void serializeProperty( const datachannel::Property& prop, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            // DataChannelType (required)
            builder.writeKey( "DataChannelType" );
            serializeDataChannelType( prop.dataChannelType(), builder );

            // Format (required)
            builder.writeKey( "Format" );
            serializeFormat( prop.format(), builder );

            // Range (optional)
            if( auto range = prop.range() )
            {
                builder.writeKey( "Range" );
                serializeRange( *range, builder );
            }

            // Unit (optional)
            if( auto unit = prop.unit() )
            {
                builder.writeKey( "Unit" );
                serializeUnit( *unit, builder );
            }

            // QualityCoding (optional)
            if( auto qualityCoding = prop.qualityCoding() )
            {
                builder.writeKey( "QualityCoding" );
                builder.write( *qualityCoding );
            }

            // AlertPriority (optional)
            if( auto alertPriority = prop.alertPriority() )
            {
                builder.writeKey( "AlertPriority" );
                builder.write( *alertPriority );
            }

            // Name (optional)
            if( auto name = prop.name() )
            {
                builder.writeKey( "Name" );
                builder.write( *name );
            }

            // Remarks (optional)
            if( auto remarks = prop.remarks() )
            {
                builder.writeKey( "Remarks" );
                builder.write( *remarks );
            }

            // CustomProperties (last)
            if( auto customProps = prop.customProperties() )
            {
                if( auto objRef = customProps->document().rootRef<Object>() )
                {
                    for( const auto& [key, valueDoc] : objRef->get() )
                    {
                        builder.write( key, valueDoc );
                    }
                }
            }

            builder.writeEndObject();
        }

        static void serializeDataChannelType( const datachannel::DataChannelType& type, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            // Type (required)
            builder.writeKey( "Type" );
            builder.write( type.type() );

            // UpdateCycle (optional)
            if( auto uc = type.updateCycle() )
            {
                builder.writeKey( "UpdateCycle" );
                builder.write( *uc );
            }

            // CalculationPeriod (optional)
            if( auto calcPeriod = type.calculationPeriod() )
            {
                builder.writeKey( "CalculationPeriod" );
                builder.write( static_cast<uint64_t>( *calcPeriod ) );
            }

            builder.writeEndObject();
        }

        static void serializeFormat( const datachannel::Format& format, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            // Type (required)
            builder.writeKey( "Type" );
            builder.write( format.type() );

            // Restriction (optional)
            if( auto restriction = format.restriction() )
            {
                builder.writeKey( "Restriction" );
                serializeRestriction( *restriction, builder );
            }

            builder.writeEndObject();
        }

        static void serializeRestriction( const datachannel::Restriction& restriction, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            if( auto enumeration = restriction.enumeration() )
            {
                builder.writeKey( "Enumeration" );
                builder.writeStartArray();
                for( const auto& val : *enumeration )
                {
                    builder.write( val );
                }
                builder.writeEndArray();
            }

            if( auto fractionDigits = restriction.fractionDigits() )
            {
                builder.writeKey( "FractionDigits" );
                builder.write( static_cast<uint64_t>( *fractionDigits ) );
            }

            if( auto length = restriction.length() )
            {
                builder.writeKey( "Length" );
                builder.write( static_cast<uint64_t>( *length ) );
            }

            if( auto maxExclusive = restriction.maxExclusive() )
            {
                builder.writeKey( "MaxExclusive" );
                builder.write( *maxExclusive );
            }

            if( auto maxInclusive = restriction.maxInclusive() )
            {
                builder.writeKey( "MaxInclusive" );
                builder.write( *maxInclusive );
            }

            if( auto maxLength = restriction.maxLength() )
            {
                builder.writeKey( "MaxLength" );
                builder.write( static_cast<uint64_t>( *maxLength ) );
            }

            if( auto minExclusive = restriction.minExclusive() )
            {
                builder.writeKey( "MinExclusive" );
                builder.write( *minExclusive );
            }

            if( auto minInclusive = restriction.minInclusive() )
            {
                builder.writeKey( "MinInclusive" );
                builder.write( *minInclusive );
            }

            if( auto minLength = restriction.minLength() )
            {
                builder.writeKey( "MinLength" );
                builder.write( static_cast<uint64_t>( *minLength ) );
            }

            if( auto pattern = restriction.pattern() )
            {
                builder.writeKey( "Pattern" );
                builder.write( *pattern );
            }

            if( auto totalDigits = restriction.totalDigits() )
            {
                builder.writeKey( "TotalDigits" );
                builder.write( static_cast<uint64_t>( *totalDigits ) );
            }

            if( auto whiteSpace = restriction.whiteSpace() )
            {
                builder.writeKey( "WhiteSpace" );
                using WS = datachannel::WhiteSpace;
                const char* wsStr = ( *whiteSpace == WS::Preserve )   ? "Preserve"
                                    : ( *whiteSpace == WS::Replace )  ? "Replace"
                                    : ( *whiteSpace == WS::Collapse ) ? "Collapse"
                                                                      : "";
                builder.write( wsStr );
            }

            builder.writeEndObject();
        }

        static void serializeRange( const datachannel::Range& range, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            builder.writeKey( "High" );
            builder.write( range.high() );

            builder.writeKey( "Low" );
            builder.write( range.low() );

            builder.writeEndObject();
        }

        static void serializeUnit( const datachannel::Unit& unit, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            builder.writeKey( "UnitSymbol" );
            builder.write( unit.unitSymbol() );

            if( auto quantityName = unit.quantityName() )
            {
                builder.writeKey( "QuantityName" );
                builder.write( *quantityName );
            }

            // CustomElements
            if( auto customElems = unit.customElements() )
            {
                if( auto objRef = customElems->document().rootRef<Object>() )
                {
                    for( const auto& [key, valueDoc] : objRef->get() )
                    {
                        builder.write( key, valueDoc );
                    }
                }
            }

            builder.writeEndObject();
        }
    };
} // namespace nfx::serialization::json
