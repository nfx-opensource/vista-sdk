/**
 * @file json-validator-cli.cpp
 * @brief CLI tool for validating ISO 19848 JSON files against schemas
 * @details Command-line interface to validate DataChannelList and TimeSeriesData JSON files
 *
 * Usage:
 *   json-validator-cli datachannel <file.json>    # Validate DataChannelList
 *   json-validator-cli timeseries <file.json>     # Validate TimeSeriesData
 *   json-validator-cli --list-schemas             # List available schemas
 *   json-validator-cli --help                     # Show help
 */

#include <EmbeddedSchemas/EmbeddedSchemas.h>

#include <nfx/json/SchemaValidator.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

// Forward declarations
void printHelp();
std::optional<std::string_view> findSchema( std::string_view schemaName );
bool validateJson( const std::filesystem::path& jsonFile, std::string_view schemaName );

int main( int argc, char* argv[] )
{
    if( argc < 2 )
    {
        printHelp();
        return 1;
    }

    std::string_view command = argv[1];

    if( command == "--help" || command == "-h" )
    {
        printHelp();
        return 0;
    }

    if( command == "datachannel" || command == "timeseries" )
    {
        if( argc < 3 )
        {
            std::cerr << "Error: Missing JSON file argument\n\n";
            printHelp();
            return 1;
        }

        std::filesystem::path jsonFile = argv[2];

        if( !std::filesystem::exists( jsonFile ) )
        {
            std::cerr << "Error: File not found: " << jsonFile << "\n";
            return 1;
        }

        std::string_view schemaName =
            ( command == "datachannel" ) ? "DataChannelList.schema.json" : "TimeSeriesData.schema.json";

        bool valid = validateJson( jsonFile, schemaName );
        return valid ? 0 : 1;
    }

    std::cerr << "Error: Unknown command '" << command << "'\n\n";
    printHelp();
    return 1;
}

void printHelp()
{
    std::cout << R"(
Vista SDK Validator - JSON Schema Validation Tool

USAGE:
    json-validator-cli <command> <file.json>

COMMANDS:
    datachannel <file.json>     Validate DataChannelList JSON file
    timeseries <file.json>      Validate TimeSeriesData JSON file
    --help, -h                  Show this help message

EXAMPLES:
    json-validator-cli datachannel DataChannelList.sample.json
    json-validator-cli timeseries TimeSeriesData.sample.json

DESCRIPTION:
    Validates ISO 19848 JSON files against their official JSON schemas.

EXIT CODES:
    0    Validation succeeded
    1    Validation failed or error occurred
)";
}

std::optional<std::string_view> findSchema( std::string_view schemaName )
{
    return dnv::vista::sdk::EmbeddedSchema::get( schemaName );
}

bool validateJson( const std::filesystem::path& jsonFile, std::string_view schemaName )
{
    std::cout << "Validating: " << jsonFile.filename() << "\n";
    std::cout << "Schema: " << schemaName << "\n\n";

    // Load embedded schema
    auto schemaDataOpt = findSchema( schemaName );
    if( !schemaDataOpt.has_value() )
    {
        std::cerr << "Error: Schema not found: " << schemaName << "\n";
        std::cerr << "Use --list-schemas to see available schemas\n";
        return false;
    }

    // Parse schema document
    auto schemaDocOpt = nfx::json::Document::fromString( *schemaDataOpt );
    if( !schemaDocOpt.has_value() )
    {
        std::cerr << "Error: Failed to parse embedded schema\n";
        return false;
    }

    // Load schema into validator
    nfx::json::SchemaValidator validator;
    if( !validator.load( *schemaDocOpt ) )
    {
        std::cerr << "Error: Failed to load schema into validator\n";
        return false;
    }

    // Load and parse JSON file
    std::ifstream file( jsonFile );
    if( !file )
    {
        std::cerr << "Error: Failed to open file: " << jsonFile << "\n";
        return false;
    }

    std::string jsonContent( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );

    auto documentOpt = nfx::json::Document::fromString( jsonContent );
    if( !documentOpt.has_value() )
    {
        std::cerr << "[ERROR] JSON Parse Error:\n";
        std::cerr << "        Failed to parse " << jsonFile.filename() << "\n";
        return false;
    }

    // Validate document against schema
    auto result = validator.validate( *documentOpt );

    if( result.isValid() )
    {
        std::cout << "[OK] Validation succeeded\n";
        std::cout << "     File conforms to " << schemaName << "\n";
        std::cout << "     Schema: " << validator.title() << " (" << validator.draftString() << ")\n";
        return true;
    }
    else
    {
        std::cerr << "[FAIL] Validation Failed (" << result.errorCount() << " errors):\n\n";

        for( size_t i = 0; i < result.errorCount(); ++i )
        {
            const auto& error = result.error( i );
            std::cerr << "  [" << ( i + 1 ) << "] " << error.path() << "\n";
            std::cerr << "      " << error.message() << "\n";

            if( !error.constraint().empty() )
            {
                std::cerr << "      Constraint: " << error.constraint() << "\n";
            }
            if( !error.expectedValue().empty() )
            {
                std::cerr << "      Expected: " << error.expectedValue() << "\n";
            }
            if( !error.actualValue().empty() )
            {
                std::cerr << "      Actual: " << error.actualValue() << "\n";
            }
            std::cerr << "\n";
        }

        return false;
    }
}
