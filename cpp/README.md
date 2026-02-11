# Vista SDK C++

> C++ implementation of the Vista maritime standards ecosystem, providing tools for vessel data standardization, sensor naming, and maritime digitalization according to **ISO 19847**, **ISO 19848**, and **DNV VIS**

## API Overview

The vista-sdk-cpp provides a full-featured C++ API for working with the DNV Vessel Information Structure (VIS) standard:

### Core Components

-   **`VIS`** - Singleton entry point providing access to versioned VIS data
    -   Access to Gmod, Codebooks, and Locations for all VIS versions
    -   Version management and latest version queries
    -   See [Sample_Gmod.cpp](samples/core/Sample_Gmod.cpp) for usage examples
-   **`Gmod`** (Generic Product Model) - Equipment hierarchy and product tree
    -   Fast node lookup via operator[]
    -   Tree traversal with customizable handlers
    -   Product type and selection navigation
    -   See [Sample_Gmod.cpp](samples/core/Sample_Gmod.cpp) for usage examples
-   **`GmodPath`** - Validated paths through the Gmod tree
    -   Parse short paths (e.g., "411.1-1P") with tree search
    -   Parse full paths (e.g., "VE/400a/410/411/411i/411.1-1P") for direct resolution
    -   Location-based individualization for equipment mapping
    -   Path comparison and format conversion
    -   See [Sample_GmodPath.cpp](samples/core/Sample_GmodPath.cpp) for usage examples
-   **`Codebooks`** - Metadata tag validation and creation
    -   11 codebook types: Quantity, Content, Calculation, State, Command, Type, Position, etc.
    -   Value validation with standard and custom tag support
    -   Position codebook with special grouping validation
    -   See [Sample_Codebooks.cpp](samples/core/Sample_Codebooks.cpp) for usage examples
-   **`Locations`** - Physical location representation and validation
    -   Parse and validate location strings (e.g., "1PS", "14C")
    -   Location components: Number, Side, Vertical, Transverse, Longitudinal
    -   Fluent builder API via `LocationBuilder`
    -   See [Sample_Locations.cpp](samples/core/Sample_Locations.cpp) for usage examples
-   **`ImoNumber`** - IMO ship identification numbers
    -   Parse from string or integer formats
    -   See [Sample_ImoNumber.cpp](samples/identifiers/Sample_ImoNumber.cpp) for usage examples
-   **`LocalId`** / **`LocalIdBuilder`** - Vista Local ID for sensor identification
    -   Parse Local ID paths (e.g., "/dnv-v2/vis-3-4a/411.1/meta/qty-temperature")
    -   Build Local IDs with primary item, metadata tags, and secondary items
    -   Verbose mode and MQTT formatting support
    -   See [Sample_LocalId.cpp](samples/identifiers/Sample_LocalId.cpp) for usage examples
-   **`UniversalId`** / **`UniversalIdBuilder`** - Vista Universal ID (globally unique sensor IDs)
    -   Combine IMO number with Local ID
    -   Parse Universal ID strings
    -   Format: `data.dnv.com/{imo-number}{local-id}`
    -   See [Sample_UniversalId.cpp](samples/identifiers/Sample_UniversalId.cpp) for usage examples

#### VIS Versioning

-   **Cross-version conversion** - Convert Gmod nodes, paths, and LocalIds between VIS versions
    -   `VIS::convertNode()` - Node conversion with mapping detection
    -   `VIS::convertPath()` - Path conversion with validation
    -   `VIS::convertLocalId()` - LocalId migration between versions
    -   See [Sample_GmodVersioning.cpp](samples/core/Sample_GmodVersioning.cpp) for usage examples

### ISO 19848 Transport Layer

-   **`ShipId`** - Ship identification for ISO 19848 transport
    -   Support for IMO numbers and other ship identification schemes
    -   Parse from string formats
    -   Format validation and conversion
-   **`DataChannel`** / **`DataChannelList`** - ISO 19848 data channel configuration
    -   Create data channel configurations with metadata
    -   Build DataChannelList packages with headers and custom properties
    -   Define validation rules: data types, formats, ranges, units
    -   JSON serialization/deserialization per ISO 19848:2024
    -   Validation with detailed error reporting
    -   See [Sample_DataChannelList.cpp](samples/transport/Sample_DataChannelList.cpp) for usage examples
-   **`TimeSeriesData`** - ISO 19848 time series data transport
    -   Create time series data points with timestamps
    -   Build TimeSeriesData packages with tabular and event data
    -   Cross-validation against DataChannelList with custom callbacks
    -   JSON serialization/deserialization per ISO 19848:2024
    -   Package validation with `ValidateResult<>` control flow
    -   See [Sample_TimeSeriesData.cpp](samples/transport/Sample_TimeSeriesData.cpp) for usage examples

### Advanced Features

#### Query System

-   **`MetadataTagsQuery`** / **`MetadataTagsQueryBuilder`** - Pattern matching for LocalIds by metadata tags
    -   Declarative query building with fluent API
    -   AND semantics for multiple tag matching
-   **`GmodPathQuery`** / **`GmodPathQueryBuilder`** - Pattern matching for Gmod paths
    -   Match paths with/without locations
    -   Node-based filtering
    -   Lambda-based path transformations
-   **`LocalIdQuery`** / **`LocalIdQueryBuilder`** - Composable LocalId filtering
    -   Combine GmodPath and MetadataTags queries
    -   Lambda-based query composition

See [Sample_Queries.cpp](samples/advanced/Sample_Queries.cpp) for usage examples of all query types.

#### ISO 19848 Specifications

-   **`ISO19848`** - Singleton providing ISO 19848 standard specifications
    -   Access to data channel type names per version
    -   Format data types and validation rules
    -   Version management and latest version queries
    -   See [Sample_ISO19848.cpp](samples/transport/Sample_ISO19848.cpp) for usage examples

### Quick Start

#### Core VIS API

```cpp
#include <dnv/vista/sdk/VIS.h>

using namespace dnv::vista::sdk;

// Get VIS singleton and access versioned data
const auto& vis = VIS::instance();
const auto& gmod = vis.gmod(vis.latest());
const auto& locations = vis.locations(vis.latest());

// Parse a GMOD path
auto path = GmodPath::fromShortPath("411.1-1P", gmod, locations);
if (path.has_value()) {
    std::cout << "Full path: " << path->toFullPathString() << "\n";
}

// Validate a location
auto location = locations.fromString("1PS");
if (location.has_value()) {
    std::cout << "Valid location: " << location->value() << "\n";
}
```

#### ISO 19848 Transport API

```cpp
#include <dnv/vista/sdk/Transport.h>
#include <dnv/vista/sdk/VIS.h>

using namespace dnv::vista::sdk;
using namespace dnv::vista::sdk::transport;

// Create DataChannelList
auto shipId = ShipId::fromString("IMO1234567").value();
auto header = datachannel::Header{
    shipId,
    datachannel::ConfigurationReference{"config-v1", DateTimeOffset{"2024-01-01T00:00:00Z"}, "1.0"}
};

datachannel::DataChannelList list;
// Add channels...

// Serialize to JSON
auto options = serialization::json::Serializer<datachannel::DataChannelListPackage>::Options{};
options.prettyPrint = true;
auto json = serialization::json::Serializer<datachannel::DataChannelListPackage>::toString(
    datachannel::DataChannelListPackage{datachannel::Package{header, list}}, 
    options
);
```

For complete examples, see [Sample_DataChannelList.cpp](samples/transport/Sample_DataChannelList.cpp) and [Sample_TimeSeriesData.cpp](samples/transport/Sample_TimeSeriesData.cpp).

## Building from Source

### Prerequisites

- **CMake** 3.20 or later
- **C++20 compiler (concepts support)**:
  - **Minimum requirements:**
    - GCC 10+ 
    - Clang 10+
    - MSVC 19.26+
  - **Tested compilers:**
    - GCC 12.2.0 (Linux)
    - GCC 14.2.0 (Linux)
    - Clang 16.0.6 (Linux)
    - Clang 19.1.7 (Linux)
    - MinGW GCC 14.2.0 (Windows)
    - MSVC 19.44.35214 (Windows)

- Internet connection (for automatic dependency fetching via CMake FetchContent)

### Basic Build

```bash
# Configure
cmake -B build

# Build the library (static by default)
cmake --build build
```

### Build Options

```cmake
# --- Library build types ---
option(DNV_VISTA_SDK_CPP_BUILD_STATIC        "Build static library" ON )
option(DNV_VISTA_SDK_CPP_BUILD_SHARED        "Build shared library" OFF)

# --- Build components ---
option(DNV_VISTA_SDK_CPP_BUILD_TOOLS         "Build tools"          OFF)
option(DNV_VISTA_SDK_CPP_BUILD_TESTS         "Build tests"          OFF)
option(DNV_VISTA_SDK_CPP_BUILD_SAMPLES       "Build samples"        OFF)
option(DNV_VISTA_SDK_CPP_BUILD_BENCHMARKS    "Build benchmarks"     OFF)
option(DNV_VISTA_SDK_CPP_BUILD_DOCUMENTATION "Build documentation"  OFF)

# --- Specialized test components ---
option(DNV_VISTA_SDK_CPP_BUILD_SMOKE_TESTS   "Build smoke tests"    OFF)
```

**Note:** Smoke tests perform **exhaustive validation** including:
- Full Gmod tree traversal across all VIS versions
- Cross-version path conversion testing (thousands of paths)
- Multi-threaded validation with detailed failure analysis
- Execution time: **hours** depending on hardware
- Use for validation before major releases

### Example: Build Everything

```bash
cmake -B build \
  -DDNV_VISTA_SDK_CPP_BUILD_SAMPLES=ON \
  -DDNV_VISTA_SDK_CPP_BUILD_TOOLS=ON \
  -DDNV_VISTA_SDK_CPP_BUILD_TESTS=ON \
  -DDNV_VISTA_SDK_CPP_BUILD_BENCHMARKS=ON

cmake --build build
```

## Build Workflow

The SDK uses a multi-stage code generation pipeline:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Stage 1: Resource Embedding                                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│ resources/*.json.gz → embed_resources() -> generated/*.cpp                  │
│                                       │                                     │
│                                       └ -> dnv-vista-cpp-embeddedresources  │
│                                            (CMake OBJECT library)           │
│                                                                             │
│ Embeds compressed JSON resources as C++ byte arrays in object files         │
└─────────────────────────────────────────────────────────────────────────────┘
                   ┌───────────────────┴───────────────────┐
                   ↓                                       ↓
┌─────────────────────────────────────┐ ┌─────────────────────────────────────┐
│ Stage 2A: VIS Versions Generation   │ │ Stage 2B: ISO Versions Generation   │
├─────────────────────────────────────┤ ├─────────────────────────────────────┤
│   dnv-vista-cpp-embeddedresources   │ │   dnv-vista-cpp-embeddedresources   │
│                  ↓                  │ │                  ↓                  │
│         VisVersionsGenerator        │ │      ISO19848VersionsGenerator      │
│                  ↓                  │ │                  ↓                  │
│         VisVersions.h               │ │      ISO19848Versions.h             │
│         VisVersionsExtensions.h     │ │      ISO19848VersionsExtensions.h   │
│                                     │ │                                     │
│ Generates VIS versions enums and    │ │ Generates ISO 19848 versions enums  │
│ utilities from embedded VIS data    │ │ and utilities from embedded data    │
└─────────────────────────────────────┘ └─────────────────────────────────────┘
                   └───────────────────┬───────────────────┘
                                       ↓
┌─────────────────────────────────────────────────────────────────────────────┐
│ Stage 3: SDK Library Compilation                                            │
├─────────────────────────────────────────────────────────────────────────────┤
│ Generated headers + SDK sources + embeddedresources objects                 │
│                                 │                                           │
│                                 └-> dnv-vista-sdk-cpp-static.a (Linux)      │
│                                    dnv-vista-sdk-cpp-static.lib (Windows)   │
└─────────────────────────────────────────────────────────────────────────────┘
```

All stages are automated via CMake custom commands and execute during the build.

## Project Structure

```
vista-sdk/cpp/
├── benchmark/              # Performance benchmarks with Google Benchmark
├── cmake/                  # CMake modules and build configuration
├── doc/                    # Doxygen documentation configuration
├── include/dnv/vista/sdk/  # Public API headers
├── licenses/               # Third-party license files
├── samples/                # Usage examples
├── src/                    # Implementation files
├── test/                   # Unit tests with GoogleTest
├── tools/                  # Command-line utilities for VIS exploration
├── CMakeLists.txt          # Main CMake build configuration
└── README.md               # This documentation file
```

## Samples

The SDK includes usage examples demonstrating the API. See [samples/README.md](samples/README.md) for detailed documentation.

```bash
cmake -B build -DDNV_VISTA_SDK_CPP_BUILD_SAMPLES=ON
cmake --build build

# Example usage
./build/bin/Sample_Codebooks
./build/bin/Sample_Gmod
./build/bin/Sample_GmodPath
./build/bin/Sample_DataChannelList
./build/bin/Sample_TimeSeriesData
./build/bin/Sample_SensorDataFlow
./build/bin/Sample_UniversalId
```

## Command-Line Tools

The SDK includes CLI utilities for exploring VIS data interactively. See [tools/README.md](tools/README.md) for detailed documentation.

```bash
cmake -B build -DDNV_VISTA_SDK_CPP_BUILD_TOOLS=ON
cmake --build build

# Example usage
./build/bin/codebooks-cli --search pressure
./build/bin/locations-cli --code S
./build/bin/gmod-cli --search "engine"
./build/bin/localid-cli /dnv-v2/vis-3-4a/411.1/meta/qty-temperature
./build/bin/json-validator-cli --schema DataChannelList sample.json
```

## Performance

The SDK includes performance benchmarks using Google Benchmark. See [benchmark/README.md](benchmark/README.md) for detailed metrics and analysis.

```bash
cmake -B build -DDNV_VISTA_SDK_CPP_BUILD_BENCHMARKS=ON
cmake --build build

# Run all benchmarks
./build/bin/BM_CodebooksLookup
./build/bin/BM_DataChannelListSerialization
./build/bin/BM_GmodLoad
./build/bin/BM_GmodLookup
# ... and more
```

## API Documentation

The SDK includes complete API documentation generated with Doxygen.

```bash
cmake -B build -DDNV_VISTA_SDK_CPP_BUILD_DOCUMENTATION=ON
cmake --build build

# Open documentation (Linux)
xdg-open build/doc/html/index.html

# Open documentation (Windows)
build\doc\index.html.cmd
```
## License

This project is licensed under the MIT License.

## Core dependencies

-   **[zlib-ng](https://github.com/zlib-ng/zlib-ng)**: zlib replacement for .gz decompression (zlib License)
-   **[nfx-serialization](https://github.com/nfx-libs/nfx-serialization)**: JSON serialization library for C++ (MIT License)
-   **[nfx-stringbuilder](https://github.com/nfx-libs/nfx-stringbuilder)**: String builder library for C++ (MIT License)
-   **[nfx-stringutils](https://github.com/nfx-libs/nfx-stringutils)**: String utilities library (MIT License)
-   **[nfx-containers](https://github.com/nfx-libs/nfx-containers)**: Perfect hash map and container library (MIT License)
-   **[nfx-hashing](https://github.com/nfx-libs/nfx-hashing)**: Hash function library (MIT License)
-   **[nfx-resource](https://github.com/nfx-libs/nfx-resource)**: Resource embedding library (MIT License)
-   **[nfx-datetime](https://github.com/nfx-libs/nfx-datetime)**: Date and time utilities (MIT License)
-   **[nfx-datatypes](https://github.com/nfx-libs/nfx-datatypes)**: Common data types (MIT License)

### Development Dependencies

-   **[GoogleTest](https://github.com/google/googletest)**: Testing framework (BSD 3-Clause License) - Development only
-   **[Google Benchmark](https://github.com/google/benchmark)**: Performance benchmarking framework (Apache 2.0 License) - Development only

All dependencies are automatically fetched via CMake FetchContent when building the library, tests, or benchmarks.

## Notes for maintainers

### Version Numbering

The SDK follows [Semantic Versioning](https://semver.org/) (MAJOR.MINOR.PATCH):
- **MAJOR**: Breaking API changes
- **MINOR**: New features, backward compatible
- **PATCH**: Bug fixes, backward compatible

### Creating a Release

**Update version in CMakeLists.txt:**
```cmake
project(dnv-vista-sdk-cpp
    VERSION X.Y.Z  # Update this line
    DESCRIPTION "C++ implementation of DNV Vista SDK for VIS and ISO 19847/19848 standards"
    HOMEPAGE_URL "https://github.com/dnv-opensource/vista-sdk"
    LANGUAGES CXX
)
```

---

_Updated on February 11, 2026_
