# 内存优化练习

实践缓存优化、数据布局和内存对齐技术。

---

## 练习 1：AOS 到 SOA 转换

### 目标

将数组结构（AOS）布局转换为结构数组（SOA），并测量性能差异。

### 准备

`examples/02-memory-cache/src/aos_vs_soa.cpp` 中的粒子系统示例演示了这一点。你将修改它。

### 任务

1. **研究现有代码**
   ```bash
   cat examples/02-memory-cache/src/aos_vs_soa.cpp
   ```

2. **添加新字段** 到粒子系统（如 `mass`）

3. **同时更新 AOS 和 SOA 结构**

4. **对修改进行基准测试**
   ```bash
   ./build/release/examples/02-memory-cache/bench/aos_soa_bench
   ```

### 问题

<details>
<summary>Q1：添加字段如何影响 AOS 与 SOA 的性能？</summary>

先思考再展开...

添加字段到 AOS 会增加同字段访问之间的步长，使按字段操作的缓存利用率变差。SOA 不受影响，因为每个字段数组是独立的。
</details>

<details>
<summary>Q2：什么时候 AOS 比 SOA 更好？</summary>

当你一起访问所有字段时，AOS 更好（如连续访问 `particle.x`、`particle.y`、`particle.z`）。这在面向对象设计中很常见，或者在一次性更新所有粒子属性时。
</details>

---

## 练习 2：修复伪共享

### 目标

识别并修复多线程计数器中的伪共享问题。

### 准备

```cpp
// 文件：false_sharing_exercise.cpp
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
    
    // TODO：这里有伪共享问题！
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

1. **编译并运行** 上面的代码
   ```bash
   g++ -O3 -pthread false_sharing_exercise.cpp -o false_sharing_exercise
   time ./false_sharing_exercise
   ```

2. **使用 `alignas(64)` 或填充修复伪共享**

3. **比较性能**

### 提示

<details>
<summary>点击查看提示</summary>

问题在于 `std::atomic<int>` 通常是 4 字节，4 个计数器可以放入单个缓存行。使用对齐确保每个计数器在自己的缓存行上：

```cpp
struct alignas(64) AlignedCounter {
    std::atomic<int> value{0};
};

AlignedCounter counters[num_threads];
```
</details>

### 问题

<details>
<summary>Q：你获得了多少加速？</summary>

典型的加速是 2-10 倍，取决于 CPU 和线程数。线程越多 = 竞争越多 = 修复伪共享的加速越大。
</details>

---

## 练习 3：预取距离调优

### 目标

找到不同访问模式的最佳预取距离。

### 准备

使用预取示例：
```bash
cat examples/02-memory-cache/src/prefetch.cpp
```

### 任务

1. **使用默认设置运行基准测试**
   ```bash
   ./build/release/examples/02-memory-cache/bench/prefetch_bench
   ```

2. **修改预取距离**（尝试 8、16、32、64、128）

3. **绘制结果图表**

### 问题

<details>
<summary>Q：最佳预取距离是多少？</summary>

最佳距离取决于：
- 内存延迟
- 循环迭代时间
- 缓存大小

经验法则：`distance = memory_latency / iteration_time`。对于典型系统，提前 32-64 个元素效果较好。
</details>

---

## 练习 4：SIMD 的内存对齐

### 目标

测量内存对齐对 SIMD 性能的影响。

### 任务

1. **创建对齐和未对齐的数组**
   ```cpp
   // 未对齐
   float* unaligned = new float[1024];
   
   // 对齐
   alignas(64) float aligned[1024];
   // 或
   float* aligned = static_cast<float*>(
       std::aligned_alloc(64, 1024 * sizeof(float))
   );
   ```

2. **在两者上运行 SIMD 操作**

3. **比较性能**

### 问题

<details>
<summary>Q：对齐在什么时候最重要？</summary>

对齐在以下情况最重要：
- 需要对齐的 SIMD 加载/存储指令（如 `_mm_load_ps` vs `_mm_loadu_ps`）
- AVX-512 有更严格的对齐要求
- 大数据集，缓存效率至关重要
</details>

---

## 挑战：分析神秘程序

### 目标

使用性能分析工具识别并修复性能瓶颈。

### 准备

练习目录中提供了一个有隐藏瓶颈的"神秘"程序。

### 任务

1. **分析程序**
   ```bash
   perf record -g ./mystery_program
   perf report
   ```

2. **生成火焰图**
   ```bash
   perf script | stackcollapse-perf.pl | flamegraph.pl > mystery_flame.svg
   ```

3. **识别瓶颈**

4. **修复它**

5. **测量改进**

### 提示

<details>
<summary>点击查看提示</summary>

查找：
- 火焰图中的宽部分（热点函数）
- 高缓存未命中率
- 意外耗时的函数
</details>

---

## 解答

完整解答请参阅 [solutions.md](solutions.md)。

---

## 下一步

- 继续学习 [SIMD 练习](module-04-simd.md)
- 阅读 [内存工具 API](../reference/api/memory-utils.md)
- 探索 [优化决策树](../guides/optimization-decision-tree.md)
