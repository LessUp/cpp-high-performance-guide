# Concurrency 练习

练习 atomic 操作、lock-free 编程与 OpenMP 并行化。

---

## 练习 1：调试 data race

### 目标

使用 ThreadSanitizer 发现并修复 data race。

### 准备

```cpp
// File: race_exercise.cpp
#include <thread>
#include <vector>
#include <iostream>

int counter = 0;  // BUG: Not atomic!

void increment(int times) {
    for (int i = 0; i < times; ++i) {
        counter++;  // Data race!
    }
}

int main() {
    const int threads = 4;
    const int increments = 1000000;
    
    std::vector<std::thread> t;
    for (int i = 0; i < threads; ++i) {
        t.emplace_back(increment, increments);
    }
    
    for (auto& thread : t) {
        thread.join();
    }
    
    std::cout << "Expected: " << threads * increments << "\n";
    std::cout << "Actual: " << counter << "\n";
    return 0;
}
```

### 任务

1. **编译并运行**
   ```bash
   g++ -O2 -pthread race_exercise.cpp -o race_exercise
   ./race_exercise
   ```
   注意结果是不正确的！

2. **用 ThreadSanitizer 检测**
   ```bash
   g++ -O2 -pthread -fsanitize=thread race_exercise.cpp -o race_exercise_tsan
   ./race_exercise_tsan
   ```

3. **修复 data race**

4. **验证修复结果**

### 问题

<details>
<summary>Q：有哪三种修复方法？</summary>

1. 使用 `std::atomic<int>` 配合适当的 memory ordering
2. 使用 mutex（`std::mutex`）
3. 使用线程局部计数器，最后再汇总
</details>

---

## 练习 2：memory ordering

### 目标

理解不同 memory ordering 之间的性能差异。

### 准备

```cpp
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

std::atomic<int> counter{0};

void increment_relaxed(int times) {
    for (int i = 0; i < times; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

void increment_seq_cst(int times) {
    for (int i = 0; i < times; ++i) {
        counter.fetch_add(1, std::memory_order_seq_cst);
    }
}
```

### 任务

1. **对两个版本进行 benchmark**

2. **比较性能**

3. **解释差异**

### 问题

<details>
<summary>Q：各 ordering 分别应在何时使用？</summary>

- `relaxed`：计数器、统计场景，精确顺序无关紧要
- `acquire/release`：生产者-消费者模式
- `seq_cst`：默认选项，不确定或需要全局顺序时使用
</details>

---

## 练习 3：实现线程安全计数器

### 目标

实现一个带 cache line padding 的线程安全计数器。

### 准备

```cpp
// TODO: Implement this
class ThreadSafeCounter {
public:
    void increment();
    int get() const;
private:
    // Your members here
};
```

### 要求

1. 支持并发自增
2. 防止多个计数器之间的 false sharing
3. 支持快速读取

### 任务

1. **实现该类**

2. **多线程测试**

3. **验证不存在 false sharing**

### 提示

<details>
<summary>点击查看提示</summary>

使用 `alignas(64)` 防止 false sharing：

```cpp
struct alignas(64) CacheLineAligned {
    std::atomic<int> value{0};
};
```
</details>

---

## 练习 4：SPSC 队列验证

### 目标

验证 lock-free 队列的正确性。

### 准备

使用 `examples/05-concurrency/src/lock_free_queue.cpp` 中的队列。

### 任务

1. **编写测试**，要求：
   - 生产者推入 N 个元素
   - 消费者弹出 N 个元素
   - 验证所有元素均已收到

2. **在负载下测试**
   ```bash
   # Run with ThreadSanitizer
   cmake --preset=tsan
   cmake --build build/tsan
   ./build/tsan/your_test
   ```

3. **验证不存在 data race**

### 问题

<details>
<summary>Q：队列必须维护哪些不变式？</summary>

1. 没有元素丢失
2. 没有元素重复
3. FIFO 顺序得到保持
4. 队列永不进入非法状态（内存安全）
</details>

---

## 练习 5：OpenMP 扩展性

### 目标

使用 OpenMP 测量线程扩展效率。

### 准备

```cpp
#include <omp.h>
#include <vector>
#include <iostream>

void parallel_sum(const std::vector<int>& data) {
    long long sum = 0;
    
    #pragma omp parallel for reduction(+:sum)
    for (size_t i = 0; i < data.size(); ++i) {
        sum += data[i];
    }
    
    std::cout << "Sum: " << sum << "\n";
}
```

### 任务

1. **用不同线程数测量**
   ```bash
   export OMP_NUM_THREADS=1
   time ./parallel_sum
   
   export OMP_NUM_THREADS=2
   time ./parallel_sum
   
   export OMP_NUM_THREADS=4
   time ./parallel_sum
   ```

2. **计算效率**
   - 效率 = 实际加速比 / (线程数 × 100%)
   - 理想值：100% 效率

3. **识别瓶颈**
   - 内存带宽？
   - 负载不均？
   - false sharing？

### 问题

<details>
<summary>Q：为什么线程越多效率越低？</summary>

Amdahl 定律：串行部分无法并行化
- 内存带宽饱和
- 同步开销
- cache 争用
- 多插槽系统的 NUMA 效应
</details>

---

## 挑战：并行化一个真实算法

### 目标

使用 OpenMP 并行化矩阵乘法。

### 准备

```cpp
void matrix_mult_scalar(const float* A, const float* B, float* C,
                        size_t N) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            float sum = 0.0f;
            for (size_t k = 0; k < N; ++k) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}
```

### 任务

1. **用 OpenMP 并行化**

2. **优化 cache 使用**（循环分块）

3. **与 SIMD 结合**

4. **测量扩展效率**

### 提示

<details>
<summary>点击查看提示</summary>

```cpp
void matrix_mult_openmp(const float* A, const float* B, float* C,
                        size_t N) {
    #pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            float sum = 0.0f;
            #pragma omp simd reduction(+:sum)
            for (size_t k = 0; k < N; ++k) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}
```
</details>

---

## 解答

完整解答见 [solutions.md](solutions.md)。

---

## 后续步骤

- 复习 [Concurrency 模块](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/05-concurrency)
- 阅读 [Memory Utilities API](../reference/api/memory-utils.md) 了解 cache line padding
- 探索 [性能分析指南](../guides/profiling-guide.md) 以分析多线程性能
