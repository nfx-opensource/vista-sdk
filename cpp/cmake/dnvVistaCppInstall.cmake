#==============================================================================
# VISTA-SDK-CPP - Library installation
#==============================================================================

#----------------------------------------------
# Installation condition check
#----------------------------------------------

if(NOT VISTA_SDK_CPP_INSTALL_PROJECT)
	message(STATUS "Installation disabled, skipping...")
	return()
endif()

#----------------------------------------------
# Installation paths configuration
#----------------------------------------------

include(GNUInstallDirs)

message(STATUS "System installation paths:")
message(STATUS "  Headers      : ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
message(STATUS "  Library      : ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
message(STATUS "  Binaries     : ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
message(STATUS "  CMake configs: ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake/dnv-vista-cpp")
message(STATUS "  Documentation: ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR}")

#----------------------------------------------
# Install headers
#----------------------------------------------

install(
	DIRECTORY "${VISTA_SDK_CPP_INCLUDE_DIR}/"
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	COMPONENT Development
	FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl"
)

#----------------------------------------------
# Install library targets
#----------------------------------------------

set(INSTALL_TARGETS)

if(VISTA_SDK_CPP_BUILD_SHARED)
	list(APPEND INSTALL_TARGETS ${PROJECT_NAME})
endif()

if(VISTA_SDK_CPP_BUILD_STATIC)
	list(APPEND INSTALL_TARGETS ${PROJECT_NAME}-static nfx-datetime-static)
endif()

if(INSTALL_TARGETS)
	install(
		TARGETS ${INSTALL_TARGETS}
		EXPORT dnv-vista-cpp-targets
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
			COMPONENT Development
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
			COMPONENT Runtime
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
			COMPONENT Runtime
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)
endif()

#----------------------------------------------
# Install CMake config files
#----------------------------------------------

install(
	EXPORT dnv-vista-cpp-targets
	FILE dnv-vista-cpp-targets.cmake
	NAMESPACE dnv-vista-cpp::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dnv-vista-cpp
	COMPONENT Development
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/dnv-vista-cpp-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/dnv-vista-cpp-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/dnv-vista-cpp-config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dnv-vista-cpp
)

install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/dnv-vista-cpp-config.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/dnv-vista-cpp-config-version.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dnv-vista-cpp
	COMPONENT Development
)

#----------------------------------------------
# Install license files
#----------------------------------------------

install(
	FILES "${VISTA_SDK_ROOT_DIR}/LICENSE"
	DESTINATION "${CMAKE_INSTALL_DOCDIR}/licenses"
)

install(
	DIRECTORY "${VISTA_SDK_CPP_ROOT_DIR}/licenses/"
	DESTINATION "${CMAKE_INSTALL_DOCDIR}/licenses"
	FILES_MATCHING PATTERN "LICENSE-*"
)

#----------------------------------------------
# Install documentation
#----------------------------------------------

if(VISTA_SDK_CPP_BUILD_DOCUMENTATION)
	install(
		DIRECTORY "${VISTA_SDK_CPP_BUILD_DIR}/doc/"
		DESTINATION ${CMAKE_INSTALL_DOCDIR}
		OPTIONAL
		COMPONENT Documentation
	)
endif()

message(STATUS "Installation configured for targets: ${INSTALL_TARGETS}")
