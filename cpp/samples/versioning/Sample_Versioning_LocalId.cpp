/**
 * @file Sample_Versioning_LocalId.cpp
 * @brief LocalId versioning usage examples for Vista SDK C++
 */

#include <iostream>
#include <optional>

#include <dnv/vista/sdk/LocalId.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::samples
{
	/**
	 * @brief Static class for LocalId versioning examples
	 * @details Converts a LocalId from one version to another.
	 */
	class ConvertLocalId
	{
	public:
		static std::optional<LocalId> convert( const std::string& localId, VisVersion targetVersion )
		{
			// Parse input path into domain model
			auto source = LocalId::parse( localId );
			// Convert LocalId to target VIS version
			auto target = VIS::instance().convertLocalId( source, targetVersion );
			return target;
		}
	};

	/**
	 * @brief Run LocalId versioning examples
	 * @details Demonstrates converting LocalIds between different VIS versions
	 */
	void runLocalIdVersioningExamples()
	{
		std::cout << "=========== Versioning sample - LocalId ===========" << std::endl;

		auto targetVersion = VisVersion::v3_9a;

		// LocalId
		std::string sampleLocalId = "/dnv-v2/vis-3-7a/691.811i/H101/meta/qty-power";
		auto convertedLocalId = ConvertLocalId::convert( sampleLocalId, targetVersion );

		if ( convertedLocalId.has_value() )
		{
			std::cout << "Converted LocalId: Source='" << sampleLocalId << "' Target='" << convertedLocalId->toString() << "'" << std::endl;
		}
		else
		{
			std::cout << "Converted LocalId: Source='" << sampleLocalId << "' Target='(conversion failed)'" << std::endl;
		}
	}
}

/**
 * @brief Main function to run the LocalId versioning examples
 */
int main()
{
	dnv::vista::sdk::samples::runLocalIdVersioningExamples();

	return 0;
}
