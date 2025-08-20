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
