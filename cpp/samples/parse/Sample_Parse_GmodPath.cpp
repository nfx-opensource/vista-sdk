/**
 * @file Sample_Parse_GmodPath.cpp
 * @brief Demonstrates parsing GMOD path strings using different methods and error handling
 *
 * This sample shows how to:
 * - Parse short GMOD paths (e.g., "691.83111i/H101") using GmodPath::parse()
 * - Parse full hierarchical paths (e.g., "VE/600a/690/691/...") using GmodPath::parseFullPath()
 * - Handle parsing errors with exceptions vs. optional return values
 * - Use different parsing methods: parse(), parseFullPath(), tryParse(), tryParseWithError()
 * - Validate both valid and invalid path formats
 *
 * Examples include testing various path formats and demonstrating robust error handling
 * patterns for production code that needs to parse GMOD path strings reliably.
 */

#include <iostream>
#include <optional>

#include <dnv/vista/sdk/GmodPath.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::samples
{
	/**
	 * @brief Static class for GmodPath parsing examples
	 * @details Converts a GmodPath from one version to another.
	 *          Working with ISO19848, the LocalId::parse() is usually sufficient.
	 *          See Sample_Parse_LocalId for example usage.
	 */
	class ParseGmodPath
	{
	public:
		/**
		 * @brief Parse a GmodPath string
		 * @param gmodPath The GmodPath string to parse
		 * @param visVersion The VIS version to use for parsing
		 * @return The parsed GmodPath
		 * @throws std::exception if parsing fails
		 */
		static GmodPath parse( const std::string& gmodPath, VisVersion visVersion )
		{
			// Simply use the static Parse method (throws on failure)
			auto path = GmodPath::parse( gmodPath, visVersion );
			return path;
		}

		/**
		 * @brief Parse a full GmodPath string
		 * @param fullPath The full GmodPath string to parse
		 * @param visVersion The VIS version to use for parsing
		 * @return The parsed GmodPath
		 * @throws std::exception if parsing fails
		 */
		static GmodPath parseFullPath( const std::string& fullPath, VisVersion visVersion )
		{
			// Simply use the static ParseFullPath method (throws on failure)
			auto path = GmodPath::parseFullPath( fullPath, visVersion );
			return path;
		}

		/**
		 * @brief Try to parse a GmodPath string
		 * @param gmodPath The GmodPath string to parse
		 * @param visVersion The VIS version to use for parsing
		 * @return Optional GmodPath, nullopt if parsing fails
		 */
		static std::optional<GmodPath> tryParse( const std::string& gmodPath, VisVersion visVersion )
		{
			try
			{
				// Use the Parse method with exception handling
				auto path = GmodPath::parse( gmodPath, visVersion );
				// Handle result as you please
				return path;
			}
			catch ( const std::exception& )
			{
				// Handle errors as you please
				return std::nullopt;
			}
		}

		/**
		 * @brief Try to parse a full GmodPath string
		 * @param fullPath The full GmodPath string to parse
		 * @param visVersion The VIS version to use for parsing
		 * @return Optional GmodPath, nullopt if parsing fails
		 */
		static std::optional<GmodPath> tryParseFullPath( const std::string& fullPath, VisVersion visVersion )
		{
			try
			{
				// Use the ParseFullPath method with exception handling
				auto path = GmodPath::parseFullPath( fullPath, visVersion );
				// Handle result as you please
				return path;
			}
			catch ( const std::exception& )
			{
				// Handle errors as you please
				return std::nullopt;
			}
		}

		/**
		 * @brief Try to parse a GmodPath string with error output (C++ enhanced version)
		 * @param gmodPath The GmodPath string to parse
		 * @param visVersion The VIS version to use for parsing
		 * @param errorMessage Output parameter for error details
		 * @return Optional GmodPath, nullopt if parsing fails
		 */
		static std::optional<GmodPath> tryParseWithError( const std::string& gmodPath, VisVersion visVersion, std::string& errorMessage )
		{
			try
			{
				auto path = GmodPath::parse( gmodPath, visVersion );
				errorMessage.clear(); // Success - no error
				return path;
			}
			catch ( const std::exception& e )
			{
				errorMessage = e.what(); // Capture the original exception message
				return std::nullopt;
			}
		}
	};

	/**
	 * @brief Example usage of GmodPath parsing
	 * @details Demonstrates all four GmodPath parsing methods with various examples
	 */
	void runGmodPathParsingExamples()
	{
		try
		{
			auto visVersion = VisVersion::v3_9a;

			// Valid GmodPath examples
			std::vector<std::string> validGmodPaths = {
				"691.83111i/H101",
				"691.811i-A/H101.11-1",
				"411.1-1/C101" };

			// Valid full path examples
			std::vector<std::string> validFullPaths = {
				"VE/600a/690/691/691.5/691.51/691.51s/691.511/691.511i/HS1/H121",
				"VE/1000a/1010/1014/1014.2/1014.21/1014.21i-3S/1014.211-3S/S110/S110.1",
				"VE/1000a/1010/1014/1014.2/1014.21/1014.21i-3S/1014.211-3S/S110/S110.1/S101",
				"VE/1000a/1010/1014/1014.2/1014.21/1014.21i-3S/1014.211-3S/S110/S110.2",
				"VE/1000a/1010/1014/1014.2/1014.21/1014.21i-3S/1014.211-3S/S110/S110.2/CS1",
				"VE/1000a/1010/1014/1014.2/1014.21/1014.21i-3S/1014.211-3S/S110/S110.2/CS1/E31",
			};

			// Invalid full path examples (for ParseFullPath - don't start with VE)
			std::vector<std::string> invalidFullPaths = {
				"/dnv-v2/vis-3-9a/691.83111i/H101",
				"/dnv-v2/vis-3-7a/691.811i-A/H101.11-1",
				"/dnv-v2/vis-3-9a/411.1-1/C101" };

			// Invalid GmodPath examples
			std::vector<std::string> invalidGmodPaths = {
				"invalid-path",
				"999.invalid/X999",
				"691.83111i/INVALID" };

			std::cout << "=== Testing Parse method (throws on failure) ===" << std::endl;

			for ( const auto& gmodPath : validGmodPaths )
			{
				try
				{
					auto parsedPath = ParseGmodPath::parse( gmodPath, visVersion );
					std::cout << "Parse SUCCESS                        : '" << gmodPath << "' -> '"
							  << parsedPath.toString() << "'" << std::endl;
				}
				catch ( const std::exception& e )
				{
					std::cout << "Parse FAILED                         : '" << gmodPath << "' -> Error: " << e.what() << std::endl;
				}
			}

			std::cout << "\n=== Testing ParseFullPath method (throws on failure) ===" << std::endl;

			for ( const auto& fullPath : validFullPaths )
			{
				try
				{
					auto parsedPath = ParseGmodPath::parseFullPath( fullPath, visVersion );
					std::cout << "ParseFullPath SUCCESS                : '" << fullPath << "' -> '"
							  << parsedPath.toString() << "'" << std::endl;
				}
				catch ( const std::exception& e )
				{
					std::cout << "ParseFullPath FAILED                 : '" << fullPath << "' -> Error: " << e.what() << std::endl;
				}
			}

			// Test invalid full paths (should fail)
			for ( const auto& fullPath : invalidFullPaths )
			{
				try
				{
					auto parsedPath = ParseGmodPath::parseFullPath( fullPath, visVersion );
					std::cout << "ParseFullPath UNEXPECTED SUCCESS     : '" << fullPath << "' -> '"
							  << parsedPath.toString() << "'" << std::endl;
				}
				catch ( const std::exception& e )
				{
					std::cout << "ParseFullPath EXPECTED FAIL          : '" << fullPath << "' -> Error: " << e.what() << std::endl;
				}
			}

			std::cout << "\n=== Testing TryParse method (returns nullopt on failure) ===" << std::endl;

			// Test valid GmodPaths
			for ( const auto& gmodPath : validGmodPaths )
			{
				auto maybePath = ParseGmodPath::tryParse( gmodPath, visVersion );
				std::cout << "TryParse SUCCESS                     : '" << gmodPath << "' -> '"
						  << ( maybePath.has_value() ? maybePath->toString() : "null" ) << "'" << std::endl;
			}

			// Test invalid GmodPaths
			for ( const auto& gmodPath : invalidGmodPaths )
			{
				auto maybePath = ParseGmodPath::tryParse( gmodPath, visVersion );
				std::cout << "TryParse FAILED                      : '" << gmodPath << "' -> '"
						  << ( maybePath.has_value() ? maybePath->toString() : "null" ) << "'" << std::endl;
			}

			std::cout << "\n=== Testing TryParseFullPath method (returns nullopt on failure) ===" << std::endl;

			for ( const auto& fullPath : validFullPaths )
			{
				auto maybePath = ParseGmodPath::tryParseFullPath( fullPath, visVersion );
				std::cout << "TryParseFullPath SUCCESS             : '" << fullPath << "' -> '"
						  << ( maybePath.has_value() ? maybePath->toString() : "null" ) << "'" << std::endl;
			}

			// Test invalid full paths
			for ( const auto& fullPath : invalidFullPaths )
			{
				auto maybePath = ParseGmodPath::tryParseFullPath( fullPath, visVersion );
				std::cout << "TryParseFullPath FAILED              : '" << fullPath << "' -> '"
						  << ( maybePath.has_value() ? maybePath->toString() : "null" ) << "'" << std::endl;
			}

			std::cout << "\n=== Testing TryParseWithError method (captures original exception details) ===" << std::endl;

			// Test invalid GmodPaths with detailed error information
			for ( const auto& gmodPath : invalidGmodPaths )
			{
				std::string errorMessage;
				auto maybePath = ParseGmodPath::tryParseWithError( gmodPath, visVersion, errorMessage );
				std::cout << "TryParseWithError FAILED             : '" << gmodPath << "' -> '"
						  << ( maybePath.has_value() ? maybePath->toString() : "null" ) << "'" << std::endl;
				if ( !errorMessage.empty() )
				{
					std::cout << "Error: " << errorMessage << std::endl;
				}
			}
		}
		catch ( const std::exception& e )
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
}

/**
 * @brief Main function to run the GmodPath parsing examples
 */
int main()
{
	std::cout << "=========== Parse sample - GmodPath ===========" << std::endl;

	dnv::vista::sdk::samples::runGmodPathParsingExamples();

	return 0;
}
