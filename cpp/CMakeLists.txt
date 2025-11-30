#==============================================================================
# VISTA-SDK-CPP - Top-level CMake configuration
#==============================================================================

#----------------------------------------------
# Project definition
#----------------------------------------------

cmake_minimum_required(VERSION 3.20)

project(vista-sdk-cpp
	VERSION 0.0.80
	DESCRIPTION "DNV VISTA SDK C++ implementation"
	LANGUAGES CXX
)

# --- Determine Vista SDK root directory ---
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH VISTA_SDK_ROOT_DIR)

#----------------------------------------------
# Version configuration
#----------------------------------------------

set(VISTA_SDK_CPP_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE INTERNAL "vista-sdk-cpp major version")
set(VISTA_SDK_CPP_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE INTERNAL "vista-sdk-cpp mminor version")
set(VISTA_SDK_CPP_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE INTERNAL "vista-sdk-cpp mpatch version")
set(VISTA_SDK_CPP_VERSION       ${PROJECT_VERSION}       CACHE INTERNAL "vista-sdk-cpp mversion")

#----------------------------------------------
# Project metadata
#----------------------------------------------

set(VISTA_SDK_CPP_PROJECT_LICENSE "MIT")
set(VISTA_SDK_CPP_PROJECT_LICENSE_FILE "${VISTA_SDK_ROOT_DIR}/LICENSE")

#----------------------------------------------
# Standalone project detection
#----------------------------------------------

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	set(VISTA_SDK_CPP_STANDALONE_PROJECT ON)
else()
	set(VISTA_SDK_CPP_STANDALONE_PROJECT OFF)
endif()

option(VISTA_SDK_CPP_DEVELOPER_MODE "Enable developer mode" OFF)

#----------------------------------------------
# Testing setup
#----------------------------------------------

enable_testing()

#----------------------------------------------
# Build options
#----------------------------------------------

# --- Build optimization ---
option(VISTA_SDK_CPP_USE_CCACHE                 "Enable compiler cache"                      ON  )

# --- Development ---
set(VISTA_SDK_CPP_DEVELOPER_DEFAULT_SHARED      ${VISTA_SDK_CPP_STANDALONE_PROJECT})
set(VISTA_SDK_CPP_DEVELOPER_DEFAULT_TESTS       ${VISTA_SDK_CPP_STANDALONE_PROJECT})
set(VISTA_SDK_CPP_DEVELOPER_DEFAULT_SAMPLES     ${VISTA_SDK_CPP_STANDALONE_PROJECT})
set(VISTA_SDK_CPP_DEVELOPER_DEFAULT_BENCHMARKS  ${VISTA_SDK_CPP_STANDALONE_PROJECT})
set(VISTA_SDK_CPP_DEVELOPER_DEFAULT_DOCS        ${VISTA_SDK_CPP_STANDALONE_PROJECT})

# Override defaults for developer mode (library-focused development)
if(VISTA_SDK_CPP_DEVELOPER_MODE)
	set(VISTA_SDK_CPP_DEVELOPER_DEFAULT_SHARED      OFF )
	set(VISTA_SDK_CPP_DEVELOPER_DEFAULT_TESTS       ON  )
	set(VISTA_SDK_CPP_DEVELOPER_DEFAULT_SAMPLES     OFF )
	set(VISTA_SDK_CPP_DEVELOPER_DEFAULT_BENCHMARKS  OFF )
	set(VISTA_SDK_CPP_DEVELOPER_DEFAULT_DOCS        OFF )
endif()

option(VISTA_SDK_CPP_BUILD_TESTS                "Build tests"                                ${VISTA_SDK_CPP_DEVELOPER_DEFAULT_TESTS})
option(VISTA_SDK_CPP_BUILD_SMOKE_TESTS          "Build smoke tests"                          OFF  )
option(VISTA_SDK_CPP_BUILD_BENCHMARKS           "Build benchmarks"                           ${VISTA_SDK_CPP_DEVELOPER_DEFAULT_BENCHMARKS})
option(VISTA_SDK_CPP_BUILD_SAMPLES              "Build vista-sdk-cpp samples"                ${VISTA_SDK_CPP_DEVELOPER_DEFAULT_SAMPLES})
option(VISTA_SDK_CPP_BUILD_DOCUMENTATION        "Build vista-sdk-cpp Doxygen documentation"  OFF)
option(VISTA_SDK_CPP_USE_CUSTOM_COMPILER_FLAGS  "Skip automatic compiler configuration"      OFF  )

# --- Library build types ---
option(VISTA_SDK_CPP_BUILD_STATIC               "Build static library"                       ON   )
option(VISTA_SDK_CPP_BUILD_SHARED               "Build shared library"                       ${VISTA_SDK_CPP_DEVELOPER_DEFAULT_SHARED})

# --- Installation ---
option(VISTA_SDK_CPP_INSTALL_PROJECT            "Install project"                            ON   )

# --- Packaging ---
option(VISTA_SDK_CPP_PACKAGE_SOURCE             "Enable source package generation"           ON   )
option(VISTA_SDK_CPP_PACKAGE_ARCHIVE            "Enable TGZ/ZIP package generation"          ON   )
option(VISTA_SDK_CPP_PACKAGE_DEB                "Enable DEB package generation"              ON   )
option(VISTA_SDK_CPP_PACKAGE_RPM                "Enable RPM package generation"              ON   )
option(VISTA_SDK_CPP_PACKAGE_NSIS               "Enable NSIS Windows installer"              ON   )

# --- nfx-serialization extension support ---
option(NFX_SERIALIZATION_WITH_JSON              "Enable JSON serialization support"          ON   )
option(NFX_SERIALIZATION_WITH_DATETIME          "Enable DateTime type serialization"         ON   )
option(NFX_SERIALIZATION_WITH_DATATYPES         "Enable Decimal/Int128 serialization"        ON   )
option(NFX_SERIALIZATION_WITH_CONTAINERS        "Enable nfx-containers serialization"        ON   )


#----------------------------------------------
# Directory and CMake modules configuration
#----------------------------------------------


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#----------------------------------------------
# Main configuration
#----------------------------------------------

include(dnvVistaCppBuildConfig)
include(dnvVistaCppDependencies)
include(dnvVistaCppSources)
include(dnvVistaCppCompilersSettings)

#----------------------------------------------
# Installation and packaging
#----------------------------------------------

include(dnvVistaCppInstall)
include(dnvVistaCppPackaging)

#----------------------------------------------
# Targets
#----------------------------------------------

add_subdirectory(${VISTA_SDK_CPP_TEST_DIR} test)
add_subdirectory(${VISTA_SDK_CPP_SAMPLES_DIR} samples)
add_subdirectory(${VISTA_SDK_CPP_BENCHMARK_DIR} benchmark)
add_subdirectory(${VISTA_SDK_CPP_DOCUMENTATION_DIR} doc)

#----------------------------------------------
# Configuration summary
#----------------------------------------------

include(dnvVistaCppSummary)
