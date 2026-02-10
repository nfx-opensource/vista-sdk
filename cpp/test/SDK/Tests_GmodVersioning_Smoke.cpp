/**
 * @file Tests_GmodVersioning_Smoke.cpp
 * @brief Unit tests for GmodVersioning functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>

namespace dnv::vista::sdk::test
{
    template <typename T>
    class ThreadSafeQueue
    {
    public:
        explicit ThreadSafeQueue( size_t maxSize = 10000 )
            : m_maxSize( maxSize )
        {
        }

        void push( T value )
        {
            std::unique_lock<std::mutex> lock( m_mutex );
            // Block if queue is full
            m_cvPush.wait( lock, [this] { return m_queue.size() < m_maxSize || m_done; } );

            if( m_done )
                return;

            m_queue.push( std::move( value ) );
            m_cvPop.notify_one();
        }

        bool tryPop( T& value )
        {
            std::lock_guard<std::mutex> lock( m_mutex );
            if( m_queue.empty() )
                return false;

            value = std::move( m_queue.front() );
            m_queue.pop();
            return true;
        }

        std::optional<T> waitAndPop()
        {
            std::unique_lock<std::mutex> lock( m_mutex );
            m_cvPop.wait( lock, [this] { return !m_queue.empty() || m_done; } );

            if( m_queue.empty() )
                return std::nullopt;

            auto value = std::move( m_queue.front() );
            m_queue.pop();
            m_cvPush.notify_one(); // Notify producer that space is available
            return value;
        }

        void setDone()
        {
            {
                std::lock_guard<std::mutex> lock( m_mutex );
                m_done = true;
            }
            m_cvPop.notify_all();
            m_cvPush.notify_all();
        }

        bool isDone() const
        {
            std::lock_guard<std::mutex> lock( m_mutex );
            return m_done && m_queue.empty();
        }

    private:
        mutable std::mutex m_mutex;
        std::condition_variable m_cvPop;  // For consumers
        std::condition_variable m_cvPush; // For producer
        std::queue<T> m_queue;
        size_t m_maxSize;
        bool m_done = false;
    };

    struct FailedConversion
    {
        std::string sourcePathStr;
        std::string targetPathStr;
        std::string errorMessage;
    };

    struct SmokeTestContext
    {
        const Gmod& targetGmod;
        ThreadSafeQueue<GmodPath>& queue;
        std::atomic<long>& counter;
        std::vector<FailedConversion>& failedConversions;
        std::vector<std::pair<std::string, std::string>>& changedPaths;
        std::mutex& failedMutex;
        std::mutex& changedMutex;
    };

    TEST( GmodVersioningSmokeTests, VersioningTest )
    {
        using namespace dnv::vista::sdk;

        const auto& vis = VIS::instance();
        const auto& sourceGmod = vis.gmod( VisVersion::v3_4a );
        const auto& targetGmod = vis.gmod( VisVersion::v3_5a );

        ThreadSafeQueue<GmodPath> queue;
        std::atomic<long> counter{ 0 };
        std::vector<FailedConversion> failedConversions;
        std::vector<std::pair<std::string, std::string>> changedPaths;
        std::mutex failedMutex;
        std::mutex changedMutex;

        SmokeTestContext context{
            targetGmod, queue, counter, failedConversions, changedPaths, failedMutex, changedMutex
        };

        constexpr int pad = 7;

        // Producer thread
        auto producer = std::thread( [&]() {
            auto completed =
                sourceGmod.traverse( [&]( const Gmod::TraversalPath& parents, const GmodNode& node ) {
                    if( parents.isEmpty() )
                        return TraversalHandlerResult::Continue;

                    // Build GmodPath from parents and node
                    std::vector<GmodNode> parentNodes;
                    parentNodes.reserve( parents.size() );
                    for( const auto* parent : parents )
                    {
                        parentNodes.push_back( *parent );
                    }
                    auto path = GmodPath( std::move( parentNodes ), node );
                    queue.push( std::move( path ) );
                    return TraversalHandlerResult::Continue;
                } );

            EXPECT_TRUE( completed );
            queue.setDone();

            auto count = counter.load();
            std::cout << "[" << std::setw( pad ) << count << "] Done traversing "
                      << VisVersions::toString( sourceGmod.version() ) << " gmod\n";
        } );

        // Consumer threads - use 50% of available cores
        const size_t numConsumers = std::max( 1u, std::thread::hardware_concurrency() / 2 );
        std::vector<std::thread> consumers;
        consumers.reserve( numConsumers );

        for( size_t i = 0; i < numConsumers; ++i )
        {
            consumers.emplace_back( [&, threadId = i]() {
                while( auto sourcePathOpt = queue.waitAndPop() )
                {
                    const auto& sourcePath = *sourcePathOpt;
                    std::optional<GmodPath> targetPath;
                    long count = 0;

                    try
                    {
                        targetPath = vis.convertPath( VisVersion::v3_4a, sourcePath, VisVersion::v3_5a );
                        EXPECT_TRUE( targetPath.has_value() );

                        if( targetPath.has_value() )
                        {
                            const auto& locations = vis.locations( VisVersion::v3_5a );
                            ParsingErrors errors;
                            auto parsedPath =
                                GmodPath::fromShortPath( targetPath->toString(), targetGmod, locations, errors );
                            EXPECT_TRUE( parsedPath.has_value() ) << "Parsing errors: " << errors.toString();
                            if( parsedPath.has_value() )
                            {
                                EXPECT_EQ( parsedPath->toString(), targetPath->toString() );
                            }

                            count = ++counter;

                            auto sourcePathStr = sourcePath.toString();
                            auto targetPathStr = targetPath->toString();
                            if( sourcePathStr != targetPathStr )
                            {
                                std::lock_guard<std::mutex> lock( changedMutex );
                                changedPaths.emplace_back( sourcePathStr, targetPathStr );
                            }
                        }
                    }
                    catch( const std::exception& e )
                    {
                        count = ++counter;
                        std::cout << "[" << std::setw( pad ) << count << "][" << std::setw( 2 ) << threadId
                                  << "] Failed to create valid path from: " << sourcePath.toString() << " -> "
                                  << ( targetPath.has_value() ? targetPath->toString() : "N/A" ) << " - " << e.what()
                                  << "\n";

                        std::lock_guard<std::mutex> lock( failedMutex );
                        failedConversions.push_back( { sourcePath.toString(),
                                                       targetPath.has_value() ? targetPath->toString() : "N/A",
                                                       e.what() } );
                    }

                    if( count % 10'000 == 0 )
                    {
                        std::cout << "[" << std::setw( pad ) << count << "][" << std::setw( 2 ) << threadId
                                  << "] Paths processed\n";
                    }
                }
            } );
        }

        // Wait for all threads
        producer.join();
        for( auto& consumer : consumers )
        {
            consumer.join();
        }

        std::cout << "[" << std::setw( pad ) << counter.load() << "] Done converting from "
                  << VisVersions::toString( sourceGmod.version() ) << " to "
                  << VisVersions::toString( targetGmod.version() ) << "\n";

        auto success = counter.load() - failedConversions.size();
        auto failed = failedConversions.size();
        auto successRate = ( static_cast<double>( success ) / static_cast<double>( counter.load() ) ) * 100.0;

        std::cout << "Success/failed - " << success << "/" << failed << " (" << std::fixed << std::setprecision( 2 )
                  << successRate << "% success)\n";

        // Group exceptions by message
        std::map<std::string, size_t> exceptionCounts;
        for( const auto& failure : failedConversions )
        {
            exceptionCounts[failure.errorMessage]++;
        }

        // Sort by count descending
        std::vector<std::pair<std::string, size_t>> sortedExceptions( exceptionCounts.begin(), exceptionCounts.end() );
        std::sort( sortedExceptions.begin(), sortedExceptions.end(), []( const auto& a, const auto& b ) {
            return a.second > b.second;
        } );

        std::cout << "Errors:\n";
        for( const auto& [message, count] : sortedExceptions )
        {
            std::cout << "[" << std::setw( 4 ) << count << "] " << message << "\n";
            std::cout << "--------------------------------------------------------\n";
        }

        // Write changed paths to files
        {
            std::string sourceFileName = "changed-source-paths-" +
                                         std::string( VisVersions::toString( sourceGmod.version() ) ) + "-" +
                                         std::string( VisVersions::toString( targetGmod.version() ) ) + ".txt";
            std::string targetFileName = "changed-target-paths-" +
                                         std::string( VisVersions::toString( sourceGmod.version() ) ) + "-" +
                                         std::string( VisVersions::toString( targetGmod.version() ) ) + ".txt";

            std::ofstream sourceFile( sourceFileName );
            std::ofstream targetFile( targetFileName );

            for( const auto& [sourcePath, targetPath] : changedPaths )
            {
                sourceFile << sourcePath << "\n";
                targetFile << targetPath << "\n";
            }
        }

        EXPECT_TRUE( failedConversions.empty() );
    }
} // namespace dnv::vista::sdk::test
