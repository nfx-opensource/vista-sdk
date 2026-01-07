/**
 * @file BM_GmodLoad.cpp
 * @brief Performance benchmark for Gmod loading
 * @details Benchmarks Gmod DTO loading (JSON parse + deserialization)
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::benchmark
{
    //=====================================================================
    // Gmod load benchmark
    //=====================================================================

    static void GmodLoad( ::benchmark::State& state )
    {
        {
            const auto& warmupGmod = VIS::instance().gmod( VisVersion::v3_7a );
            ::benchmark::DoNotOptimize( &warmupGmod );
        }

        for( auto _ : state )
        {
            (void)_;

            const auto& gmod = VIS::instance().gmod( VisVersion::v3_7a );
            ::benchmark::DoNotOptimize( &gmod );
        }
    }

    //----------------------------------------------
    // Benchmarks registration
    //----------------------------------------------

    BENCHMARK( GmodLoad )->Unit( ::benchmark::kNanosecond )->Repetitions( 3 );
} // namespace dnv::vista::sdk::benchmark

BENCHMARK_MAIN();
