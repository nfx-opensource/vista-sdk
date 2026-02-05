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
| **Windows** | BenchmarkDotNet v0.13.10 | .NET 8.0.20 X64 RyuJIT AVX2 |        v0.1.0        |

---

# Performance Results

## Core Components

### Codebooks

#### Codebooks lookup

| Benchmark           | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| ------------------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **StdUnorderedMap** | 6.39  ns  |   1.64 ns   |     _N/A_     |   6.36 ns   |   1.87 ns    |      _N/A_      |
| **StdMap**          | 3.63  ns  |   5.58 ns   |     _N/A_     |   3.60 ns   |   10.2 ns    |      _N/A_      |
| **HashMap**         | 3.25  ns  |   3.30 ns   |     _N/A_     |   3.23 ns   |   3.22 ns    |      _N/A_      |
| **Dict**            |   _N/A_   |    _N/A_    |   4.452 ns    |    _N/A_    |    _N/A_     |    4.437 ns     |
| **FrozenDict**      |   _N/A_   |    _N/A_    |   1.284 ns    |    _N/A_    |    _N/A_     |    1.538 ns     |
| **Codebooks**       | 0.472 ns  |  0.673 ns   |   0.654 ns    |  0.468 ns   |   2.56 ns    |    0.717 ns     |

### Gmod

#### Gmod load

**Note:** C++ VIS caches Gmod instances (singleton). C++ benchmark measures cached access, C# measures full load (parse + construct). Not directly comparable.

| Benchmark    | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| ------------ | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **GmodLoad** |  15.3 ns  |   15.1 ns   |   34.84 ms    |   62.6 ns   |   14.0 ns    |    29.99 ms     |

#### Gmod lookup

| Benchmark           | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| ------------------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **StdUnorderedMap** |  29.0 ns  |   32.0 ns   |     _N/A_     |   31.9 ns   |   32.8 ns    |      _N/A_      |
| **FrozenDict**      |   _N/A_   |    _N/A_    |    24.7 ns    |    _N/A_    |    _N/A_     |     27.7 ns     |
| **Dict**            |   _N/A_   |    _N/A_    |   16.51 ns    |    _N/A_    |    _N/A_     |    15.29 ns     |
| **Gmod**            |  6.69 ns  |   8.68 ns   |   15.39 ns    |   6.47 ns   |   17.1 ns    |    15.32 ns     |

#### Gmod path parse

| Benchmark                                                       | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| --------------------------------------------------------------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **FromShortPath / TryParse**                                    | 1.77  us  |  1.61  us   |   3.122 us    |   2.53 us   |   3.09 us    |     2.87 us     |
| **FromFullPath / TryParseFullPath**                             | 0.555 us  |  0.582 us   |   0.555 us    |  0.787 us   |   1.02 us    |    0.517 us     |
| **FromShortPathIndividualized / TryParseIndividualized**        | 1.22  us  |  1.07  us   |   1.443 us    |   1.68 us   |   1.81 us    |     1.24 us     |
| **FromFullPathIndividualized / TryParseFullPathIndividualized** | 0.823 us  |  0.782 us   |   0.760 us    |   1.15 us   |   1.31 us    |    0.646 us     |

#### Gmod traversal

| Benchmark         | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| ----------------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **FullTraversal** |  87.3 ms  |   96.0 ms   |   126.2 ms    |   99.1 ms   |    113 ms    |    121.2 ms     |

#### Gmod versioning convert path

| Benchmark                   | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| --------------------------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **ConvertPath**             |  1.56 us  |   1.47 us   |    1.33 us    |   2.12 us   |   2.33 us    |     1.27 us     |
| **ConvertPathWithLocation** |  1.62 us  |   1.45 us   |     _N/A_     |   1.99 us   |   2.19 us    |      _N/A_      |

## Transport Layer

### DataChannelList Serialization

| Benchmark | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| --------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **Json**  |  8.66 us  |   8.53 us   |   10.11 us    |   14.3 us   |   13.4 us    |    10.15 us     |

### TimeSeriesDataSerialization Serialization

| Benchmark | linux GCC | linux clang | linux C# .NET | Windows GCC | Windows MSVC | Windows C# .NET |
| --------- | :-------: | :---------: | :-----------: | :---------: | :----------: | :-------------: |
| **Json**  |  5.04 us  |   4.72 us   |     _N/A_     |   7.62 us   |   7.33 us    |      _N/A_      |

## See Also

- [Main SDK Documentation](../README.md)
- [Sample Programs](../samples/README.md)
- [Command-Line Tools](../tools/README.md)

---

_Benchmarked on February 5, 2026_
