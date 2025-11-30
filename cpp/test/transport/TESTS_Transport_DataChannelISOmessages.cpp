/**
 * @file TESTS_Transport_DataChannelISOmessages.cpp
 * @brief Comprehensive tests for DataChannel infrastructure
 */

#include <gtest/gtest.h>

#include <nfx/datetime/DateTimeOffset.h>

#include <dnv/vista/sdk/transport/datachannel/DataChannel.h>
#include <dnv/vista/sdk/transport/ShipId.h>
#include <dnv/vista/sdk/LocalId.h>
#include <dnv/vista/sdk/LocalIdBuilder.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::tests
{
	//=====================================================================
	// Helper functions for creating test data
	//=====================================================================

	/**
	 * @brief Creates ValidFullyCustomDataChannelList
	 */
	transport::DataChannelListPackage createValidFullyCustomDataChannelList()
	{
		// Create ConfigurationReference for DataChannelListId
		auto timeStamp = nfx::time::DateTimeOffset{ "2016-01-01T00:00:00Z" };
		transport::ConfigurationReference dataChannelListId{
			"DataChannelList.xml",
			timeStamp,
			"1.0" };

		// Create Header
		transport::VersionInformation versionInfo{ "some_naming_rule", "2.0", "http://somewhere.net" };

		// Add custom headers
		nfx::containers::FastHashMap<std::string, transport::Value> customHeaders;
		customHeaders.insertOrAssign( "nr:CustomHeaderElement", transport::Value::String{ "Vendor specific headers" } );

		transport::Header header{
			"Author1",
			nfx::time::DateTimeOffset{ "2015-12-01T00:00:00Z" },
			transport::ShipId::parse( "IMO1234567" ),
			dataChannelListId,
			versionInfo,
			std::move( customHeaders ) };

		// Create DataChannelList
		transport::DataChannelList dataChannelList;

		// First DataChannel - Temperature sensor
		{
			std::optional<LocalIdBuilder> localIdBuilderOpt;
			bool parsed = LocalIdBuilder::tryParse(
				"/dnv-v2/vis-3-4a/411.1-1/C101.63/S206/meta/qty-temperature/cnt-cooling.air",
				localIdBuilderOpt );
			EXPECT_TRUE( parsed );
			auto localId = localIdBuilderOpt->build();

			transport::DataChannelId dataChannelId{ localId, "0010" };

			// Add custom name objects
			nfx::containers::FastHashMap<std::string, transport::Value> customNameObjects;
			customNameObjects.insertOrAssign( "nr:CustomNameObject", transport::Value::String{ "Vendor specific NameObject" } );

			transport::NameObject nameObject{
				"Naming_Rule",				   // namingRule
				std::move( customNameObjects ) // customNameObjects
			};

			dataChannelId.setNameObject( nameObject );

			transport::DataChannelType dataChannelType{ "Inst" };
			dataChannelType.setUpdateCycle( 1.0 );

			transport::Format format{ "Decimal" };
			transport::Restriction restriction;
			restriction.setFractionDigits( 1 );
			restriction.setMaxInclusive( 200.0 );
			restriction.setMinInclusive( -150.0 );
			format.setRestriction( restriction );

			transport::Range range{ 0.0, 150.0 };

			// Custom elements for unit
			nfx::containers::FastHashMap<std::string, transport::Value> customElements;
			customElements.insertOrAssign( "nr:CustomUnitElement", transport::Value::String{ "Vendor specific unit element" } );

			transport::Unit unit{ "°C", "Temperature", customElements };

			// Custom properties
			nfx::containers::FastHashMap<std::string, transport::Value> customProperties;
			customProperties.insertOrAssign( "nr:CustomPropertyElement", transport::Value::String{ "Vendor specific property element" } );

			transport::Property property{
				dataChannelType,
				format,
				range,
				unit,
				std::string{ "OPC_QUALITY" },												 // qualityCoding
				std::nullopt,																 // alertPriority
				std::string{ "M/E #1 Air Cooler CFW OUT Temp" },							 // name
				std::string{ " Location: ECR, Manufacturer: AAA Company, Type: TYPE-AAA " }, // remarks
				customProperties															 // customProperties
			};

			transport::DataChannel dataChannel{ dataChannelId, property };
			dataChannelList.add( dataChannel );
		}

		// Second DataChannel - Alert type
		{
			std::optional<LocalIdBuilder> localIdBuilderOpt;
			bool parsed = LocalIdBuilder::tryParse(
				"/dnv-v2/vis-3-4a/511.15-1/E32/meta/qty-power",
				localIdBuilderOpt );
			EXPECT_TRUE( parsed );
			auto localId = localIdBuilderOpt->build();

			transport::DataChannelId dataChannelId{ localId, "0020" };

			transport::DataChannelType dataChannelType{ "Alert" };

			transport::Format format{ "String" };
			transport::Restriction restriction;
			restriction.setMaxLength( 100 );
			restriction.setMinLength( 0 );
			format.setRestriction( restriction );

			transport::Property property{
				dataChannelType,
				format,
				std::nullopt,			  // range
				std::nullopt,			  // unit
				std::nullopt,			  // qualityCoding
				std::string{ "Warning" }, // alertPriority
				std::nullopt,			  // name
				std::nullopt,			  // remarks
				std::nullopt			  // customProperties
			};

			transport::DataChannel dataChannel{ dataChannelId, property };
			dataChannelList.add( dataChannel );
		}

		transport::Package package{ header, std::move( dataChannelList ) };
		return transport::DataChannelListPackage{ std::move( package ) };
	}

	/**
	 * @brief Creates ValidDataChannelList
	 */
	transport::DataChannelListPackage createValidDataChannelList()
	{
		// Create ConfigurationReference for DataChannelListId
		transport::ConfigurationReference dataChannelListId{
			"some-id",
			nfx::time::DateTimeOffset( "2016-01-01T00:00:00Z" ) };

		// Create Header
		transport::Header header{
			"some-author",							  // Author
			nfx::time::DateTimeOffset::utcNow(),	  // Creation date
			transport::ShipId::parse( "IMO1234567" ), // ShipId
			dataChannelListId,						  // Configuration reference
			std::nullopt,							  // versionInformation
			std::nullopt };							  // customHeaders

		// Create DataChannelList
		transport::DataChannelList dataChannelList;

		// Single DataChannel
		{
			std::optional<LocalIdBuilder> localIdBuilderOpt;
			bool parsed = LocalIdBuilder::tryParse(
				"/dnv-v2/vis-3-4a/511.15-1/E32/meta/qty-power",
				localIdBuilderOpt );
			EXPECT_TRUE( parsed );
			auto localId = localIdBuilderOpt->build();

			transport::DataChannelId dataChannelId{ localId, "0010" };

			transport::DataChannelType dataChannelType{ "Inst" };
			transport::Format format{ "String" };

			transport::Property property{
				dataChannelType,
				format,
				std::nullopt, // range
				std::nullopt, // unit
				std::nullopt, // qualityCoding
				std::nullopt, // alertPriority
				std::nullopt, // name
				std::nullopt, // remarks
				std::nullopt  // customProperties
			};

			transport::DataChannel dataChannel{ dataChannelId, property };
			dataChannelList.add( dataChannel );
		}

		transport::Package package{ header, std::move( dataChannelList ) };
		return transport::DataChannelListPackage{ std::move( package ) };
	}

	//=====================================================================
	// Test methods
	//=====================================================================

	TEST( IsoMessageTests, Test_DataChannelList )
	{
		auto message = createValidDataChannelList();
		EXPECT_FALSE( message.package().dataChannelList().isEmpty() );

		auto& first = message.package().dataChannelList().dataChannels().front();
		EXPECT_TRUE( first.dataChannelId().localId().builder().isValid() );

		EXPECT_NO_THROW( (void)first.property() );
	}

	TEST( IsoMessageTests, Test_LocalId_Lookup )
	{
		auto message = createValidDataChannelList();
		const auto& dataChannel = message.package().dataChannelList()[0];
		const auto& localId = dataChannel.dataChannelId().localId();
		const auto& lookup = message.package().dataChannelList()[localId];
		const auto* lookup2 = message.package().dataChannelList().tryGetByLocalId( localId );
		EXPECT_NE( lookup2, nullptr );
		EXPECT_EQ( &dataChannel, &lookup );
		EXPECT_EQ( &dataChannel, lookup2 );
	}

	TEST( IsoMessageTests, Test_ShortId_Lookup )
	{
		auto message = createValidDataChannelList();
		const auto& dataChannel = message.package().dataChannelList()[0];
		const auto& shortId = dataChannel.dataChannelId().shortId();
		EXPECT_TRUE( shortId.has_value() );
		const auto& lookup = message.package().dataChannelList()[*shortId];
		const auto* lookup2 = message.package().dataChannelList().tryGetByShortId( *shortId );
		EXPECT_NE( lookup2, nullptr );
		EXPECT_EQ( &dataChannel, &lookup );
		EXPECT_EQ( &dataChannel, lookup2 );
	}

	TEST( IsoMessageTests, Test_DataChannelList_Enumerator )
	{
		auto message = createValidDataChannelList();
		const auto expectedLength = message.package().dataChannelList().dataChannels().size();
		const auto actualLength = message.package().dataChannelList().size();
		size_t counter = 0;
		EXPECT_EQ( expectedLength, actualLength );

		// Test range-based for loop iteration
		for ( const auto& dc : message.package().dataChannelList() )
		{
			EXPECT_NE( &dc, nullptr );
			counter++;
		}
		EXPECT_EQ( expectedLength, counter );

		// Test iterator-based iteration
		counter = 0;
		for ( auto it = message.package().dataChannelList().begin(); it != message.package().dataChannelList().end(); ++it )
		{
			EXPECT_NE( &( *it ), nullptr );
			counter++;
		}
		EXPECT_EQ( expectedLength, counter );
	}

	//=====================================================================
	// JSON Serialization Tests
	//=====================================================================

	/**
	 * @brief Test DataChannelList JSON serialization
	 * @details Tests JSON round-trip serialization
	 */
	TEST( IsoMessageTests, DataChannelListJson )
	{
		// TODO
	}

	//=====================================================================
	// Advanced DataChannel Tests
	//=====================================================================

	/**
	 * @brief Test fully populated DataChannel with all properties
	 * @details Tests complex DataChannel with all optional fields populated
	 */
	TEST( DataChannelTests, Test_FullyCustomDataChannel )
	{
		auto dataChannelList = createValidFullyCustomDataChannelList();

		EXPECT_EQ( dataChannelList.package().dataChannelList().size(), 2U );

		// Test custom headers
		const auto& header = dataChannelList.package().header();
		EXPECT_TRUE( header.customHeaders().has_value() );
		const auto& customHeaders = *header.customHeaders();
		EXPECT_TRUE( customHeaders.find( "nr:CustomHeaderElement" ) != nullptr );
		const auto* headerValuePtr = customHeaders.find( "nr:CustomHeaderElement" );
		ASSERT_NE( headerValuePtr, nullptr );
		const auto& headerValue = *headerValuePtr;
		EXPECT_EQ( headerValue.type(), transport::Value::Type::String );
		EXPECT_EQ( headerValue.string().value(), "Vendor specific headers" );

		// Test first DataChannel (temperature sensor)
		const auto& tempChannel = dataChannelList.package().dataChannelList()[0];
		const auto& tempProperty = tempChannel.property();

		EXPECT_EQ( tempProperty.dataChannelType().type(), "Inst" );
		EXPECT_TRUE( tempProperty.dataChannelType().updateCycle().has_value() );
		EXPECT_EQ( *tempProperty.dataChannelType().updateCycle(), 1.0 );

		EXPECT_EQ( tempProperty.format().type(), "Decimal" );
		EXPECT_TRUE( tempProperty.format().restriction().has_value() );
		const auto& restriction = *tempProperty.format().restriction();
		EXPECT_TRUE( restriction.fractionDigits().has_value() );
		EXPECT_EQ( *restriction.fractionDigits(), 1U );
		EXPECT_TRUE( restriction.maxInclusive().has_value() );
		EXPECT_EQ( *restriction.maxInclusive(), 200.0 );
		EXPECT_TRUE( restriction.minInclusive().has_value() );
		EXPECT_EQ( *restriction.minInclusive(), -150.0 );

		EXPECT_TRUE( tempProperty.range().has_value() );
		const auto& range = *tempProperty.range();
		EXPECT_EQ( range.low(), 0.0 );
		EXPECT_EQ( range.high(), 150.0 );

		EXPECT_TRUE( tempProperty.unit().has_value() );
		const auto& unit = *tempProperty.unit();
		EXPECT_EQ( unit.unitSymbol(), "°C" );
		EXPECT_TRUE( unit.quantityName().has_value() );
		EXPECT_EQ( *unit.quantityName(), "Temperature" );

		EXPECT_TRUE( tempProperty.qualityCoding().has_value() );
		EXPECT_EQ( *tempProperty.qualityCoding(), "OPC_QUALITY" );

		EXPECT_TRUE( tempProperty.name().has_value() );
		EXPECT_EQ( *tempProperty.name(), "M/E #1 Air Cooler CFW OUT Temp" );

		EXPECT_TRUE( tempProperty.remarks().has_value() );
		EXPECT_EQ( *tempProperty.remarks(), " Location: ECR, Manufacturer: AAA Company, Type: TYPE-AAA " );

		// Test second DataChannel (alert type)
		const auto& alertChannel = dataChannelList.package().dataChannelList()[1];
		const auto& alertProperty = alertChannel.property();

		EXPECT_EQ( alertProperty.dataChannelType().type(), "Alert" );
		EXPECT_EQ( alertProperty.format().type(), "String" );
		EXPECT_TRUE( alertProperty.alertPriority().has_value() );
		EXPECT_EQ( *alertProperty.alertPriority(), "Warning" );

		EXPECT_TRUE( alertProperty.format().restriction().has_value() );
		const auto& stringRestriction = *alertProperty.format().restriction();
		EXPECT_TRUE( stringRestriction.maxLength().has_value() );
		EXPECT_EQ( *stringRestriction.maxLength(), 100U );
		EXPECT_TRUE( stringRestriction.minLength().has_value() );
		EXPECT_EQ( *stringRestriction.minLength(), 0U );
	}

	//=====================================================================
	// Validation Tests
	//=====================================================================

	/**
	 * @brief Test Property validation rules
	 * @details Tests business rule validation in Property class
	 */
	TEST( DataChannelTests, Test_Property_Validation )
	{
		// Test valid decimal property
		{
			transport::DataChannelType dataChannelType{ "Inst" };
			transport::Format format{ "Decimal" };
			transport::Range range{ 0.0, 100.0 };
			transport::Unit unit{ "°C" };

			transport::Property property{
				dataChannelType,
				format,
				range,
				unit,
				std::nullopt, // qualityCoding
				std::nullopt, // alertPriority
				std::nullopt, // name
				std::nullopt, // remarks
				std::nullopt  // customProperties
			};
			auto result = property.validate();
			EXPECT_TRUE( result.isOk() );
		}

		// Test alert property with priority
		{
			transport::DataChannelType dataChannelType{ "Alert" };
			transport::Format format{ "String" };

			transport::Property property{
				dataChannelType,
				format,
				std::nullopt,			   // range
				std::nullopt,			   // unit
				std::nullopt,			   // qualityCoding
				std::string{ "Critical" }, // alertPriority
				std::nullopt,			   // name
				std::nullopt,			   // remarks
				std::nullopt			   // customProperties
			};
			auto result = property.validate();
			EXPECT_TRUE( result.isOk() );
		}
	}

	/**
	 * @brief Test Restriction validation functionality
	 * @details Tests various restriction validation scenarios
	 */
	TEST( DataChannelTests, Test_Restriction_Validation )
	{
		transport::Restriction restriction;
		transport::Format format{ "String" };

		// Test length validation
		restriction.setLength( 5 );
		auto result = restriction.validateValue( "hello", format );
		EXPECT_TRUE( result.isOk() );

		result = restriction.validateValue( "hi", format );
		EXPECT_FALSE( result.isOk() );

		result = restriction.validateValue( "toolong", format );
		EXPECT_FALSE( result.isOk() );

		// Test min/max length validation
		restriction = transport::Restriction{};
		restriction.setMinLength( 2 );
		restriction.setMaxLength( 10 );

		result = restriction.validateValue( "ok", format );
		EXPECT_TRUE( result.isOk() );

		result = restriction.validateValue( "x", format );
		EXPECT_FALSE( result.isOk() );

		result = restriction.validateValue( "thisstringistoolong", format );
		EXPECT_FALSE( result.isOk() );
	}

	/**
	 * @brief Test Range validation and construction
	 * @details Tests Range class validation rules
	 */
	TEST( RangeTest, RangeCreationAndValidation )
	{
		// Test valid range construction
		transport::Range range{ 0.0, 100.0 };
		EXPECT_EQ( range.low(), 0.0 );
		EXPECT_EQ( range.high(), 100.0 );

		// Test invalid range construction should throw
		EXPECT_THROW( transport::Range( 100.0, 0.0 ), std::invalid_argument );

		// Test setting range values
		range.setLow( 10.0 );
		EXPECT_EQ( range.low(), 10.0 );

		range.setHigh( 90.0 );
		EXPECT_EQ( range.high(), 90.0 );

		// low > high
		EXPECT_THROW( range.setLow( 200.0 ), std::invalid_argument );
		/*high < low*/
		EXPECT_THROW( range.setHigh( -10.0 ), std::invalid_argument );
	}

	/**
	 * @brief Test DataChannelList collection operations
	 * @details Tests add, remove, and lookup operations
	 */
	TEST( DataChannelTests, Test_DataChannelList_Operations )
	{
		transport::DataChannelList dataChannelList;
		EXPECT_TRUE( dataChannelList.isEmpty() );
		EXPECT_EQ( dataChannelList.size(), 0U );

		// Add single DataChannel
		auto singleList = createValidDataChannelList();
		const auto& originalChannel = singleList.package().dataChannelList()[0];

		// Create a copy for adding
		transport::DataChannel channelCopy = originalChannel;
		dataChannelList.add( channelCopy );

		EXPECT_FALSE( dataChannelList.isEmpty() );
		EXPECT_EQ( dataChannelList.size(), 1U );

		// Test lookups work
		const auto& localId = originalChannel.dataChannelId().localId();
		const auto* foundChannel = dataChannelList.tryGetByLocalId( localId );
		EXPECT_NE( foundChannel, nullptr );

		// Test clear
		dataChannelList.clear();
		EXPECT_TRUE( dataChannelList.isEmpty() );
		EXPECT_EQ( dataChannelList.size(), 0U );
	}

	//=====================================================================
	// Error Handling Tests
	//=====================================================================

	/**
	 * @brief Test error scenarios and exception handling
	 * @details Tests various error conditions and proper error reporting
	 */
	TEST( DataChannelTests, Test_Error_Handling )
	{
		// Test invalid DataChannelType construction
		EXPECT_THROW( transport::DataChannelType{ "InvalidType" }, std::invalid_argument );

		// Test invalid Format type
		EXPECT_THROW( transport::Format{ "InvalidFormat" }, std::invalid_argument );

		// Test duplicate LocalId addition
		transport::DataChannelList dataChannelList;
		auto validList = createValidDataChannelList();
		const auto& originalChannel = validList.package().dataChannelList()[0];

		// Add first channel
		transport::DataChannel firstChannel = originalChannel;
		dataChannelList.add( firstChannel );

		// Try to add duplicate LocalId - should throw
		transport::DataChannel duplicateChannel = originalChannel;
		EXPECT_THROW( dataChannelList.add( duplicateChannel ), std::invalid_argument );
	}

	/**
	 * @brief Test string optimization in error messages
	 */
	TEST( RestrictionTest, StringLengthRestrictionValidation )
	{
		// Test exact length validation with StringBuilderPool error messages
		transport::Restriction restriction;
		restriction.setLength( 5 );

		transport::Format format{ "String" };

		// Test that error messages are properly constructed using StringBuilderPool
		auto result = restriction.validateValue( "toolongstring", format );
		EXPECT_FALSE( result.isOk() );

		const auto& errorResult = result.invalid();
		EXPECT_FALSE( errorResult.errors().empty() );
		EXPECT_TRUE( errorResult.errors()[0].find( "exactly 5 characters" ) != std::string::npos );
	}
} // namespace dnv::vista::sdk::tests
