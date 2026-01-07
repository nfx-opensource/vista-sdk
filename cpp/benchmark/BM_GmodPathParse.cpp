/**
 * @file BM_GmodPathParse.cpp
 * @brief Performance benchmark for GmodPath parsing operations
 * @details Benchmarks short path and full path parsing with and without location individualization.
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::benchmarks
{
	class GmodPathParse
	{
	private:
		const Gmod* m_gmod;
		const Locations* m_locations;

	public:
		GmodPathParse() = default;
		GmodPathParse( const GmodPathParse& ) = delete;
		GmodPathParse& operator=( const GmodPathParse& ) = delete;
		GmodPathParse( GmodPathParse&& ) = delete;
		GmodPathParse& operator=( GmodPathParse&& ) = delete;

		void Setup()
		{
			const auto& vis = VIS::instance();
			m_gmod = &vis.gmod( VisVersion::v3_4a );
			m_locations = &vis.locations( VisVersion::v3_4a );
		}

		// No location benchmarks
		bool fromShortPath()
		{
			auto result = GmodPath::fromShortPath( "411.1/C101.72/I101", *m_gmod, *m_locations );
			return result.has_value();
		}

		bool fromFullPath()
		{
			auto result = GmodPath::fromFullPath( "VE/400a/410/411/411i/411.1/CS1/C101/C101.7/C101.72/I101", *m_gmod, *m_locations );
			return result.has_value();
		}

		// With location benchmarks
		bool fromShortPathIndividualized()
		{
			auto result = GmodPath::fromShortPath( "612.21-1/C701.13/S93", *m_gmod, *m_locations );
			return result.has_value();
		}

		bool fromFullPathIndividualized()
		{
			auto result = GmodPath::fromFullPath( "VE/600a/610/612/612.2/612.2i/612.21-1/CS10/C701/C701.1/C701.13/S93", *m_gmod, *m_locations );
			return result.has_value();
		}
	};

	//=====================================================================
	// Benchmark fixture for proper setup/teardown
	//=====================================================================

	static GmodPathParse g_benchmarkInstance;

	class GmodPathParseFixture : public benchmark::Fixture
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

	BENCHMARK_F( GmodPathParseFixture, FromShortPath )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			bool result = g_benchmarkInstance.fromShortPath();
			benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK_F( GmodPathParseFixture, FromFullPath )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			bool result = g_benchmarkInstance.fromFullPath();
			benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK_F( GmodPathParseFixture, FromShortPathIndividualized )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			bool result = g_benchmarkInstance.fromShortPathIndividualized();
			benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK_F( GmodPathParseFixture, FromFullPathIndividualized )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			bool result = g_benchmarkInstance.fromFullPathIndividualized();
			benchmark::DoNotOptimize( result );
		}
	}

	//=====================================================================
	// Benchmark registrations
	//=====================================================================

	BENCHMARK_REGISTER_F( GmodPathParseFixture, FromShortPath )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
	BENCHMARK_REGISTER_F( GmodPathParseFixture, FromFullPath )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
	BENCHMARK_REGISTER_F( GmodPathParseFixture, FromShortPathIndividualized )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
	BENCHMARK_REGISTER_F( GmodPathParseFixture, FromFullPathIndividualized )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
} // namespace dnv::vista::sdk::benchmarks

BENCHMARK_MAIN();
