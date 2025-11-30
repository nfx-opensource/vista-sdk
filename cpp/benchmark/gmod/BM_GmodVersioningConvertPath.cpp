/**
 * @file BM_GmodVersioningConvertPath.cpp
 * @brief GMOD version conversion performance benchmark testing path conversion between VIS versions
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/GmodPath.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::benchmarks
{
	class GmodVersioningConvertPath
	{
	private:
		const Gmod* m_gmod;
		GmodPath m_gmodPath;

	public:
		GmodVersioningConvertPath() = default;
		GmodVersioningConvertPath( const GmodVersioningConvertPath& ) = delete;
		GmodVersioningConvertPath& operator=( const GmodVersioningConvertPath& ) = delete;
		GmodVersioningConvertPath( GmodVersioningConvertPath&& ) = delete;
		GmodVersioningConvertPath& operator=( GmodVersioningConvertPath&& ) = delete;

		void Setup()
		{
			const auto& vis = VIS::instance();
			m_gmod = &vis.gmod( VisVersion::v3_4a );
			m_gmodPath = m_gmod->parsePath( "411.1/C101.72/I101" );
		}

		std::optional<GmodPath> ConvertPath()
		{
			return VIS::instance().convertPath( VisVersion::v3_4a, m_gmodPath, VisVersion::v3_5a );
		}
	};

	//=====================================================================
	// Benchmark fixture for proper setup/teardown
	//=====================================================================

	static GmodVersioningConvertPath g_benchmarkInstance;

	class GmodVersioningConvertPathFixture : public benchmark::Fixture
	{
	public:
		void SetUp( [[maybe_unused]] const benchmark::State& state ) override
		{
			g_benchmarkInstance.Setup();
		}

		void TearDown( [[maybe_unused]] const benchmark::State& state ) override
		{
		}
	};

	//=====================================================================
	// Benchmark implementations
	//=====================================================================

	/** @brief GMOD path version conversion benchmark */
	BENCHMARK_F( GmodVersioningConvertPathFixture, ConvertPath )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto result = g_benchmarkInstance.ConvertPath();
			benchmark::DoNotOptimize( result );
		}
	}

	//=====================================================================
	// Benchmark registrations
	//=====================================================================

	BENCHMARK_REGISTER_F( GmodVersioningConvertPathFixture, ConvertPath )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
} // namespace dnv::vista::sdk::benchmarks

BENCHMARK_MAIN();
