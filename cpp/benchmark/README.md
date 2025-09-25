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

### C#

| Platform    | Benchmark Framework      | .NET Runtime                | Vista SDK C# Version |
| ----------- | ------------------------ | --------------------------- | -------------------- |
| **Windows** | BenchmarkDotNet v0.13.10 | .NET 8.0.20 X64 RyuJIT AVX2 | v0.1.0               |

---

# Performance Results

## Core Components

### Codebooks

#### Codebooks lookup

| Benchmark               | linux GCC | linux clang | Windows GCC | Windows MSVC | C# .NET  |
| ----------------------- | --------- | ----------- | ----------- | ------------ | -------- |
| **std_unordered_map**   | ns        | ns          | 6.42 ns     | 2.08 ns      | -        |
| **std_map**             | ns        | ns          | 3.73 ns     | 11.2 ns      | -        |
| **Dict**                | -         | -           | -           | -            | 4.77 ns  |
| **FrozenDict**          | -         | -           | -           | -            | 4.95 ns  |
| **sdk_codebooks**       | ns        | ns          | 0.409 ns    | 0.815 ns     | 0.622 ns |
| **sdk_lookup_operator** | ns        | ns          | 0.406 ns    | 0.815 ns     | -        |

### Gmod

#### Gmod load (C++)

| Benchmark           | linux GCC | linux clang | Windows MINGW GCC | Windows MSVC |
| ------------------- | --------- | ----------- | ----------------- | ------------ |
| **gmodCacheAccess** | ns        | ns          | 42.6 ns           | 47.5 ns      |

#### Gmod load (C#)

| Benchmark | Performance |
| --------- | ----------- |
| **Load**  | 30.9 ms     |

#### Gmod lookup

| Benchmark             | linux GCC | linux clang | Windows GCC | Windows MSVC | C# .NET |
| --------------------- | --------- | ----------- | ----------- | ------------ | ------- |
| **std_unordered_map** | ns        | ns          | 35.6 ns     | 13.7 ns      | -       |
| **std_map**           | ns        | ns          | 217 ns      | 250 ns       | -       |
| **std_unordered_set** | ns        | ns          | 35.1 ns     | 13.2 ns      | -       |
| **std_set**           | ns        | ns          | 213 ns      | 261 ns       | -       |
| **std_vector**        | ns        | ns          | 4566 ns     | 15218 ns     | -       |
| **nfx_hashmap**       | ns        | ns          | 7.88 ns     | 14.7 ns      | -       |
| **nfx_stringmap**     | ns        | ns          | 52.3 ns     | 10.8 ns      | -       |
| **nfx_stringset**     | ns        | ns          | 52.1 ns     | 11.2 ns      | -       |
| **nfx_chd_hashmap**   | ns        | ns          | 12.0 ns     | 14.6 ns      | -       |
| **Dict**              | -         | -           | -           | -            | 39.9 ns |
| **FrozenDict**        | -         | -           | -           | -            | 16.3 ns |
| **sdk_gmod**          | ns        | ns          | 12.7 ns     | 14.6 ns      | 16.5 ns |

#### Gmod path parse

| Benchmark                          | linux GCC | linux clang | Windows GCC | Windows MSVC | C# .NET |
| ---------------------------------- | --------- | ----------- | ----------- | ------------ | ------- |
| **TryParse**                       | μs        | μs          | 10.6 μs     | 19.2 μs      | 3.36 μs |
| **TryParseFullPath**               | μs        | μs          | 5.79 μs     | 11.8 μs      | 581 ns  |
| **TryParseIndividualized**         | μs        | μs          | 8.75 μs     | 13.7 μs      | 1.44 μs |
| **TryParseFullPathIndividualized** | μs        | μs          | 6.63 μs     | 10.1 μs      | 744 ns  |

#### Gmod traversal

| Benchmark         | linux GCC | linux clang | Windows GCC | Windows MSVC | C# .NET |
| ----------------- | --------- | ----------- | ----------- | ------------ | ------- |
| **FullTraversal** | ms        | ms          | 188 ms      | 184 ms       | 159 ms  |

#### Gmod versioning convert path

| Benchmark       | linux GCC | linux clang | Windows GCC | Windows MSVC | C# .NET |
| --------------- | --------- | ----------- | ----------- | ------------ | ------- |
| **ConvertPath** | μs        | μs          | 9.50μs      | 13.2 μs      | 1.54 μs |

---

_Benchmarked on September 25, 2025_
