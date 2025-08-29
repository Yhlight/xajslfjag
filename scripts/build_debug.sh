#!/bin/bash

# CHTL Debug Build Script
# This script builds CHTL in debug mode with debugging symbols

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build_debug"
CMAKE_BUILD_TYPE="Debug"

echo -e "${BLUE}CHTL Debug Build Script${NC}"
echo "============================"

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

# Check for debug tools
if command -v gdb &> /dev/null; then
    echo -e "${GREEN}GDB debugger found${NC}"
    GDB_AVAILABLE=true
else
    echo -e "${YELLOW}GDB debugger not found (optional)${NC}"
    GDB_AVAILABLE=false
fi

if command -v valgrind &> /dev/null; then
    echo -e "${GREEN}Valgrind memory checker found${NC}"
    VALGRIND_AVAILABLE=true
else
    echo -e "${YELLOW}Valgrind not found (optional)${NC}"
    VALGRIND_AVAILABLE=false
fi

echo -e "${GREEN}Dependencies check passed${NC}"

# Clean previous build
if [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Cleaning previous debug build...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
echo -e "${YELLOW}Creating debug build directory...${NC}"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake for debug
echo -e "${YELLOW}Configuring with CMake for debug...${NC}"
cmake .. \
    -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_CXX_FLAGS_DEBUG="-g -O0 -Wall -Wextra -DDEBUG" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DBUILD_SHARED_LIBS=OFF \
    -DENABLE_TESTING=ON \
    -DENABLE_DEBUGGING=ON \
    -DENABLE_MEMORY_DEBUGGING=ON \
    -DENABLE_SANITIZERS=ON

# Build
echo -e "${YELLOW}Building CHTL in debug mode...${NC}"
make -j$(nproc)

# Run tests if available
if [ -f "tests/chtl_tests" ]; then
    echo -e "${YELLOW}Running debug tests...${NC}"
    make test
fi

echo -e "${GREEN}Debug build completed successfully!${NC}"
echo "Debug binary location: $(pwd)/src/chtl"

# Create debug helper scripts
echo -e "${YELLOW}Creating debug helper scripts...${NC}"

# GDB debug script
if [ "$GDB_AVAILABLE" = true ]; then
    cat > debug_with_gdb.sh << 'EOF'
#!/bin/bash
# Debug CHTL with GDB
echo "Starting CHTL with GDB debugger..."
echo "Type 'run <input.chtl>' to start debugging"
echo "Type 'bt' for backtrace, 'l' for list, 'p <var>' to print variables"
gdb ./src/chtl
EOF
    chmod +x debug_with_gdb.sh
    echo -e "${GREEN}Created debug_with_gdb.sh${NC}"
fi

# Valgrind memory check script
if [ "$VALGRIND_AVAILABLE" = true ]; then
    cat > debug_with_valgrind.sh << 'EOF'
#!/bin/bash
# Debug CHTL with Valgrind
if [ $# -eq 0 ]; then
    echo "Usage: $0 <input.chtl> [output.html]"
    exit 1
fi

echo "Running CHTL with Valgrind memory checker..."
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes ./src/chtl "$@"
EOF
    chmod +x debug_with_valgrind.sh
    echo -e "${GREEN}Created debug_with_valgrind.sh${NC}"
fi

# Create debug info
cat > CHTL_DEBUG_INFO.txt << EOF
CHTL Debug Build Information
============================
Build Date: $(date)
Build Type: Debug
Compiler: $(cmake --version | head -n1)
Debug Flags: -g -O0 -Wall -Wextra -DDEBUG
Binary: $(pwd)/src/chtl
Version: $(git describe --tags --always 2>/dev/null || echo "dev")

Debug Tools Available:
GDB: $GDB_AVAILABLE
Valgrind: $VALGRIND_AVAILABLE

Debug Usage:
./src/chtl input.chtl -o output.html --debug

GDB Debugging:
./debug_with_gdb.sh

Memory Debugging:
./debug_with_valgrind.sh input.chtl

Compile Commands:
compile_commands.json generated for IDE integration
EOF

echo -e "${GREEN}Debug information saved to CHTL_DEBUG_INFO.txt${NC}"
echo -e "${BLUE}Debug build process completed!${NC}"
echo ""
echo -e "${YELLOW}Debug Tips:${NC}"
echo "- Use compile_commands.json for IDE integration"
echo "- Run debug_with_gdb.sh for interactive debugging"
if [ "$VALGRIND_AVAILABLE" = true ]; then
    echo "- Run debug_with_valgrind.sh for memory leak detection"
fi