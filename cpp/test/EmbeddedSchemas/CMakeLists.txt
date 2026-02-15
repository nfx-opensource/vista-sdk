#==============================================================================
# dnv-vista-sdk-cpp - EmbeddedSchemas test suite CMake configuration
#==============================================================================

#----------------------------------------------
# Target definition
#----------------------------------------------

add_executable(Tests_EmbeddedSchemas
    Tests_EmbeddedSchemas.cpp
)

#----------------------------------------------
# Link libraries
#----------------------------------------------

target_link_libraries(Tests_EmbeddedSchemas
    PRIVATE
        GTest::gtest_main
        dnv-vista-cpp-embeddedschemas
)

#----------------------------------------------
# Include directories
#----------------------------------------------

target_include_directories(Tests_EmbeddedSchemas
    PRIVATE
        ${DNV_VISTA_SDK_CPP_SOURCE_DIR}
)

#----------------------------------------------
# Enable specific CPU features
#----------------------------------------------

if(DNV_VISTA_SDK_CPP_ENABLE_SIMD)
    target_compile_options(Tests_EmbeddedSchemas
        PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
            $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
    )
endif()

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(Tests_EmbeddedSchemas
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

gtest_discover_tests(Tests_EmbeddedSchemas
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:Tests_EmbeddedSchemas>"
    DISCOVERY_MODE POST_BUILD
    PROPERTIES
        TIMEOUT 120
        RUN_SERIAL OFF
)
