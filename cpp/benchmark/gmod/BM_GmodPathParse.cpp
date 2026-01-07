/**
 * @file BM_GmodPathParse.cpp
 * @brief Performance benchmark for GmodPath parsing operations
 * @details Benchmarks short path and full path parsing with and without location individualization.
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::benchmarks
{
    //=====================================================================
    // GmodPath parsing benchmarks
    //=====================================================================

    //----------------------------------------------
    // Benchmark fixture
    //----------------------------------------------

    class GmodPathParseFixture : public benchmark::Fixture
    {
    protected:
        const Gmod* m_gmod;
        const Locations* m_locations;

    public:
        void SetUp( [[maybe_unused]] const benchmark::State& state ) override
        {
            const auto& vis = VIS::instance();
            m_gmod = &vis.gmod( VisVersion::v3_4a );
            m_locations = &vis.locations( VisVersion::v3_4a );
        }
    };

    //----------------------------------------------
    // Benchmarks implementations
    //----------------------------------------------

    BENCHMARK_F( GmodPathParseFixture, FromShortPath )( benchmark::State& state )
    {
        for( auto _ : state )
        {
            (void)_;

            auto result = GmodPath::fromShortPath( "411.1/C101.72/I101", *m_gmod, *m_locations );
            benchmark::DoNotOptimize( result );
        }
    }

    BENCHMARK_F( GmodPathParseFixture, FromFullPath )( benchmark::State& state )
    {
        for( auto _ : state )
        {
            (void)_;

            auto result = GmodPath::fromFullPath(
                "VE/400a/410/411/411i/411.1/CS1/C101/C101.7/C101.72/I101", *m_gmod, *m_locations );
            benchmark::DoNotOptimize( result );
        }
    }

    BENCHMARK_F( GmodPathParseFixture, FromShortPathIndividualized )( benchmark::State& state )
    {
        for( auto _ : state )
        {
            (void)_;

            auto result = GmodPath::fromShortPath( "612.21-1/C701.13/S93", *m_gmod, *m_locations );
            benchmark::DoNotOptimize( result );
        }
    }

    BENCHMARK_F( GmodPathParseFixture, FromFullPathIndividualized )( benchmark::State& state )
    {
        for( auto _ : state )
        {
            (void)_;

            auto result = GmodPath::fromFullPath(
                "VE/600a/610/612/612.2/612.2i/612.21-1/CS10/C701/C701.1/C701.13/S93", *m_gmod, *m_locations );
            benchmark::DoNotOptimize( result );
        }
    }

    //----------------------------------------------
    // Benchmarks registrations
    //----------------------------------------------

    BENCHMARK_REGISTER_F( GmodPathParseFixture, FromShortPath )->Unit( benchmark::kMicrosecond )->Repetitions( 3 );
    BENCHMARK_REGISTER_F( GmodPathParseFixture, FromFullPath )->Unit( benchmark::kMicrosecond )->Repetitions( 3 );
    BENCHMARK_REGISTER_F( GmodPathParseFixture, FromShortPathIndividualized )
        ->Unit( benchmark::kMicrosecond )
        ->Repetitions( 3 );
    BENCHMARK_REGISTER_F( GmodPathParseFixture, FromFullPathIndividualized )
        ->Unit( benchmark::kMicrosecond )
        ->Repetitions( 3 );
} // namespace dnv::vista::sdk::benchmarks

BENCHMARK_MAIN();
