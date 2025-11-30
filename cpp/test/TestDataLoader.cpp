/**
 * @file TestDataLoader.cpp
 * @brief Helper utilities for loading test data from JSON files.
 */
#include <fstream>
#include <filesystem>

#include <nfx/Containers.h>
#include <nfx/serialization/json/Document.h>

#include "TestDataLoader.h"

namespace dnv::vista::sdk::test
{
	namespace
	{
		//=====================================================================
		// Testdata directory configuration
		//=====================================================================

		/**
		 * @brief Get the configured testdata directory path
		 * @return The testdata directory path to use for loading resources
		 */
		static std::filesystem::path testdataDirectory()
		{
#ifdef VISTA_SDK_TESTDATA_DIR
			// Use compile-time configured testdata directory
			return std::filesystem::path{ VISTA_SDK_TESTDATA_DIR };
#else
			// Default: testdata subdirectory relative to current working directory
			return std::filesystem::current_path() / "testdata";
#endif
		}

		//=====================================================================
		// Test data cache
		//=====================================================================

		static nfx::containers::FastHashMap<std::string, nfx::serialization::json::Document> g_testDataCache;
	} // namespace

	const nfx::serialization::json::Document& loadTestData( const char* testDataPath )
	{
		const auto fullPath = testdataDirectory() / testDataPath;
		const std::string fullPathStr = fullPath.string();

		// Check if already cached
		const auto* cachedDataPtr = g_testDataCache.find( fullPathStr );
		if ( cachedDataPtr != nullptr )
		{
			return *cachedDataPtr;
		}

		// Load the data
		std::ifstream jsonFile( fullPath );
		if ( !jsonFile.is_open() )
		{
			throw std::runtime_error{ std::string{ "Failed to open test data file: " } + fullPathStr };
		}

		try
		{
			std::string jsonContent( ( std::istreambuf_iterator<char>( jsonFile ) ),
				std::istreambuf_iterator<char>() );

			auto docOpt = nfx::serialization::json::Document::fromString( jsonContent );
			if ( !docOpt )
			{
				throw std::runtime_error{ "Failed to parse JSON from file: " + fullPathStr };
			}

			g_testDataCache.insertOrAssign( fullPathStr, std::move( *docOpt ) );

			const auto* result = g_testDataCache.find( fullPathStr );
			if ( result != nullptr )
			{
				return *result;
			}
			throw std::runtime_error{ "Failed to retrieve cached test data after insertion" };
		}
		catch ( const std::runtime_error& )
		{
			throw;
		}
		catch ( const std::exception& ex )
		{
			throw std::runtime_error{ std::string{ "Error parsing JSON file '" } + fullPathStr + "': " + ex.what() };
		}
	}
} // namespace dnv::vista::sdk::test
