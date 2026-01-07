/**
 * @file BM_CodebooksLookup.cpp
 * @brief Performance benchmark for Codebooks lookup operations
 * @details Compares std::unordered_map, std::map, Vista SDK containers, and Codebooks class lookup performance.
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/detail/Containers.h>

#include <map>
#include <unordered_map>

namespace dnv::vista::sdk::benchmark
{
    //=====================================================================
    // Codebooks lookup benchmark
    //=====================================================================

    //----------------------------------------------
    // Benchmark fixtures
    //----------------------------------------------

    class CodebooksLookupFixture : public ::benchmark::Fixture
    {
    public:
        void SetUp( const ::benchmark::State& ) override
        {
            const auto& vis = VIS::instance();
            m_codebooks = &vis.codebooks( VisVersion::v3_7a );

            m_unorderedMap.clear();
            m_unorderedMap.reserve( 11 );

            // Populate both maps with all available codebooks
            m_unorderedMap.emplace( CodebookName::Quantity, ( *m_codebooks )[CodebookName::Quantity] );
            m_unorderedMap.emplace( CodebookName::Content, ( *m_codebooks )[CodebookName::Content] );
            m_unorderedMap.emplace( CodebookName::Calculation, ( *m_codebooks )[CodebookName::Calculation] );
            m_unorderedMap.emplace( CodebookName::State, ( *m_codebooks )[CodebookName::State] );
            m_unorderedMap.emplace( CodebookName::Command, ( *m_codebooks )[CodebookName::Command] );
            m_unorderedMap.emplace( CodebookName::Type, ( *m_codebooks )[CodebookName::Type] );
            m_unorderedMap.emplace(
                CodebookName::FunctionalServices, ( *m_codebooks )[CodebookName::FunctionalServices] );
            m_unorderedMap.emplace(
                CodebookName::MaintenanceCategory, ( *m_codebooks )[CodebookName::MaintenanceCategory] );
            m_unorderedMap.emplace( CodebookName::ActivityType, ( *m_codebooks )[CodebookName::ActivityType] );
            m_unorderedMap.emplace( CodebookName::Position, ( *m_codebooks )[CodebookName::Position] );
            m_unorderedMap.emplace( CodebookName::Detail, ( *m_codebooks )[CodebookName::Detail] );

            m_map.emplace( CodebookName::Quantity, ( *m_codebooks )[CodebookName::Quantity] );
            m_map.emplace( CodebookName::Content, ( *m_codebooks )[CodebookName::Content] );
            m_map.emplace( CodebookName::Calculation, ( *m_codebooks )[CodebookName::Calculation] );
            m_map.emplace( CodebookName::State, ( *m_codebooks )[CodebookName::State] );
            m_map.emplace( CodebookName::Command, ( *m_codebooks )[CodebookName::Command] );
            m_map.emplace( CodebookName::Type, ( *m_codebooks )[CodebookName::Type] );
            m_map.emplace( CodebookName::FunctionalServices, ( *m_codebooks )[CodebookName::FunctionalServices] );
            m_map.emplace( CodebookName::MaintenanceCategory, ( *m_codebooks )[CodebookName::MaintenanceCategory] );
            m_map.emplace( CodebookName::ActivityType, ( *m_codebooks )[CodebookName::ActivityType] );
            m_map.emplace( CodebookName::Position, ( *m_codebooks )[CodebookName::Position] );
            m_map.emplace( CodebookName::Detail, ( *m_codebooks )[CodebookName::Detail] );

            // Populate HashMap
            m_hashMap.emplace( CodebookName::Quantity, ( *m_codebooks )[CodebookName::Quantity] );
            m_hashMap.emplace( CodebookName::Content, ( *m_codebooks )[CodebookName::Content] );
            m_hashMap.emplace( CodebookName::Calculation, ( *m_codebooks )[CodebookName::Calculation] );
            m_hashMap.emplace( CodebookName::State, ( *m_codebooks )[CodebookName::State] );
            m_hashMap.emplace( CodebookName::Command, ( *m_codebooks )[CodebookName::Command] );
            m_hashMap.emplace( CodebookName::Type, ( *m_codebooks )[CodebookName::Type] );
            m_hashMap.emplace( CodebookName::FunctionalServices, ( *m_codebooks )[CodebookName::FunctionalServices] );
            m_hashMap.emplace( CodebookName::MaintenanceCategory, ( *m_codebooks )[CodebookName::MaintenanceCategory] );
            m_hashMap.emplace( CodebookName::ActivityType, ( *m_codebooks )[CodebookName::ActivityType] );
            m_hashMap.emplace( CodebookName::Position, ( *m_codebooks )[CodebookName::Position] );
            m_hashMap.emplace( CodebookName::Detail, ( *m_codebooks )[CodebookName::Detail] );
        }

    protected:
        const Codebooks* m_codebooks = nullptr;
        std::unordered_map<CodebookName, Codebook> m_unorderedMap;
        std::map<CodebookName, Codebook> m_map;
        HashMap<CodebookName, Codebook> m_hashMap;
    };

    //----------------------------------------------
    // Benchmark implementations
    //----------------------------------------------

    BENCHMARK_F( CodebooksLookupFixture, StdUnorderedMap )( ::benchmark::State& state )
    {
        for( auto _ : state )
        {
            (void)_;

            bool result = m_unorderedMap.find( CodebookName::Quantity ) != m_unorderedMap.end() &&
                          m_unorderedMap.find( CodebookName::Type ) != m_unorderedMap.end() &&
                          m_unorderedMap.find( CodebookName::Detail ) != m_unorderedMap.end();

            ::benchmark::DoNotOptimize( result );
            ::benchmark::ClobberMemory();
        }
    }

    BENCHMARK_F( CodebooksLookupFixture, StdMap )( ::benchmark::State& state )
    {
        for( auto _ : state )
        {
            (void)_;

            bool result = m_map.find( CodebookName::Quantity ) != m_map.end() &&
                          m_map.find( CodebookName::Type ) != m_map.end() &&
                          m_map.find( CodebookName::Detail ) != m_map.end();

            ::benchmark::DoNotOptimize( result );
            ::benchmark::ClobberMemory();
        }
    }

    BENCHMARK_F( CodebooksLookupFixture, HashMap )( ::benchmark::State& state )
    {
        for( auto _ : state )
        {
            (void)_;

            bool result = m_hashMap.find( CodebookName::Quantity ) != nullptr &&
                          m_hashMap.find( CodebookName::Type ) != nullptr &&
                          m_hashMap.find( CodebookName::Detail ) != nullptr;

            ::benchmark::DoNotOptimize( result );
            ::benchmark::ClobberMemory();
        }
    }

    BENCHMARK_F( CodebooksLookupFixture, Codebooks )( ::benchmark::State& state )
    {
        for( auto _ : state )
        {
            (void)_;

            const auto& a = ( *m_codebooks )[CodebookName::Quantity];
            const auto& b = ( *m_codebooks )[CodebookName::Type];
            const auto& c = ( *m_codebooks )[CodebookName::Detail];

            bool result = true;

            ::benchmark::DoNotOptimize( &a );
            ::benchmark::DoNotOptimize( &b );
            ::benchmark::DoNotOptimize( &c );
            ::benchmark::DoNotOptimize( result );
        }
    }

    //----------------------------------------------
    // Benchmarks registration
    //----------------------------------------------

    BENCHMARK_REGISTER_F( CodebooksLookupFixture, StdUnorderedMap )->Repetitions( 3 );
    BENCHMARK_REGISTER_F( CodebooksLookupFixture, StdMap )->Repetitions( 3 );
    BENCHMARK_REGISTER_F( CodebooksLookupFixture, HashMap )->Repetitions( 3 );
    BENCHMARK_REGISTER_F( CodebooksLookupFixture, Codebooks )->Repetitions( 3 );
} // namespace dnv::vista::sdk::benchmark

BENCHMARK_MAIN();
