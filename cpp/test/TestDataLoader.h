/**
 * @file TestDataLoader.h
 * @brief Helper utilities for loading test data from JSON files.
 */

#pragma once

#include <nfx/serialization/json/Document.h>

namespace dnv::vista::sdk::test
{
	const nfx::serialization::json::Document& loadTestData( const char* testDataPath );
}
