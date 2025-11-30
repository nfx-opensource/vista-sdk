#==============================================================================
# VISTA-SDK-CPP - Configuration Summary
#==============================================================================

#----------------------------------------------
# Configuration summary
#----------------------------------------------

message(STATUS "=========================================================")
message(STATUS "====== VISTA SDK C++ library configuration summary ======")
message(STATUS "=========================================================")
message(STATUS "")

#----------------------------
# Project information
#----------------------------

message(STATUS "--- Project information ---")
message(STATUS "Project                         : ${PROJECT_NAME}")
message(STATUS "Version                         : ${PROJECT_VERSION}")
message(STATUS "Description                     : ${PROJECT_DESCRIPTION}")
message(STATUS "")

#----------------------------
# Directory paths
#----------------------------

message(STATUS "--- Directory paths ---")
message(STATUS "Main Vista SDK root directory   : ${VISTA_SDK_ROOT_DIR}")
message(STATUS "Main Vista SDK resources dir    : ${VISTA_SDK_RESOURCES_DIR}")
message(STATUS "Main Vista SDK test data dir    : ${VISTA_SDK_TESTDATA_DIR}")
message(STATUS "Vista c++ SDK root directory    : ${VISTA_SDK_CPP_ROOT_DIR}")
message(STATUS "Vista c++ SDK include directory : ${VISTA_SDK_CPP_INCLUDE_DIR}")
message(STATUS "Vista c++ SDK source directory  : ${VISTA_SDK_CPP_SOURCE_DIR}")
message(STATUS "Vista c++ SDK build directory   : ${VISTA_SDK_CPP_BUILD_DIR}")
message(STATUS "Install prefix                  : ${VISTA_SDK_CPP_INSTALL_DIR}")
message(STATUS "")

#----------------------------
# Build options
#----------------------------

message(STATUS "--- Build options ---")
if(VISTA_SDK_CPP_BUILD_STATIC AND VISTA_SDK_CPP_BUILD_SHARED)
	message(STATUS "Library Types                   : Static + Shared")
elseif(VISTA_SDK_CPP_BUILD_STATIC)
	message(STATUS "Library Type                    : Static")
elseif(VISTA_SDK_CPP_BUILD_SHARED)
	message(STATUS "Library Type                    : Shared")
endif()
message(STATUS "Build tests                     : ${VISTA_SDK_CPP_BUILD_TESTS}")
message(STATUS "Build smoke tests               : ${VISTA_SDK_CPP_BUILD_SMOKE_TESTS}")
message(STATUS "Build benchmarks                : ${VISTA_SDK_CPP_BUILD_BENCHMARKS}")
message(STATUS "Build samples                   : ${VISTA_SDK_CPP_BUILD_SAMPLES}")
message(STATUS "Build documentation             : ${VISTA_SDK_CPP_BUILD_DOCUMENTATION}")
message(STATUS "")

#----------------------------
# Dependencies
#----------------------------

message(STATUS "--- Dependencies ---")
message(STATUS "nfx-meta                        : ${NFX_META_VERSION}")
message(STATUS "zlib-ng version                 : ${ZLIBNG_HEADER_VERSION}")
message(STATUS "GoogleTest version              : ${GTEST_VERSION}")
message(STATUS "Google Benchmark version        : ${BENCHMARK_VERSION}")
message(STATUS "")

#----------------------------
# CMake configuration
#----------------------------

message(STATUS "--- CMake configuration ---")
message(STATUS "CMake version                   : ${CMAKE_VERSION}")
message(STATUS "Generator                       : ${CMAKE_GENERATOR}")
message(STATUS "Build configurations            : ${CMAKE_CONFIGURATION_TYPES}")
message(STATUS "")

#----------------------------
# Compiler configuration
#----------------------------

message(STATUS "--- Compiler configuration ---")
message(STATUS "C++ compiler                    : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ compiler path               : ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ standard                    : ${CMAKE_CXX_STANDARD}")
message(STATUS "")

#----------------------------
# Compile and link flags
#----------------------------

message(STATUS "--- Compile and link flags ---")

# --- Compile flags ---
if(CMAKE_CXX_FLAGS)
	message(STATUS "Default compile flags           : ${CMAKE_CXX_FLAGS}")
else()
	message(STATUS "Default compile flags           : (none)")
endif()
message(STATUS "Custom compile flags            : ${FORMATTED_COMPILE_FLAGS}")

# --- Linker flags ---
if(CMAKE_CXX_LINK_FLAGS)
	message(STATUS "Default link flags              : ${CMAKE_CXX_LINK_FLAGS}")
else()
	message(STATUS "Default link flags              : (none)")
endif()
message(STATUS "Custom link options             : ${FORMATTED_LINK_OPTIONS}")

# --- Definitions ---
message(STATUS "Compile definitions             : ${FORMATTED_COMPILE_DEFINITIONS}")
message(STATUS "")

