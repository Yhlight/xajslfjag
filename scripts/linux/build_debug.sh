#!/bin/bash
# CHTL Compiler Debug Build Script for Linux
# Build the complete CHTL compiler in debug mode

echo "================================================"
echo "CHTL Compiler Debug Build for Linux"
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
mkdir -p build_debug
cd build_debug

echo "Configuring CMake for Debug build..."
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++

if [ $? -ne 0 ]; then
    echo "CMake configuration failed"
    exit 1
fi

echo "Building CHTL Compiler in Debug mode..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

echo "================================================"
echo "CHTL Compiler Debug Build Completed Successfully"
echo "================================================"
echo "Binaries location: $(pwd)/"
echo ""
echo "Built components:"
echo "  - chtl_compiler (Debug)"
echo "  - CHTL Core Libraries (Debug)"
echo "  - Debug symbols included"
echo "  - Ready for development and debugging"
echo "================================================"