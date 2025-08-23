# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

-   NIL

### Changed

-   NIL

### Deprecated

-   NIL

### Removed

-   NIL

### Fixed

-   NIL

### Security

-   NIL

## [0.0.13] - 2025-08-23

### Changed

-   **Int128 API**: Remove Int128 friendship dependency
    -   Remove friend class Decimal128 from Int128
    -   Add public toNative() method for accessing native \_\_int128 value (GCC/Clang)

## [0.0.12] - 2025-08-23

### Added

-   **DataChannel transport layer**: Complete DataChannel transport layer with triple-indexed collections
-   **DataChannelList**: LocalId and ShortId lookup capabilities
-   **Package containers**: Package and DataChannelListPackage transport containers
-   **LocalId hash support**: std::hash specialization for LocalId (enables unordered_map usage)
-   **Enhanced parsing methods**: parse() methods for DateTime, DateTimeOffset, TimeSpan, Decimal128, Int128
-   **Value type system**: Comprehensive Value type system for ISO19848 format validation
-   **DataChannel test suite**: Comprehensive testing with validation, error handling, and lookup functionality

### Changed

-   **FormatDataType refactoring**: Extract FormatDataType::Value to transport::Value
-   **Constants consolidation**: Consolidate ISO19848 Annex C naming constants with proper documentation
-   **Constant migration**: Move NAMING_RULE constant from LocalIdConstants to ISO19848Constants

### Removed

-   **UniversalIdConstants.h**: Migrated functionality to ISO19848Constants

## [0.0.11] - 2025-08-23

### Changed

-   **Code organization**: Centralize ValidateResult type from ISO19848 to shared Results.h for better reusability
    -   Remove duplicate ValidateResult implementation from ISO19848
    -   Update FormatDataType::validate() signature to use shared type
    -   Establish unified validation pattern for DataChannel and future components

## [0.0.10] - 2025-08-22

### Added

-   **DataChannelId class**: Implement DataChannelId class with discriminated union pattern in transport domain
    -   Tag-based discriminated union (LocalId | string/shortId)
    -   Pattern matching with `matchOn<T>()` and `switchOn()` methods for type-safe access
    -   Parse factory method with LocalId validation fallback to shortId
    -   Complete test suite covering all functionality

## [0.0.9] - 2025-08-22

### Changed

-   **ImoNumber API**: Consolidate ImoNumber methods to use std::string_view for improved performance

## [0.0.8] - 2025-08-22

### Added

-   **ShipId class**: Implement ShipId class with comprehensive test suite

## [0.0.7] - 2025-08-22

### Fixed

-   **Linux compatibility**: Fix DateTime chrono round-trip limitations on Linux platform

## [0.0.6] - 2025-08-21

### Fixed

-   **Cross-platform compatibility**: Add cpuid.h in precompiled headers for GCC/Clang compatibility

## [0.0.5] - 2025-08-21

### Changed

-   **Internal API restructuring**: Move LocalIdParsingErrorBuilder, LocationParsingErrorBuilder, and StringBuilderPool from public API to `dnv::vista::sdk::internal` namespace
-   **Build system restructuring**:
    -   **CMake Sources.cmake**: Move parsing error builders and StringBuilderPool from public to internal sources
    -   **CMake Dependencies.cmake**: Remove unnecessary installation rules and skip install flags
    -   **File organization**: Relocate headers from `utils/` and public API to `internal/` namespace
-   **Parser infrastructure updates**: All LocalId parsing logic now uses `internal::LocalIdParsingState` and `internal::LocalIdParsingErrorBuilder`
-   **String building infrastructure**: All string operations migrated to `internal::StringBuilderPool`
-   **API boundary improvements**: Hide complex parsing implementation details while maintaining all public functionality

## [0.0.4] - 2025-08-21

### Removed

-   **Dependency cleanup**: Remove cpuid dependency from CMake and header files

## [0.0.3] - 2025-08-20

### Changed

-   **Code consistency**: Adjust inline specifiers for consistency across the SDK

## [0.0.2] - 2025-08-20

### Fixed

-   **Unicode support**: Enable Unicode support in fmt library

## [0.0.1] - 2025-08-20

### Added

-   **Core Implementation**: Complete Vista SDK implementation in C++20
-   **GMOD (Generic Product Model)** with versioning support
-   **Codebooks, LocalId, UniversalId** and VIS version management
-   **Advanced datatypes**: Decimal128, Int128, DateTimeISO8601
-   **ISO19848 transport layer** with JSON serialization
-   **Thread-safe memory cache** and internal utilities
-   **Comprehensive CMake configuration** with FetchContent
-   **Automatic dependency management**
-   **Multi-platform support** (Windows/Linux)
-   **Cross-compiler compatibility**:
    -   Windows: Clang 19.1.5 (GNU/MSVC), GCC 14.2.0 (MinGW), MSVC 19.44
    -   Linux: Clang 16.0.6, GCC 11+
-   **Complete GoogleTest suite**
-   **Google Benchmark performance testing suite**
-   **Hash compatibility validation** across platforms (C++/C#)
-   **Generic MemoryCache template** with LRU eviction
-   **Thread-local buffer optimization** for path conversion
-   **Memory-efficient string operations** and caching
-   **Cross-platform datetime handling** with ISO8601 support
-   **Dependencies**:
    -   nlohmann/json for JSON processing
    -   zlib-ng for compression
    -   libcpuid for CPU feature detection
    -   fmt for string formatting
    -   GoogleTest for unit testing
    -   Google Benchmark for performance testing

### TODO

-   Source generator for VisVersion enum generation from embedded resources
-   TimeSeriesData & DataChannel transport layer
