/**
 * @file Sample_Builders_LocalIdQuery.cpp
 * @brief LocalIdQuery builder usage examples for Vista SDK C++
 */

#include <iostream>

#include <dnv/vista/sdk/GmodPathQuery.h>
#include <dnv/vista/sdk/LocalId.h>
#include <dnv/vista/sdk/LocalIdQuery.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::samples
{
	/**
	 * @brief Examples of how to use the LocalIdQuery builder
	 * @details The LocalIdQuery builder allows for constructing queries against LocalId instances.
	 *          Due to the wide range of configurations of a LocalId, this example only covers the most common use cases
	 */
	class BuildLocalIdQuery
	{
	public:
		/**
		 * @brief Create a query that matches only a specific LocalId
		 * @param localId The LocalId to match exactly
		 * @return True if the query matches the same LocalId
		 */
		static bool matchSelf( const LocalId& localId )
		{
			// Init builder from LocalId - this will only match this specific LocalId
			auto builder = LocalIdQueryBuilder::from( localId );
			// Modify as needed
			return builder.build().match( localId );
		}

		/**
		 * @brief Create a query that matches all LocalIds
		 * @param localId Any LocalId to test against
		 * @return Always true (matches all LocalIds)
		 */
		static bool matchAll( const LocalId& localId )
		{
			// Simply create an empty builder - should match everything
			auto builder = LocalIdQueryBuilder{};
			return builder.build().match( localId );
		}

		/**
		 * @brief Create a query that matches across VIS versions
		 * @param oldLocalId The source LocalId
		 * @param newLocalId The target LocalId (potentially different VIS version)
		 * @return True if the LocalIds match across versions
		 */
		static bool matchAcrossVisVersions( const LocalId& oldLocalId, const LocalId& newLocalId )
		{
			auto builder = LocalIdQueryBuilder::from( oldLocalId );
			// No action required - Automatically converts the localids up to latest VIS version
			return builder.build().match( newLocalId );
		}

		/**
		 * @brief Create a query that matches a specific metadata tag
		 * @param tagName The codebook name for the tag
		 * @param tagValue The tag value to match
		 * @param matchLocalId The LocalId to test against
		 * @return True if the LocalId contains the specified metadata tag
		 */
		static bool matchMetadataTag( CodebookName tagName, const std::string& tagValue, const LocalId& matchLocalId )
		{
			// This is a simplified implementation - actual MetadataTagsQuery construction needed
			for ( const auto& tag : matchLocalId.metadataTags() )
			{
				if ( tag.name() == tagName && tag.value() == tagValue )
				{
					return true;
				}
			}
			return false;
		}

		/**
		 * @brief Create a query that matches all instances of a GmodPath
		 * @param initLocalId The LocalId to initialize the query from
		 * @param matchLocalId The LocalId to test against
		 * @return True if the GmodPath matches (ignoring locations)
		 */
		static bool matchAllInstancesOfAGmodPath( const LocalId& initLocalId, const LocalId& matchLocalId )
		{
			auto builder = LocalIdQueryBuilder::from( initLocalId ).withoutLocations();

			return builder.build().match( matchLocalId );
		}

		/**
		 * @brief Create a query that matches all instances of a specific GmodNode in a GmodPath
		 * @param initLocalId The LocalId to initialize the query from
		 * @param node The node code to match
		 * @param matchLocalId The LocalId to test against
		 * @return True if the specified node matches with all locations
		 */
		static bool matchAllInstancesOfAGmodNodeInGmodPath( const LocalId& initLocalId, const std::string& node, const LocalId& matchLocalId )
		{
			auto builder = LocalIdQueryBuilder::from( initLocalId );

			// Configure the specified node using proper C++ API
			auto pathQuery = GmodPathQuery::fromPath( initLocalId.primaryItem().value() )
								 .withNode( [&node]( const auto& nodes ) -> const GmodNode& {
									 return nodes.at( node );
								 },
									 true ); // matchAllLocations = true

			builder = builder.withPrimaryItem( pathQuery );

			return builder.build().match( matchLocalId );
		}
	};

	/**
	 * @brief Example usage of the LocalIdQuery builder samples
	 * @details Demonstrates all LocalIdQuery builder patterns with realistic test data
	 */
	void runLocalIdQueryBuilderExamples()
	{
		try
		{
			auto sampleLocalId = LocalId::parse( "/dnv-v2/vis-3-7a/691.811i-A/H101.11-1/meta/qty-power" );
			auto convertedSampleLocalId = VIS::instance().convertLocalId( sampleLocalId, VisVersion::v3_9a );

			if ( !convertedSampleLocalId.has_value() )
			{
				throw std::runtime_error( "convertedSampleLocalId should not be null" );
			}

			auto randomOtherLocalId = LocalId::parse( "/dnv-v2/vis-3-9a/411.1-1/C101/meta/cnt-lubricating.oil" );

			// Match self
			auto result = BuildLocalIdQuery::matchSelf( sampleLocalId );
			std::cout << "MatchSelf                                : LocalId='" << sampleLocalId.toString()
					  << "' Match='" << ( result ? "True" : "False" ) << "' Expects='True'" << std::endl;

			// Match all
			result = BuildLocalIdQuery::matchAll( sampleLocalId );
			std::cout << "MatchAll                                 : LocalId='" << sampleLocalId.toString()
					  << "' Match='" << ( result ? "True" : "False" ) << "' Expects='True'" << std::endl;

			result = BuildLocalIdQuery::matchAll( randomOtherLocalId );
			std::cout << "MatchAll                                 : LocalId='" << randomOtherLocalId.toString()
					  << "' Match='" << ( result ? "True" : "False" ) << "' Expects='True'" << std::endl;

			// Match across VIS versions
			result = BuildLocalIdQuery::matchAcrossVisVersions( sampleLocalId, convertedSampleLocalId.value() );
			std::cout << "MatchAcrossVisVersions                   : Source='" << sampleLocalId.toString()
					  << "' Target='" << convertedSampleLocalId.value().toString()
					  << "' Match='" << ( result ? "True" : "False" ) << "' Expects='True'" << std::endl;

			// Match metadata tags
			result = BuildLocalIdQuery::matchMetadataTag( CodebookName::Quantity, "power", sampleLocalId );
			std::cout << "MatchMetadataTag                         : LocalId='" << sampleLocalId.toString()
					  << "' Match='" << ( result ? "True" : "False" ) << "' Expects='True'" << std::endl;

			result = BuildLocalIdQuery::matchMetadataTag( CodebookName::Quantity, "power", randomOtherLocalId );
			std::cout << "MatchMetadataTag                         : LocalId='" << randomOtherLocalId.toString()
					  << "' Match='" << ( result ? "True" : "False" ) << "' Expects='False'" << std::endl;

			// Match all instances of a GmodPath
			// Note - changed location of 691.811i from -A to -F and H101.11-1 to H101.11-2
			auto builder = sampleLocalId.builder();
			auto pathMatchLocalId = builder.withPrimaryItem( GmodPath::parse( "691.811i-F/H101.11-2", sampleLocalId.visVersion() ) )
										.build();

			result = BuildLocalIdQuery::matchAllInstancesOfAGmodPath( sampleLocalId, pathMatchLocalId );
			std::cout << "MatchAllInstancesOfAGmodPath             : Source='" << sampleLocalId.toString()
					  << "' Target='" << pathMatchLocalId.toString()
					  << "' Match='" << ( result ? "True" : "False" ) << "' Expects='True'" << std::endl;

			// Match all instances of a GmodNode in a GmodPath - Expects false because H101.11-2 is not configured to "match all"
			result = BuildLocalIdQuery::matchAllInstancesOfAGmodNodeInGmodPath( sampleLocalId, "691.811i", pathMatchLocalId );
			std::cout << "MatchAllInstancesOfAGmodNodeInGmodPath   : Source='" << sampleLocalId.toString()
					  << "' Target='" << pathMatchLocalId.toString()
					  << "' Match='" << ( result ? "True" : "False" ) << "' Expects='False'" << std::endl;
		}
		catch ( const std::exception& e )
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
}

/**
 * @brief Main function to run the LocalIdQuery builder examples
 */
int main()
{
	std::cout << "=========== Builder sample - LocalIdQuery ===========" << std::endl;

	dnv::vista::sdk::samples::runLocalIdQueryBuilderExamples();

	return 0;
}
