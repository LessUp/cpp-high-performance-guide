# 07 - I/O 性能

文件 I/O 的性能模型与内存完全不同：瓶颈在**系统调用次数**、**页缓存**
和**数据拷贝次数**，而不是 CPU 指令。本模块用最直接的对照实验展示这三点。
仅 Linux/POSIX（非 Linux 平台配置时自动跳过）。

## 内容

| 示例 | 主题 |
|------|------|
| `src/mmap_vs_read.cpp` | 整文件读取三条路径对比：`read()` 循环 vs `pread()` 循环 vs `mmap` 顺序触摸 |
| `src/buffered_write.cpp` | 写路径系统调用开销：逐字节写 vs 4 KiB 缓冲 vs 单次大块写 |
| `bench/io_bench.cpp` | 受控基准：三种读路径随文件尺寸的吞吐（Google Benchmark） |

规范库头文件：[`include/hpc/io_utils.hpp`](../../include/hpc/io_utils.hpp)
（RAII 文件描述符、RAII mmap 视图、`pread` 整读、临时文件构造）。

## 构建与运行

```bash
cmake --preset=release
cmake --build build/release

./build/release/examples/07-io-performance/mmap_vs_read 64
./build/release/examples/07-io-performance/buffered_write
./build/release/examples/07-io-performance/io_bench
```

## 诚实说明

- 所有被测文件都是**刚写入页缓存**的，测的是缓存驻留读写的系统调用与
  拷贝开销，不是磁盘带宽。测设备带宽需要 `O_DIRECT` 绕过页缓存并控制
  预读，那是另一个话题。
- `mmap` vs `read` 的顺序全扫描差距通常不大（同样的页缓存 I/O）；
  `mmap` 的真正优势在随机访问、共享映射与省一次内核→用户拷贝，别指望
  顺序读出现数量级差异。
- 逐字节写会发起上百万次系统调用，耗时可达秒级，属于刻意展示的反例。
