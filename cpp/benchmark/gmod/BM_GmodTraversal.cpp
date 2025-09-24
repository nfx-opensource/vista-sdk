/**
 * @file BM_GmodTraversal.cpp
 * @brief GMOD tree traversal performance benchmark testing full tree iteration
 */

#include <vector>

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/Gmod.h>
#include <dnv/vista/sdk/GmodNode.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::benchmarks
{
	class GmodTraversal
	{
	private:
		const Gmod* m_gmod;

	public:
		GmodTraversal() = default;
		GmodTraversal( const GmodTraversal& ) = delete;
		GmodTraversal& operator=( const GmodTraversal& ) = delete;
		GmodTraversal( GmodTraversal&& ) = delete;
		GmodTraversal& operator=( GmodTraversal&& ) = delete;

		void Setup()
		{
			const auto& vis = VIS::instance();
			m_gmod = &vis.gmod( VisVersion::v3_4a );
		}

		bool FullTraversal()
		{
			// Simple traversal handler that continues through all nodes
			TraverseHandler handler = []( [[maybe_unused]] const std::vector<const GmodNode*>& parents, [[maybe_unused]] const GmodNode& node ) -> TraversalHandlerResult {
				return TraversalHandlerResult::Continue;
			};

			return m_gmod->traverse( handler );
		}
	};

	//=====================================================================
	// Benchmark fixture for proper setup/teardown
	//=====================================================================

	static GmodTraversal g_benchmarkInstance;

	class GmodTraversalFixture : public benchmark::Fixture
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

	/** @brief Full GMOD tree traversal benchmark */
	BENCHMARK_F( GmodTraversalFixture, FullTraversal )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.FullTraversal();
			benchmark::DoNotOptimize( result );
		}
	}

	//=====================================================================
	// Benchmark registrations
	//=====================================================================

	BENCHMARK_REGISTER_F( GmodTraversalFixture, FullTraversal )->MinTime( 10.0 )->Unit( benchmark::kMillisecond );
}

BENCHMARK_MAIN();
