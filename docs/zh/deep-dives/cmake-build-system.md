# 现代 CMake 构建

## 为什么构建系统影响性能

同样的 C++ 源码，不同的构建配置可以产生数量级的性能差异。构建系统控制着三个关键维度：

**编译优化标志。** `-O3` 启用激进优化（循环展开、向量化、内联）；`-march=native` 允许编译器使用当前 CPU 的全部指令集（AVX2、FMA）；`-ffast-math` 放松浮点语义以换取向量化机会。缺少这些标志的 Release 构建可能只有峰值性能的 20-30%。

**链接时优化（LTO）。** 跨翻译单元内联、死代码消除、全局常量传播。没有 LTO，编译器只能在单个 `.cpp` 文件内优化。（注意：本仓库当前尚未集成 LTO，扩展方式见文末 preset 工作流一节。）

**调试信息与性能分析。** `-g` 生成调试符号，`RelWithDebInfo` 配置在保持 `-O2` 优化的同时保留符号信息，使 perf/VTune 等工具能将热点映射回源码行。

本仓库的构建系统（CMake 3.20+）将这些决策编码为可复现的 preset，而非散落在命令行参数中。

---

## 目标驱动 vs 目录驱动

传统 CMake 使用目录级命令（`include_directories`、`add_definitions`、`link_libraries`），所有目标隐式继承设置。现代 CMake 使用目标级命令，属性随依赖关系精确传播。

`examples/01-cmake-modern/best-practices/CMakeLists.txt` 展示了核心模式：

```cmake
# 目标级 include 路径，带可见性控制
target_include_directories(my_library
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
```

**PUBLIC vs PRIVATE vs INTERFACE：**
- `PUBLIC`：本目标使用，依赖者也继承（如库的头文件路径）
- `PRIVATE`：仅本目标使用（如实现文件的内部路径）
- `INTERFACE`：仅依赖者使用（如 header-only 库）

```cmake
# Header-only 库：INTERFACE 即可
add_library(header_only_lib INTERFACE)
target_include_directories(header_only_lib
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_compile_features(header_only_lib INTERFACE cxx_std_20)
```

**为什么这影响性能？** 目标驱动确保每个编译单元只看到必要的 include 路径（减少预处理器搜索时间），且优化标志精确应用——benchmark 目标可以开启 `-ffast-math` 而不影响正确性敏感的库代码。

本仓库的 `cmake/CompilerOptions.cmake` 将此模式封装为函数：

```cmake
function(hpc_set_compiler_options target)
    # 警告：所有目标统一
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic ...)

    # 优化：按配置区分
    _hpc_add_config_options(${target}
        RELEASE "-O3;-march=native;-mtune=native"
        RELWITHDEBINFO "-O2;-g;-march=native"
    )
endfunction()
```

---

## CMake Presets

`CMakePresets.json` 将构建配置从命令行参数提升为版本控制的声明式文件。本仓库定义了以下 preset：

| Preset | 用途 | 关键设置 |
|---|---|---|
| `debug` | 开发调试 | `-O0 -g`，启用测试和 benchmark |
| `release` | 性能验证 | `-O3 -march=native`，启用测试和 benchmark |
| `relwithdebinfo` | 性能分析 | `-O2 -g`，perf 可映射到源码 |
| `asan` | 内存错误检测 | ASan + Debug，关闭 benchmark |
| `tsan` | 数据竞争检测 | TSan + Clang + Debug，关闭 benchmark |
| `ubsan` | 未定义行为检测 | UBSan + Debug，关闭 benchmark |
| `coverage` | 代码覆盖率 | gcov flags + Debug |

所有 preset 继承自隐藏的 `base`：

```json
{
  "name": "base",
  "hidden": true,
  "generator": "Ninja",
  "binaryDir": "${sourceDir}/build/${presetName}",
  "cacheVariables": {
    "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
  }
}
```

**为什么 preset 优于命令行参数：**

1. **可复现性。** `cmake --preset=release` 在任何机器上产生相同配置。命令行参数依赖 shell history 或 Makefile wrapper。
2. **IDE 集成。** VS Code、CLion、Visual Studio 自动发现 preset，无需手动配置。
3. **CI 一致性。** CI 脚本和本地开发使用完全相同的构建路径。
4. **组合爆炸管理。** 7 个 preset × 3 个阶段（configure/build/test）= 21 种组合，全部声明在一个文件中。

每个 preset 还有对应的 `buildPresets` 和 `testPresets`，test preset 中设置了 sanitizer 运行时选项：

```json
{
  "name": "tsan",
  "configurePreset": "tsan",
  "environment": {
    "TSAN_OPTIONS": "halt_on_error=1"
  }
}
```

---

## 依赖管理

`cmake/Dependencies.cmake` 使用 FetchContent 管理三个外部依赖：

```cmake
function(hpc_fetch_google_benchmark)
    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG        v1.8.3
        GIT_SHALLOW    TRUE
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(benchmark)
endfunction()
```

设计决策：

- **`GIT_SHALLOW TRUE`**：只拉取指定 tag 的单个 commit，减少克隆时间。
- **关闭依赖自身的测试和安装**：避免构建无关目标，缩短构建时间。
- **`FETCHCONTENT_UPDATES_DISCONNECTED ON`**：首次拉取后不再检查远端更新，避免 CI 中不必要的网络请求。
- **封装为函数**：按需调用，`HPC_BUILD_TESTS=OFF` 时不拉取 GTest 和 RapidCheck。

与 git submodule 相比，FetchContent 的优势：依赖版本锁定在 CMake 文件中（单一事实来源），无需 `.gitmodules` 和手动 `git submodule update --init`。

---

## Sanitizer 集成

`cmake/Sanitizers.cmake` 提供目标级 sanitizer 启用：

```cmake
function(hpc_enable_sanitizers target)
    if(ENABLE_ASAN)
        list(APPEND SANITIZER_FLAGS -fsanitize=address -fno-omit-frame-pointer)
        list(APPEND SANITIZER_LINK_FLAGS -fsanitize=address)
    endif()
    if(ENABLE_TSAN)
        list(APPEND SANITIZER_FLAGS -fsanitize=thread)
        list(APPEND SANITIZER_LINK_FLAGS -fsanitize=thread)
    endif()
    if(ENABLE_UBSAN)
        list(APPEND SANITIZER_FLAGS -fsanitize=undefined)
        list(APPEND SANITIZER_LINK_FLAGS -fsanitize=undefined)
    endif()
    # ...
    target_compile_options(${target} PRIVATE ${SANITIZER_FLAGS})
    target_link_options(${target} PRIVATE ${SANITIZER_LINK_FLAGS})
endfunction()
```

关键约束：

- **ASan 与 TSan 互斥。** 同时启用会链接失败。CMake 中用 `FATAL_ERROR` 提前拦截。
- **`-fno-omit-frame-pointer`**：保留帧指针，使 sanitizer 报告的调用栈完整。
- **仅 GCC/Clang 支持。** MSVC 有独立的 `/fsanitize=address`，此处不处理。
- **Sanitizer 预设关闭 benchmark。** Sanitizer 引入 2-10x 运行时开销，benchmark 数据无意义。

`cmake/ExampleTemplate.cmake` 中的 `hpc_add_example()` 自动为每个目标调用 `hpc_enable_sanitizers()`，无需手动配置。

---

## 动手验证

```bash
# 1. Release 构建（性能验证用）
cmake --preset=release
cmake --build build/release

# 2. 运行所有测试
ctest --preset=release

# 3. 运行 benchmark
./build/release/examples/03-modern-cpp/vector_reserve_bench

# 4. ASan 检测内存错误
cmake --preset=asan
cmake --build build/asan
ctest --preset=asan

# 5. TSan 检测数据竞争（需要 Clang）
cmake --preset=tsan
cmake --build build/tsan
ctest --preset=tsan

# 6. 查看编译命令（IDE 集成 / 调试用）
cat build/release/compile_commands.json | head -20
```

> **关于 LTO：** 本仓库目前尚未集成链接时优化（没有 `HPC_ENABLE_LTO` 之类的开关）。
> 如需启用，可在 `cmake/CompilerOptions.cmake` 中为 Release 配置添加 `-flto=auto`
> （GCC/Clang）或设置 `CMAKE_INTERPROCEDURAL_OPTIMIZATION`，然后用
> `cmake --build build/release --verbose 2>&1 | grep -i flto` 验证是否生效。

构建系统本身也是"可验证的性能工程"的一部分：相同的源码 + 相同的 preset = 相同的二进制行为，任何性能回归都可以定位到具体的代码变更而非构建配置漂移。
