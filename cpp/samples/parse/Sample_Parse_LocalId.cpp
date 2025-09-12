/**
 * @file Sample_Parse_LocalId.cpp
 * @brief Demonstrates parsing complete LocalId URIs using different methods and error handling
 *
 * This sample shows how to:
 * - Parse complete LocalId URIs (e.g., "/dnv-v2/vis-3-9a/691.83111i/H101/meta/qty-power")
 * - Handle parsing errors with exceptions vs. optional return values
 * - Use different parsing methods: parse(), tryParse(), tryParseWithError()
 * - Validate both valid and invalid LocalId formats
 * - Extract detailed error information for debugging
 *
 * Examples include testing various LocalId URI formats from different VIS versions
 * and demonstrating robust error handling patterns for production ISO19848 applications.
 */

#include <iostream>
#include <optional>

#include <dnv/vista/sdk/LocalId.h>

namespace dnv::vista::sdk::samples
{
	/**
	 * @brief Static class for LocalId parsing examples
	 */
	class ParseLocalId
	{
	public:
		/**
		 * @brief Parse a LocalId string
		 * @param localIdString The LocalId string to parse
		 * @return The parsed LocalId
		 * @throws std::exception if parsing fails
		 */
		static LocalId parse( const std::string& localIdString )
		{
			// Simply use the static Parse method (throws on failure)
			auto id = LocalId::parse( localIdString );
			return id;
		}

		/**
		 * @brief Try to parse a LocalId string
		 * @param localIdString The LocalId string to parse
		 * @return Optional LocalId, nullopt if parsing fails
		 */
		static std::optional<LocalId> tryParse( const std::string& localIdString )
		{
			try
			{
				// Use the Parse method with exception handling
				auto id = LocalId::parse( localIdString );
				// Handle success as you please
				return id;
			}
			catch ( [[maybe_unused]] const std::exception& e )
			{
				return std::nullopt;
			}
		}

		/**
		 * @brief Try to parse a LocalId string with error output (C++ enhanced version)
		 * @param localIdString The LocalId string to parse
		 * @param errorMessage Output parameter for error details
		 * @return Optional LocalId, nullopt if parsing fails
		 */
		static std::optional<LocalId> tryParseWithError( const std::string& localIdString, std::string& errorMessage )
		{
			try
			{
				auto id = LocalId::parse( localIdString );
				errorMessage.clear(); // Success - no error
				return id;
			}
			catch ( const std::exception& e )
			{
				errorMessage = e.what(); // Capture the original exception message
				return std::nullopt;
			}
		}
	};

	/**
	 * @brief Example usage of LocalId parsing
	 * @details Demonstrates both Parse and TryParse methods with various LocalId examples
	 */
	void runLocalIdParsingExamples()
	{
		try
		{
			// Valid LocalId examples
			std::vector<std::string> validLocalIds = {
				"/dnv-v2/vis-3-9a/691.83111i/H101/meta/qty-power",
				"/dnv-v2/vis-3-7a/691.811i-A/H101.11-1/meta/qty-power",
				"/dnv-v2/vis-3-9a/411.1-1/C101/meta/cnt-lubricating.oil" };

			// Invalid LocalId examples
			std::vector<std::string> invalidLocalIds = {
				"invalid-localid",
				"/dnv-v2/vis-invalid/691.83111i/H101/meta/qty-power",
				"/dnv-v2/vis-3-9a/invalid-path/meta/qty-power" };

			std::cout << "=== Testing Parse method (throws on failure) ===" << std::endl;

			for ( const auto& localIdString : validLocalIds )
			{
				try
				{
					auto parsedLocalId = ParseLocalId::parse( localIdString );
					std::cout << "Parse SUCCESS                        : '" << localIdString << "' -> '"
							  << parsedLocalId.toString() << "'" << std::endl;
				}
				catch ( const std::exception& e )
				{
					std::cout << "Parse FAILED                         : '" << localIdString << "' -> Error: " << e.what() << std::endl;
				}
			}

			std::cout << "\n=== Testing TryParse method (returns nullopt on failure) ===" << std::endl;

			// Test valid LocalIds
			for ( const auto& localIdString : validLocalIds )
			{
				auto maybeLocalId = ParseLocalId::tryParse( localIdString );
				std::cout << "TryParse SUCCESS                     : '" << localIdString << "' -> '"
						  << ( maybeLocalId.has_value() ? maybeLocalId->toString() : "null" ) << "'" << std::endl;
			}

			// Test invalid LocalIds
			for ( const auto& localIdString : invalidLocalIds )
			{
				auto maybeLocalId = ParseLocalId::tryParse( localIdString );
				std::cout << "TryParse FAILED                      : '" << localIdString << "' -> '"
						  << ( maybeLocalId.has_value() ? maybeLocalId->toString() : "null" ) << "'" << std::endl;
			}

			std::cout << "\n=== Testing TryParseWithError method (captures original exception details) ===" << std::endl;

			// Test invalid LocalIds with detailed error information
			for ( const auto& localIdString : invalidLocalIds )
			{
				std::string errorMessage;
				auto maybeLocalId = ParseLocalId::tryParseWithError( localIdString, errorMessage );
				std::cout << "TryParseWithError FAILED             : '" << localIdString << "' -> '"
						  << ( maybeLocalId.has_value() ? maybeLocalId->toString() : "null" ) << "'" << std::endl;
				if ( !errorMessage.empty() )
				{
					std::cout << "Error: " << errorMessage << std::endl;
				}
			}

			// Test valid LocalIds with error method to show it works for success cases too
			std::cout << "\n=== Testing TryParseWithError method (success cases) ===" << std::endl;
			for ( const auto& localIdString : validLocalIds )
			{
				std::string errorMessage;
				auto maybeLocalId = ParseLocalId::tryParseWithError( localIdString, errorMessage );
				std::cout << "TryParseWithError SUCCESS            : '" << localIdString << "' -> '"
						  << ( maybeLocalId.has_value() ? maybeLocalId->toString() : "null" ) << "'" << std::endl;
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
 * @brief Main function to run the LocalId parsing examples
 */
int main()
{
	std::cout << "=========== Parse sample - LocalId ===========" << std::endl;

	dnv::vista::sdk::samples::runLocalIdParsingExamples();

	return 0;
}
