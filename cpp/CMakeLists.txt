#==============================================================================
# dnv-vista-sdk-cpp - Top-level CMake configuration
#==============================================================================

#----------------------------------------------
# Project definition
#----------------------------------------------

cmake_minimum_required(VERSION 3.20)

project(dnv-vista-sdk-cpp
    VERSION 0.0.0
    DESCRIPTION "C++ implementation of DNV Vista SDK for VIS and ISO 19847/19848 standards"
    HOMEPAGE_URL "https://github.com/dnv-opensource/vista-sdk"
    LANGUAGES CXX
)

#----------------------------------------------
# Version configuration
#----------------------------------------------

set(DNV_VISTA_SDK_CPP_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE INTERNAL "dnv-vista-sdk-cpp major version")
set(DNV_VISTA_SDK_CPP_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE INTERNAL "dnv-vista-sdk-cpp minor version")
set(DNV_VISTA_SDK_CPP_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE INTERNAL "dnv-vista-sdk-cpp patch version")
set(DNV_VISTA_SDK_CPP_VERSION       ${PROJECT_VERSION}       CACHE INTERNAL "dnv-vista-sdk-cpp version")

#----------------------------------------------
# Testing setup
#----------------------------------------------

enable_testing()

#----------------------------------------------
# Build options
#----------------------------------------------

# --- Library build types ---
option(DNV_VISTA_SDK_CPP_BUILD_STATIC        "Build static library" ON)
option(DNV_VISTA_SDK_CPP_BUILD_SHARED        "Build shared library" OFF)

# --- Build components ---
option(DNV_VISTA_SDK_CPP_BUILD_TOOLS         "Build tools"          OFF)
option(DNV_VISTA_SDK_CPP_BUILD_TESTS         "Build tests"          OFF)
option(DNV_VISTA_SDK_CPP_BUILD_SAMPLES       "Build samples"        OFF)
option(DNV_VISTA_SDK_CPP_BUILD_BENCHMARKS    "Build benchmarks"     OFF)
option(DNV_VISTA_SDK_CPP_BUILD_DOCUMENTATION "Build documentation"  OFF)

# --- Linking options ---
option(DNV_VISTA_SDK_CPP_LINK_TESTS_SHARED   "Link samples, tests, tools and benchmarks to shared library instead of static" ON)

# --- Specialized test components ---
option(DNV_VISTA_SDK_CPP_BUILD_SMOKE_TESTS   "Build smoke tests"    OFF)

#----------------------------------------------
# CMake modules configuration
#----------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#----------------------------------------------
# Main configuration
#----------------------------------------------

include(dnvVistaSdkCppBuildConfig)
include(dnvVistaSdkCppDependencies)
include(dnvVistaSdkCppSources)
include(dnvVistaSdkCppTargets)

#----------------------------------------------
# Code generation
#----------------------------------------------

include(dnvVistaSdkCppEmbeddedResources)
include(dnvVistaSdkCppEmbeddedSchemas)
include(dnvVistaSdkCppVisVersionsGenerator)
include(dnvVistaSdkCppISO19848VersionsGenerator)

#----------------------------------------------
# Targets
#----------------------------------------------

add_subdirectory(tools)
add_subdirectory(test)
add_subdirectory(samples)
add_subdirectory(benchmark)
add_subdirectory(doc)
