/**
 * @file TESTS_Serialization_JSON_DataChannelListPackage_SchemaValidation.cpp
 * @brief Comprehensive JSON Schema validation tests for Vista SDK
 * @details Validates realistic marine engine monitoring data channels with complete Restriction implementation
 */

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

#include <nfx/serialization/json/SerializationTraits.h>
#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/SchemaValidator.h>
#include <nfx/DateTime.h>
#include <nfx/datatypes/Decimal.h>

#include "dnv/vista/sdk/transport/datachannel/DataChannel.h"
#include "dnv/vista/sdk/serialization/json/DataChannelListSerializationTraits.h"

namespace dnv::vista::sdk::tests
{
	using namespace dnv::vista::sdk;
	using namespace dnv::vista::sdk::transport;
	using namespace nfx::serialization::json;

	//=====================================================================
	// Schema Validation Test Suite
	//=====================================================================

	class SchemaValidationTests : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			std::filesystem::path schemaPath = std::filesystem::path( VISTA_SDK_SCHEMAS_DIR ) / "json" / "DataChannelList.schema.json";

			std::ifstream schemaFile( schemaPath );
			ASSERT_TRUE( schemaFile.is_open() ) << "Failed to open schema file: " << schemaPath;

			std::string schemaContent( ( std::istreambuf_iterator<char>( schemaFile ) ), std::istreambuf_iterator<char>() );
			schemaFile.close();

			auto schemaDocOpt = Document::fromString( schemaContent );
			ASSERT_TRUE( schemaDocOpt.has_value() ) << "Failed to parse schema JSON";

			m_schemaDoc = std::move( schemaDocOpt.value() );
			m_validator = std::make_unique<SchemaValidator>( m_schemaDoc );
		}

		Document m_schemaDoc;
		std::unique_ptr<SchemaValidator> m_validator;
	};

	//=====================================================================
	// DataChannelListPackage Schema Validation Test
	//=====================================================================

	TEST_F( SchemaValidationTests, DataChannelListPackage_CompleteSchemaValidation )
	{
		// Create realistic marine engine monitoring data channels based on the sample JSON

		// ===== HEADER =====
		std::string author = "DNV";
		auto dateCreated = nfx::time::DateTimeOffset{ "2025-09-03T14:22:44.6401818+00:00" };
		auto shipId = ShipId::parse( "IMO1234567" );

		// ConfigurationReference matching sample structure
		ConfigurationReference dataChannelListId(
			"ExampleDataChannelList",
			nfx::time::DateTimeOffset{ "2025-09-03T14:22:44.6401818+00:00" },
			"1.0.0" );

		// VersionInformation matching sample
		VersionInformation versionInfo(
			"dnv",
			"v2",
			"https://example.com/datachannellist" );

		// CustomHeaders for marine vessel telemetry system
		nfx::containers::FastHashMap<std::string, Value> customHeaders;
		customHeaders.insertOrAssign( "VesselType", Value::String{ "Container Ship" } );
		customHeaders.insertOrAssign( "EngineManufacturer", Value::String{ "MAN Energy Solutions" } );
		customHeaders.insertOrAssign( "EngineModel", Value::String{ "MAN B&W 6G60ME-C9.5" } );
		customHeaders.insertOrAssign( "MaxPower", Value::Integer{ 15240 } ); // kW
		customHeaders.insertOrAssign( "MonitoringSystem", Value::String{ "DNV Vessel Insight" } );
		customHeaders.insertOrAssign( "CalibrationDate", Value::DateTime{ nfx::time::DateTimeOffset{ "2025-01-15T08:00:00Z" } } );
		customHeaders.insertOrAssign( "SamplingRate", Value::Decimal{ nfx::datatypes::Decimal{ "10.0" } } ); // Hz
		customHeaders.insertOrAssign( "CriticalAlarmsEnabled", Value::Boolean{ true } );

		Header header( author, dateCreated, shipId, dataChannelListId, versionInfo, customHeaders );

		// ===== DATA CHANNELS =====
		std::vector<DataChannel> dataChannels;

		// === Channel 1: Turbocharger Speed with Complete Restrictions ===
		{
			auto localId1 = LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-rotational.frequency" );
			NameObject nameObject1( "/dnv-v2", std::nullopt );
			DataChannelId channelId1( localId1, "TC-Speed", nameObject1 );

			DataChannelType channelType1( "Inst", 1.0, std::nullopt ); // 1 Hz sampling

			// Turbocharger speed validation restrictions
			Restriction restriction1;
			// Valid operating states
			std::vector<std::string> speedStates = { "NORMAL", "HIGH", "CRITICAL", "SHUTDOWN" };
			restriction1.setEnumeration( speedStates );
			// Pattern for operational status codes
			restriction1.setPattern( "^[A-Z]+$" );
			// Operating range: 5000-25000 RPM
			restriction1.setMinInclusive( 5000.0 );
			restriction1.setMaxInclusive( 25000.0 );
			// Critical thresholds
			restriction1.setMinExclusive( 4500.0 );
			restriction1.setMaxExclusive( 26000.0 );
			// Status code length
			restriction1.setMinLength( 4 );
			restriction1.setMaxLength( 12 );
			// Precision for RPM readings
			restriction1.setTotalDigits( 5 );
			restriction1.setFractionDigits( 1 );
			restriction1.setWhiteSpace( WhiteSpace::Collapse );

			Format format1( "String", restriction1 );
			Range range1( 5000.0, 25000.0 );

			// Unit with engine specifications
			nfx::containers::FastHashMap<std::string, Value> unitCustomElements1;
			unitCustomElements1.insertOrAssign( "EngineType", Value::String{ "Main Engine Turbocharger" } );
			unitCustomElements1.insertOrAssign( "TurbochargerID", Value::Integer{ 1 } );
			unitCustomElements1.insertOrAssign( "Manufacturer", Value::String{ "ABB" } );
			unitCustomElements1.insertOrAssign( "Model", Value::String{ "A175-L" } );
			unitCustomElements1.insertOrAssign( "CalibrationCoeff", Value::Decimal{ nfx::datatypes::Decimal{ "1.0245" } } );
			unitCustomElements1.insertOrAssign( "LastMaintenance", Value::DateTime{ nfx::time::DateTimeOffset{ "2024-12-01T10:30:00Z" } } );
			unitCustomElements1.insertOrAssign( "IsRedundant", Value::Boolean{ false } );

			Unit unit1( "rpm", "rotational frequency", unitCustomElements1 );

			// Engine performance custom properties
			nfx::containers::FastHashMap<std::string, Value> customProperties1;
			customProperties1.insertOrAssign( "AlarmThresholdHigh", Value::Decimal{ nfx::datatypes::Decimal{ "24000.0" } } );
			customProperties1.insertOrAssign( "AlarmThresholdLow", Value::Decimal{ nfx::datatypes::Decimal{ "6000.0" } } );
			customProperties1.insertOrAssign( "EfficiencyTarget", Value::Decimal{ nfx::datatypes::Decimal{ "85.5" } } );
			customProperties1.insertOrAssign( "MaintenanceInterval", Value::Integer{ 8760 } ); // hours
			customProperties1.insertOrAssign( "CriticalParameter", Value::Boolean{ true } );
			customProperties1.insertOrAssign( "LastAlarmTime", Value::DateTime{ nfx::time::DateTimeOffset{ "2024-11-15T14:20:30Z" } } );
			customProperties1.insertOrAssign( "SensorLocation", Value::String{ "Turbocharger Inlet" } );

			Property property1(
				channelType1,
				format1,
				range1,
				unit1,
				"A",														 // High accuracy quality coding
				"High",														 // High priority for engine safety
				"TC-Speed",													 // Name matching sample
				"Turbocharger rotational speed measurement with validation", // Remarks
				customProperties1 );

			dataChannels.emplace_back( channelId1, property1 );
		}

		// === Channel 2: Lubricating Oil Pressure Inlet ===
		{
			auto localId2 = LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-pressure/cnt-lubricating.oil/pos-inlet" );
			NameObject nameObject2( "/dnv-v2", std::nullopt );
			DataChannelId channelId2( localId2, "TC-LubeOilPressureInlet", nameObject2 );

			DataChannelType channelType2( "Inst", 0.5, std::nullopt ); // 0.5 Hz sampling

			// Pressure validation for lubrication system
			Restriction restriction2;
			// Pressure states
			std::vector<std::string> pressureStates = { "LOW", "NORMAL", "HIGH" };
			restriction2.setEnumeration( pressureStates );
			// Operating pressure range: 2.0-8.0 bar
			restriction2.setMinInclusive( 2.0 );
			restriction2.setMaxInclusive( 8.0 );
			// Critical bounds
			restriction2.setMinExclusive( 1.5 );
			restriction2.setMaxExclusive( 9.0 );
			// Pressure measurement precision
			restriction2.setTotalDigits( 4 );
			restriction2.setFractionDigits( 2 );

			Format format2( "Decimal", restriction2 );
			Range range2( 2.0, 8.0 );
			Unit unit2( "bar", "pressure", std::nullopt );

			// Lubrication system properties
			nfx::containers::FastHashMap<std::string, Value> customProperties2;
			customProperties2.insertOrAssign( "MinOperatingPressure", Value::Decimal{ nfx::datatypes::Decimal{ "2.5" } } );
			customProperties2.insertOrAssign( "MaxOperatingPressure", Value::Decimal{ nfx::datatypes::Decimal{ "7.5" } } );
			customProperties2.insertOrAssign( "FilterType", Value::String{ "Duplex Auto-Cleaning" } );
			customProperties2.insertOrAssign( "OilViscosity", Value::String{ "SAE 40" } );
			customProperties2.insertOrAssign( "SystemVolume", Value::Integer{ 2500 } ); // liters

			Property property2(
				channelType2,
				format2,
				range2,
				unit2,
				"A",
				"High",
				"TC-LubeOilPressureInlet",
				"Turbocharger lubricating oil inlet pressure monitoring for bearing protection",
				customProperties2 );

			dataChannels.emplace_back( channelId2, property2 );
		}

		// === Channel 3: Exhaust Gas Temperature Inlet ===
		{
			auto localId3 = LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
			NameObject nameObject3( "/dnv-v2", std::nullopt );
			DataChannelId channelId3( localId3, "TC-LubeOilTemperatureInlet", nameObject3 );

			DataChannelType channelType3( "Inst", 2.0, 1.0 ); // 2 Hz sampling, 1s calculation

			// Temperature restrictions for exhaust gas
			Restriction restriction3;
			// Temperature operating range: 300-650°C
			restriction3.setMinInclusive( 300.0 );
			restriction3.setMaxInclusive( 650.0 );
			// Safety limits
			restriction3.setMinExclusive( 250.0 );
			restriction3.setMaxExclusive( 700.0 );
			// Temperature precision
			restriction3.setTotalDigits( 4 );
			restriction3.setFractionDigits( 1 );

			Format format3( "Decimal", restriction3 );
			Range range3( 300.0, 650.0 );

			// Exhaust system custom elements
			nfx::containers::FastHashMap<std::string, Value> exhaustCustomElements;
			exhaustCustomElements.insertOrAssign( "ExhaustManifold", Value::String{ "Water-Cooled" } );
			exhaustCustomElements.insertOrAssign( "ThermocouplePairs", Value::Integer{ 6 } );
			exhaustCustomElements.insertOrAssign( "ResponseTime", Value::Decimal{ nfx::datatypes::Decimal{ "0.8" } } ); // seconds
			exhaustCustomElements.insertOrAssign( "MaterialGrade", Value::String{ "Inconel 625" } );

			Unit unit3( "°C", "temperature", exhaustCustomElements );

			Property property3(
				channelType3,
				format3,
				range3,
				unit3,
				"B+",
				"Critical",
				"TC-LubeOilTemperatureInlet",
				"Exhaust gas inlet temperature for turbocharger efficiency monitoring",
				std::nullopt );

			dataChannels.emplace_back( channelId3, property3 );
		}

		// === Channel 4: Lubricating Oil Temperature Outlet ===
		{
			auto localId4 = LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-temperature/cnt-lubricating.oil/pos-outlet" );
			NameObject nameObject4( "/dnv-v2", std::nullopt );
			DataChannelId channelId4( localId4, "TC-LubeOilTemperatureOutlet", nameObject4 );

			DataChannelType channelType4( "Average", 5.0, 60.0 ); // 5Hz sampling, 1-minute average
			Format format4( "Decimal", std::nullopt );
			Range range4( 60.0, 120.0 ); // 60-120°C operating range
			Unit unit4( "°C", "temperature", std::nullopt );

			Property property4(
				channelType4,
				format4,
				range4,
				unit4,
				"A-",
				"Medium",
				"TC-LubeOilTemperatureOutlet",
				"Lubricating oil outlet temperature for thermal management",
				std::nullopt );

			dataChannels.emplace_back( channelId4, property4 );
		}

		// === Channel 5: Exhaust Gas Temperature Outlet ===
		{
			auto localId5 = LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-temperature/cnt-exhaust.gas/pos-outlet" );
			NameObject nameObject5( "/dnv-v2", std::nullopt );
			DataChannelId channelId5( localId5, "ExhaustGasTemperatureOutlet", nameObject5 );

			DataChannelType channelType5( "Max", 1.0, 300.0 ); // Maximum over 5 minutes
			Format format5( "Decimal", std::nullopt );
			Range range5( 250.0, 450.0 );
			Unit unit5( "°C", "temperature", std::nullopt );

			Property property5(
				channelType5,
				format5,
				range5,
				unit5,
				"B",
				"High",
				"ExhaustGasTemperatureOutlet",
				"Maximum exhaust gas outlet temperature for efficiency analysis",
				std::nullopt );

			dataChannels.emplace_back( channelId5, property5 );
		}

		// === Channel 6: GPS Latitude ===
		{
			auto localId6 = LocalId::parse( "/dnv-v2/vis-3-9a/710.1/F211.11/meta/qty-latitude" );
			NameObject nameObject6( "/dnv-v2", std::nullopt );
			DataChannelId channelId6( localId6, "TC-Latitude", nameObject6 );

			DataChannelType channelType6( "Inst", 0.1, std::nullopt ); // 0.1 Hz GPS update

			// GPS coordinate restrictions
			Restriction restriction6;
			// Latitude range: -90 to +90 degrees
			restriction6.setMinInclusive( -90.0 );
			restriction6.setMaxInclusive( 90.0 );
			// GPS precision
			restriction6.setTotalDigits( 10 );
			restriction6.setFractionDigits( 7 );

			Format format6( "Decimal", restriction6 );
			Range range6( -90.0, 90.0 );

			// Navigation system custom elements
			nfx::containers::FastHashMap<std::string, Value> navCustomElements;
			navCustomElements.insertOrAssign( "GPSReceiver", Value::String{ "Furuno GP-170" } );
			navCustomElements.insertOrAssign( "SatelliteCount", Value::Integer{ 12 } );
			navCustomElements.insertOrAssign( "HDOP", Value::Decimal{ nfx::datatypes::Decimal{ "0.8" } } );
			navCustomElements.insertOrAssign( "DifferentialMode", Value::Boolean{ true } );

			Unit unit6( "deg", "angle", navCustomElements );

			Property property6(
				channelType6,
				format6,
				range6,
				unit6,
				"A+",
				"Low",
				"TC-Latitude",
				"Vessel GPS latitude position for navigation and compliance tracking",
				std::nullopt );

			dataChannels.emplace_back( channelId6, property6 );
		}

		// === Channel 7: GPS Longitude ===
		{
			auto localId7 = LocalId::parse( "/dnv-v2/vis-3-9a/710.1/F211.11/meta/qty-longitude" );
			NameObject nameObject7( "/dnv-v2", std::nullopt );
			DataChannelId channelId7( localId7, "TC-Longitude", nameObject7 );

			DataChannelType channelType7( "Inst", 0.1, std::nullopt ); // 0.1 Hz GPS update

			// GPS longitude restrictions
			Restriction restriction7;
			// Longitude range: -180 to +180 degrees
			restriction7.setMinInclusive( -180.0 );
			restriction7.setMaxInclusive( 180.0 );
			// GPS precision
			restriction7.setTotalDigits( 11 );
			restriction7.setFractionDigits( 7 );

			Format format7( "Decimal", restriction7 );
			Range range7( -180.0, 180.0 );
			Unit unit7( "deg", "angle", std::nullopt );

			// Navigation compliance properties
			nfx::containers::FastHashMap<std::string, Value> navProperties;
			navProperties.insertOrAssign( "IMOComplianceZone", Value::String{ "SECA_North_Sea" } );
			navProperties.insertOrAssign( "LastPortCall", Value::String{ "Rotterdam" } );
			navProperties.insertOrAssign( "NextDestination", Value::String{ "Hamburg" } );
			navProperties.insertOrAssign( "VoyageNumber", Value::Integer{ 2025001 } );
			navProperties.insertOrAssign( "ECAMonitoring", Value::Boolean{ true } );

			Property property7(
				channelType7,
				format7,
				range7,
				unit7,
				"A+",
				"Low",
				"TC-Longitude",
				"Vessel GPS longitude position for navigation and emission control area compliance",
				navProperties );

			dataChannels.emplace_back( channelId7, property7 );
		}

		// Create complete Package and DataChannelListPackage
		Package package( header, dataChannels );
		DataChannelListPackage dataChannelListPackage( package );

		// ===== SERIALIZE TO JSON =====
		Document doc;
		SerializationTraits<DataChannelListPackage>::serialize( dataChannelListPackage, doc );

		std::string jsonOutput = doc.toString( 2 );
		std::cout << "\n=== REALISTIC MARINE ENGINE MONITORING DATA ===\n";
		std::cout << jsonOutput << std::endl;
		std::cout << "===============================================\n";

		// ===== VALIDATE AGAINST SCHEMA =====
		auto validationResult = m_validator->validate( doc );

		if ( !validationResult.isValid() )
		{
			std::cout << "\n=== SCHEMA VALIDATION ERRORS ===\n";
			for ( const auto& error : validationResult.errors() )
			{
				std::cout << "ERROR: " << error.message() << std::endl;
			}
			std::cout << "================================\n";
		}

		ASSERT_TRUE( validationResult.isValid() ) << "JSON Schema validation failed - see errors above";

		// ===== ROUND-TRIP DESERIALIZATION TEST =====
		DataChannelListPackage deserializedPackage;
		ASSERT_NO_THROW( SerializationTraits<DataChannelListPackage>::deserialize( deserializedPackage, doc ) );

		// Verify round-trip integrity
		EXPECT_EQ( deserializedPackage.package().dataChannelList().size(), 7 );
		EXPECT_EQ( deserializedPackage.package().header().author(), "DNV" );
		EXPECT_TRUE( deserializedPackage.package().header().versionInformation().has_value() );
		EXPECT_TRUE( deserializedPackage.package().header().customHeaders().has_value() );

		// Verify Restriction deserialization for Turbocharger Speed channel (Channel 1)
		const auto& tcSpeedChannel = deserializedPackage.package().dataChannelList()[0];
		ASSERT_TRUE( tcSpeedChannel.property().format().restriction().has_value() );

		const auto& restriction1 = tcSpeedChannel.property().format().restriction().value();
		EXPECT_TRUE( restriction1.enumeration().has_value() );
		EXPECT_TRUE( restriction1.pattern().has_value() );
		EXPECT_TRUE( restriction1.minInclusive().has_value() );
		EXPECT_TRUE( restriction1.maxInclusive().has_value() );
		EXPECT_TRUE( restriction1.minExclusive().has_value() );
		EXPECT_TRUE( restriction1.maxExclusive().has_value() );
		EXPECT_TRUE( restriction1.minLength().has_value() );
		EXPECT_TRUE( restriction1.maxLength().has_value() );
		EXPECT_TRUE( restriction1.totalDigits().has_value() );
		EXPECT_TRUE( restriction1.fractionDigits().has_value() );
		EXPECT_TRUE( restriction1.whiteSpace().has_value() );

		EXPECT_EQ( restriction1.enumeration().value().size(), 4 ); // NORMAL, HIGH, CRITICAL, SHUTDOWN
		EXPECT_EQ( restriction1.pattern().value(), "^[A-Z]+$" );
		EXPECT_EQ( restriction1.minInclusive().value(), 5000.0 );
		EXPECT_EQ( restriction1.maxInclusive().value(), 25000.0 );
		EXPECT_EQ( restriction1.minExclusive().value(), 4500.0 );
		EXPECT_EQ( restriction1.maxExclusive().value(), 26000.0 );
		EXPECT_EQ( restriction1.whiteSpace().value(), WhiteSpace::Collapse );

		// Verify oil pressure channel (Channel 2) has restrictions
		const auto& oilPressureChannel = deserializedPackage.package().dataChannelList()[1];
		ASSERT_TRUE( oilPressureChannel.property().format().restriction().has_value() );

		// Verify GPS latitude channel (Channel 6) has coordinate restrictions
		const auto& latitudeChannel = deserializedPackage.package().dataChannelList()[5];
		ASSERT_TRUE( latitudeChannel.property().format().restriction().has_value() );
		const auto& latRestriction = latitudeChannel.property().format().restriction().value();
		EXPECT_EQ( latRestriction.minInclusive().value(), -90.0 );
		EXPECT_EQ( latRestriction.maxInclusive().value(), 90.0 );

		// Verify CustomHeaders contain marine vessel information
		const auto& deserializedCustomHeaders = deserializedPackage.package().header().customHeaders().value();
		EXPECT_TRUE( deserializedCustomHeaders.find( "VesselType" ) != nullptr );
		EXPECT_TRUE( deserializedCustomHeaders.find( "EngineManufacturer" ) != nullptr );
		EXPECT_TRUE( deserializedCustomHeaders.find( "MaxPower" ) != nullptr );
	}
} // namespace dnv::vista::sdk::tests
