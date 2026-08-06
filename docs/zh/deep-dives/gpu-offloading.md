# GPU 卸载加速概览

> 本篇是路线图文档：GPU 工具链（CUDA/SYCL/HIP）重、硬件依赖强，本仓库不提供 GPU 代码示例，这里回答的是"什么时候该上 GPU、选哪条技术路线、怎么和已有的 SIMD/MPI 工作衔接"。

## 什么时候 GPU 才值得

GPU 的优势建立在一个交换上：**用极高的并行度和带宽，换取更贵的控制流与更慢的单线程**。判断标准是三条：

1. **计算密度足够高**：同一份数据要执行大量算术（如矩阵乘、stencil、FFT）。每字节只加一次的流式操作在 GPU 上往往打不过 CPU SIMD + 大页。
2. **批量足够大**：核函数（kernel）要能展开成数千个线程。元素数小于几万时，启动开销（µs 级）会吃掉全部收益。
3. **传输开销可摊销**：CPU↔GPU 走 PCIe/NVLink，PCIe 4.0 x16 实测 ~25 GB/s，比本机内存带宽低一个数量级。**数据必须留在 GPU 上反复使用**，每个算法步骤都来回搬运一定亏。

一句话：GPU 适合"搬进去一次、算很久、搬出来一次"的工作负载。

## 三条技术路线

| 路线 | 抽象层级 | 生态 | 适用 |
|------|----------|------|------|
| CUDA | 线程块/网格显式管理 | 最成熟（cuBLAS/cuDNN/Thrust/CUB） | NVIDIA 专用，性能上限最高 |
| SYCL (oneAPI) | 单源 C++17/20，lambda 即 kernel | 跨厂商（Intel/AMD/NVIDIA 后端） | 想保持 C++ 代码形态、多硬件 |
| HIP | CUDA 的移植方言 | AMD 主场，可回编 NVIDIA | AMD 硬件或双平台 |
| OpenACC/OpenMP offload | pragma 指令式 | 编译器生成 kernel | 遗留 Fortran/C 代码快速卸载 |

对现代 C++ 项目，SYCL 的吸引力在于 kernel 就是 lambda、内存管理有 RAII 风格 accessor；代价是编译链更重、调优手段不如 CUDA 直接。性能关键路径（如 GEMM）通常直接调用厂商库（cuBLAS/oneMKL），自己写的 kernel 只覆盖库不提供的部分。

## 编程模型最小集（以 CUDA 心智模型为例）

```cpp
// 伪代码：向量加法的 kernel 与启动
__global__ void add(const float* a, const float* b, float* c, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;   // 全局线程编号
    if (i < n) c[i] = a[i] + b[i];
}

add<<<(n + 255) / 256, 256>>>(d_a, d_b, d_c, n);    // 网格 × 线程块
```

三个必须理解的概念：

- **线程层次**：线程组成块（block，同块可共享内存/同步），块组成网格（grid）。调度以块为单位，所以块数要远超 SM 数量才能占满硬件。
- **内存层次**：寄存器 → shared memory（块内手动管理，~ns）→ global memory（HBM，百 ns 但带宽 1–3 TB/s）。优化的本质是把访问模式搬进前两层：合并访问（coalescing）、分块（tiling）。
- **流与异步**：拷贝与计算放进不同 stream 才能重叠，`cudaMemcpyAsync` + pinned 内存是 H2D/D2H  overlap 的前提。

## 与本仓库主题的衔接

GPU 不推翻前面学的内容，而是放大它们：

- **内存布局**（[内存布局与缓存](memory-layout.md)）：GPU 的合并访问就是"线程 i 访问第 i 个元素"，AOS 在 GPU 上同样是反模式，SOA 收益比 CPU 更大。
- **SIMD**（[SIMD 向量化](simd-internals.md)）：GPU 的 warp/wavefront（32/64 线程锁步执行）本质是超宽 SIMD，分支发散（warp divergence）就是 SIMT 版的"坏的模式"。
- **MPI**（[MPI 分布式并行](mpi-distributed.md)）：多机多卡 = MPI 管节点间、GPU 管节点内。现代实现支持 GPUDirect RDMA，让网卡直接读写显存，绕过主机内存。
- **测量纪律**：GPU 的基准更容易撒谎——kernel 异步执行，不 `cudaDeviceSynchronize` 计时就是假的；缓存预热、显存复用都要和 CPU 基准一样对待。

## 不引入 GPU 代码的原因

本仓库的原则是**每个示例可在标准 CI 上构建验证**。GPU 工具链（CUDA Toolkit 数 GB、需要设备节点）与这一原则冲突，且 GPU 硬件差异（消费卡/数据中心卡/集成卡）会让"参考数据"失去意义。若未来引入，也应遵循现有模式：可选依赖、configure 期探测、无设备时优雅跳过。
