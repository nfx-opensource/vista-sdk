/**
 * @file Sample_SimpleDataChannelList.cpp
 * @brief ISO 19848 DataChannelList creation example
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
 * @brief Helper class for creating DataChannelList packages
 */
class UseDataChannelList
{
private:
	// You should define the properties for each DataChannel, not default like this
	static const Property defaultProperty()
	{
		return {
			DataChannelType{
				"Inst",		  // See Table 1 in ISO19848 for acceptable values
				std::nullopt, // Represents the cycle of updating measurement value
				std::nullopt  // When a value is a result of a calculation that uses a measurement value of specific time periods, the Calculation Period shall be used to describe the said period
			},

			Format{
				"String",	  // See Table 2 in ISO19848 for acceptable values
				Restriction{} // Restrictive properties can be defined here, such as a list of acceptable values, strict max/min value, pattern, etc
			},

			std::nullopt, // Range = null - Mandatory for Decimal format types
			std::nullopt, // Unit = null - Mandatory for Decimal format types
			std::nullopt, // QualityCoding = null - If quality is included in the timeseries packages, this should be defined
			std::nullopt, // AlertPriority = null - Mandatory for Alert DataChannelType
			std::nullopt, // Name = null - we will set it later
			std::nullopt, // Remarks = null
			std::nullopt  // CustomProperties = null
		};
	}

public:
	static DataChannelListPackage create( const std::string& shipId, const nfx::containers::FastHashMap<std::string, LocalId>& dataChannels )
	{
		auto now = nfx::time::DateTimeOffset::now();

		Header header{
			"DNV",						  // Author
			now,						  // Date created
			ShipId::parse( shipId ),	  // Either IMO number or proprietary ID. Recommend using IMO
			ConfigurationReference{		  // Important reference - will be used in TimeSeriesDataPackage
				"ExampleDataChannelList", // Id
				now,					  // Configuration timestamp
				"1.0.0" },				  // Optional version string
			std::nullopt,				  // versionInformation
			std::nullopt };				  // customHeaders

		std::vector<DataChannel> channels;
		for ( const auto& kvp : dataChannels )
		{
			DataChannelId dataChannelId{
				kvp.second,	 // Required local identifier
				kvp.first,	 // Optional short identifier,
				std::nullopt // Optional name object
			};

			Property property = defaultProperty();
			property.setName( kvp.first ); // name was null, customize properties here

			channels.emplace_back( dataChannelId, property );
		}

		DataChannelList dataChannelList{ channels };

		Package package{ header, dataChannelList };

		return DataChannelListPackage{ package };
	}
};

/**
 * @brief Main function demonstrating DataChannelList creation
 */
int main()
{
	try
	{
		std::cout << "=========== ISO 19848 Manually build DataChannelList example ===========" << std::endl;

		std::string shipId = "IMO1234567";

		nfx::containers::FastHashMap<std::string, LocalId> dataChannels;
		dataChannels.insertOrAssign( "TC-Speed", LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-rotational.frequency" ) );
		dataChannels.insertOrAssign( "TC-LubeOilPressureInlet", LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-pressure/cnt-lubricating.oil/pos-inlet" ) );
		dataChannels.insertOrAssign( "TC-LubeOilTemperatureInlet", LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" ) );
		dataChannels.insertOrAssign( "TC-LubeOilTemperatureOutlet", LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-temperature/cnt-lubricating.oil/pos-outlet" ) );
		dataChannels.insertOrAssign( "ExhaustGasTemperatureOutlet", LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-temperature/cnt-exhaust.gas/pos-outlet" ) );
		dataChannels.insertOrAssign( "TC-Latitude", LocalId::parse( "/dnv-v2/vis-3-9a/710.1/F211.11/meta/qty-latitude" ) );
		dataChannels.insertOrAssign( "TC-Longitude", LocalId::parse( "/dnv-v2/vis-3-9a/710.1/F211.11/meta/qty-longitude" ) );

		auto dataChannelList = UseDataChannelList::create( shipId, dataChannels );

		std::cout << "DataChannelListPackage created with " << dataChannelList.package().dataChannelList().size() << " channels" << std::endl;

		auto options = nfx::serialization::json::Serializer<DataChannelListPackage>::Options{};
		options.includeNullFields = false;
		options.prettyPrint = true;
		options.validateOnDeserialize = true;

		std::string json = nfx::serialization::json::Serializer<DataChannelListPackage>::toString( dataChannelList, options );
		std::cout << "Produced DataChannelList JSON:\n"
				  << json << std::endl;

		return 0;
	}
	catch ( const std::exception& ex )
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}
}
