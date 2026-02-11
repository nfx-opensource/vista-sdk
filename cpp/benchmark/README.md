# VISTA SDK C++ - Performance Benchmarks

---

## Test Environment

### Hardware Configuration

| Component                | Specification                                                 |
| ------------------------ | ------------------------------------------------------------- |
| **CPU**                  | 12th Gen Intel Core i7-12800H (20 logical, 14 physical cores) |
| **Base Clock**           | 2.80 GHz                                                      |
| **Turbo Clock**          | 4.80 GHz                                                      |
| **L1 Data Cache**        | 48 KiB (×10 cores)                                            |
| **L1 Instruction Cache** | 32 KiB (×10 cores)                                            |
| **L2 Unified Cache**     | 1280 KiB (×10 cores)                                          |
| **L3 Unified Cache**     | 24576 KiB (×1 shared)                                         |
| **RAM**                  | DDR4-3200 (32GB)                                              |
| **GPU**                  | NVIDIA RTX A2000 4GB GDDR6                                    |

### Software Configuration

### C++

| Platform    | Benchmark Framework     | C++ Compiler         | Vista SDK c++ Version |
| ----------- | ----------------------- | -------------------- | :-------------------: |
| **Linux**   | Google Benchmark v1.9.5 | GCC 14.2.0           |        v0.0.0         |
| **Linux**   | Google Benchmark v1.9.5 | Clang 16.0.6         |        v0.0.0         |
| **Windows** | Google Benchmark v1.9.5 | MinGW GCC 14.2.0-x64 |        v0.0.0         |
| **Windows** | Google Benchmark v1.9.5 | MSVC 19.44.35214-x64 |        v0.0.0         |

### C#

| Platform    | Benchmark Framework      | .NET Runtime                | Vista SDK C# Version |
| ----------- | ------------------------ | --------------------------- | :------------------: |
| **Linux**   | BenchmarkDotNet v0.13.10 | .NET 10.0.2 X64 RyuJIT AVX2 |        v0.1.0        |
| **Windows** | BenchmarkDotNet v0.13.10 | .NET 10.0.3 X64 RyuJIT AVX2 |        v0.1.0        |

---

# Performance Results

## Core Components

### Codebooks

#### Codebooks lookup

| Benchmark           | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| ------------------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **StdUnorderedMap** | 6.94  ns  |   1.65 ns   |     _N/A_     |   7.15 ns   |   2.09 ns    |      _N/A_      |
| **StdMap**          | 4.15  ns  |   5.62 ns   |     _N/A_     |   4.34 ns   |   11.1 ns    |      _N/A_      |
| **HashMap**         | 3.62  ns  |   3.50 ns   |     _N/A_     |   3.70 ns   |   3.52 ns    |      _N/A_      |
| **Dict**            |   _N/A_   |    _N/A_    |   4.452 ns    |    _N/A_    |    _N/A_     |     4.68 ns     |
| **FrozenDict**      |   _N/A_   |    _N/A_    |   1.284 ns    |    _N/A_    |    _N/A_     |     1.65 ns     |
| **Codebooks**       | 0.536 ns  |  0.683 ns   |   0.654 ns    |  0.552 ns   |   2.76 ns    |     0.78 ns     |

### Gmod

#### Gmod load

**Note:** C++ VIS caches Gmod instances (singleton). C++ benchmark measures cached access, C# measures full load (parse + construct). Not directly comparable.

| Benchmark    | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| ------------ | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **GmodLoad** |  16.6 ns  |   15.1 ns   |   34.84 ms    |   75.0 ns   |   16.1 ns    |    31.37 ms     |

#### Gmod lookup

| Benchmark           | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| ------------------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **StdUnorderedMap** |  33.0 ns  |   36.5 ns   |     _N/A_     |   38.2 ns   |   34.9 ns    |      _N/A_      |
| **FrozenDict**      |   _N/A_   |    _N/A_    |    24.7 ns    |    _N/A_    |    _N/A_     |    27.75 ns     |
| **Dict**            |   _N/A_   |    _N/A_    |   16.51 ns    |    _N/A_    |    _N/A_     |    16.67 ns     |
| **Gmod**            |  6.75 ns  |   8.17 ns   |   15.39 ns    |   6.89 ns   |   16.1 ns    |    16.33 ns     |

#### Gmod path parse

| Benchmark                                                       | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| --------------------------------------------------------------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **FromShortPath / TryParse**                                    | 1.45  us  |   1.41 us   |   3.122 us    |   1.97 us   |   2.52 us    |     2.91 us     |
| **FromFullPath / TryParseFullPath**                             | 0.526 us  |  0.613 us   |   0.555 us    |  0.721 us   |   1.02 us    |    0.540 us     |
| **FromShortPathIndividualized / TryParseIndividualized**        | 1.01  us  |  0.986 us   |   1.443 us    |   1.43 us   |   1.48 us    |     1.28 us     |
| **FromFullPathIndividualized / TryParseFullPathIndividualized** | 0.838 us  |  0.799 us   |   0.760 us    |   1.09 us   |   1.30 us    |    0.670 us     |

#### Gmod traversal

| Benchmark         | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| ----------------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **FullTraversal** |  34.2 ms  |   44.4 ms   |   126.2 ms    |   38.7 ms   |   60.4 ms    |    121.2 ms     |

#### Gmod versioning convert path

| Benchmark                   | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| --------------------------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **ConvertPath**             |  1.56 us  |   1.67 us   |    1.33 us    |   2.11 us   |   2.33 us    |     1.27 us     |
| **ConvertPathWithLocation** |  1.62 us  |   1.70 us   |     _N/A_     |   1.98 us   |   2.19 us    |      _N/A_      |

## Transport Layer

### DataChannelList Serialization

| Benchmark            | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| -------------------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **JsonUncompressed** |  9.03 us  |   8.42 us   |   10.11 us    |   13.8 us   |   13.6 us    |    10.13 us     |

### TimeSeriesDataSerialization Serialization

| Benchmark            | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| -------------------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **JsonUncompressed** |  4.80 us  |   4.77 us   |     _N/A_     |   7.61 us   |   7.74 us    |      _N/A_      |

## See Also

- [Main SDK Documentation](../README.md)
- [Sample Programs](../samples/README.md)
- [Command-Line Tools](../tools/README.md)

---

_Benchmarked on February 12, 2026_
