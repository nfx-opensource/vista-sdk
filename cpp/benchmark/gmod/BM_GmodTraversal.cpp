/**
 * @file BM_GmodTraversal.cpp
 * @brief GMOD tree traversal performance benchmark testing full tree iteration
 */

#include <vector>

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::benchmarks
{
    //=====================================================================
    // Gmod traversal benchmarks
    //=====================================================================

    //----------------------------------------------
    // Benchmark fixture
    //----------------------------------------------

    class GmodTraversalFixture : public benchmark::Fixture
    {
    protected:
        const Gmod* m_gmod;

    public:
        void SetUp( [[maybe_unused]] const benchmark::State& state ) override
        {
            const auto& vis = VIS::instance();
            m_gmod = &vis.gmod( VisVersion::v3_4a );
        }
    };

    //----------------------------------------------
    // Benchmarks implementation
    //----------------------------------------------

    BENCHMARK_F( GmodTraversalFixture, FullTraversal )( benchmark::State& state )
    {
        for( auto _ : state )
        {
            (void)_;

            // Simple traversal handler that continues through all nodes
            bool result = m_gmod->traverse(
                []( [[maybe_unused]] const Gmod::TraversalPath& parents,
                    [[maybe_unused]] const GmodNode& node ) -> TraversalHandlerResult {
                    return TraversalHandlerResult::Continue;
                } );
            benchmark::DoNotOptimize( result );
        }
    }

    //----------------------------------------------
    // Benchmarks registration
    //----------------------------------------------

    BENCHMARK_REGISTER_F( GmodTraversalFixture, FullTraversal )->Unit( benchmark::kMillisecond )->Repetitions( 3 );
} // namespace dnv::vista::sdk::benchmarks

BENCHMARK_MAIN();
