/**
 * @file Sample_Versioning_GmodNode.cpp
 * @brief Demonstrates converting GMOD nodes between different VIS versions
 *
 * This sample shows how to:
 * - Convert a GMOD node code from one VIS version to another (e.g., VIS 3.7a to 3.9a)
 * - Handle conversion failures when nodes don't exist in target versions
 * - Use VIS::convertNode() API for GMOD node version migration
 *
 * Example: Converts node "691.81" from VIS 3.7a to "691.8311" in VIS 3.9a
 * This is useful when migrating data or supporting multiple VIS versions.
 */

#include <iostream>
#include <optional>

#include <dnv/vista/sdk/GmodNode.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::samples
{
	/**
	 * @brief Static class for GmodNode versioning examples
	 * @details Converts a GmodNode from one version to another.
	 *          Working with ISO19848, the VIS::convertLocalId() is usually sufficient.
	 *          See Sample_Versioning_LocalId for example usage.
	 */
	class ConvertGmodNode
	{
	public:
		static std::optional<GmodNode> convert( const std::string& gmodNode, VisVersion sourceVersion, VisVersion targetVersion )
		{
			// Get Gmod for source version to get source GmodNode
			auto gmod = VIS::instance().gmod( sourceVersion );

			// Get source GmodNode
			const GmodNode* sourceNodePtr = nullptr;
			if ( !gmod.tryGetNode( gmodNode, sourceNodePtr ) || !sourceNodePtr )
			{
				return std::nullopt;
			}

			// Convert GmodNode to target VIS version
			auto target = VIS::instance().convertNode( *sourceNodePtr, targetVersion );
			return target;
		}
	};

	/**
	 * @brief Run GmodNode versioning examples
	 * @details Demonstrates converting GmodNodes between different VIS versions
	 */
	void runGmodNodeVersioningExamples()
	{
		std::cout << "=========== Versioning sample - Expects 691.81 -> 691.8311 ===========" << std::endl;

		auto sourceVisVersion = VisVersion::v3_7a;
		auto targetVisVersion = VisVersion::v3_9a;

		// GmodNode
		std::string sampleGmodNode = "691.81";
		auto convertedGmod = ConvertGmodNode::convert( sampleGmodNode, sourceVisVersion, targetVisVersion );

		if ( convertedGmod.has_value() )
		{
			std::cout << "Converted GmodNode: Source='" << sampleGmodNode << "' Target='" << convertedGmod->code() << "'" << std::endl;
		}
		else
		{
			std::cout << "Converted GmodNode: Source='" << sampleGmodNode << "' Target='(conversion failed)'" << std::endl;
		}
	}
}

/**
 * @brief Main function to run the GmodNode versioning examples
 */
int main()
{
	dnv::vista::sdk::samples::runGmodNodeVersioningExamples();

	return 0;
}
