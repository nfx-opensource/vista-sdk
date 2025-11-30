#==============================================================================
# VISTA-SDK-CPP - C++ Library dependencies configuration
#==============================================================================

#----------------------------
# Output configuration
#----------------------------

set(_SAVED_CMAKE_REQUIRED_QUIET     ${CMAKE_REQUIRED_QUIET})
set(_SAVED_CMAKE_MESSAGE_LOG_LEVEL  ${CMAKE_MESSAGE_LOG_LEVEL})
set(_SAVED_CMAKE_FIND_QUIETLY       ${CMAKE_FIND_QUIETLY})

set(CMAKE_REQUIRED_QUIET    ON         )
set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE    ) # [ERROR, WARNING, NOTICE, STATUS, VERBOSE, DEBUG]
set(CMAKE_FIND_QUIETLY      ON         )

#----------------------------------------------
# FetchContent dependencies
#----------------------------------------------

include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/.deps/${COMPILER_DIR_NAME}")

if(DEFINED ENV{CI})
	set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
else()
	set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
endif()
set(FETCHCONTENT_QUIET OFF)

#----------------------------
# Dependency version requirements
#----------------------------

set(VISTA_SDK_CPP_NFX_META_MIN_VERSION   "0.7.0" )
set(VISTA_SDK_CPP_ZLIB_NG_VERSION        "2.2.0" )
set(VISTA_SDK_CPP_GTEST_MIN_VERSION      "1.12.1")
set(VISTA_SDK_CPP_BENCHMARK_MIN_VERSION  "1.9.1" )

#----------------------------
# Dependency declarations
#----------------------------

# --- nfx-meta ---
find_package(nfx-meta ${VISTA_SDK_CPP_NFX_META_MIN_VERSION} QUIET)
if(NOT nfx-meta_FOUND)
	set(NFX_META_BUILD_STATIC         ON   CACHE BOOL  "Build static library"                FORCE)
	set(NFX_META_BUILD_SHARED         OFF  CACHE BOOL  "Build shared library"                FORCE)
	set(NFX_META_WITH_CONTAINERS      ON   CACHE BOOL  "Enable container utilities"          FORCE)
	set(NFX_META_WITH_DATATYPES       ON   CACHE BOOL  "Enable mathematical datatypes"       FORCE)
	set(NFX_META_WITH_MEMORY          ON   CACHE BOOL  "Enable memory management utilities"  FORCE)
	set(NFX_META_WITH_JSON            ON   CACHE BOOL  "Enable JSON serialization support"   FORCE)
	set(NFX_META_WITH_STRING          ON   CACHE BOOL  "Enable string utilities"             FORCE)
	set(NFX_META_WITH_TIME            ON   CACHE BOOL  "Enable temporal classes"             FORCE)
	set(NFX_META_BUILD_TESTS          OFF  CACHE BOOL  "Build tests"                         FORCE)
	set(NFX_META_BUILD_SAMPLES        OFF  CACHE BOOL  "Build samples"                       FORCE)
	set(NFX_META_BUILD_BENCHMARKS     OFF  CACHE BOOL  "Build benchmarks"                    FORCE)
	set(NFX_META_BUILD_DOCUMENTATION  OFF  CACHE BOOL  "Build Doxygen documentation"         FORCE)
	set(NFX_META_INSTALL_PROJECT      OFF  CACHE BOOL  "Install project"                     FORCE)
	set(NFX_META_PACKAGE_SOURCE       OFF  CACHE BOOL  "Enable source package generation"    FORCE)
	set(NFX_META_PACKAGE_ARCHIVE      OFF  CACHE BOOL  "Enable TGZ/ZIP package generation"   FORCE)
	set(NFX_META_PACKAGE_DEB          OFF  CACHE BOOL  "Enable DEB package generation"       FORCE)
	set(NFX_META_PACKAGE_RPM          OFF  CACHE BOOL  "Enable RPM package generation"       FORCE)
	set(NFX_META_PACKAGE_NSIS         OFF  CACHE BOOL  "Enable NSIS Windows installer"       FORCE)

	FetchContent_Declare(
		nfx-meta
		GIT_REPOSITORY https://github.com/ronan-fdev/____nfx-meta.git
		GIT_TAG        main
		GIT_SHALLOW    TRUE
	)
endif()

# --- zlib-ng ---
find_package(zlib-ng ${VISTA_SDK_CPP_ZLIB_NG_VERSION} QUIET)
if(NOT zlib-ng_FOUND)
	set(_SAVED_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
	set(BUILD_SHARED_LIBS           OFF  CACHE BOOL  "Build shared libraries"                 FORCE)

	set(ZLIB_COMPAT                 OFF  CACHE BOOL  "Compile with zlib compatible API"       FORCE)
	set(ZLIB_ENABLE_TESTS           OFF  CACHE BOOL  "Build zlib-ng test binaries"            FORCE)
	set(ZLIBNG_ENABLE_TESTS         OFF  CACHE BOOL  "Build zlib-ng test binaries"            FORCE)
	set(WITH_GZFILEOP               ON   CACHE BOOL  "Support for gzFile related functions"   FORCE)
	set(WITH_OPTIM                  ON   CACHE BOOL  "Build with optimisations"               FORCE)
	set(WITH_NEW_STRATEGIES         ON   CACHE BOOL  "Use new strategies"                     FORCE)
	set(WITH_NATIVE_INSTRUCTIONS    OFF  CACHE BOOL  "Use host-specific instructions"         FORCE)
	set(WITH_RUNTIME_CPU_DETECTION  OFF  CACHE BOOL  "Runtime CPU detection"                  FORCE)
	set(WITH_SANITIZER              OFF  CACHE BOOL  "Build with sanitizer"                   FORCE)
	set(WITH_GTEST                  OFF  CACHE BOOL  "Build gtest_zlib"                       FORCE)
	set(WITH_FUZZERS                OFF  CACHE BOOL  "Build test/fuzz"                        FORCE)
	set(WITH_BENCHMARKS             OFF  CACHE BOOL  "Build test/benchmarks"                  FORCE)
	set(WITH_MAINTAINER_WARNINGS    OFF  CACHE BOOL  "Build with maintainer warnings"         FORCE)
	set(WITH_CODE_COVERAGE          OFF  CACHE BOOL  "Enable code coverage reporting"         FORCE)

	FetchContent_Declare(
		zlib-ng
		GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng.git
		GIT_TAG        2.2.5
		GIT_SHALLOW    TRUE
	)

	set(BUILD_SHARED_LIBS ${_SAVED_BUILD_SHARED_LIBS} CACHE BOOL "Build shared libraries" FORCE)
endif()

# --- Google test ---
if(VISTA_SDK_CPP_BUILD_TESTS)
	find_package(GTest ${VISTA_SDK_CPP_GTEST_MIN_VERSION}  QUIET)
	if(NOT GTest_FOUND)
		set(BUILD_GMOCK                     OFF  CACHE BOOL  "Build GoogleMock library"              FORCE)
		set(INSTALL_GTEST                   OFF  CACHE BOOL  "Install GoogleTest targets"            FORCE)
		set(GTEST_HAS_ABSL                  OFF  CACHE BOOL  "Disable Abseil support in GoogleTest"  FORCE)

		FetchContent_Declare(
			googletest
			GIT_REPOSITORY https://github.com/google/googletest.git
			GIT_TAG        v1.17.0
			GIT_SHALLOW    TRUE
		)
	endif()
endif()


# --- Google benchmark ---
if(VISTA_SDK_CPP_BUILD_BENCHMARKS)
	find_package(benchmark ${VISTA_SDK_CPP_BENCHMARK_MIN_VERSION} QUIET)
	if(NOT benchmark_FOUND)
		set(BENCHMARK_ENABLE_TESTING         OFF  CACHE BOOL  "Build benchmark tests"                          FORCE)
		set(BENCHMARK_ENABLE_EXCEPTIONS      ON   CACHE BOOL  "Enable exceptions in benchmark library"         FORCE)
		set(BENCHMARK_ENABLE_LTO             OFF  CACHE BOOL  "Enable link time optimization"                  FORCE)
		set(BENCHMARK_USE_LIBCXX             OFF  CACHE BOOL  "Use libc++ as the standard library"             FORCE)
		set(BENCHMARK_ENABLE_WERROR          OFF  CACHE BOOL  "Treat warnings as errors"                       FORCE)
		set(BENCHMARK_FORCE_WERROR           OFF  CACHE BOOL  "Force warnings as errors regardless of issues"  FORCE)
		set(BENCHMARK_BUILD_32_BITS          OFF  CACHE BOOL  "Build a 32-bit version of the library"          FORCE)
		set(BENCHMARK_ENABLE_INSTALL         OFF  CACHE BOOL  "Install benchmark targets"                      FORCE)
		set(BENCHMARK_ENABLE_DOXYGEN         OFF  CACHE BOOL  "Build documentation with Doxygen"               FORCE)
		set(BENCHMARK_INSTALL_DOCS           OFF  CACHE BOOL  "Install documentation"                          FORCE)
		set(BENCHMARK_DOWNLOAD_DEPENDENCIES  OFF  CACHE BOOL  "Download and build unmet dependencies"          FORCE)
		set(BENCHMARK_ENABLE_GTEST_TESTS     OFF  CACHE BOOL  "Build benchmark GTest-based tests"              FORCE)
		set(BENCHMARK_USE_BUNDLED_GTEST      OFF  CACHE BOOL  "Use bundled GoogleTest for benchmark"           FORCE)
		set(BENCHMARK_ENABLE_LIBPFM          OFF  CACHE BOOL  "Enable performance counters via libpfm"         FORCE)
		set(ENABLE_ASSEMBLY_TESTS_DEFAULT    OFF  CACHE BOOL  "Enable assembly tests by default"               FORCE)

		FetchContent_Declare(
			googlebenchmark
			GIT_REPOSITORY https://github.com/google/benchmark.git
			GIT_TAG        v1.9.4
			GIT_SHALLOW    TRUE
		)
	endif()
endif()

#----------------------------
# Dependency fetching
#----------------------------

message(STATUS "Fetching dependencies...")

# Fetch only dependencies not found on system
if(NOT nfx-meta_FOUND)
	FetchContent_MakeAvailable(nfx-meta)
endif()

if(NOT zlib-ng_FOUND)
	FetchContent_MakeAvailable(zlib-ng)
endif()

if(VISTA_SDK_CPP_BUILD_TESTS AND NOT GTest_FOUND)
	FetchContent_MakeAvailable(googletest)
endif()

if(VISTA_SDK_CPP_BUILD_BENCHMARKS AND NOT benchmark_FOUND)
	FetchContent_MakeAvailable(googlebenchmark)
endif()

#----------------------------------------------
# Target configuration and version extraction
#----------------------------------------------

#----------------------------
# zlib-ng configuration
#----------------------------

get_directory_property(_zlib_targets DIRECTORY ${zlib-ng_SOURCE_DIR} BUILDSYSTEM_TARGETS)
foreach(_target ${_zlib_targets})
	if(TARGET ${_target})
		set_target_properties(${_target} PROPERTIES
			EXCLUDE_FROM_ALL TRUE
			EXCLUDE_FROM_DEFAULT_BUILD TRUE
			POSITION_INDEPENDENT_CODE TRUE
		)

		set_property(TARGET ${_target} PROPERTY EXCLUDE_FROM_ALL TRUE)
		get_target_property(_target_type ${_target} TYPE)
		if(NOT _target_type STREQUAL "INTERFACE_LIBRARY")
			install(TARGETS ${_target} EXCLUDE_FROM_ALL)
		endif()
	endif()
endforeach()

#----------------------------
# googletest configuration
#----------------------------

if(TARGET GTest::gtest)
	get_target_property(GTEST_VERSION GTest::gtest VERSION)
endif()

#----------------------------
# googlebenchmark configuration
#----------------------------

if(TARGET benchmark::benchmark)
	get_target_property(BENCHMARK_VERSION benchmark::benchmark VERSION)
endif()

#----------------------------
# Cleanup
#----------------------------

set(CMAKE_REQUIRED_QUIET ${_SAVED_CMAKE_REQUIRED_QUIET})
set(CMAKE_MESSAGE_LOG_LEVEL ${_SAVED_CMAKE_MESSAGE_LOG_LEVEL})
set(CMAKE_FIND_QUIETLY ${_SAVED_CMAKE_FIND_QUIETLY})
