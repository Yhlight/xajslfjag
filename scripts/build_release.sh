#!/bin/bash

# CHTL Release Build Script
# This script builds CHTL in release mode with optimizations

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build_release"
INSTALL_PREFIX="/usr/local"
CMAKE_BUILD_TYPE="Release"

echo -e "${BLUE}CHTL Release Build Script${NC}"
echo "=============================="

# Check dependencies
echo -e "${YELLOW}Checking dependencies...${NC}"
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}ERROR: CMake is not installed${NC}"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo -e "${RED}ERROR: Make is not installed${NC}"
    exit 1
fi

# Check C++ compiler
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo -e "${RED}ERROR: No C++ compiler found (g++ or clang++)${NC}"
    exit 1
fi

echo -e "${GREEN}Dependencies check passed${NC}"

# Clean previous build
if [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
echo -e "${YELLOW}Creating build directory...${NC}"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo -e "${YELLOW}Configuring with CMake...${NC}"
cmake .. \
    -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG -march=native" \
    -DBUILD_SHARED_LIBS=ON \
    -DENABLE_TESTING=OFF \
    -DENABLE_BENCHMARKS=OFF

# Build
echo -e "${YELLOW}Building CHTL...${NC}"
make -j$(nproc)

# Run basic tests if available
if [ -f "tests/chtl_tests" ]; then
    echo -e "${YELLOW}Running basic tests...${NC}"
    make test
fi

echo -e "${GREEN}Release build completed successfully!${NC}"
echo "Binary location: $(pwd)/src/chtl"
echo "To install system-wide, run: sudo make install"

# Create package info
echo -e "${YELLOW}Creating package info...${NC}"
cat > CHTL_RELEASE_INFO.txt << EOF
CHTL Release Build Information
==============================
Build Date: $(date)
Build Type: Release
Compiler: $(cmake --version | head -n1)
Optimization: -O3 -march=native
Binary: $(pwd)/src/chtl
Version: $(git describe --tags --always 2>/dev/null || echo "dev")

Installation:
sudo make install

Usage:
./src/chtl input.chtl -o output.html
EOF

echo -e "${GREEN}Build information saved to CHTL_RELEASE_INFO.txt${NC}"
echo -e "${BLUE}Release build process completed!${NC}"