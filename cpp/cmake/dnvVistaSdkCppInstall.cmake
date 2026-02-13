#==============================================================================
# dnv-vista-sdk-cpp - Installation configuration
#==============================================================================

#----------------------------------------------
# Install condition check
#----------------------------------------------

if(NOT DNV_VISTA_SDK_CPP_INSTALL_PROJECT)
    return()
endif()

#----------------------------------------------
# CMake utilities
#----------------------------------------------

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

#----------------------------------------------
# Create install target that depends on documentation
#----------------------------------------------

if(TARGET vista-sdk-cpp-documentation)
    # Create a custom target for installation that depends on documentation
    add_custom_target(vista-sdk-cpp-install-with-docs
        DEPENDS vista-sdk-cpp-documentation
    )
endif()

#----------------------------------------------
# Install library targets
#----------------------------------------------

# Install static library if built
if(TARGET dnv-vista-sdk-cpp-static)
    install(TARGETS dnv-vista-sdk-cpp-static
        EXPORT dnv-vista-sdk-cpp-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

# Install shared library if built
if(TARGET dnv-vista-sdk-cpp)
    install(TARGETS dnv-vista-sdk-cpp
        EXPORT dnv-vista-sdk-cpp-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

#----------------------------------------------
# Install public headers
#----------------------------------------------

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl"
)

#----------------------------------------------
# Install generated headers
#----------------------------------------------

install(DIRECTORY "${CMAKE_BINARY_DIR}/generated/include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
)

#----------------------------------------------
# Install CMake package files
#----------------------------------------------

# Generate and install targets file
install(EXPORT dnv-vista-sdk-cpp-targets
    FILE dnv-vista-sdk-cpp-targets.cmake
    NAMESPACE dnv::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dnv-vista-sdk-cpp
)

# Generate package configuration file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/dnv-vista-sdk-cpp-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/dnv-vista-sdk-cpp-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dnv-vista-sdk-cpp
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)

# Generate package version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/dnv-vista-sdk-cpp-config-version.cmake
    VERSION ${DNV_VISTA_SDK_CPP_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install package files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/dnv-vista-sdk-cpp-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/dnv-vista-sdk-cpp-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dnv-vista-sdk-cpp
)

#----------------------------------------------
# Install documentation (if available)
#----------------------------------------------

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE")
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE"
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
    )
endif()

#----------------------------------------------
# Install Doxygen documentation (if generated)
#----------------------------------------------

install(DIRECTORY "${CMAKE_BINARY_DIR}/doc/html"
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    OPTIONAL
)

# Install index.html shortcut (symlink on Unix, batch file on Windows)
if(UNIX)
    install(FILES "${CMAKE_BINARY_DIR}/doc/index.html"
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        OPTIONAL
    )
else()
    install(FILES "${CMAKE_BINARY_DIR}/doc/index.html.cmd"
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        OPTIONAL
    )
endif()
