#==============================================================================
# dnv-vista-sdk-cpp - EmbeddedTestData test suite CMake configuration
#==============================================================================

#----------------------------------------------
# Target definition
#----------------------------------------------

add_executable(Tests_EmbeddedTestData
    Tests_EmbeddedTestData.cpp
)

#----------------------------------------------
# Link libraries
#----------------------------------------------

target_link_libraries(Tests_EmbeddedTestData
    PRIVATE
        GTest::gtest_main
        dnv-vista-cpp-embeddedtestdata
)

#----------------------------------------------
# Include directories
#----------------------------------------------

target_include_directories(Tests_EmbeddedTestData
    PRIVATE
        ${DNV_VISTA_SDK_CPP_SOURCE_DIR}
)

#----------------------------------------------
# Enable specific CPU features
#----------------------------------------------

if(DNV_VISTA_SDK_CPP_ENABLE_SIMD)
    target_compile_options(Tests_EmbeddedTestData
        PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
            $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
    )
endif()

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(Tests_EmbeddedTestData
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

#----------------------------------------------
# Test discovery and registration
#----------------------------------------------

gtest_discover_tests(Tests_EmbeddedTestData
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:Tests_EmbeddedTestData>"
    DISCOVERY_MODE POST_BUILD
    PROPERTIES
        TIMEOUT 120
        RUN_SERIAL OFF
)
