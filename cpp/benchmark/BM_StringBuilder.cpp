/**
 * @file BM_StringBuilder.cpp
 * @brief Performance benchmark for StringBuilder wrapper overhead
 * @details Compares StringBuilder performance against std::string and std::ostringstream
 *          for various string building patterns used in VIS metadata path construction.
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/StringBuilder.h>

#include <sstream>
#include <string>

namespace dnv::vista::sdk::benchmark
{
	//=====================================================================
	// StringBuilder benchmark suite
	//=====================================================================

	//----------------------------------------------
	// Simple append operations
	//----------------------------------------------

	static void BM_StdString_ShortAppends( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			std::string result;
			result += "qty";
			result += "-";
			result += "temperature";
			result += "/";

			// Create final string to match StringBuilder's toString() cost
			std::string final = result;
			::benchmark::DoNotOptimize( final );
		}
	}

	static void BM_StringBuilder_ShortAppends( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			auto sb = StringBuilder();

			sb << "qty"
			   << "-"
			   << "temperature"
			   << "/";

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	//----------------------------------------------
	// Stream operator usage
	//----------------------------------------------

	static void BM_StdStringStream_StreamOps( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			std::ostringstream oss;
			oss << "qty" << "-" << "temperature" << "/" << "cnt" << "-" << "oil" << "/" << "pos" << "-" << "1" << "/";

			auto result = oss.str();
			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_StringBuilder_StreamOps( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			auto sb = StringBuilder();

			sb << "qty" << "-" << "temperature" << "/" << "cnt" << "-" << "oil" << "/" << "pos" << "-" << "1" << "/";

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	//----------------------------------------------
	// MetadataTag-like usage (realistic workload)
	//----------------------------------------------

	static void BM_StdString_MetadataPath( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			std::string result;
			result.reserve( 256 );

			result += "qty";
			result += "-";
			result += "temperature";
			result += "/";
			result += "cnt";
			result += "-";
			result += "oil";
			result += "/";
			result += "pos";
			result += "-";
			result += "1";
			result += "/";
			result += "state";
			result += "-";
			result += "running";
			result += "/";
			result += "detail";
			result += "-";
			result += "alarm";
			result += "/";
			result += "cmd";
			result += "-";
			result += "start";
			result += "/";

			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_StringBuilder_MetadataPath( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			auto sb = StringBuilder( 256 );

			// Simulate building a VIS Local ID metadata path (same operations as std::string version)
			sb << "qty"
			   << "-"
			   << "temperature"
			   << "/"
			   << "cnt"
			   << "-"
			   << "oil"
			   << "/"
			   << "pos"
			   << "-"
			   << "1"
			   << "/"
			   << "state"
			   << "-"
			   << "running"
			   << "/"
			   << "detail"
			   << "-"
			   << "alarm"
			   << "/"
			   << "cmd"
			   << "-"
			   << "start"
			   << "/";

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	//----------------------------------------------
	// Large string building
	//----------------------------------------------

	static void BM_StdString_LargeString( ::benchmark::State& state )
	{
		const int iterations = state.range( 0 );

		for ( auto _ : state )
		{
			(void)_;

			std::string result;
			result.reserve( iterations * 20 );

			for ( int i = 0; i < iterations; ++i )
			{
				result += "item-";
				result += std::to_string( i );
				result += "/";
			}

			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_StringBuilder_LargeString( ::benchmark::State& state )
	{
		const int iterations = state.range( 0 );

		for ( auto _ : state )
		{
			(void)_;

			auto sb = StringBuilder( iterations * 20 );

			for ( int i = 0; i < iterations; ++i )
			{
				sb << "item-" << i << "/";
			}

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	//----------------------------------------------
	// Multiple toString()
	//----------------------------------------------

	static void BM_StringBuilder_MultipleToString( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			auto sb = StringBuilder();

			sb << "qty" << "-" << "temperature";
			auto result1 = sb.toString();
			::benchmark::DoNotOptimize( result1 );

			sb << "/" << "cnt" << "-" << "oil";
			auto result2 = sb.toString();
			::benchmark::DoNotOptimize( result2 );

			sb << "/" << "pos" << "-" << "1";
			auto result3 = sb.toString();
			::benchmark::DoNotOptimize( result3 );
		}
	}

	//----------------------------------------------
	// ToStringView vs ToString
	//----------------------------------------------

	static void BM_StringBuilder_ToString( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			auto sb = StringBuilder();
			sb << "qty" << "-" << "temperature" << "/" << "cnt" << "-" << "oil" << "/";

			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_StringBuilder_ToStringView( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			auto sb = StringBuilder();
			sb << "qty" << "-" << "temperature" << "/" << "cnt" << "-" << "oil" << "/";

			auto view = sb.toStringView();
			::benchmark::DoNotOptimize( view );
		}
	}

	//----------------------------------------------
	// Builder reuse with clear()
	//----------------------------------------------

	static void BM_StringBuilder_SingleUse( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			(void)_;

			auto sb = StringBuilder( 256 );
			sb << "qty" << "-" << "temperature" << "/";
			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_StringBuilder_ClearReuse( ::benchmark::State& state )
	{
		auto sb = StringBuilder( 256 );

		for ( auto _ : state )
		{
			(void)_;

			sb.clear();
			sb << "qty" << "-" << "temperature" << "/";
			auto result = sb.toString();
			::benchmark::DoNotOptimize( result );
		}
	}
} // namespace dnv::vista::sdk::benchmark

//=====================================================================
// Benchmarks registration
//=====================================================================

//----------------------------------------------
// Simple append operations
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_StdString_ShortAppends );
BENCHMARK( dnv::vista::sdk::benchmark::BM_StringBuilder_ShortAppends );

//----------------------------------------------
// Stream operator usage
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_StdStringStream_StreamOps );
BENCHMARK( dnv::vista::sdk::benchmark::BM_StringBuilder_StreamOps );

//----------------------------------------------
// MetadataTag-like usage (realistic workload)
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_StdString_MetadataPath );
BENCHMARK( dnv::vista::sdk::benchmark::BM_StringBuilder_MetadataPath );

//----------------------------------------------
// Large string building
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_StdString_LargeString )->Arg( 10 )->Arg( 100 )->Arg( 1000 );
BENCHMARK( dnv::vista::sdk::benchmark::BM_StringBuilder_LargeString )->Arg( 10 )->Arg( 100 )->Arg( 1000 );

//----------------------------------------------
// Multiple toString()
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_StringBuilder_MultipleToString );

//----------------------------------------------
// ToStringView vs ToString
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_StringBuilder_ToString );
BENCHMARK( dnv::vista::sdk::benchmark::BM_StringBuilder_ToStringView );

//----------------------------------------------
// Builder reuse with clear()
//----------------------------------------------

BENCHMARK( dnv::vista::sdk::benchmark::BM_StringBuilder_SingleUse );
BENCHMARK( dnv::vista::sdk::benchmark::BM_StringBuilder_ClearReuse );

//----------------------------------------------
// Benchmark main entry point
//----------------------------------------------

BENCHMARK_MAIN();
