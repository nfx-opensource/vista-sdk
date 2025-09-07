/**
 * @file Sample_Builders_LocalId.cpp
 * @brief LocalId builder usage examples for Vista SDK C++
 */

#include <iostream>
#include <optional>

#include <dnv/vista/sdk/LocalId.h>
#include <dnv/vista/sdk/LocalIdBuilder.h>
#include <dnv/vista/sdk/GmodPath.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::samples
{
	/**
	 * @brief Examples of how to use the LocalId builder
	 * @details This class demonstrates various LocalId builder patterns including creation from scratch,
	 *          parsing from existing LocalIds, enabling verbose mode, and property manipulation.
	 */
	class BuildLocalId
	{
	public:
		/**
		 * @brief Example of a LocalId builder usage
		 * @param visVersion The VIS version to use
		 * @param primaryItem The primary item path as string
		 * @param quantity The quantity metadata tag value
		 * @return A constructed LocalId
		 * @details Creates a LocalId like "/dnv-v2/vis-3-9a/691.83111i/H101/meta/qty-power"
		 *          For the sake of this example - only primaryItem and quantity is configured
		 */
		static LocalId buildNew(
			VisVersion visVersion,
			const std::string& primaryItem,
			const std::string& quantity )
		{
			// Get codebooks for metadata creation
			auto codebooks = VIS::instance().codebooks( visVersion );

			// Init from scratch
			auto builder = LocalIdBuilder::create( visVersion )
							   // Allows for chaining
							   .withPrimaryItem( GmodPath::parse( primaryItem, visVersion ) );

			// Or no chaining
			builder = builder.withMetadataTag( codebooks.createTag( CodebookName::Quantity, quantity ) );

			return builder.build();
		}

		/**
		 * @brief Build LocalId from existing LocalId string
		 * @param inputLocalId The input LocalId string to parse
		 * @return A modified LocalId
		 * @details Shows two alternative approaches: direct builder parsing or accessing builder from LocalId
		 */
		static LocalId buildFromExisting( const std::string& inputLocalId )
		{
			// Alt 1 - Init builder directly
			auto builder = LocalIdBuilder::parse( inputLocalId );

			// Alt 2 - Or continue from existing LocalId
			auto localId = LocalId::parse( inputLocalId );
			auto builderFromLocalId = localId.builder(); // Access builder from LocalId

			// Modify as needed
			return builderFromLocalId.build();
		}

		/**
		 * @brief Enables verbose mode
		 * @param localId The LocalId to enable verbose mode for
		 * @return LocalId with verbose mode enabled
		 * @details See LocalId::verboseMode and LocalId::toString for more information
		 */
		static LocalId buildVerbose( const LocalId& localId )
		{
			// Access builder from existing LocalId
			auto builder = localId.builder();

			// Enables verbose mode
			return builder.withVerboseMode( true ).build();
		}

		/**
		 * @brief Remove properties from LocalId
		 * @param localId The LocalId to remove properties from
		 * @return Optional LocalIdBuilder, nullopt if builder becomes invalid
		 * @details Note - builder may become invalid, as a LocalId must have a VIS version,
		 *          primary item and at least one metadata tag. However, it is still possible
		 *          to access properties and toString on an invalid builder.
		 */
		static std::optional<LocalIdBuilder> removeProperties( const LocalId& localId )
		{
			// Access builder from existing LocalId
			auto builder = localId.builder();

			// Remove properties as needed
			builder = builder.withoutPrimaryItem();
			builder = builder.withoutSecondaryItem();
			builder = builder.withoutMetadataTag( CodebookName::Quantity );
			builder = builder.withoutVisVersion();

			if ( !builder.isValid() )
			{
				// Note - builder is now invalid, as a LocalId must have a VIS version,
				// primary item and at least one metadata tag
				// However, it is still possible to access properties and ToString

				return std::nullopt;
			}
			else
			{
				return builder;
			}
		}
	};

	/**
	 * @brief Example usage of the LocalId builder samples
	 * @details Demonstrates all four LocalId builder patterns with error handling
	 */
	void runLocalIdBuilderExamples()
	{
		try
		{
			// Creating new - "/dnv-v2/vis-3-9a/691.83111i/H101/meta/qty-power"
			auto newLocalId = BuildLocalId::buildNew(
				VisVersion::v3_9a,
				// For the sake of this example - only primaryItem and quantity is configured
				"691.83111i/H101",
				"power" );
			std::cout << "Built new LocalId                 : '" << newLocalId.toString() << "'" << std::endl;

			// From existing
			auto fromExisting = BuildLocalId::buildFromExisting( "/dnv-v2/vis-3-9a/691.83111i/H101/meta/qty-power" );
			std::cout << "Built LocalId from existing       : '" << fromExisting.toString() << "'" << std::endl;

			// Verbose mode
			auto verbose = BuildLocalId::buildVerbose( fromExisting );
			std::cout << "Built LocalId (verbose)           : '" << verbose.toString() << "'" << std::endl;

			// Remove properties - always null due to invalidity
			auto maybeBuilder = BuildLocalId::removeProperties( fromExisting );
			if ( maybeBuilder.has_value() )
			{
				auto modifiedLocalId = maybeBuilder->build();
				std::cout << "Modified                          : '" << modifiedLocalId.toString() << "'" << std::endl;
			}
			else
			{
				std::cout << "Built LocalId (without properties): 'Not valid'" << std::endl;
			}
		}
		catch ( const std::exception& e )
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
}

/**
 * @brief Main function to run the LocalId builder examples
 */
int main()
{
	std::cout << "=========== Builder sample - LocalId ===========" << std::endl;

	dnv::vista::sdk::samples::runLocalIdBuilderExamples();

	return 0;
}
