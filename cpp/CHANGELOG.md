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

## [0.0.25] - 2025-09-05

### Changed

-   **LocationSetsVisitor extraction**: Extract LocationSetsVisitor into separate component for better separation of concerns
    -   Refactor location set traversal logic into dedicated visitor pattern implementation
    -   Improve code modularity and reusability for location set operations
    -   Enhance maintainability through cleaner architectural separation
    -   Enable better testing and extensibility of location set processing

## [0.0.24] - 2025-09-05

### Changed

-   **C++ source directory structure**: Flatten source directory hierarchy for improved organization
    -   Simplify nested directory structure in C++ source tree
    -   Improve build system navigation and maintenance
    -   Reduce complexity in include path management
    -   Enhance developer experience with cleaner project layout

## [0.0.23] - 2025-09-05

### Fixed

-   **Path conversion individualization**: Preserve individualization during path conversion operations
    -   Maintain individual path component identity during conversion processes
    -   Prevent loss of specific path individualization markers
    -   Ensure accurate path transformation while preserving semantic meaning

## [0.0.22] - 2025-08-30

### Fixed

-   **installSubstructure logic**: Fix logic to properly skip children during installation
    -   Resolves parsing incompatibility with GMOD v3-9a containing `installSubstructure: false` fields
    -   References C# fix: [dnv-opensource@4302bf0](https://github.com/dnv-opensource/vista-sdk/commit/4302bf0052fdf0c37604c8dadb3e8b918a98cd5c)

## [0.0.21] - 2025-08-29

### Added

-   **C++ sample suite**: Comprehensive collection of example applications demonstrating SDK usage
    -   Complete sample applications showcasing core SDK functionality
    -   Example implementations for common use cases and patterns
    -   Educational code examples for SDK adoption and learning
    -   Reference implementations for best practices and proper API usage

## [0.0.20] - 2025-08-27

### Added

-   **LocalIdQuery fluent API**: Complete fluent filtering and matching API for LocalId objects
    -   Fluent builder pattern for constructing complex LocalId queries
    -   Method chaining for readable and maintainable filtering operations
    -   Comprehensive matching capabilities for LocalId validation and selection
    -   Integration with existing LocalId infrastructure for seamless querying

## [0.0.19] - 2025-08-27

### Added

-   **GmodPathQuery fluent API**: Complete fluent path matching and filtering API for GMOD paths
    -   Fluent builder pattern for constructing complex path queries
    -   Method chaining for readable and maintainable path filtering
    -   Comprehensive path matching capabilities for GMOD navigation
    -   Integration with existing GMOD infrastructure for seamless querying

## [0.0.18] - 2025-08-26

### Changed

-   **StringMap/StringSet containers**: Move containers from utils to internal namespace
    -   Refactor StringMap/StringSet from utils to internal namespace for better encapsulation
    -   Remove heterogeneous lookup functors (StringViewHash/StringViewEqual) from StringUtils.h
    -   Reorganize DynamicStringBuffer_impl files (remove impl/ subdirectory)
    -   Improve internal organization and reduce public API surface

## [0.0.17] - 2025-08-25

### Changed

-   **Parsing error builders**: Move to internal implementation for better encapsulation
    -   Move LocalIdParsingErrorBuilder and LocationParsingErrorBuilder from include/ to src/
    -   Remove unnecessary .inl files and inline simple methods in .cpp
    -   Improve API boundaries by hiding implementation details from public interface

## [0.0.16] - 2025-08-25

### Changed

-   **ShipId discriminated union**: Replace integer tags with enum class for type safety
    -   Refactor ShipId to use enum class instead of integer tags
    -   Improve type safety and code clarity for discriminated union pattern
    -   Enhance maintainability of ShipId implementation

## [0.0.15] - 2025-08-25

### Added

-   **DynamicStringBuffer class**: Pimpl wrapper around fmt::memory_buffer for framework-agnostic buffer management
-   **DynamicStringBufferPool**: Thread-safe pool management for dynamic string buffers
-   **StringBuilder::Enumerator**: Character iteration support for StringBuilder
-   **Stream operators**: Added << operators for StringBuilder
-   **Pool statistics**: Size tracking and performance statistics for StringBuilderPool
-   **Performance test suites**: New benchmark and test suites for StringBuilderPool performance testing

### Changed

-   **StringBuilderPool API redesign**: Complete Pimpl pattern implementation for framework-agnostic design
    -   Transform StringBuilderPool from fmt-dependent to framework-agnostic
    -   Abstract implementation from public API surface
    -   Eliminate all direct fmt:: dependencies from public API
-   **StringBuilder interface**: Use DynamicStringBuffer& instead of fmt::memory_buffer&
-   **StringBuilderLease management**: Manage DynamicStringBuffer* instead of fmt::memory_buffer*
-   **Thread-local cache**: Manage DynamicStringBuffer* instead of fmt::memory_buffer*
-   **Append operations**: Simplified from m_buffer.append(str.data(), str.data() + str.size()) to m_buffer.append(str)

### Removed

-   **Manual calculations**: Eliminate manual strlen() calculations in C-string append operations

## [0.0.14] - 2025-08-23

### Added

-   **TimeSeriesData transport layer**: Complete transport layer for time-series data handling
    -   TimeSeriesData class for managing temporal data collections
    -   TimeSeriesDataPackage for packaging time-series transport containers
    -   Integration with existing DataChannel infrastructure
-   **Comprehensive TimeSeriesData test suite**: Complete testing coverage for time-series functionality
-   **Enhanced constructor overloads**: string_view overloads for Unit, Format, DataChannelType constructors and setters
-   **Extended DataChannelList API**: string_view overload for DataChannelList::tryGetByShortId() method

### Changed

-   **API consistency improvements**: Rename empty() → isEmpty() for consistency across HashMap, MemoryCache, DataChannelList
    -   Standardize method naming patterns across transport layer components
    -   Consistent method signatures for data access and manipulation
    -   Improved error handling patterns throughout transport layer
-   **Performance optimizations**: Replace std::function with perfect forwarding templates in DataChannelId pattern matching
    -   Add StringBuilderPool usage for optimized error message construction
    -   Improved template-based dispatch for better compile-time optimization

### Removed

-   **Code cleanup**: Remove redundant String constructor overload in ISO19848

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
