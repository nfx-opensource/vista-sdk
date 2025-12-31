/**
 * @file DataChannelListSerializationTraits.h
 * @brief JSON serialization specialization for DataChannelList domain objects
 * @details Provides SerializationTraits specializations for Vista SDK DataChannelList
 */

#pragma once

#include <nfx/serialization/json/SerializationTraits.h>
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
     * @details Converts Vista SDK domain objects directly to/from JSON
     */
    template <>
    struct SerializationTraits<datachannel::DataChannelListPackage>
    {
    public:
        /**
         * @brief Serialize DataChannelListPackage to JSON document
         * @param obj The DataChannelListPackage to serialize
         * @param doc The JSON document to serialize into
         */
        static void serialize( const datachannel::DataChannelListPackage& obj, SerializableDocument& doc )
        {
            auto& jsonDoc = doc.document();
            const auto& package = obj.package();
            const auto& header = package.header();

            const auto& configRef = header.dataChannelListId();
            nfx::json::Document configRefDoc = nfx::json::Document::object();
            if( auto configRefRef = configRefDoc.rootRef<Object>() )
            {
                auto& configObj = configRefRef->get();
                configObj.reserve( 3 ); // ID, TimeStamp, Version (optional)

                configObj.emplace_back( "ID", nfx::json::Document{ configRef.id() } );
                configObj.emplace_back(
                    "TimeStamp",
                    nfx::json::Document{ configRef.timeStamp().toString( DateTime::Format::Iso8601PreciseTrimmed ) } );

                if( auto version = configRef.version() )
                {
                    configObj.emplace_back( "Version", nfx::json::Document{ version.value() } );
                }
            }

            // Build VersionInformation (optional)
            nfx::json::Document versionInfoDoc;
            bool hasVersionInfo = false;
            if( auto versionInfoOpt = header.versionInformation() )
            {
                hasVersionInfo = true;
                const auto& version = *versionInfoOpt;
                versionInfoDoc = nfx::json::Document::object();
                if( auto versionInfoRef = versionInfoDoc.rootRef<Object>() )
                {
                    auto& versionObj = versionInfoRef->get();
                    versionObj.reserve( 3 ); // NamingRule, NamingSchemeVersion, ReferenceURL (optional)

                    versionObj.emplace_back( "NamingRule", nfx::json::Document{ version.namingRule() } );
                    versionObj.emplace_back(
                        "NamingSchemeVersion", nfx::json::Document{ version.namingSchemeVersion() } );

                    if( auto refUrl = version.referenceUrl() )
                    {
                        versionObj.emplace_back( "ReferenceURL", nfx::json::Document{ *refUrl } );
                    }
                }
            }

            // Header
            nfx::json::Document headerDoc = nfx::json::Document::object();
            if( auto headerRef = headerDoc.rootRef<Object>() )
            {
                auto& headerObj = headerRef->get();

                // ShipID, DataChannelListID, VersionInformation, Author, DateCreated, CustomHeaders
                headerObj.reserve( 7 );
                headerObj.emplace_back( "ShipID", nfx::json::Document{ header.shipId().toString() } );
                headerObj.emplace_back( "DataChannelListID", std::move( configRefDoc ) );

                if( hasVersionInfo )
                {
                    headerObj.emplace_back( "VersionInformation", std::move( versionInfoDoc ) );
                }

                if( auto author = header.author() )
                {
                    headerObj.emplace_back( "Author", nfx::json::Document{ author.value() } );
                }

                if( auto dateCreated = header.dateCreated() )
                {
                    headerObj.emplace_back(
                        "DateCreated",
                        nfx::json::Document{ dateCreated->toString( DateTime::Format::Iso8601PreciseTrimmed ) } );
                }

                // CustomHeaders
                if( auto customHeadersOpt = header.customHeaders() )
                {
                    const auto& customHdrs = *customHeadersOpt;
                    if( auto customHdrsObj = customHdrs.get<Object>( "" ) )
                    {
                        for( auto&& [key, valueDoc] : customHdrsObj.value() )
                        {
                            headerObj.emplace_back( std::move( key ), std::move( valueDoc ) );
                        }
                    }
                }
            }

            // DataChannelList
            const auto& channels = package.dataChannelList();
            Array dataChannelArray;
            dataChannelArray.reserve( channels.size() );

            for( size_t i = 0; i < channels.size(); ++i )
            {
                const auto& channel = channels[i];
                const auto& channelId = channel.dataChannelId();
                const auto& property = channel.property();

                // DataChannelID
                nfx::json::Document dataChannelIdDoc = nfx::json::Document::object();
                if( auto dataChannelIdRef = dataChannelIdDoc.rootRef<Object>() )
                {
                    auto& idObj = dataChannelIdRef->get();
                    idObj.reserve( 3 ); // LocalID, ShortID (optional), NameObject (optional)

                    idObj.emplace_back( "LocalID", nfx::json::Document{ channelId.localId().toString() } );

                    if( auto shortId = channelId.shortId() )
                    {
                        idObj.emplace_back( "ShortID", nfx::json::Document{ shortId.value() } );
                    }

                    // NameObject (optional)
                    if( auto nameObjectOpt = channelId.nameObject() )
                    {
                        const auto& nameObject = *nameObjectOpt;
                        nfx::json::Document nameObjectDoc = nfx::json::Document::object();
                        if( auto nameObjectRef = nameObjectDoc.rootRef<Object>() )
                        {
                            auto& nameObj = nameObjectRef->get();
                            nameObj.reserve( 2 ); // NamingRule, CustomNameObjects

                            nameObj.emplace_back( "NamingRule", nfx::json::Document{ nameObject.namingRule() } );

                            // CustomNameObjects (optional)
                            if( auto customNameObjsOpt = nameObject.customNameObjects() )
                            {
                                const auto& customNameObjs = *customNameObjsOpt;
                                if( auto customNameObjsObj = customNameObjs.get<Object>( "" ) )
                                {
                                    for( auto&& [key, valueDoc] : customNameObjsObj.value() )
                                    {
                                        nameObj.emplace_back( std::move( key ), std::move( valueDoc ) );
                                    }
                                }
                            }
                        }
                        idObj.emplace_back( "NameObject", std::move( nameObjectDoc ) );
                    }
                }

                // DataChannelType
                const auto& channelType = property.dataChannelType();
                nfx::json::Document dataChannelTypeDoc = nfx::json::Document::object();
                if( auto dataChannelTypeRef = dataChannelTypeDoc.rootRef<Object>() )
                {
                    auto& obj = dataChannelTypeRef->get();
                    obj.reserve( 3 ); // Type, UpdateCycle, CalculationPeriod

                    obj.emplace_back( "Type", nfx::json::Document{ channelType.type() } );

                    if( auto updateCycle = channelType.updateCycle() )
                    {
                        double val = updateCycle.value();
                        if( val == std::trunc( val ) && std::isfinite( val ) )
                        {
                            obj.emplace_back( "UpdateCycle", nfx::json::Document{ static_cast<int64_t>( val ) } );
                        }
                        else
                        {
                            obj.emplace_back( "UpdateCycle", nfx::json::Document{ val } );
                        }
                    }
                    if( auto calcPeriod = channelType.calculationPeriod() )
                    {
                        double val = calcPeriod.value();
                        if( val == std::trunc( val ) && std::isfinite( val ) )
                        {
                            obj.emplace_back( "CalculationPeriod", nfx::json::Document{ static_cast<int64_t>( val ) } );
                        }
                        else
                        {
                            obj.emplace_back( "CalculationPeriod", nfx::json::Document{ val } );
                        }
                    }
                }

                // Format
                const auto& format = property.format();
                nfx::json::Document formatDoc = nfx::json::Document::object();
                if( auto formatRef = formatDoc.rootRef<Object>() )
                {
                    auto& formatObj = formatRef->get();
                    formatObj.reserve( 2 ); // Type, Restriction (optional)

                    formatObj.emplace_back( "Type", nfx::json::Document{ format.type() } );

                    // Restriction
                    if( auto restrictionOpt = format.restriction() )
                    {
                        const auto& restriction = *restrictionOpt;
                        nfx::json::Document restrictionDoc = nfx::json::Document::object();
                        if( auto restrictionRef = restrictionDoc.rootRef<Object>() )
                        {
                            auto& resObj = restrictionRef->get();
                            resObj.reserve( 12 ); // Max possible restriction fields

                            // Restrictions
                            if( auto enumOpt = restriction.enumeration() )
                            {
                                const auto& enumValues = *enumOpt;
                                Array enumArray;
                                enumArray.reserve( enumValues.size() );
                                for( const auto& enumVal : enumValues )
                                {
                                    enumArray.push_back( nfx::json::Document{ enumVal } );
                                }
                                resObj.emplace_back( "Enumeration", nfx::json::Document{ std::move( enumArray ) } );
                            }

                            if( auto fracDigits = restriction.fractionDigits() )
                            {
                                resObj.emplace_back(
                                    "FractionDigits",
                                    nfx::json::Document{ static_cast<uint64_t>( fracDigits.value() ) } );
                            }

                            if( auto length = restriction.length() )
                            {
                                resObj.emplace_back(
                                    "Length", nfx::json::Document{ static_cast<uint64_t>( length.value() ) } );
                            }

                            if( auto maxExc = restriction.maxExclusive() )
                            {
                                resObj.emplace_back( "MaxExclusive", nfx::json::Document{ maxExc.value() } );
                            }

                            if( auto maxInc = restriction.maxInclusive() )
                            {
                                resObj.emplace_back( "MaxInclusive", nfx::json::Document{ maxInc.value() } );
                            }

                            if( auto maxLen = restriction.maxLength() )
                            {
                                resObj.emplace_back(
                                    "MaxLength", nfx::json::Document{ static_cast<uint64_t>( maxLen.value() ) } );
                            }

                            if( auto minExc = restriction.minExclusive() )
                            {
                                resObj.emplace_back( "MinExclusive", nfx::json::Document{ minExc.value() } );
                            }

                            if( auto minInc = restriction.minInclusive() )
                            {
                                resObj.emplace_back( "MinInclusive", nfx::json::Document{ minInc.value() } );
                            }

                            if( auto minLen = restriction.minLength() )
                            {
                                resObj.emplace_back(
                                    "MinLength", nfx::json::Document{ static_cast<uint64_t>( minLen.value() ) } );
                            }

                            if( auto pattern = restriction.pattern() )
                            {
                                resObj.emplace_back( "Pattern", nfx::json::Document{ pattern.value() } );
                            }

                            if( auto totalDigits = restriction.totalDigits() )
                            {
                                resObj.emplace_back(
                                    "TotalDigits",
                                    nfx::json::Document{ static_cast<uint64_t>( totalDigits.value() ) } );
                            }

                            if( auto whiteSpace = restriction.whiteSpace() )
                            {
                                std::string whiteSpaceValue;
                                switch( *whiteSpace )
                                {
                                    case datachannel::WhiteSpace::Preserve:
                                        whiteSpaceValue = "Preserve";
                                        break;
                                    case datachannel::WhiteSpace::Replace:
                                        whiteSpaceValue = "Replace";
                                        break;
                                    case datachannel::WhiteSpace::Collapse:
                                        whiteSpaceValue = "Collapse";
                                        break;
                                }
                                resObj.emplace_back(
                                    "WhiteSpace", nfx::json::Document{ std::move( whiteSpaceValue ) } );
                            }
                        }
                        formatObj.emplace_back( "Restriction", std::move( restrictionDoc ) );
                    }
                }

                // Range (optional)
                nfx::json::Document rangeDoc;
                bool hasRange = false;
                if( auto rangeOpt = property.range() )
                {
                    hasRange = true;
                    const auto& range = *rangeOpt;
                    rangeDoc = nfx::json::Document::object();
                    if( auto rangeRef = rangeDoc.rootRef<Object>() )
                    {
                        auto& rangeObj = rangeRef->get();
                        rangeObj.reserve( 2 );

                        double high = range.high();
                        if( high == std::trunc( high ) && std::isfinite( high ) )
                        {
                            rangeObj.emplace_back( "High", nfx::json::Document{ static_cast<int64_t>( high ) } );
                        }
                        else
                        {
                            rangeObj.emplace_back( "High", nfx::json::Document{ high } );
                        }

                        double low = range.low();
                        if( low == std::trunc( low ) && std::isfinite( low ) )
                        {
                            rangeObj.emplace_back( "Low", nfx::json::Document{ static_cast<int64_t>( low ) } );
                        }
                        else
                        {
                            rangeObj.emplace_back( "Low", nfx::json::Document{ low } );
                        }
                    }
                }

                // Unit (optional)
                nfx::json::Document unitDoc;
                bool hasUnit = false;
                if( auto unitOpt = property.unit() )
                {
                    hasUnit = true;
                    const auto& unit = *unitOpt;
                    unitDoc = nfx::json::Document::object();
                    if( auto unitRef = unitDoc.rootRef<Object>() )
                    {
                        auto& unitObj = unitRef->get();
                        unitObj.reserve( 3 ); // UnitSymbol, QuantityName, CustomElements

                        unitObj.emplace_back( "UnitSymbol", nfx::json::Document{ unit.unitSymbol() } );

                        if( auto quantityName = unit.quantityName() )
                        {
                            unitObj.emplace_back( "QuantityName", nfx::json::Document{ quantityName.value() } );
                        }

                        // CustomElements (optional)
                        if( auto customElemsOpt = unit.customElements() )
                        {
                            const auto& customElems = *customElemsOpt;
                            if( auto customElemsObj = customElems.get<Object>( "" ) )
                            {
                                for( auto&& [key, valueDoc] : customElemsObj.value() )
                                {
                                    unitObj.emplace_back( std::move( key ), std::move( valueDoc ) );
                                }
                            }
                        }
                    }
                }

                // Property
                nfx::json::Document propertyObj = nfx::json::Document::object();
                if( auto propertyRef = propertyObj.rootRef<Object>() )
                {
                    auto& propObj = propertyRef->get();
                    propObj.reserve( 10 ); // DataChannelType, Format, Range, Unit, + up to 6 optional fields

                    propObj.emplace_back( "DataChannelType", std::move( dataChannelTypeDoc ) );
                    propObj.emplace_back( "Format", std::move( formatDoc ) );

                    // Range (optional)
                    if( hasRange )
                    {
                        propObj.emplace_back( "Range", std::move( rangeDoc ) );
                    }
                    if( hasUnit )
                    {
                        propObj.emplace_back( "Unit", std::move( unitDoc ) );
                    }

                    if( auto qualityCoding = property.qualityCoding() )
                    {
                        propObj.emplace_back( "QualityCoding", nfx::json::Document{ qualityCoding.value() } );
                    }
                    if( auto alertPriority = property.alertPriority() )
                    {
                        propObj.emplace_back( "AlertPriority", nfx::json::Document{ alertPriority.value() } );
                    }
                    if( auto name = property.name() )
                    {
                        propObj.emplace_back( "Name", nfx::json::Document{ name.value() } );
                    }
                    if( auto remarks = property.remarks() )
                    {
                        propObj.emplace_back( "Remarks", nfx::json::Document{ remarks.value() } );
                    }

                    // CustomProperties (optional)
                    if( auto customPropsOpt = property.customProperties() )
                    {
                        const auto& customProps = *customPropsOpt;
                        if( auto customPropsObj = customProps.get<Object>( "" ) )
                        {
                            for( auto&& [key, valueDoc] : customPropsObj.value() )
                            {
                                propObj.emplace_back( std::move( key ), std::move( valueDoc ) );
                            }
                        }
                    }
                }

                nfx::json::Document channelJsonDoc = nfx::json::Document::object();
                if( auto channelRef = channelJsonDoc.rootRef<Object>() )
                {
                    auto& channelObj = channelRef->get();
                    channelObj.reserve( 2 ); // DataChannelID, Property
                    channelObj.emplace_back( "DataChannelID", std::move( dataChannelIdDoc ) );
                    channelObj.emplace_back( "Property", std::move( propertyObj ) );
                }

                dataChannelArray.emplace_back( std::move( channelJsonDoc ) );
            }

            // DataChannelList
            nfx::json::Document dataChannelListDoc = nfx::json::Document::object();
            if( auto dataChannelListRef = dataChannelListDoc.rootRef<Object>() )
            {
                auto& dataChannelListObj = dataChannelListRef->get();
                dataChannelListObj.reserve( 1 ); // DataChannel
                dataChannelListObj.emplace_back( "DataChannel", nfx::json::Document{ std::move( dataChannelArray ) } );
            }

            // Package
            nfx::json::Document packageDoc = nfx::json::Document::object();
            if( auto packageRef = packageDoc.rootRef<Object>() )
            {
                auto& packageObj = packageRef->get();
                packageObj.reserve( 2 ); // Header, DataChannelList
                packageObj.emplace_back( "Header", std::move( headerDoc ) );
                packageObj.emplace_back( "DataChannelList", std::move( dataChannelListDoc ) );
            }

            // Assign to root document
            jsonDoc["Package"] = std::move( packageDoc );
        }

        /**
         * @brief Deserialize DataChannelListPackage from JSON document
         * @param obj The object to deserialize into (will be assigned)
         * @param doc The JSON document to deserialize from
         */
        static void deserialize( const SerializableDocument& doc, datachannel::DataChannelListPackage& obj )
        {
            auto& jsonDoc = doc.document();

            auto& packageObj = jsonDoc["Package"];
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
            if( auto headerObjOpt = headerObj.get<Object>( "" ) )
            {
                static const std::unordered_set<std::string> knownKeys = {
                    "ShipID", "Author", "DateCreated", "DataChannelListID", "VersionInformation"
                };

                SerializableDocument customHdrs;
                auto& customHdrsDoc = customHdrs.document();
                bool hasCustomHeaders = false;
                for( const auto& [key, valueDoc] : headerObjOpt.value() )
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
                    customHeaders = std::move( customHdrs );
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
                            if( auto nameObjOpt = nameObjectDoc.get<Object>( "" ) )
                            {
                                SerializableDocument customNameObjs;
                                auto& customNameObjsDoc = customNameObjs.document();
                                bool hasCustomNameObjects = false;
                                for( const auto& [key, valueDoc] : nameObjOpt.value() )
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
                                    customNameObjects = std::move( customNameObjs );
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
                                if( auto unitObjOpt = unitObj.get<Object>( "" ) )
                                {
                                    SerializableDocument customElems;
                                    auto& customElemsDoc = customElems.document();
                                    bool hasCustomElements = false;
                                    for( const auto& [key, valueDoc] : unitObjOpt.value() )
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
                                        customElements = std::move( customElems );
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
                        if( auto propObjOpt = propertyObj.get<Object>( "" ) )
                        {
                            static const std::unordered_set<std::string> knownKeys = {
                                "DataChannelType", "Format",        "Range", "Unit",
                                "QualityCoding",   "AlertPriority", "Name",  "Remarks"
                            };
                            SerializableDocument customProps;
                            auto& customPropsDoc = customProps.document();
                            bool hasCustomProperties = false;
                            for( const auto& [key, valueDoc] : propObjOpt.value() )
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
                                customProperties = std::move( customProps );
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

            // Assign to obj
            obj = datachannel::DataChannelListPackage{ std::move( package ) };
        }
    };
} // namespace nfx::serialization::json
