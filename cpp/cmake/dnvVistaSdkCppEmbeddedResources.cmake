#==============================================================================
# dnv-vista-sdk-cpp - EmbeddedResources library CMake configuration
#==============================================================================

nfx_embed_resources(
    TARGET dnv-vista-cpp-embeddedresources
    RESOURCE_DIR "${DNV_VISTA_SDK_CPP_RESOURCES_DIR}"
    OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated/dnv-vista-cpp-resources"
    NAMESPACE "dnv::vista::sdk::resources"
    REGISTRY_NAME "resources"
    PATTERN "*.gz"
)

target_sources(dnv-vista-cpp-embeddedresources
    PRIVATE
        ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/EmbeddedResources/EmbeddedResources.cpp
)

target_include_directories(dnv-vista-cpp-embeddedresources
    PUBLIC
        ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/EmbeddedResources
    PRIVATE
        ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/dto
)

target_link_libraries(dnv-vista-cpp-embeddedresources
    PRIVATE
        zlib-ng::zlib
        nfx::meta
)

target_compile_options(dnv-vista-cpp-embeddedresources
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
)
