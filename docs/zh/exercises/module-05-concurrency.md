# 并发练习

实践原子操作、无锁编程和 OpenMP 并行化。

---

## 练习 1：调试数据竞争

### 目标

使用 ThreadSanitizer 查找并修复数据竞争。

### 准备

```cpp
// 文件：race_exercise.cpp
#include <thread>
#include <vector>
#include <iostream>

int counter = 0;  // BUG：不是原子的！

void increment(int times) {
    for (int i = 0; i < times; ++i) {
        counter++;  // 数据竞争！
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
    
    std::cout << "期望值：" << threads * increments << "\n";
    std::cout << "实际值：" << counter << "\n";
    return 0;
}
```

### 任务

1. **编译并运行**
   ```bash
   g++ -O2 -pthread race_exercise.cpp -o race_exercise
   ./race_exercise
   ```
   注意结果是错误的！

2. **使用 ThreadSanitizer 检测**
   ```bash
   g++ -O2 -pthread -fsanitize=thread race_exercise.cpp -o race_exercise_tsan
   ./race_exercise_tsan
   ```

3. **修复数据竞争**

4. **验证修复**

### 问题

<details>
<summary>Q：有哪三种方法可以修复这个问题？</summary>

1. 使用带有适当内存序的 `std::atomic<int>`
2. 使用互斥锁（`std::mutex`）
3. 使用线程局部计数器，最后合并结果
</details>

---

## 练习 2：内存序

### 目标

理解不同内存序之间的性能差异。

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

1. **对两个版本进行基准测试**

2. **比较性能**

3. **解释差异**

### 问题

<details>
<summary>Q：什么时候应该使用每种内存序？</summary>

- `relaxed`：计数器、统计信息，精确顺序不重要
- `acquire/release`：生产者-消费者模式
- `seq_cst`：默认，不确定或需要全序时使用
</details>

---

## 练习 3：实现线程安全计数器

### 目标

实现带有缓存行填充的线程安全计数器。

### 准备

```cpp
// TODO：实现这个
class ThreadSafeCounter {
public:
    void increment();
    int get() const;
private:
    // 你的成员变量
};
```

### 要求

1. 支持并发递增
2. 防止多个计数器之间的伪共享
3. 支持快速读取

### 任务

1. **实现该类**

2. **使用多线程测试**

3. **验证没有伪共享**

### 提示

<details>
<summary>点击查看提示</summary>

使用 `alignas(64)` 防止伪共享：

```cpp
struct alignas(64) CacheLineAligned {
    std::atomic<int> value{0};
};
```
</details>

---

## 练习 4：SPSC 队列验证

### 目标

验证无锁队列的正确性。

### 准备

使用 `examples/05-concurrency/src/lock_free_queue.cpp` 中的队列。

### 任务

1. **编写测试**：
   - 生产者推送 N 个元素
   - 消费者弹出 N 个元素
   - 验证所有元素都被接收

2. **在负载下测试**
   ```bash
   # 使用 ThreadSanitizer 运行
   cmake --preset=tsan
   cmake --build build/tsan
   ./build/tsan/your_test
   ```

3. **验证没有数据竞争**

### 问题

<details>
<summary>Q：队列必须维护哪些不变量？</summary>

1. 没有元素丢失
2. 没有元素重复
3. FIFO 顺序被保持
4. 队列永远不会进入无效状态（内存安全）
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
    
    std::cout << "总和：" << sum << "\n";
}
```

### 任务

1. **使用不同线程数测量**
   ```bash
   export OMP_NUM_THREADS=1
   time ./parallel_sum
   
   export OMP_NUM_THREADS=2
   time ./parallel_sum
   
   export OMP_NUM_THREADS=4
   time ./parallel_sum
   ```

2. **计算效率**
   - 效率 = 实际加速 / 线程数 × 100%
   - 理想：100% 效率

3. **识别瓶颈**
   - 内存带宽？
   - 负载不均衡？
   - 伪共享？

### 问题

<details>
<summary>Q：为什么效率随线程数增加而下降？</summary>

阿姆达尔定律：串行部分无法并行化
- 内存带宽饱和
- 同步开销
- 缓存竞争
- 多插槽系统的 NUMA 效应
</details>

---

## 挑战：并行化真实算法

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

1. **使用 OpenMP 并行化**

2. **优化缓存使用**（循环分块）

3. **结合 SIMD**

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

完整解答请参阅 [solutions.md](solutions.md)。

---

## 下一步

- 复习 [并发模块](https://github.com/LessUp/cpp-high-performance-guide/tree/master/examples/05-concurrency)
- 阅读 [内存工具 API](../reference/api/memory-utils.md) 了解缓存行填充
- 探索 [性能分析指南](../guides/profiling-guide.md) 分析多线程性能
