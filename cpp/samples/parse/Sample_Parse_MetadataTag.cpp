/**
 * @file Sample_Parse_MetadataTag.cpp
 * @brief Demonstrates parsing and creating metadata tags from different VIS codebooks
 *
 * This sample shows how to:
 * - Create MetadataTag objects from string values using specific codebooks
 * - Work with different codebook types (Quantity, Content, Position, etc.)
 * - Handle both standard tags ("power", "lubricating.oil") and custom tags
 * - Detect whether a tag is custom or from the official VIS codebook
 * - Use VIS::codebooks() API to access codebook functionality
 *
 * Examples include parsing tags like "power" (quantity), "lubricating.oil" (content),
 * "inlet" (position), and custom tags, showing how to validate metadata components
 * of LocalId URIs against VIS codebooks.
 */

#include <iostream>
#include <optional>

#include <dnv/vista/sdk/MetadataTag.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::samples
{
	/**
	 * @brief Parse a metadata tag using codebook and VIS version
	 * @param tag The tag string to parse
	 * @param codebook The codebook name to use for parsing
	 * @param visVersion The VIS version to use
	 * @return Optional MetadataTag, nullopt if parsing fails
	 */
	static std::optional<MetadataTag> parseMetadataTag( const std::string& tag, CodebookName codebook, VisVersion visVersion )
	{
		try
		{
			// Need to create a metadata tag in scope of a codebook
			auto& vis = VIS::instance();
			const auto& codebooks = vis.codebooks( visVersion );

			auto metadataTag = codebooks.tryCreateTag( codebook, tag );
			if ( !metadataTag.has_value() )
			{
				// Handle the case where the metadata tag could not be created
				return std::nullopt;
			}
			return metadataTag;
		}
		catch ( const std::exception& )
		{
			return std::nullopt;
		}
	}

	/**
	 * @brief Example usage of MetadataTag parsing
	 * @details Demonstrates parsing MetadataTag with different codebooks and values
	 */
	void runMetadataTagParsingExamples()
	{
		try
		{
			auto visVersion = VisVersion::v3_9a;

			// Metadata tag examples
			auto sampleMetadataTag = "power";
			auto sampleCodebook = CodebookName::Quantity;
			auto parsedMetadataTag = parseMetadataTag( sampleMetadataTag, sampleCodebook, visVersion );

			std::cout << "Parsed MetadataTag                   : '" << sampleMetadataTag << "' -> '"
					  << ( parsedMetadataTag.has_value() ? parsedMetadataTag->toString() : "null" ) << "' (isCustom: "
					  << ( parsedMetadataTag.has_value() ? ( parsedMetadataTag->isCustom() ? "TRUE" : "FALSE" ) : "N/A" ) << ")" << std::endl;

			// Additional examples with different codebooks
			auto contentTag = "lubricating.oil";
			auto contentCodebook = CodebookName::Content;
			auto parsedContentTag = parseMetadataTag( contentTag, contentCodebook, visVersion );

			std::cout << "Parsed Content MetadataTag           : '" << contentTag << "' -> '"
					  << ( parsedContentTag.has_value() ? parsedContentTag->toString() : "null" ) << "' (isCustom: "
					  << ( parsedContentTag.has_value() ? ( parsedContentTag->isCustom() ? "TRUE" : "FALSE" ) : "N/A" ) << ")" << std::endl;

			// Example with position
			auto positionTag = "inlet";
			auto positionCodebook = CodebookName::Position;
			auto parsedPositionTag = parseMetadataTag( positionTag, positionCodebook, visVersion );

			std::cout << "Parsed Position MetadataTag          : '" << positionTag << "' -> '"
					  << ( parsedPositionTag.has_value() ? parsedPositionTag->toString() : "null" ) << "' (isCustom: "
					  << ( parsedPositionTag.has_value() ? ( parsedPositionTag->isCustom() ? "TRUE" : "FALSE" ) : "N/A" ) << ")" << std::endl;

			// Example with invalid tag
			auto customTag = "custom-tag-name";
			auto customParsedTag = parseMetadataTag( customTag, CodebookName::Quantity, visVersion );

			std::cout << "Parsed Custom MetadataTag            : '" << customTag << "' -> '"
					  << ( customParsedTag.has_value() ? customParsedTag->toString() : "null" ) << "' (isCustom: "
					  << ( customParsedTag.has_value() ? ( customParsedTag->isCustom() ? "TRUE" : "FALSE" ) : "N/A" ) << ")" << std::endl;
		}
		catch ( const std::exception& e )
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
} // namespace dnv::vista::sdk::samples

/**
 * @brief Main function to run the MetadataTag parsing examples
 */
int main()
{
	std::cout << "=========== Parse sample - MetadataTag ===========" << std::endl;

	dnv::vista::sdk::samples::runMetadataTagParsingExamples();

	return 0;
}
