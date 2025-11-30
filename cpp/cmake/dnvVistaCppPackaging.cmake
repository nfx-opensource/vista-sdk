#==============================================================================
# VISTA-SDK-CPP - Library packaging configuration (CPack)
#==============================================================================

#----------------------------------------------
# Packaging condition check
#----------------------------------------------

if(NOT VISTA_SDK_CPP_PACKAGE_SOURCE)
	message(STATUS "Packaging disabled, skipping...")
	return()
endif()

#----------------------------------------------
# CPack configuration
#----------------------------------------------

# --- Common settings ---
set(CPACK_PACKAGE_NAME                  ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR                "DNV")
set(CPACK_PACKAGE_DIRECTORY             "${CMAKE_BINARY_DIR}/packages")
set(CPACK_PACKAGE_VERSION_MAJOR         ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR         ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH         ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION               ${PROJECT_VERSION})
#set(CPACK_PACKAGE_DESCRIPTION           "")
#set(CPACK_PACKAGE_DESCRIPTION_FILE      "")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   "DNV VISTA SDK C++ implementation for maritime data processing")
set(CPACK_PACKAGE_HOMEPAGE_URL          ${CMAKE_PROJECT_HOMEPAGE_URL})
#set(CPACK_PACKAGE_FILE_NAME             "")
set(CPACK_PACKAGE_INSTALL_DIRECTORY     ${CPACK_PACKAGE_NAME})
#set(CPACK_PACKAGE_ICON                  "")
set(CPACK_PACKAGE_CHECKSUM              "SHA256")
#set(CPACK_PROJECT_CONFIG_FILE           "")
set(CPACK_RESOURCE_FILE_LICENSE         "${VISTA_SDK_CPP_PROJECT_LICENSE_FILE}")
#set(CPACK_RESOURCE_FILE_README          "")
#set(CPACK_RESOURCE_FILE_WELCOME         "")
set(CPACK_MONOLITHIC_INSTALL            TRUE)
#set(CPACK_GENERATOR                     "")
#set(CPACK_OUTPUT_CONFIG_FILE            "")
#set(CPACK_PACKAGE_EXECUTABLES           "")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CPACK_STRIP_FILES               TRUE)
	message(STATUS "Release build: Debug symbols will be stripped from packages")
else()
	set(CPACK_STRIP_FILES               FALSE)
	message(STATUS "${CMAKE_BUILD_TYPE} build: Debug symbols preserved in packages")
endif()
set(CPACK_VERBATIM_VARIABLES            TRUE)
set(CPACK_THREADS                       ${VISTA_SDK_CPP_THREADS})

# --- Source package settings ---
set(CPACK_SOURCE_PACKAGE_FILE_NAME      "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-Source")
#set(CPACK_SOURCE_STRIP_FILES            "")
if(VISTA_SDK_CPP_PACKAGE_SOURCE)
	set(CPACK_SOURCE_GENERATOR          "TGZ;ZIP")
else()
	set(CPACK_SOURCE_GENERATOR          "")
endif()
#set(CPACK_SOURCE_OUTPUT_CONFIG_FILE     "")
set(CPACK_SOURCE_IGNORE_FILES           ".git/;.github;.gitignore;build/;.deps/;Testing/;.vs/;.vscode/;.*~$")

# --- Advanced settings ---
#set(CPACK_INSTALL_COMMANDS              "")
#set(CPACK_INSTALL_SCRIPTS               "")
#set(CPACK_PRE_BUILD_SCRIPTS             "")
#set(CPACK_POST_BUILD_SCRIPTS            "")
#set(CPACK_PACKAGE_FILES                 "")
#set(CPACK_INSTALLED_DIRECTORIES         "")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY  "vista-sdk-cpp")
#set(CPACK_CREATE_DESKTOP_LINKS          "")

#----------------------------------------------
# Platform-specific generators
#----------------------------------------------

# --- Generators ---
if(NOT DEFINED CPACK_GENERATOR)
	set(CPACK_GENERATOR "ZIP")

	# --- Archive packages ---
	if(VISTA_SDK_CPP_PACKAGE_ARCHIVE)
		if(UNIX AND NOT APPLE)
			set(CPACK_GENERATOR "${CPACK_GENERATOR};TGZ")
		elseif(WIN32)
			set(CPACK_GENERATOR "${CPACK_GENERATOR};ZIP")
		endif()
	endif()

	# --- DEB packages ---
	if(VISTA_SDK_CPP_PACKAGE_DEB AND UNIX AND NOT APPLE)
		find_program(DPKG_CMD dpkg)
		if(DPKG_CMD)
			set(CPACK_GENERATOR "${CPACK_GENERATOR};DEB")
		else()
			message(STATUS "DEB packaging disabled - install dpkg-dev for .deb support")
		endif()
	endif()

	# --- RPM packages ---
	if(VISTA_SDK_CPP_PACKAGE_RPM AND UNIX AND NOT APPLE)
		find_program(RPM_CMD rpm)
		if(RPM_CMD)
			set(CPACK_GENERATOR "${CPACK_GENERATOR};RPM")
		else()
			message(STATUS "RPM packaging disabled - install rpm for .rpm support")
		endif()
	endif()

	# --- NSIS packages ---
	if(VISTA_SDK_CPP_PACKAGE_NSIS AND WIN32)
		find_program(MAKENSIS_CMD makensis)
		if(MAKENSIS_CMD)
			set(CPACK_GENERATOR "${CPACK_GENERATOR};NSIS")
			message(STATUS "NSIS found: ${MAKENSIS_CMD} - Windows installer generation enabled")
		else()
			message(STATUS "NSIS not found - install NSIS for Windows installer support")
		endif()
	endif()

	# Clean up leading semicolon
	string(REGEX REPLACE "^;" "" CPACK_GENERATOR "${CPACK_GENERATOR}")
endif()

message(STATUS "CPack configured:")
message(STATUS "  Generators: ${CPACK_GENERATOR}")
message(STATUS "  Output dir: ${CPACK_PACKAGE_DIRECTORY}")

#----------------------------------------------
# Architecture detection
#----------------------------------------------

if("DEB" IN_LIST CPACK_GENERATOR AND UNIX AND NOT APPLE)
	if(NOT DEFINED CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
		find_program(DPKG_CMD dpkg)
		if(DPKG_CMD)
			execute_process(COMMAND ${DPKG_CMD} --print-architecture
				OUTPUT_VARIABLE DETECTED_DEB_ARCH
				OUTPUT_STRIP_TRAILING_WHITESPACE
				ERROR_QUIET)
			set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${DETECTED_DEB_ARCH}")
		endif()
	endif()

	message(STATUS "Debian package architecture: ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
endif()

if("RPM" IN_LIST CPACK_GENERATOR AND UNIX AND NOT APPLE)
	if(NOT DEFINED CPACK_RPM_PACKAGE_ARCHITECTURE)
		execute_process(COMMAND uname -m
			OUTPUT_VARIABLE DETECTED_RPM_ARCH
			OUTPUT_STRIP_TRAILING_WHITESPACE
			ERROR_QUIET)
		set(CPACK_RPM_PACKAGE_ARCHITECTURE "${DETECTED_RPM_ARCH}")
	endif()

	message(STATUS "RPM package architecture: ${CPACK_RPM_PACKAGE_ARCHITECTURE}")
endif()

#----------------------------------------------
# Package-specific settings
#----------------------------------------------

# --- DEB package settings ---
if("DEB" IN_LIST CPACK_GENERATOR AND UNIX AND NOT APPLE)
	set(CPACK_DEBIAN_PACKAGE_NAME          ${CPACK_PACKAGE_NAME})
	set(CPACK_DEBIAN_FILE_NAME             DEB-DEFAULT)
	set(CPACK_DEBIAN_PACKAGE_VERSION       ${PROJECT_VERSION})
	set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE  ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER    ${CPACK_PACKAGE_VENDOR})
	set(CPACK_DEBIAN_PACKAGE_DESCRIPTION   ${PROJECT_DESCRIPTION})
	set(CPACK_DEBIAN_PACKAGE_SECTION       "libs")
	set(CPACK_DEBIAN_COMPRESSION_TYPE      "xz")
	set(CPACK_DEBIAN_PACKAGE_PRIORITY      "optional")
	set(CPACK_DEBIAN_PACKAGE_HOMEPAGE      ${CPACK_PACKAGE_HOMEPAGE_URL})

	# --- Core runtime dependencies  ---
	set(DEB_DEPENDS "libc6, libstdc++6, libgcc-s1, zlib1g")

	set(CPACK_DEBIAN_PACKAGE_DEPENDS "${DEB_DEPENDS}")
	message(STATUS "DEB dependencies: ${CPACK_DEBIAN_PACKAGE_DEPENDS}")
endif()

# --- RPM package settings ---
if("RPM" IN_LIST CPACK_GENERATOR AND UNIX AND NOT APPLE)
	set(CPACK_RPM_PACKAGE_SUMMARY       ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
	set(CPACK_RPM_PACKAGE_NAME          ${CPACK_PACKAGE_NAME})
	set(CPACK_RPM_FILE_NAME             "RPM-DEFAULT")
	set(CPACK_RPM_PACKAGE_VERSION       ${PROJECT_VERSION})
	set(CPACK_RPM_PACKAGE_ARCHITECTURE  ${CPACK_RPM_PACKAGE_ARCHITECTURE})
	set(CPACK_RPM_PACKAGE_RELEASE       "1")
	set(CPACK_RPM_PACKAGE_LICENSE       ${VISTA_SDK_CPP_PROJECT_LICENSE$})
	set(CPACK_RPM_PACKAGE_GROUP         "Development/Libraries")
	set(CPACK_RPM_COMPRESSION_TYPE      "xz")
	set(CPACK_RPM_PACKAGE_VENDOR        ${CPACK_PACKAGE_VENDOR})
	set(CPACK_RPM_PACKAGE_DESCRIPTION   ${PROJECT_DESCRIPTION})
	set(CPACK_RPM_PACKAGE_URL           ${CPACK_PACKAGE_HOMEPAGE_URL})

	# --- Core runtime dependencies ---
	set(RPM_REQUIRES "glibc, libstdc++, zlib")

	set(CPACK_RPM_PACKAGE_REQUIRES "${RPM_REQUIRES}")
	message(STATUS "RPM dependencies: ${CPACK_RPM_PACKAGE_REQUIRES}")
endif()

# --- NSIS package settings ---
if("NSIS" IN_LIST CPACK_GENERATOR AND WIN32)
	set(CPACK_NSIS_INSTALL_ROOT                     "$PROGRAMFILES64")
	set(CPACK_NSIS_MUI_ICON                         ${CPACK_PACKAGE_ICON})
	set(CPACK_NSIS_MUI_UNIICON                      ${CPACK_PACKAGE_ICON})
	set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE          ${CPACK_PACKAGE_ICON})
#	set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP     "")
#	set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP   "")
#	set(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS        "")
#	set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS           "")
#	set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS         "")
#	set(CPACK_NSIS_COMPRESSOR                       "")
	set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL  TRUE)
	set(CPACK_NSIS_MODIFY_PATH                      TRUE)
	set(CPACK_NSIS_DISPLAY_NAME                     ${CPACK_PACKAGE_NAME})
	set(CPACK_NSIS_PACKAGE_NAME                     ${CPACK_PACKAGE_NAME})
	set(CPACK_NSIS_INSTALLED_ICON_NAME              "bin\\\\${CPACK_PACKAGE_NAME}.exe")
	set(CPACK_NSIS_HELP_LINK                        ${CPACK_PACKAGE_HOMEPAGE_URL})
	set(CPACK_NSIS_URL_INFO_ABOUT                   ${CPACK_PACKAGE_HOMEPAGE_URL})
	set(CPACK_NSIS_CONTACT                          ${CPACK_PACKAGE_VENDOR})
#	set(CPACK_NSIS_RUNTIME_INSTALL_DIRECTORY        "$INSTDIR\\bin")
#	set(CPACK_NSIS_HEADERS_INSTALL_DIRECTORY        "$INSTDIR\\include")
#	set(CPACK_NSIS_LIBRARIES_INSTALL_DIRECTORY      "$INSTDIR\\lib")
#	set(CPACK_NSIS_CREATE_ICONS_EXTRA               "")
#	set(CPACK_NSIS_DELETE_ICONS_EXTRA               "")
	set(CPACK_NSIS_EXECUTABLES_DIRECTORY            "bin")
#	set(CPACK_NSIS_MUI_FINISHPAGE_RUN               "")
#	set(CPACK_NSIS_MENU_LINKS                       "")
	set(CPACK_NSIS_UNINSTALL_NAME                   "Uninstall ${CPACK_PACKAGE_NAME}")
	set(CPACK_NSIS_WELCOME_TITLE                    "Welcome to ${CPACK_PACKAGE_NAME} Setup")
	set(CPACK_NSIS_WELCOME_TITLE_3LINES             FALSE)
	set(CPACK_NSIS_FINISH_TITLE                     "${CPACK_PACKAGE_NAME} Installation Complete")
	set(CPACK_NSIS_FINISH_TITLE_3LINES              FALSE)
#	set(CPACK_NSIS_MUI_HEADERIMAGE                  "")
	set(CPACK_NSIS_MANIFEST_DPI_AWARE               TRUE)
	set(CPACK_NSIS_BRANDING_TEXT                    "DNV VISTA SDK C++ Library")
	set(CPACK_NSIS_BRANDING_TEXT_TRIM_POSITION      "CENTER")  # Not supported on Linux NSIS
#	set(CPACK_NSIS_EXECUTABLE                       "")
	set(CPACK_NSIS_IGNORE_LICENSE_PAGE              FALSE)
#	set(CPACK_NSIS_EXECUTABLE_PRE_ARGUMENTS         "")
#	set(CPACK_NSIS_EXECUTABLE_POST_ARGUMENTS        "")
endif()

#----------------------------------------------
# Include CPack
#----------------------------------------------

include(CPack)
