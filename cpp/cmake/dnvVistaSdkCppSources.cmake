#==============================================================================
# dnv-vista-sdk-cpp - CMake Sources
#==============================================================================

#----------------------------------------------
# Source files
#----------------------------------------------

set(private_sources)

list(APPEND private_sources
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/internal/parsing/LocalIdParsingErrorBuilder.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/internal/parsing/LocationParsingErrorBuilder.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/internal/versioning/GmodVersioning.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/transport/datachannel/DataChannel.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/transport/ISO19848.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/transport/ShipId.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/Codebook.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/Codebooks.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/Gmod.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/GmodNode.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/GmodPath.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/GmodPathQuery.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/ImoNumber.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/LocalId.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/LocalIdBuilder.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/LocalIdQuery.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/LocationBuilder.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/Locations.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/MetadataTagsQuery.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/ParsingErrors.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/UniversalId.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/UniversalIdBuilder.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/VIS.cpp
)
