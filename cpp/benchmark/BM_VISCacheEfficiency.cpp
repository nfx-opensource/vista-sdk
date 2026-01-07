/**
 * @file BM_VISCacheEfficiency.cpp
 * @brief Benchmark demonstrating VIS cache efficiency
 * @details Measures first access (cache miss) vs subsequent accesses (cache hits)
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::benchmark
{
	//=====================================================================
	// VIS caching benchmark suite
	//=====================================================================

	//----------------------------------------------
	// Cold start
	//----------------------------------------------

	static void BM_Codebooks_ColdStart( ::benchmark::State& state )
	{
		const auto& vis = VIS::instance();
		const auto version = VisVersion::v3_9a;

		// First iteration will be cold (cache miss), subsequent iterations warm (cache hit)
		// The average time includes both, showing amortized cost
		for ( auto _ : state )
		{
			(void)_;

			auto& codebooks = vis.codebooks( version );
			::benchmark::DoNotOptimize( &codebooks );
		}
		state.SetLabel( "Amortized cost (first=cold, rest=warm)" );
	}

	//----------------------------------------------
	// Warm cache
	//----------------------------------------------

	static void BM_Codebooks_CachedAccess( ::benchmark::State& state )
	{
		const auto& vis = VIS::instance();
		const auto version = VisVersion::v3_9a;

		// Prime the cache (first access loads from embedded resource)
		auto& primed = vis.codebooks( version );
		::benchmark::DoNotOptimize( &primed );

		// Now benchmark cache hits
		for ( auto _ : state )
		{
			(void)_;

			auto& codebooks = vis.codebooks( version );
			::benchmark::DoNotOptimize( &codebooks );
		}
		state.SetLabel( "Cache hit (pointer dereference only)" );
	}

	//----------------------------------------------
	// Multiple version access pattern
	//----------------------------------------------

	static void BM_Codebooks_MultipleVersions( ::benchmark::State& state )
	{
		const auto& vis = VIS::instance();

		// Cycle through all versions
		const std::vector<VisVersion> versions = {
			VisVersion::v3_4a,
			VisVersion::v3_5a,
			VisVersion::v3_6a,
			VisVersion::v3_7a,
			VisVersion::v3_8a,
			VisVersion::v3_9a,
		};

		// Prime all caches
		for ( const auto& version : versions )
		{
			auto& cb = vis.codebooks( version );
			::benchmark::DoNotOptimize( &cb );
		}

		std::size_t idx = 0;
		for ( auto _ : state )
		{
			(void)_;
			auto& codebooks = vis.codebooks( versions[idx % versions.size()] );
			::benchmark::DoNotOptimize( &codebooks );
			++idx;
		}
		state.SetLabel( "Round-robin across 6 cached versions" );
	}

	//----------------------------------------------
	// Realistic usage pattern
	//----------------------------------------------

	static void BM_Codebooks_RealisticUsage( ::benchmark::State& state )
	{
		const auto& vis = VIS::instance();
		const auto version = VisVersion::v3_9a;

		for ( auto _ : state )
		{
			(void)_;

			// Simulate typical usage: multiple operations on same version
			auto& codebooks1 = vis.codebooks( version );
			auto& quantity1 = codebooks1[CodebookName::Quantity];
			::benchmark::DoNotOptimize( &quantity1 );

			auto& codebooks2 = vis.codebooks( version );
			auto& position2 = codebooks2[CodebookName::Position];
			::benchmark::DoNotOptimize( &position2 );

			auto& codebooks3 = vis.codebooks( version );
			auto& state3 = codebooks3[CodebookName::State];
			::benchmark::DoNotOptimize( &state3 );
		}
		state.SetLabel( "3 VIS calls per iteration (all cache hits)" );
	}

	//----------------------------------------------
	// Cache efficiency
	//----------------------------------------------

	static void BM_Codebooks_LookupAfterCache( ::benchmark::State& state )
	{
		const auto& vis = VIS::instance();
		const auto version = VisVersion::v3_9a;

		// Prime cache
		auto& codebooks = vis.codebooks( version );
		auto& quantity = codebooks[CodebookName::Quantity];

		for ( auto _ : state )
		{
			(void)_;

			auto hasTemp = quantity.hasStandardValue( "temperature" );
			::benchmark::DoNotOptimize( hasTemp );
		}
		state.SetLabel( "Lookup on cached codebook" );
	}
} // namespace dnv::vista::sdk::benchmark

//=====================================================================
// Benchmarks registration
//=====================================================================

BENCHMARK( dnv::vista::sdk::benchmark::BM_Codebooks_ColdStart );
BENCHMARK( dnv::vista::sdk::benchmark::BM_Codebooks_CachedAccess );
BENCHMARK( dnv::vista::sdk::benchmark::BM_Codebooks_MultipleVersions );
BENCHMARK( dnv::vista::sdk::benchmark::BM_Codebooks_RealisticUsage );
BENCHMARK( dnv::vista::sdk::benchmark::BM_Codebooks_LookupAfterCache );

//----------------------------------------------
// Benchmark main entry point
//----------------------------------------------

BENCHMARK_MAIN();
