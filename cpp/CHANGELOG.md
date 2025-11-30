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

## [0.0.70] - 2025-11-01

-   **Dependencies**: Updated nfx-core from v0.7.0 to v0.8.0

## [0.0.69] - 2025-10-31

### Changed

-   **BREAKING CHANGE: Dependency migration**: Complete migration from nfx-core to nfx-meta
    -   Updated all CMake configurations to use nfx-meta v0.7.0
    -   Updated all target linking from `nfx-core::static` to `nfx-meta::static`
    -   Updated package configuration for nfx-meta dependency resolution

## [0.0.68] - 2025-10-31

-   **Dependencies**: Updated nfx-core from v0.5.9 to v0.6.0

## [0.0.67] - 2025-10-30

-   **Dependencies**: Updated nfx-core from v0.5.8 to v0.5.9

## [0.0.66] - 2025-10-30

-   **Dependencies**: Updated nfx-core from v0.5.7 to v0.5.8

## [0.0.65] - 2025-10-30

### Changed

-   **Dependencies**: Updated nfx-core from v0.5.6 to v0.5.7
-   **BREAKING CHANGE**: Migrated StringBuilderPool API from `lease.builder()` to `lease.create()` for nfx-core v0.5.7 compatibility

## [0.0.64] - 2025-10-30

-   **Dependencies**: Updated nfx-core from v0.5.5 to v0.5.6

## [0.0.63] - 2025-10-29

-   **Dependencies**: Updated nfx-core from v0.5.4 to v0.5.5

## [0.0.62] - 2025-10-28

-   **Dependencies**: Updated nfx-core from v0.5.3 to v0.5.4

## [0.0.61] - 2025-10-26

### Changed

-   **Dependencies**: Updated nfx-core from v0.5.2 to v0.5.3

## [0.0.60] - 2025-10-26

### Changed

-   **Dependencies**: Updated nfx-core from v0.5.1 to v0.5.2 with externalized datatypes infrastructure

## [0.0.59] - 2025-10-25

### Changed

-   **Dependencies**: Updated nfx-core from v0.5.0 to v0.5.1 with integrated StringUtils library

## [0.0.58] - 2025-10-25

### Changed

-   **Dependencies**: Updated nfx-core from v0.4.1 to v0.5.0 for enhanced DateTime infrastructure and nfx DateTime library integration

-   **BREAKING CHANGE: NFX DateTime namespace migration**: Complete migration from `nfx::time` to `nfx::datetime` namespace

    -   Migrated all `nfx::time::DateTime` references to `nfx::datetime::DateTime`
    -   Migrated all `nfx::time::DateTimeOffset` references to `nfx::datetime::DateTimeOffset`
    -   Migrated all `nfx::time::TimeSpan` references to `nfx::datetime::TimeSpan`
    -   Updated all timestamp handling throughout transport layer, serialization, and test suites

## [0.0.57] - 2025-10-18

### Changed

-   **Dependencies**: Updated nfx-core from v0.3.5 to v0.4.1 for enhanced JSON serialization capabilities and Document API improvements

-   **BREAKING CHANGE: JSON Serialization API modernization**: Complete migration from JSON Pointer methods to modern Document API

    -   Replaced all `*ByPointer()` methods with modern `doc.set()` and `doc.get<T>()` template methods

-   **File naming standardization**: Renamed serialization headers for better API clarity

    -   `DataChannelList.h` → `DataChannelListSerializationTraits.h`
    -   `TimeSeriesData.h` → `TimeSeriesDataSerializationTraits.h`

-   **JSON serialization performance optimization**: Eliminated intermediate Document object creation

    -   Direct JSON path construction using `doc.set("/Package/Header/ShipID", value)` pattern
    -   Removed complex object hierarchy building in favor of direct path-based serialization

-   **Restriction handling enhancement**: Complete implementation of JSON schema restriction serialization
    -   Added full support for enumeration, pattern, numeric bounds, length constraints, digit constraints, and whitespace handling
    -   Enhanced Format restriction serialization with comprehensive constraint support

## [0.0.56] - 2025-10-12

### Added

-   **JSON Serialization Framework**: Complete infrastructure with comprehensive type system integration

    -   Added advanced Value type system with complete ISO 19848 format validation and serialization support
    -   Added DataChannel metadata management with rich property annotation capabilities
    -   Added comprehensive DataChannelList indexing with LocalId and ShortId lookup optimization
    -   Added Package and DataChannelListPackage transport containers for efficient data serialization
    -   Added DataChannel creation and manipulation examples with full metadata integration

### Changed

-   **Dependencies**: Updated nfx-core from v0.3.3 to v0.3.5 for enhanced JSON serialization capabilities and performance improvements

-   **Value type system architecture consolidation**: Complete refactoring of Value types for ISO 19848 compliance and type safety

### Removed

-   **Legacy Value type implementations**: Removed non-ISO 19848 compliant Value types and validation patterns

## [0.0.55] - 2025-10-09

### Added

-   **CMake architecture redesign**: Complete modularization of build system into specialized component files

    -   Added `dnvVistaCppBuildConfig.cmake` - Core build configuration and directory management
    -   Added `dnvVistaCppInstall.cmake` - Installation rules and export configuration
    -   Added `dnvVistaCppPackaging.cmake` - Multi-platform packaging with CPack integration
    -   Added `dnvVistaCppSummary.cmake` - Detailed configuration summary reporting

-   **Cross-Platform Packaging**: CPack configuration for TGZ, ZIP, DEB, RPM, and NSIS packages

    -   Linux packages include automatic dependency resolution
    -   Windows NSIS installer
    -   Component-based installation (Runtime, Development, Documentation)
    -   CMake package configuration with `find_package()` support

-   **Advanced build options**: Extended CMake configuration

    -   Added `VISTA_SDK_CPP_USE_CCACHE` for compiler cache optimization
    -   Added `VISTA_SDK_CPP_USE_CUSTOM_COMPILER_FLAGS` for custom compiler configuration
    -   Added comprehensive packaging control options (ARCHIVE, DEB, RPM, NSIS, SOURCE)
    -   Added `VISTA_SDK_CPP_INSTALL_PROJECT` for installation control

### Changed

-   **Dependencies**: Updated nfx-core from v0.2.1 to v0.3.2

-   **Library target architecture modernization**:

    -   Enhanced target configuration with BUILD_INTERFACE generator expressions for proper FetchContent support
    -   Updated target linking to use proper static/shared library preference patterns

-   **Enhanced compiler configuration**: Improved compiler detection and optimization settings

    -   Added user override option `VISTA_SDK_CPP_USE_CUSTOM_COMPILER_FLAGS` for custom build environments

### Fixed

-   **CMake export dependency conflicts**: Resolved critical FetchContent integration issues

    -   Fixed BUILD_INTERFACE generator expressions for proper dependency export in consuming projects
    -   Resolved .deps directory location from VISTA_SDK_ROOT_DIR to CMAKE_BINARY_DIR for proper build isolation

## [0.0.54] - 2025-10-05

### Changed

-   **Dependencies**: Updated nfx-core from v0.1.9 to v0.2.1

-   **Internal JSON serialization migration**: Replaced nlohmann::json with nfx-core JSON serialization framework (nlohmann::json abstraction)

    -   Migrated all internal DTO from class-based to struct-based architecture with public members for reflection-based serialization
    -   Replaced ADL serialization hooks with nfx `SerializationTraits` template specializations using compile-time reflection
    -   All JSON operations now use RFC 6901 JSON Pointer methods (`*ByPointer`) for standardized path-based field access

### Removed

-   **nlohmann::json direct dependency**: Removed nlohmann::json library and associated CMake dependencies
-   **DTO implementation files**: Removed .cpp files for DTO classes (now header-only)

## [0.0.53] - 2025-10-02

### Added

-   **Test Suite**:

    -   Added `TESTS_HashingConsistencyGmodNode.cpp`
    -   Added `TESTS_HashingConsistencyGmodPath.cpp`
    -   Added `TESTS_HashingConsistencyImoNumber.cpp`
    -   Added `TESTS_HashingConsistencyLocalId.cpp`
    -   Added `TESTS_HashingConsistencyLocalIdBuilder.cpp`
    -   Added `TESTS_HashingConsistencyLocations.cpp`
    -   Added `TESTS_HashingConsistencyMetadataTags.cpp`
    -   Added `TESTS_HashingConsistencyParsingErrors.cpp`
    -   Added `TESTS_HashingConsistencyUniversalId.cpp`
    -   Added `TESTS_HashingConsistencyUniversalIdBuilder.cpp`

-   **STL Hash Specializations**: enabling following objects as keys in hash-based STL containers

    -   Added `std::hash<LocalIdBuilder>`
    -   Added `std::hash<UniversalId>`

-   **Hash Consistency Infrastructure**:

    -   Added hash computation in all fluent API methods
    -   Ensures hash consistency across all LocalIdBuilder operations and state changes

-   **UniversalId Architecture Improvements**: Refactored internal structure for better maintainability

    -   Added `hashCode()` method returning cached hash value
    -   Enhanced internal structure using UniversalIdBuilder for centralized data management

-   **Test data files**:

    -   Add `testdata/GmodNodes_unique_vis-3-4a.txt` for GmodNode datasets validation
    -   Add `testdata/GmodPaths_unique_vis-3-4a.txt` for GmodPath datasets validation
    -   Add `testdata/ImoNumbers_unique.txt` for IMO numbers datasets validation
    -   Add `testdata/LocalIds_unique_3-4a.txt` for LocalId datasets validation
    -   Add `testdata/LocalIds_unique_3-6a.txt` for LocalId datasets validation
    -   Add `testdata/LocalIds_unique_3-7a.txt` for LocalId datasets validation

-   **Cross-platform CPU feature detection**: Comprehensive AVX2 runtime detection system for improved deployment compatibility
    -   Added configure-time CPU capability detection using `CheckCXXSourceRuns` with AVX2 intrinsics test
    -   Added compiler-specific flag detection (MSVC `/arch:AVX2` vs GCC/Clang `-mavx2`)
    -   Added `VISTA_SDK_CPP_HAS_AVX2_RUNTIME_SUPPORT` and `VISTA_SDK_CPP_ENABLE_AVX2` CMake variables for build system integration

### Changed

-   **Dependencies**:

    -   Updated `nfx-core` from `0.1.7` to `0.1.9` for improved hash combining algorithms

-   **MetadataTag Hash Implementation**: Enhanced hash computation for improved uniqueness and collision resistance

    -   Changed from value-only hashing to composite hash including CodebookName, value, and isCustom flag
    -   Ensures unique hash values for MetadataTags with identical values but different codebook names or custom flags

-   **Locations API**: Simplified parsing interface by removing redundant overloads

    -   Removed `bool tryParse(const std::string& value, Location& location)` method (use string_view version)
    -   Removed `bool tryParse(const std::optional<std::string>& value, Location& location)` method (use version with ParsingErrors)
    -   Enhanced error handling in remaining `tryParse()` methods with proper error builder initialization

-   **ImoNumber API**: Improved parsing interface with better error handling

    -   Added default constructor `ImoNumber()` for container compatibility
    -   Added out-parameter parsing method `static bool tryParse(std::string_view value, ImoNumber& imoNumber)`

-   **UniversalId Internal Structure**: Streamlined architecture for better performance and maintainability

    -   Changed internal storage from separate `m_imoNumber` and `m_localId` to unified `m_builder` approach
    -   Refactored `toString()` implementation to delegate to builder for consistency
    -   Updated equality comparison to use builder-based comparison for better reliability

-   **LocalIdBuilder hash system integration**: Complete hash computation support in LocalIdBuilder architecture

    -   Enhanced `LocalIdBuilder()` constructor with FNV-1a hash initialization (`VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS`)
    -   Added `m_hashCode` member management in move assignment operators
    -   Enabled copy assignment operator for `LocalIdBuilder`

-   **LocalId copy semantics**: Enhanced LocalId with proper copy assignment support

-   **LocalIdItems copy semantics**: Enhanced LocalIdItems container with proper copy support

### Removed

-   **Code Cleanup**:

    -   Removed unused internal constructor from `ParsingErrors` class
    -   Removed redundant move constructor implementation from `ParsingErrors`

-   **Hash Consistency Issues**: Resolved critical hash computation inconsistencies in LocalIdBuilder
    -   Fixed hash inconsistencies between fluent API construction and parsing methods

### Fixed

-   **Build System**:

    -   Updated lambda capture to prevent potential dangling reference issues in `GmodPathQuery` tests

-   **Deployment compatibility issues**: Resolved "illegal instruction" crashes on CPUs without AVX2 support on some platforms

    -   Fixed runtime crashes caused by hardcoded advanced CPU instruction compilation
    -   Ensured binaries work on both modern CPUs (with AVX2 optimizations) and older CPUs (with SSE2 fallback)

-   **GmodPath parsing consistency**: Fixed critical bug in GmodPath hierarchy completion logic causing duplicate nodes

    -   Resolved `parse()` method creating malformed duplicate hierarchies (e.g., `"VE/000a"` becoming `VE/VE/000a` instead of `VE/000a`)
    -   Fixed duplicate node insertion in hierarchy completion algorithm by adding duplicate checking before node insertion
    -   Fixed duplicate root node insertion by verifying root node existence before adding to hierarchy
    -   Ensured consistent hash values across all parsing methods (`parse()`, `parseFullPath()`, `tryParse()`, `tryParseFullPath()`)

-   **LocalIdBuilder hash computation**: Fixed hash calculation consistency in `tryParse()` method

    -   Added proper hash computation after successful parsing to match behavior of other parsing methods
    -   Ensured hash consistency across all LocalIdBuilder creation and parsing methods

-   **LocalIdBuilder hash computation**: Fixed hash consistency and integration issues in LocalIdBuilder architecture
    -   Resolved hash initialization problems with proper FNV-1a offset basis setup
    -   Fixed hash computation integration during builder finalization process
    -   Corrected copy semantics across LocalId ecosystem (LocalId, LocalIdBuilder, LocalIdItems)
    -   Ensured hash consistency across all Vista Objects for STL container compatibility

## [0.0.52] - 2025-09-28

### Added

-   **Comprehensive hash system implementation**: Added cached hash support across all major VISTA SDK classes

    -   **GmodNode hash system**: Added `m_hashCode` member and `hashCode()` method with SSE4.2/FNV-1a optimization
    -   **GmodPath hash system**: Added hierarchical path hash combining all parent and target node hash codes
    -   **ImoNumber hash system**: Added cached hash for IMO number validation with integer hashing optimization
    -   **LocalIdBuilder hash system**: Added comprehensive hash combining all metadata components and builder state
    -   **Location hash system**: Added cached hash for location strings with SSE4.2/FNV-1a algorithms
    -   **MetadataTag hash system**: Added hash combining CodebookName enum, string value, and custom flag
    -   **ParsingErrors hash system**: Added hash combining all error entries for diagnostic collection management
    -   **UniversalIdBuilder hash system**: Added hash combining ImoNumber and LocalIdBuilder components

-   **std::hash specializations**: Complete STL container integration for all hash-enabled classes

    -   Added specialized `std::hash` templates for GmodNode, GmodPath, ImoNumber, Location, MetadataTag, ParsingErrors
    -   Enables seamless usage in std::unordered_map, std::unordered_set, and other STL hash containers
    -   O(1) hash access performance through cached pre-computed values

### Changed

-   **LocalId hash implementation**: Refactored to delegate hash computation to underlying LocalIdBuilder
-   **GmodNode constructor architecture**: Enhanced with hash computation during construction
-   **GmodPath constructor architecture**: Enhanced with hierarchical hash computation
-   **ParsingErrors constructor architecture**: Enhanced with error collection hash computation

## [0.0.51] - 2025-09-27

### Added

-   **LocalId cached hash system**: Implemented performance-optimized cached hash computation
    -   Added `m_hashCode` member to LocalId class for O(1) hash access
    -   Added `hashCode()` method returning pre-computed hash value
    -   Enhanced std::hash specialization to use cached hash instead of recomputing from string
    -   std::hash specialization enables zero-overhead hash-based container usage (std::unordered_map, nfx::HashMap)

### Changed

-   **nfx-core dependency update**: Updated from v0.1.5 to v0.1.7
-   **LocalId constructor optimization**: Hash computation now performed during construction using hardware-accelerated CRC32/FNV-1a

### Fixed

-   **Hash performance optimization**: Resolved O(n) to O(1) hash access transformation

## [0.0.50] - 2025-09-27

### Added

-   **Hash algorithm constants**: Added `VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS` and `VISTA_SDK_CPP_HASH_FNV_PRIME` constants in config.h

### Changed

-   **nfx-core dependency update**: Updated from v0.1.4 to v0.1.5
-   **nfx::containers::ChdHashMap template parameter enhancement**: Added explicit hash constant template parameters
    -   Updated `ChdHashMap<GmodNode>` to `ChdHashMap<GmodNode, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS, VISTA_SDK_CPP_HASH_FNV_PRIME>`
    -   Enhanced type safety and performance through compile-time hash constant specification
-   **LocalId hash specialization enhancement**: Updated std::hash specialization with explicit template parameters
    -   Added explicit FNV-1a constants to `nfx::core::hashing::hashStringView` calls
    -   Improved hash compatibility and deterministic behavior across platforms

## [0.0.49] - 2025-09-27

### Added

-   **VersionInformation constructor overload**: New constructor accepting custom values for namingRule, namingSchemeVersion, and referenceUrl
-   **ConfigurationReference::timeStamp() accessor**: New const accessor method for timeStamp property
-   **Enhanced header documentation**: Comprehensive documentation improvements across all major SDK components

### Changed

-   **nfx-core dependency update**: Updated from v0.1.2 to v0.1.4
-   **BREAKING CHANGE: VersionInformation API simplification**: Removed setter methods in favor of constructor-based initialization
    -   Removed `setNamingRule()`, `setNamingSchemeVersion()`, and `setReferenceUrl()` methods

### Removed

-   **Forward declarations cleanup**: Removed unnecessary `nfx::datatypes::DateTimeOffset` forward declarations
-   **Setter-based initialization pattern**: Eliminated mutable setter pattern in favor of immutable constructor initialization

## [0.0.48] - 2025-09-26

### Added

-   **MQTT LocalId support**: Complete MQTT-compatible LocalId implementation with inheritance pattern
-   **Comprehensive MQTT test suite**: MQTT topic formatting validation

### Changed

-   **LocalId architecture**: Removed `final` keyword to enable controlled inheritance
-   **LocalIdBuilder API enhancement**: Added `buildMqtt()` method

## [0.0.47] - 2025-09-24

### Added

-   **Comprehensive Doxygen documentation**: Complete API documentation coverage for all public classes and methods

### Changed

-   **nfx-core dependency update**: Updated from v0.1.1 to v0.1.2
-   **CMake dependency management improvements**: All dependencies now prefer system-installed packages via `find_package()` before falling back to `FetchContent`

## [0.0.46] - 2025-09-24

### Added

-   **GitHub Actions CI/CD**:
    -   MinGW workflow support added
    -   ccache integration across all CI/CD workflows
        -   Enhanced workflow security with explicit `contents: read` permissions

### Changed

-   **Core dependency upgrade**: nfx-core updated from v0.0.8 to v0.1.1
-   **Performance enhancements**:
    -   Migrated from `std::unordered_map` to `nfx::containers::HashMap` in `DataChannelList`
    -   Enhanced LocalId hashing with SSE4.2/FNV-1a string processing
    -   Streamlined data channel storage using direct pointers over reference wrappers

## [0.0.45] - 2025-09-21

### Added

-   **Comprehensive benchmark documentation**: Complete performance benchmarking suite with detailed results
-   **GitHub Actions CI/CD**: Comprehensive multi-compiler build workflows
    -   Separate workflows for Windows MSVC 2022, Linux GCC 14, and Linux Clang 18
    -   Automated testing with CTest on all supported platforms
    -   Build artifact archiving with 7-day retention
    -   Path-filtered triggers for efficient CI execution

### Changed

-   **NFX Core dependency update** from v0.0.7 to v0.0.8
-   **Memory architecture migration**: Complete transition from MemoryCache to LruCache implementation
    -   Migrate all cache implementations from `nfx::memory::MemoryCache` to `nfx::memory::LruCache`
    -   Update cache options from `MemoryCacheOptions` to `LruCacheOptions`
    -   Enhanced LRU (Least Recently Used) eviction strategy for better memory management
-   **GmodVersioning algorithm optimization**: Complete refactoring of version conversion logic
    -   Simplified conversion algorithm with better performance characteristics
    -   Enhanced error handling and location preservation during conversions

## [0.0.44] - 2025-09-21

### Changed

-   **NFX Core dependency update** from v0.0.6 to v0.0.7
-   **Internal code organization refactoring**: Relocated internal constants to more appropriate modules

## [0.0.43] - 2025-09-20

### Added

-   **Shared reference semantics**: GmodNode collections (`m_children`, `m_parents`, `m_childrenSet`) now use `std::shared_ptr` for efficient copying

### Changed

-   **NFX Core dependency update** from v0.0.5 to v0.0.6 with improved string manipulation capabilities
-   **GmodPath validation API simplification**
    -   **Simplified `isValid()` signature**: Changed from `isValid(const std::vector<GmodNode*>&, const GmodNode&, int&)` to `isValid(const std::vector<GmodNode>&, const GmodNode&)`
    -   **Direct value semantics**: GmodPath validation now works directly with node values instead of pointer vectors
-   **LocationSetsVisitor interface**
    -   **Direct vector access**: `visit()` method now accepts `const std::vector<GmodNode>&` instead of `const std::vector<GmodNode*>&`
    -   **Elimination of pointer conversions**: Removed all temporary pointer vector creation throughout GMOD path processing
-   **GmodNode architecture optimization**: Complete transition to shared reference semantics
    -   **Shared collections**: All relationship collections now use `std::shared_ptr` for optimal copy performance
    -   **Access pattern update**: All collection access now dereferences shared_ptr (e.g., `*m_children` instead of `m_children`)
-   **Path parsing optimizations**: Performance improvements in GmodPath processing
    -   **Vector reservation**: Improved capacity estimation for node vectors based on path length
    -   **Pointer elimination**: Removed temporary pointer vector conversions in parsing algorithms
    -   **Direct value semantics**: Path validation and processing now work directly with node values

### Removed

-   **Temporary vector allocations**: Removed temporary pointer vector creation in path processing
    -   **GmodPath processing**: Removed `tempParents`, `tempParentsView`, `nodePointers` temporary vectors
    -   **LocationSetsVisitor**: Reduced pointer conversion overhead in visit operations
    -   **Memory optimization**: Reduced temporary allocations throughout GMOD path processing pipeline
-   **Legacy validation methods**: Removed duplicate and complex validation overloads
    -   **Inline wrapper removal**: Eliminated redundant `GmodPath::isValid()` inline wrapper method
    -   **Simplified interface**: Single validation method with internal error tracking implementation

## [0.0.42] - 2025-09-20

### Changed

-   **GmodVersioning architecture consolidation**: Major refactoring eliminating redundant internal functions and optimizing performance
    -   **Removed duplicate internal functions**: Eliminated redundant `internal::convertNode()` overloaded functions and `tryGetVersioningNode()` wrapper
    -   **Inlined conversion logic**: All node conversion logic now directly embedded in public methods
    -   **Optimized thread-local buffers**: Replaced separate helper functions with embedded `static thread_local` variables within `addToPath()`
    -   **Streamlined validation**: Removed redundant `validateSourceAndTargetVersionPair()` function
    -   **Direct API usage**: HashMap calls now made directly without wrapper functions for better performance

### Removed

-   **Internal helper functions**: `internal::convertNode()` (2 variants), `tryGetVersioningNode()`, `validateSourceAndTargetVersionPair()`, `currentParentsBuffer()`, `remainingBuffer()`

### Fixed

-   **Code organization**: Eliminated duplicate logic and simplified architecture for better maintainability

## [0.0.41] - 2025-09-20

### Changed

-   **Benchmark architecture unification**: Complete rewriting of existing benchmark suite with unified patterns

## [0.0.40] - 2025-09-20

### Added

-   **Comprehensive GMOD lookup benchmark suite**: Complete performance comparison framework for container types

### Changed

-   **NFX Core dependency update** from v0.0.4 to v0.0.5
-   **BREAKING CHANGE: HashMap API migration for nfx-core v0.0.5 compatibility**
    -   Updated SDK from `tryGetValue(key)` returning pointer to `tryGetValue(key, outParam)` returning bool

## [0.0.39] - 2025-09-20

### Added

-   **Benchmark infrastructure improvements**: Re-enabled `BM_GmodLoad.cpp` benchmark

### Changed

-   **BREAKING CHANGE: Internal constants namespace reorganization**
    -   Migrated all `internal::constants::*` namespaces to simplified `internal::*` structure
    -   `internal::constants::codebook` → `internal::codebook`
    -   `internal::constants::gmodnode` → `internal::gmodnode`
    -   `internal::constants::localId` → `internal::localId`
    -   `internal::constants::locations` → `internal::locations`
    -   `internal::constants::dto` → `internal::dto`
    -   `internal::constants::iso19848` → `internal::iso19848`
-   **Internal namespace structure consolidation**: Moved magic numbers and configuration constants to more appropriate locations
    -   Relocated position validation constants (`MAX_POSITIONS`, `MAX_GROUPS`, `MAX_NON_NUMERIC`) from `Codebook.cpp` to `internal/constants/Codebook.h`
    -   Consolidated namespace-specific internal functions under their respective namespaces
-   **VIS implementation improvements**: Better organization of internal caching and resource management
    -   Reorganized `VIS.cpp` internal namespace from generic `internal` to specific `internal::vis`
    -   Improved separation of concerns between different internal subsystems
    -   Enhanced cache management with more specific namespace organization

### Fixed

-   **Code organization consistency**: Standardized internal namespace usage patterns across the entire codebase
    -   Updated all references to use new simplified namespace structure
    -   Maintained API compatibility while improving internal code organization
    -   Enhanced maintainability through consistent namespace naming conventions

## [0.0.38] - 2025-09-19

### Changed

-   **BREAKING CHANGE: VIS singleton const correctness implementation**
    -   VIS::instance() now returns `const VIS&` instead of `VIS&` for thread-safe access
    -   All VIS public methods (gmod, codebooks, locations, conversion methods) now const-qualified
    -   Enhanced thread-safety guarantees with const interface design
-   **BREAKING CHANGE: Gmod::tryGetNode signature modification**
    -   Changed from `tryGetNode(code, const GmodNode*& node)` to `tryGetNode(code, GmodNode*& node)`
    -   Maintains const-correctness while enabling internal implementation flexibility
    -   Uses const_cast internally for performance optimizations without breaking logical const-correctness
-   **Internal API const-correctness improvements**
    -   GmodPath parsing methods use const_cast for internal API access while preserving const public interface
    -   Enhanced documentation explaining const_cast usage patterns for performance optimization
    -   All test files updated to use const VIS references following new const-correct patterns
-   **NFX Core dependency update** from v0.0.3 to v0.0.4

## [0.0.37] - 2025-09-17

### Added

-   **Comprehensive architectural documentation** across all major SDK public API headers
    -   Detailed system purpose sections with ASCII diagrams for core components
    -   Performance characteristics documentation for critical operations
    -   Memory layout documentation with structure diagrams and data flow architecture
    -   Design philosophy sections explaining architectural decisions
    -   Complete API usage examples with TODO placeholders for future implementation
-   **New transport layer organization** with `timeseries/` namespace for time series data handling
-   **ISO19848Constants.h** for centralized transport layer constants

### Changed

-   **NFX Core dependency update** from v0.0.2 to v0.0.3
-   **Major API encapsulation improvements** with friend class pattern implementation across core classes
    -   Made `Codebook` constructor private, accessible only through friend classes (`Codebooks`, `VIS`)
    -   Made `Gmod` constructor private, accessible only through friend class (`VIS`)
    -   Added comprehensive friend class declarations for controlled object construction
-   **Constructor implementation changes**
    -   Changed `Codebook` copy constructor from `= default` to custom implementation
    -   Enhanced move semantics patterns across data structures
-   **Public API cleanup and reorganization**
    -   Moved all DTO classes from public API to internal implementation (`*Dto.h` files moved to `src/internal/dto/`)
    -   Relocated transport layer DTOs to internal namespace
    -   Reorganized time series data under `transport/timeseries/` namespace
-   **DataChannel transport layer major refactoring**

### Fixed

-   **Constructor visibility and access control** issues in core data structures
-   **Documentation consistency** across all public API header files with standardized format

### Removed

-   **12 public API files** moved to internal implementation or deleted entirely:
    -   `CodebooksDto.h/inl` (moved to internal)
    -   `GmodDto.h/inl` (moved to internal)
    -   `GmodVersioning.h` (moved to internal core)
    -   `GmodVersioningDto.h/inl` (moved to internal)
    -   `LocationsDto.h/inl` (moved to internal)
    -   `config/ISO19848.h` (consolidated)
    -   `transport/ISO19848Dtos.h/inl` (moved to internal)
    -   `transport/TimeSeriesData/TimeSeriesData.h` (moved and reorganized)
-   **Public exposure of DTO classes** - now properly encapsulated in internal namespace

### Security

-   **Enhanced encapsulation patterns** to prevent unauthorized object construction
    -   Friend class pattern restricts direct instantiation of core classes
    -   Private constructors ensure controlled object creation through factory patterns
-   **Reduced public API surface** by moving implementation details to internal namespace
-   **Better access control** through strategic friend class declarations and private constructors
    -   Friend class pattern restricts direct instantiation of core classes
    -   Private constructors ensure controlled object creation through factory patterns
-   **Improved memory safety** in move constructors and destructors

## [0.0.36] - 2025-09-12

### Added

-   **Sample application**: New `Sample_LocalId_Validator.cpp` sample for Local ID validation and analysis
    -   Support command-line and file-based Local ID input
    -   Include verbose mode for detailed component analysis
-   **Sample application**: New `Sample_Gmod_Tree.cpp` sample for GMOD tree visualization and analysis
    -   Print hierarchical GMOD tree structure with configurable depth
    -   Multiple output formats (simple, detailed, with paths, JSON-like)
    -   Filter by node types (leaf nodes, function nodes, etc.)
    -   Statistics and summary information with command-line options

### Changed

-   **Sample documentation**: Enhanced header documentation for all sample applications

## [0.0.35] - 2025-09-12

### Added

-   **CodebookNames API expansion**: New enum conversion methods for improved type safety and consistency
    -   Add `CodebookNames::fromString()` method for string-to-enum conversion with size-optimized lookup
    -   Add `CodebookNames::toString()` method for enum-to-string conversion for error messages (returns singular, capitalized names like "Content", "Quantity")
    -   Add new `ENUM_*` constants in internal namespace for proper error message formatting (e.g., `ENUM_CONTENT = "Content"`)

### Changed

-   **BREAKING CHANGE**: CodebookNames API refactoring for better consistency and performance
    -   Change `CodebookNames::toPrefix()` return type from `std::string_view` to `std::string` for consistent API patterns
    -   Move `LocalIdBuilder::codebookNametoString()` functionality to `CodebookNames::toString()` for better separation of concerns
-   **MetadataTag performance optimization**: Streamline string building operations for better performance
    -   Change `MetadataTag::toString()` to use `nfx::string::StringBuilder&` instead of `std::string&` for zero-allocation operations
    -   Simplify `LocalIdBuilder` metadata appending logic with direct `tag.value().toString(builder)` calls
-   **Internal architecture improvements**: Better separation of public API from implementation details
    -   Move enum mapping functions from `Codebook.cpp` to `CodebookName.cpp` for better cohesion
    -   Extract enum-to-string conversion logic to dedicated methods with size-based optimization

### Removed

-   **API cleanup**: Remove redundant and misplaced functionality
    -   Remove `LocalIdBuilder::codebookNametoString()` method (functionality moved to `CodebookNames::toString()`)
    -   Remove manual StringBuilderPool usage in simple error message construction
    -   Remove duplicate enum mapping logic from multiple source files

### Fixed

-   **Error message consistency**: Resolve critical mismatch between expected and actual error message formats
    -   Fix error messages to use proper enum names ("Content", "Quantity") instead of internal operation names ("contents", "quantities")
    -   Update all error message construction to use standardized `CodebookNames::toString()` method
-   **Code organization**: Improve maintainability and reduce duplication
    -   Consolidate all CodebookName conversion logic in single location
    -   Eliminate redundant constant definitions across multiple files
    -   Fix inconsistent naming patterns in internal implementation

## [0.0.34] - 2025-09-11

### Changed

-   **Internal namespace architecture refactoring**: Complete migration of implementation details from private class members to `internal` namespace
    -   Move `LocationBuilder::withValueInternal()` methods from private class members to `internal::withValue()` static function
    -   Move `Locations::tryParseInternal()` method from private class member to `internal::tryParse()` static function
    -   Extract `GmodPath` parsing functions to consistent `internal` namespace naming (`parseInternal` → `parse`, `parseFullPathInternal` → `parseFullPath`)
    -   Extract `LocalIdBuilder::tryParseInternal()` to `internal::tryParse()` for consistent naming patterns
    -   Extract `GmodVersioning::convertNodeInternal()` to `internal::convertNode()` for consistent naming patterns
-   **Public API surface cleanup**: Remove implementation details from public header files
    -   Remove private method declarations from `LocationBuilder.h` (withValueInternal methods no longer exposed)
    -   Remove private method declarations from `Locations.h` (tryParseInternal method no longer exposed)
-   **Implementation consistency improvements**: Standardize internal function naming across all parsing components
    -   Remove "Internal" suffixes from functions now properly encapsulated in `internal` namespace
    -   Improve code organization with consistent public/private separation patterns
    -   Establish unified architecture for internal implementation details

### Fixed

-   **LocationBuilder fluent API**: Resolve critical bug in fluent method chaining where methods were not properly creating immutable copies
    -   Fix `withSide()`, `withVertical()`, `withTransverse()`, `withLongitudinal()` methods to create proper result copies before modification
    -   Fix `withValue(char)` method to use copy-based modification pattern instead of const member modification
    -   Ensure all fluent methods follow immutable builder pattern with `LocationBuilder result = *this` copy semantics
    -   Resolve compilation errors from attempting to modify const member variables in const methods
-   **Method implementation**: Simplify `withNumber()` and `withValue(int)` with direct inline validation instead of complex internal delegation
    -   Replace `withValueInternal()` delegation with direct number validation and assignment
    -   Improve code readability and reduce unnecessary function call overhead
    -   Maintain identical validation behavior while simplifying implementation architecture

## [0.0.33] - 2025-09-11

### Added

-   **Internal API structures**: New internal headers `GmodVersioningNode.h` and `GmodVersioningNode.inl` for complete implementation hiding
-   **Pimpl pattern implementation**: Complete void pointer implementation for `GmodVersioning` class ensuring zero implementation exposure
-   **Internal namespace architecture**: `dnv::vista::sdk::internal` namespace with all implementation details completely hidden from public API

### Changed

-   **GmodVersioning API**: Complete architectural transformation from exposed implementation to opaque Pimpl pattern
    -   Removed `GmodVersioningNode` and `GmodNodeConversion` from public API surface
    -   Deleted copy constructor and copy assignment operator for proper resource management
    -   Added custom move constructor with proper ownership transfer semantics
-   **VIS API**: Changed `gmodVersioningDto()` from static method to instance method
-   **Implementation extraction**: Moved all static helper functions and versioning logic to `internal` namespace
    -   Extracted `addToPath()`, `validateSourceAndTargetVersions()`, `validateSourceAndTargetVersionPair()` to internal scope
    -   Moved `convertNodeInternal()` overloads from class methods to internal static functions
    -   Relocated thread-local buffer utilities to internal implementation
-   **File organization**: Restructured implementation files for clean public/private separation
    -   Deleted `GmodVersioning.inl` from public headers
    -   Added internal implementation files with proper encapsulation

### Removed

-   **Public implementation exposure**: Eliminated all internal data structures from public API
    -   Removed `nfx::containers::HashMap<VisVersion, GmodVersioningNode> m_versioningsMap` from public interface
    -   Removed `GmodVersioningNode` nested class from public API
    -   Removed `GmodNodeConversion` struct from public interface
    -   Removed `ConversionType` enum from public API
-   **Copy semantics**: Deleted copy constructor and copy assignment operator to prevent resource duplication

### Fixed

-   **Memory management**: Proper RAII implementation with custom destructor for Pimpl pattern
-   **API encapsulation**: Zero internal implementation leakage through complete void pointer abstraction
-   **Move semantics**: Correct ownership transfer in move constructor preventing double deletion

## [0.0.32] - 2025-09-11

### Changed

-   **EmbeddedResource API cleanup**: Moved `EmbeddedResource` class from public API to internal implementation
-   **Resource caching simplification**: Removed all resource-level caching and synchronization

## [0.0.31] - 2025-09-10

### Changed

-   **Parsing architecture refactoring**: Complete reorganization of parsing logic from private static methods to internal namespace
    -   Move `LocalIdBuilder` parsing methods from private class members to `internal` namespace for better separation of concerns
    -   Move `GmodPath` parsing methods from private static methods to `internal` namespace for consistent architecture
    -   Extract `parseInternal()` and `parseFullPathInternal()` from `GmodPath` class to `internal` namespace
    -   Extract `tryParseInternal()`, `parseMetaTag()`, `advanceParser()`, and helper functions from `LocalIdBuilder` class to `internal` namespace
    -   Remove internal implementation details from public header files for cleaner API surface
    -   Improve code organization with consistent internal/public separation across parsing components

## [0.0.30] - 2025-09-10

### Added

-   **ISO19848 configuration header**: New centralized configuration for ISO 19848 transport layer
    -   Add `config/ISO19848.h` for ISO 19848 transport layer configuration constants
    -   Provide centralized configuration for ISO 19848 implementation

### Changed

-   **Constants architecture refactoring**: Complete reorganization of SDK constants from public API to internal implementation
    -   Move all constants files from `include/dnv/vista/sdk/constants/` to `src/internal/constants/` for internal-only access
    -   Change namespace from `dnv::vista::sdk::constants::` to `dnv::vista::sdk::internal::constants::`
-   **DTO parsing constants refactoring**: Complete migration to domain-specific namespace organization
    -   Reorganize `DtoKeys.h` constants into domain-specific namespaces: `codebook`, `gmod`, `gmodversioning`, `iso19848`, `locations`, `error`
    -   Update all DTO implementation files to use new namespace structure: `internal::constants::dto::<domain>::KEY_*`
    -   Consolidate error constants into unified `error` namespace with consistent naming conventions
-   **Implementation file migrations**: Move implementations from inline headers to separate source files
    -   Extract `CodebookName` implementation from `CodebookName.inl` to `CodebookName.cpp`
    -   Extract `LocalIdItems` implementation from `LocalIdItems.inl` to `LocalIdItems.cpp`
    -   Extract GMOD static methods from `Gmod.inl` to `Gmod.cpp` and node methods from `GmodNode.inl` to `GmodNode.cpp`
-   **Code optimization and library usage improvements**:
    -   Simplify `PositionValidationResults::fromString()` with direct if-else pattern instead of array lookup
    -   Remove unnecessary static array `s_validationResultMap` for better performance and simplicity
-   **Namespace restructuring**: Improve API encapsulation across all constant domains
    -   Update codebook namespace: `constants::codebook::` → `internal::constants::codebook::`
    -   Update GMOD namespace: `constants::gmod::` → `internal::constants::gmodnode::`
    -   Update LocalId namespace: `constants::localId::` → `internal::constants::localId::`
    -   Update locations namespace: `constants::locations::` → `internal::constants::locations::`
    -   Update ISO19848 namespace: `constants::iso19848::` → `internal::constants::iso19848::`

### Removed

-   **Public API cleanup**: Remove unnecessary files and reduce public interface complexity
    -   Remove `CodebookName.inl` file (implementation moved to `.cpp`)
    -   Remove redundant array-based string-to-enum lookup patterns
    -   Remove `appendCommonName()` static method declaration from `LocalIdItems.h` (moved to anonymous namespace in implementation)
    -   Remove constant prefixes (e.g., `CODEBOOK_NAME_POSITION` → `NAME_POSITION`) for cleaner namespace organization
    -   Remove unnecessary dependency includes from header files

### Fixed

-   **Compiler warnings**: Add `[[maybe_unused]]` attributes to suppress unused parameter warnings in sample files
    -   Update `Sample_Parse_LocalId.cpp` and `Sample_VIS_Gmod.cpp` with proper parameter annotations
    -   Improve code quality and eliminate build warnings

## [0.0.29] - 2025-09-07

### Added

-   **Complete sample file suite**: Comprehensive collection of example applications demonstrating all SDK functionality
    -   Add `builders/Sample_Builders_LocalId.cpp` - LocalId builder patterns with fluent API examples
    -   Add `builders/Sample_Builders_LocalIdQuery.cpp` - LocalIdQuery builder for complex filtering operations
    -   Add `parse/Sample_Parse_MetadataTag.cpp` - MetadataTag parsing with codebook validation
    -   Add `parse/Sample_Parse_LocalId.cpp` - LocalId parsing with enhanced error handling patterns
    -   Add `parse/Sample_Parse_GmodPath.cpp` - GmodPath parsing with full path and error capture support
    -   Add `versioning/Sample_Versioning_GmodNode.cpp` - GmodNode version conversion (691.81 → 691.8311)
    -   Add `versioning/Sample_Versioning_GmodPath.cpp` - GmodPath version conversion with metadata preservation
    -   Add `versioning/Sample_Versioning_LocalId.cpp` - LocalId version conversion between VIS versions

### Changed

-   **Comprehensive sample organization**: Complete restructuring of C++ sample files into logical functional categories
    -   Add `basic/` directory for fundamental SDK concepts (BasicUsage, Codebooks, GMOD, LocalId)
    -   Add `builders/` directory for builder pattern examples (LocalId, LocalIdQuery)
    -   Add `parse/` directory for parsing functionality examples (MetadataTag, LocalId, GmodPath)
    -   Add `versioning/` directory for version conversion examples (GmodNode, GmodPath, LocalId)
    -   Add `vis/` directory for VIS-specific functionality (Gmod usage patterns)
    -   Add `iso19848/` directory for transport layer examples

### Fixed

-   **Code formatting and error handling**: Clean up whitespace and improve error messaging across multiple source files
    -   Comment out debug fprintf statements in DTO classes for cleaner production output
    -   Improve GmodPath error message formatting for better user experience
    -   Enhance LocalIdBuilder error message formatting with proper newline handling

## [0.0.28] - 2025-09-07

### Added

-   **Configurable resource and test data paths**: Implement compile-time configurable directory paths for development flexibility
    -   Add VISTA_SDK_RESOURCES_DIR compile definition for configurable resource loading during development
    -   Add VISTA_SDK_TESTDATA_DIR compile definition for configurable test data access during development
    -   Replace hardcoded "resources" and "testdata" strings with macro-based configuration
    -   Enable flexible development environment setup with different directory structures

### Changed

-   **Build system cleanup**: Remove deprecated CMake options and improve organization
    -   Remove VISTA_SDK_CPP_RUN_TESTS and VISTA_SDK_CPP_RUN_BENCHMARKS options (unused)
    -   Remove VISTA_SDK_CPP_COPY_RESOURCES legacy resource copying system
    -   Organize build outputs: tests in bin/tests/, benchmarks in bin/benchmarks/
    -   Improve CTest discovery with proper working directory configuration
-   **Test infrastructure modernization**: Replace hardcoded paths with portable solutions
    -   Update all test files to use TestDataLoader for JSON files or macro-based path resolution for text files
    -   Remove terrible multiple-path guessing patterns (e.g., "../testdata/", "../../testdata/")
    -   Implement consistent path resolution across all test files
    -   Fix EmbeddedResource to use configurable resource directory instead of hardcoded "resources"

### Removed

-   **Legacy resource management**: Remove outdated resource copying and hardcoded path dependencies
    -   Remove CopyResources and CopyTestData build targets
    -   Remove hardcoded "resources" directory dependencies
    -   Remove multiple fallback path attempts in test files
    -   Remove build targets (tests, benchmarks)

### Fixed

-   **Development workflow improvements**: Resolve hardcoded path issues in development environment
    -   Replace terrible multiple-path guessing patterns with consistent macro-based fallbacks
    -   Resolve CTest discovery failures with proper working directory configuration
    -   Fix systematic hardcoded "testdata/" paths across multiple test files for development consistency
    -   Note: Runtime SDK relocation still requires manual path configuration (absolute paths embedded at compile time)

## [0.0.27] - 2025-09-07

### Added

-   **LocalIdQueryBuilder.withoutLocations() method**: New fluent API method for removing location constraints
    -   Remove location constraints from primary and secondary items in queries
    -   Enable location-agnostic LocalId matching for flexible query operations
    -   Maintain fluent builder pattern consistency with immutable design

### Changed

-   **LocalIdQueryBuilder API improvements**: Enhanced builder pattern implementation
    -   Make accessor methods inline for improved runtime performance
    -   Optimize factory method implementation for better consistency
    -   Update method signatures to follow established patterns

## [0.0.26] - 2025-09-06

### Added

-   **nfx-core library integration**: Complete migration to nfx-core library replacing internal implementations
    -   Migrate from internal HashMap to nfx::containers::HashMap
    -   Migrate from internal ChdDictionary to nfx::containers::ChdHashMap
    -   Migrate from internal MemoryCache to nfx::memory::MemoryCache
    -   Migrate to nfx::containers::StringMap/StringSet for zero-copy string operations
    -   Migrate to nfx::datatypes::Decimal for enhanced decimal arithmetic
    -   Migrate to nfx::time::DateTime for improved time handling
    -   Replace fmt::format with nfx::string::StringBuilderPool

### Changed

-   **BREAKING CHANGE**: Replace all internal container, memory, and string implementations with nfx-core equivalents
    -   Complete architectural shift to nfx-core foundation
    -   Updated method signatures and interfaces to align with nfx-core patterns
    -   Modified internal architecture to leverage nfx-core capabilities
-   **Compression upgrade**: Upgrade from zlib to zlib-ng v2.2.5 native API
    -   Switch from zlib.h to zlib-ng.h header
    -   Use zng_stream instead of z_stream type
    -   Replace inflate functions with zng\_\* native API equivalents
    -   Update EmbeddedResource decompression implementation
-   **CMake build system modernization**: Clean up build configuration
    -   Remove redundant include directories and dependencies
    -   Clean up compiler settings and warning suppressions
-   **API improvements**: Update Vista SDK C++ to v0.0.26 standards
    -   Update GmodPathQuery API with improved NodeItem constructor
    -   Replace VISTA_SDK_CPP_FORCE_INLINE with VISTA_SDK_CPP_INLINE
    -   Update config.h documentation to reflect actual implementation

### Fixed

-   **Compilation warnings and includes**: Resolve build issues and missing dependencies
    -   Fix string_view fprintf warnings in ISO19848Dtos.cpp
    -   Add missing std headers for proper compilation
    -   Replace fmt::print calls with std::cerr and std::fprintf

### Removed

-   **Legacy implementations**: Remove internal implementations in favor of nfx-core
    -   Remove internal HashMap, ChdDictionary, MemoryCache implementations
    -   Remove fmt:: dependencies replaced by nfx-core equivalents
    -   Remove unused C++20 NO_UNIQUE_ADDRESS attributes

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
