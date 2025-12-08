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
