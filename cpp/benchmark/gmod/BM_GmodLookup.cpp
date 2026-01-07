/**
 * @file BM_GmodLookup.cpp
 * @brief Performance benchmark for Gmod node lookup operations
 * @details Compares lookup performance: std::unordered_map vs Gmod class
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>

#include <unordered_map>

namespace dnv::vista::sdk::benchmark
{
    //=====================================================================
    // Gmod lookup benchmarks
    //=====================================================================

    //----------------------------------------------
    // Benchmark fixture
    //----------------------------------------------

    class GmodLookupFixture : public ::benchmark::Fixture
    {
    public:
        void SetUp( const ::benchmark::State& ) override
        {
            const auto& vis = VIS::instance();
            m_gmod = &vis.gmod( VisVersion::v3_7a );

            m_unorderedMap.clear();
            m_unorderedMap.reserve( 10000 );
            for( const auto& [code, node] : *m_gmod )
            {
                m_unorderedMap[code] = &node;
            }
        }

    protected:
        const Gmod* m_gmod = nullptr;
        std::unordered_map<std::string, const GmodNode*> m_unorderedMap;
    };

    //----------------------------------------------
    // Benchmark implementations
    //----------------------------------------------

    BENCHMARK_F( GmodLookupFixture, StdUnorderedMap )( ::benchmark::State& state )
    {
        for( auto _ : state )
        {
            (void)_;

            bool result = m_unorderedMap.find( "VE" ) != m_unorderedMap.end() &&
                          m_unorderedMap.find( "400a" ) != m_unorderedMap.end() &&
                          m_unorderedMap.find( "400" ) != m_unorderedMap.end() &&
                          m_unorderedMap.find( "H346.11112" ) != m_unorderedMap.end();

            ::benchmark::DoNotOptimize( result );
        }
    }

    BENCHMARK_F( GmodLookupFixture, Gmod )( ::benchmark::State& state )
    {
        for( auto _ : state )
        {
            (void)_;

            bool result = m_gmod->node( "VE" ).has_value() && m_gmod->node( "400a" ).has_value() &&
                          m_gmod->node( "400" ).has_value() && m_gmod->node( "H346.11112" ).has_value();

            ::benchmark::DoNotOptimize( result );
        }
    }

    //----------------------------------------------
    // Benchmarks registrations
    //----------------------------------------------

    BENCHMARK_REGISTER_F( GmodLookupFixture, StdUnorderedMap )->Repetitions( 3 );
    BENCHMARK_REGISTER_F( GmodLookupFixture, Gmod )->Repetitions( 3 );
} // namespace dnv::vista::sdk::benchmark

BENCHMARK_MAIN();
