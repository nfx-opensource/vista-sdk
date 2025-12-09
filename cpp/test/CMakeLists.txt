#==============================================================================
# dnv-vista-sdk-cpp - Test suite CMake configuration
#==============================================================================

#----------------------------------------------
# Test condition check
#----------------------------------------------

if(NOT DNV_VISTA_SDK_CPP_BUILD_TESTS)
	return()
endif()

#----------------------------------------------
# Embed test data
#----------------------------------------------

nfx_embed_resources(
    TARGET dnv-vista-cpp-embeddedtestdata
    RESOURCE_DIR "${DNV_VISTA_SDK_CPP_TESTDATA_DIR}"
    OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated/dnv-vista-cpp-testdata"
    NAMESPACE "dnv::vista::sdk::testdata"
    REGISTRY_NAME "testdata"
    PATTERN "*.json" "*.txt" "*.gz"
)

# Add the EmbeddedTestData helper class to the target
target_sources(dnv-vista-cpp-embeddedtestdata
    PRIVATE
        ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/EmbeddedTestData/EmbeddedTestData.cpp
)

target_include_directories(dnv-vista-cpp-embeddedtestdata
    PUBLIC
        ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/EmbeddedTestData
)

target_link_libraries(dnv-vista-cpp-embeddedtestdata
    PUBLIC
        nfx-serialization::static
)

#----------------------------------------------
# GoogleTest dependency
#----------------------------------------------

include(FetchContent)

set(DNV_VISTA_SDK_CPP_DEPS_GOOGLETEST_VERSION "1.17.0")

find_package(GTest ${DNV_VISTA_SDK_CPP_DEPS_GOOGLETEST_VERSION} QUIET)
if(NOT GTest_FOUND)
    set(_SAVED_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS           OFF CACHE BOOL "Build shared libraries"      FORCE)

    set(BUILD_GMOCK                 OFF CACHE BOOL "Build GMock"                 FORCE)
    set(INSTALL_GTEST               OFF CACHE BOOL "Install GoogleTest"          FORCE)
    set(GTEST_HAS_ABSL              OFF CACHE BOOL "Use Abseil library"          FORCE)
    set(gtest_build_samples         OFF CACHE BOOL "Build samples"               FORCE)
    set(gtest_build_tests           OFF CACHE BOOL "Build tests"                 FORCE)
    set(gtest_disable_pthreads      OFF CACHE BOOL "Disable pthreads"            FORCE)
    set(gtest_force_shared_crt      ON  CACHE BOOL "Force shared CRT on Windows" FORCE)
    set(gtest_hide_internal_symbols OFF CACHE BOOL "Hide internal symbols"       FORCE)

    FetchContent_Declare(
        googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v${DNV_VISTA_SDK_CPP_DEPS_GOOGLETEST_VERSION}
            GIT_SHALLOW    TRUE
    )

    set(BUILD_SHARED_LIBS ${_SAVED_BUILD_SHARED_LIBS} CACHE BOOL "Build shared libraries" FORCE)

    FetchContent_MakeAvailable(googletest)
endif()

#----------------------------------------------
# GoogleTest integration
#----------------------------------------------

include(GoogleTest)

#----------------------------------------------
# Parallel test configuration
#----------------------------------------------

include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
    set(CTEST_PARALLEL_LEVEL ${N})
    message(STATUS "Setting parallel test level to ${N} cores")
else()
    set(CTEST_PARALLEL_LEVEL 4)
    message(STATUS "Could not detect CPU count, setting parallel test level to 4")
endif()

#----------------------------------------------
# Tests source files
#----------------------------------------------

set(test_sources)

list(APPEND test_sources
    SDK/Tests_VisVersions.cpp
)

#----------------------------------------------
# Configure test executables
#----------------------------------------------

set(sdk_test_targets)

foreach(test_source ${test_sources})
    get_filename_component(test_target_name ${test_source} NAME_WE)

    if(NOT TARGET ${test_target_name})
        add_executable(${test_target_name} ${test_source})

        list(APPEND sdk_test_targets ${test_target_name})

        #----------------------------------------------
        # Target linking
        #----------------------------------------------

        target_link_libraries(${test_target_name}
            PRIVATE
                GTest::gtest_main
        )

        target_include_directories(${test_target_name}
            PRIVATE
                ${DNV_VISTA_SDK_CPP_INCLUDE_DIR}
        )

        #----------------------------------------------
        # Enable specific CPU features
        #----------------------------------------------

        target_compile_options(${test_target_name}
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
        )

        #----------------------------------------------
        # Properties
        #----------------------------------------------

        set_target_properties(${test_target_name}
            PROPERTIES
                CXX_STANDARD 20
                CXX_STANDARD_REQUIRED ON
                CXX_EXTENSIONS OFF
                POSITION_INDEPENDENT_CODE ON
                DEBUG_POSTFIX "-d"
                RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )

        #----------------------------------------------
        # Test discovery and registration
        #----------------------------------------------

        gtest_discover_tests(${test_target_name}
            WORKING_DIRECTORY "$<TARGET_FILE_DIR:${test_target_name}>"
            DISCOVERY_MODE POST_BUILD
            PROPERTIES
                TIMEOUT 120
                RUN_SERIAL OFF
        )
    endif()
endforeach()

#----------------------------------------------
# SDK test dependencies
#----------------------------------------------

# Ensure VisVersions.h is generated before building SDK tests
if(sdk_test_targets AND TARGET dnv-vista-cpp-generate-visversions)
    add_dependencies(${sdk_test_targets} dnv-vista-cpp-generate-visversions)
endif()

#----------------------------------------------
# SourceGenerator tests
#----------------------------------------------

add_subdirectory(EmbeddedResources)
add_subdirectory(EmbeddedSchemas)
add_subdirectory(EmbeddedTestData)
