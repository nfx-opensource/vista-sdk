#==============================================================================
# dnv-vista-sdk-cpp - CMake build configuration
#==============================================================================

#----------------------------------------------
# Build validation
#----------------------------------------------

# --- Validate CMake version ---
if(CMAKE_VERSION VERSION_LESS "3.20")
    message(FATAL_ERROR "CMake 3.20 or higher is required for reliable C++20 support")
endif()

# --- Prevent in-source builds ---
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

# --- Ensure at least one library type is built ---
if(NOT DNV_VISTA_SDK_CPP_BUILD_STATIC AND NOT DNV_VISTA_SDK_CPP_BUILD_SHARED)
    message(WARNING "Neither DNV_VISTA_SDK_CPP_BUILD_STATIC nor DNV_VISTA_SDK_CPP_BUILD_SHARED is enabled.")

    if(DEFINED CACHE{DNV_VISTA_SDK_CPP_BUILD_STATIC} AND DEFINED CACHE{DNV_VISTA_SDK_CPP_BUILD_SHARED})
        message(STATUS "Both library types were explicitly disabled.")
        message(STATUS "Applying fallback: Enabling static library build")
        set(DNV_VISTA_SDK_CPP_BUILD_STATIC ON CACHE BOOL "Build static library (fallback)" FORCE)
    else()
        message(STATUS "Defaulting to static library build")
        set(DNV_VISTA_SDK_CPP_BUILD_STATIC ON)
    endif()
endif()

#----------------------------------------------
# Multi-config generator setup
#----------------------------------------------

# --- For multi-config generators, set available configurations ---
if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Release;Debug;RelWithDebInfo;MinSizeRel" CACHE STRING "Available build configurations" FORCE)
    message(STATUS "Multi-config generator detected. Available configurations: ${CMAKE_CONFIGURATION_TYPES}")
else()
    # --- For single-config generators (Makefiles, Ninja), set default build type ---
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
        message(STATUS "Single-config generator detected. Defaulting to optimized build type: ${CMAKE_BUILD_TYPE}")
    else()
        message(STATUS "Single-config generator detected. Using specified build type: ${CMAKE_BUILD_TYPE}")
    endif()
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Debug" "RelWithDebInfo" "MinSizeRel")
endif()

#----------------------------------------------
# Directory configuration
#----------------------------------------------

# --- Vista SDK root ---
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH VISTA_SDK_ROOT)

set(DNV_VISTA_SDK_CPP_RESOURCES_DIR "${VISTA_SDK_ROOT}/resources" CACHE PATH "Vista SDK resources directory")
set(DNV_VISTA_SDK_CPP_SCHEMAS_DIR   "${VISTA_SDK_ROOT}/schemas"   CACHE PATH "Vista SDK schemas directory")
set(DNV_VISTA_SDK_CPP_TESTDATA_DIR  "${VISTA_SDK_ROOT}/testdata"  CACHE PATH "Vista SDK test data directory")

# --- Vista SDK cpp ---
set(DNV_VISTA_SDK_CPP_DIR         "${PROJECT_SOURCE_DIR}"            CACHE PATH "C++ SDK root directory")
set(DNV_VISTA_SDK_CPP_INCLUDE_DIR "${DNV_VISTA_SDK_CPP_DIR}/include" CACHE PATH "C++ include directory")
set(DNV_VISTA_SDK_CPP_SOURCE_DIR  "${DNV_VISTA_SDK_CPP_DIR}/src"     CACHE PATH "C++ source directory")

#----------------------------------------------
# Output directory configuration
#----------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# --- Override multi-config generator subdirectories (MSVC adds Debug/Release by default) ---
foreach(config_type ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config_type} config_upper)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/lib)
endforeach()
