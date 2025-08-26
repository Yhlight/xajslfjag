#!/bin/bash

# CHTL编译器构建脚本

# 设置颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 构建类型（默认为Release）
BUILD_TYPE=${1:-Release}

echo -e "${GREEN}CHTL编译器构建脚本${NC}"
echo "构建类型: $BUILD_TYPE"

# 创建构建目录
BUILD_DIR="build-${BUILD_TYPE,,}"
mkdir -p $BUILD_DIR

# 进入构建目录
cd $BUILD_DIR

# 运行CMake
echo -e "${YELLOW}配置项目...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

if [ $? -ne 0 ]; then
    echo -e "${RED}CMake配置失败！${NC}"
    exit 1
fi

# 获取CPU核心数
if [ -f /proc/cpuinfo ]; then
    CORES=$(grep -c ^processor /proc/cpuinfo)
else
    CORES=$(sysctl -n hw.ncpu 2>/dev/null || echo 1)
fi

# 编译项目
echo -e "${YELLOW}编译项目（使用 $CORES 个核心）...${NC}"
make -j$CORES

if [ $? -ne 0 ]; then
    echo -e "${RED}编译失败！${NC}"
    exit 1
fi

echo -e "${GREEN}编译成功！${NC}"
echo "可执行文件位置: $BUILD_DIR/bin/chtl"

# 创建模块目录
mkdir -p bin/module/CMOD
mkdir -p bin/module/CJMOD

echo -e "${GREEN}构建完成！${NC}"