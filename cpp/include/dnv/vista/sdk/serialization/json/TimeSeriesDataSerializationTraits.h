/**
 * @file TimeSeriesDataSerializationTraits.h
 * @brief JSON serialization specialization for TimeSeriesData domain objects
 * @details Provides SerializationTraits specializations for Vista SDK TimeSeriesData
 */

#pragma once

#include <nfx/serialization/json/SerializationTraits.h>
#include <nfx/serialization/json/extensions/ContainersTraits.h>
#include <nfx/serialization/json/extensions/DatatypesTraits.h>
#include <nfx/serialization/json/extensions/DateTimeTraits.h>

#include "dnv/vista/sdk/transport/ISO19848.h"

#include "dnv/vista/sdk/transport/timeseries/TimeSeriesData.h"
#include "ISO19848Validation.h"

namespace nfx::serialization::json
{
	using nfx::time::DateTime;

	/**
	 * @brief SerializationTraits specialization for TimeSeriesDataPackage
	 * @details Converts Vista SDK TimeSeriesData domain objects directly to/from JSON
	 */
	template <>
	struct SerializationTraits<dnv::vista::sdk::transport::timeseries::TimeSeriesDataPackage>
	{
	public:
		/**
		 * @brief Serialize TimeSeriesDataPackage to JSON document
		 * @param obj The TimeSeriesDataPackage to serialize
		 * @param doc The JSON document to serialize into
		 */
		static void serialize( const dnv::vista::sdk::transport::timeseries::TimeSeriesDataPackage& obj, Document& doc )
		{
			using namespace dnv::vista::sdk::transport::timeseries;
			using Value = dnv::vista::sdk::transport::Value;

			const auto& package = obj.package();

			// Serialize Header
			if ( package.header().has_value() )
			{
				const auto& header = *package.header();
				std::string headerPath = "/Package/Header";

				// ShipId
				doc.set( headerPath + "/ShipID", header.shipId().toString() );

				// TimeSpan (optional)
				if ( auto timeRangeOpt = header.timeRange() )
				{
					const auto& timeRange = *timeRangeOpt;
					doc.set( headerPath + "/TimeSpan/Start", timeRange.start().toString( DateTime::Format::Iso8601Precise ) );
					doc.set( headerPath + "/TimeSpan/End", timeRange.end().toString( DateTime::Format::Iso8601Precise ) );
				}

				// DateCreated (optional)
				if ( auto dateCreated = header.dateCreated() )
				{
					doc.set( headerPath + "/DateCreated", dateCreated->toString( DateTime::Format::Iso8601Precise ) );
				}

				// DateModified (optional)
				if ( auto dateModified = header.dateModified() )
				{
					doc.set( headerPath + "/DateModified", dateModified->toString( DateTime::Format::Iso8601Precise ) );
				}

				// Author (optional)
				if ( auto author = header.author() )
				{
					doc.set<std::string>( headerPath + "/Author", author.value() );
				}

				// SystemConfiguration (optional array)
				if ( auto sysConfigsOpt = header.systemConfiguration() )
				{
					const auto& sysConfigs = *sysConfigsOpt;
					for ( std::size_t i = 0; i < sysConfigs.size(); ++i )
					{
						const auto& config = sysConfigs[i];
						std::string configPath = headerPath + "/SystemConfiguration/" + std::to_string( i );
						doc.set( configPath + "/ID", config.id() );
						doc.set( configPath + "/TimeStamp", config.timeStamp().toString( DateTime::Format::Iso8601Precise ) );
					}
				}

				// CustomHeaders (JsonExtensionData - flattened at Header level)
				if ( auto customHeadersOpt = header.customHeaders() )
				{
					const auto& customHdrs = *customHeadersOpt;
					if ( auto customHdrsObj = customHdrs.get<Document::Object>( "" ) )
					{
						for ( const auto& [key, valueDoc] : customHdrsObj.value() )
						{
							doc.set( headerPath + "/" + key, valueDoc );
						}
					}
				}
			}

			// Serialize TimeSeriesData array
			const auto& timeSeriesData = package.timeSeriesData();
			for ( std::size_t tsIdx = 0; tsIdx < timeSeriesData.size(); ++tsIdx )
			{
				const auto& tsData = timeSeriesData[tsIdx];
				std::string tsPath = "/Package/TimeSeriesData/" + std::to_string( tsIdx );

				// DataConfiguration (optional)
				if ( tsData.dataConfiguration().has_value() )
				{
					const auto& dataConfig = *tsData.dataConfiguration();
					doc.set( tsPath + "/DataConfiguration/ID", dataConfig.id() );
					doc.set( tsPath + "/DataConfiguration/TimeStamp", dataConfig.timeStamp().toString( DateTime::Format::Iso8601Precise ) );
				}

				// TabularData (optional array)
				if ( auto tabularDataListOpt = tsData.tabularData() )
				{
					const auto& tabularDataList = *tabularDataListOpt;
					for ( std::size_t tabIdx = 0; tabIdx < tabularDataList.size(); ++tabIdx )
					{
						const auto& tabularData = tabularDataList[tabIdx];
						std::string tabPath = tsPath + "/TabularData/" + std::to_string( tabIdx );

						// NumberOfDataSet
						const auto& dataSets = tabularData.dataSets();
						doc.set( tabPath + "/NumberOfDataSet", static_cast<int>( dataSets.size() ) );

						// NumberOfDataChannel
						const auto& channelIds = tabularData.dataChannelIds();
						doc.set( tabPath + "/NumberOfDataChannel", static_cast<int>( channelIds.size() ) );

						// DataChannelID
						for ( std::size_t cidIdx = 0; cidIdx < channelIds.size(); ++cidIdx )
						{
							doc.set( tabPath + "/DataChannelID/" + std::to_string( cidIdx ), channelIds[cidIdx].toString() );
						}

						// DataSet
						for ( std::size_t dsIdx = 0; dsIdx < dataSets.size(); ++dsIdx )
						{
							const auto& dataSet = dataSets[dsIdx];
							std::string dsPath = tabPath + "/DataSet/" + std::to_string( dsIdx );

							doc.set( dsPath + "/TimeStamp", dataSet.timeStamp().toString( DateTime::Format::Iso8601Precise ) );

							// Values
							const auto& values = dataSet.value();
							for ( std::size_t vIdx = 0; vIdx < values.size(); ++vIdx )
							{
								doc.set( dsPath + "/Value/" + std::to_string( vIdx ), values[vIdx] );
							}

							// Quality (optional)
							if ( dataSet.quality().has_value() )
							{
								const auto& qualities = *dataSet.quality();
								for ( std::size_t qIdx = 0; qIdx < qualities.size(); ++qIdx )
								{
									doc.set( dsPath + "/Quality/" + std::to_string( qIdx ), qualities[qIdx] );
								}
							}
						}
					}
				}

				// EventData (optional)
				if ( tsData.eventData().has_value() )
				{
					const auto& eventData = *tsData.eventData();
					if ( eventData.dataSet().has_value() )
					{
						const auto& eventDataSets = *eventData.dataSet();
						// NumberOfDataSet
						doc.set( tsPath + "/EventData/NumberOfDataSet", static_cast<int>( eventDataSets.size() ) );
						for ( std::size_t edIdx = 0; edIdx < eventDataSets.size(); ++edIdx )
						{
							const auto& eventDataSet = eventDataSets[edIdx];
							std::string edPath = tsPath + "/EventData/DataSet/" + std::to_string( edIdx );

							doc.set( edPath + "/TimeStamp", eventDataSet.timeStamp().toString( DateTime::Format::Iso8601Precise ) );
							doc.set( edPath + "/DataChannelID", eventDataSet.dataChannelId().toString() );
							doc.set( edPath + "/Value", eventDataSet.value() );

							// Quality (optional)
							if ( eventDataSet.quality().has_value() )
							{
								doc.set( edPath + "/Quality", *eventDataSet.quality() );
							}
						}
					}
				}

				// CustomData (JsonExtensionData - flattened at TimeSeriesData level)
				if ( auto customDataKindsOpt = tsData.customDataKinds() )
				{
					const auto& customDataKinds = *customDataKindsOpt;
					if ( auto customDataKindsObj = customDataKinds.get<Document::Object>( "" ) )
					{
						for ( const auto& [key, valueDoc] : customDataKindsObj.value() )
						{
							doc.set( tsPath + "/" + key, valueDoc );
						}
					}
				}
			}
		}

		/**
		 * @brief Deserialize TimeSeriesDataPackage from JSON document
		 * @param doc The JSON document to deserialize from
		 * @param obj The TimeSeriesDataPackage to populate
		 */
		static void deserialize( const Document& doc, dnv::vista::sdk::transport::timeseries::TimeSeriesDataPackage& obj )
		{
			using namespace dnv::vista::sdk::transport::timeseries;
			using namespace dnv::vista::sdk;

			// Deserialize Header
			std::optional<dnv::vista::sdk::transport::timeseries::Header> header;
			std::string headerPath = "/Package/Header";

			if ( doc.contains( headerPath ) )
			{
				// ShipId (required)
				auto shipIdStr = doc.get<std::string>( headerPath + "/ShipID" );
				if ( !shipIdStr.has_value() )
				{
					throw std::runtime_error{ "Missing required ShipID in TimeSeriesData Header" };
				}

				auto shipIdOpt = ShipId::fromString( *shipIdStr );
				if ( !shipIdOpt.has_value() )
				{
					throw std::runtime_error{ "Invalid ShipID format: " + *shipIdStr };
				}

				// TimeSpan (optional - called TimeRange in domain model)
				std::optional<TimeRange> timeRange;
				if ( doc.contains( headerPath + "/TimeSpan" ) )
				{
					auto startStr = doc.get<DateTimeOffset>( headerPath + "/TimeSpan/Start" );
					auto endStr = doc.get<DateTimeOffset>( headerPath + "/TimeSpan/End" );
					if ( startStr.has_value() && endStr.has_value() )
					{
						auto start = DateTimeOffset{ *startStr };
						auto end = DateTimeOffset{ *endStr };
						timeRange = TimeRange{ start, end };
					}
				}

				// DateCreated (optional)
				std::optional<DateTimeOffset> dateCreated;
				if ( auto dcStr = doc.get<DateTimeOffset>( headerPath + "/DateCreated" ) )
				{
					dateCreated = DateTimeOffset{ *dcStr };
				}

				// DateModified (optional)
				std::optional<DateTimeOffset> dateModified;
				if ( auto dmStr = doc.get<DateTimeOffset>( headerPath + "/DateModified" ) )
				{
					dateModified = DateTimeOffset{ *dmStr };
				}

				// Author (optional)
				std::optional<std::string> author;
				if ( auto authorStr = doc.get<std::string>( headerPath + "/Author" ) )
				{
					author = *authorStr;
				}

				// SystemConfiguration (optional array)
				std::optional<std::vector<ConfigurationReference>> systemConfiguration;
				if ( doc.contains( headerPath + "/SystemConfiguration" ) )
				{
					std::vector<ConfigurationReference> sysConfigs;
					std::size_t idx = 0;
					while ( true )
					{
						std::string configPath = headerPath + "/SystemConfiguration/" + std::to_string( idx );
						if ( !doc.contains( configPath ) )
							break;

						auto idOpt = doc.get<std::string>( configPath + "/ID" );
						auto tsOpt = doc.get<std::string>( configPath + "/TimeStamp" );

						if ( idOpt.has_value() && tsOpt.has_value() )
						{
							if ( !isValidIso19848DateTimeOffset( *tsOpt ) )
							{
								throw std::runtime_error{ "Invalid ISO 19848 format for SystemConfiguration/TimeStamp: '" + *tsOpt + "'" };
							}
							auto timestamp = DateTimeOffset{ *tsOpt };
							sysConfigs.emplace_back( ConfigurationReference{ *idOpt, timestamp } );
						}
						++idx;
					}
					if ( !sysConfigs.empty() )
					{
						systemConfiguration = std::move( sysConfigs );
					}
				}

				// CustomHeaders (Optional)
				std::optional<CustomHeaders> customHeaders;
				if ( auto headerObjOpt = doc.get<Document::Object>( headerPath ) )
				{
					// Known property names in Header
					const std::unordered_set<std::string> knownKeys = {
						"ShipID", "TimeSpan", "DateCreated", "DateModified", "Author", "SystemConfiguration" };

					Document customHdrs;
					bool hasCustomHeaders = false;
					for ( const auto& [key, valueDoc] : headerObjOpt.value() )
					{
						// Skip known properties
						if ( knownKeys.find( key ) != knownKeys.end() )
						{
							continue;
						}

						// Add custom header (Document can hold arbitrary JSON)
						customHdrs.set( key, valueDoc );
						hasCustomHeaders = true;
					}
					if ( hasCustomHeaders )
					{
						customHeaders = std::move( customHdrs );
					}
				}

				header = Header{
					*shipIdOpt,
					timeRange,
					dateCreated,
					dateModified,
					author,
					systemConfiguration,
					customHeaders };
			}

			// Deserialize TimeSeriesData array
			std::vector<TimeSeriesData> timeSeriesDataList;
			std::size_t tsIdx = 0;

			while ( true )
			{
				std::string tsPath = "/Package/TimeSeriesData/" + std::to_string( tsIdx );
				if ( !doc.contains( tsPath ) )
					break;

				// DataConfiguration (optional)
				std::optional<ConfigurationReference> dataConfiguration;
				if ( doc.contains( tsPath + "/DataConfiguration" ) )
				{
					auto idOpt = doc.get<std::string>( tsPath + "/DataConfiguration/ID" );
					auto tsOpt = doc.get<std::string>( tsPath + "/DataConfiguration/TimeStamp" );
					if ( idOpt.has_value() && tsOpt.has_value() )
					{
						if ( !isValidIso19848DateTimeOffset( *tsOpt ) )
						{
							throw std::runtime_error{ "Invalid ISO 19848 format for SystemConfiguration/TimeStamp: '" + *tsOpt + "'" };
						}
						auto timestamp = DateTimeOffset{ *tsOpt };
						dataConfiguration = ConfigurationReference{ *idOpt, timestamp };
					}
				}

				// TabularData (optional array)
				std::optional<std::vector<TabularData>> tabularDataList;
				if ( doc.contains( tsPath + "/TabularData" ) )
				{
					std::vector<TabularData> tabList;
					std::size_t tabIdx = 0;

					while ( true )
					{
						std::string tabPath = tsPath + "/TabularData/" + std::to_string( tabIdx );
						if ( !doc.contains( tabPath ) )
							break;

						// DataChannelIDs
						std::vector<DataChannelId> channelIds;
						std::size_t cidIdx = 0;
						while ( true )
						{
							auto cidStr = doc.get<std::string>( tabPath + "/DataChannelID/" + std::to_string( cidIdx ) );
							if ( !cidStr.has_value() )
								break;

							auto cidOpt = DataChannelId::fromString( *cidStr );
							if ( cidOpt.has_value() )
							{
								channelIds.push_back( *cidOpt );
							}
							++cidIdx;
						}

						// Validate NumberOfDataChannel if present
						if ( auto numberOfDataChannelOpt = doc.get<int>( tabPath + "/NumberOfDataChannel" ) )
						{
							if ( *numberOfDataChannelOpt != static_cast<int>( channelIds.size() ) )
							{
								throw std::runtime_error{ "Number of data channels does not match the expected count" };
							}
						}

						// DataSets
						std::vector<TabularDataSet> dataSets;
						std::size_t dsIdx = 0;
						while ( true )
						{
							std::string dsPath = tabPath + "/DataSet/" + std::to_string( dsIdx );
							if ( !doc.contains( dsPath ) )
								break;

							auto timestampStr = doc.get<std::string>( dsPath + "/TimeStamp" );
							if ( !timestampStr.has_value() )
								break;

							auto timestamp = DateTimeOffset{ *timestampStr };

							// Values
							std::vector<std::string> values;
							std::size_t vIdx = 0;
							while ( true )
							{
								auto valueOpt = doc.get<std::string>( dsPath + "/Value/" + std::to_string( vIdx ) );
								if ( !valueOpt.has_value() )
									break;
								values.push_back( *valueOpt );
								++vIdx;
							}

							// Quality (optional)
							std::optional<std::vector<std::string>> quality;
							if ( doc.contains( dsPath + "/Quality" ) )
							{
								std::vector<std::string> qualities;
								std::size_t qIdx = 0;
								while ( true )
								{
									auto qualityOpt = doc.get<std::string>( dsPath + "/Quality/" + std::to_string( qIdx ) );
									if ( !qualityOpt.has_value() )
										break;
									qualities.push_back( *qualityOpt );
									++qIdx;
								}
								if ( !qualities.empty() )
								{
									quality = std::move( qualities );
								}
							}

							dataSets.emplace_back( TabularDataSet{ timestamp, values, quality } );
							++dsIdx;
						}

						// Validate NumberOfDataSet if present
						if ( auto numberOfDataSetOpt = doc.get<int>( tabPath + "/NumberOfDataSet" ) )
						{
							if ( *numberOfDataSetOpt != static_cast<int>( dataSets.size() ) )
							{
								throw std::runtime_error{ "Number of data sets does not match the expected count" };
							}
						}

						if ( !channelIds.empty() && !dataSets.empty() )
						{
							tabList.emplace_back( TabularData{ channelIds, dataSets } );
						}
						++tabIdx;
					}

					if ( !tabList.empty() )
					{
						tabularDataList = std::move( tabList );
					}
				}

				// EventData (optional)
				std::optional<EventData> eventData;
				if ( doc.contains( tsPath + "/EventData/DataSet" ) )
				{
					std::vector<EventDataSet> eventDataSets;
					std::size_t edIdx = 0;

					while ( true )
					{
						std::string edPath = tsPath + "/EventData/DataSet/" + std::to_string( edIdx );
						if ( !doc.contains( edPath ) )
							break;

						auto timestampStr = doc.get<std::string>( edPath + "/TimeStamp" );
						auto dataChannelIdStr = doc.get<std::string>( edPath + "/DataChannelID" );
						auto valueStr = doc.get<std::string>( edPath + "/Value" );

						if ( timestampStr.has_value() && dataChannelIdStr.has_value() && valueStr.has_value() )
						{
							auto timestamp = DateTimeOffset{ *timestampStr };
							auto dataChannelIdOpt = DataChannelId::fromString( *dataChannelIdStr );

							if ( dataChannelIdOpt.has_value() )
							{
								std::optional<std::string> quality;
								if ( auto qualityOpt = doc.get<std::string>( edPath + "/Quality" ) )
								{
									quality = *qualityOpt;
								}

								eventDataSets.emplace_back( EventDataSet{
									timestamp,
									*dataChannelIdOpt,
									*valueStr,
									quality } );
							}
						}
						++edIdx;
					}

					if ( !eventDataSets.empty() )
					{
						eventData = EventData{ eventDataSets };
					}
				}

				// CustomDataKinds (Optional)
				std::optional<CustomData> customDataKinds;
				if ( auto tsObjOpt = doc.get<Document::Object>( tsPath ) )
				{
					// Known property names in TimeSeriesData
					const std::unordered_set<std::string> knownKeys = {
						"DataConfiguration", "TabularData", "EventData" };

					CustomData customData;
					bool hasCustomData = false;
					for ( const auto& [key, valueDoc] : tsObjOpt.value() )
					{
						// Skip known properties
						if ( knownKeys.find( key ) != knownKeys.end() )
						{
							continue;
						}

						// Add custom property (Document can hold arbitrary JSON)
						customData.set( key, valueDoc );
						hasCustomData = true;
					}
					if ( hasCustomData )
					{
						customDataKinds = std::move( customData );
					}
				}
				// Create TimeSeriesData
				timeSeriesDataList.emplace_back( TimeSeriesData{
					dataConfiguration,
					tabularDataList,
					eventData,
					customDataKinds } );

				++tsIdx;
			}

			// Create Package
			Package package{ header, std::move( timeSeriesDataList ) };

			// Assign to obj
			obj = TimeSeriesDataPackage{ std::move( package ) };
		}
	};
} // namespace nfx::serialization::json
