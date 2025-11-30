/**
 * @file TESTS_Serialization_JSON_TimeSeriesData.cpp
 * @brief TimeSeriesData JSON serialization and deserialization tests
 * @details Validates that serialization and deserialization operations maintain data integrity
 */

#include <gtest/gtest.h>

#include <fstream>
#include <filesystem>

#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/SchemaValidator.h>

#include <dnv/vista/sdk/serialization/json/TimeSeriesDataSerializationTraits.h>
#include <dnv/vista/sdk/transport/ShipId.h>
#include <dnv/vista/sdk/transport/timeseries/TimeSeriesData.h>

using namespace nfx::serialization::json;
using namespace dnv::vista::sdk::transport;
using namespace dnv::vista::sdk::transport::timeseries;

namespace dnv::vista::sdk::tests
{
	//=====================================================================
	// TimeSeriesData serialization and deserialization round-trip
	//=====================================================================

	TEST( TimeSeriesSerializationTests, CompleteSerializationDeserializationRoundTrip )
	{
		//----------------------------------------------
		// Original TimeSeriesDataPackage creation
		//----------------------------------------------

		// Create ShipId
		auto shipId = ShipId::parse( "IMO1234567" );

		// Create TimeRange for the data package
		auto startTime = nfx::time::DateTimeOffset{ "2025-01-15T08:00:00Z" };
		auto endTime = nfx::time::DateTimeOffset{ "2025-01-15T18:00:00Z" };
		timeseries::TimeRange timeRange{ startTime, endTime };

		// Create SystemConfiguration references
		std::vector<timeseries::ConfigurationReference> systemConfigs;
		systemConfigs.emplace_back( "MainSystemConfig", nfx::time::DateTimeOffset{ "2025-01-01T00:00:00Z" } );
		systemConfigs.emplace_back( "SensorConfig", nfx::time::DateTimeOffset{ "2025-01-10T12:00:00Z" } );

		// Create custom headers for Header with metadata
		nfx::containers::FastHashMap<std::string, Value> customHeaders;
		customHeaders.insertOrAssign( "DataProcessingVersion", Value::String{ "TimeSeriesProcessor-2.1.0" } );
		customHeaders.insertOrAssign( "MaxSampleRate", Value::Integer{ 1000 } );
		customHeaders.insertOrAssign( "IsRealTimeData", Value::Boolean{ true } );
		customHeaders.insertOrAssign( "ProcessingAccuracy", Value::Decimal{ 0.0001 } );
		customHeaders.insertOrAssign( "LastProcessed", Value::DateTime{ "2025-01-15T20:30:00Z" } );

		// Create Header
		timeseries::Header header{
			shipId,
			timeRange,
			nfx::time::DateTimeOffset::utcNow(),
			nfx::time::DateTimeOffset::utcNow(),
			"TimeSeriesDataProcessor",
			systemConfigs,
			customHeaders };

		// Create TimeSeriesData list
		std::vector<timeseries::TimeSeriesData> timeSeriesDataList;

		//----------------------------------------------
		// TimeSeriesData with TabularData & EventData creation
		//----------------------------------------------

		{
			// Create DataConfiguration reference
			dnv::vista::sdk::transport::timeseries::ConfigurationReference dataConfig{
				"NavigationSensors",
				nfx::time::DateTimeOffset{ "2025-01-10T10:00:00Z" } };

			// Create tabular data
			std::vector<timeseries::TabularData> tabularDataList;

			// Tabular data 1: GPS Position Data (synchronized measurements)
			{
				// Create DataChannelIds for tabular data
				std::vector<timeseries::DataChannelId> channelIds;
				channelIds.push_back( timeseries::DataChannelId::parse( "/dnv-v2/vis-3-9a/710.1/F211.11/meta/qty-latitude" ) );
				channelIds.push_back( timeseries::DataChannelId::parse( "/dnv-v2/vis-3-9a/710.1/F211.12/meta/qty-longitude" ) );
				channelIds.push_back( timeseries::DataChannelId::parse( "GPSSpeed" ) ); // Create TabularDataSets (synchronized sensor readings)
				std::vector<timeseries::TabularDataSet> dataSets;

				// Dataset 1: 08:00:00
				{
					auto timestamp1 = nfx::time::DateTimeOffset{ "2025-01-15T08:00:00Z" };
					std::vector<std::string> values1{ "59.9139", "10.7522", "12.5" };
					std::vector<std::string> qualities1{ "A", "A", "A" };
					dataSets.emplace_back( timestamp1, std::move( values1 ), qualities1 );
				}

				// Dataset 2: 08:05:00
				{
					auto timestamp2 = nfx::time::DateTimeOffset{ "2025-01-15T08:05:00Z" };
					std::vector<std::string> values2{ "59.9145", "10.7530", "13.2" };
					std::vector<std::string> qualities2{ "A", "A", "A" };
					dataSets.emplace_back( timestamp2, std::move( values2 ), qualities2 );
				}

				// Dataset 3: 08:10:00
				{
					auto timestamp3 = nfx::time::DateTimeOffset{ "2025-01-15T08:10:00Z" };
					std::vector<std::string> values3{ "59.9152", "10.7538", "14.1" };
					// No quality for this set (testing optional quality)
					dataSets.emplace_back( timestamp3, std::move( values3 ) );
				}

				tabularDataList.emplace_back( std::move( channelIds ), std::move( dataSets ) );
			}

			// Tabular data 2: Engine Parameters (synchronized measurements)
			{
				// Create DataChannelIds for engine data
				std::vector<dnv::vista::sdk::transport::timeseries::DataChannelId> engineChannelIds;
				engineChannelIds.push_back( dnv::vista::sdk::transport::timeseries::DataChannelId::parse( "MainEngineRPM" ) );
				engineChannelIds.push_back( dnv::vista::sdk::transport::timeseries::DataChannelId::parse( "EngineTemp" ) );

				// Create TabularDataSets for engine data
				std::vector<TabularDataSet> engineDataSets;

				// Engine Dataset 1
				{
					auto timestamp = nfx::time::DateTimeOffset{ "2025-01-15T08:00:00Z" };
					std::vector<std::string> values{ "850", "85.5" };
					std::vector<std::string> qualities{ "A", "A" };
					engineDataSets.emplace_back( timestamp, std::move( values ), qualities );
				}

				// Engine Dataset 2
				{
					auto timestamp = nfx::time::DateTimeOffset{ "2025-01-15T08:05:00Z" };
					std::vector<std::string> values{ "875", "87.2" };
					std::vector<std::string> qualities{ "A", "A" };
					engineDataSets.emplace_back( timestamp, std::move( values ), qualities );
				}

				tabularDataList.emplace_back( std::move( engineChannelIds ), std::move( engineDataSets ) );
			}

			{ // Create event data
				std::vector<EventDataSet> eventDataSets;

				// Event 1: Door opened
				eventDataSets.emplace_back(
					nfx::time::DateTimeOffset{ "2025-01-15T08:15:30Z" },
					timeseries::DataChannelId::parse( "BridgeDoorStatus" ), // ShortId
					"Open",
					"A" );

				// Event 2: Alarm triggered
				eventDataSets.emplace_back(
					nfx::time::DateTimeOffset{ "2025-01-15T08:22:15Z" },
					timeseries::DataChannelId::parse( "/dnv-v2/vis-3-9a/831.1/G240.1/meta/state-alarm" ), // Fire alarm
					"AlarmActive",
					"A" );

				// Event 3: System status change (no quality)
				eventDataSets.emplace_back(
					nfx::time::DateTimeOffset{ "2025-01-15T08:45:00Z" },
					timeseries::DataChannelId::parse( "SystemStatus" ),
					"Operational"
					// No quality specified (testing optional quality)
				);

				EventData eventData{ std::move( eventDataSets ) };

				// Create custom data kinds
				nfx::containers::FastHashMap<std::string, Value> customDataKinds;
				customDataKinds.insertOrAssign( "DataSource", Value::String{ "PrimaryNavigation" } );
				customDataKinds.insertOrAssign( "SamplingRate", Value::Integer{ 300 } );
				customDataKinds.insertOrAssign( "IsValidated", Value::Boolean{ true } );
				customDataKinds.insertOrAssign( "ProcessingDelay", Value::Decimal{ 0.125 } );
				customDataKinds.insertOrAssign( "LastValidation", Value::DateTime{ "2025-01-15T07:55:00Z" } );

				// Create TimeSeriesData
				timeSeriesDataList.emplace_back(
					dataConfig,		 // dataConfiguration
					tabularDataList, // tabularData
					eventData,		 // eventData
					customDataKinds	 // customDataKinds
				);
			}
		}

		// Create Package
		dnv::vista::sdk::transport::timeseries::Package package{ header, std::move( timeSeriesDataList ) };

		// Create Original TimeSeriesDataPackage
		TimeSeriesDataPackage originalPackage{ package };

		//----------------------------------------------
		// Serialize to JSON
		//----------------------------------------------

		auto options = Serializer<TimeSeriesDataPackage>::Options{};
		options.prettyPrint = true;
		options.includeNullFields = false;

		std::string jsonString = Serializer<TimeSeriesDataPackage>::toString( originalPackage, options );

		// Verify JSON is not empty
		ASSERT_FALSE( jsonString.empty() ) << "Serialized JSON should not be empty!";
		ASSERT_TRUE( jsonString.find( "Package" ) != std::string::npos ) << "JSON should contain Package root!";
		ASSERT_TRUE( jsonString.find( "Header" ) != std::string::npos ) << "JSON should contain Header!";
		ASSERT_TRUE( jsonString.find( "TimeSeriesData" ) != std::string::npos ) << "JSON should contain TimeSeriesData!";
		ASSERT_TRUE( jsonString.find( "IMO1234567" ) != std::string::npos ) << "JSON should contain ShipID!";
		ASSERT_TRUE( jsonString.find( "TabularData" ) != std::string::npos ) << "JSON should contain TabularData!";
		ASSERT_TRUE( jsonString.find( "EventData" ) != std::string::npos ) << "JSON should contain EventData!";

		std::cout << "=== SERIALIZED TIMESERIES JSON ===" << std::endl;
		std::cout << jsonString << std::endl;
		std::cout << "==================================" << std::endl;

		//----------------------------------------------
		// JSON Schema Validation
		//----------------------------------------------

		// Load the TimeSeriesData JSON schema
		std::string schemaPath = std::string( VISTA_SDK_SCHEMAS_DIR ) + "/json/TimeSeriesData.schema.json";
		ASSERT_TRUE( std::filesystem::exists( schemaPath ) ) << "Schema file must exist at: " << schemaPath;

		std::ifstream schemaFile( schemaPath );
		ASSERT_TRUE( schemaFile.is_open() ) << "Could not open schema file: " << schemaPath;

		std::string schemaContent( ( std::istreambuf_iterator<char>( schemaFile ) ), std::istreambuf_iterator<char>() );
		schemaFile.close();

		ASSERT_FALSE( schemaContent.empty() ) << "Schema file must not be empty!";

		// Create schema document
		auto schemaDoc = Document::fromString( schemaContent );
		ASSERT_TRUE( schemaDoc.has_value() ) << "Failed to parse schema JSON!";

		// Create JSON document from serialized string
		auto jsonDoc = Document::fromString( jsonString );
		ASSERT_TRUE( jsonDoc.has_value() ) << "Failed to parse serialized JSON!";

		// Validate JSON against schema
		SchemaValidator validator( schemaDoc.value() );
		auto validationResult = validator.validate( jsonDoc.value() );

		// Assert validation success
		if ( !validationResult.isValid() )
		{
			std::string errorMsg = "JSON validation failed! Errors:\n";
			for ( const auto& error : validationResult.errors() )
			{
				errorMsg += "  - " + error.message() + "\n";
			}
			FAIL() << errorMsg;
		}

		//----------------------------------------------
		// Deserialize from JSON
		//----------------------------------------------

		TimeSeriesDataPackage deserializedPackage = Serializer<TimeSeriesDataPackage>::fromString( jsonString, options );

		//----------------------------------------------
		// Check for exact equality
		//----------------------------------------------

		// Verify Package structure
		ASSERT_EQ( originalPackage.package().header().has_value(),
			deserializedPackage.package().header().has_value() )
			<< "Header presence must be exactly equal!";

		if ( originalPackage.package().header().has_value() )
		{
			const auto& originalHeader = originalPackage.package().header().value();
			const auto& deserializedHeader = deserializedPackage.package().header().value();

			// Verify ShipID
			ASSERT_EQ( originalHeader.shipId().toString(),
				deserializedHeader.shipId().toString() )
				<< "ShipID must be exactly equal!";

			// Verify Author
			ASSERT_EQ( originalHeader.author().has_value(),
				deserializedHeader.author().has_value() )
				<< "Author presence must be exactly equal!";

			if ( originalHeader.author().has_value() )
			{
				ASSERT_EQ( originalHeader.author().value(),
					deserializedHeader.author().value() )
					<< "Author value must be exactly equal!";
			}

			// Verify DateCreated
			ASSERT_EQ( originalHeader.dateCreated().has_value(),
				deserializedHeader.dateCreated().has_value() )
				<< "DateCreated presence must be exactly equal!";

			if ( originalHeader.dateCreated().has_value() )
			{
				ASSERT_EQ( originalHeader.dateCreated()->toIso8601Extended(),
					deserializedHeader.dateCreated()->toIso8601Extended() )
					<< "DateCreated value must be exactly equal!";
			}

			// Verify DateModified
			ASSERT_EQ( originalHeader.dateModified().has_value(),
				deserializedHeader.dateModified().has_value() )
				<< "DateModified presence must be exactly equal!";

			if ( originalHeader.dateModified().has_value() )
			{
				ASSERT_EQ( originalHeader.dateModified()->toIso8601Extended(),
					deserializedHeader.dateModified()->toIso8601Extended() )
					<< "DateModified value must be exactly equal!";
			}

			// Verify TimeRange
			ASSERT_EQ( originalHeader.timeRange().has_value(),
				deserializedHeader.timeRange().has_value() )
				<< "TimeRange presence must be exactly equal!";

			if ( originalHeader.timeRange().has_value() )
			{
				ASSERT_EQ( originalHeader.timeRange()->start().toIso8601Extended(),
					deserializedHeader.timeRange()->start().toIso8601Extended() )
					<< "TimeRange.Start must be exactly equal!";

				ASSERT_EQ( originalHeader.timeRange()->end().toIso8601Extended(),
					deserializedHeader.timeRange()->end().toIso8601Extended() )
					<< "TimeRange.End must be exactly equal!";
			}

			// Verify SystemConfiguration
			ASSERT_EQ( originalHeader.systemConfiguration().has_value(),
				deserializedHeader.systemConfiguration().has_value() )
				<< "SystemConfiguration presence must be exactly equal!";

			if ( originalHeader.systemConfiguration().has_value() )
			{
				const auto& originalConfigs = originalHeader.systemConfiguration().value();
				const auto& deserializedConfigs = deserializedHeader.systemConfiguration().value();

				ASSERT_EQ( originalConfigs.size(), deserializedConfigs.size() )
					<< "SystemConfiguration size must be exactly equal!";

				for ( size_t i = 0; i < originalConfigs.size(); ++i )
				{
					ASSERT_EQ( originalConfigs[i].id(), deserializedConfigs[i].id() )
						<< "SystemConfiguration[" << i << "].ID must be exactly equal!";

					ASSERT_EQ( originalConfigs[i].timeStamp().toIso8601Extended(),
						deserializedConfigs[i].timeStamp().toIso8601Extended() )
						<< "SystemConfiguration[" << i << "].TimeStamp must be exactly equal!";
				}
			}

			// Verify Header CustomHeaders
			ASSERT_EQ( originalHeader.customHeaders().has_value(),
				deserializedHeader.customHeaders().has_value() )
				<< "Header CustomHeaders presence must be exactly equal!";

			if ( originalHeader.customHeaders().has_value() )
			{
				const auto& originalCustomHeaders = originalHeader.customHeaders().value();
				const auto& deserializedCustomHeaders = deserializedHeader.customHeaders().value();

				ASSERT_EQ( originalCustomHeaders.size(), deserializedCustomHeaders.size() )
					<< "Header CustomHeaders size must be exactly equal!";

				// Verify each CustomHeader entry by key and value type
				for ( const auto& [key, originalValue] : originalCustomHeaders )
				{
					const auto* deserializedValuePtr = deserializedCustomHeaders.find( key );
					ASSERT_TRUE( deserializedValuePtr != nullptr )
						<< "Header CustomHeaders must contain key '" << key << "'!";

					const auto& deserializedValue = *deserializedValuePtr;
					ASSERT_EQ( originalValue.type(), deserializedValue.type() )
						<< "Header CustomHeaders['" << key << "'] type must be exactly equal!"; // Verify value content based on type
					switch ( originalValue.type() )
					{
						case dnv::vista::sdk::transport::Value::Type::String:
						{
							ASSERT_EQ( originalValue.string().value(), deserializedValue.string().value() )
								<< "Header CustomHeaders['" << key << "'] String value must be exactly equal!";
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::Integer:
						{
							ASSERT_EQ( originalValue.integer().value(), deserializedValue.integer().value() )
								<< "Header CustomHeaders['" << key << "'] Integer value must be exactly equal!";
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::Boolean:
						{
							ASSERT_EQ( originalValue.boolean().value(), deserializedValue.boolean().value() )
								<< "Header CustomHeaders['" << key << "'] Boolean value must be exactly equal!";
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::Decimal:
						{
							ASSERT_EQ( originalValue.decimal().value(), deserializedValue.decimal().value() )
								<< "Header CustomHeaders['" << key << "'] Decimal value must be exactly equal!";
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::DateTime:
						{
							ASSERT_EQ( originalValue.dateTime().value(), deserializedValue.dateTime().value() )
								<< "Header CustomHeaders['" << key << "'] DateTime value must be exactly equal!";
							break;
						}
						default:
						{
							FAIL() << "Unexpected Value type in Header CustomHeaders['" << key << "']!";
							break;
						}
					}
				}
			}
		}

		// Verify TimeSeriesData size
		ASSERT_EQ( originalPackage.package().timeSeriesData().size(),
			deserializedPackage.package().timeSeriesData().size() )
			<< "TimeSeriesData size must be exactly equal!";

		// Verify each TimeSeriesData
		for ( size_t i = 0; i < originalPackage.package().timeSeriesData().size(); ++i )
		{
			const auto& originalTSD = originalPackage.package().timeSeriesData()[i];
			const auto& deserializedTSD = deserializedPackage.package().timeSeriesData()[i];

			// Verify DataConfiguration
			ASSERT_EQ( originalTSD.dataConfiguration().has_value(),
				deserializedTSD.dataConfiguration().has_value() )
				<< "TimeSeriesData[" << i << "] DataConfiguration presence must be exactly equal!";

			if ( originalTSD.dataConfiguration().has_value() )
			{
				ASSERT_EQ( originalTSD.dataConfiguration()->id(),
					deserializedTSD.dataConfiguration()->id() )
					<< "TimeSeriesData[" << i << "] DataConfiguration.ID must be exactly equal!";

				ASSERT_EQ( originalTSD.dataConfiguration()->timeStamp().toIso8601Extended(),
					deserializedTSD.dataConfiguration()->timeStamp().toIso8601Extended() )
					<< "TimeSeriesData[" << i << "] DataConfiguration.TimeStamp must be exactly equal!";
			}

			// Verify TabularData
			ASSERT_EQ( originalTSD.tabularData().has_value(),
				deserializedTSD.tabularData().has_value() )
				<< "TimeSeriesData[" << i << "] TabularData presence must be exactly equal!";

			if ( originalTSD.tabularData().has_value() )
			{
				const auto& originalTabular = originalTSD.tabularData().value();
				const auto& deserializedTabular = deserializedTSD.tabularData().value();

				ASSERT_EQ( originalTabular.size(), deserializedTabular.size() )
					<< "TimeSeriesData[" << i << "] TabularData size must be exactly equal!";

				for ( size_t j = 0; j < originalTabular.size(); ++j )
				{
					const auto& origTab = originalTabular[j];
					const auto& deserTab = deserializedTabular[j];

					// Verify DataChannelIds
					ASSERT_EQ( origTab.dataChannelIds().size(), deserTab.dataChannelIds().size() )
						<< "TabularData[" << j << "] DataChannelIds size must be exactly equal!";

					for ( size_t k = 0; k < origTab.dataChannelIds().size(); ++k )
					{
						ASSERT_EQ( origTab.dataChannelIds()[k].toString(),
							deserTab.dataChannelIds()[k].toString() )
							<< "TabularData[" << j << "] DataChannelId[" << k << "] must be exactly equal!";
					}

					// Verify DataSets
					ASSERT_EQ( origTab.dataSets().size(), deserTab.dataSets().size() )
						<< "TabularData[" << j << "] DataSets size must be exactly equal!";

					for ( size_t k = 0; k < origTab.dataSets().size(); ++k )
					{
						const auto& origDataSet = origTab.dataSets()[k];
						const auto& deserDataSet = deserTab.dataSets()[k];

						ASSERT_EQ( origDataSet.timeStamp().toIso8601Extended(),
							deserDataSet.timeStamp().toIso8601Extended() )
							<< "DataSet[" << k << "] TimeStamp must be exactly equal!";

						ASSERT_EQ( origDataSet.value().size(), deserDataSet.value().size() )
							<< "DataSet[" << k << "] Value size must be exactly equal!";

						for ( size_t l = 0; l < origDataSet.value().size(); ++l )
						{
							ASSERT_EQ( origDataSet.value()[l], deserDataSet.value()[l] )
								<< "DataSet[" << k << "] Value[" << l << "] must be exactly equal!";
						}

						ASSERT_EQ( origDataSet.quality().has_value(), deserDataSet.quality().has_value() )
							<< "DataSet[" << k << "] Quality presence must be exactly equal!";

						if ( origDataSet.quality().has_value() )
						{
							ASSERT_EQ( origDataSet.quality()->size(), deserDataSet.quality()->size() )
								<< "DataSet[" << k << "] Quality size must be exactly equal!";

							for ( size_t l = 0; l < origDataSet.quality()->size(); ++l )
							{
								ASSERT_EQ( ( *origDataSet.quality() )[l], ( *deserDataSet.quality() )[l] )
									<< "DataSet[" << k << "] Quality[" << l << "] must be exactly equal!";
							}
						}
					}
				}
			}

			// Verify EventData
			ASSERT_EQ( originalTSD.eventData().has_value(),
				deserializedTSD.eventData().has_value() )
				<< "TimeSeriesData[" << i << "] EventData presence must be exactly equal!";

			if ( originalTSD.eventData().has_value() )
			{
				const auto& originalEvent = originalTSD.eventData().value();
				const auto& deserializedEvent = deserializedTSD.eventData().value();

				ASSERT_EQ( originalEvent.dataSet().size(), deserializedEvent.dataSet().size() )
					<< "EventData DataSet size must be exactly equal!";

				for ( size_t j = 0; j < originalEvent.dataSet().size(); ++j )
				{
					const auto& origEventSet = originalEvent.dataSet()[j];
					const auto& deserEventSet = deserializedEvent.dataSet()[j];

					ASSERT_EQ( origEventSet.timeStamp().toIso8601Extended(),
						deserEventSet.timeStamp().toIso8601Extended() )
						<< "EventDataSet[" << j << "] TimeStamp must be exactly equal!";

					ASSERT_EQ( origEventSet.dataChannelId().toString(),
						deserEventSet.dataChannelId().toString() )
						<< "EventDataSet[" << j << "] DataChannelId must be exactly equal!";

					ASSERT_EQ( origEventSet.value(), deserEventSet.value() )
						<< "EventDataSet[" << j << "] Value must be exactly equal!";

					ASSERT_EQ( origEventSet.quality().has_value(), deserEventSet.quality().has_value() )
						<< "EventDataSet[" << j << "] Quality presence must be exactly equal!";

					if ( origEventSet.quality().has_value() )
					{
						ASSERT_EQ( origEventSet.quality().value(), deserEventSet.quality().value() )
							<< "EventDataSet[" << j << "] Quality value must be exactly equal!";
					}
				}
			}

			// Verify CustomDataKinds
			ASSERT_EQ( originalTSD.customDataKinds().has_value(),
				deserializedTSD.customDataKinds().has_value() )
				<< "TimeSeriesData[" << i << "] CustomDataKinds presence must be exactly equal!";

			if ( originalTSD.customDataKinds().has_value() )
			{
				const auto& originalCustomData = originalTSD.customDataKinds().value();
				const auto& deserializedCustomData = deserializedTSD.customDataKinds().value();

				ASSERT_EQ( originalCustomData.size(), deserializedCustomData.size() )
					<< "CustomDataKinds size must be exactly equal!";

				// Verify each CustomDataKind entry
				for ( const auto& [key, originalValue] : originalCustomData )
				{
					const auto* deserializedValuePtr = deserializedCustomData.find( key );
					ASSERT_TRUE( deserializedValuePtr != nullptr )
						<< "CustomDataKinds must contain key '" << key << "'!";

					const auto& deserializedValue = *deserializedValuePtr;
					ASSERT_EQ( originalValue.type(), deserializedValue.type() )
						<< "CustomDataKinds['" << key << "'] type must be exactly equal!"; // Verify value content based on type
					switch ( originalValue.type() )
					{
						case dnv::vista::sdk::transport::Value::Type::String:
						{
							ASSERT_EQ( originalValue.string().value(), deserializedValue.string().value() )
								<< "CustomDataKinds['" << key << "'] String value must be exactly equal!";
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::Integer:
						{
							ASSERT_EQ( originalValue.integer().value(), deserializedValue.integer().value() )
								<< "CustomDataKinds['" << key << "'] Integer value must be exactly equal!";
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::Boolean:
						{
							ASSERT_EQ( originalValue.boolean().value(), deserializedValue.boolean().value() )
								<< "CustomDataKinds['" << key << "'] Boolean value must be exactly equal!";
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::Decimal:
						{
							ASSERT_EQ( originalValue.decimal().value(), deserializedValue.decimal().value() )
								<< "CustomDataKinds['" << key << "'] Decimal value must be exactly equal!";
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::DateTime:
						{
							ASSERT_EQ( originalValue.dateTime().value(), deserializedValue.dateTime().value() )
								<< "CustomDataKinds['" << key << "'] DateTime value must be exactly equal!";
							break;
						}
						default:
						{
							FAIL() << "Unexpected Value type in CustomDataKinds['" << key << "']!";
							break;
						}
					}
				}
			}
		}
	}
} // namespace dnv::vista::sdk::tests
