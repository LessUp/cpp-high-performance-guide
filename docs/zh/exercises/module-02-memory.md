# Memory 优化练习

练习 cache 优化、数据布局与内存对齐技术。

---

## 练习 1：AOS 到 SOA 的转换

### 目标

将 Array of Structures 布局转换为 Structure of Arrays，并测量性能差异。

### 准备

`examples/02-memory-cache/src/aos_vs_soa.cpp` 中的粒子系统示例演示了这一对比。你将在此基础上进行修改。

### 任务

1. **研读现有代码**
   ```bash
   cat examples/02-memory-cache/src/aos_vs_soa.cpp
   ```

2. **为粒子系统添加一个新字段**（例如 `mass`）

3. **同时更新 AOS 和 SOA 结构**

4. **对改动进行 benchmark**
   ```bash
   ./build/release/examples/02-memory-cache/bench/aos_soa_bench
   ```

### 问题

<details>
<summary>Q1：添加字段对 AOS 与 SOA 的性能有何影响？</summary>

展开前先思考一下……

向 AOS 添加字段会增加同字段访问之间的 stride，使按字段操作时的 cache 利用率变差。SOA 不受影响，因为每个字段数组是独立的。
</details>

<details>
<summary>Q2：什么时候 AOS 比 SOA 更好？</summary>

当你同时访问所有字段时（例如依次访问 `particle.x`、`particle.y`、`particle.z`），AOS 更优。这在面向对象设计中或一次性更新所有粒子属性时很常见。
</details>

---

## 练习 2：修复 false sharing

### 目标

识别并修复多线程计数器中的 false sharing。

### 准备

```cpp
// File: false_sharing_exercise.cpp
#include <vector>
#include <thread>
#include <atomic>
#include <iostream>

void increment_counters(std::atomic<int>* counters, int thread_id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        counters[thread_id].fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    const int num_threads = 4;
    const int iterations = 10000000;
    
    // TODO: This has false sharing!
    std::atomic<int> counters[num_threads];
    
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        counters[i] = 0;
        threads.emplace_back(increment_counters, counters, i, iterations);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```

### 任务

1. **编译并运行**上面的代码
   ```bash
   g++ -O3 -pthread false_sharing_exercise.cpp -o false_sharing_exercise
   time ./false_sharing_exercise
   ```

2. **使用 `alignas(64)` 或 padding 修复 false sharing**

3. **比较性能**

### 提示

<details>
<summary>点击查看提示</summary>

问题在于 `std::atomic<int>` 通常为 4 字节，4 个计数器会挤在同一条 cache line 中。使用对齐确保每个计数器独占一条 cache line：

```cpp
struct alignas(64) AlignedCounter {
    std::atomic<int> value{0};
};

AlignedCounter counters[num_threads];
```
</details>

### 问题

<details>
<summary>Q：你获得了多少加速比？</summary>

典型加速比为 2-10 倍，取决于 CPU 和线程数。线程越多 = 争用越激烈 = 修复 false sharing 带来的加速越显著。
</details>

---

## 练习 3：prefetch 距离调优

### 目标

为不同的访问模式找到最优的 prefetch 距离。

### 准备

使用 prefetch 示例：
```bash
cat examples/02-memory-cache/src/prefetch.cpp
```

### 任务

1. **以默认设置运行 benchmark**
   ```bash
   ./build/release/examples/02-memory-cache/bench/prefetch_bench
   ```

2. **修改 prefetch 距离**（尝试 8、16、32、64、128）

3. **绘制结果**

### 问题

<details>
<summary>Q：最优的 prefetch 距离是多少？</summary>

最优距离取决于：
- 内存延迟
- 循环迭代时间
- cache 大小

经验法则：`distance = memory_latency / iteration_time`。对于典型系统，提前 32-64 个元素效果较好。
</details>

---

## 练习 4：面向 SIMD 的内存对齐

### 目标

测量内存对齐对 SIMD 性能的影响。

### 任务

1. **创建对齐与未对齐的数组**
   ```cpp
   // Unaligned
   float* unaligned = new float[1024];
   
   // Aligned
   alignas(64) float aligned[1024];
   // or
   float* aligned = static_cast<float*>(
       std::aligned_alloc(64, 1024 * sizeof(float))
   );
   ```

2. **对两者执行 SIMD 操作**

3. **比较性能**

### 问题

<details>
<summary>Q：对齐在什么时候最重要？</summary>

对齐在以下情况最为重要：
- 要求对齐的 SIMD load/store 指令（例如 `_mm_load_ps` 与 `_mm_loadu_ps`）
- AVX-512，其对齐要求更严格
- 大数据集，cache 效率至关重要
</details>

---

## 挑战：剖析一个神秘程序

### 目标

使用性能分析工具定位并修复性能瓶颈。

### 准备

练习目录中提供了一个带有隐藏瓶颈的"神秘"程序。

### 任务

1. **对程序进行性能分析**
   ```bash
   perf record -g ./mystery_program
   perf report
   ```

2. **生成 FlameGraph**
   ```bash
   perf script | stackcollapse-perf.pl | flamegraph.pl > mystery_flame.svg
   ```

3. **定位瓶颈**

4. **修复它**

5. **测量改进幅度**

### 提示

<details>
<summary>点击查看提示</summary>

留意以下迹象：
- FlameGraph 中的宽区段（热点函数）
- 高 cache miss 率
- 耗时异常长的函数
</details>

---

## 解答

完整解答见 [solutions.md](solutions.md)。

---

## 后续步骤

- 继续 [SIMD 练习](module-04-simd.md)
- 阅读 [Memory Utilities API](../reference/api/memory-utils.md)
- 探索 [优化决策树](../guides/optimization-decision-tree.md)
