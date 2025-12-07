#==============================================================================
# dnv-vista-sdk-cpp - EmbeddedSchemas library CMake configuration
#==============================================================================

nfx_embed_resources(
    TARGET dnv-vista-cpp-embeddedschemas
    RESOURCE_DIR "${DNV_VISTA_SDK_CPP_SCHEMAS_DIR}"
    OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated/dnv-vista-cpp-schemas"
    NAMESPACE "dnv::vista::sdk::schemas"
    REGISTRY_NAME "schemas"
    PATTERN "*.json" "*.xsd"
    RECURSE
)

target_sources(dnv-vista-cpp-embeddedschemas
    PRIVATE
        ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/EmbeddedSchemas/EmbeddedSchemas.cpp
)

target_include_directories(dnv-vista-cpp-embeddedschemas
    PUBLIC
        ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/EmbeddedSchemas
)
