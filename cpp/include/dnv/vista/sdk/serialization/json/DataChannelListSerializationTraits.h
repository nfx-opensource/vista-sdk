/**
 * @file DataChannelListSerializationTraits.h
 * @brief JSON serialization specialization for DataChannelList domain objects
 * @details Provides SerializationTraits specializations for Vista SDK DataChannelList
 */

#pragma once

#include <nfx/serialization/json/SerializationTraits.h>
#include <nfx/serialization/json/extensions/ContainersTraits.h>
#include <nfx/serialization/json/Document.h>

#include "dnv/vista/sdk/transport/ISO19848.h"
#include "dnv/vista/sdk/transport/ISO19848Constants.h"
#include "dnv/vista/sdk/transport/datachannel/DataChannel.h"

namespace nfx::serialization::json
{
	//=====================================================================
	// DataChannelListPackage serialization specialization
	//=====================================================================

	/**
	 * @brief SerializationTraits specialization for DataChannelListPackage
	 * @details Converts Vista SDK domain objects to JSON-serializable format
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::transport::DataChannelListPackage>
	{
		/**
		 * @brief Serialize DataChannelListPackage to JSON document
		 * @param obj The DataChannelListPackage to serialize
		 * @param doc The JSON document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::transport::DataChannelListPackage& obj, Document& doc )
		{
			// Serialize Header
			const auto& header = obj.package().header();

			doc.set( "/Package/Header/ShipID", header.shipId().toString() );
			doc.set( "/Package/Header/Author", header.author() );
			doc.set( "/Package/Header/DateCreated", header.dateCreated().toIso8601Extended() );

			// Serialize DataChannelListID (ConfigurationReference)
			const auto& configRef = header.dataChannelListId();
			doc.set( "/Package/Header/DataChannelListID/ID", configRef.id() );
			doc.set( "/Package/Header/DataChannelListID/TimeStamp", configRef.timeStamp().toIso8601Extended() );
			if ( configRef.version().has_value() )
			{
				doc.set( "/Package/Header/DataChannelListID/Version", configRef.version().value() );
			}

			// Serialize VersionInformation if present
			if ( header.versionInformation().has_value() )
			{
				const auto& version = header.versionInformation().value();
				doc.set<std::string>( "/Package/Header/VersionInformation/NamingRule", version.namingRule() );
				doc.set<std::string>( "/Package/Header/VersionInformation/NamingSchemeVersion", version.namingSchemeVersion() );
				if ( version.referenceUrl().has_value() )
				{
					doc.set<std::string>( "/Package/Header/VersionInformation/ReferenceURL", version.referenceUrl().value() );
				}
			}

			// Serialize CustomHeaders if present
			if ( header.customHeaders().has_value() )
			{
				const auto& customHdrs = header.customHeaders().value();

				for ( const auto& [key, value] : customHdrs )
				{
					std::string customHeaderBasePath = "/Package/Header/CustomHeaders/" + key;

					// Serialize ONLY ISO 19848 compliant Value types with constants
					switch ( value.type() )
					{
						case dnv::vista::sdk::transport::Value::Type::Decimal:
						{
							doc.set( customHeaderBasePath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::DECIMAL );
							doc.set( customHeaderBasePath + "/value", value.decimal().value().toString() );
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::Integer:
						{
							doc.set( customHeaderBasePath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::INTEGER );
							doc.set( customHeaderBasePath + "/value", value.integer().value() );
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::Boolean:
						{
							doc.set( customHeaderBasePath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::BOOLEAN );
							doc.set( customHeaderBasePath + "/value", value.boolean().value() );
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::String:
						{
							doc.set( customHeaderBasePath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::STRING );
							doc.set( customHeaderBasePath + "/value", value.string().value() );
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::DateTime:
						{
							doc.set( customHeaderBasePath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::DATETIME );
							doc.set( customHeaderBasePath + "/value", value.dateTime().value().toIso8601Extended() );
							break;
						}
						// NON-ISO 19848 types - throw runtime error
						default:
						{
							throw std::runtime_error( "Value type not supported by ISO 19848 standard in Header CustomHeaders serialization" );
						}
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

				if ( channelId.shortId().has_value() )
				{
					doc.set( channelBasePath + "/DataChannelID/ShortID", channelId.shortId().value() );
				}

				// Serialize NameObject if present
				if ( channelId.nameObject().has_value() )
				{
					const auto& nameObj = channelId.nameObject().value();
					doc.set( channelBasePath + "/DataChannelID/NameObject/NamingRule", nameObj.namingRule() );

					// Serialize CustomNameObjects if present
					if ( nameObj.customNameObjects().has_value() )
					{
						const auto& customMap = nameObj.customNameObjects().value();

						for ( const auto& [key, value] : customMap )
						{
							std::string customPath = channelBasePath + "/DataChannelID/NameObject/CustomNameObjects/" + key;

							// Serialize ONLY ISO 19848 compliant Value types with constants
							switch ( value.type() )
							{
								case dnv::vista::sdk::transport::Value::Type::Decimal:
								{
									doc.set( customPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::DECIMAL );
									doc.set( customPath + "/value", value.decimal().value().toString() );
									break;
								}
								case dnv::vista::sdk::transport::Value::Type::Integer:
								{
									doc.set( customPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::INTEGER );
									doc.set( customPath + "/value", value.integer().value() );
									break;
								}
								case dnv::vista::sdk::transport::Value::Type::Boolean:
								{
									doc.set( customPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::BOOLEAN );
									doc.set( customPath + "/value", value.boolean().value() );
									break;
								}
								case dnv::vista::sdk::transport::Value::Type::String:
								{
									doc.set( customPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::STRING );
									doc.set( customPath + "/value", value.string().value() );
									break;
								}
								case dnv::vista::sdk::transport::Value::Type::DateTime:
								{
									doc.set( customPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::DATETIME );
									doc.set( customPath + "/value", value.dateTime().value().toIso8601Extended() );
									break;
								}
								// NON-ISO 19848 types - throw runtime error
								default:
								{
									throw std::runtime_error( "Value type not supported by ISO 19848 standard in CustomNameObjects serialization" );
								}
							}
						}
					}
				}

				// Serialize Property
				const auto& property = channel.property();

				// DataChannelType
				const auto& channelType = property.dataChannelType();
				doc.set( channelBasePath + "/Property/DataChannelType/Type", channelType.type() );
				if ( channelType.updateCycle().has_value() )
				{
					doc.set( channelBasePath + "/Property/DataChannelType/UpdateCycle", channelType.updateCycle().value() );
				}
				if ( channelType.calculationPeriod().has_value() )
				{
					doc.set( channelBasePath + "/Property/DataChannelType/CalculationPeriod", channelType.calculationPeriod().value() );
				}

				// Format
				const auto& format = property.format();
				doc.set( channelBasePath + "/Property/Format/Type", format.type() );

				// Restriction
				if ( format.restriction().has_value() )
				{
					const auto& restriction = format.restriction().value();
					std::string restrictionPath = channelBasePath + "/Property/Format/Restriction";

					// Enumeration
					if ( restriction.enumeration().has_value() )
					{
						const auto& enumValues = restriction.enumeration().value();
						for ( size_t j = 0; j < enumValues.size(); ++j )
						{
							doc.set( restrictionPath + "/Enumeration/" + std::to_string( j ), enumValues[j] );
						}
					}

					// Pattern
					if ( restriction.pattern().has_value() )
					{
						doc.set( restrictionPath + "/Pattern", restriction.pattern().value() );
					}

					// Numeric bounds
					if ( restriction.minInclusive().has_value() )
					{
						doc.set( restrictionPath + "/MinInclusive", restriction.minInclusive().value() );
					}
					if ( restriction.maxInclusive().has_value() )
					{
						doc.set( restrictionPath + "/MaxInclusive", restriction.maxInclusive().value() );
					}
					if ( restriction.minExclusive().has_value() )
					{
						doc.set( restrictionPath + "/MinExclusive", restriction.minExclusive().value() );
					}
					if ( restriction.maxExclusive().has_value() )
					{
						doc.set( restrictionPath + "/MaxExclusive", restriction.maxExclusive().value() );
					}

					// Length constraints
					if ( restriction.length().has_value() )
					{
						doc.set( restrictionPath + "/Length", restriction.length().value() );
					}
					if ( restriction.minLength().has_value() )
					{
						doc.set( restrictionPath + "/MinLength", restriction.minLength().value() );
					}
					if ( restriction.maxLength().has_value() )
					{
						doc.set( restrictionPath + "/MaxLength", restriction.maxLength().value() );
					}

					// Digit constraints
					if ( restriction.totalDigits().has_value() )
					{
						doc.set( restrictionPath + "/TotalDigits", restriction.totalDigits().value() );
					}
					if ( restriction.fractionDigits().has_value() )
					{
						doc.set( restrictionPath + "/FractionDigits", restriction.fractionDigits().value() );
					}

					// Whitespace handling
					if ( restriction.whiteSpace().has_value() )
					{
						std::string whiteSpaceValue;
						switch ( restriction.whiteSpace().value() )
						{
							case dnv::vista::sdk::transport::WhiteSpace::Preserve:
							{
								whiteSpaceValue = "Preserve";
								break;
							}
							case dnv::vista::sdk::transport::WhiteSpace::Replace:
							{
								whiteSpaceValue = "Replace";
								break;
							}
							case dnv::vista::sdk::transport::WhiteSpace::Collapse:
							{
								whiteSpaceValue = "Collapse";
								break;
							}
						}
						doc.set( restrictionPath + "/WhiteSpace", std::move( whiteSpaceValue ) );
					}
				}

				// Range (optional)
				if ( property.range().has_value() )
				{
					const auto& range = property.range().value();
					doc.set( channelBasePath + "/Property/Range/High", range.high() );
					doc.set( channelBasePath + "/Property/Range/Low", range.low() );
				}

				// Unit (optional)
				if ( property.unit().has_value() )
				{
					const auto& unit = property.unit().value();
					doc.set( channelBasePath + "/Property/Unit/UnitSymbol", unit.unitSymbol() );
					if ( unit.quantityName().has_value() )
					{
						doc.set( channelBasePath + "/Property/Unit/QuantityName", unit.quantityName().value() );
					}

					// CustomElements (optional)
					if ( unit.customElements().has_value() )
					{
						const auto& customElems = unit.customElements().value();

						for ( const auto& [key, value] : customElems )
						{
							std::string customElemPath = channelBasePath + "/Property/Unit/CustomElements/" + key;

							// Serialize ONLY ISO 19848 compliant Value types with constants
							switch ( value.type() )
							{
								case dnv::vista::sdk::transport::Value::Type::Decimal:
								{
									doc.set( customElemPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::DECIMAL );
									doc.set( customElemPath + "/value", value.decimal().value().toString() );
									break;
								}
								case dnv::vista::sdk::transport::Value::Type::Integer:
								{
									doc.set( customElemPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::INTEGER );
									doc.set( customElemPath + "/value", value.integer().value() );
									break;
								}
								case dnv::vista::sdk::transport::Value::Type::Boolean:
								{
									doc.set( customElemPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::BOOLEAN );
									doc.set( customElemPath + "/value", value.boolean().value() );
									break;
								}
								case dnv::vista::sdk::transport::Value::Type::String:
								{
									doc.set( customElemPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::STRING );
									doc.set( customElemPath + "/value", value.string().value() );
									break;
								}
								case dnv::vista::sdk::transport::Value::Type::DateTime:
								{
									doc.set( customElemPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::DATETIME );
									doc.set( customElemPath + "/value", value.dateTime().value().toIso8601Extended() );
									break;
								}
								// NON-ISO 19848 types - throw runtime error
								default:
								{
									throw std::runtime_error( "Value type not supported by ISO 19848 standard in Unit CustomElements serialization" );
								}
							}
						}
					}
				}

				// Optional Property fields
				if ( property.name().has_value() )
				{
					doc.set( channelBasePath + "/Property/Name", property.name().value() );
				}
				if ( property.qualityCoding().has_value() )
				{
					doc.set( channelBasePath + "/Property/QualityCoding", property.qualityCoding().value() );
				}
				if ( property.alertPriority().has_value() )
				{
					doc.set( channelBasePath + "/Property/AlertPriority", property.alertPriority().value() );
				}
				if ( property.remarks().has_value() )
				{
					doc.set( channelBasePath + "/Property/Remarks", property.remarks().value() );
				}

				// CustomProperties (optional)
				if ( property.customProperties().has_value() )
				{
					const auto& customProps = property.customProperties().value();

					for ( const auto& [key, value] : customProps )
					{
						std::string customPropPath = channelBasePath + "/Property/CustomProperties/" + key;

						// Serialize ONLY ISO 19848 compliant Value types with constants
						switch ( value.type() )
						{
							case dnv::vista::sdk::transport::Value::Type::Decimal:
							{
								doc.set( customPropPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::DECIMAL );
								doc.set( customPropPath + "/value", value.decimal().value().toString() );
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::Integer:
							{
								doc.set( customPropPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::INTEGER );
								doc.set( customPropPath + "/value", value.integer().value() );
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::Boolean:
							{
								doc.set( customPropPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::BOOLEAN );
								doc.set( customPropPath + "/value", value.boolean().value() );
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::String:
							{
								doc.set( customPropPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::STRING );
								doc.set( customPropPath + "/value", value.string().value() );
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::DateTime:
							{
								doc.set( customPropPath + "/type", dnv::vista::sdk::iso19848::formatdatatypes::DATETIME );
								doc.set( customPropPath + "/value", value.dateTime().value().toIso8601Extended() );
								break;
							}
							// NON-ISO 19848 types - throw runtime error
							default:
							{
								throw std::runtime_error( "Value type not supported by ISO 19848 standard in CustomProperties serialization" );
							}
						}
					}
				}
			}
		}

		/**
		 * @brief Deserialize DataChannelListPackage from JSON document
		 * @param obj The DataChannelListPackage to deserialize into
		 * @param doc The JSON document to deserialize from
		 */
		static void deserialize( dnv::vista::sdk::transport::DataChannelListPackage& obj, const Document& doc )
		{
			using namespace dnv::vista::sdk;
			using namespace dnv::vista::sdk::transport;

			// Get Package root
			auto packageOpt = doc.get<Document>( "/Package" );
			if ( !packageOpt.has_value() )
			{
				throw std::runtime_error( "Missing 'Package' root object in JSON" );
			}
			const auto& packageDoc = packageOpt.value();

			// Get Header
			auto headerOpt = packageDoc.get<Document>( "/Header" );
			if ( !headerOpt.has_value() )
			{
				throw std::runtime_error( "Missing 'Header' object in Package" );
			}
			const auto& headerDoc = headerOpt.value();

			// Parse ShipID
			auto shipIdOpt = headerDoc.get<std::string>( "/ShipID" );
			if ( !shipIdOpt.has_value() )
			{
				throw std::runtime_error( "Missing 'ShipID' in Header" );
			}
			auto shipId = ShipId::parse( shipIdOpt.value() );

			// Parse Author (optional)
			auto authorOpt = headerDoc.get<std::string>( "/Author" );
			std::string author = authorOpt.has_value() ? authorOpt.value() : "";

			// Parse DateCreated (optional)
			auto dateCreatedOpt = headerDoc.get<std::string>( "/DateCreated" );
			nfx::time::DateTimeOffset dateCreated = dateCreatedOpt.has_value()
														? nfx::time::DateTimeOffset{ dateCreatedOpt.value() }
														: nfx::time::DateTimeOffset::utcNow();

			// Parse DataChannelListID (ConfigurationReference)
			auto configRefOpt = headerDoc.get<Document>( "/DataChannelListID" );
			if ( !configRefOpt.has_value() )
			{
				throw std::runtime_error( "Missing 'DataChannelListID' in Header" );
			}
			const auto& configRefDoc = configRefOpt.value();

			auto configIdOpt = configRefDoc.get<std::string>( "/ID" );
			if ( !configIdOpt.has_value() )
			{
				throw std::runtime_error( "Missing 'ID' in DataChannelListID" );
			}

			auto configTimestampOpt = configRefDoc.get<std::string>( "/TimeStamp" );
			nfx::time::DateTimeOffset configTimestamp = configTimestampOpt.has_value() ? nfx::time::DateTimeOffset{ configTimestampOpt.value() }
																					   : nfx::time::DateTimeOffset::utcNow();

			auto configVersionOpt = configRefDoc.get<std::string>( "/Version" );
			std::optional<std::string> configVersion = configVersionOpt.has_value()
														   ? std::make_optional( configVersionOpt.value() )
														   : std::nullopt;

			ConfigurationReference dataChannelListId( configIdOpt.value(), configTimestamp, configVersion );

			// Parse VersionInformation (optional)
			std::optional<VersionInformation> versionInfo;
			auto versionInfoOpt = headerDoc.get<Document>( "/VersionInformation" );
			if ( versionInfoOpt.has_value() )
			{
				const auto& versionDoc = versionInfoOpt.value();

				auto namingRuleOpt = versionDoc.get<std::string>( "/NamingRule" );
				auto schemeVersionOpt = versionDoc.get<std::string>( "/NamingSchemeVersion" );
				auto referenceUrlOpt = versionDoc.get<std::string>( "/ReferenceURL" );

				if ( namingRuleOpt.has_value() && schemeVersionOpt.has_value() )
				{
					std::optional<std::string> referenceUrl = referenceUrlOpt.has_value() ? std::make_optional( referenceUrlOpt.value() ) : std::nullopt;
					versionInfo = VersionInformation( namingRuleOpt.value(), schemeVersionOpt.value(), referenceUrl );
				}
			}

			// Parse CustomHeaders (optional)
			std::optional<nfx::containers::FastHashMap<std::string, Value>> customHeaders;
			auto customHeadersOpt = headerDoc.get<Document>( "/CustomHeaders" );
			if ( customHeadersOpt.has_value() )
			{
				nfx::containers::FastHashMap<std::string, Value> customHdrsMap;
				auto objectOpt = customHeadersOpt->get<Document::Object>( "" );
				if ( objectOpt )
				{
					for ( const auto& [key, valueDoc] : objectOpt.value() )
					{
						// Get type string
						auto typeOpt = valueDoc.get<std::string>( "/type" );
						if ( typeOpt.has_value() )
						{
							const std::string& typeStr = typeOpt.value();

							// Deserialize based on ISO 19848 compliant type information
							if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::DECIMAL )
							{
								auto strOpt = valueDoc.get<std::string>( "/value" );
								if ( strOpt.has_value() )
								{
									nfx::datatypes::Decimal decimal;
									if ( nfx::datatypes::Decimal::fromString( strOpt.value(), decimal ) )
									{
										customHdrsMap.insertOrAssign( key, Value::Decimal{ decimal } );
									}
								}
							}
							else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::INTEGER )
							{
								auto intOpt = valueDoc.get<int64_t>( "/value" );
								if ( intOpt.has_value() )
								{
									customHdrsMap.insertOrAssign( key, Value::Integer{ intOpt.value() } );
								}
							}
							else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::BOOLEAN )
							{
								auto boolOpt = valueDoc.get<bool>( "/value" );
								if ( boolOpt.has_value() )
								{
									customHdrsMap.insertOrAssign( key, Value::Boolean{ boolOpt.value() } );
								}
							}
							else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::STRING )
							{
								auto strOpt = valueDoc.get<std::string>( "/value" );
								if ( strOpt.has_value() )
								{
									customHdrsMap.insertOrAssign( key, Value::String{ strOpt.value() } );
								}
							}
							else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::DATETIME )
							{
								auto strOpt = valueDoc.get<std::string>( "/value" );
								if ( strOpt.has_value() )
								{
									try
									{
										nfx::time::DateTimeOffset dateTime{ strOpt.value() };
										customHdrsMap.insertOrAssign( key, Value::DateTime{ dateTime } );
									}
									catch ( ... )
									{
										// Failed to parse - skip this value
									}
								}
							}
							else
							{
								// Non-ISO 19848 type encountered during deserialization
								throw std::runtime_error( "Unsupported Value type '" + typeStr + "' - not compliant with ISO 19848 standard in Header CustomHeaders" );
							}
						}
					}
				}
				customHeaders = std::make_optional( customHdrsMap );
			}

			// Create Header
			Header header( author, dateCreated, shipId, dataChannelListId, versionInfo, customHeaders );

			// Parse DataChannelList
			auto dataChannelListOpt = packageDoc.get<Document>( "/DataChannelList" );
			if ( !dataChannelListOpt.has_value() )
			{
				throw std::runtime_error( "Missing 'DataChannelList' in Package" );
			}
			const auto& dataChannelListDoc = dataChannelListOpt.value();

			auto dataChannelArrayOpt = dataChannelListDoc.get<Document>( "/DataChannel" );
			if ( !dataChannelArrayOpt.has_value() )
			{
				throw std::runtime_error( "Missing 'DataChannel' array in DataChannelList" );
			}
			const auto& dataChannelArray = dataChannelArrayOpt.value();

			std::vector<DataChannel> dataChannels;
			auto dataChannelArrayItems = dataChannelArray.get<Document::Array>( "" );
			if ( dataChannelArrayItems.has_value() )
			{
				for ( const auto& dataChannelDoc : dataChannelArrayItems.value() )
				{
					// Parse DataChannelID
					LocalId localId;
					std::optional<std::string> shortId;
					std::optional<NameObject> nameObject;

					auto dataChannelIdOpt = dataChannelDoc.get<Document>( "/DataChannelID" );
					if ( !dataChannelIdOpt.has_value() )
					{
						throw std::runtime_error( "Missing 'DataChannelID' in DataChannel" );
					}
					const auto& channelIdDoc = dataChannelIdOpt.value();

					auto channelLocalIdOpt = channelIdDoc.get<std::string>( "/LocalID" );
					if ( !channelLocalIdOpt.has_value() )
					{
						throw std::runtime_error( "Missing 'LocalID' in DataChannelID" );
					}
					localId = LocalId::parse( channelLocalIdOpt.value() );

					auto channelShortIdOpt = channelIdDoc.get<std::string>( "/ShortID" );
					shortId = channelShortIdOpt.has_value() ? std::make_optional( channelShortIdOpt.value() ) : std::nullopt;

					auto channelNameObjectOpt = channelIdDoc.get<Document>( "/NameObject" );
					if ( channelNameObjectOpt.has_value() )
					{
						const auto& nameDoc = channelNameObjectOpt.value();
						auto namingRuleOpt = nameDoc.get<std::string>( "/NamingRule" );
						if ( namingRuleOpt.has_value() )
						{
							// Parse CustomNameObjects if present
							std::optional<nfx::containers::FastHashMap<std::string, Value>> customNameObjects;
							auto customDoc = nameDoc.get<Document>( "/CustomNameObjects" );
							if ( customDoc.has_value() )
							{
								nfx::containers::FastHashMap<std::string, Value> customMap;
								auto customObjItems = customDoc.value().get<Document::Object>( "" );
								if ( customObjItems.has_value() )
								{
									for ( const auto& [key, valueDoc] : customObjItems.value() )
									{
										// Get type string
										auto typeOpt = valueDoc.get<std::string>( "/type" );
										if ( typeOpt.has_value() )
										{
											const std::string& typeStr = typeOpt.value();

											// Deserialize based on ISO 19848 compliant type information
											if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::DECIMAL )
											{
												auto strOpt = valueDoc.get<std::string>( "/value" );
												if ( strOpt.has_value() )
												{
													nfx::datatypes::Decimal decimal;
													if ( nfx::datatypes::Decimal::fromString( strOpt.value(), decimal ) )
													{
														customMap.insertOrAssign( key, Value::Decimal{ decimal } );
													}
												}
											}
											else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::INTEGER )
											{
												auto intOpt = valueDoc.get<int64_t>( "/value" );
												if ( intOpt.has_value() )
												{
													customMap.insertOrAssign( key, Value::Integer{ intOpt.value() } );
												}
											}
											else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::BOOLEAN )
											{
												auto boolOpt = valueDoc.get<bool>( "/value" );
												if ( boolOpt.has_value() )
												{
													customMap.insertOrAssign( key, Value::Boolean{ boolOpt.value() } );
												}
											}

											else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::STRING )
											{
												auto strOpt = valueDoc.get<std::string>( "/value" );
												if ( strOpt.has_value() )
												{
													customMap.insertOrAssign( key, Value::String{ strOpt.value() } );
												}
											}
											else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::DATETIME )
											{
												auto strOpt = valueDoc.get<std::string>( "/value" );
												if ( strOpt.has_value() )
												{
													try
													{
														nfx::time::DateTimeOffset dateTime{ strOpt.value() };
														customMap.insertOrAssign( key, Value::DateTime{ dateTime } );
													}
													catch ( ... )
													{
														// Failed to parse - skip this value
													}
												}
											}
											else
											{
												// Non-ISO 19848 type encountered during deserialization
												throw std::runtime_error( "Unsupported Value type '" + typeStr + "' - not compliant with ISO 19848 standard" );
											}
										}
									}
								}
								customNameObjects = std::make_optional( customMap );
							}
							nameObject = NameObject( namingRuleOpt.value(), customNameObjects );
						}
					}

					DataChannelId dataChannelId( localId, shortId, nameObject );

					// Parse Property
					auto propertyOpt = dataChannelDoc.get<Document>( "/Property" );
					if ( !propertyOpt.has_value() )
					{
						throw std::runtime_error( "Missing 'Property' in DataChannel" );
					}
					const auto& propertyDoc = propertyOpt.value();

					// Parse DataChannelType
					auto dataChannelTypeOpt = propertyDoc.get<Document>( "/DataChannelType" );
					if ( !dataChannelTypeOpt.has_value() )
					{
						throw std::runtime_error( "Missing 'DataChannelType' in Property" );
					}
					const auto& dataChannelTypeDoc = dataChannelTypeOpt.value();

					auto typeOpt = dataChannelTypeDoc.get<std::string>( "/Type" );
					if ( !typeOpt.has_value() )
					{
						throw std::runtime_error( "Missing 'Type' in DataChannelType" );
					}

					auto updateCycleOpt = dataChannelTypeDoc.get<double>( "/UpdateCycle" );
					std::optional<double> updateCycle = updateCycleOpt.has_value() ? std::make_optional( updateCycleOpt.value() ) : std::nullopt;

					auto calculationPeriodOpt = dataChannelTypeDoc.get<double>( "/CalculationPeriod" );
					std::optional<double> calculationPeriod = calculationPeriodOpt.has_value() ? std::make_optional( calculationPeriodOpt.value() ) : std::nullopt;

					DataChannelType dataChannelType( typeOpt.value(), updateCycle, calculationPeriod );

					// Parse Format
					auto formatOpt = propertyDoc.get<Document>( "/Format" );
					if ( !formatOpt.has_value() )
					{
						throw std::runtime_error( "Missing 'Format' in Property" );
					}
					const auto& formatDoc = formatOpt.value();

					auto formatTypeOpt = formatDoc.get<std::string>( "/Type" );
					if ( !formatTypeOpt.has_value() )
					{
						throw std::runtime_error( "Missing 'Type' in Format" );
					}

					// Parse Restriction (complete implementation)
					std::optional<Restriction> restriction;
					auto restrictionOpt = formatDoc.get<Document>( "/Restriction" );
					if ( restrictionOpt.has_value() )
					{
						const auto& restrictionDoc = restrictionOpt.value();

						// Parse enumeration
						std::optional<std::vector<std::string>> enumeration;
						auto enumerationOpt = restrictionDoc.get<Document>( "/Enumeration" );
						if ( enumerationOpt.has_value() )
						{
							std::vector<std::string> enumValues;
							auto enumArrayItems = enumerationOpt.value().get<Document::Array>( "" );
							if ( enumArrayItems.has_value() )
							{
								for ( const auto& enumDoc : enumArrayItems.value() )
								{
									auto valueOpt = enumDoc.get<std::string>( "/" );
									if ( valueOpt.has_value() )
									{
										enumValues.push_back( valueOpt.value() );
									}
								}
							}
							if ( !enumValues.empty() )
							{
								enumeration = std::make_optional( enumValues );
							}
						}

						// Parse pattern
						auto patternOpt = restrictionDoc.get<std::string>( "/Pattern" );
						std::optional<std::string> pattern = patternOpt.has_value() ? std::make_optional( patternOpt.value() ) : std::nullopt;

						// Parse numeric bounds
						auto minInclusiveOpt = restrictionDoc.get<double>( "/MinInclusive" );
						std::optional<double> minInclusive = minInclusiveOpt.has_value() ? std::make_optional( minInclusiveOpt.value() ) : std::nullopt;

						auto maxInclusiveOpt = restrictionDoc.get<double>( "/MaxInclusive" );
						std::optional<double> maxInclusive = maxInclusiveOpt.has_value() ? std::make_optional( maxInclusiveOpt.value() ) : std::nullopt;

						auto minExclusiveOpt = restrictionDoc.get<double>( "/MinExclusive" );
						std::optional<double> minExclusive = minExclusiveOpt.has_value() ? std::make_optional( minExclusiveOpt.value() ) : std::nullopt;

						auto maxExclusiveOpt = restrictionDoc.get<double>( "/MaxExclusive" );
						std::optional<double> maxExclusive = maxExclusiveOpt.has_value() ? std::make_optional( maxExclusiveOpt.value() ) : std::nullopt;

						// Parse length constraints
						auto lengthOpt = restrictionDoc.get<uint32_t>( "/Length" );
						std::optional<uint32_t> length = lengthOpt.has_value() ? std::make_optional( lengthOpt.value() ) : std::nullopt;

						auto minLengthOpt = restrictionDoc.get<uint32_t>( "/MinLength" );
						std::optional<uint32_t> minLength = minLengthOpt.has_value() ? std::make_optional( minLengthOpt.value() ) : std::nullopt;

						auto maxLengthOpt = restrictionDoc.get<uint32_t>( "/MaxLength" );
						std::optional<uint32_t> maxLength = maxLengthOpt.has_value() ? std::make_optional( maxLengthOpt.value() ) : std::nullopt;

						// Parse digit constraints
						auto totalDigitsOpt = restrictionDoc.get<uint32_t>( "/TotalDigits" );
						std::optional<uint32_t> totalDigits = totalDigitsOpt.has_value() ? std::make_optional( totalDigitsOpt.value() ) : std::nullopt;

						auto fractionDigitsOpt = restrictionDoc.get<uint32_t>( "/FractionDigits" );
						std::optional<uint32_t> fractionDigits = fractionDigitsOpt.has_value() ? std::make_optional( fractionDigitsOpt.value() ) : std::nullopt;

						// Parse whitespace handling
						std::optional<dnv::vista::sdk::transport::WhiteSpace> whiteSpace;
						auto whiteSpaceOpt = restrictionDoc.get<std::string>( "/WhiteSpace" );
						if ( whiteSpaceOpt.has_value() )
						{
							const std::string& whiteSpaceStr = whiteSpaceOpt.value();
							if ( whiteSpaceStr == "Preserve" )
							{
								whiteSpace = dnv::vista::sdk::transport::WhiteSpace::Preserve;
							}
							else if ( whiteSpaceStr == "Replace" )
							{
								whiteSpace = dnv::vista::sdk::transport::WhiteSpace::Replace;
							}
							else if ( whiteSpaceStr == "Collapse" )
							{
								whiteSpace = dnv::vista::sdk::transport::WhiteSpace::Collapse;
							}
						}

						// Create the Restriction and set all parsed values using setters
						Restriction restrictionObj;

						if ( enumeration.has_value() )
						{
							restrictionObj.setEnumeration( enumeration );
						}
						if ( pattern.has_value() )
						{
							restrictionObj.setPattern( pattern );
						}
						if ( minInclusive.has_value() )
						{
							restrictionObj.setMinInclusive( minInclusive );
						}
						if ( maxInclusive.has_value() )
						{
							restrictionObj.setMaxInclusive( maxInclusive );
						}
						if ( minExclusive.has_value() )
						{
							restrictionObj.setMinExclusive( minExclusive );
						}
						if ( maxExclusive.has_value() )
						{
							restrictionObj.setMaxExclusive( maxExclusive );
						}
						if ( length.has_value() )
						{
							restrictionObj.setLength( length );
						}
						if ( minLength.has_value() )
						{
							restrictionObj.setMinLength( minLength );
						}
						if ( maxLength.has_value() )
						{
							restrictionObj.setMaxLength( maxLength );
						}
						if ( totalDigits.has_value() )
						{
							restrictionObj.setTotalDigits( totalDigits );
						}
						if ( fractionDigits.has_value() )
						{
							restrictionObj.setFractionDigits( fractionDigits );
						}
						if ( whiteSpace.has_value() )
						{
							restrictionObj.setWhiteSpace( whiteSpace );
						}

						restriction = std::make_optional( restrictionObj );
					}

					Format format( formatTypeOpt.value(), restriction );

					// Parse Range (optional)
					std::optional<Range> range;
					auto rangeOpt = propertyDoc.get<Document>( "/Range" );
					if ( rangeOpt.has_value() )
					{
						const auto& rangeDoc = rangeOpt.value();
						auto highOpt = rangeDoc.get<double>( "/High" );
						auto lowOpt = rangeDoc.get<double>( "/Low" );
						if ( highOpt.has_value() && lowOpt.has_value() )
						{
							range = Range( lowOpt.value(), highOpt.value() );
						}
					}

					// Parse Unit (optional)
					std::optional<Unit> unit;
					auto unitOpt = propertyDoc.get<Document>( "/Unit" );
					if ( unitOpt.has_value() )
					{
						const auto& unitDoc = unitOpt.value();
						auto unitSymbolOpt = unitDoc.get<std::string>( "/UnitSymbol" );
						if ( unitSymbolOpt.has_value() )
						{
							auto quantityNameOpt = unitDoc.get<std::string>( "/QuantityName" );
							std::optional<std::string> quantityName = quantityNameOpt.has_value() ? std::make_optional( quantityNameOpt.value() ) : std::nullopt;

							// Parse CustomElements (optional)
							std::optional<nfx::containers::FastHashMap<std::string, Value>> customElements;
							auto customElementsOpt = unitDoc.get<Document>( "/CustomElements" );
							if ( customElementsOpt.has_value() )
							{
								nfx::containers::FastHashMap<std::string, Value> customElemsMap;
								auto customElemsObjItems = customElementsOpt.value().get<Document::Object>( "" );
								if ( customElemsObjItems.has_value() )
								{
									for ( const auto& [key, valueDoc] : customElemsObjItems.value() )
									{
										// Get type string
										auto customElemTypeOpt = valueDoc.get<std::string>( "/type" );
										if ( customElemTypeOpt.has_value() )
										{
											const std::string& typeStr = customElemTypeOpt.value(); // Deserialize based on ISO 19848 compliant type information
											if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::DECIMAL )
											{
												auto strOpt = valueDoc.get<std::string>( "/value" );
												if ( strOpt.has_value() )
												{
													nfx::datatypes::Decimal decimal;
													if ( nfx::datatypes::Decimal::fromString( strOpt.value(), decimal ) )
													{
														customElemsMap.insertOrAssign( key, Value::Decimal{ decimal } );
													}
												}
											}
											else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::INTEGER )
											{
												auto intOpt = valueDoc.get<int64_t>( "/value" );
												if ( intOpt.has_value() )
												{
													customElemsMap.insertOrAssign( key, Value::Integer{ intOpt.value() } );
												}
											}
											else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::BOOLEAN )
											{
												auto boolOpt = valueDoc.get<bool>( "/value" );
												if ( boolOpt.has_value() )
												{
													customElemsMap.insertOrAssign( key, Value::Boolean{ boolOpt.value() } );
												}
											}
											else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::STRING )
											{
												auto strOpt = valueDoc.get<std::string>( "/value" );
												if ( strOpt.has_value() )
												{
													customElemsMap.insertOrAssign( key, Value::String{ strOpt.value() } );
												}
											}
											else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::DATETIME )
											{
												auto strOpt = valueDoc.get<std::string>( "/value" );
												if ( strOpt.has_value() )
												{
													try
													{
														nfx::time::DateTimeOffset dateTime{ strOpt.value() };
														customElemsMap.insertOrAssign( key, Value::DateTime{ dateTime } );
													}
													catch ( ... )
													{
														// Failed to parse - skip this value
													}
												}
											}
											else
											{
												// Non-ISO 19848 type encountered during deserialization
												throw std::runtime_error( "Unsupported Value type '" + typeStr + "' - not compliant with ISO 19848 standard in Unit CustomElements" );
											}
										}
									}
								}
								customElements = std::make_optional( customElemsMap );
							}

							unit = Unit( unitSymbolOpt.value(), quantityName, customElements );
						}
					}

					// Parse optional Property fields
					auto nameOpt = propertyDoc.get<std::string>( "/Name" );
					std::optional<std::string> name = nameOpt.has_value() ? std::make_optional( nameOpt.value() ) : std::nullopt;

					auto qualityCodingOpt = propertyDoc.get<std::string>( "/QualityCoding" );
					std::optional<std::string> qualityCoding = qualityCodingOpt.has_value() ? std::make_optional( qualityCodingOpt.value() ) : std::nullopt;

					auto alertPriorityOpt = propertyDoc.get<std::string>( "/AlertPriority" );
					std::optional<std::string> alertPriority = alertPriorityOpt.has_value() ? std::make_optional( alertPriorityOpt.value() ) : std::nullopt;

					auto remarksOpt = propertyDoc.get<std::string>( "/Remarks" );
					std::optional<std::string> remarks = remarksOpt.has_value() ? std::make_optional( remarksOpt.value() ) : std::nullopt;

					// Parse CustomProperties (optional)
					std::optional<nfx::containers::FastHashMap<std::string, Value>> customProperties;
					auto customPropertiesOpt = propertyDoc.get<Document>( "/CustomProperties" );
					if ( customPropertiesOpt.has_value() )
					{
						nfx::containers::FastHashMap<std::string, Value> customPropsMap;
						auto customPropsObjItems = customPropertiesOpt.value().get<Document::Object>( "" );
						if ( customPropsObjItems.has_value() )
						{
							for ( const auto& [key, valueDoc] : customPropsObjItems.value() )
							{
								// Get type string
								auto customPropTypeOpt = valueDoc.get<std::string>( "/type" );
								if ( customPropTypeOpt.has_value() )
								{
									const std::string& typeStr = customPropTypeOpt.value(); // Deserialize based on ISO 19848 compliant type information
									if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::DECIMAL )
									{
										auto strOpt = valueDoc.get<std::string>( "/value" );
										if ( strOpt.has_value() )
										{
											nfx::datatypes::Decimal decimal;
											if ( nfx::datatypes::Decimal::fromString( strOpt.value(), decimal ) )
											{
												customPropsMap.insertOrAssign( key, Value::Decimal{ decimal } );
											}
										}
									}
									else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::INTEGER )
									{
										auto intOpt = valueDoc.get<int64_t>( "/value" );
										if ( intOpt.has_value() )
										{
											customPropsMap.insertOrAssign( key, Value::Integer{ intOpt.value() } );
										}
									}
									else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::BOOLEAN )
									{
										auto boolOpt = valueDoc.get<bool>( "/value" );
										if ( boolOpt.has_value() )
										{
											customPropsMap.insertOrAssign( key, Value::Boolean{ boolOpt.value() } );
										}
									}
									else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::STRING )
									{
										auto strOpt = valueDoc.get<std::string>( "/value" );
										if ( strOpt.has_value() )
										{
											customPropsMap.insertOrAssign( key, Value::String{ strOpt.value() } );
										}
									}
									else if ( typeStr == dnv::vista::sdk::iso19848::formatdatatypes::DATETIME )
									{
										auto strOpt = valueDoc.get<std::string>( "/value" );
										if ( strOpt.has_value() )
										{
											try
											{
												nfx::time::DateTimeOffset dateTime{ strOpt.value() };
												customPropsMap.insertOrAssign( key, Value::DateTime{ dateTime } );
											}
											catch ( ... )
											{
												// Failed to parse - skip this value
											}
										}
									}
									else
									{
										// Non-ISO 19848 type encountered during deserialization
										throw std::runtime_error( "Unsupported Value type '" + typeStr + "' - not compliant with ISO 19848 standard" );
									}
								}
							}
						}
						customProperties = std::make_optional( customPropsMap );
					}

					// Create Property
					Property property(
						dataChannelType,
						format,
						range,
						unit,
						qualityCoding,
						alertPriority,
						name,
						remarks,
						customProperties );

					// Create DataChannel
					DataChannel dataChannel( dataChannelId, property );
					dataChannels.push_back( dataChannel );
				}
			}

			// Create the complete Package and DataChannelListPackage
			auto package = Package( header, dataChannels );
			obj = DataChannelListPackage( package );
		}
	};
} // namespace nfx::serialization::json
