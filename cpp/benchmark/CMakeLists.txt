#==============================================================================
# dnv-vista-sdk-cpp - Benchmark suite CMake configuration
#==============================================================================

#----------------------------------------------
# Benchmarks condition check
#----------------------------------------------

if(NOT DNV_VISTA_SDK_CPP_BUILD_BENCHMARKS)
    return()
endif()

#----------------------------------------------
# Google Benchmark dependency
#----------------------------------------------

include(FetchContent)

set(VISTA_SDK_CPP_DEPS_BENCHMARK_VERSION "1.9.5")

find_package(benchmark ${VISTA_SDK_CPP_DEPS_BENCHMARK_MIN_VERSION} QUIET)
if(NOT benchmark_FOUND)
    set(BENCHMARK_ENABLE_TESTING        OFF CACHE BOOL "Build benchmark tests"                         FORCE)
    set(BENCHMARK_ENABLE_EXCEPTIONS     ON  CACHE BOOL "Enable exceptions in benchmark library"        FORCE)
    set(BENCHMARK_ENABLE_LTO            OFF CACHE BOOL "Enable link time optimization"                 FORCE)
    set(BENCHMARK_USE_LIBCXX            OFF CACHE BOOL "Use libc++ as the standard library"            FORCE)
    set(BENCHMARK_ENABLE_WERROR         OFF CACHE BOOL "Treat warnings as errors"                      FORCE)
    set(BENCHMARK_FORCE_WERROR          OFF CACHE BOOL "Force warnings as errors regardless of issues" FORCE)
    set(BENCHMARK_BUILD_32_BITS         OFF CACHE BOOL "Build a 32-bit version of the library"         FORCE)
    set(BENCHMARK_ENABLE_INSTALL        OFF CACHE BOOL "Install benchmark targets"                     FORCE)
    set(BENCHMARK_ENABLE_DOXYGEN        OFF CACHE BOOL "Build documentation with Doxygen"              FORCE)
    set(BENCHMARK_INSTALL_DOCS          OFF CACHE BOOL "Install documentation"                         FORCE)
    set(BENCHMARK_DOWNLOAD_DEPENDENCIES OFF CACHE BOOL "Download and build unmet dependencies"         FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS    OFF CACHE BOOL "Build benchmark GTest-based tests"             FORCE)
    set(BENCHMARK_USE_BUNDLED_GTEST     OFF CACHE BOOL "Use bundled GoogleTest for benchmark"          FORCE)
    set(BENCHMARK_ENABLE_LIBPFM         OFF CACHE BOOL "Enable performance counters via libpfm"        FORCE)
    set(ENABLE_ASSEMBLY_TESTS_DEFAULT   OFF CACHE BOOL "Enable assembly tests by default"              FORCE)

    FetchContent_Declare(
        googlebenchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG        v${VISTA_SDK_CPP_DEPS_BENCHMARK_VERSION}
            GIT_SHALLOW    TRUE
    )

    FetchContent_MakeAvailable(googleBenchmark)
endif()

set(benchmark_sources)

list(APPEND benchmark_sources
    codebooks/BM_CodebooksLookup.cpp
    gmod/BM_GmodLoad.cpp
    gmod/BM_GmodLookup.cpp
    gmod/BM_GmodPathParse.cpp
    gmod/BM_GmodTraversal.cpp
    gmod/BM_GmodVersioningConvertPath.cpp
    transport/BM_DataChannelListSerialization.cpp
    transport/BM_TimeSeriesDataSerialization.cpp
)

#----------------------------------------------
# Configure benchmark executables
#----------------------------------------------

foreach(benchmark_source ${benchmark_sources})
    get_filename_component(benchmark_target_name ${benchmark_source} NAME_WE)
    if(NOT TARGET ${benchmark_target_name})
        add_executable(${benchmark_target_name} ${benchmark_source})

        #----------------------------------------------
        # Target linking
        #----------------------------------------------

        target_link_libraries(${benchmark_target_name}
            PRIVATE
                benchmark::benchmark
                dnv-vista-sdk-cpp::static
                dnv-vista-cpp-embeddedresources
                dnv-vista-cpp-embeddedschemas
        )

        target_include_directories(${benchmark_target_name}
            PRIVATE
                ${DNV_VISTA_SDK_CPP_SOURCE_DIR}
        )

        #----------------------------------------------
        # Enable specific CPU features
        #----------------------------------------------

        target_compile_options(${benchmark_target_name}
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
        )

        #----------------------------------------------
        # Properties
        #----------------------------------------------

        set_target_properties(${benchmark_target_name}
            PROPERTIES
                CXX_STANDARD 20
                CXX_STANDARD_REQUIRED ON
                CXX_EXTENSIONS OFF
                POSITION_INDEPENDENT_CODE ON
                DEBUG_POSTFIX "-d"
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )
    endif()
endforeach()
