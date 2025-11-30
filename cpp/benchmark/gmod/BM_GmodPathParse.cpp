/**
 * @file BM_GmodPathParse.cpp
 * @brief GMOD path parsing performance benchmarks comparing different parsing strategies
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/GmodPath.h>
#include <dnv/vista/sdk/Locations.h>
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
		bool TryParse()
		{
			std::optional<GmodPath> result;
			return GmodPath::tryParse( "411.1/C101.72/I101", *m_gmod, *m_locations, result );
		}

		bool TryParseFullPath()
		{
			std::optional<GmodPath> result;
			return GmodPath::tryParseFullPath( "VE/400a/410/411/411i/411.1/CS1/C101/C101.7/C101.72/I101", *m_gmod, *m_locations, result );
		}

		// With location benchmarks
		bool TryParseIndividualized()
		{
			std::optional<GmodPath> result;
			return GmodPath::tryParse( "612.21-1/C701.13/S93", *m_gmod, *m_locations, result );
		}

		bool TryParseFullPathIndividualized()
		{
			std::optional<GmodPath> result;
			return GmodPath::tryParseFullPath( "VE/600a/610/612/612.2/612.2i/612.21-1/CS10/C701/C701.1/C701.13/S93", *m_gmod, *m_locations, result );
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

	/** @brief TryParse benchmark - No location */
	BENCHMARK_F( GmodPathParseFixture, TryParse )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.TryParse();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief TryParseFullPath benchmark - No location */
	BENCHMARK_F( GmodPathParseFixture, TryParseFullPath )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.TryParseFullPath();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief TryParseIndividualized benchmark - With location */
	BENCHMARK_F( GmodPathParseFixture, TryParseIndividualized )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.TryParseIndividualized();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief TryParseFullPathIndividualized benchmark - With location */
	BENCHMARK_F( GmodPathParseFixture, TryParseFullPathIndividualized )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.TryParseFullPathIndividualized();
			benchmark::DoNotOptimize( result );
		}
	}

	//=====================================================================
	// Benchmark registrations
	//=====================================================================

	BENCHMARK_REGISTER_F( GmodPathParseFixture, TryParse )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
	BENCHMARK_REGISTER_F( GmodPathParseFixture, TryParseFullPath )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
	BENCHMARK_REGISTER_F( GmodPathParseFixture, TryParseIndividualized )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
	BENCHMARK_REGISTER_F( GmodPathParseFixture, TryParseFullPathIndividualized )->MinTime( 10.0 )->Unit( benchmark::kMicrosecond );
} // namespace dnv::vista::sdk::benchmarks

BENCHMARK_MAIN();
