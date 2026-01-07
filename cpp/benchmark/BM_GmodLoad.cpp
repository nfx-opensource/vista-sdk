/**
 * @file BM_GmodLoad.cpp
 * @brief Performance benchmark for Gmod operations
 * @details Benchmarks Gmod loading (parse + construction) and cache access performance.
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>

#include <unordered_map>

namespace dnv::vista::sdk::benchmark
{
	//=====================================================================
	// Gmod benchmarks
	//=====================================================================

	static void BM_GmodCacheAccess( ::benchmark::State& state )
	{
		const auto& vis = VIS::instance();
		const auto& gmodwarmup = vis.gmod( VisVersion::v3_7a );
		(void)( gmodwarmup );

		for ( auto _ : state )
		{
			(void)_;

			// This measures: Cache lookup and reference return
			const auto& gmod = vis.gmod( VisVersion::v3_7a );
			::benchmark::DoNotOptimize( &gmod );
		}
	}

	static void BM_GmodLookup_UnorderedMap( ::benchmark::State& state )
	{
		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( VisVersion::v3_7a );

		std::unordered_map<std::string, const GmodNode*> dict;
		for ( const auto& [code, node] : gmod )
		{
			dict[code] = &node;
		}

		for ( auto _ : state )
		{
			(void)_;

			bool result = dict.find( "VE" ) != dict.end() && dict.find( "400a" ) != dict.end() &&
						  dict.find( "400" ) != dict.end() && dict.find( "H346.11112" ) != dict.end();

			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_GmodLookup_Gmod( ::benchmark::State& state )
	{
		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( VisVersion::v3_7a );

		for ( auto _ : state )
		{
			(void)_;

			bool result = gmod.node( "VE" ).has_value() && gmod.node( "400a" ).has_value() &&
						  gmod.node( "400" ).has_value() && gmod.node( "H346.11112" ).has_value();

			::benchmark::DoNotOptimize( result );
		}
	}
} // namespace dnv::vista::sdk::benchmark

//=====================================================================
// Benchmarks registration
//=====================================================================

BENCHMARK( dnv::vista::sdk::benchmark::BM_GmodCacheAccess )->Unit( ::benchmark::kNanosecond );
BENCHMARK( dnv::vista::sdk::benchmark::BM_GmodLookup_UnorderedMap )->Unit( ::benchmark::kNanosecond );
BENCHMARK( dnv::vista::sdk::benchmark::BM_GmodLookup_Gmod )->Unit( ::benchmark::kNanosecond );

//----------------------------------------------
// Benchmark main entry point
//----------------------------------------------

BENCHMARK_MAIN();
