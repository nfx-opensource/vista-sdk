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

#include "dnv/vista/sdk/transport/ISO19848.h"
#include "dnv/vista/sdk/transport/datachannel/DataChannel.h"
#include "ISO19848Validation.h"

#include <cmath>

namespace nfx::serialization::json
{

	/**
	 * @brief SerializationTraits specialization for DataChannelListPackage
	 * @details Converts Vista SDK domain objects directly to/from JSON
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::transport::datachannel::DataChannelListPackage>
	{
	public:
		/**
		 * @brief Serialize DataChannelListPackage to JSON document
		 * @param obj The DataChannelListPackage to serialize
		 * @param doc The JSON document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::transport::datachannel::DataChannelListPackage& obj, Document& doc )
		{
			using namespace dnv::vista::sdk;
			using namespace dnv::vista::sdk::transport;

			// Serialize Header
			const auto& header = obj.package().header();

			doc.set( "/Package/Header/ShipID", header.shipId().toString() );

			// Serialize DataChannelListID (ConfigurationReference)
			const auto& configRef = header.dataChannelListId();
			doc.set( "/Package/Header/DataChannelListID/ID", configRef.id() );

			if ( auto version = configRef.version() )
			{
				doc.set<std::string>( "/Package/Header/DataChannelListID/Version", version.value() );
			}
			doc.set( "/Package/Header/DataChannelListID/TimeStamp", configRef.timeStamp().toString( DateTime::Format::Iso8601PreciseTrimmed ) );

			// Serialize VersionInformation if present
			if ( auto versionInfoOpt = header.versionInformation() )
			{
				const auto& version = *versionInfoOpt;
				doc.set<std::string>( "/Package/Header/VersionInformation/NamingRule", version.namingRule() );
				doc.set<std::string>( "/Package/Header/VersionInformation/NamingSchemeVersion", version.namingSchemeVersion() );
				if ( auto refUrl = version.referenceUrl() )
				{
					doc.set<std::string>( "/Package/Header/VersionInformation/ReferenceURL", *refUrl );
				}
			}

			if ( auto author = header.author() )
			{
				doc.set<std::string>( "/Package/Header/Author", author.value() );
			}

			if ( auto dateCreated = header.dateCreated() )
			{
				doc.set( "/Package/Header/DateCreated", dateCreated->toString( DateTime::Format::Iso8601PreciseTrimmed ) );
			}

			// Serialize CustomHeaders if present
			if ( auto customHeadersOpt = header.customHeaders() )
			{
				const auto& customHdrs = *customHeadersOpt;
				if ( auto customHdrsObj = customHdrs.get<Document::Object>( "" ) )
				{
					for ( const auto& [key, valueDoc] : customHdrsObj.value() )
					{
						doc.set( "/Package/Header/" + key, valueDoc );
					}
				}
			}

			// Serialize DataChannelList
			const auto& channels = obj.package().dataChannelList();
			for ( size_t i = 0; i < channels.size(); ++i )
			{
				const auto& channel = channels[i];
				std::string channelBasePath = "/Package/DataChannelList/DataChannel/" + std::to_string( i );

				// Serialize DataChannelID
				const auto& channelId = channel.dataChannelId();
				doc.set( channelBasePath + "/DataChannelID/LocalID", channelId.localId().toString() );

				if ( auto shortId = channelId.shortId() )
				{
					doc.set<std::string>( channelBasePath + "/DataChannelID/ShortID", shortId.value() );
				}

				// Serialize NameObject if present
				if ( auto nameObjectOpt = channelId.nameObject() )
				{
					const auto& nameObj = *nameObjectOpt;
					doc.set( channelBasePath + "/DataChannelID/NameObject/NamingRule", nameObj.namingRule() );

					// Serialize CustomNameObjects if present
					if ( auto customNameObjsOpt = nameObj.customNameObjects() )
					{
						const auto& customNameObjs = *customNameObjsOpt;
						if ( auto customNameObjsObj = customNameObjs.get<Document::Object>( "" ) )
						{
							for ( const auto& [key, valueDoc] : customNameObjsObj.value() )
							{
								doc.set( channelBasePath + "/DataChannelID/NameObject/" + key, valueDoc );
							}
						}
					}
				}

				// Serialize Property
				const auto& property = channel.property();

				// DataChannelType
				const auto& channelType = property.dataChannelType();
				doc.set( channelBasePath + "/Property/DataChannelType/Type", channelType.type() );
				if ( auto updateCycle = channelType.updateCycle() )
				{
					double val = updateCycle.value();
					if ( val == std::trunc( val ) && std::isfinite( val ) )
					{
						doc.set<int64_t>( channelBasePath + "/Property/DataChannelType/UpdateCycle", static_cast<int64_t>( val ) );
					}
					else
					{
						doc.set<double>( channelBasePath + "/Property/DataChannelType/UpdateCycle", val );
					}
				}
				if ( auto calcPeriod = channelType.calculationPeriod() )
				{
					double val = calcPeriod.value();
					if ( val == std::trunc( val ) && std::isfinite( val ) )
					{
						doc.set<int64_t>( channelBasePath + "/Property/DataChannelType/CalculationPeriod", static_cast<int64_t>( val ) );
					}
					else
					{
						doc.set<double>( channelBasePath + "/Property/DataChannelType/CalculationPeriod", val );
					}
				}

				// Format
				const auto& format = property.format();
				doc.set( channelBasePath + "/Property/Format/Type", format.type() );

				// Restriction
				if ( auto restrictionOpt = format.restriction() )
				{
					const auto& restriction = *restrictionOpt;
					std::string restrictionPath = channelBasePath + "/Property/Format/Restriction";

					// 1. Enumeration
					if ( auto enumOpt = restriction.enumeration() )
					{
						const auto& enumValues = *enumOpt;
						for ( size_t j = 0; j < enumValues.size(); ++j )
						{
							doc.set( restrictionPath + "/Enumeration/" + std::to_string( j ), enumValues[j] );
						}
					}

					// 2. FractionDigits
					if ( auto fracDigits = restriction.fractionDigits() )
					{
						doc.set<uint32_t>( restrictionPath + "/FractionDigits", fracDigits.value() );
					}

					// 3. Length
					if ( auto length = restriction.length() )
					{
						doc.set<uint32_t>( restrictionPath + "/Length", length.value() );
					}

					// 4. MaxExclusive
					if ( auto maxExc = restriction.maxExclusive() )
					{
						doc.set<double>( restrictionPath + "/MaxExclusive", maxExc.value() );
					}

					// 5. MaxInclusive
					if ( auto maxInc = restriction.maxInclusive() )
					{
						doc.set<double>( restrictionPath + "/MaxInclusive", maxInc.value() );
					}

					// 6. MaxLength
					if ( auto maxLen = restriction.maxLength() )
					{
						doc.set<uint32_t>( restrictionPath + "/MaxLength", maxLen.value() );
					}

					// 7. MinExclusive
					if ( auto minExc = restriction.minExclusive() )
					{
						doc.set<double>( restrictionPath + "/MinExclusive", minExc.value() );
					}

					// 8. MinInclusive
					if ( auto minInc = restriction.minInclusive() )
					{
						doc.set<double>( restrictionPath + "/MinInclusive", minInc.value() );
					}

					// 9. MinLength
					if ( auto minLen = restriction.minLength() )
					{
						doc.set<uint32_t>( restrictionPath + "/MinLength", minLen.value() );
					}

					// 10. Pattern
					if ( auto pattern = restriction.pattern() )
					{
						doc.set<std::string>( restrictionPath + "/Pattern", pattern.value() );
					}

					// 11. TotalDigits
					if ( auto totalDigits = restriction.totalDigits() )
					{
						doc.set<uint32_t>( restrictionPath + "/TotalDigits", totalDigits.value() );
					}

					// 12. WhiteSpace
					if ( auto whiteSpace = restriction.whiteSpace() )
					{
						std::string whiteSpaceValue;
						switch ( *whiteSpace )
						{
							case datachannel::WhiteSpace::Preserve:
							{
								whiteSpaceValue = "Preserve";
								break;
							}
							case datachannel::WhiteSpace::Replace:
							{
								whiteSpaceValue = "Replace";
								break;
							}
							case datachannel::WhiteSpace::Collapse:
							{
								whiteSpaceValue = "Collapse";
								break;
							}
						}
						doc.set( restrictionPath + "/WhiteSpace", std::move( whiteSpaceValue ) );
					}
				}

				// Range (optional)
				if ( auto rangeOpt = property.range() )
				{
					const auto& range = *rangeOpt;
					double high = range.high();
					if ( high == std::trunc( high ) && std::isfinite( high ) )
					{
						doc.set<int64_t>( channelBasePath + "/Property/Range/High", static_cast<int64_t>( high ) );
					}
					else
					{
						doc.set<double>( channelBasePath + "/Property/Range/High", high );
					}
					double low = range.low();
					if ( low == std::trunc( low ) && std::isfinite( low ) )
					{
						doc.set<int64_t>( channelBasePath + "/Property/Range/Low", static_cast<int64_t>( low ) );
					}
					else
					{
						doc.set<double>( channelBasePath + "/Property/Range/Low", low );
					}
				}

				// Unit (optional)
				if ( auto unitOpt = property.unit() )
				{
					const auto& unit = *unitOpt;
					doc.set( channelBasePath + "/Property/Unit/UnitSymbol", unit.unitSymbol() );
					if ( auto quantityName = unit.quantityName() )
					{
						doc.set<std::string>( channelBasePath + "/Property/Unit/QuantityName", quantityName.value() );
					}

					// CustomElements (optional)
					if ( auto customElemsOpt = unit.customElements() )
					{
						const auto& customElems = *customElemsOpt;
						if ( auto customElemsObj = customElems.get<Document::Object>( "" ) )
						{
							for ( const auto& [key, valueDoc] : customElemsObj.value() )
							{
								doc.set( channelBasePath + "/Property/Unit/" + key, valueDoc );
							}
						}
					}
				}

				// Optional Property fields
				if ( auto qualityCoding = property.qualityCoding() )
				{
					doc.set<std::string>( channelBasePath + "/Property/QualityCoding", qualityCoding.value() );
				}
				if ( auto alertPriority = property.alertPriority() )
				{
					doc.set<std::string>( channelBasePath + "/Property/AlertPriority", alertPriority.value() );
				}
				if ( auto name = property.name() )
				{
					doc.set<std::string>( channelBasePath + "/Property/Name", name.value() );
				}
				if ( auto remarks = property.remarks() )
				{
					doc.set<std::string>( channelBasePath + "/Property/Remarks", remarks.value() );
				}

				// CustomProperties (optional)
				if ( auto customPropsOpt = property.customProperties() )
				{
					const auto& customProps = *customPropsOpt;
					if ( auto customPropsObj = customProps.get<Document::Object>( "" ) )
					{
						for ( const auto& [key, valueDoc] : customPropsObj.value() )
						{
							doc.set( channelBasePath + "/Property/" + key, valueDoc );
						}
					}
				}
			}
		}

		/**
		 * @brief Deserialize DataChannelListPackage from JSON document
		 * @param obj The object to deserialize into (will be assigned)
		 * @param doc The JSON document to deserialize from
		 */
		static void
		deserialize( dnv::vista::sdk::transport::datachannel::DataChannelListPackage& obj, const Document& doc )

		{
			using namespace dnv::vista::sdk;
			using namespace dnv::vista::sdk::transport;

			// Deserialize Header
			auto shipIdStr = doc.get<std::string>( "/Package/Header/ShipID" ).value();
			auto shipId = ShipId::fromString( shipIdStr ).value();

			// Deserialize DataChannelListID (ConfigurationReference)
			auto configId = doc.get<std::string>( "/Package/Header/DataChannelListID/ID" ).value();
			auto configTimeStampStr = doc.get<std::string>( "/Package/Header/DataChannelListID/TimeStamp" ).value();
			if ( !isValidIso19848DateTimeOffset( configTimeStampStr ) )
			{
				throw std::runtime_error{ "Invalid ISO 19848 DateTimeOffset format in DataChannelListID/TimeStamp: " + configTimeStampStr };
			}
			DateTimeOffset configTimeStamp;
			if ( !DateTimeOffset::fromString( configTimeStampStr, configTimeStamp ) )
			{
				throw std::runtime_error{ "Failed to parse DateTimeOffset in DataChannelListID/TimeStamp: " + configTimeStampStr };
			}

			std::optional<std::string> configVersion;
			if ( auto versionOpt = doc.get<std::string>( "/Package/Header/DataChannelListID/Version" ) )
			{
				configVersion = versionOpt.value();
			}

			datachannel::ConfigurationReference dataChannelListId{ configId, configTimeStamp, configVersion };

			// Deserialize VersionInformation if present
			std::optional<datachannel::VersionInformation> versionInfo;
			if ( auto namingRuleOpt = doc.get<std::string>( "/Package/Header/VersionInformation/NamingRule" ) )
			{
				auto namingRule = namingRuleOpt.value();
				auto namingSchemeVersion = doc.get<std::string>( "/Package/Header/VersionInformation/NamingSchemeVersion" ).value();
				std::optional<std::string> referenceUrl;
				if ( auto refUrlOpt = doc.get<std::string>( "/Package/Header/VersionInformation/ReferenceURL" ) )
				{
					referenceUrl = refUrlOpt.value();
				}
				versionInfo = datachannel::VersionInformation{ namingRule, namingSchemeVersion, referenceUrl };
			}

			// Deserialize Author
			std::optional<std::string> author;
			if ( auto authorOpt = doc.get<std::string>( "/Package/Header/Author" ) )
			{
				author = authorOpt.value();
			}

			// Deserialize DateCreated
			std::optional<DateTimeOffset> dateCreated;
			if ( auto dateCreatedStrOpt = doc.get<std::string>( "/Package/Header/DateCreated" ) )
			{
				if ( !isValidIso19848DateTimeOffset( dateCreatedStrOpt.value() ) )
				{
					throw std::runtime_error{ "Invalid ISO 19848 DateTimeOffset format in DateCreated: " + dateCreatedStrOpt.value() };
				}
				DateTimeOffset dt;
				if ( DateTimeOffset::fromString( dateCreatedStrOpt.value(), dt ) )
				{
					dateCreated = dt;
				}
			}
			std::optional<datachannel::CustomHeaders> customHeaders;
			if ( auto headerObjOpt = doc.get<Document::Object>( "/Package/Header" ) )
			{
				// Known property names in Header
				const std::unordered_set<std::string> knownKeys = {
					"ShipID", "Author", "DateCreated", "DataChannelListID", "VersionInformation" };

				Document customHdrs;
				bool hasCustomHeaders = false;
				for ( const auto& [key, valueDoc] : headerObjOpt.value() )
				{
					// Skip known properties
					if ( knownKeys.find( key ) != knownKeys.end() )
					{
						continue;
					}

					// Deserialize unknown properties as custom headers
					customHdrs.set( key, valueDoc );
					hasCustomHeaders = true;
				}
				if ( hasCustomHeaders )
				{
					customHeaders = std::move( customHdrs );
				}
			}

			// Create Header
			datachannel::Header header{ shipId, dataChannelListId, versionInfo, author, dateCreated, customHeaders };

			// Deserialize DataChannelList
			std::vector<datachannel::DataChannel> channels;

			// Iterate through DataChannel array
			size_t channelIndex = 0;
			while ( true )
			{
				std::string channelBasePath = "/Package/DataChannelList/DataChannel/" + std::to_string( channelIndex );

				// Check if this channel exists
				if ( !doc.contains( channelBasePath + "/DataChannelID/LocalID" ) )
				{
					break;
				}

				// Deserialize DataChannelID
				auto localIdStr = doc.get<std::string>( channelBasePath + "/DataChannelID/LocalID" ).value();
				auto localId = LocalId::fromString( localIdStr ).value();

				std::optional<std::string> shortId;
				if ( auto shortIdOpt = doc.get<std::string>( channelBasePath + "/DataChannelID/ShortID" ) )
				{
					shortId = shortIdOpt.value();
				}

				// Deserialize NameObject if present
				std::optional<datachannel::NameObject> nameObject;
				if ( auto namingRuleOpt = doc.get<std::string>( channelBasePath + "/DataChannelID/NameObject/NamingRule" ) )
				{
					auto namingRule = namingRuleOpt.value();

					// Deserialize CustomNameObjects if present
					std::optional<datachannel::CustomNameObjects> customNameObjects;
					std::string nameObjectPath = channelBasePath + "/DataChannelID/NameObject";
					if ( auto nameObjOpt = doc.get<Document::Object>( nameObjectPath ) )
					{
						const std::unordered_set<std::string> knownKeys = { "NamingRule" };
						Document customNameObjs;
						bool hasCustomNameObjects = false;
						for ( const auto& [key, valueDoc] : nameObjOpt.value() )
						{
							if ( knownKeys.find( key ) != knownKeys.end() )
							{
								continue;
							}
							customNameObjs.set( key, valueDoc );
							hasCustomNameObjects = true;
						}
						if ( hasCustomNameObjects )
						{
							customNameObjects = std::move( customNameObjs );
						}
					}

					datachannel::NameObject no{ namingRule, customNameObjects };
					nameObject = std::move( no );
				}

				datachannel::DataChannelId dataChannelId{ localId, shortId, nameObject };

				// Deserialize Property
				auto dataChannelTypeStr = doc.get<std::string>( channelBasePath + "/Property/DataChannelType/Type" ).value();
				std::optional<double> updateCycle;
				if ( auto intOpt = doc.get<int64_t>( channelBasePath + "/Property/DataChannelType/UpdateCycle" ) )
				{
					updateCycle = static_cast<double>( intOpt.value() );
				}
				else if ( auto dblOpt = doc.get<double>( channelBasePath + "/Property/DataChannelType/UpdateCycle" ) )
				{
					updateCycle = dblOpt.value();
				}
				std::optional<double> calculationPeriod;
				if ( auto intOpt = doc.get<int64_t>( channelBasePath + "/Property/DataChannelType/CalculationPeriod" ) )
				{
					calculationPeriod = static_cast<double>( intOpt.value() );
				}
				else if ( auto dblOpt = doc.get<double>( channelBasePath + "/Property/DataChannelType/CalculationPeriod" ) )
				{
					calculationPeriod = dblOpt.value();
				}
				datachannel::DataChannelType dataChannelType{ dataChannelTypeStr, updateCycle, calculationPeriod };

				// Deserialize Format
				auto formatTypeStr = doc.get<std::string>( channelBasePath + "/Property/Format/Type" ).value();

				// Deserialize Restriction if present
				std::optional<datachannel::Restriction> restriction;
				if ( doc.contains( channelBasePath + "/Property/Format/Restriction" ) )
				{
					datachannel::Restriction r{};
					// Numeric bounds
					if ( auto maxIncOpt = doc.get<double>( channelBasePath + "/Property/Format/Restriction/MaxInclusive" ) )
					{
						r.setMaxInclusive( maxIncOpt.value() );
					}
					if ( auto minIncOpt = doc.get<double>( channelBasePath + "/Property/Format/Restriction/MinInclusive" ) )
					{
						r.setMinInclusive( minIncOpt.value() );
					}
					if ( auto maxExcOpt = doc.get<double>( channelBasePath + "/Property/Format/Restriction/MaxExclusive" ) )
					{
						r.setMaxExclusive( maxExcOpt.value() );
					}
					if ( auto minExcOpt = doc.get<double>( channelBasePath + "/Property/Format/Restriction/MinExclusive" ) )
					{
						r.setMinExclusive( minExcOpt.value() );
					}
					// Length constraints
					if ( auto fracDigitsOpt = doc.get<uint32_t>( channelBasePath + "/Property/Format/Restriction/FractionDigits" ) )
					{
						r.setFractionDigits( fracDigitsOpt.value() );
					}
					if ( auto maxLengthOpt = doc.get<uint32_t>( channelBasePath + "/Property/Format/Restriction/MaxLength" ) )
					{
						r.setMaxLength( maxLengthOpt.value() );
					}
					if ( auto minLengthOpt = doc.get<uint32_t>( channelBasePath + "/Property/Format/Restriction/MinLength" ) )
					{
						r.setMinLength( minLengthOpt.value() );
					}
					if ( auto lengthOpt = doc.get<uint32_t>( channelBasePath + "/Property/Format/Restriction/Length" ) )
					{
						r.setLength( lengthOpt.value() );
					}
					if ( auto totalDigitsOpt = doc.get<uint32_t>( channelBasePath + "/Property/Format/Restriction/TotalDigits" ) )
					{
						r.setTotalDigits( totalDigitsOpt.value() );
					}
					// Pattern
					if ( auto patternOpt = doc.get<std::string>( channelBasePath + "/Property/Format/Restriction/Pattern" ) )
					{
						r.setPattern( patternOpt.value() );
					}
					// Enumeration (array of strings)
					if ( doc.contains( channelBasePath + "/Property/Format/Restriction/Enumeration" ) )
					{
						std::vector<std::string> enumVec;
						size_t enumIdx = 0;
						while ( true )
						{
							std::string enumPath = channelBasePath + "/Property/Format/Restriction/Enumeration/" + std::to_string( enumIdx );
							if ( auto enumValOpt = doc.get<std::string>( enumPath ) )
							{
								enumVec.push_back( enumValOpt.value() );
								++enumIdx;
							}
							else
							{
								break;
							}
						}
						if ( !enumVec.empty() )
						{
							r.setEnumeration( std::move( enumVec ) );
						}
					}
					// WhiteSpace
					if ( auto whiteSpaceOpt = doc.get<std::string>( channelBasePath + "/Property/Format/Restriction/WhiteSpace" ) )
					{
						auto wsStr = whiteSpaceOpt.value();
						if ( wsStr == "Preserve" )
						{
							r.setWhiteSpace( datachannel::WhiteSpace::Preserve );
						}
						else if ( wsStr == "Replace" )
						{
							r.setWhiteSpace( datachannel::WhiteSpace::Replace );
						}
						else if ( wsStr == "Collapse" )
						{
							r.setWhiteSpace( datachannel::WhiteSpace::Collapse );
						}
					}
					restriction = std::move( r );
				}

				datachannel::Format format{ formatTypeStr, restriction };

				// Deserialize optional Property fields
				std::optional<datachannel::Range> range;
				if ( doc.contains( channelBasePath + "/Property/Range/High" ) )
				{
					double high = 0.0;
					if ( auto highIntOpt = doc.get<int64_t>( channelBasePath + "/Property/Range/High" ) )
					{
						high = static_cast<double>( highIntOpt.value() );
					}
					else if ( auto highDblOpt = doc.get<double>( channelBasePath + "/Property/Range/High" ) )
					{
						high = highDblOpt.value();
					}
					double low = 0.0;
					if ( auto lowIntOpt = doc.get<int64_t>( channelBasePath + "/Property/Range/Low" ) )
					{
						low = static_cast<double>( lowIntOpt.value() );
					}
					else if ( auto lowDblOpt = doc.get<double>( channelBasePath + "/Property/Range/Low" ) )
					{
						low = lowDblOpt.value();
					}
					range = datachannel::Range{ low, high };
				}

				std::optional<datachannel::Unit> unit;
				if ( auto unitSymbolOpt = doc.get<std::string>( channelBasePath + "/Property/Unit/UnitSymbol" ) )
				{
					auto unitSymbol = unitSymbolOpt.value();
					std::optional<std::string> quantityName;
					if ( auto qnOpt = doc.get<std::string>( channelBasePath + "/Property/Unit/QuantityName" ) )
					{
						quantityName = qnOpt.value();
					}

					// Deserialize CustomElements if present
					std::optional<datachannel::CustomElements> customElements;
					std::string unitPath = channelBasePath + "/Property/Unit";
					if ( auto unitObjOpt = doc.get<Document::Object>( unitPath ) )
					{
						const std::unordered_set<std::string> knownKeys = { "UnitSymbol", "QuantityName" };
						Document customElems;
						bool hasCustomElements = false;
						for ( const auto& [key, valueDoc] : unitObjOpt.value() )
						{
							if ( knownKeys.find( key ) != knownKeys.end() )
							{
								continue;
							}
							customElems.set( key, valueDoc );
							hasCustomElements = true;
						}
						if ( hasCustomElements )
						{
							customElements = std::move( customElems );
						}
					}

					unit = datachannel::Unit{ unitSymbol, quantityName, customElements };
				}

				std::optional<std::string> name;
				if ( auto nameOpt = doc.get<std::string>( channelBasePath + "/Property/Name" ) )
				{
					name = nameOpt.value();
				}

				std::optional<std::string> qualityCoding;
				if ( auto qcOpt = doc.get<std::string>( channelBasePath + "/Property/QualityCoding" ) )
				{
					qualityCoding = qcOpt.value();
				}

				std::optional<std::string> alertPriority;
				if ( auto apOpt = doc.get<std::string>( channelBasePath + "/Property/AlertPriority" ) )
				{
					alertPriority = apOpt.value();
				}

				std::optional<std::string> remarks;
				if ( auto remarksOpt = doc.get<std::string>( channelBasePath + "/Property/Remarks" ) )
				{
					remarks = remarksOpt.value();
				}

				// Deserialize CustomProperties if present
				std::optional<datachannel::CustomProperties> customProperties;
				std::string propertyPath = channelBasePath + "/Property";
				if ( auto propObjOpt = doc.get<Document::Object>( propertyPath ) )
				{
					const std::unordered_set<std::string> knownKeys = {
						"DataChannelType", "Format", "Range", "Unit", "QualityCoding",
						"AlertPriority", "Name", "Remarks" };
					Document customProps;
					bool hasCustomProperties = false;
					for ( const auto& [key, valueDoc] : propObjOpt.value() )
					{
						if ( knownKeys.find( key ) != knownKeys.end() )
						{
							continue;
						}
						customProps.set( key, valueDoc );
						hasCustomProperties = true;
					}
					if ( hasCustomProperties )
					{
						customProperties = std::move( customProps );
					}
				}

				datachannel::Property property{ dataChannelType, format, range, unit, qualityCoding, alertPriority, name, remarks, customProperties };

				channels.emplace_back( datachannel::DataChannel{ dataChannelId, property } );
				++channelIndex;
			}

			datachannel::DataChannelList dataChannelList{ std::move( channels ) };

			// Create Package
			datachannel::Package package{ header, std::move( dataChannelList ) };

			// Assign to obj
			obj = datachannel::DataChannelListPackage{ std::move( package ) };
		}
	};
} // namespace nfx::serialization::json
