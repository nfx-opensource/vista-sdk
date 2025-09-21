# Vista SDK - C++ Implementation

[![GitHub](https://img.shields.io/github/license/ronan-fdev/vista-sdk?style=flat-square)](https://github.com/ronan-fdev/vista-sdk/blob/main/LICENSE) [![Version](https://img.shields.io/badge/Version-0.0.45-brightgreen?style=flat-square)](#)<br/>
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue?style=flat-square)](#) [![Cross Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey?style=flat-square)](#)<br/>
[![Windows MSVC 2022](https://img.shields.io/github/actions/workflow/status/ronan-fdev/vista-sdk/build-cpp-windows-msvc.yml?branch=cpp&label=MSVC%202022&style=flat-square)](https://github.com/ronan-fdev/vista-sdk/actions/workflows/build-cpp-windows-msvc.yml)[![Linux GCC 14](https://img.shields.io/github/actions/workflow/status/ronan-fdev/vista-sdk/build-cpp-linux-gcc.yml?branch=cpp&label=GCC%2014&style=flat-square)](https://github.com/ronan-fdev/vista-sdk/actions/workflows/build-cpp-linux-gcc.yml) [![Linux Clang 18](https://img.shields.io/github/actions/workflow/status/ronan-fdev/vista-sdk/build-cpp-linux-clang.yml?branch=cpp&label=Clang%2018&style=flat-square)](https://github.com/ronan-fdev/vista-sdk/actions/workflows/build-cpp-linux-clang.yml)

> C++ implementation of the Vista maritime standards ecosystem, providing tools for vessel data standardization, sensor naming, and maritime digitalization according to **ISO 19848**, **ISO 19847**, and **DNV VIS**

> **⚠️ WARNING: API NOT STABLE - LIBRARY UNDER ACTIVE DEVELOPMENT**
> This library is in active development and the API may change without notice. Use with caution in production environments.

## Overview

Vista SDK C++ implements the complete **Vista standards ecosystem** for maritime digitalization:

-   **[ISO 19847](https://docs.vista.dnv.com/docs/standards/iso-19847)** - Shipboard data servers to share field data at sea
-   **[ISO 19848](https://docs.vista.dnv.com/docs/standards/iso-19848)** - Standard data for shipboard machinery and equipment
-   **[DNV VIS](https://docs.vista.dnv.com/docs/standards/vis)** - Vessel Information Structure (DNV's maritime information model since 2005)
-   **[DNV Naming Rule](https://docs.vista.dnv.com/docs/standards/dnv-naming-rule)** - ISO 19848 Annex C ("dnv-v2" naming rule)

This C++ implementation provides tools for creating **standardized LocalIDs**, processing **GMOD hierarchies**, and enabling **interoperable maritime data exchange** across vessel fleets and maritime systems.

Originally ported from the [official DNV Vista SDK](https://github.com/dnv-opensource/vista-sdk) with modern C++ design patterns and cross-platform compatibility.

## Features

### ⚓ Vessel Information Structure (VIS)

-   **GMOD (Generic Product Model)**: Functionally-oriented vessel breakdown with hierarchical structure
-   **Codebooks**: Standardized vocabulary and meta tags for data classification and identification
-   **Locations**: Spatial positioning within vessel structures
-   **Product Types**: Comprehensive maritime equipment classifications across engineering disciplines
-   **Multi-version support**: VIS 3.4a through 3.9a with automatic conversion

### 🔄 LocalID Generation & Parsing

-   **DNV-v2 Naming Rule**: Complete ISO 19848 Annex C implementation
-   **LocalID Structure**: `/dnv-v2/vis-{version}/{PrimaryItem}[/sec/{SecondaryItem}]/meta/{TagElements}`
-   **UniversalID Support**: Global vessel identification with IMO numbers
-   **Meta Tag Processing**: Codebook validation for standardized values, custom value support
-   **Path Conversion**: Cross-version GMOD path transformation and validation

### 🗂️ Maritime Data Standards

-   **Sensor Naming**: Standardized identification eliminating proprietary naming schemes
-   **Data Channel Types**: Time series (tabular/event) and metadata structures
-   **Equipment Classifications**: DNV maritime equipment taxonomies and codebooks
-   **Transport Packages**: ISO 19847 compliant data server integration

## Quick Start

### Requirements

-   C++20 compatible compiler:
    -   **MSVC 2022+** (19.44+ tested)
    -   **GCC 10+** (14.2.0 tested)
    -   **Clang 12+** (19.1.5 tested)
-   CMake 3.20 or higher
-   **Multi-compiler builds supported** across x64/x86 architectures

### CMake Integration

The library supports modular compilation through CMake options:

```cmake
# Library build types
option(VISTA_SDK_CPP_BUILD_STATIC          "Build static library"                       ON   )
option(VISTA_SDK_CPP_BUILD_SHARED          "Build shared library"                       OFF  )

# Development
option(VISTA_SDK_CPP_BUILD_TESTS           "Build tests"                                ON   )
option(VISTA_SDK_CPP_BUILD_SMOKE_TESTS     "Build smoke tests"                          OFF  )
option(VISTA_SDK_CPP_BUILD_BENCHMARKS      "Build benchmarks"                           ON   )
option(VISTA_SDK_CPP_BUILD_SAMPLES         "Build vista-sdk-cpp samples"                ON   )

# Documentation
option(VISTA_SDK_CPP_BUILD_DOCUMENTATION   "Build vista-sdk-cpp doxygen documentation"  ON   )
```

### Using in Your Project

```cmake
include(FetchContent)
FetchContent_Declare(
  vista-sdk-cpp
  GIT_REPOSITORY https://github.com/ronan-fdev/vista-sdk.git
  GIT_TAG        cpp
  SOURCE_SUBDIR  cpp
)
FetchContent_MakeAvailable(vista-sdk-cpp)
target_link_libraries(your_target vista-sdk-cpp-static)
```

### Building

```bash
# Clone the repository (C++ implementation currently available on fork)
git clone https://github.com/ronan-fdev/vista-sdk.git
cd vista-sdk
git checkout cpp
cd cpp

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the library
cmake --build . --config Release

# Run tests (optional)
ctest --config Release

# Run benchmarks (optional)
cmake --build . --config Release --target benchmarks
```

### Usage Example

```cpp
#include <iostream>

#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/GmodPath.h>
#include <dnv/vista/sdk/LocalId.h>
#include <dnv/vista/sdk/LocalIdBuilder.h>

using namespace dnv::vista::sdk;

int main()
{
	// Initialize Vista SDK
	auto& vis = VIS::instance();
	auto version = VisVersion::v3_9a;
	const auto& codebooks = vis.codebooks( version );

	// Parse GMOD paths (vessel functional hierarchy)
	auto primaryPath = GmodPath::parse( "411.1/C101.31-2", version );
	auto secondaryPath = GmodPath::parse( "411.1/C101.63/S206", version );

	// Build standardized LocalIDs following DNV-v2 naming rule
	auto localId = LocalIdBuilder::create( version )
					   .withPrimaryItem( std::move( primaryPath ) )
					   .withSecondaryItem( std::move( secondaryPath ) )
					   .withMetadataTag( codebooks.createTag( CodebookName::Quantity, "temperature" ) )
					   .withMetadataTag( codebooks.createTag( CodebookName::Content, "exhaust.gas" ) )
					   .withMetadataTag( codebooks.createTag( CodebookName::Position, "inlet" ) )
					   .withVerboseMode( true )
					   .build();

	// Result: /dnv-v2/vis-3-9a/411.1/C101.31-2/sec/411.1/C101.63/S206/~propulsion.engine/~cylinder.2/~for.propulsion.engine/~cooling.system/meta/qty-temperature/cnt-exhaust.gas/pos-inlet
	std::cout << "Generated LocalID: " << localId.toString() << std::endl;

	// Parse existing LocalID
	auto parsedLocalId = LocalId::parse( "/dnv-v2/vis-3-9a/411.1/C101.41/meta/qty-pressure" );
	std::cout << "Primary item: " << parsedLocalId.primaryItem().value().toString() << std::endl;
}

```

### Sample Applications

The `samples/` directory contains comprehensive examples demonstrating all SDK features:

-   **`basic/`** - Core functionality examples

    -   `Sample_LocalId.cpp` - LocalID building, parsing, and validation
    -   `Sample_GMOD.cpp` - GMOD hierarchy navigation and path operations
    -   `Sample_Codebooks.cpp` - Metadata tag creation and validation
    -   `Sample_BasicUsage.cpp` - Quick start examples

-   **`builders/`** - Advanced builder patterns

    -   `Sample_Builders_LocalId.cpp` - LocalID builder variations and patterns

-   **`gmod/`** - GMOD-specific operations
-   **`iso19848/`** - ISO 19848 transport layer examples
-   **`parse/`** - Parsing and validation examples
-   **`versioning/`** - Cross-version conversion examples
-   **`vis/`** - VIS data structure examples

To build and run samples:

```bash
cmake --build . --config Release --target samples
```

## Project Structure

```
vista-sdk/cpp/
├── CHANGELOG.md            # Version history and changes
├── CMakeLists.txt          # Main CMake build configuration
├── README.md               # This documentation file
├── benchmark/              # Performance benchmarks with Google Benchmark
├── cmake/                  # CMake modules and build configuration
├── doc/                    # Documentation generation
├── include/dnv/vista/sdk/  # Public API headers
│   ├── config/             # Performance optimization macros
│   ├── detail/             # Implementation details and inline files
│   ├── transport/          # ISO 19848 transport layer (Data channel, Time series)
│   ├── Codebook.h          # Individual codebook interface
│   ├── Codebooks.h         # Codebook collection management
│   ├── Gmod.h              # Generic vessel model interface
│   ├── GmodNode.h          # Individual GMOD nodes
│   ├── GmodPath.h          # Hierarchical component paths
│   ├── LocalId.h           # Local identifier structures
│   ├── LocalIdBuilder.h    # LocalId construction utilities
│   ├── UniversalId.h       # Universal identifier structures
│   ├── VIS.h               # Main VIS singleton interface
│   └── VISVersion.h        # VIS standard version management
├── licenses/               # Third-party license files
├── samples/                # Comprehensive usage examples
├── src/                    # Implementation files
└── test/                   # Comprehensive unit tests with GoogleTest
```

## Dependencies

-   **[nfx-core](https://github.com/ronan-fdev/nfx-core)**: High-performance containers and utilities
-   **[nlohmann/json](https://github.com/nlohmann/json)**: JSON parsing for VIS standard data
-   **[zlib-ng](https://github.com/zlib-ng/zlib-ng)**: High-performance zlib compression library
-   **[GoogleTest](https://github.com/google/googletest)**: Testing framework
-   **[Google Benchmark](https://github.com/google/benchmark)**: Performance benchmarking framework

## Performance

Vista SDK C++ is designed for efficient maritime data processing with modern C++ optimization techniques.

For detailed performance metrics and cross-language comparisons, see [benchmark/README.md](benchmark/README.md).

## Version History

For detailed version history, feature additions, and breaking changes, see [CHANGELOG.md](CHANGELOG.md).

### Maritime Data Optimization

-   Zero-copy vessel component path parsing
-   Cached GMOD hierarchy lookups with LRU eviction
-   Compressed VIS standard data with lazy decompression
-   Memory-efficient codebook storage and retrieval

### Transport Layer Performance

-   ISO 19848 compliant data channel validation
-   Streaming vessel data processing
-   Minimal allocation message parsing

### Cross-Platform Maritime Standards

-   VIS version conversion algorithms (v3.4a through v3.9a)
-   GMOD component hierarchy traversal
-   Equipment codebook classification lookup

## Compatibility

-   **Platforms**: Windows, Linux, macOS
-   **Compilers**:
    -   **MSVC 2022+** (tested with Visual Studio 19.44.35214.0)
    -   **GCC 10+** (tested with GCC 14.2.0)
    -   **Clang 12+** (tested with Clang 19.1.5)
-   **Architectures**: x64
-   **Standards**: C++20

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### Third-Party Attributions

-   **nfx-core**: High-performance containers and utilities (MIT License)
-   **nlohmann/json**: JSON parsing library (MIT License)
-   **zlib-ng**: Compression library for VIS data (zlib License)
-   **Google Benchmark**: Performance measurement framework (Apache 2.0 License)
-   **Google Test**: Unit testing framework (BSD 3-Clause License)

## Official Vista Resources

### 📚 Documentation

-   **[VISTA Documentation](https://docs.vista.dnv.com/)** - Complete standards documentation

### 🛠️ Tools

-   **[GMOD Viewer](https://vista.dnv.com/gmod)** - Interactive VIS hierarchy explorer
-   **[Learning Tool](https://vista.dnv.com/learning-tool)** - LocalID construction tutorial

---

_Updated September 21, 2025_
