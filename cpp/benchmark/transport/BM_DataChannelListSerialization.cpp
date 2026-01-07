/**
 * @file BM_DataChannelListSerialization.cpp
 * @brief Benchmark for DataChannelList JSON serialization performance
 */

#include <benchmark/benchmark.h>

#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/serialization/json/DataChannelListSerializationTraits.h>
#include <dnv/vista/sdk/transport/datachannel/DataChannel.h>

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
                        throw std::runtime_error( "Failed to get embedded DataChannelList.sample.json" );
                    }

                    auto jsonContent = std::string{ *sampleJsonOpt };

                    auto docOpt = nfx::serialization::json::SerializableDocument::fromString( jsonContent );
                    if( !docOpt.has_value() )
                    {
                        throw std::runtime_error( "Failed to parse JSON" );
                    }

                    auto shipId = ShipId::fromString( "IMO0000000" ).value();
                    auto configRef = transport::datachannel::ConfigurationReference{ "temp", DateTimeOffset::now() };
                    auto header = transport::datachannel::Header{ shipId, configRef };
                    auto dataChannelList = transport::datachannel::DataChannelList{};
                    auto package = transport::datachannel::Package{ header, dataChannelList };
                    auto dataChannelListPackage = transport::datachannel::DataChannelListPackage{ package };

                    nfx::serialization::json::SerializationTraits<
                        transport::datachannel::DataChannelListPackage>::deserialize( *docOpt, dataChannelListPackage );

                    m_package = std::move( dataChannelListPackage );

                    m_options.includeNullFields = false;
                    m_options.prettyPrint = false;
                    m_options.validateOnDeserialize = false;

                    auto warmupJson =
                        nfx::serialization::json::Serializer<transport::datachannel::DataChannelListPackage>::toString(
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
        nfx::serialization::json::Serializer<transport::datachannel::DataChannelListPackage>::Options m_options;
    };

    //----------------------------------------------
    // Benchmark implementations
    //----------------------------------------------

    BENCHMARK_F( DataChannelListSerializationFixture, Json )( ::benchmark::State& state )
    {
        for( auto _ : state )
        {
            (void)_;

            auto json = nfx::serialization::json::Serializer<transport::datachannel::DataChannelListPackage>::toString(
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
