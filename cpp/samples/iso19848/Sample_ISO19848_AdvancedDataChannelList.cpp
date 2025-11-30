/**
 * @file Sample_AdvancedDataChannelList.cpp
 * @brief Advanced ISO 19848 DataChannelList creation example with ALL custom fields
 * @details Demonstrates comprehensive usage of:
 *          - Header.customHeaders
 *          - NameObject.customNameObjects
 *          - Unit.customElements
 *          - Property.customProperties
 */

#include <iostream>
#include <string>

#include <nfx/Containers.h>
#include <nfx/serialization/json/Serializer.h>
#include <nfx/DateTime.h>

#include <dnv/vista/sdk/LocalId.h>
#include <dnv/vista/sdk/serialization/json/DataChannelListSerializationTraits.h>
#include <dnv/vista/sdk/transport/ShipId.h>
#include <dnv/vista/sdk/transport/datachannel/DataChannel.h>

using namespace dnv::vista::sdk;
using namespace dnv::vista::sdk::transport;

/**
 * @brief Advanced helper class demonstrating ALL custom fields in DataChannelList
 */
class AdvancedDataChannelList
{
public:
	/**
	 * @brief Create comprehensive DataChannelListPackage with all custom fields
	 * @param shipId Ship identifier (IMO or proprietary)
	 * @return Complete DataChannelListPackage with rich metadata
	 */
	static DataChannelListPackage create( const std::string& shipId )
	{
		auto now = nfx::time::DateTimeOffset::now();

		//----------------------------------------------
		// Create header with Custom headers
		//----------------------------------------------

		auto configurationReference = ConfigurationReference{
			"AdvancedDataChannelList_v2.1", // Id
			now,							// Timestamp
			"2.1.0"							// Version
		};

		auto versionInformation = VersionInformation{
			"dnv-v2",					  // Naming rule
			"vis-3-9a",					  //  Naming scheme version
			"https://docs.vista.dnv.com/" // Optional reference URL
		};

		// Create custom headers with comprehensive system metadata
		auto customHeaders = nfx::containers::FastHashMap<std::string, Value>{};
		customHeaders["SystemVersion"] = Value::String{ "Vista-SDK-CPP-Advanced-1.2.0" };
		customHeaders["ConfigurationRevision"] = Value::Integer{ 15 };
		customHeaders["IsProductionEnvironment"] = Value::Boolean{ true };
		customHeaders["SystemPrecision"] = Value::Decimal{ 0.0001 };
		customHeaders["LastSystemUpdate"] = Value::DateTime{ "2025-10-11T12:00:00Z" };
		customHeaders["DatabaseVersion"] = Value::String{ "PostgreSQL-16.1" };
		customHeaders["MaxChannels"] = Value::Integer{ 10000 };
		customHeaders["SupportsRealTime"] = Value::Boolean{ true };

		Header header{
			"Advanced Sample System", // Author information
			now,					  // Creation timestamp
			ShipId::parse( shipId ),  // Ship identifier
			configurationReference,	  // Configuration reference for data channel list
			versionInformation,		  // Version information
			customHeaders			  // Custom headers dictionary
		};

		//----------------------------------------------
		// Create Data channels
		//----------------------------------------------

		std::vector<DataChannel> channels;

		// Channel 1: GPS latitude with advanced navigation metadata
		channels.push_back( createGPSLatitudeChannel() );

		// Channel 2: GPS longitude with advanced navigation metadata
		channels.push_back( createGPSLongitudeChannel() );

		// Channel 3: Main Engine Speed with full metadata
		channels.push_back( createEngineSpeedChannel() );

		// Channel 4: Oil Pressure with sensor metadata
		channels.push_back( createOilPressureChannel() );

		// Channel 5: Fuel Consumption with economic metadata
		channels.push_back( createFuelConsumptionChannel() );

		DataChannelList dataChannelList{ channels };
		Package package{ header, dataChannelList };

		return DataChannelListPackage{ package };
	}

private:
	/**
	 * @brief Create GPS latitude channel with advanced navigation metadata
	 */
	static DataChannel createGPSLatitudeChannel()
	{
		auto localId = LocalId::parse( "/dnv-v2/vis-3-9a/710.1/F211.11/meta/qty-latitude" );

		// Create custom name objects with GPS system metadata
		nfx::containers::FastHashMap<std::string, Value> customNameObjects;
		customNameObjects["GPSManufacturer"] = Value::String{ "Furuno" };
		customNameObjects["GPSModel"] = Value::String{ "GP-170" };
		customNameObjects["AntennaLocation"] = Value::String{ "Bridge-Roof-Center" };
		customNameObjects["SatelliteSystem"] = Value::String{ "GPS+GLONASS+Galileo" };
		customNameObjects["DifferentialCorrection"] = Value::String{ "DGPS" };
		customNameObjects["AccuracyRating"] = Value::Decimal{ 1.0 }; // meters
		customNameObjects["InstallationDate"] = Value::DateTime{ "2023-11-15T14:30:00Z" };
		customNameObjects["LastCalibration"] = Value::DateTime{ "2024-09-01T09:00:00Z" };

		NameObject nameObject{ "dnv-v2", customNameObjects };
		DataChannelId channelId{ localId, "GPSLatitude", nameObject };

		// Create unit with geographic coordinate metadata
		nfx::containers::FastHashMap<std::string, Value> unitCustomElements;
		unitCustomElements["CoordinateSystem"] = Value::String{ "WGS84" };
		unitCustomElements["AngularUnit"] = Value::String{ "DecimalDegrees" };
		unitCustomElements["Hemisphere"] = Value::String{ "Both" };
		unitCustomElements["ValidRange"] = Value::String{ "-90 to +90" };
		unitCustomElements["Precision"] = Value::Integer{ 8 }; // decimal places

		Unit unit{ "deg", "latitude angle", unitCustomElements };

		// Create property with navigation metadata
		nfx::containers::FastHashMap<std::string, Value> customProperties;
		customProperties["NavigationPurpose"] = Value::String{ "PrimaryPositioning" };
		customProperties["UpdateRate"] = Value::Decimal{ 10.0 }; // Hz
		customProperties["GeodeticDatum"] = Value::String{ "WGS84" };
		customProperties["CoordinateFormat"] = Value::String{ "DecimalDegrees" };
		customProperties["IsAISSource"] = Value::Boolean{ true };
		customProperties["VoyageDataRecorder"] = Value::Boolean{ true };
		customProperties["ECDISIntegration"] = Value::Boolean{ true };

		Property property{
			DataChannelType{ "Inst", 0.1, std::nullopt },			  // Channel type
			Format{ "Decimal", std::nullopt },						  // Data format
			Range{ -90.0, 90.0 },									  // Numeric range
			unit,													  // Unit information
			"A",													  // Quality coding validation
			"Normal",												  // Alert priority
			"GPS Latitude Position",								  // Property name
			"Primary navigation latitude coordinate from GPS system", // Additional remarks
			customProperties										  // Custom properties
		};

		return DataChannel{ channelId, property };
	}

	/**
	 * @brief Create GPS longitude channel with advanced navigation metadata
	 */
	static DataChannel createGPSLongitudeChannel()
	{
		auto localId = LocalId::parse( "/dnv-v2/vis-3-9a/710.1/F211.12/meta/qty-longitude" );

		// Create custom name objects with GPS system metadata (same system as latitude)
		nfx::containers::FastHashMap<std::string, Value> customNameObjects;
		customNameObjects["GPSManufacturer"] = Value::String{ "Furuno" };
		customNameObjects["GPSModel"] = Value::String{ "GP-170" };
		customNameObjects["AntennaLocation"] = Value::String{ "Bridge-Roof-Center" };
		customNameObjects["SatelliteSystem"] = Value::String{ "GPS+GLONASS+Galileo" };
		customNameObjects["DifferentialCorrection"] = Value::String{ "DGPS" };
		customNameObjects["AccuracyRating"] = Value::Decimal{ 1.0 }; // meters
		customNameObjects["InstallationDate"] = Value::DateTime{ "2023-11-15T14:30:00Z" };
		customNameObjects["LastCalibration"] = Value::DateTime{ "2024-09-01T09:00:00Z" };

		NameObject nameObject{ "dnv-v2", customNameObjects };
		DataChannelId channelId{ localId, "GPSLongitude", nameObject };

		// Create unit with geographic coordinate metadata for longitude
		nfx::containers::FastHashMap<std::string, Value> unitCustomElements;
		unitCustomElements["CoordinateSystem"] = Value::String{ "WGS84" };
		unitCustomElements["AngularUnit"] = Value::String{ "DecimalDegrees" };
		unitCustomElements["Hemisphere"] = Value::String{ "Both" };
		unitCustomElements["ValidRange"] = Value::String{ "-180 to +180" };
		unitCustomElements["Precision"] = Value::Integer{ 8 }; // decimal places

		Unit unit{ "deg", "longitude angle", unitCustomElements };

		// Create property with navigation metadata
		nfx::containers::FastHashMap<std::string, Value> customProperties;
		customProperties["NavigationPurpose"] = Value::String{ "PrimaryPositioning" };
		customProperties["UpdateRate"] = Value::Decimal{ 10.0 }; // Hz
		customProperties["GeodeticDatum"] = Value::String{ "WGS84" };
		customProperties["CoordinateFormat"] = Value::String{ "DecimalDegrees" };
		customProperties["IsAISSource"] = Value::Boolean{ true };
		customProperties["VoyageDataRecorder"] = Value::Boolean{ true };
		customProperties["ECDISIntegration"] = Value::Boolean{ true };
		customProperties["PrimeMeridian"] = Value::String{ "Greenwich" };

		Property property{
			DataChannelType{ "Inst", 0.1, std::nullopt },
			Format{ "Decimal", std::nullopt },
			Range{ -180.0, 180.0 },
			unit,
			"A",
			"Normal",
			"GPS Longitude Position",
			"Primary navigation longitude coordinate from GPS system",
			customProperties };

		return DataChannel{ channelId, property };
	}

	/**
	 * @brief Create main engine speed channel with comprehensive metadata
	 */
	static DataChannel createEngineSpeedChannel()
	{
		auto localId = LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.41/C465/meta/qty-rotational.frequency" );

		// Create custom name objects with sensor and installation metadata
		nfx::containers::FastHashMap<std::string, Value> customNameObjects;
		customNameObjects["Manufacturer"] = Value::String{ "MAN Energy Solutions" };
		customNameObjects["Model"] = Value::String{ "ME-C9.5-175" };
		customNameObjects["SerialNumber"] = Value::String{ "MAN20240815789" };
		customNameObjects["InstallationDate"] = Value::DateTime{ "2024-01-20T10:00:00Z" };
		customNameObjects["EngineType"] = Value::String{ "TwoStroke" };
		customNameObjects["MaxRPM"] = Value::Integer{ 175 };
		customNameObjects["PowerRating"] = Value::Decimal{ 82440.0 }; // kW
		customNameObjects["IsMainEngine"] = Value::Boolean{ true };

		NameObject nameObject{ "dnv-v2", customNameObjects };
		DataChannelId channelId{ localId, "MainEngineSpeed", nameObject };

		// Create unit with comprehensive unit metadata
		nfx::containers::FastHashMap<std::string, Value> unitCustomElements;
		unitCustomElements["SIBaseUnit"] = Value::String{ "hertz" };
		unitCustomElements["ConversionToSI"] = Value::Decimal{ 60.0 };
		unitCustomElements["UnitCategory"] = Value::String{ "RotationalFrequency" };
		unitCustomElements["IsDeriviedUnit"] = Value::Boolean{ true };
		unitCustomElements["StandardDefinition"] = Value::String{ "Revolutions per minute" };

		Unit unit{ "rpm", "rotational frequency", unitCustomElements };

		// Create property with operational metadata
		nfx::containers::FastHashMap<std::string, Value> customProperties;
		customProperties["OperationalRange"] = Value::String{ "60-175 RPM" };
		customProperties["IdleSpeed"] = Value::Integer{ 60 };
		customProperties["ServiceSpeed"] = Value::Integer{ 150 };
		customProperties["MaxContinuousRating"] = Value::Integer{ 175 };
		customProperties["FuelType"] = Value::String{ "HFO380" };
		customProperties["EmissionTier"] = Value::String{ "IMO-Tier-III" };
		customProperties["MaintenanceInterval"] = Value::Integer{ 8760 }; // hours
		customProperties["IsMonitoringCritical"] = Value::Boolean{ true };

		Property property{
			DataChannelType{ "Inst", 1.0, std::nullopt },
			Format{ "Decimal", std::nullopt },
			Range{ 0.0, 200.0 },
			unit,
			"A",	// quality coding
			"High", // alert priority
			"Main Engine Rotational Speed",
			"Primary propulsion engine rotational frequency measurement",
			customProperties };

		return DataChannel{ channelId, property };
	}

	/**
	 * @brief Create oil pressure channel with sensor and maintenance metadata
	 */
	static DataChannel createOilPressureChannel()
	{
		auto localId = LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-pressure/cnt-lubricating.oil/pos-inlet" );

		// Create custom name objects with sensor maintenance metadata
		nfx::containers::FastHashMap<std::string, Value> customNameObjects;
		customNameObjects["SensorManufacturer"] = Value::String{ "WIKA" };
		customNameObjects["SensorModel"] = Value::String{ "A-10" };
		customNameObjects["SensorRange"] = Value::String{ "0-16 bar" };
		customNameObjects["CalibrationCertificate"] = Value::String{ "WIKA-2024-C789456" };
		customNameObjects["InstallationLocation"] = Value::String{ "Engine-Port-Side" };
		customNameObjects["MaintenanceResponsible"] = Value::String{ "ChiefEngineer" };
		customNameObjects["NextCalibrationDate"] = Value::DateTime{ "2025-03-15T10:00:00Z" };
		customNameObjects["IsRedundancyAvailable"] = Value::Boolean{ true };

		NameObject nameObject{ "dnv-v2", customNameObjects };
		DataChannelId channelId{ localId, "LubeOilPressure", nameObject };

		// Create unit with pressure measurement metadata
		nfx::containers::FastHashMap<std::string, Value> unitCustomElements;
		unitCustomElements["PressureType"] = Value::String{ "Gauge" };
		unitCustomElements["SIEquivalent"] = Value::String{ "pascal" };
		unitCustomElements["ConversionToSI"] = Value::Integer{ 100000 };
		unitCustomElements["ReferencePressure"] = Value::String{ "Atmospheric" };
		unitCustomElements["TemperatureCompensation"] = Value::Boolean{ true };

		Unit unit{ "bar", "pressure", unitCustomElements };

		// Create property with lubrication system metadata
		nfx::containers::FastHashMap<std::string, Value> customProperties;
		customProperties["LubricantType"] = Value::String{ "MineralOil-SAE40" };
		customProperties["OperatingTemperature"] = Value::Integer{ 85 }; // Celsius
		customProperties["MinimumPressure"] = Value::Decimal{ 2.5 };
		customProperties["NormalOperatingPressure"] = Value::Decimal{ 4.2 };
		customProperties["MaximumPressure"] = Value::Decimal{ 8.0 };
		customProperties["AlarmThreshold"] = Value::Decimal{ 2.0 };
		customProperties["ShutdownThreshold"] = Value::Decimal{ 1.5 };
		customProperties["IsCirculationPump"] = Value::Boolean{ true };

		Property property{
			DataChannelType{ "Average", 5.0, 1.0 },
			Format{ "Decimal", std::nullopt },
			Range{ 0.0, 16.0 },
			unit,
			"A",
			"High",
			"Lubricating Oil Inlet Pressure",
			"Main engine lubricating oil system inlet pressure monitoring",
			customProperties };

		return DataChannel{ channelId, property };
	}

	/**
	 * @brief Create fuel consumption channel with economic and environmental metadata
	 */
	static DataChannel createFuelConsumptionChannel()
	{
		auto localId = LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101/sec/620.1/M201.32/meta/qty-volume.flow.rate" );

		// Create custom name objects with fuel system metadata
		nfx::containers::FastHashMap<std::string, Value> customNameObjects;
		customNameObjects["FlowMeterManufacturer"] = Value::String{ "Endress+Hauser" };
		customNameObjects["FlowMeterModel"] = Value::String{ "Promass-83F" };
		customNameObjects["MeterAccuracy"] = Value::Decimal{ 0.1 }; // percent
		customNameObjects["FlowMeterType"] = Value::String{ "Coriolis" };
		customNameObjects["CalibrationStandard"] = Value::String{ "ISO-4006" };
		customNameObjects["TemperatureCompensation"] = Value::Boolean{ true };
		customNameObjects["DensityMeasurement"] = Value::Boolean{ true };
		customNameObjects["LastMaintenance"] = Value::DateTime{ "2024-08-20T11:30:00Z" };

		NameObject nameObject{ "dnv-v2", customNameObjects };
		DataChannelId channelId{ localId, "MainEngineFuelFlow", nameObject };

		// Create unit with fuel flow measurement metadata
		nfx::containers::FastHashMap<std::string, Value> unitCustomElements;
		unitCustomElements["FlowType"] = Value::String{ "VolumetricFlow" };
		unitCustomElements["FluidType"] = Value::String{ "HeavyFuelOil" };
		unitCustomElements["ReferenceTemperature"] = Value::Integer{ 15 }; // Celsius
		unitCustomElements["SIEquivalent"] = Value::String{ "cubic_meter_per_second" };
		unitCustomElements["ConversionToSI"] = Value::Decimal{ 2.777778e-7 }; // l/h to m³/s

		Unit unit{ "l/h", "volumetric flow rate", unitCustomElements };

		// Create property with fuel consumption metadata
		nfx::containers::FastHashMap<std::string, Value> customProperties;
		customProperties["FuelGrade"] = Value::String{ "IFO380" };
		customProperties["FuelDensity"] = Value::Decimal{ 991.0 };			// kg/m³
		customProperties["FuelSulfurContent"] = Value::Decimal{ 0.5 };		// percent
		customProperties["CarbonContent"] = Value::Decimal{ 85.7 };			// percent
		customProperties["CalorificValue"] = Value::Integer{ 40200 };		// kJ/kg
		customProperties["ConsumptionEfficiency"] = Value::Decimal{ 42.5 }; // g/kWh
		customProperties["CO2EmissionFactor"] = Value::Decimal{ 3.114 };	// kg CO2/kg fuel
		customProperties["IsEUMRVReporting"] = Value::Boolean{ true };

		Property property{
			DataChannelType{ "Average", 60.0, 60.0 }, // 1-minute average
			Format{ "Decimal", std::nullopt },
			Range{ 0.0, 5000.0 }, // liters per hour
			unit,
			"A",
			"Normal",
			"Main Engine Fuel Consumption",
			"Primary fuel consumption monitoring for main propulsion engine",
			customProperties };

		return DataChannel{ channelId, property };
	}
};

/**
 * @brief Main function demonstrating advanced DataChannelList creation with ALL custom fields
 */
int main()
{
	try
	{
		std::cout << "============ ISO 19848 Advanced DataChannelList Example ============" << std::endl;
		std::cout << "Demonstrating ALL custom fields:" << std::endl;
		std::cout << "  - Header.customHeaders" << std::endl;
		std::cout << "  - NameObject.customNameObjects" << std::endl;
		std::cout << "  - Unit.customElements" << std::endl;
		std::cout << "  - Property.customProperties" << std::endl;
		std::cout << "=====================================================================" << std::endl;

		std::string shipId = "IMO9876543";

		auto dataChannelList = AdvancedDataChannelList::create( shipId );

		std::cout << "\nDataChannelListPackage created with "
				  << dataChannelList.package().dataChannelList().size()
				  << " advanced channels" << std::endl;

		std::cout << "Header includes "
				  << dataChannelList.package().header().customHeaders().value().size()
				  << " custom headers" << std::endl;

		// Count total custom fields
		int totalCustomNameObjects = 0;
		int totalCustomElements = 0;
		int totalCustomProperties = 0;

		for ( const auto& channel : dataChannelList.package().dataChannelList().dataChannels() )
		{
			if ( channel.dataChannelId().nameObject().has_value() &&
				 channel.dataChannelId().nameObject()->customNameObjects().has_value() )
			{
				totalCustomNameObjects += static_cast<int>( channel.dataChannelId().nameObject()->customNameObjects()->size() );
			}

			if ( channel.property().unit().has_value() &&
				 channel.property().unit()->customElements().has_value() )
			{
				totalCustomElements += static_cast<int>( channel.property().unit()->customElements()->size() );
			}

			if ( channel.property().customProperties().has_value() )
			{
				totalCustomProperties += static_cast<int>( channel.property().customProperties()->size() );
			}
		}

		std::cout << "Total CustomNameObjects: " << totalCustomNameObjects << std::endl;
		std::cout << "Total Unit CustomElements: " << totalCustomElements << std::endl;
		std::cout << "Total Property CustomProperties: " << totalCustomProperties << std::endl;

		auto options = nfx::serialization::json::Serializer<DataChannelListPackage>::Options{};
		options.includeNullFields = false;
		options.prettyPrint = true;
		options.validateOnDeserialize = true;

		std::string json = nfx::serialization::json::Serializer<DataChannelListPackage>::toString( dataChannelList, options );

		std::cout << "\nProduced Advanced DataChannelList JSON:" << std::endl;
		std::cout << "=========================================" << std::endl;
		std::cout << json << std::endl;

		std::cout << "\nAdvanced DataChannelList example completed successfully!" << std::endl;
		std::cout << "JSON includes ALL custom fields with comprehensive metadata!" << std::endl;

		return 0;
	}
	catch ( const std::exception& ex )
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}
}
