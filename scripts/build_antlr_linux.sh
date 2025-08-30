#!/bin/bash

# ANTLR4 Linux 编译脚本
# 用于编译Linux版本的ANTLR4运行时库

echo "🔧 开始编译ANTLR4 Linux版本..."
echo "================================"

# 设置变量
ANTLR_DIR="third-party/antlr/ANTLR4"
BUILD_DIR="build/antlr"
INSTALL_DIR="third-party/antlr/ANTLR4/lib/linux"

# 创建构建目录
echo "📁 创建构建目录..."
mkdir -p "$BUILD_DIR"
mkdir -p "$INSTALL_DIR"

# 进入构建目录
cd "$BUILD_DIR"

# 设置编译选项
export CXX=g++
export CXXFLAGS="-std=c++17 -fPIC -O2 -DNDEBUG"

echo "🔍 收集ANTLR源文件..."

# 收集所有ANTLR源文件
ANTLR_SOURCES=""
for cpp_file in $(find "../../$ANTLR_DIR" -name "*.cpp" | grep -v test | grep -v example); do
    ANTLR_SOURCES="$ANTLR_SOURCES $cpp_file"
done

echo "📊 找到 $(echo $ANTLR_SOURCES | wc -w) 个源文件"

# 编译为静态库
echo "🏗️  编译ANTLR4静态库..."

# 先编译目标文件
echo "  编译目标文件..."
OBJECT_FILES=""
for src_file in $ANTLR_SOURCES; do
    obj_file=$(basename "$src_file" .cpp).o
    echo "    编译 $src_file -> $obj_file"
    
    if ! $CXX $CXXFLAGS -I"../../$ANTLR_DIR/include" -c "$src_file" -o "$obj_file"; then
        echo "❌ 编译失败: $src_file"
        exit 1
    fi
    
    OBJECT_FILES="$OBJECT_FILES $obj_file"
done

# 创建静态库
echo "  创建静态库..."
if ar rcs libantlr4-runtime.a $OBJECT_FILES; then
    echo "✅ 静态库创建成功"
else
    echo "❌ 静态库创建失败"
    exit 1
fi

# 编译为动态库
echo "🔗 编译ANTLR4动态库..."
if $CXX -shared -fPIC $CXXFLAGS -I"../../$ANTLR_DIR/include" $ANTLR_SOURCES -o libantlr4-runtime.so; then
    echo "✅ 动态库创建成功"
else
    echo "❌ 动态库创建失败"
    exit 1
fi

# 安装库文件
echo "📦 安装库文件..."
cp libantlr4-runtime.a "../../$INSTALL_DIR/"
cp libantlr4-runtime.so "../../$INSTALL_DIR/"

# 创建pkg-config文件
echo "📝 创建pkg-config文件..."
cat > "../../$INSTALL_DIR/antlr4-runtime.pc" << EOF
prefix=$(pwd)/../../$INSTALL_DIR
exec_prefix=\${prefix}
libdir=\${exec_prefix}
includedir=\${prefix}/../include

Name: ANTLR4 C++ Runtime
Description: ANTLR4 C++ Runtime Library
Version: 4.13.2
Libs: -L\${libdir} -lantlr4-runtime
Cflags: -I\${includedir}
EOF

# 返回项目根目录
cd ../..

echo ""
echo "🎉 ANTLR4 Linux编译完成！"
echo "========================"
echo "✅ 静态库: $INSTALL_DIR/libantlr4-runtime.a"
echo "✅ 动态库: $INSTALL_DIR/libantlr4-runtime.so"
echo "✅ 配置文件: $INSTALL_DIR/antlr4-runtime.pc"
echo ""
echo "📋 使用方法:"
echo "  编译时添加: -I$ANTLR_DIR/include -L$INSTALL_DIR -lantlr4-runtime"
echo "  或使用pkg-config: \$(pkg-config --cflags --libs $INSTALL_DIR/antlr4-runtime.pc)"
echo ""