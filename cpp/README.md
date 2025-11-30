# Vista SDK - C++ Implementation

<!-- Project Info -->

[![License](https://img.shields.io/github/license/ronan-fdev/vista-sdk?style=flat-square)](https://github.com/ronan-fdev/vista-sdk/blob/main/LICENSE)<br/>

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue?style=flat-square)](#) [![CMake](https://img.shields.io/badge/CMake-3.20+-green.svg?style=flat-square)](#) [![Cross Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-lightgrey?style=flat-square)](#) <br/>

<!-- CI/CD Status -->

[![Linux GCC](https://img.shields.io/github/actions/workflow/status/ronan-fdev/vista-sdk/build-cpp-linux-gcc.yml?branch=cpp&label=Linux%20GCC&style=flat-square)](https://github.com/ronan-fdev/vista-sdk/actions/workflows/build-cpp-linux-gcc.yml) [![Linux Clang](https://img.shields.io/github/actions/workflow/status/ronan-fdev/vista-sdk/build-cpp-linux-clang.yml?branch=cpp&label=Linux%20Clang&style=flat-square)](https://github.com/ronan-fdev/vista-sdk/actions/workflows/build-cpp-linux-clang.yml) [![Windows MinGW](https://img.shields.io/github/actions/workflow/status/ronan-fdev/vista-sdk/build-cpp-windows-mingw.yml?branch=cpp&label=Windows%20MinGW&style=flat-square)](https://github.com/ronan-fdev/vista-sdk/actions/workflows/build-cpp-windows-mingw.yml) [![Windows MSVC](https://img.shields.io/github/actions/workflow/status/ronan-fdev/vista-sdk/build-cpp-windows-msvc.yml?branch=cpp&label=Windows%20MSVC&style=flat-square)](https://github.com/ronan-fdev/vista-sdk/actions/workflows/build-cpp-windows-msvc.yml)

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
-   Multi-compiler builds supported

### CMake Integration

The library supports modular compilation through CMake options:

```cmake
# Build optimization
option(VISTA_SDK_CPP_USE_CCACHE                 "Enable compiler cache"                      ON  )

# Library build types
option(VISTA_SDK_CPP_BUILD_STATIC               "Build static library"                       ON   )
option(VISTA_SDK_CPP_BUILD_SHARED               "Build shared library"                       AUTO )

# Development
option(VISTA_SDK_CPP_BUILD_TESTS                "Build tests"                                AUTO )
option(VISTA_SDK_CPP_BUILD_SMOKE_TESTS          "Build smoke tests"                          OFF  )
option(VISTA_SDK_CPP_BUILD_BENCHMARKS           "Build benchmarks"                           AUTO )
option(VISTA_SDK_CPP_BUILD_SAMPLES              "Build vista-sdk-cpp samples"                AUTO )
option(VISTA_SDK_CPP_BUILD_DOCUMENTATION        "Build vista-sdk-cpp Doxygen documentation"  AUTO )
option(VISTA_SDK_CPP_USE_CUSTOM_COMPILER_FLAGS  "Skip automatic compiler configuration"      OFF  )

# Installation
option(VISTA_SDK_CPP_INSTALL_PROJECT            "Install project"                            ON   )

# Packaging
option(VISTA_SDK_CPP_PACKAGE_SOURCE             "Enable source package generation"           ON   )
option(VISTA_SDK_CPP_PACKAGE_ARCHIVE            "Enable TGZ/ZIP package generation"          ON   )
option(VISTA_SDK_CPP_PACKAGE_DEB                "Enable DEB package generation"              ON   )
option(VISTA_SDK_CPP_PACKAGE_RPM                "Enable RPM package generation"              ON   )
option(VISTA_SDK_CPP_PACKAGE_NSIS               "Enable NSIS Windows installer"              ON   )
```

### Using in Your Project

#### Option 1: Using FetchContent (Build from source)

```cmake
include(FetchContent)
FetchContent_Declare(
  vista-sdk-cpp
  GIT_REPOSITORY https://github.com/ronan-fdev/vista-sdk.git
  GIT_TAG        cpp
  SOURCE_SUBDIR  cpp
)
FetchContent_MakeAvailable(vista-sdk-cpp)

# Link with static library (recommended for most use cases)
target_link_libraries(your_target vista-sdk-cpp::static)

# Or link with shared library
# target_link_libraries(your_target vista-sdk-cpp::vista-sdk-cpp)
```

#### Option 2: As a Git Submodule

**Setup Steps (one-time):**

```bash
# From your project root directory
git submodule add https://github.com/ronan-fdev/vista-sdk.git third-party/vista-sdk
git commit -m "Add vista-sdk as submodule"
```

**CMakeLists.txt:**

```cmake
# Add Vista SDK C++ as a subdirectory
add_subdirectory(third-party/vista-sdk/cpp)

# Link with the library
target_link_libraries(your_target vista-sdk-cpp::static)
```

**Clone/Update Commands:**

```bash
# When cloning your project (for new developers)
# Replace with your actual project URL:
git clone --recursive https://github.com/YOUR-USERNAME/YOUR-PROJECT.git

# Or if already cloned without --recursive
git submodule update --init --recursive

# To update the submodule to latest version
cd third-party/vista-sdk
git pull origin cpp
cd ../..
git add third-party/vista-sdk
git commit -m "Update vista-sdk submodule"
```

#### Option 3: Using find_package (for installed libraries)

```cmake
# Find the installed Vista SDK C++ library
find_package(vista-sdk-cpp REQUIRED)

# Link with static library (recommended for most use cases)
target_link_libraries(your_target vista-sdk-cpp::static)

# Or link with shared library
# target_link_libraries(your_target vista-sdk-cpp::vista-sdk-cpp)
```

#### Integration Method Comparison

| Method           | Best For                              | Pros                                              | Cons                                              |
| ---------------- | ------------------------------------- | ------------------------------------------------- | ------------------------------------------------- |
| **find_package** | Production builds, system packages    | Fast builds, version control, clean separation    | Requires pre-installation                         |
| **FetchContent** | CI/CD, development, specific versions | Always available, version pinning, no pre-install | Longer build times, downloads on each clean build |
| **Submodule**    | Long-term projects, offline builds    | Full source control, offline capability           | Manual updates, Git complexity                    |

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
ctest -C Release
```

### Documentation

Vista SDK C++ includes comprehensive API documentation generated with Doxygen.

#### Building Documentation

```bash
# Configure with documentation enabled
cmake .. -DCMAKE_BUILD_TYPE=Release -DVISTA_SDK_CPP_BUILD_DOCUMENTATION=ON

# Build the documentation
cmake --build . --target documentation

# Documentation will be generated in build/doc/html/
```

#### Requirements

-   **Doxygen** - Documentation generation tool
-   **Graphviz Dot** (optional) - For generating class diagrams and dependency graphs

#### Accessing Documentation

After building, open the documentation in your browser:

```bash
# Linux
xdg-open build/vista-sdk-cpp-*/*/Release/doc/html/index.html     # Most Linux systems
firefox build/vista-sdk-cpp-*/*/Release/doc/html/index.html      # If Firefox is installed

# Windows
start build/vista-sdk-cpp-*/*/Release/doc/html/index.html
```

## Installation & Packaging

Vista SDK C++ provides comprehensive packaging options for distribution across multiple platforms and package managers.

### Package Generation

The library supports generating packages in multiple formats:

```bash
# Configure with packaging options (including both static and shared libraries)
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DVISTA_SDK_CPP_BUILD_STATIC=ON \
         -DVISTA_SDK_CPP_BUILD_SHARED=ON \
         -DVISTA_SDK_CPP_PACKAGE_ARCHIVE=ON \
         -DVISTA_SDK_CPP_PACKAGE_DEB=ON \
         -DVISTA_SDK_CPP_PACKAGE_RPM=ON \
         -DVISTA_SDK_CPP_PACKAGE_NSIS=ON \
         -DVISTA_SDK_CPP_PACKAGE_SOURCE=ON

# Generate binary packages
cmake --build . --target package

# Generate source packages (recommended method)
cd build
cpack --config CPackSourceConfig.cmake
```

### Supported Package Formats

| Format      | Platform       | Description                              | Requirements |
| ----------- | -------------- | ---------------------------------------- | ------------ |
| **TGZ/ZIP** | Cross-platform | Compressed archive packages              | None         |
| **DEB**     | Debian/Ubuntu  | Native Debian packages with dependencies | `dpkg-dev`   |
| **RPM**     | RedHat/SUSE    | Native RPM packages with dependencies    | `rpm-build`  |
| **NSIS**    | Windows        | Professional Windows installer (.exe)    | `NSIS 3.03+` |
| **Source**  | Cross-platform | Source code distribution                 | None         |

### Package Options

Control which packages are generated with CMake options:

```cmake
# Package generation control
option(VISTA_SDK_CPP_PACKAGE_ARCHIVE      "Generate TGZ/ZIP packages"          ON   )
option(VISTA_SDK_CPP_PACKAGE_DEB          "Generate DEB packages (Linux)"      ON   )
option(VISTA_SDK_CPP_PACKAGE_RPM          "Generate RPM packages (Linux)"      ON   )
option(VISTA_SDK_CPP_PACKAGE_NSIS         "Generate NSIS installer (Windows)"  ON   )
option(VISTA_SDK_CPP_PACKAGE_SOURCE       "Generate source packages"           ON   )
```

### Linux Package Dependencies

**DEB packages** automatically include runtime dependencies:

-   `libc6`, `libstdc++6`, `libgcc-s1` (core runtime)
-   `zlib1g` (compression support)

**RPM packages** automatically include runtime dependencies:

-   `glibc`, `libstdc++` (core runtime)
-   `zlib` (compression support)

### Windows Installer Features

The NSIS installer provides:

-   **64-bit Program Files installation**
-   **Automatic PATH modification** for easy command-line access
-   **Proper uninstaller registration** in Windows Add/Remove Programs
-   **DPI-aware installation** for modern displays

### Cross-Platform Installation

```bash
# Linux (DEB-based systems)
sudo dpkg -i vista-sdk-cpp-*.deb
sudo apt-get install -f  # Fix dependencies if needed

# Linux (RPM-based systems)
sudo rpm -ivh vista-sdk-cpp-*.rpm

# Windows
# Run the .exe installer with administrator privileges
vista-sdk-cpp-*-win64.exe

# Manual installation (all platforms)
# Extract TGZ/ZIP to desired location
tar -xzf vista-sdk-cpp-*.tar.gz -C /usr/local/
# or
unzip vista-sdk-cpp-*.zip -d "C:\Program Files\"
```

### Package Contents

All packages include:

-   **Headers** (`include/dnv/vista/sdk/`) - Public API headers
-   **Libraries** (`lib/`) - Static/shared libraries
-   **Documentation** (`doc/`) - API documentation (if built)
-   **Licenses** (`licenses/`) - All license files
-   **Examples** (`samples/`) - Usage examples (if built)

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

The `samples/` directory contains comprehensive examples demonstrating all Vista SDK C++- features.

To build and run samples:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DVISTA_SDK_CPP_BUILD_SAMPLES=ON
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

## Version History

For detailed version history, feature additions, and breaking changes, see [CHANGELOG.md](CHANGELOG.md).

## Performance

Vista SDK C++ is designed for efficient maritime data processing with modern C++ optimization techniques.

For detailed performance metrics and cross-language comparisons, see [benchmark/README.md](benchmark/README.md).

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

## CPU Architecture Detection

Vista SDK C++ includes CPU feature detection to optimize performance across different hardware generations while ensuring broad compatibility for source builds.

### Build-Time CPU Detection

The library automatically detects CPU capabilities during CMake configuration and conditionally enables optimizations:

-   **Configure-time AVX2 detection**: Uses `CheckCXXSourceRuns` with AVX2 intrinsics (`_mm256_*`) to test actual CPU support on the build machine
-   **Conditional compilation flags**:
    -   **MSVC**: Adds `/arch:AVX2` only if CPU supports it, otherwise falls back to `/arch:SSE2`
    -   **GCC/Clang**: Adds `-mavx2` and `-mfma` only if CPU supports AVX2, otherwise uses SSE4.2 baseline
-   **Hardware-specific builds**: Each build is optimized for the specific CPU it's built on (Haswell+ gets AVX2, older CPUs get SSE fallback)

## License

This project is licensed under the MIT License - see the [LICENSE](https://github.com/dnv-opensource/vista-sdk/blob/main/LICENSE) file for details.

### Dependencies & Third-Party Attributions

-   **[nfx-meta](https://github.com/ronan-fdev/nfx-meta)**: High-performance containers, utilities, and JSON serialization framework (embeds nlohmann/json)
-   **[zlib-ng](https://github.com/zlib-ng/zlib-ng)**: High-performance zlib compression library
-   **[GoogleTest](https://github.com/google/googletest)**: Testing framework
-   **[Google Benchmark](https://github.com/google/benchmark)**: Performance benchmarking framework

## Official Vista Resources

### 📚 Documentation

-   **[VISTA Documentation](https://docs.vista.dnv.com/)** - Complete standards documentation

### 🛠️ Tools

-   **[GMOD Viewer](https://vista.dnv.com/gmod)** - Interactive VIS hierarchy explorer
-   **[Learning Tool](https://vista.dnv.com/learning-tool)** - LocalID construction tutorial

---

_Updated on October 9, 2025_
