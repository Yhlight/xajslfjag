#!/bin/bash

# CHTL 构建脚本

echo "=== CHTL 构建脚本 ==="
echo ""

# 检查CMake是否安装
if ! command -v cmake &> /dev/null; then
    echo "错误: CMake未安装。请先安装CMake。"
    exit 1
fi

# 创建构建目录
if [ ! -d "build" ]; then
    mkdir build
    echo "创建构建目录: build/"
fi

# 进入构建目录
cd build

# 配置项目
echo "配置项目..."
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "错误: CMake配置失败"
    exit 1
fi

# 编译项目
echo ""
echo "编译项目..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "错误: 编译失败"
    exit 1
fi

# 运行测试
echo ""
echo "运行测试..."
make test

echo ""
echo "=== 构建完成 ==="
echo "可执行文件位于: build/bin/chtlc"