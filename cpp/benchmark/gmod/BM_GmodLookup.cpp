/**
 * @file BM_GmodLookup.cpp
 * @brief GMOD node lookup performance comparison between different container types
 */

#include <benchmark/benchmark.h>

#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <nfx/containers/ChdHashMap.h>
#include <nfx/containers/HashMap.h>
#include <nfx/containers/StringMap.h>
#include <nfx/containers/StringSet.h>

#include <dnv/vista/sdk/Gmod.h>
#include <dnv/vista/sdk/GmodNode.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::benchmarks
{
	class GmodLookup
	{
	private:
		std::unordered_map<std::string, GmodNode> m_unordered_map;
		std::map<std::string, GmodNode> m_map;
		std::unordered_set<std::string> m_unordered_set;
		std::set<std::string> m_set;
		std::vector<std::pair<std::string, GmodNode>> m_vector;

		// nfx containers
		nfx::containers::HashMap<std::string, GmodNode> m_nfx_hashmap;
		nfx::containers::StringMap<GmodNode> m_nfx_stringmap;
		nfx::containers::StringSet m_nfx_stringset;
		nfx::containers::ChdHashMap<GmodNode> m_nfx_chd_hashmap;

		const Gmod* m_sdk_gmod;

	public:
		GmodLookup() = default;
		GmodLookup( const GmodLookup& ) = delete;
		GmodLookup& operator=( const GmodLookup& ) = delete;
		GmodLookup( GmodLookup&& ) = delete;
		GmodLookup& operator=( GmodLookup&& ) = delete;

		void Setup()
		{
			// Initialize SDK
			{
				const auto& vis = VIS::instance();
				m_sdk_gmod = &vis.gmod( VisVersion::v3_7a );
			}

			{ // Clear and reserve space for containers (GMOD v3.7a has ~16K nodes)

				constexpr size_t NUM_NODES = 32768;

				m_unordered_map.clear();
				m_unordered_map.reserve( NUM_NODES );

				m_map.clear();
				m_unordered_set.clear();
				m_unordered_set.reserve( NUM_NODES );
				m_set.clear();
				m_vector.clear();
				m_vector.reserve( NUM_NODES );

				// Clear nfx containers
				m_nfx_hashmap.reserve( NUM_NODES );
				m_nfx_stringmap.clear();
				m_nfx_stringmap.reserve( NUM_NODES );
				m_nfx_stringset.clear();
				m_nfx_stringset.reserve( NUM_NODES );
			}

			{ // First pass: collect all key-value pairs for CHD construction
				std::vector<std::pair<std::string, GmodNode>> gmodPairs;
				auto enumerator = m_sdk_gmod->enumerator();
				while ( enumerator.next() )
				{
					const auto& node = enumerator.current();
					std::string code{ node.code() };
					gmodPairs.emplace_back( code, node );
				}

				// Construct CHD HashMap
				m_nfx_chd_hashmap = nfx::containers::ChdHashMap<GmodNode>( std::move( gmodPairs ) );
			}

			{ // Populate all other containers - iterate through CHD map since gmod_pairs was moved
				for ( const auto& [code, node] : m_nfx_chd_hashmap )
				{
					m_unordered_map.emplace( code, node );
					m_map.emplace( code, node );
					m_unordered_set.emplace( code );
					m_set.emplace( code );
					m_vector.emplace_back( code, node );

					// nfx containers
					m_nfx_hashmap.insertOrAssign( code, node );
					m_nfx_stringmap.emplace( code, node );
					m_nfx_stringset.emplace( code );
				}
			}
		}

		bool std_unordered_map()
		{
			return m_unordered_map.find( "VE" ) != m_unordered_map.end() &&
				   m_unordered_map.find( "400a" ) != m_unordered_map.end() &&
				   m_unordered_map.find( "400" ) != m_unordered_map.end() &&
				   m_unordered_map.find( "H346.11112" ) != m_unordered_map.end();
		}

		bool std_map()
		{
			return m_map.find( "VE" ) != m_map.end() &&
				   m_map.find( "400a" ) != m_map.end() &&
				   m_map.find( "400" ) != m_map.end() &&
				   m_map.find( "H346.11112" ) != m_map.end();
		}

		bool std_unordered_set()
		{
			return m_unordered_set.find( "VE" ) != m_unordered_set.end() &&
				   m_unordered_set.find( "400a" ) != m_unordered_set.end() &&
				   m_unordered_set.find( "400" ) != m_unordered_set.end() &&
				   m_unordered_set.find( "H346.11112" ) != m_unordered_set.end();
		}

		bool std_set()
		{
			return m_set.find( "VE" ) != m_set.end() &&
				   m_set.find( "400a" ) != m_set.end() &&
				   m_set.find( "400" ) != m_set.end() &&
				   m_set.find( "H346.11112" ) != m_set.end();
		}

		bool std_vector()
		{
			auto findCode = [&]( const std::string& code ) {
				return std::find_if( m_vector.begin(), m_vector.end(),
						   [&]( const auto& pair ) { return pair.first == code; } ) != m_vector.end();
			};

			return findCode( "VE" ) &&
				   findCode( "400a" ) &&
				   findCode( "400" ) &&
				   findCode( "H346.11112" );
		}

		bool nfx_hashmap()
		{
			GmodNode* nodePtr = nullptr;
			return m_nfx_hashmap.tryGetValue( "VE", nodePtr ) &&
				   m_nfx_hashmap.tryGetValue( "400a", nodePtr ) &&
				   m_nfx_hashmap.tryGetValue( "400", nodePtr ) &&
				   m_nfx_hashmap.tryGetValue( "H346.11112", nodePtr );
		}

		bool nfx_stringmap()
		{
			return m_nfx_stringmap.find( "VE" ) != m_nfx_stringmap.end() &&
				   m_nfx_stringmap.find( "400a" ) != m_nfx_stringmap.end() &&
				   m_nfx_stringmap.find( "400" ) != m_nfx_stringmap.end() &&
				   m_nfx_stringmap.find( "H346.11112" ) != m_nfx_stringmap.end();
		}

		bool nfx_stringset()
		{
			return m_nfx_stringset.find( "VE" ) != m_nfx_stringset.end() &&
				   m_nfx_stringset.find( "400a" ) != m_nfx_stringset.end() &&
				   m_nfx_stringset.find( "400" ) != m_nfx_stringset.end() &&
				   m_nfx_stringset.find( "H346.11112" ) != m_nfx_stringset.end();
		}

		bool nfx_chd_hashmap()
		{
			GmodNode* nodePtr = nullptr;

			return m_nfx_chd_hashmap.tryGetValue( "VE", nodePtr ) &&
				   m_nfx_chd_hashmap.tryGetValue( "400a", nodePtr ) &&
				   m_nfx_chd_hashmap.tryGetValue( "400", nodePtr ) &&
				   m_nfx_chd_hashmap.tryGetValue( "H346.11112", nodePtr );
		}

		bool sdk_gmod()
		{
			GmodNode* nodePtr = nullptr;
			return m_sdk_gmod->tryGetNode( "VE", nodePtr ) &&
				   m_sdk_gmod->tryGetNode( "400a", nodePtr ) &&
				   m_sdk_gmod->tryGetNode( "400", nodePtr ) &&
				   m_sdk_gmod->tryGetNode( "H346.11112", nodePtr );
		}
	};

	//=====================================================================
	// Benchmark fixture for proper setup/teardown
	//=====================================================================

	static GmodLookup g_benchmarkInstance;

	class GmodLookupFixture : public benchmark::Fixture
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

	/** @brief std::unordered_map lookup benchmark */
	BENCHMARK_F( GmodLookupFixture, std_unordered_map )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.std_unordered_map();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief std::map lookup benchmark */
	BENCHMARK_F( GmodLookupFixture, std_map )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.std_map();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief std::unordered_set lookup benchmark */
	BENCHMARK_F( GmodLookupFixture, std_unordered_set )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.std_unordered_set();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief std::set lookup benchmark */
	BENCHMARK_F( GmodLookupFixture, std_set )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.std_set();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief std::vector lookup benchmark */
	BENCHMARK_F( GmodLookupFixture, std_vector )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.std_vector();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief nfx::containers::HashMap lookup benchmark */
	BENCHMARK_F( GmodLookupFixture, nfx_hashmap )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.nfx_hashmap();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief nfx::containers::StringMap lookup benchmark */
	BENCHMARK_F( GmodLookupFixture, nfx_stringmap )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.nfx_stringmap();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief nfx::containers::StringSet lookup benchmark */
	BENCHMARK_F( GmodLookupFixture, nfx_stringset )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.nfx_stringset();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief nfx::containers::ChdHashMap lookup benchmark */
	BENCHMARK_F( GmodLookupFixture, nfx_chd_hashmap )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.nfx_chd_hashmap();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief SDK GMOD lookup benchmark */
	BENCHMARK_F( GmodLookupFixture, sdk_gmod )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.sdk_gmod();
			benchmark::DoNotOptimize( result );
		}
	}

	//=====================================================================
	// Benchmark registrations
	//=====================================================================

	BENCHMARK_REGISTER_F( GmodLookupFixture, std_unordered_map )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
	BENCHMARK_REGISTER_F( GmodLookupFixture, std_map )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
	BENCHMARK_REGISTER_F( GmodLookupFixture, std_unordered_set )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
	BENCHMARK_REGISTER_F( GmodLookupFixture, std_set )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
	BENCHMARK_REGISTER_F( GmodLookupFixture, std_vector )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
	BENCHMARK_REGISTER_F( GmodLookupFixture, nfx_hashmap )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
	BENCHMARK_REGISTER_F( GmodLookupFixture, nfx_stringmap )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
	BENCHMARK_REGISTER_F( GmodLookupFixture, nfx_stringset )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
	BENCHMARK_REGISTER_F( GmodLookupFixture, nfx_chd_hashmap )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
	BENCHMARK_REGISTER_F( GmodLookupFixture, sdk_gmod )->MinTime( 10.0 )->Unit( benchmark::kNanosecond );
}

BENCHMARK_MAIN();
