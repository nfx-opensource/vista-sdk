/**
 * @file Sample_Versioning_GmodPath.cpp
 * @brief Demonstrates converting GMOD paths between different VIS versions
 *
 * This sample shows how to:
 * - Parse a GMOD path string in one VIS version (e.g., "691.811i/H101" in VIS 3.7a)
 * - Convert the entire path to another VIS version (e.g., "691.83111i/H101" in VIS 3.9a)
 * - Handle conversion failures when paths don't exist in target versions
 * - Use VIS::convertPath() API for complete path migration
 *
 * Example: Converts path "691.811i/H101" from VIS 3.7a to "691.83111i/H101" in VIS 3.9a
 * This is essential for migrating hierarchical GMOD references between VIS versions.
 */

#include <iostream>
#include <optional>

#include <dnv/vista/sdk/GmodPath.h>
#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/VISVersion.h>

namespace dnv::vista::sdk::samples
{
	/**
	 * @brief Static class for GmodPath versioning examples
	 * @details Converts a GmodPath from one version to another.
	 *          Working with ISO19848, the VIS::convertLocalId() is usually sufficient.
	 *          See Sample_Versioning_LocalId for example usage.
	 */
	class ConvertGmodPath
	{
	public:
		static std::optional<GmodPath> convert( const std::string& gmodPath, VisVersion sourceVersion, VisVersion targetVersion )
		{
			// Parse input path into domain model
			auto source = GmodPath::parse( gmodPath, sourceVersion );
			// Convert GmodPath to target VIS version
			auto target = VIS::instance().convertPath( source, targetVersion );
			return target;
		}
	};

	/**
	 * @brief Run GmodPath versioning examples
	 * @details Demonstrates converting GmodPaths between different VIS versions
	 */
	void runGmodPathVersioningExamples()
	{
		std::cout << "=========== Versioning sample - Expects 691.811i -> 691.83111i ===========" << std::endl;

		auto sourceVisVersion = VisVersion::v3_7a;
		auto targetVisVersion = VisVersion::v3_9a;

		// GmodPath
		std::string sampleGmodPath = "691.811i/H101";
		auto convertedGmodPath = ConvertGmodPath::convert( sampleGmodPath, sourceVisVersion, targetVisVersion );

		if ( convertedGmodPath.has_value() )
		{
			std::cout << "Converted GmodPath: Source='" << sampleGmodPath << "' Target='" << convertedGmodPath->toString() << "'" << std::endl;
		}
		else
		{
			std::cout << "Converted GmodPath: Source='" << sampleGmodPath << "' Target='(conversion failed)'" << std::endl;
		}
	}
} // namespace dnv::vista::sdk::samples

/**
 * @brief Main function to run the GmodPath versioning examples
 */
int main()
{
	std::cout << "=========== Versioning sample - GmodPath ===========" << std::endl;

	dnv::vista::sdk::samples::runGmodPathVersioningExamples();

	return 0;
}
