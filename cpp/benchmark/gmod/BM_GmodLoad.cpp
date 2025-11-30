/**
 * @file BM_GmodLoad.cpp
 * @brief GMOD caching and access performance analysis for VIS system
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/Gmod.h>
#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/VISVersion.h>

#include <internal/core/EmbeddedResource.h>

namespace dnv::vista::sdk::benchmarks
{
	static void BM_gmodCacheAccess( benchmark::State& state )
	{
		const auto& vis = VIS::instance();

		for ( auto _ : state )
		{
			// This measures: Cache lookup and reference return
			const auto& gmod = vis.gmod( VisVersion::v3_7a );
			benchmark::DoNotOptimize( &gmod );
		}
	}

	BENCHMARK( BM_gmodCacheAccess )->Unit( benchmark::kNanosecond );
} // namespace dnv::vista::sdk::benchmarks

BENCHMARK_MAIN();
