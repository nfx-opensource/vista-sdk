/**
 * @file Sample_LocalId_Validator.cpp
 * @brief Local ID Validator and Analyzer for Vista SDK C++
 *
 * This utility validates and analyzes Vista Local IDs:
 * - Parses Local IDs from command line or file
 * - Validates Local ID syntax and structure
 * - Shows detailed analysis of each Local ID component
 * - Supports verbose mode for detailed output
 * - Provides success/failure statistics
 *
 * Usage examples:
 * - Validate single Local ID: validator "/dnv-v2/vis-3-9a/710.1/F211.11/meta/qty-position"
 * - Validate from file: validator -f localids.txt
 * - Verbose mode: validator -f localids.txt -v
 */

#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

#include <dnv/vista/sdk/CodebookName.h>
#include <dnv/vista/sdk/LocalId.h>
#include <dnv/vista/sdk/LocalIdBuilder.h>
#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/VISVersion.h>

std::vector<std::string> loadLocalIdsFromFile( const std::string& filename )
{
	std::vector<std::string> localIds;
	std::ifstream file( filename );

	if ( !file.is_open() )
	{
		std::cerr << "Error: Could not open file '" << filename << "'\n";
		return localIds;
	}

	std::string line;
	while ( std::getline( file, line ) )
	{
		line.erase( 0, line.find_first_not_of( " \t\r\n" ) );
		line.erase( line.find_last_not_of( " \t\r\n" ) + 1 );

		if ( !line.empty() && line[0] != '#' )
		{
			localIds.push_back( line );
		}
	}

	std::cout << "Loaded " << localIds.size() << " Local IDs from file: " << filename << "\n";
	return localIds;
}

using namespace dnv::vista::sdk;

void printUsage( const char* programName )
{
	std::cout << "Usage: " << programName << " [options] [local-id1] [local-id2] ...\n";
	std::cout << "Options:\n";
	std::cout << "  -f, --file <filename>    Load Local IDs from file (one per line)\n";
	std::cout << "  -h, --help              Show this help message\n";
	std::cout << "  -v, --verbose           Enable verbose output\n";
	std::cout << "\nExamples:\n";
	std::cout << "  " << programName << " \"/dnv-v2/vis-3-9a/710.1/F211.11/meta/qty-position\"\n";
	std::cout << "  " << programName << " -f localids.txt\n";
	std::cout << "  " << programName << " -f localids.txt \"/dnv-v2/vis-3-9a/710.1/F211/meta/qty-speed.through.water\"\n";
}

int main( int argc, char* argv[] )
{
	try
	{
		std::vector<std::string> localIds;
		bool verboseMode = false;
		std::string inputFile;

		for ( int i = 1; i < argc; ++i )
		{
			std::string arg = argv[i];

			if ( arg == "-h" || arg == "--help" )
			{
				printUsage( argv[0] );
				return 0;
			}
			else if ( arg == "-v" || arg == "--verbose" )
			{
				verboseMode = true;
			}
			else if ( arg == "-f" || arg == "--file" )
			{
				if ( i + 1 < argc )
				{
					inputFile = argv[++i];
				}
				else
				{
					std::cerr << "Error: -f option requires a filename\n";
					printUsage( argv[0] );
					return 1;
				}
			}
			else if ( arg.substr( 0, 1 ) == "-" )
			{
				std::cerr << "Error: Unknown option '" << arg << "'\n";
				printUsage( argv[0] );
				return 1;
			}
			else
			{
				localIds.push_back( arg );
			}
		}

		if ( !inputFile.empty() )
		{
			auto fileLocalIds = loadLocalIdsFromFile( inputFile );
			localIds.insert( localIds.end(), fileLocalIds.begin(), fileLocalIds.end() );
		}

		if ( localIds.empty() )
		{
			std::cout << "No Local IDs provided. Use -f to load from file or provide Local IDs as arguments.\n\n";
			printUsage( argv[0] );
			return 1;
		}

		auto version = VisVersion::v3_9a;

		std::cout << "=== Local ID Analysis ===\n\n";
		std::cout << "Using VIS instance with codebooks for version: "
				  << VisVersionExtensions::toVersionString( version ) << "\n";
		std::cout << "Testing " << localIds.size() << " Local IDs\n\n";

		size_t successCount = 0;
		size_t failCount = 0;

		for ( const auto& localIdStr : localIds )
		{
			std::cout << "\n\tAnalyzing: " << localIdStr << "\n";

			try
			{
				auto localId = LocalId::parse( localIdStr );

				std::cout << "\t\t[OK] Successfully parsed and built\n";
				std::cout << "\t\t[OK] VIS Version: " << VisVersionExtensions::toVersionString( localId.visVersion() ) << "\n";
				std::cout << "\t\t[OK] Has custom tags: " << ( localId.hasCustomTag() ? "true" : "false" ) << "\n";

				if ( localId.primaryItem().has_value() )
				{
					std::cout << "\t\t[OK] Primary item: " << localId.primaryItem().value().toString() << "\n";
				}

				if ( localId.secondaryItem().has_value() )
				{
					std::cout << "\t\t[OK] Secondary item: " << localId.secondaryItem().value().toString() << "\n";
				}

				std::cout << "\t\t[OK] Metadata tags:\n";
				for ( const auto& tag : localId.metadataTags() )
				{
					std::cout << "\t\t\t- " << CodebookNames::toPrefix( tag.name() ) << ": " << tag.value()
							  << " (custom: " << ( tag.isCustom() ? "true" : "false" ) << ")\n";
				}

				if ( verboseMode && localId.primaryItem().has_value() )
				{
					try
					{
						auto primaryItemCopy1 = localId.primaryItem().value();
						auto primaryItemCopy2 = localId.primaryItem().value();

						auto verboseBuilder = LocalIdBuilder::create( version )
												  .withVerboseMode( true )
												  .withPrimaryItem( std::move( primaryItemCopy1 ) );

						if ( localId.secondaryItem().has_value() )
						{
							auto secondaryItemCopy1 = localId.secondaryItem().value();
							verboseBuilder = verboseBuilder.withSecondaryItem( std::move( secondaryItemCopy1 ) );
						}

						for ( const auto& tag : localId.metadataTags() )
						{
							verboseBuilder = verboseBuilder.withMetadataTag( tag );
						}

						auto verboseLocalId = verboseBuilder.build();

						auto regularBuilder = LocalIdBuilder::create( version )
												  .withVerboseMode( false )
												  .withPrimaryItem( std::move( primaryItemCopy2 ) );

						if ( localId.secondaryItem().has_value() )
						{
							auto secondaryItemCopy2 = localId.secondaryItem().value();
							regularBuilder = regularBuilder.withSecondaryItem( std::move( secondaryItemCopy2 ) );
						}

						for ( const auto& tag : localId.metadataTags() )
						{
							regularBuilder = regularBuilder.withMetadataTag( tag );
						}

						auto regularLocalId = regularBuilder.build();

						std::cout << "\t\t[OK] Verbose mode: " << verboseLocalId.toString() << "\n";
						std::cout << "\t\t[OK] Regular mode: " << regularLocalId.toString() << "\n";
					}
					catch ( const std::exception& builderEx )
					{
						std::cout << "\t\t[WARN] Verbose mode demonstration failed: " << builderEx.what() << "\n";
					}
				}

				successCount++;
			}
			catch ( const std::exception& e )
			{
				std::cout << "\t\t[FAIL] Failed to parse: " << e.what() << "\n";
				failCount++;
			}
		}

		std::cout << "\n=== Summary ===\n";
		std::cout << "Total Local IDs tested: " << localIds.size() << "\n";
		std::cout << "Successfully parsed: " << successCount << "\n";
		std::cout << "Failed to parse: " << failCount << "\n";
		std::cout << "Success rate: " << ( static_cast<double>( successCount ) / static_cast<double>( localIds.size() ) * 100.0 ) << "%\n";

		std::cout << "\n=== Local ID Analysis completed! ===\n";
		return 0;
	}
	catch ( const std::exception& ex )
	{
		std::cerr << "Error: " << ex.what() << "\n";
		return 1;
	}
}
