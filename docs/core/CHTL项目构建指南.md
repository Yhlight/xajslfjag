# CHTL项目构建指南

## 概述

本指南详细介绍了如何在不同平台上构建、测试和部署CHTL项目。CHTL采用现代C++17标准，使用CMake作为构建系统，支持多种编译器和平台。

## 系统要求

### 📋 **基础要求**

#### 编译器要求
- **GCC**: 7.0 或更高版本
- **Clang**: 6.0 或更高版本  
- **MSVC**: Visual Studio 2017 (15.7) 或更高版本
- **C++标准**: C++17 或更高

#### 构建工具
- **CMake**: 3.15 或更高版本
- **Make**: GNU Make 4.0+ 或 Ninja 1.8+
- **Git**: 2.20+ (用于版本控制和子模块)

#### 可选依赖
- **ANTLR4**: 4.13.2 (用于CSS和JavaScript解析)
- **Google Test**: 1.10+ (用于单元测试)
- **Doxygen**: 1.8+ (用于文档生成)
- **Valgrind**: 3.15+ (Linux下内存检查)

### 🖥️ **平台支持**

| 平台 | 架构 | 编译器 | 状态 |
|------|------|--------|------|
| Linux | x86_64 | GCC 7+, Clang 6+ | ✅ 完全支持 |
| Linux | ARM64 | GCC 8+, Clang 8+ | ✅ 完全支持 |
| Windows | x86_64 | MSVC 2017+, MinGW | ✅ 完全支持 |
| macOS | x86_64 | Clang 10+ | ✅ 完全支持 |
| macOS | ARM64 (M1/M2) | Clang 12+ | ✅ 完全支持 |

## 快速开始

### 🚀 **一键构建**

```bash
# 1. 克隆项目
git clone https://github.com/CHTL-Team/CHTL.git
cd CHTL

# 2. 构建Release版本 (推荐)
./scripts/build_release.sh

# 3. 运行测试
cd build_release
make test

# 4. 安装 (可选)
sudo make install
```

### 🐛 **开发者构建**

```bash
# 构建Debug版本
./scripts/build_debug.sh

# 构建完成后的调试
cd build_debug
./debug_with_gdb.sh        # 使用GDB调试
./debug_with_valgrind.sh   # 内存检查 (Linux)
```

## 详细构建流程

### 📁 **项目结构**

```
CHTL/
├── CMakeLists.txt          # 主CMake配置
├── cmake/                  # CMake模块和脚本
│   ├── FindANTLR4.cmake   # ANTLR4查找脚本
│   ├── CompilerFlags.cmake # 编译器标志配置
│   └── Testing.cmake      # 测试配置
├── src/                   # 源代码
├── tests/                 # 测试代码
├── third-party/           # 第三方依赖
├── scripts/               # 构建脚本
└── docs/                  # 文档
```

### 🔧 **CMake配置选项**

```bash
# 基础配置选项
-DCMAKE_BUILD_TYPE=Release          # 构建类型: Debug, Release, RelWithDebInfo
-DCMAKE_CXX_STANDARD=17             # C++标准版本
-DCMAKE_INSTALL_PREFIX=/usr/local   # 安装路径

# CHTL特定选项
-DCHTL_BUILD_TESTS=ON               # 构建测试 (默认: ON)
-DCHTL_BUILD_EXAMPLES=ON            # 构建示例 (默认: ON)
-DCHTL_ENABLE_ANTLR=ON              # 启用ANTLR支持 (默认: AUTO)
-DCHTL_ENABLE_PROFILING=OFF         # 启用性能分析 (默认: OFF)
-DCHTL_STATIC_BUILD=OFF             # 静态链接构建 (默认: OFF)

# 调试和开发选项
-DCHTL_ENABLE_SANITIZERS=OFF        # 启用AddressSanitizer等 (默认: OFF)
-DCHTL_ENABLE_COVERAGE=OFF          # 启用代码覆盖率 (默认: OFF)
-DCHTL_VERBOSE_BUILD=OFF            # 详细构建输出 (默认: OFF)
```

### 🏗️ **手动构建步骤**

#### 1. 环境准备
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake git
sudo apt install libgtest-dev  # 可选: Google Test

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install cmake3 git

# macOS
brew install cmake git
xcode-select --install

# Windows (使用vcpkg)
vcpkg install gtest
```

#### 2. 下载和配置
```bash
# 克隆项目
git clone --recursive https://github.com/CHTL-Team/CHTL.git
cd CHTL

# 创建构建目录
mkdir build && cd build

# 配置构建
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCHTL_BUILD_TESTS=ON \
    -DCHTL_ENABLE_ANTLR=ON
```

#### 3. 编译
```bash
# 并行编译 (使用所有CPU核心)
make -j$(nproc)

# 或者使用Ninja (更快)
ninja

# Windows (Visual Studio)
cmake --build . --config Release --parallel
```

#### 4. 测试
```bash
# 运行所有测试
make test

# 或者使用CTest
ctest --output-on-failure

# 运行特定测试
ctest -R "CHTL.*Parser" --verbose
```

#### 5. 安装
```bash
# 安装到系统
sudo make install

# 或者安装到自定义目录
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/chtl
make install
```

## 高级构建配置

### ⚡ **性能优化构建**

```bash
# 最大性能优化
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native -flto" \
    -DCHTL_STATIC_BUILD=ON \
    -DCHTL_BUILD_TESTS=OFF

# 链接时优化 (LTO)
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
```

### 🐛 **调试和分析构建**

```bash
# Debug构建 + 调试符号
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS_DEBUG="-g3 -O0 -DDEBUG" \
    -DCHTL_ENABLE_SANITIZERS=ON

# 内存检查构建
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=address -fsanitize=undefined" \
    -DCMAKE_LINKER_FLAGS="-fsanitize=address -fsanitize=undefined"

# 代码覆盖率构建
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCHTL_ENABLE_COVERAGE=ON \
    -DCMAKE_CXX_FLAGS="--coverage" \
    -DCMAKE_LINKER_FLAGS="--coverage"
```

### 📊 **性能分析构建**

```bash
# 启用性能分析
cmake .. \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCHTL_ENABLE_PROFILING=ON \
    -DCMAKE_CXX_FLAGS="-pg -fno-omit-frame-pointer"

# 使用perf优化
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O2 -g -fno-omit-frame-pointer"
```

## 平台特定配置

### 🐧 **Linux构建**

```bash
# Ubuntu/Debian完整构建环境
sudo apt install \
    build-essential cmake git \
    libgtest-dev libantlr4-runtime-dev \
    valgrind gdb doxygen

# 使用系统包管理器的ANTLR4
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCHTL_ENABLE_ANTLR=ON \
    -DANTLR4_ROOT=/usr

# 静态链接构建 (便于分发)
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCHTL_STATIC_BUILD=ON \
    -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++"
```

### 🍎 **macOS构建**

```bash
# 使用Homebrew安装依赖
brew install cmake antlr4-cpp-runtime

# Apple Silicon (M1/M2) 构建
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCHTL_ENABLE_ANTLR=ON

# Universal Binary构建 (x86_64 + arm64)
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
```

### 🪟 **Windows构建**

#### Visual Studio
```cmd
# 使用Visual Studio 2019/2022
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release

# 或者使用MSBuild
msbuild CHTL.sln /p:Configuration=Release /p:Platform=x64
```

#### MinGW
```bash
# 使用MinGW-w64
cmake .. \
    -G "MinGW Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_C_COMPILER=gcc

mingw32-make -j$(nproc)
```

#### MSYS2
```bash
# 在MSYS2环境中
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc

cmake .. \
    -G "MSYS Makefiles" \
    -DCMAKE_BUILD_TYPE=Release

make -j$(nproc)
```

## 构建脚本详解

### 📜 **build_release.sh**

```bash
# 功能说明
./scripts/build_release.sh

# 主要步骤:
# 1. 检查依赖 (cmake, make, 编译器)
# 2. 清理旧构建目录
# 3. 配置CMake (Release模式)
# 4. 并行编译
# 5. 运行基础测试
# 6. 生成构建信息文件

# 输出文件:
# - build_release/src/chtl (主程序)
# - CHTL_RELEASE_INFO.txt (构建信息)
```

### 🐛 **build_debug.sh**

```bash
# 功能说明
./scripts/build_debug.sh

# 主要特性:
# 1. Debug符号和调试信息
# 2. 启用AddressSanitizer (可选)
# 3. 生成compile_commands.json
# 4. 创建调试辅助脚本

# 输出文件:
# - build_debug/src/chtl (调试版本)
# - debug_with_gdb.sh (GDB调试脚本)
# - debug_with_valgrind.sh (Valgrind脚本)
# - compile_commands.json (IDE支持)
```

### 🧹 **clean_build.sh**

```bash
# 清理构建文件
./scripts/clean_build.sh --all

# 选项说明:
# --build     : 清理构建目录
# --temp      : 清理临时文件
# --packages  : 清理.cmod/.cjmod包
# --cache     : 清理CMake缓存
# --all       : 清理所有内容
# --dry-run   : 预览清理内容
```

## 测试系统

### 🧪 **测试框架结构**

```
tests/
├── Unit/                   # 单元测试
│   ├── CHTL/              # CHTL编译器测试
│   ├── CHTLJS/            # CHTL JS编译器测试
│   ├── Scanner/           # 扫描器测试
│   └── Util/              # 工具类测试
├── Integration/            # 集成测试
│   ├── CompilerTests/     # 编译器集成测试
│   ├── ModuleTests/       # 模块系统测试
│   └── EndToEnd/          # 端到端测试
├── Performance/            # 性能测试
│   ├── CompileTime/       # 编译时间测试
│   ├── Memory/            # 内存使用测试
│   └── Throughput/        # 吞吐量测试
└── Regression/             # 回归测试
    ├── BugFixes/          # 已修复Bug测试
    └── FeatureTests/      # 功能回归测试
```

### 🎯 **运行测试**

```bash
# 运行所有测试
make test

# 运行特定类别测试
ctest -L Unit                # 单元测试
ctest -L Integration         # 集成测试
ctest -L Performance         # 性能测试

# 运行特定测试
ctest -R "CHTL.*Parser"      # 解析器相关测试
ctest -R "CJMOD.*"           # CJMOD相关测试

# 详细输出
ctest --verbose --output-on-failure

# 并行测试
ctest -j$(nproc)
```

### 📊 **测试报告**

```bash
# 生成HTML测试报告
ctest -D ExperimentalTest
ctest -D ExperimentalSubmit

# 代码覆盖率报告 (需要启用覆盖率)
make coverage
# 生成 coverage_report/index.html

# 性能测试报告
make benchmark
# 生成 performance_report.html
```

## 包管理和分发

### 📦 **创建分发包**

```bash
# 使用CPack创建安装包
cd build_release
cpack

# 生成的包类型:
# Linux: .deb, .rpm, .tar.gz
# Windows: .exe, .msi, .zip
# macOS: .dmg, .pkg, .tar.gz
```

### 🐳 **Docker构建**

```dockerfile
# Dockerfile示例
FROM ubuntu:20.04

RUN apt-get update && apt-get install -y \
    build-essential cmake git \
    libantlr4-runtime-dev

COPY . /workspace/CHTL
WORKDIR /workspace/CHTL

RUN ./scripts/build_release.sh

CMD ["./build_release/src/chtl"]
```

```bash
# 构建Docker镜像
docker build -t chtl:latest .

# 运行
docker run -v $(pwd):/workspace chtl:latest input.chtl -o output.html
```

## 故障排除

### ❌ **常见构建错误**

#### 1. CMake版本过低
```bash
# 错误信息
CMake Error: CMake 3.15 or higher is required.

# 解决方案
# Ubuntu/Debian
sudo apt install cmake

# 或者从源码编译最新版本
wget https://github.com/Kitware/CMake/releases/download/v3.25.0/cmake-3.25.0.tar.gz
```

#### 2. C++17支持问题
```bash
# 错误信息
error: 'std::optional' is not a member of 'std'

# 解决方案
# 确保编译器支持C++17
g++ --version  # 需要7.0+
clang++ --version  # 需要6.0+

# 或者显式设置C++标准
cmake .. -DCMAKE_CXX_STANDARD=17
```

#### 3. ANTLR4依赖缺失
```bash
# 错误信息
Could NOT find ANTLR4 (missing: ANTLR4_LIBRARIES ANTLR4_INCLUDE_DIRS)

# 解决方案
# 禁用ANTLR4支持
cmake .. -DCHTL_ENABLE_ANTLR=OFF

# 或者安装ANTLR4
sudo apt install libantlr4-runtime-dev  # Ubuntu
brew install antlr4-cpp-runtime         # macOS
```

#### 4. 内存不足
```bash
# 错误信息
virtual memory exhausted: Cannot allocate memory

# 解决方案
# 减少并行编译数
make -j2  # 使用2个线程而不是所有核心

# 或者增加交换空间
sudo fallocate -l 2G /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
```

### 🔧 **调试构建问题**

```bash
# 详细CMake输出
cmake .. --debug-output

# 详细编译输出
make VERBOSE=1

# 检查编译器标志
cmake .. -DCHTL_VERBOSE_BUILD=ON

# 生成编译数据库
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

## 持续集成

### 🔄 **GitHub Actions配置**

```yaml
# .github/workflows/build.yml
name: Build and Test

on: [push, pull_request]

jobs:
  build:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        build_type: [Release, Debug]
    
    runs-on: ${{ matrix.os }}
    
    steps:
    - uses: actions/checkout@v3
      with:
        submodules: recursive
    
    - name: Install dependencies (Ubuntu)
      if: matrix.os == 'ubuntu-latest'
      run: |
        sudo apt update
        sudo apt install build-essential cmake libgtest-dev
    
    - name: Configure CMake
      run: |
        cmake -B build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
    
    - name: Build
      run: cmake --build build --parallel
    
    - name: Test
      run: cd build && ctest --output-on-failure
```

### 📈 **性能回归检测**

```bash
# 性能基准测试
cd build_release
make benchmark

# 与历史数据比较
./scripts/compare_performance.sh baseline.json current.json
```

---

通过本指南，您应该能够在任何支持的平台上成功构建CHTL项目，并根据需要配置不同的构建选项。如果遇到问题，请参考故障排除部分或提交Issue到项目仓库。