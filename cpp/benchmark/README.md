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

| Platform    | Benchmark Framework     | C++ Compiler         | Vista SDK c++ Version | NFX Core Version |
| ----------- | ----------------------- | -------------------- | --------------------- | ---------------- |
| **Windows** | Google Benchmark v1.9.4 | VC++ 19.44.35214-x64 | v0.0.47               | v0.1.2           |
| **Windows** | Google Benchmark v1.9.4 | GCC 14.2.0-x64       | v0.0.47               | v0.1.2           |
| **Linux**   | Google Benchmark v1.9.4 | GCC 12.0-x64         | v0.0.47               | v0.1.2           |
| **Linux**   | Google Benchmark v1.9.4 | Clang 16.0.6         | v0.0.47               | v0.1.2           |

### C#

| Platform    | Benchmark Framework      | .NET Runtime                | Vista SDK C# Version |
| ----------- | ------------------------ | --------------------------- | -------------------- |
| **Windows** | BenchmarkDotNet v0.13.10 | .NET 8.0.20 X64 RyuJIT AVX2 | v0.1.0               |
| **Linux**   | _TBD_                    | _TBD_                       | v0.1.0               |

---

# Performance Results

## Core Components

### Codebooks

#### Codebooks lookup

| Benchmark               | linux GCC | linux clang | Windows GCC | Windows MSVC | linux C# .NET | Windows C# .NET |
| ----------------------- | --------- | ----------- | ----------- | ------------ | ------------- | --------------- |
| **std_unordered_map**   | 6.67 ns   | 1.72 ns     | 6.42 ns     | 2.08 ns      | _N/A_         | _N/A_           |
| **std_map**             | 4.99 ns   | 5.61 ns     | 3.73 ns     | 11.2 ns      | _N/A_         | _N/A_           |
| **Dict**                | _N/A_     | _N/A_       | _N/A_       | _N/A_        | _TBD_         | 4.77 ns         |
| **FrozenDict**          | _N/A_     | _N/A_       | _N/A_       | _N/A_        | _TBD_         | 4.95 ns         |
| **sdk_codebooks**       | 0.396 ns  | 0.500 ns    | 0.409 ns    | 0.815 ns     | _TBD_         | 0.622 ns        |
| **sdk_lookup_operator** | 0.403 ns  | 0.506 ns    | 0.406 ns    | 0.815 ns     | _N/A_         | _N/A_           |

### Gmod

#### Gmod load (C++)

| Benchmark           | linux GCC | linux clang | Windows MINGW GCC | Windows MSVC |
| ------------------- | --------- | ----------- | ----------------- | ------------ |
| **gmodCacheAccess** | 32.2 ns   | 35.3 ns     | 42.6 ns           | 47.5 ns      |

#### Gmod load (C#)

| Benchmark | linux C# .NET | Windows C# .NET |
| --------- | ------------- | --------------- |
| **Load**  | ms            | 30.9 ms         |

#### Gmod lookup

| Benchmark             | linux GCC | linux clang | Windows GCC | Windows MSVC | linux C# .NET | Windows C# .NET |
| --------------------- | --------- | ----------- | ----------- | ------------ | ------------- | --------------- |
| **std_unordered_map** | 46.5 ns   | 42.0 ns     | 35.6 ns     | 13.7 ns      | _N/A_         | _N/A_           |
| **std_map**           | 203 ns    | 474 ns      | 217 ns      | 250 ns       | _N/A_         | _N/A_           |
| **std_unordered_set** | 47.2 ns   | 44.4 ns     | 35.1 ns     | 13.2 ns      | _N/A_         | _N/A_           |
| **std_set**           | 200 ns    | 518 ns      | 213 ns      | 261 ns       | _N/A_         | _N/A_           |
| **std_vector**        | 4869 ns   | 7853 ns     | 4566 ns     | 15218 ns     | _N/A_         | _N/A_           |
| **nfx_hashmap**       | 15.7 ns   | 22.2 ns     | 7.88 ns     | 14.7 ns      | _N/A_         | _N/A_           |
| **nfx_stringmap**     | 31.3 ns   | 52.6 ns     | 52.3 ns     | 10.8 ns      | _N/A_         | _N/A_           |
| **nfx_stringset**     | 29.5 ns   | 54.0 ns     | 52.1 ns     | 11.2 ns      | _N/A_         | _N/A_           |
| **nfx_CHD_hashmap**   | 24.0 ns   | 13.6 ns     | 12.0 ns     | 14.6 ns      | _N/A_         | _N/A_           |
| **Dict**              | _N/A_     | _N/A_       | _N/A_       | _N/A_        | _TBD_         | 39.9 ns         |
| **FrozenDict**        | _N/A_     | _N/A_       | _N/A_       | _N/A_        | _TBD_         | 16.3 ns         |
| **sdk_gmod**          | 22.6 ns   | 14.2 ns     | 12.7 ns     | 14.6 ns      | _TBD_         | 16.5 ns         |

#### Gmod path parse

| Benchmark                          | linux GCC | linux clang | Windows GCC | Windows MSVC | linux C# .NET | Windows C# .NET |
| ---------------------------------- | --------- | ----------- | ----------- | ------------ | ------------- | --------------- |
| **TryParse**                       | 8.18 μs   | 10.3 μs     | 10.6 μs     | 19.2 μs      | _TBD_         | 3.36 μs         |
| **TryParseFullPath**               | 4.19 μs   | 5.22 μs     | 5.79 μs     | 11.8 μs      | _TBD_         | 581 ns          |
| **TryParseIndividualized**         | 6.95 μs   | 8.28 μs     | 8.75 μs     | 13.7 μs      | _TBD_         | 1.44 μs         |
| **TryParseFullPathIndividualized** | 4.86 μs   | 5.85 μs     | 6.63 μs     | 10.1 μs      | _TBD_         | 744 ns          |

#### Gmod traversal

| Benchmark         | linux GCC | linux clang | Windows GCC | Windows MSVC | linux C# .NET | Windows C# .NET |
| ----------------- | --------- | ----------- | ----------- | ------------ | ------------- | --------------- |
| **FullTraversal** | 184 ms    | 223 ms      | 188 ms      | 184 ms       | _TBD_         | 159 ms          |

#### Gmod versioning convert path

| Benchmark       | linux GCC | linux clang | Windows GCC | Windows MSVC | linux C# .NET | Windows C# .NET |
| --------------- | --------- | ----------- | ----------- | ------------ | ------------- | --------------- |
| **ConvertPath** | 6.75 μs   | 7.97 μs     | 9.50μs      | 13.2 μs      | _TBD_         | 1.54 μs         |

---

_Benchmarked on September 25, 2025_
