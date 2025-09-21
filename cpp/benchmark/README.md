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

| Platform    | Benchmark Framework     | C++ Compiler         | Vista SDK c++ Version | NFX Core Version |
| ----------- | ----------------------- | -------------------- | --------------------- | ---------------- |
| **Windows** | Google Benchmark v1.9.4 | VC++ 19.44.35214-x64 | v0.0.45               | v0.0.8           |

---

# Performance Results

## Core Components

### Codebooks (C++)

| Benchmark               | Performance |
| ----------------------- | ----------- |
| **std_unordered_map**   | 2.09 ns     |
| **std_map**             | 10.9 ns     |
| **sdk_codebooks**       | 0.816 ns    |
| **sdk_lookup_operator** | 0.809 ns    |

### Codebooks (C#)

| Benchmark      | Performance |
| -------------- | ----------- |
| **Dict**       | 4.77 ns     |
| **FrozenDict** | 4.95 ns     |
| **Codebooks**  | 0.622 ns    |

### Gmod

#### Gmod load (C++)

| Benchmark           | Performance |
| ------------------- | ----------- |
| **gmodCacheAccess** | 47.5 ns     |

#### Gmod load (C#)

| Benchmark | Performance |
| --------- | ----------- |
| **Load**  | 30.9 ms     |

#### Gmod lookup (C++)

| Benchmark             | Performance |
| --------------------- | ----------- |
| **std_unordered_map** | 14.2 ns     |
| **std_map**           | 272 ns      |
| **std_unordered_set** | 13.1 ns     |
| **std_set**           | 273 ns      |
| **std_vector**        | 16483 ns    |
| **nfx_hashmap**       | 15.3 ns     |
| **nfx_stringmap**     | 11.1 ns     |
| **nfx_stringset**     | 11.6 ns     |
| **nfx_chd_hashmap**   | 15.3 ns     |
| **sdk_gmod**          | 15.1 ns     |

#### Gmod lookup (C#)

| Benchmark      | Performance |
| -------------- | ----------- |
| **Dict**       | 39.9 ns     |
| **Gmod**       | 16.5 ns     |
| **FrozenDict** | 16.3 ns     |

#### Gmod path parse (C++)

| Benchmark                          | Performance |
| ---------------------------------- | ----------- |
| **TryParse**                       | 18.2 μs     |
| **TryParseFullPath**               | 10.6 μs     |
| **TryParseIndividualized**         | 13.5 μs     |
| **TryParseFullPathIndividualized** | 9.83 μs     |

#### Gmod path parse (C#)

| Benchmark                          | Performance |
| ---------------------------------- | ----------- |
| **TryParse**                       | 3.36 μs     |
| **TryParseFullPath**               | 581 ns      |
| **TryParseIndividualized**         | 1.44 μs     |
| **TryParseFullPathIndividualized** | 744 ns      |

#### Gmod traversal (C++)

| Benchmark         | Performance |
| ----------------- | ----------- |
| **FullTraversal** | 184 ms      |

#### Gmod traversal (C#)

| Benchmark         | Performance |
| ----------------- | ----------- |
| **FullTraversal** | 159 ms      |

#### Gmod versioning convert path (C++)

| Benchmark       | Performance |
| --------------- | ----------- |
| **ConvertPath** | 14.2 μs     |

#### Gmod versioning convert path (C#)

| Benchmark       | Performance |
| --------------- | ----------- |
| **ConvertPath** | 1.54 μs     |

---

_Benchmarked on September 21, 2025_
