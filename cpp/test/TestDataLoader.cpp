/**
 * @file TestDataLoader.cpp
 * @brief Helper utilities for loading test data from JSON files.
 */
#include <fstream>
#include <filesystem>

#include <nfx/containers/HashMap.h>

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
		static nfx::containers::HashMap<std::string, nlohmann::json> g_testDataCache;
	}

	const nlohmann::json& loadTestData( const char* testDataPath )
	{
		const auto fullPath = testdataDirectory() / testDataPath;
		const std::string fullPathStr = fullPath.string();

		const nlohmann::json* cachedData = g_testDataCache.tryGetValue( fullPathStr );
		if ( cachedData )
		{
			return *cachedData;
		}

		std::ifstream jsonFile( fullPath );
		if ( !jsonFile.is_open() )
		{
			throw std::runtime_error{ std::string{ "Failed to open test data file: " } + fullPathStr };
		}

		try
		{
			nlohmann::json data;
			jsonFile >> data;

			g_testDataCache.insertOrAssign( fullPathStr, std::move( data ) );

			const nlohmann::json* result = g_testDataCache.tryGetValue( fullPathStr );
			if ( result )
			{
				return *result;
			}

			throw std::runtime_error{ "Failed to retrieve cached test data after insertion" };
		}
		catch ( const nlohmann::json::parse_error& ex )
		{
			throw std::runtime_error{
				std::string{ "JSON parse error in '" } + fullPathStr +
				"'. Type: " + std::to_string( ex.id ) +
				", Byte: " + std::to_string( ex.byte ) +
				". Original what() likely too long." };
		}
	}
}
