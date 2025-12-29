#==============================================================================
# dnv-vista-sdk-cpp - ISO19848VersionsGenerator executable CMake configuration
#==============================================================================

#----------------------------------------------
# Target definition
#----------------------------------------------

add_executable(dnv-vista-cpp-iso19848versionsgenerator
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SourceGenerators/ISO19848VersionsGenerator.cpp
)

#----------------------------------------------
# Link libraries
#----------------------------------------------

target_link_libraries(dnv-vista-cpp-iso19848versionsgenerator
    PRIVATE
        dnv-vista-cpp-embeddedresources
        nfx-stringbuilder::static
)

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(dnv-vista-cpp-iso19848versionsgenerator
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
        DEBUG_POSTFIX "-d"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)

#----------------------------------------------
# Code generation
#----------------------------------------------

set(DNV_VISTA_SDK_CPP_GENERATED_HEADER     "${DNV_VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/transport/ISO19848Versions.h")
set(DNV_VISTA_SDK_CPP_GENERATED_EXTENSIONS "${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/transport/ISO19848VersionsExtensions.h")

add_custom_command(
    OUTPUT "${DNV_VISTA_SDK_CPP_GENERATED_HEADER}" "${DNV_VISTA_SDK_CPP_GENERATED_EXTENSIONS}"
    COMMAND dnv-vista-cpp-iso19848versionsgenerator "${DNV_VISTA_SDK_CPP_GENERATED_HEADER}" "${DNV_VISTA_SDK_CPP_GENERATED_EXTENSIONS}" "${DNV_VISTA_SDK_CPP_VERSION}"
    COMMAND clang-format -i "${DNV_VISTA_SDK_CPP_GENERATED_HEADER}" || ${CMAKE_COMMAND} -E true
    COMMAND clang-format -i "${DNV_VISTA_SDK_CPP_GENERATED_EXTENSIONS}" || ${CMAKE_COMMAND} -E true
    DEPENDS dnv-vista-cpp-iso19848versionsgenerator
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMENT "Generating ISO19848Versions.h and ISO19848VersionsExtensions.h from embedded resources..."
    VERBATIM
)

add_custom_target(dnv-vista-cpp-generate-iso19848versions ALL
    DEPENDS "${DNV_VISTA_SDK_CPP_GENERATED_HEADER}" "${DNV_VISTA_SDK_CPP_GENERATED_EXTENSIONS}"
)
