/**
 * @file TimeSeriesDataSerializationTraits.h
 * @brief JSON serialization specialization for TimeSeriesData domain objects
 * @details Provides SerializationTraits specializations for Vista SDK TimeSeriesData
 */

#pragma once

#include <nfx/serialization/json/SerializationTraits.h>
#include <nfx/serialization/json/extensions/ContainersTraits.h>
#include <nfx/serialization/json/Document.h>

#include "dnv/vista/sdk/transport/ISO19848.h"
#include "dnv/vista/sdk/transport/ISO19848Constants.h"
#include "dnv/vista/sdk/transport/timeseries/TimeSeriesData.h"

namespace nfx::serialization::json
{
	//=====================================================================
	// TimeSeriesDataPackage serialization specialization
	//=====================================================================

	/**
	 * @brief SerializationTraits specialization for TimeSeriesDataPackage
	 * @details Converts Vista SDK TimeSeriesData domain objects to JSON-serializable format
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::transport::timeseries::TimeSeriesDataPackage>
	{
		/**
		 * @brief Serialize TimeSeriesDataPackage to JSON document
		 * @param obj The TimeSeriesDataPackage to serialize
		 * @param doc The JSON document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::transport::timeseries::TimeSeriesDataPackage& obj, Document& doc )
		{
			// Serialize Header if present
			if ( obj.package().header().has_value() )
			{
				const auto& header = obj.package().header().value();

				// Required fields
				doc.set( "/Package/Header/ShipID", header.shipId().toString() );

				// Optional fields
				if ( header.author().has_value() )
				{
					doc.set( "/Package/Header/Author", header.author().value() );
				}

				if ( header.dateCreated().has_value() )
				{
					doc.set( "/Package/Header/DateCreated", header.dateCreated().value().toIso8601Extended() );
				}

				if ( header.dateModified().has_value() )
				{
					doc.set( "/Package/Header/DateModified", header.dateModified().value().toIso8601Extended() );
				}

				// Serialize TimeSpan if present (C++ uses timeRange())
				if ( header.timeRange().has_value() )
				{
					const auto& timeRange = header.timeRange().value();
					doc.set( "/Package/Header/TimeSpan/Start", timeRange.start().toIso8601Extended() );
					doc.set( "/Package/Header/TimeSpan/End", timeRange.end().toIso8601Extended() );
				}

				// Serialize SystemConfiguration if present
				if ( header.systemConfiguration().has_value() )
				{
					const auto& systemConfigs = header.systemConfiguration().value();

					for ( size_t i = 0; i < systemConfigs.size(); ++i )
					{
						const auto& config = systemConfigs[i];
						std::string basePath = "/Package/Header/SystemConfiguration/" + std::to_string( i );

						doc.set( basePath + "/ID", config.id() );
						doc.set( basePath + "/TimeStamp", config.timeStamp().toIso8601Extended() );
					}
				}

				// Serialize CustomHeaders if present
				if ( header.customHeaders().has_value() )
				{
					const auto& customHdrs = header.customHeaders().value();

					for ( const auto& [key, value] : customHdrs )
					{
						std::string basePath = "/Package/Header/CustomHeaders/" + key;
						switch ( value.type() )
						{
							case dnv::vista::sdk::transport::Value::Type::Decimal:
							{
								doc.set( basePath + "/type", std::string{ "Decimal" } );
								doc.set( basePath + "/value", value.decimal().value().toString() );
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::Integer:
							{
								doc.set( basePath + "/type", std::string{ "Integer" } );
								doc.set( basePath + "/value", static_cast<int64_t>( value.integer().value() ) );
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::Boolean:
							{
								doc.set( basePath + "/type", std::string{ "Boolean" } );
								doc.set( basePath + "/value", value.boolean().value() );
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::String:
							{
								doc.set( basePath + "/type", std::string{ "String" } );
								doc.set( basePath + "/value", value.string().value() );
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::DateTime:
							{
								doc.set( basePath + "/type", std::string{ "DateTime" } );
								doc.set( basePath + "/value", value.dateTime().value().toIso8601Extended() );
								break;
							}
							default:
							{
								// Skip unknown Value types
								break;
							}
						}
					}
				}
			}

			// Serialize TimeSeriesData array
			const auto& timeSeriesData = obj.package().timeSeriesData();

			for ( size_t i = 0; i < timeSeriesData.size(); ++i )
			{
				const auto& tsd = timeSeriesData[i];
				std::string tsdBasePath = "/Package/TimeSeriesData/" + std::to_string( i );

				// Serialize DataConfiguration if present
				if ( tsd.dataConfiguration().has_value() )
				{
					const auto& config = tsd.dataConfiguration().value();

					doc.set( tsdBasePath + "/DataConfiguration/ID", config.id() );
					doc.set( tsdBasePath + "/DataConfiguration/TimeStamp", config.timeStamp().toIso8601Extended() );
				}

				// Serialize TabularData if present
				if ( tsd.tabularData().has_value() )
				{
					const auto& tabularDataList = tsd.tabularData().value();

					for ( size_t j = 0; j < tabularDataList.size(); ++j )
					{
						const auto& tabular = tabularDataList[j];
						std::string tabularBasePath = tsdBasePath + "/TabularData/" + std::to_string( j );

						// NumberOfDataChannel (computed from dataChannelIds size)
						doc.set( tabularBasePath + "/NumberOfDataChannel", static_cast<int64_t>( tabular.dataChannelIds().size() ) );

						// NumberOfDataSet (computed from dataSets size)
						doc.set( tabularBasePath + "/NumberOfDataSet", static_cast<int64_t>( tabular.dataSets().size() ) );

						// DataChannelID array
						if ( !tabular.dataChannelIds().empty() )
						{
							const auto& channelIds = tabular.dataChannelIds();

							for ( size_t k = 0; k < channelIds.size(); ++k )
							{
								doc.set( tabularBasePath + "/DataChannelID/" + std::to_string( k ), channelIds[k].toString() );
							}
						}

						// DataSet array
						if ( !tabular.dataSets().empty() )
						{
							const auto& dataSets = tabular.dataSets();

							for ( size_t k = 0; k < dataSets.size(); ++k )
							{
								const auto& dataSet = dataSets[k];
								std::string dataSetBasePath = tabularBasePath + "/DataSet/" + std::to_string( k );

								doc.set( dataSetBasePath + "/TimeStamp", dataSet.timeStamp().toIso8601Extended() );

								// Value array
								const auto& values = dataSet.value();
								for ( size_t l = 0; l < values.size(); ++l )
								{
									doc.set( dataSetBasePath + "/Value/" + std::to_string( l ), values[l] );
								}

								// Quality array (optional)
								if ( dataSet.quality().has_value() )
								{
									const auto& qualities = dataSet.quality().value();
									for ( size_t l = 0; l < qualities.size(); ++l )
									{
										doc.set( dataSetBasePath + "/Quality/" + std::to_string( l ), qualities[l] );
									}
								}
							}
						}
					}
				}

				// Serialize EventData if present
				if ( tsd.eventData().has_value() )
				{
					const auto& eventData = tsd.eventData().value();

					// NumberOfDataSet (computed from dataSet size)
					doc.set( tsdBasePath + "/EventData/NumberOfDataSet", static_cast<int64_t>( eventData.dataSet().size() ) );

					// DataSet array
					if ( !eventData.dataSet().empty() )
					{
						const auto& eventDataSets = eventData.dataSet();

						for ( size_t j = 0; j < eventDataSets.size(); ++j )
						{
							const auto& eventDataSet = eventDataSets[j];
							std::string eventDataSetBasePath = tsdBasePath + "/EventData/DataSet/" + std::to_string( j );

							doc.set( eventDataSetBasePath + "/TimeStamp", eventDataSet.timeStamp().toIso8601Extended() );
							doc.set( eventDataSetBasePath + "/DataChannelID", eventDataSet.dataChannelId().toString() );
							doc.set( eventDataSetBasePath + "/Value", eventDataSet.value() );

							if ( eventDataSet.quality().has_value() )
							{
								doc.set( eventDataSetBasePath + "/Quality", eventDataSet.quality().value() );
							}
						}
					}
				}

				// Serialize CustomDataKinds if present
				if ( tsd.customDataKinds().has_value() )
				{
					const auto& customData = tsd.customDataKinds().value();

					for ( const auto& [key, value] : customData )
					{
						std::string customDataBasePath = tsdBasePath + "/CustomData/" + key;
						switch ( value.type() )
						{
							case dnv::vista::sdk::transport::Value::Type::Decimal:
							{
								doc.set( customDataBasePath + "/type", std::string{ "Decimal" } );
								doc.set( customDataBasePath + "/value", value.decimal().value().toString() );
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::Integer:
							{
								doc.set( customDataBasePath + "/type", std::string{ "Integer" } );
								doc.set( customDataBasePath + "/value", value.integer().value() );
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::Boolean:
							{
								doc.set( customDataBasePath + "/type", std::string{ "Boolean" } );
								doc.set( customDataBasePath + "/value", value.boolean().value() );
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::String:
							{
								doc.set( customDataBasePath + "/type", std::string{ "String" } );
								doc.set( customDataBasePath + "/value", value.string().value() );
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::DateTime:
							{
								doc.set( customDataBasePath + "/type", std::string{ "DateTime" } );
								doc.set( customDataBasePath + "/value", value.dateTime().value().toIso8601Extended() );
								break;
							}
							default:
							{
								// Skip unknown Value types
								break;
							}
						}
					}
				}
			}
		}

		/**
		 * @brief Deserialize TimeSeriesDataPackage from JSON document
		 * @param obj The TimeSeriesDataPackage to deserialize into
		 * @param doc The JSON document to deserialize from
		 */
		static void deserialize( dnv::vista::sdk::transport::timeseries::TimeSeriesDataPackage& obj, const Document& doc )
		{
			using namespace dnv::vista::sdk;
			using namespace dnv::vista::sdk::transport;

			// Parse the root Package object
			auto packageOpt = doc.get<Document>( "/Package" );
			if ( !packageOpt.has_value() )
			{
				throw std::runtime_error( "Missing 'Package' root object in JSON" );
			}
			const auto& packageDoc = packageOpt.value();

			// Parse Header if present
			std::optional<timeseries::Header> header;
			auto headerOpt = packageDoc.get<Document>( "/Header" );
			if ( headerOpt.has_value() )
			{
				const auto& headerDoc = headerOpt.value();

				// Parse required ShipID
				auto shipIdOpt = headerDoc.get<std::string>( "/ShipID" );
				if ( !shipIdOpt.has_value() )
				{
					throw std::runtime_error( "Missing required 'ShipID' in Header" );
				}
				auto shipId = ShipId::parse( shipIdOpt.value() );

				// Parse optional TimeSpan (C++ uses timeRange())
				std::optional<timeseries::TimeRange> timeRange;
				auto timeSpanOpt = headerDoc.get<Document>( "/TimeSpan" );
				if ( timeSpanOpt.has_value() )
				{
					const auto& timeSpanDoc = timeSpanOpt.value();
					auto startOpt = timeSpanDoc.get<std::string>( "/Start" );
					auto endOpt = timeSpanDoc.get<std::string>( "/End" );
					if ( startOpt.has_value() && endOpt.has_value() )
					{
						auto start = nfx::time::DateTimeOffset{ startOpt.value() };
						auto end = nfx::time::DateTimeOffset{ endOpt.value() };
						timeRange = timeseries::TimeRange( start, end );
					}
				}

				// Parse optional dates
				std::optional<nfx::time::DateTimeOffset> dateCreated;
				auto dateCreatedOpt = headerDoc.get<std::string>( "/DateCreated" );
				if ( dateCreatedOpt.has_value() )
				{
					dateCreated = nfx::time::DateTimeOffset{ dateCreatedOpt.value() };
				}

				std::optional<nfx::time::DateTimeOffset> dateModified;
				auto dateModifiedOpt = headerDoc.get<std::string>( "/DateModified" );
				if ( dateModifiedOpt.has_value() )
				{
					dateModified = nfx::time::DateTimeOffset{ dateModifiedOpt.value() };
				}

				// Parse optional author
				std::optional<std::string> author;
				auto authorOpt = headerDoc.get<std::string>( "/Author" );
				if ( authorOpt.has_value() )
				{
					author = authorOpt.value();
				}

				// Parse optional SystemConfiguration array
				std::optional<std::vector<timeseries::ConfigurationReference>> systemConfiguration;
				auto systemConfigOpt = headerDoc.get<Document>( "/SystemConfiguration" );
				if ( systemConfigOpt.has_value() )
				{
					const auto& systemConfigArrayDoc = systemConfigOpt.value();
					std::vector<timeseries::ConfigurationReference> configs;

					auto arrayOpt = systemConfigArrayDoc.get<Document::Array>( "" );
					if ( arrayOpt )
					{
						for ( const auto& configDoc : arrayOpt.value() )
						{
							auto idOpt = configDoc.get<std::string>( "/ID" );
							auto timeStampOpt = configDoc.get<std::string>( "/TimeStamp" );
							if ( idOpt.has_value() && timeStampOpt.has_value() )
							{
								auto timeStamp = nfx::time::DateTimeOffset{ timeStampOpt.value() };
								configs.emplace_back( idOpt.value(), timeStamp );
							}
						}
					}

					systemConfiguration = std::move( configs );
				}

				// Parse optional CustomHeaders
				std::optional<nfx::containers::FastHashMap<std::string, Value>> customHeaders;
				auto customHeadersOpt = headerDoc.get<Document>( "/CustomHeaders" );
				if ( customHeadersOpt.has_value() )
				{
					nfx::containers::FastHashMap<std::string, Value> customHdrs;
					auto objectOpt = customHeadersOpt->get<Document::Object>( "" );

					if ( objectOpt )
					{
						for ( const auto& [key, valueDoc] : objectOpt.value() )
						{
							auto typeOpt = valueDoc.get<std::string>( "/type" );
							if ( typeOpt.has_value() )
							{
								const std::string& typeStr = typeOpt.value();

								if ( typeStr == "Decimal" )
								{
									auto strOpt = valueDoc.get<std::string>( "/value" );
									if ( strOpt.has_value() )
									{
										nfx::datatypes::Decimal decimal;
										if ( nfx::datatypes::Decimal::fromString( strOpt.value(), decimal ) )
										{
											customHdrs.insertOrAssign( key, Value::Decimal{ decimal } );
										}
									}
								}
								else if ( typeStr == "Integer" )
								{
									auto intOpt = valueDoc.get<int64_t>( "/value" );
									if ( intOpt.has_value() )
									{
										customHdrs.insertOrAssign( key, Value::Integer{ intOpt.value() } );
									}
								}
								else if ( typeStr == "Boolean" )
								{
									auto boolOpt = valueDoc.get<bool>( "/value" );
									if ( boolOpt.has_value() )
									{
										customHdrs.insertOrAssign( key, Value::Boolean{ boolOpt.value() } );
									}
								}
								else if ( typeStr == "String" )
								{
									auto stringOpt = valueDoc.get<std::string>( "/value" );
									if ( stringOpt.has_value() )
									{
										customHdrs.insertOrAssign( key, Value::String{ stringOpt.value() } );
									}
								}
								else if ( typeStr == "DateTime" )
								{
									auto dateTimeOpt = valueDoc.get<std::string>( "/value" );
									if ( dateTimeOpt.has_value() )
									{
										auto dateTime = nfx::time::DateTimeOffset{ dateTimeOpt.value() };
										customHdrs.insertOrAssign( key, Value::DateTime{ dateTime } );
									}
								}
							}
						}
					}

					customHeaders = std::move( customHdrs );
				}

				// Create Header with all parsed fields
				header = timeseries::Header( shipId, timeRange, dateCreated, dateModified, author, systemConfiguration, customHeaders );
			}

			// Parse TimeSeriesData array
			std::vector<timeseries::TimeSeriesData> timeSeriesData;
			auto timeSeriesOpt = packageDoc.get<Document>( "/TimeSeriesData" );
			if ( timeSeriesOpt.has_value() )
			{
				const auto& timeSeriesArrayDoc = timeSeriesOpt.value();

				auto timeSeriesArrayOpt = timeSeriesArrayDoc.get<Document::Array>( "" );
				if ( timeSeriesArrayOpt )
				{
					for ( const auto& tsdDoc : timeSeriesArrayOpt.value() )
					{
						// Parse optional DataConfiguration
						std::optional<timeseries::ConfigurationReference> dataConfiguration;
						auto dataConfigOpt = tsdDoc.get<Document>( "/DataConfiguration" );
						if ( dataConfigOpt.has_value() )
						{
							const auto& dataConfigDoc = dataConfigOpt.value();
							auto idOpt = dataConfigDoc.get<std::string>( "/ID" );
							auto timeStampOpt = dataConfigDoc.get<std::string>( "/TimeStamp" );
							if ( idOpt.has_value() && timeStampOpt.has_value() )
							{
								auto timeStamp = nfx::time::DateTimeOffset{ timeStampOpt.value() };
								dataConfiguration = timeseries::ConfigurationReference( idOpt.value(), timeStamp );
							}
						}

						// Parse optional TabularData array
						std::optional<std::vector<timeseries::TabularData>> tabularData;
						auto tabularOpt = tsdDoc.get<Document>( "/TabularData" );
						if ( tabularOpt.has_value() )
						{
							const auto& tabularArrayDoc = tabularOpt.value();
							std::vector<timeseries::TabularData> tabularDataList;

							auto tabularArrayOpt = tabularArrayDoc.get<Document::Array>( "" );
							if ( tabularArrayOpt )
							{
								for ( const auto& tabularDoc : tabularArrayOpt.value() )
								{
									// Parse DataChannelID array
									std::vector<timeseries::DataChannelId> dataChannelIds;
									auto channelIdsOpt = tabularDoc.get<Document>( "/DataChannelID" );
									if ( channelIdsOpt.has_value() )
									{
										auto channelIdsArrayOpt = channelIdsOpt->get<Document::Array>( "" );
										if ( channelIdsArrayOpt )
										{
											for ( const auto& channelIdDoc : channelIdsArrayOpt.value() )
											{
												auto channelIdOpt = channelIdDoc.get<std::string>( "" );
												if ( channelIdOpt.has_value() )
												{
													dataChannelIds.push_back( timeseries::DataChannelId::parse( channelIdOpt.value() ) );
												}
											}
										}
									}

									// Parse DataSet array
									std::vector<timeseries::TabularDataSet> dataSets;
									auto dataSetsOpt = tabularDoc.get<Document>( "/DataSet" );
									if ( dataSetsOpt.has_value() )
									{
										auto dataSetsArrayOpt = dataSetsOpt->get<Document::Array>( "" );
										if ( dataSetsArrayOpt )
										{
											for ( const auto& dataSetDoc : dataSetsArrayOpt.value() )
											{
												// Parse TimeStamp
												auto timeStampOpt = dataSetDoc.get<std::string>( "/TimeStamp" );
												if ( timeStampOpt.has_value() )
												{
													auto timeStamp = nfx::time::DateTimeOffset{ timeStampOpt.value() };

													// Parse Value array
													std::vector<std::string> values;
													auto valueOpt = dataSetDoc.get<Document>( "/Value" );
													if ( valueOpt.has_value() )
													{
														auto valueArrayOpt = valueOpt->get<Document::Array>( "" );
														if ( valueArrayOpt )
														{
															for ( const auto& valueDoc : valueArrayOpt.value() )
															{
																auto valueStrOpt = valueDoc.get<std::string>( "" );
																if ( valueStrOpt.has_value() )
																{
																	values.push_back( valueStrOpt.value() );
																}
															}
														}
													}

													// Parse optional Quality array
													std::optional<std::vector<std::string>> quality;
													auto qualityOpt = dataSetDoc.get<Document>( "/Quality" );
													if ( qualityOpt.has_value() )
													{
														std::vector<std::string> qualities;
														auto qualityArrayOpt = qualityOpt->get<Document::Array>( "" );
														if ( qualityArrayOpt )
														{
															for ( const auto& qualityDoc : qualityArrayOpt.value() )
															{
																auto qualityStrOpt = qualityDoc.get<std::string>( "" );
																if ( qualityStrOpt.has_value() )
																{
																	qualities.push_back( qualityStrOpt.value() );
																}
															}
														}
														quality = std::move( qualities );
													}

													dataSets.emplace_back( timeStamp, std::move( values ), quality );
												}
											}
										}
									}

									tabularDataList.emplace_back( std::move( dataChannelIds ), std::move( dataSets ) );
								}
							}

							tabularData = std::move( tabularDataList );
						}

						// Parse optional EventData
						std::optional<timeseries::EventData> eventData;
						auto eventDataOpt = tsdDoc.get<Document>( "/EventData" );
						if ( eventDataOpt.has_value() )
						{
							const auto& eventDataDoc = eventDataOpt.value();
							std::vector<timeseries::EventDataSet> eventDataSets;

							auto eventDataSetsOpt = eventDataDoc.get<Document>( "/DataSet" );
							if ( eventDataSetsOpt.has_value() )
							{
								auto eventDataSetsArrayOpt = eventDataSetsOpt->get<Document::Array>( "" );
								if ( eventDataSetsArrayOpt )
								{
									for ( const auto& eventDataSetDoc : eventDataSetsArrayOpt.value() )
									{
										// Parse required fields
										auto timeStampOpt = eventDataSetDoc.get<std::string>( "/TimeStamp" );
										auto dataChannelIdOpt = eventDataSetDoc.get<std::string>( "/DataChannelID" );
										auto valueOpt = eventDataSetDoc.get<std::string>( "/Value" );

										if ( timeStampOpt.has_value() && dataChannelIdOpt.has_value() && valueOpt.has_value() )
										{
											auto timeStamp = nfx::time::DateTimeOffset{ timeStampOpt.value() };
											auto dataChannelId = timeseries::DataChannelId::parse( dataChannelIdOpt.value() );
											auto value = valueOpt.value();

											// Parse optional Quality
											std::optional<std::string> quality;
											auto qualityOpt = eventDataSetDoc.get<std::string>( "/Quality" );
											if ( qualityOpt.has_value() )
											{
												quality = qualityOpt.value();
											}

											eventDataSets.emplace_back( timeStamp, dataChannelId, value, quality );
										}
									}
								}
							}

							eventData = timeseries::EventData( std::move( eventDataSets ) );
						}

						// Parse optional CustomDataKinds (serialized as CustomData)
						std::optional<nfx::containers::FastHashMap<std::string, Value>> customDataKinds;
						auto customDataOpt = tsdDoc.get<Document>( "/CustomData" );
						if ( customDataOpt.has_value() )
						{
							nfx::containers::FastHashMap<std::string, Value> customData;
							auto customDataObjectOpt = customDataOpt->get<Document::Object>( "" );

							if ( customDataObjectOpt )
							{
								for ( const auto& [key, valueDoc] : customDataObjectOpt.value() )
								{
									auto typeOpt = valueDoc.get<std::string>( "/type" );
									if ( typeOpt.has_value() )
									{
										const std::string& typeStr = typeOpt.value();

										if ( typeStr == "Decimal" )
										{
											auto strOpt = valueDoc.get<std::string>( "/value" );
											if ( strOpt.has_value() )
											{
												nfx::datatypes::Decimal decimal;
												if ( nfx::datatypes::Decimal::fromString( strOpt.value(), decimal ) )
												{
													customData.insertOrAssign( key, Value::Decimal{ decimal } );
												}
											}
										}
										else if ( typeStr == "Integer" )
										{
											auto intOpt = valueDoc.get<int64_t>( "/value" );
											if ( intOpt.has_value() )
											{
												customData.insertOrAssign( key, Value::Integer{ intOpt.value() } );
											}
										}
										else if ( typeStr == "Boolean" )
										{
											auto boolOpt = valueDoc.get<bool>( "/value" );
											if ( boolOpt.has_value() )
											{
												customData.insertOrAssign( key, Value::Boolean{ boolOpt.value() } );
											}
										}
										else if ( typeStr == "String" )
										{
											auto stringOpt = valueDoc.get<std::string>( "/value" );
											if ( stringOpt.has_value() )
											{
												customData.insertOrAssign( key, Value::String{ stringOpt.value() } );
											}
										}
										else if ( typeStr == "DateTime" )
										{
											auto dateTimeOpt = valueDoc.get<std::string>( "/value" );
											if ( dateTimeOpt.has_value() )
											{
												auto dateTime = nfx::time::DateTimeOffset{ dateTimeOpt.value() };
												customData.insertOrAssign( key, Value::DateTime{ dateTime } );
											}
										}
									}
								}
							}

							customDataKinds = std::move( customData );
						}

						// Create TimeSeriesData with all parsed components
						timeSeriesData.emplace_back( dataConfiguration, tabularData, eventData, customDataKinds );
					}
				}
			}

			// Create the complete Package and TimeSeriesDataPackage
			auto package = timeseries::Package( header, std::move( timeSeriesData ) );
			obj = timeseries::TimeSeriesDataPackage( std::move( package ) );
		}
	};
} // namespace nfx::serialization::json
