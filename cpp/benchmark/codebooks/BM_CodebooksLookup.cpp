/**
 * @file BM_CodebooksLookup.cpp
 * @brief Performance comparison of codebook lookup methods
 */

#include <benchmark/benchmark.h>

#include <map>
#include <unordered_map>

#include <dnv/vista/sdk/Codebook.h>
#include <dnv/vista/sdk/CodebookName.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::benchmarks
{
	class CodebooksLookup
	{
	private:
		std::unordered_map<CodebookName, Codebook> m_unordered_map;
		std::map<CodebookName, Codebook> m_map;
		const Codebooks* m_sdk_codebooks;

	public:
		CodebooksLookup() = default;
		CodebooksLookup( const CodebooksLookup& ) = delete;
		CodebooksLookup& operator=( const CodebooksLookup& ) = delete;
		CodebooksLookup( CodebooksLookup&& ) = delete;
		CodebooksLookup& operator=( CodebooksLookup&& ) = delete;

		void Setup()
		{
			const auto& vis = VIS::instance();
			m_sdk_codebooks = &vis.codebooks( VisVersion::v3_7a );

			m_unordered_map.clear();
			m_unordered_map.reserve( 11 );

			for ( const auto& codebook : *m_sdk_codebooks )
			{
				m_unordered_map.emplace( codebook.name(), codebook );
				m_map.emplace( codebook.name(), codebook );
			}
		}

		bool std_unordered_map()
		{
			return m_unordered_map.find( CodebookName::Quantity ) != m_unordered_map.end() &&
				   m_unordered_map.find( CodebookName::Type ) != m_unordered_map.end() &&
				   m_unordered_map.find( CodebookName::Detail ) != m_unordered_map.end();
		}

		bool std_map()
		{
			return m_map.find( CodebookName::Quantity ) != m_map.end() &&
				   m_map.find( CodebookName::Type ) != m_map.end() &&
				   m_map.find( CodebookName::Detail ) != m_map.end();
		}

		bool sdk_codebooks()
		{
			const auto& a = m_sdk_codebooks->codebook( CodebookName::Quantity );
			const auto& b = m_sdk_codebooks->codebook( CodebookName::Type );
			const auto& c = m_sdk_codebooks->codebook( CodebookName::Detail );

			return a.standardValues().count() > 0 &&
				   b.standardValues().count() > 0 &&
				   c.standardValues().count() > 0;
		}

		bool sdk_lookup_operator()
		{
			const auto& a = ( *m_sdk_codebooks )[CodebookName::Quantity];
			const auto& b = ( *m_sdk_codebooks )[CodebookName::Type];
			const auto& c = ( *m_sdk_codebooks )[CodebookName::Detail];

			return a.standardValues().count() > 0 &&
				   b.standardValues().count() > 0 &&
				   c.standardValues().count() > 0;
		}
	};

	//=====================================================================
	// Benchmark fixture for proper setup/teardown
	//=====================================================================

	static CodebooksLookup g_benchmarkInstance;

	class CodebooksLookupFixture : public benchmark::Fixture
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
	BENCHMARK_F( CodebooksLookupFixture, std_unordered_map )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.std_unordered_map();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief std::map lookup benchmark */
	BENCHMARK_F( CodebooksLookupFixture, std_map )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.std_map();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief Direct SDK access benchmark */
	BENCHMARK_F( CodebooksLookupFixture, codebooks )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.sdk_codebooks();
			benchmark::DoNotOptimize( result );
		}
	}

	/** @brief Direct SDK operator[] access benchmark */
	BENCHMARK_F( CodebooksLookupFixture, sdk_lookup_operator )( benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool result = g_benchmarkInstance.sdk_lookup_operator();
			benchmark::DoNotOptimize( result );
		}
	}

	//=====================================================================
	// Benchmark registrations
	//=====================================================================

	BENCHMARK_REGISTER_F( CodebooksLookupFixture, std_unordered_map )->MinTime( 10.0 );
	BENCHMARK_REGISTER_F( CodebooksLookupFixture, std_map )->MinTime( 10.0 );
	BENCHMARK_REGISTER_F( CodebooksLookupFixture, codebooks )->MinTime( 10.0 );
	BENCHMARK_REGISTER_F( CodebooksLookupFixture, sdk_lookup_operator )->MinTime( 10.0 );
}

BENCHMARK_MAIN();
