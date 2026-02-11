/**
 * @file BM_DataChannelListSerialization.cpp
 * @brief Benchmark for DataChannelList JSON serialization performance
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/Transport.h>
#include <dnv/vista/sdk/VIS.h>

#include <EmbeddedSchemas.h>

#include <iostream>
#include <string>

namespace dnv::vista::sdk::benchmark
{
    //=====================================================================
    // DataChannelList serialization benchmarks
    //=====================================================================

    //----------------------------------------------
    // Benchmark fixture
    //----------------------------------------------

    class DataChannelListSerializationFixture : public ::benchmark::Fixture
    {
    public:
        void SetUp( const ::benchmark::State& state ) override
        {
            if( state.thread_index() == 0 && !m_package.has_value() )
            {
                try
                {
                    auto sampleJsonOpt = EmbeddedSchema::get( "DataChannelList.sample.json" );
                    if( !sampleJsonOpt.has_value() )
                    {
                        throw std::runtime_error{ "Failed to get embedded DataChannelList.sample.json" };
                    }

                    auto jsonContent = std::string{ *sampleJsonOpt };

                    auto docOpt = json::Document::fromString( jsonContent );
                    if( !docOpt.has_value() )
                    {
                        throw std::runtime_error{ "Failed to parse JSON" };
                    }

                    m_package = serialization::json::SerializationTraits<
                        transport::datachannel::DataChannelListPackage>::fromDocument( *docOpt );

                    m_options.includeNullFields = false;
                    m_options.prettyPrint = false;
                    m_options.validateOnDeserialize = false;

                    auto warmupJson =
                        serialization::json::Serializer<transport::datachannel::DataChannelListPackage>::toString(
                            *m_package, m_options );
                    (void)warmupJson;
                }
                catch( const std::exception& ex )
                {
                    std::cerr << "Error during benchmark setup: " << ex.what() << std::endl;
                    throw;
                }
            }
        }

    protected:
        std::optional<transport::datachannel::DataChannelListPackage> m_package;
        serialization::json::Serializer<transport::datachannel::DataChannelListPackage>::Options m_options;
    };

    //----------------------------------------------
    // Benchmark implementations
    //----------------------------------------------

    BENCHMARK_F( DataChannelListSerializationFixture, Json )( ::benchmark::State& state )
    {
        for( auto _ : state )
        {
            (void)_;

            auto json = serialization::json::Serializer<transport::datachannel::DataChannelListPackage>::toString(
                *m_package, m_options );
            ::benchmark::DoNotOptimize( json );
        }
    }

    //----------------------------------------------
    // Benchmarks registration
    //----------------------------------------------

    BENCHMARK_REGISTER_F( DataChannelListSerializationFixture, Json )
        ->Unit( ::benchmark::kMicrosecond )
        ->Repetitions( 3 );
} // namespace dnv::vista::sdk::benchmark

BENCHMARK_MAIN();
