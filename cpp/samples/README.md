# Vista SDK C++ - Usage Examples

Sample programs demonstrating the Vista SDK C++ API.

## Available Samples

### Core VIS Components

**Sample_Codebooks.cpp** - VIS Codebooks API
- Access VIS singleton and version management
- Browse and search codebook standard values
- Validate metadata tags
- Create custom metadata tags

**Sample_Gmod.cpp** - Generic Product Model Navigation
- Access GMOD tree nodes
- Navigate product hierarchy
- Query node metadata and definitions
- Explore product types and selections

**Sample_GmodPath.cpp** - GMOD Path Parsing
- Parse short paths (e.g., "411.1-1P")
- Parse full paths (e.g., "VE/400a/410/411/411i/411.1-1P")
- Handle parsing errors
- Path comparison and validation
- Location-based individualization

**Sample_Locations.cpp** - Location Codes
- Parse location strings
- Validate location components
- Use LocationBuilder fluent API
- Access location metadata

**Sample_ImoNumber.cpp** - IMO Numbers
- Parse IMO numbers from strings
- Validate IMO checksum
- Format IMO numbers

### Local ID and Universal ID

**Sample_LocalId.cpp** - Vista Local ID
- Parse Local ID paths
- Access ID components (naming rule, VIS version, primary item, metadata)
- Validate Local IDs

**Sample_UniversalId.cpp** - Vista Universal ID
- Parse Universal ID strings
- Access ID components (IMO number, Local ID)
- Build Universal IDs programmatically
- Combine IMO numbers with Local IDs

### ISO 19848 Transport

**Sample_ISO19848.cpp** - ISO 19848 Package Validation
- Validate DataChannelList packages
- Validate TimeSeriesData packages
- Access validation results and error details

**Sample_DataChannelList.cpp** - DataChannelList API
- Create DataChannel configurations
- Build DataChannelList packages
- Serialize to JSON (ISO 19848)
- Deserialize and validate

**Sample_TimeSeriesData.cpp** - TimeSeriesData API
- Create time series data points
- Build TimeSeriesData packages
- Serialize to JSON (ISO 19848)
- Deserialize and validate

### Advanced Features

**Sample_GmodVersioning.cpp** - GMOD Path Versioning
- Convert paths between VIS versions
- Handle version compatibility
- Map equipment across versions

**Sample_Queries.cpp** - Advanced Queries
- Search across GMOD tree
- Filter by node properties
- Custom traversal handlers

## Building and Running

Build all samples:

```bash
cmake -B build -DDNV_VISTA_SDK_CPP_BUILD_SAMPLES=ON
cmake --build build
```

Run a specific sample:

```bash
./build/bin/Sample_Codebooks
./build/bin/Sample_Gmod
./build/bin/Sample_GmodPath
./build/bin/Sample_Locations
./build/bin/Sample_ImoNumber
./build/bin/Sample_LocalId
./build/bin/Sample_UniversalId
./build/bin/Sample_ISO19848
./build/bin/Sample_DataChannelList
./build/bin/Sample_TimeSeriesData
./build/bin/Sample_GmodVersioning
./build/bin/Sample_Queries
```

## See Also

- [Main SDK Documentation](../README.md)
- [Command-Line Tools](../tools/README.md)
- [Benchmark Results](../benchmark/README.md)

---

_Updated on January 28, 2026_
