#!/bin/bash

# CHTL CJMOD Package Script
# This script packages CJMOD modules (C++ extensions for CHTL JS)

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}CHTL CJMOD Package Script${NC}"
echo "=============================="

# Function to show usage
show_usage() {
    echo "Usage: $0 <module_directory> [output_directory]"
    echo ""
    echo "Arguments:"
    echo "  module_directory    Path to the CJMOD module directory"
    echo "  output_directory    Output directory for .cjmod file (default: current directory)"
    echo ""
    echo "CJMOD Standard Structure:"
    echo "  CJModuleName/"
    echo "  ├── src/"
    echo "  │   ├── main.cpp           (C++ implementation)"
    echo "  │   ├── main.h             (C++ headers)"
    echo "  │   └── SubModule/"
    echo "  │       ├── src/"
    echo "  │       │   ├── sub.cpp"
    echo "  │       │   └── sub.h"
    echo "  │       └── info/"
    echo "  │           └── SubModule.chtl"
    echo "  └── info/"
    echo "      └── CJModuleName.chtl  (module information)"
    echo ""
    echo "Required fields in info/CJModuleName.chtl:"
    echo "  [Info] {"
    echo "    name = \"cjmodule_name\";"
    echo "    version = \"1.0.0\";"
    echo "    description = \"CJMOD description\";"
    echo "    author = \"Author Name\";"
    echo "    license = \"MIT\";"
    echo "    cppStandard = \"17\";"
    echo "    dependencies = \"\";"
    echo "    chtlJSVersion = \"1.0.0\";"
    echo "  }"
    exit 1
}

# Check arguments
if [ $# -lt 1 ] || [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    show_usage
fi

MODULE_DIR="$1"
OUTPUT_DIR="${2:-.}"

# Validate module directory
if [ ! -d "$MODULE_DIR" ]; then
    echo -e "${RED}ERROR: Module directory '$MODULE_DIR' does not exist${NC}"
    exit 1
fi

MODULE_NAME=$(basename "$MODULE_DIR")

echo -e "${YELLOW}Validating CJMOD structure for: $MODULE_NAME${NC}"

# Check required directories
if [ ! -d "$MODULE_DIR/src" ]; then
    echo -e "${RED}ERROR: Missing required 'src' directory${NC}"
    exit 1
fi

if [ ! -d "$MODULE_DIR/info" ]; then
    echo -e "${RED}ERROR: Missing required 'info' directory${NC}"
    exit 1
fi

# Check info file
INFO_FILE="$MODULE_DIR/info/$MODULE_NAME.chtl"
if [ ! -f "$INFO_FILE" ]; then
    echo -e "${RED}ERROR: Missing required info file: $INFO_FILE${NC}"
    exit 1
fi

echo -e "${GREEN}Basic structure validation passed${NC}"

# Validate info file content
echo -e "${YELLOW}Validating module information...${NC}"

# Check required fields in info file
required_fields=("name" "version" "description" "author" "license" "cppStandard" "chtlJSVersion")
missing_fields=()

for field in "${required_fields[@]}"; do
    if ! grep -q "$field\s*=" "$INFO_FILE"; then
        missing_fields+=("$field")
    fi
done

if [ ${#missing_fields[@]} -ne 0 ]; then
    echo -e "${RED}ERROR: Missing required fields in info file:${NC}"
    for field in "${missing_fields[@]}"; do
        echo "  - $field"
    done
    exit 1
fi

# Extract module information
MODULE_VERSION=$(grep -o 'version\s*=\s*"[^"]*"' "$INFO_FILE" | sed 's/.*"\([^"]*\)".*/\1/' || echo "unknown")
MODULE_AUTHOR=$(grep -o 'author\s*=\s*"[^"]*"' "$INFO_FILE" | sed 's/.*"\([^"]*\)".*/\1/' || echo "unknown")
CPP_STANDARD=$(grep -o 'cppStandard\s*=\s*"[^"]*"' "$INFO_FILE" | sed 's/.*"\([^"]*\)".*/\1/' || echo "17")

echo -e "${GREEN}Module information validation passed${NC}"
echo "  Name: $MODULE_NAME"
echo "  Version: $MODULE_VERSION"
echo "  Author: $MODULE_AUTHOR"
echo "  C++ Standard: C++$CPP_STANDARD"

# Check for C++ source files
CPP_FILES=$(find "$MODULE_DIR/src" -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | wc -l)
if [ $CPP_FILES -eq 0 ]; then
    echo -e "${RED}ERROR: No C++ source files found in src directory${NC}"
    exit 1
fi

echo -e "${GREEN}Found $CPP_FILES C++ source files${NC}"

# List C++ files for verification
echo -e "${YELLOW}C++ source files:${NC}"
find "$MODULE_DIR/src" -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | while read file; do
    rel_path=$(echo "$file" | sed "s|^$MODULE_DIR/||")
    echo "  - $rel_path"
done

# Check for submodules
SUBMODULES=()
for subdir in "$MODULE_DIR/src"/*; do
    if [ -d "$subdir" ] && [ -d "$subdir/src" ] && [ -d "$subdir/info" ]; then
        submodule_name=$(basename "$subdir")
        if [ -f "$subdir/info/$submodule_name.chtl" ]; then
            SUBMODULES+=("$submodule_name")
        fi
    fi
done

if [ ${#SUBMODULES[@]} -gt 0 ]; then
    echo -e "${GREEN}Found submodules:${NC}"
    for submodule in "${SUBMODULES[@]}"; do
        echo "  - $submodule"
        # Count C++ files in submodule
        sub_cpp_files=$(find "$MODULE_DIR/src/$submodule/src" -name "*.cpp" -o -name "*.h" -o -name "*.hpp" 2>/dev/null | wc -l)
        echo "    ($sub_cpp_files C++ files)"
    done
fi

# Validate C++ syntax (basic check)
echo -e "${YELLOW}Performing basic C++ syntax validation...${NC}"
SYNTAX_ERRORS=0

find "$MODULE_DIR/src" -name "*.cpp" -o -name "*.hpp" | while read cpp_file; do
    # Check for basic C++ syntax patterns
    if grep -q "#include.*CJMOD" "$cpp_file" || \
       grep -q "namespace.*CJMOD" "$cpp_file" || \
       grep -q "class.*Syntax" "$cpp_file" || \
       grep -q "class.*Arg" "$cpp_file"; then
        echo -e "${GREEN}  ✓ $cpp_file contains CJMOD patterns${NC}"
    fi
done

echo -e "${GREEN}C++ validation completed${NC}"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Package the module
OUTPUT_FILE="$OUTPUT_DIR/$MODULE_NAME-$MODULE_VERSION.cjmod"
TEMP_PACKAGE_DIR="/tmp/cjmod_package_$$"

echo -e "${YELLOW}Creating CJMOD package...${NC}"

# Create temporary directory for packaging
mkdir -p "$TEMP_PACKAGE_DIR"

# Copy module contents
cp -r "$MODULE_DIR"/* "$TEMP_PACKAGE_DIR/"

# Create CMakeLists.txt for the CJMOD
cat > "$TEMP_PACKAGE_DIR/CMakeLists.txt" << EOF
# Generated CMakeLists.txt for CJMOD: $MODULE_NAME
cmake_minimum_required(VERSION 3.15)
project($MODULE_NAME VERSION $MODULE_VERSION)

set(CMAKE_CXX_STANDARD $CPP_STANDARD)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find CHTL JS headers (should be provided by main CHTL installation)
find_path(CHTLJS_INCLUDE_DIR 
    NAMES CHTLJS/CJMODSystem/Syntax.h
    PATHS \${CMAKE_INSTALL_PREFIX}/include /usr/local/include /usr/include
)

if(NOT CHTLJS_INCLUDE_DIR)
    message(FATAL_ERROR "CHTL JS headers not found. Please install CHTL first.")
endif()

include_directories(\${CHTLJS_INCLUDE_DIR})

# Collect all C++ source files
file(GLOB_RECURSE CJMOD_SOURCES "src/*.cpp")
file(GLOB_RECURSE CJMOD_HEADERS "src/*.h" "src/*.hpp")

# Create shared library
add_library($MODULE_NAME SHARED \${CJMOD_SOURCES})

# Set output directory
set_target_properties($MODULE_NAME PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "\${CMAKE_BINARY_DIR}/cjmod"
    RUNTIME_OUTPUT_DIRECTORY "\${CMAKE_BINARY_DIR}/cjmod"
)

# Installation rules
install(TARGETS $MODULE_NAME
    LIBRARY DESTINATION lib/chtl/cjmod
    RUNTIME DESTINATION lib/chtl/cjmod
)

install(FILES info/$MODULE_NAME.chtl
    DESTINATION share/chtl/cjmod/info
)
EOF

# Create package metadata
cat > "$TEMP_PACKAGE_DIR/CJMOD_METADATA" << EOF
CJMOD Package Metadata
======================
Package: $MODULE_NAME
Version: $MODULE_VERSION
Author: $MODULE_AUTHOR
C++ Standard: C++$CPP_STANDARD
Created: $(date -u +"%Y-%m-%dT%H:%M:%SZ")
CHTL Compiler: $(git describe --tags --always 2>/dev/null || echo "dev")
C++ Files: $CPP_FILES files
Submodules: ${#SUBMODULES[@]}

Build Instructions:
1. Extract .cjmod file
2. mkdir build && cd build
3. cmake ..
4. make
5. Copy resulting .so/.dll to CHTL JS cjmod directory

Structure:
$(find "$TEMP_PACKAGE_DIR" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.chtl" \) | sed "s|$TEMP_PACKAGE_DIR/||" | sort)
EOF

# Create the .cjmod file (tar.gz format with .cjmod extension)
cd "$TEMP_PACKAGE_DIR"
tar -czf "$OUTPUT_FILE" ./*

# Clean up temporary directory
rm -rf "$TEMP_PACKAGE_DIR"

# Verify package
if [ -f "$OUTPUT_FILE" ]; then
    PACKAGE_SIZE=$(du -h "$OUTPUT_FILE" | cut -f1)
    echo -e "${GREEN}CJMOD package created successfully!${NC}"
    echo "  File: $OUTPUT_FILE"
    echo "  Size: $PACKAGE_SIZE"
    
    # Create package info file
    INFO_OUTPUT="$OUTPUT_DIR/$MODULE_NAME-$MODULE_VERSION.cjmod.info"
    cat > "$INFO_OUTPUT" << EOF
CJMOD Package Information
=========================
Package: $MODULE_NAME-$MODULE_VERSION.cjmod
Module: $MODULE_NAME
Version: $MODULE_VERSION
Author: $MODULE_AUTHOR
C++ Standard: C++$CPP_STANDARD
Size: $PACKAGE_SIZE
Created: $(date)

Installation:
1. Extract .cjmod file to temporary directory
2. cd temporary_directory && mkdir build && cd build
3. cmake .. && make
4. Copy resulting shared library to CHTL installation

Usage in CHTL JS:
Import @CJmod from $MODULE_NAME;

Requirements:
- CHTL JS development headers
- C++$CPP_STANDARD compatible compiler
- CMake 3.15+
EOF

    echo -e "${GREEN}Package info created: $INFO_OUTPUT${NC}"
    
    # Show package contents
    echo -e "${YELLOW}Package contents:${NC}"
    tar -tzf "$OUTPUT_FILE" | head -20
    if [ $(tar -tzf "$OUTPUT_FILE" | wc -l) -gt 20 ]; then
        echo "... and $(($(tar -tzf "$OUTPUT_FILE" | wc -l) - 20)) more files"
    fi
    
else
    echo -e "${RED}ERROR: Failed to create CJMOD package${NC}"
    exit 1
fi

echo -e "${BLUE}CJMOD packaging completed successfully!${NC}"