#!/bin/bash
# CHTL Compiler Release Build Script for macOS
# Build the complete CHTL compiler in release mode

echo "================================================"
echo "CHTL Compiler Release Build for macOS"
echo "================================================"

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$PROJECT_ROOT"

# Check if src directory exists
if [ ! -d "src" ]; then
    echo "Error: src directory not found"
    exit 1
fi

cd src

# Create build directory
mkdir -p build_release
cd build_release

echo "Configuring CMake for Release build..."
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++

if [ $? -ne 0 ]; then
    echo "CMake configuration failed"
    exit 1
fi

echo "Building CHTL Compiler..."
make -j$(sysctl -n hw.ncpu)

if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

echo "================================================"
echo "CHTL Compiler Release Build Completed Successfully"
echo "================================================"
echo "Binaries location: $(pwd)/"
echo ""
echo "Built components:"
echo "  - chtl_compiler"
echo "  - CHTL Core Libraries"
echo "  - ANTLR4 Integration"
echo "  - Official Modules Support"
echo ""
echo "Ready for distribution on macOS!"
echo "================================================"