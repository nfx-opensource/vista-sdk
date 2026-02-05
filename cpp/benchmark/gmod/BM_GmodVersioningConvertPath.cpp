/**
 * @file BM_GmodVersioningConvertPath.cpp
 * @brief GMOD version conversion performance benchmark testing path conversion between VIS versions
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::benchmarks
{
    class GmodVersioningConvertPath
    {
    private:
        const Gmod* m_gmod = nullptr;
        std::optional<GmodPath> m_gmodPath;

    public:
        GmodVersioningConvertPath() = default;
        GmodVersioningConvertPath( const GmodVersioningConvertPath& ) = delete;
        GmodVersioningConvertPath& operator=( const GmodVersioningConvertPath& ) = delete;
        GmodVersioningConvertPath( GmodVersioningConvertPath&& ) = delete;
        GmodVersioningConvertPath& operator=( GmodVersioningConvertPath&& ) = delete;

        void Setup( std::string_view pathStr )
        {
            const auto& vis = VIS::instance();
            m_gmod = &vis.gmod( VisVersion::v3_4a );
            const auto& locations = vis.locations( VisVersion::v3_4a );

            ParsingErrors errors;
            auto path = GmodPath::fromShortPath( pathStr, *m_gmod, locations, errors );
            if( !path.has_value() )
            {
                throw std::runtime_error{ "Failed to parse benchmark path: " + std::string{ pathStr } };
            }
            m_gmodPath = *path;
        }

        std::optional<GmodPath> ConvertPath()
        {
            return VIS::instance().convertPath( VisVersion::v3_4a, *m_gmodPath, VisVersion::v3_5a );
        }
    };

    static struct BenchmarkState
    {
        GmodVersioningConvertPath instanceNoLocation;
        GmodVersioningConvertPath instanceWithLocation;

        BenchmarkState()
        {
            instanceNoLocation.Setup( "411.1/C101.72/I101" );
            instanceWithLocation.Setup( "612.21-1/C701.13/S93" );
        }
    } g_benchmarkState;

    /** @brief GMOD path version conversion benchmark without location */
    static void BM_ConvertPath( benchmark::State& state )
    {
        for( auto _ : state )
        {
            auto result = g_benchmarkState.instanceNoLocation.ConvertPath();
            benchmark::DoNotOptimize( result );
        }
    }

    /** @brief GMOD path version conversion benchmark with locations */
    static void BM_ConvertPathWithLocation( benchmark::State& state )
    {
        for( auto _ : state )
        {
            auto result = g_benchmarkState.instanceWithLocation.ConvertPath();
            benchmark::DoNotOptimize( result );
        }
    }

    BENCHMARK( BM_ConvertPath )->MinTime( 2.0 )->Unit( benchmark::kMicrosecond );
    BENCHMARK( BM_ConvertPathWithLocation )->MinTime( 2.0 )->Unit( benchmark::kMicrosecond );
} // namespace dnv::vista::sdk::benchmarks

BENCHMARK_MAIN();
