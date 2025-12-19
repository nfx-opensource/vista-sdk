#==============================================================================
# dnv-vista-sdk-cpp - CMake Sources
#==============================================================================

#----------------------------------------------
# Source files
#----------------------------------------------

set(private_sources)

list(APPEND private_sources
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/internal/parsing/LocationParsingErrorBuilder.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/Codebook.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/Codebooks.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/ImoNumber.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/LocationBuilder.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/Locations.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/ParsingErrors.cpp
    ${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/VIS.cpp
)
