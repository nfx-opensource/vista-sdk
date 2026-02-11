#==============================================================================
# dnv-vista-sdk-cpp - Dependencies configuration
#==============================================================================

#----------------------------------------------
# Output configuration
#----------------------------------------------

set(_SAVED_CMAKE_REQUIRED_QUIET    ${CMAKE_REQUIRED_QUIET})
set(_SAVED_CMAKE_MESSAGE_LOG_LEVEL ${CMAKE_MESSAGE_LOG_LEVEL})
set(_SAVED_CMAKE_FIND_QUIETLY      ${CMAKE_FIND_QUIETLY})

set(CMAKE_REQUIRED_QUIET    ON     )
set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE) # [ERROR, WARNING, NOTICE, STATUS, VERBOSE, DEBUG]
set(CMAKE_FIND_QUIETLY      ON     )

#----------------------------------------------
# Dependency versions
#----------------------------------------------

set(DNV_VISTA_SDK_CPP_DEPS_ZLIB_NG_VERSION "2.3.3")
set(DNV_VISTA_SDK_CPP_NFX_META_VERSION     "1.0.0")

#----------------------------------------------
# FetchContent dependencies
#----------------------------------------------

include(FetchContent)

if(DEFINED ENV{CI})
    set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
else()
    set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
endif()
set(FETCHCONTENT_QUIET OFF)

# --- zlib-ng ---
find_package(zlib-ng ${DNV_VISTA_SDK_CPP_DEPS_ZLIB_NG_VERSION} EXACT QUIET)
if(NOT zlib-ng_FOUND)
    set(_SAVED_BUILD_SHARED_LIBS                ${BUILD_SHARED_LIBS})
    set(_SAVED_CMAKE_POSITION_INDEPENDENT_CODE  ${CMAKE_POSITION_INDEPENDENT_CODE})

    set(BUILD_SHARED_LIBS               OFF CACHE BOOL "Build shared libraries"               FORCE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON  CACHE BOOL "Build with -fPIC"                     FORCE)

    set(ZLIB_COMPAT                     OFF CACHE BOOL "Compile with zlib compatible API"     FORCE)
    set(BUILD_TESTING                   OFF CACHE BOOL "Build zlib-ng test binaries"          FORCE)
    set(ZLIBNG_ENABLE_TESTS             OFF CACHE BOOL "Build zlib-ng test binaries"          FORCE)
    set(WITH_GZFILEOP                   ON  CACHE BOOL "Support for gzFile related functions" FORCE)
    set(WITH_OPTIM                      ON  CACHE BOOL "Build with optimisations"             FORCE)
    set(WITH_NEW_STRATEGIES             ON  CACHE BOOL "Use new strategies"                   FORCE)
    set(WITH_NATIVE_INSTRUCTIONS        OFF CACHE BOOL "Use host-specific instructions"       FORCE)
    set(WITH_RUNTIME_CPU_DETECTION      OFF CACHE BOOL "Runtime CPU detection"                FORCE)
    set(WITH_SANITIZER                  OFF CACHE BOOL "Build with sanitizer"                 FORCE)
    set(WITH_GTEST                      OFF CACHE BOOL "Build gtest_zlib"                     FORCE)
    set(WITH_FUZZERS                    OFF CACHE BOOL "Build test/fuzz"                      FORCE)
    set(WITH_BENCHMARKS                 OFF CACHE BOOL "Build test/benchmarks"                FORCE)
    set(WITH_MAINTAINER_WARNINGS        OFF CACHE BOOL "Build with maintainer warnings"       FORCE)
    set(WITH_CODE_COVERAGE              OFF CACHE BOOL "Enable code coverage reporting"       FORCE)

    FetchContent_Declare(
        zlib-ng
            GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng.git
            GIT_TAG        ${DNV_VISTA_SDK_CPP_DEPS_ZLIB_NG_VERSION}
            GIT_SHALLOW    TRUE
    )
endif()

if(NOT zlib-ng_FOUND)
    FetchContent_MakeAvailable(zlib-ng)
    set(BUILD_SHARED_LIBS ${_SAVED_BUILD_SHARED_LIBS}                             CACHE BOOL "Build shared libraries" FORCE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ${_SAVED_CMAKE_POSITION_INDEPENDENT_CODE} CACHE BOOL "Build with -fPIC"       FORCE)
endif()

# --- nfx-meta ---
set(NFX_META_ENABLE_CONTAINERS       ON CACHE BOOL "Enable nfx-containers library"     FORCE)
set(NFX_META_ENABLE_DATATYPES        ON CACHE BOOL "Enable nfx-datatypes library"      FORCE)
set(NFX_META_ENABLE_DATETIME         ON CACHE BOOL "Enable nfx-datetime library"       FORCE)
set(NFX_META_ENABLE_RESOURCE         ON CACHE BOOL "Enable nfx-resource library"       FORCE)
set(NFX_META_ENABLE_SERIALIZATION    ON CACHE BOOL "Enable nfx-serialization library"  FORCE)
set(NFX_META_ENABLE_STRINGBUILDER    ON CACHE BOOL "Enable nfx-stringbuilder library"  FORCE)
set(NFX_META_ENABLE_STRINGUTILS      ON CACHE BOOL "Enable nfx-stringutils library"    FORCE)
set(NFX_META_WITH_JSON_SERIALIZATION ON CACHE BOOL "Enable JSON serialization support" FORCE)

FetchContent_Declare(
    nfx-meta
            GIT_REPOSITORY https://github.com/nfx-libs/nfx-meta.git
            GIT_TAG        ${DNV_VISTA_SDK_CPP_NFX_META_VERSION}
            GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(nfx-meta)

#----------------------------------------------
# Cleanup
#----------------------------------------------

set(CMAKE_REQUIRED_QUIET    ${_SAVED_CMAKE_REQUIRED_QUIET})
set(CMAKE_MESSAGE_LOG_LEVEL ${_SAVED_CMAKE_MESSAGE_LOG_LEVEL})
set(CMAKE_FIND_QUIETLY      ${_SAVED_CMAKE_FIND_QUIETLY})
