#==============================================================================
# dnv-vista-sdk-cpp - CPack packaging configuration
#==============================================================================

#----------------------------------------------
# Packaging condition check
#----------------------------------------------

if(NOT DNV_VISTA_SDK_CPP_ENABLE_PACKAGING)
    return()
endif()

#----------------------------------------------
# Package output directory
#----------------------------------------------

set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/packages")

#----------------------------------------------
# Basic package information
#----------------------------------------------

set(CPACK_PACKAGE_NAME "dnv-vista-sdk-cpp")
set(CPACK_PACKAGE_VENDOR "DNV")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ implementation of DNV Vista SDK for VIS and ISO 19847/19848 standards")
set(CPACK_PACKAGE_DESCRIPTION "DNV Vista SDK provides C++ implementations for working with VIS (Vessel Information Structure) and ISO 19847/19848 standards for maritime data exchange.")
set(CPACK_PACKAGE_VERSION_MAJOR ${DNV_VISTA_SDK_CPP_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${DNV_VISTA_SDK_CPP_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${DNV_VISTA_SDK_CPP_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "vista@dnv.com")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/dnv-opensource/vista-sdk")

#----------------------------------------------
# License and documentation
#----------------------------------------------

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

#----------------------------------------------
# Source package configuration
#----------------------------------------------

set(CPACK_SOURCE_PACKAGE_FILE_NAME "dnv-vista-sdk-cpp-${DNV_VISTA_SDK_CPP_VERSION}-source")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
    "/\\\\.git/"
    "/\\\\.github/"
    "/build/"
    "/\\\\.vscode/"
    "/\\\\.idea/"
    "/\\\\.cache/"
    "\\\\.swp$"
    "\\\\.orig$"
    "\\\\.DS_Store$"
    "/CMakeLists\\\\.txt\\\\.user$"
    "~$"
)

#----------------------------------------------
# Binary packages - TGZ
#----------------------------------------------

set(CPACK_GENERATOR "TGZ")
set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)

#----------------------------------------------
# DEB package configuration (Linux)
#----------------------------------------------

if(UNIX AND NOT APPLE)
    list(APPEND CPACK_GENERATOR "DEB")

    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "DNV <vista@dnv.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/dnv-opensource/vista-sdk")

    # Determine architecture
    execute_process(
        COMMAND dpkg --print-architecture
        OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    endif()

    set(CPACK_DEBIAN_FILE_NAME "dnv-vista-sdk-cpp_${DNV_VISTA_SDK_CPP_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")

    # Dependencies - disable automatic dependency detection
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)

    # Manual dependencies (adjust as needed)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.34), libstdc++6 (>= 12)")
endif()

#----------------------------------------------
# RPM package configuration (Linux)
#----------------------------------------------

if(UNIX AND NOT APPLE)
    list(APPEND CPACK_GENERATOR "RPM")

    set(CPACK_RPM_PACKAGE_LICENSE "Apache-2.0")
    set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
    set(CPACK_RPM_PACKAGE_URL "https://github.com/dnv-opensource/vista-sdk")
    set(CPACK_RPM_PACKAGE_DESCRIPTION "C++ implementation of DNV Vista SDK for VIS and ISO 19847/19848 standards")

    # Determine architecture
    execute_process(
        COMMAND uname -m
        OUTPUT_VARIABLE CPACK_RPM_PACKAGE_ARCHITECTURE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(NOT CPACK_RPM_PACKAGE_ARCHITECTURE)
        set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
    endif()

    set(CPACK_RPM_FILE_NAME "dnv-vista-sdk-cpp-${DNV_VISTA_SDK_CPP_VERSION}.${CPACK_RPM_PACKAGE_ARCHITECTURE}.rpm")

    # Disable automatic dependency detection
    set(CPACK_RPM_PACKAGE_AUTOREQ OFF)

    # Manual dependencies (adjust as needed)
    set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.34, libstdc++ >= 12")
endif()

#----------------------------------------------
# Include CPack
#----------------------------------------------

include(CPack)
