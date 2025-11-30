/**
 * @file TESTS_Serialization_JSON_DataChannelListPackage.cpp
 * @brief DataChannelListPackage JSON serialization and deserialization tests
 * @details Validates that serialization and deserialization operations maintain data integrity
 */

#include <gtest/gtest.h>

#include <fstream>
#include <filesystem>

#include <nfx/serialization/json/Serializer.h>
#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/SchemaValidator.h>

#include <dnv/vista/sdk/serialization/json/DataChannelListSerializationTraits.h>
#include <dnv/vista/sdk/transport/datachannel/DataChannel.h>

using namespace dnv::vista::sdk;
using namespace dnv::vista::sdk::transport;
using namespace nfx::serialization::json;

namespace dnv::vista::sdk::tests
{
	//=====================================================================
	// DataChannelListPackage serialization and deserialization round-trip
	//=====================================================================

	TEST( SerializationTests, CompleteSerializationDeserializationRoundTrip )
	{
		//----------------------------------------------
		// Original Datachannel list creation
		//----------------------------------------------

		// Create ShipId
		auto shipId = ShipId::parse( "IMO9876543" );

		// Create ConfigurationReference for DataChannelListId
		ConfigurationReference dataChannelListId{
			"TestDataChannelList",				 // Id
			nfx::time::DateTimeOffset::utcNow(), // Timestamp
			"2.0.0"								 // Version
		};

		// Create VersionInformation
		VersionInformation versionInfo{
			"dnv-v2",				   // Naming rule
			"vis-3-9a",				   //  Naming scheme version
			"https://www.example.com/" // Optional reference URL
		};

		// Create custom headers for Header with metadata
		nfx::containers::FastHashMap<std::string, Value> customHeaders;
		customHeaders.insertOrAssign( "SystemVersion", Value::String{ "Vista-SDK-CPP-1.0.0" } );
		customHeaders.insertOrAssign( "ConfigurationVersion", Value::Integer{ 42 } );
		customHeaders.insertOrAssign( "IsProduction", Value::Boolean{ true } );
		customHeaders.insertOrAssign( "LastUpdated", Value::DateTime{ "2025-11-11T15:30:00Z" } );
		customHeaders.insertOrAssign( "Precision", Value::Decimal{ 0.001 } );

		// Create Header
		Header header{
			"Ronan",							 // Author information
			nfx::time::DateTimeOffset::utcNow(), // Creation timestamp
			shipId,								 // Ship identifier
			dataChannelListId,					 // Configuration reference for data channel list
			versionInfo,						 // Version information
			customHeaders						 // Custom headers dictionary
		};

		//----------------------------------------------
		// Datachannels creation
		//----------------------------------------------

		std::vector<DataChannel> dataChannels;

		// Channel 1: GPS latitude
		{
			auto localId = LocalId::parse( "/dnv-v2/vis-3-9a/710.1/F211.11/meta/qty-latitude" );
			DataChannelId channelId{ localId, "GPSLatitude", std::nullopt };

			DataChannelType channelType{ "Inst", std::nullopt, std::nullopt };
			Format format{ "Decimal", std::nullopt };

			// Add valid range for GPS latitude (-90 to 90 degrees)
			Range latitudeRange{ -90.0, 90.0 };

			// Add unit for degrees with custom elements
			nfx::containers::FastHashMap<std::string, Value> latitudeCustomElements;
			latitudeCustomElements.insertOrAssign( "ConversionFactor", Value::Decimal{ 1.0 } );
			latitudeCustomElements.insertOrAssign( "UnitCategory", Value::String{ "Angular" } );
			latitudeCustomElements.insertOrAssign( "IsSIUnit", Value::Boolean{ false } );
			latitudeCustomElements.insertOrAssign( "StandardUnit", Value::String{ "radian" } );
			latitudeCustomElements.insertOrAssign( "CreatedDate", Value::DateTime{ "2024-01-01T00:00:00Z" } );

			Unit latitudeUnit{ "deg", "angle", latitudeCustomElements };

			Property property{
				channelType,	// Channel type
				format,			// Data format
				latitudeRange,	// Numeric range
				latitudeUnit,	// Unit information
				"A",			// Quality coding validation
				std::nullopt,	// Alert priority
				"GPS Latitude", // Property name
				std::nullopt,	// Additional remarks
				std::nullopt	// Custom properties
			};

			dataChannels.emplace_back( channelId, property );
		}

		// Channel 2: GPS longitude
		{
			auto localId = LocalId::parse( "/dnv-v2/vis-3-9a/710.1/F211.11/meta/qty-longitude" );
			DataChannelId channelId{ localId, "GPSLongitude", std::nullopt };

			DataChannelType channelType{ "Inst", std::nullopt, std::nullopt };
			Format format{ "Decimal", std::nullopt };

			// Add valid range for GPS longitude (-180 to 180 degrees)
			Range longitudeRange{ -180.0, 180.0 };

			// Add unit for degrees
			Unit longitudeUnit{ "deg", "angle" };

			// Add custom properties to longitude channel
			nfx::containers::FastHashMap<std::string, Value> longitudeCustomProperties;
			longitudeCustomProperties.insertOrAssign( "GeodeticDatum", Value::String{ "WGS84" } );
			longitudeCustomProperties.insertOrAssign( "Hemisphere", Value::String{ "Both" } );
			longitudeCustomProperties.insertOrAssign( "CoordinateSystem", Value::String{ "Geographic" } );
			longitudeCustomProperties.insertOrAssign( "MeasurementAccuracy", Value::Decimal{ 0.0001 } );
			longitudeCustomProperties.insertOrAssign( "SampleCount", Value::Integer{ 1000 } );
			longitudeCustomProperties.insertOrAssign( "IsValidated", Value::Boolean{ true } );
			longitudeCustomProperties.insertOrAssign( "LastCalibration", Value::DateTime{ "2024-09-15T12:00:00Z" } );

			Property property{
				channelType,			  // Channel type
				format,					  // Data format
				longitudeRange,			  // Numeric range
				longitudeUnit,			  // Unit information
				"A",					  // Quality coding validation
				std::nullopt,			  // Alert priority
				"GPS Longitude",		  // Property name
				std::nullopt,			  // Additional remarks
				longitudeCustomProperties // Custom properties
			};

			dataChannels.emplace_back( channelId, property );
		}

		// Channel 3: Engine speed
		{
			auto localId = LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.41/C465/meta/qty-rotational.frequency" );
			DataChannelId channelId{ localId, "EngineSpeed", std::nullopt };

			DataChannelType channelType{ "Inst", 1.0, 0.1 };
			Format format{ "Decimal", std::nullopt };

			// Add valid range for Decimal format (0 to 5000 RPM)
			Range engineRange{ 0.0, 5000.0 };

			// Add unit for RPM with custom elements
			nfx::containers::FastHashMap<std::string, Value> engineCustomElements;
			engineCustomElements.insertOrAssign( "BaseUnit", Value::String{ "hertz" } );
			engineCustomElements.insertOrAssign( "ConversionToHz", Value::Decimal{ 60.0 } );
			engineCustomElements.insertOrAssign( "IsMetric", Value::Boolean{ false } );
			engineCustomElements.insertOrAssign( "UnitType", Value::String{ "Derived" } );
			engineCustomElements.insertOrAssign( "StandardSymbol", Value::String{ "min⁻¹" } );

			Unit engineUnit{ "rpm", "rotational frequency", engineCustomElements };

			// Add custom properties to engine speed channel
			nfx::containers::FastHashMap<std::string, Value> engineCustomProperties;
			engineCustomProperties.insertOrAssign( "EngineType", Value::String{ "MainEngine" } );
			engineCustomProperties.insertOrAssign( "CylinderCount", Value::Integer{ 8 } );
			engineCustomProperties.insertOrAssign( "MaxRPM", Value::Integer{ 4500 } );
			engineCustomProperties.insertOrAssign( "IdleRPM", Value::Integer{ 800 } );
			engineCustomProperties.insertOrAssign( "EfficiencyRating", Value::Decimal{ 92.5 } );
			engineCustomProperties.insertOrAssign( "IsElectronicControl", Value::Boolean{ true } );
			engineCustomProperties.insertOrAssign( "LastOverhaul", Value::DateTime{ "2023-06-20T10:00:00Z" } );

			Property property{
				channelType,								   // Channel type
				format,										   // Data format
				engineRange,								   // Numeric range
				engineUnit,									   // Unit information
				"A",										   // Quality coding validation
				"Normal",									   // Alert priority
				"Main Engine Speed",						   // Property name
				"Primary engine rotational speed measurement", // Additional remarks
				engineCustomProperties						   // Custom properties
			};

			dataChannels.emplace_back( channelId, property );
		}

		// Channel 4: Oil pressure with NameObject and custom metadata
		{
			auto localId = LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-pressure/cnt-lubricating.oil/pos-inlet" );

			// Create custom name objects with practical manufacturer and installation details using various Value types
			nfx::containers::FastHashMap<std::string, Value> customNameObjects;
			// String values
			customNameObjects.insertOrAssign( "Manufacturer", Value::String{ "Wärtsilä" } );					  // Sensor manufacturer
			customNameObjects.insertOrAssign( "Brand", Value::String{ "Duotech GMBH" } );						  // Product brand
			customNameObjects.insertOrAssign( "Model", Value::String{ "MF-PRESS-4000" } );						  // Sensor model
			customNameObjects.insertOrAssign( "SerialNumber", Value::String{ "WS240810001234" } );				  // Unique serial number
			customNameObjects.insertOrAssign( "PartNumber", Value::String{ "34.587.901" } );					  // Manufacturer part number
			customNameObjects.insertOrAssign( "InstallationDate", Value::DateTime{ "2024-03-15" } );			  // When installed
			customNameObjects.insertOrAssign( "CalibrationDate", Value::DateTime{ "2024-10-01" } );				  // Last calibration
			customNameObjects.insertOrAssign( "MaintenanceGroup", Value::String{ "CriticalEngineSensors" } );	  // Maintenance category
			customNameObjects.insertOrAssign( "InstallationLocation", Value::String{ "EngineRoom-Deck3-Port" } ); // Physical location
			customNameObjects.insertOrAssign( "ResponsibleCrew", Value::String{ "ChiefEngineer" } );			  // Responsible crew member

			// Numeric values - demonstrating various Value types
			customNameObjects.insertOrAssign( "SensorAge", Value::Integer{ 18 } );			   // Age in months (Integer)
			customNameObjects.insertOrAssign( "DeviceId", Value::Integer{ 4567 } );			   // Device ID (UnsignedInteger)
			customNameObjects.insertOrAssign( "LastServiceHours", Value::Integer{ 8760 } );	   // Service hours (Long)
			customNameObjects.insertOrAssign( "CalibrationAccuracy", Value::Decimal{ 0.01 } ); // Accuracy ±0.01 bar (Double)

			// Boolean values
			customNameObjects.insertOrAssign( "IsActive", Value::Boolean{ true } );				// Sensor is active
			customNameObjects.insertOrAssign( "RequiresMaintenance", Value::Boolean{ false } ); // Maintenance required
			customNameObjects.insertOrAssign( "IsRedundant", Value::Boolean{ true } );			// Has backup sensor

			// Decimal and DateTime values to test serialization/deserialization
			customNameObjects.insertOrAssign( "PrecisionRating", Value::Decimal{ 99.95 } );					  // High precision decimal
			customNameObjects.insertOrAssign( "LastMaintenance", Value::DateTime{ "2024-01-15T14:30:00Z" } ); // Maintenance timestamp

			NameObject nameObject{ "dnv-v2", customNameObjects }; // Naming rule + custom metadata
			DataChannelId channelId{ localId, "OilPressure", nameObject };

			DataChannelType channelType{ "Average", 5.0, 1.0 };
			Format format{ "Decimal", std::nullopt };

			// Add valid range for Oil Pressure (0 to 10 bar)
			Range pressureRange{ 0.0, 10.0 };

			// Add unit for bar (pressure) with custom elements
			nfx::containers::FastHashMap<std::string, Value> pressureCustomElements;
			pressureCustomElements.insertOrAssign( "SIEquivalent", Value::String{ "pascal" } );
			pressureCustomElements.insertOrAssign( "ConversionFactor", Value::Integer{ 100000 } );
			pressureCustomElements.insertOrAssign( "AtmosphereEquivalent", Value::Decimal{ 0.98692 } );
			pressureCustomElements.insertOrAssign( "IsAbsolutePressure", Value::Boolean{ false } );
			pressureCustomElements.insertOrAssign( "ReferenceCondition", Value::String{ "SeaLevel" } );

			Unit pressureUnit{ "bar", "pressure", pressureCustomElements };

			// Add custom properties to oil pressure channel
			nfx::containers::FastHashMap<std::string, Value> pressureCustomProperties;
			pressureCustomProperties.insertOrAssign( "FluidType", Value::String{ "LubricatingOil" } );
			pressureCustomProperties.insertOrAssign( "FluidGrade", Value::String{ "SAE-40" } );
			pressureCustomProperties.insertOrAssign( "OperatingTemp", Value::Integer{ 85 } );
			pressureCustomProperties.insertOrAssign( "MinimumPressure", Value::Decimal{ 2.5 } );
			pressureCustomProperties.insertOrAssign( "NormalPressure", Value::Decimal{ 4.2 } );
			pressureCustomProperties.insertOrAssign( "HasBackupSensor", Value::Boolean{ true } );
			pressureCustomProperties.insertOrAssign( "InstallationDate", Value::DateTime{ "2024-03-15T08:30:00Z" } );

			Property property{
				channelType,							// Channel type
				format,									// Data format
				pressureRange,							// Numeric range
				pressureUnit,							// Unit information
				"A",									// Quality coding validation
				"High",									// Alert priority
				"Lubricating Oil Pressure",				// Property name
				"Critical engine lubrication pressure", // Additional remarks
				pressureCustomProperties				// Custom properties
			};

			dataChannels.emplace_back( channelId, property );
		}

		// Create Package
		Package package{ header, dataChannels };

		// Create Original DataChannelListPackage
		DataChannelListPackage originalPackage{ package };

		//----------------------------------------------
		// Serialize to JSON
		//----------------------------------------------

		auto options = Serializer<DataChannelListPackage>::Options{};
		options.prettyPrint = true;
		options.includeNullFields = false;

		std::string jsonString = Serializer<DataChannelListPackage>::toString( originalPackage, options );

		// Verify JSON is not empty
		ASSERT_FALSE( jsonString.empty() ) << "Serialized JSON should not be empty!";
		ASSERT_TRUE( jsonString.find( "Package" ) != std::string::npos ) << "JSON should contain Package root!";
		ASSERT_TRUE( jsonString.find( "Header" ) != std::string::npos ) << "JSON should contain Header!";
		ASSERT_TRUE( jsonString.find( "DataChannelList" ) != std::string::npos ) << "JSON should contain DataChannelList!";
		ASSERT_TRUE( jsonString.find( "IMO9876543" ) != std::string::npos ) << "JSON should contain ShipID!";
		ASSERT_TRUE( jsonString.find( "EngineSpeed" ) != std::string::npos ) << "JSON should contain EngineSpeed!";

		std::cout << "=== SERIALIZED DATACHANNEL LIST JSON ===" << std::endl;
		std::cout << jsonString << std::endl;
		std::cout << "=========================================" << std::endl;

		//----------------------------------------------
		// JSON Schema Validation
		//----------------------------------------------

		// Load the DataChannelList JSON schema
		std::string schemaPath = std::string( VISTA_SDK_SCHEMAS_DIR ) + "/json/DataChannelList.schema.json";
		ASSERT_TRUE( std::filesystem::exists( schemaPath ) ) << "Schema file must exist at: " << schemaPath;

		std::ifstream schemaFile( schemaPath );
		ASSERT_TRUE( schemaFile.is_open() ) << "Could not open schema file: " << schemaPath;

		std::string schemaContent( ( std::istreambuf_iterator<char>( schemaFile ) ), std::istreambuf_iterator<char>() );
		schemaFile.close();

		ASSERT_FALSE( schemaContent.empty() ) << "Schema file must not be empty!";

		// Create schema document
		auto schemaDoc = Document::fromString( schemaContent );

		// Create JSON document from serialized string
		auto jsonDoc = Document::fromString( jsonString );

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

		DataChannelListPackage deserializedPackage = Serializer<DataChannelListPackage>::fromString( jsonString, options );

		//----------------------------------------------
		// Check for exact equality
		//----------------------------------------------

		// Verify Package structure
		ASSERT_EQ( originalPackage.package().header().shipId().toString(),
			deserializedPackage.package().header().shipId().toString() )
			<< "ShipID must be exactly equal!";

		ASSERT_EQ( originalPackage.package().header().author(),
			deserializedPackage.package().header().author() )
			<< "Author must be exactly equal!";

		ASSERT_EQ( originalPackage.package().header().dataChannelListId().id(),
			deserializedPackage.package().header().dataChannelListId().id() )
			<< "DataChannelListId.ID must be exactly equal!";

		ASSERT_EQ( originalPackage.package().header().dataChannelListId().version().has_value(),
			deserializedPackage.package().header().dataChannelListId().version().has_value() )
			<< "DataChannelListId.Version presence must be exactly equal!";

		if ( originalPackage.package().header().dataChannelListId().version().has_value() )
		{
			ASSERT_EQ( originalPackage.package().header().dataChannelListId().version().value(),
				deserializedPackage.package().header().dataChannelListId().version().value() )
				<< "DataChannelListId.Version value must be exactly equal!";
		}

		// Verify VersionInformation
		ASSERT_EQ( originalPackage.package().header().versionInformation().has_value(),
			deserializedPackage.package().header().versionInformation().has_value() )
			<< "VersionInformation presence must be exactly equal!";

		if ( originalPackage.package().header().versionInformation().has_value() )
		{
			ASSERT_EQ( originalPackage.package().header().versionInformation()->namingRule(),
				deserializedPackage.package().header().versionInformation()->namingRule() )
				<< "VersionInformation.NamingRule must be exactly equal!";

			ASSERT_EQ( originalPackage.package().header().versionInformation()->namingSchemeVersion(),
				deserializedPackage.package().header().versionInformation()->namingSchemeVersion() )
				<< "VersionInformation.NamingSchemeVersion must be exactly equal!";

			ASSERT_EQ( originalPackage.package().header().versionInformation()->referenceUrl().has_value(),
				deserializedPackage.package().header().versionInformation()->referenceUrl().has_value() )
				<< "VersionInformation.ReferenceUrl presence must be exactly equal!";

			if ( originalPackage.package().header().versionInformation()->referenceUrl().has_value() )
			{
				ASSERT_EQ( originalPackage.package().header().versionInformation()->referenceUrl().value(),
					deserializedPackage.package().header().versionInformation()->referenceUrl().value() )
					<< "VersionInformation.ReferenceUrl value must be exactly equal!";
			}
		}

		// Verify Header CustomHeaders
		ASSERT_EQ( originalPackage.package().header().customHeaders().has_value(),
			deserializedPackage.package().header().customHeaders().has_value() )
			<< "Header CustomHeaders presence must be exactly equal!";

		if ( originalPackage.package().header().customHeaders().has_value() )
		{
			const auto& originalCustomHeaders = originalPackage.package().header().customHeaders().value();
			const auto& deserializedCustomHeaders = deserializedPackage.package().header().customHeaders().value();

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

		// Verify DataChannelList size
		ASSERT_EQ( originalPackage.package().dataChannelList().size(),
			deserializedPackage.package().dataChannelList().size() )
			<< "DataChannelList size must be exactly equal!";

		// Verify each DataChannel
		for ( size_t i = 0; i < originalPackage.package().dataChannelList().size(); ++i )
		{
			const auto& originalChannel = originalPackage.package().dataChannelList()[i];
			const auto& deserializedChannel = deserializedPackage.package().dataChannelList()[i];

			// Verify DataChannelId
			ASSERT_EQ( originalChannel.dataChannelId().localId().toString(),
				deserializedChannel.dataChannelId().localId().toString() )
				<< "Channel " << i << " LocalID must be exactly equal!";

			ASSERT_EQ( originalChannel.dataChannelId().shortId().has_value(),
				deserializedChannel.dataChannelId().shortId().has_value() )
				<< "Channel " << i << " ShortID presence must be exactly equal!";

			if ( originalChannel.dataChannelId().shortId().has_value() )
			{
				ASSERT_EQ( originalChannel.dataChannelId().shortId().value(),
					deserializedChannel.dataChannelId().shortId().value() )
					<< "Channel " << i << " ShortID value must be exactly equal!";
			}

			ASSERT_EQ( originalChannel.dataChannelId().nameObject().has_value(),
				deserializedChannel.dataChannelId().nameObject().has_value() )
				<< "Channel " << i << " NameObject presence must be exactly equal!";

			if ( originalChannel.dataChannelId().nameObject().has_value() )
			{
				ASSERT_EQ( originalChannel.dataChannelId().nameObject()->namingRule(),
					deserializedChannel.dataChannelId().nameObject()->namingRule() )
					<< "Channel " << i << " NameObject.NamingRule must be exactly equal!";

				// Verify CustomNameObjects presence
				ASSERT_EQ( originalChannel.dataChannelId().nameObject()->customNameObjects().has_value(),
					deserializedChannel.dataChannelId().nameObject()->customNameObjects().has_value() )
					<< "Channel " << i << " NameObject.CustomNameObjects presence must be exactly equal!";

				if ( originalChannel.dataChannelId().nameObject()->customNameObjects().has_value() )
				{
					const auto& originalCustomObjects = originalChannel.dataChannelId().nameObject()->customNameObjects().value();
					const auto& deserializedCustomObjects = deserializedChannel.dataChannelId().nameObject()->customNameObjects().value();

					// Verify CustomNameObjects size
					ASSERT_EQ( originalCustomObjects.size(), deserializedCustomObjects.size() )
						<< "Channel " << i << " CustomNameObjects size must be exactly equal!";

					// Verify each CustomNameObject entry by key and value AND type
					for ( const auto& [key, originalValue] : originalCustomObjects )
					{
						const auto* deserializedValuePtr = deserializedCustomObjects.find( key );
						ASSERT_TRUE( deserializedValuePtr != nullptr )
							<< "Channel " << i << " CustomNameObjects must contain key '" << key << "'!";

						const auto& deserializedValue = *deserializedValuePtr; // Verify Value content based on original type (accounting for JSON serialization type conversions)
						switch ( originalValue.type() )
						{
							case dnv::vista::sdk::transport::Value::Type::Decimal:
							{ // Decimal serialized as string, but properly deserialized back as Decimal!
								ASSERT_EQ( deserializedValue.type(), dnv::vista::sdk::transport::Value::Type::Decimal )
									<< "Channel " << i << " CustomNameObjects['" << key << "'] Decimal type preserved through string serialization!";
								ASSERT_EQ( originalValue.decimal().value().toString(), deserializedValue.decimal().value().toString() )
									<< "Channel " << i << " CustomNameObjects['" << key << "'] Decimal value must be exactly equal!";
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::Integer:
							{
								ASSERT_EQ( deserializedValue.type(), dnv::vista::sdk::transport::Value::Type::Integer )
									<< "Channel " << i << " CustomNameObjects['" << key << "'] Integer type must be preserved!";
								ASSERT_EQ( originalValue.integer().value(), deserializedValue.integer().value() )
									<< "Channel " << i << " CustomNameObjects['" << key << "'] Integer value must be exactly equal!";
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::Boolean:
							{
								ASSERT_EQ( deserializedValue.type(), dnv::vista::sdk::transport::Value::Type::Boolean )
									<< "Channel " << i << " CustomNameObjects['" << key << "'] Boolean type must be preserved!";
								ASSERT_EQ( originalValue.boolean().value(), deserializedValue.boolean().value() )
									<< "Channel " << i << " CustomNameObjects['" << key << "'] Boolean value must be exactly equal!";
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::String:
							{
								ASSERT_EQ( deserializedValue.type(), dnv::vista::sdk::transport::Value::Type::String )
									<< "Channel " << i << " CustomNameObjects['" << key << "'] String type must be preserved!";
								ASSERT_EQ( originalValue.string().value(), deserializedValue.string().value() )
									<< "Channel " << i << " CustomNameObjects['" << key << "'] String value must be exactly equal!";
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::DateTime:
							{ // DateTime serialized as ISO string, but properly deserialized back as DateTime!
								ASSERT_EQ( deserializedValue.type(), dnv::vista::sdk::transport::Value::Type::DateTime )
									<< "Channel " << i << " CustomNameObjects['" << key << "'] DateTime type preserved through ISO string serialization!";
								ASSERT_EQ( originalValue.dateTime().value().toIso8601Extended(), deserializedValue.dateTime().value().toIso8601Extended() )
									<< "Channel " << i << " CustomNameObjects['" << key << "'] DateTime value must be exactly equal!";

								break;
							}
						}
					}
				}
			}

			// Verify Property
			ASSERT_EQ( originalChannel.property().dataChannelType().type(),
				deserializedChannel.property().dataChannelType().type() )
				<< "Channel " << i << " DataChannelType.Type must be exactly equal!";

			ASSERT_EQ( originalChannel.property().dataChannelType().updateCycle().has_value(),
				deserializedChannel.property().dataChannelType().updateCycle().has_value() )
				<< "Channel " << i << " DataChannelType.UpdateCycle presence must be exactly equal!";

			if ( originalChannel.property().dataChannelType().updateCycle().has_value() )
			{
				ASSERT_EQ( originalChannel.property().dataChannelType().updateCycle().value(),
					deserializedChannel.property().dataChannelType().updateCycle().value() )
					<< "Channel " << i << " DataChannelType.UpdateCycle value must be exactly equal!";
			}

			ASSERT_EQ( originalChannel.property().dataChannelType().calculationPeriod().has_value(),
				deserializedChannel.property().dataChannelType().calculationPeriod().has_value() )
				<< "Channel " << i << " DataChannelType.CalculationPeriod presence must be exactly equal!";

			if ( originalChannel.property().dataChannelType().calculationPeriod().has_value() )
			{
				ASSERT_EQ( originalChannel.property().dataChannelType().calculationPeriod().value(),
					deserializedChannel.property().dataChannelType().calculationPeriod().value() )
					<< "Channel " << i << " DataChannelType.CalculationPeriod value must be exactly equal!";
			}

			// Verify Format
			ASSERT_EQ( originalChannel.property().format().type(),
				deserializedChannel.property().format().type() )
				<< "Channel " << i << " Format.Type must be exactly equal!";

			// Verify optional Property fields
			ASSERT_EQ( originalChannel.property().name().has_value(),
				deserializedChannel.property().name().has_value() )
				<< "Channel " << i << " Property.Name presence must be exactly equal!";

			if ( originalChannel.property().name().has_value() )
			{
				ASSERT_EQ( originalChannel.property().name().value(),
					deserializedChannel.property().name().value() )
					<< "Channel " << i << " Property.Name value must be exactly equal!";
			}

			ASSERT_EQ( originalChannel.property().qualityCoding().has_value(),
				deserializedChannel.property().qualityCoding().has_value() )
				<< "Channel " << i << " Property.QualityCoding presence must be exactly equal!";

			if ( originalChannel.property().qualityCoding().has_value() )
			{
				ASSERT_EQ( originalChannel.property().qualityCoding().value(),
					deserializedChannel.property().qualityCoding().value() )
					<< "Channel " << i << " Property.QualityCoding value must be exactly equal!";
			}

			ASSERT_EQ( originalChannel.property().alertPriority().has_value(),
				deserializedChannel.property().alertPriority().has_value() )
				<< "Channel " << i << " Property.AlertPriority presence must be exactly equal!";

			if ( originalChannel.property().alertPriority().has_value() )
			{
				ASSERT_EQ( originalChannel.property().alertPriority().value(),
					deserializedChannel.property().alertPriority().value() )
					<< "Channel " << i << " Property.AlertPriority value must be exactly equal!";
			}

			ASSERT_EQ( originalChannel.property().remarks().has_value(),
				deserializedChannel.property().remarks().has_value() )
				<< "Channel " << i << " Property.Remarks presence must be exactly equal!";

			if ( originalChannel.property().remarks().has_value() )
			{
				ASSERT_EQ( originalChannel.property().remarks().value(),
					deserializedChannel.property().remarks().value() )
					<< "Channel " << i << " Property.Remarks value must be exactly equal!";
			}

			// Verify Range
			ASSERT_EQ( originalChannel.property().range().has_value(),
				deserializedChannel.property().range().has_value() )
				<< "Channel " << i << " Property.Range presence must be exactly equal!";

			if ( originalChannel.property().range().has_value() )
			{
				ASSERT_EQ( originalChannel.property().range()->low(),
					deserializedChannel.property().range()->low() )
					<< "Channel " << i << " Property.Range.Low must be exactly equal!";

				ASSERT_EQ( originalChannel.property().range()->high(),
					deserializedChannel.property().range()->high() )
					<< "Channel " << i << " Property.Range.High must be exactly equal!";
			}

			// Verify Unit
			ASSERT_EQ( originalChannel.property().unit().has_value(),
				deserializedChannel.property().unit().has_value() )
				<< "Channel " << i << " Property.Unit presence must be exactly equal!";

			if ( originalChannel.property().unit().has_value() )
			{
				ASSERT_EQ( originalChannel.property().unit()->unitSymbol(),
					deserializedChannel.property().unit()->unitSymbol() )
					<< "Channel " << i << " Property.Unit.UnitSymbol must be exactly equal!";

				ASSERT_EQ( originalChannel.property().unit()->quantityName().has_value(),
					deserializedChannel.property().unit()->quantityName().has_value() )
					<< "Channel " << i << " Property.Unit.QuantityName presence must be exactly equal!";

				if ( originalChannel.property().unit()->quantityName().has_value() )
				{
					ASSERT_EQ( originalChannel.property().unit()->quantityName().value(),
						deserializedChannel.property().unit()->quantityName().value() )
						<< "Channel " << i << " Property.Unit.QuantityName value must be exactly equal!";
				}

				// Verify Unit CustomElements
				ASSERT_EQ( originalChannel.property().unit()->customElements().has_value(),
					deserializedChannel.property().unit()->customElements().has_value() )
					<< "Channel " << i << " Property.Unit.CustomElements presence must be exactly equal!";

				if ( originalChannel.property().unit()->customElements().has_value() )
				{
					const auto& originalCustomElements = originalChannel.property().unit()->customElements().value();
					const auto& deserializedCustomElements = deserializedChannel.property().unit()->customElements().value();

					ASSERT_EQ( originalCustomElements.size(), deserializedCustomElements.size() )
						<< "Channel " << i << " Property.Unit.CustomElements size must be exactly equal!";

					// Verify each CustomElement entry
					for ( const auto& [key, originalValue] : originalCustomElements )
					{
						const auto* deserializedValuePtr = deserializedCustomElements.find( key );
						ASSERT_TRUE( deserializedValuePtr != nullptr )
							<< "Channel " << i << " Property.Unit.CustomElements must contain key '" << key << "'!";

						const auto& deserializedValue = *deserializedValuePtr;
						ASSERT_EQ( originalValue.type(), deserializedValue.type() )
							<< "Channel " << i << " Property.Unit.CustomElements['" << key << "'] type must be exactly equal!"; // Verify value content based on type
						switch ( originalValue.type() )
						{
							case dnv::vista::sdk::transport::Value::Type::String:
							{
								ASSERT_EQ( originalValue.string().value(), deserializedValue.string().value() )
									<< "Channel " << i << " Unit.CustomElements['" << key << "'] String value must be exactly equal!";
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::Integer:
							{
								ASSERT_EQ( originalValue.integer().value(), deserializedValue.integer().value() )
									<< "Channel " << i << " Unit.CustomElements['" << key << "'] Integer value must be exactly equal!";
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::Boolean:
							{
								ASSERT_EQ( originalValue.boolean().value(), deserializedValue.boolean().value() )
									<< "Channel " << i << " Unit.CustomElements['" << key << "'] Boolean value must be exactly equal!";
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::Decimal:
							{
								ASSERT_EQ( originalValue.decimal().value(), deserializedValue.decimal().value() )
									<< "Channel " << i << " Unit.CustomElements['" << key << "'] Decimal value must be exactly equal!";
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::DateTime:
							{
								ASSERT_EQ( originalValue.dateTime().value(), deserializedValue.dateTime().value() )
									<< "Channel " << i << " Unit.CustomElements['" << key << "'] DateTime value must be exactly equal!";
								break;
							}
							default:
							{
								FAIL() << "Unexpected Value type in Channel " << i << " Unit.CustomElements['" << key << "']!";
								break;
							}
						}
					}
				}
			}

			// Verify Property CustomProperties
			ASSERT_EQ( originalChannel.property().customProperties().has_value(),
				deserializedChannel.property().customProperties().has_value() )
				<< "Channel " << i << " Property.CustomProperties presence must be exactly equal!";

			if ( originalChannel.property().customProperties().has_value() )
			{
				const auto& originalCustomProperties = originalChannel.property().customProperties().value();
				const auto& deserializedCustomProperties = deserializedChannel.property().customProperties().value();

				ASSERT_EQ( originalCustomProperties.size(), deserializedCustomProperties.size() )
					<< "Channel " << i << " Property.CustomProperties size must be exactly equal!";

				// Verify each CustomProperty entry
				for ( const auto& [key, originalValue] : originalCustomProperties )
				{
					const auto* deserializedValuePtr = deserializedCustomProperties.find( key );
					ASSERT_TRUE( deserializedValuePtr != nullptr )
						<< "Channel " << i << " Property.CustomProperties must contain key '" << key << "'!";

					const auto& deserializedValue = *deserializedValuePtr;
					ASSERT_EQ( originalValue.type(), deserializedValue.type() )
						<< "Channel " << i << " Property.CustomProperties['" << key << "'] type must be exactly equal!"; // Verify value content based on type
					switch ( originalValue.type() )
					{
						case dnv::vista::sdk::transport::Value::Type::String:
						{
							ASSERT_EQ( originalValue.string().value(), deserializedValue.string().value() )
								<< "Channel " << i << " Property.CustomProperties['" << key << "'] String value must be exactly equal!";
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::Integer:
						{
							ASSERT_EQ( originalValue.integer().value(), deserializedValue.integer().value() )
								<< "Channel " << i << " Property.CustomProperties['" << key << "'] Integer value must be exactly equal!";
							break;
						}
						case dnv::vista::sdk::transport::Value::Type::Boolean:
						{
							{
								ASSERT_EQ( originalValue.boolean().value(), deserializedValue.boolean().value() )
									<< "Channel " << i << " Property.CustomProperties['" << key << "'] Boolean value must be exactly equal!";
								break;
							}
							case dnv::vista::sdk::transport::Value::Type::Decimal:
								ASSERT_EQ( originalValue.decimal().value(), deserializedValue.decimal().value() )
									<< "Channel " << i << " Property.CustomProperties['" << key << "'] Decimal value must be exactly equal!";
								break;
						}
						case dnv::vista::sdk::transport::Value::Type::DateTime:
						{
							ASSERT_EQ( originalValue.dateTime().value(), deserializedValue.dateTime().value() )
								<< "Channel " << i << " Property.CustomProperties['" << key << "'] DateTime value must be exactly equal!";
							break;
						}
						default:
						{
							FAIL() << "Unexpected Value type in Channel " << i << " Property.CustomProperties['" << key << "']!";
							break;
						}
					}
				}
			}
		}
	}
} // namespace dnv::vista::sdk::tests
