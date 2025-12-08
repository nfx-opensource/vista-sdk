#==============================================================================
# dnv-vista-sdk-cpp - EmbeddedResources test suite CMake configuration
#==============================================================================

#----------------------------------------------
# Target definition
#----------------------------------------------

add_executable(Tests_EmbeddedResources
    Tests_EmbeddedResources.cpp
)

#----------------------------------------------
# Include directories
#----------------------------------------------

target_include_directories(Tests_EmbeddedResources
    PRIVATE
        ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/dto
)

#----------------------------------------------
# Link libraries
#----------------------------------------------

target_link_libraries(Tests_EmbeddedResources
    PRIVATE
        GTest::gtest_main
        dnv-vista-cpp-embeddedresources
        nfx-serialization::static
)

#----------------------------------------------
# Enable specific CPU features
#----------------------------------------------

target_compile_options(Tests_EmbeddedResources
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
)

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(Tests_EmbeddedResources
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

gtest_discover_tests(Tests_EmbeddedResources
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:Tests_EmbeddedResources>"
    DISCOVERY_MODE POST_BUILD
    PROPERTIES
        TIMEOUT 120
        RUN_SERIAL OFF
)
