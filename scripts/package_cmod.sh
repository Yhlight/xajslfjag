#!/bin/bash

# CHTL CMOD Package Script
# This script packages CMOD modules according to CHTL specification

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}CHTL CMOD Package Script${NC}"
echo "============================="

# Function to show usage
show_usage() {
    echo "Usage: $0 <module_directory> [output_directory]"
    echo ""
    echo "Arguments:"
    echo "  module_directory    Path to the CMOD module directory"
    echo "  output_directory    Output directory for .cmod file (default: current directory)"
    echo ""
    echo "CMOD Standard Structure:"
    echo "  ModuleName/"
    echo "  ├── src/"
    echo "  │   ├── ModuleName.chtl    (main module file)"
    echo "  │   └── Other.chtl         (other module files)"
    echo "  └── info/"
    echo "      └── ModuleName.chtl    (module information)"
    echo ""
    echo "Required fields in info/ModuleName.chtl:"
    echo "  [Info] {"
    echo "    name = \"module_name\";"
    echo "    version = \"1.0.0\";"
    echo "    description = \"Module description\";"
    echo "    author = \"Author Name\";"
    echo "    license = \"MIT\";"
    echo "    dependencies = \"\";"
    echo "    category = \"category\";"
    echo "    minCHTLVersion = \"1.0.0\";"
    echo "    maxCHTLVersion = \"2.0.0\";"
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

echo -e "${YELLOW}Validating CMOD structure for: $MODULE_NAME${NC}"

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
required_fields=("name" "version" "description" "author" "license" "category" "minCHTLVersion" "maxCHTLVersion")
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

echo -e "${GREEN}Module information validation passed${NC}"
echo "  Name: $MODULE_NAME"
echo "  Version: $MODULE_VERSION"
echo "  Author: $MODULE_AUTHOR"

# Check for main module file (optional for modules with submodules)
MAIN_MODULE_FILE="$MODULE_DIR/src/$MODULE_NAME.chtl"
HAS_MAIN_MODULE=false
if [ -f "$MAIN_MODULE_FILE" ]; then
    HAS_MAIN_MODULE=true
    echo -e "${GREEN}Main module file found: $MAIN_MODULE_FILE${NC}"
fi

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
    done
fi

# Validate that module has content
if [ "$HAS_MAIN_MODULE" = false ] && [ ${#SUBMODULES[@]} -eq 0 ]; then
    echo -e "${RED}ERROR: Module has no main module file and no submodules${NC}"
    exit 1
fi

# Count CHTL files
CHTL_FILES_COUNT=$(find "$MODULE_DIR/src" -name "*.chtl" | wc -l)
echo -e "${GREEN}Found $CHTL_FILES_COUNT CHTL files${NC}"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Package the module
OUTPUT_FILE="$OUTPUT_DIR/$MODULE_NAME-$MODULE_VERSION.cmod"
TEMP_PACKAGE_DIR="/tmp/cmod_package_$$"

echo -e "${YELLOW}Creating CMOD package...${NC}"

# Create temporary directory for packaging
mkdir -p "$TEMP_PACKAGE_DIR"

# Copy module contents
cp -r "$MODULE_DIR"/* "$TEMP_PACKAGE_DIR/"

# Create package metadata
cat > "$TEMP_PACKAGE_DIR/CMOD_METADATA" << EOF
CMOD Package Metadata
=====================
Package: $MODULE_NAME
Version: $MODULE_VERSION
Author: $MODULE_AUTHOR
Created: $(date -u +"%Y-%m-%dT%H:%M:%SZ")
CHTL Compiler: $(git describe --tags --always 2>/dev/null || echo "dev")
Files: $CHTL_FILES_COUNT CHTL files
Submodules: ${#SUBMODULES[@]}

Structure:
$(find "$TEMP_PACKAGE_DIR" -type f -name "*.chtl" | sed "s|$TEMP_PACKAGE_DIR/||" | sort)
EOF

# Create the .cmod file (tar.gz format with .cmod extension)
cd "$TEMP_PACKAGE_DIR"
tar -czf "$OUTPUT_FILE" ./*

# Clean up temporary directory
rm -rf "$TEMP_PACKAGE_DIR"

# Verify package
if [ -f "$OUTPUT_FILE" ]; then
    PACKAGE_SIZE=$(du -h "$OUTPUT_FILE" | cut -f1)
    echo -e "${GREEN}CMOD package created successfully!${NC}"
    echo "  File: $OUTPUT_FILE"
    echo "  Size: $PACKAGE_SIZE"
    
    # Create package info file
    INFO_OUTPUT="$OUTPUT_DIR/$MODULE_NAME-$MODULE_VERSION.cmod.info"
    cat > "$INFO_OUTPUT" << EOF
CMOD Package Information
========================
Package: $MODULE_NAME-$MODULE_VERSION.cmod
Module: $MODULE_NAME
Version: $MODULE_VERSION
Author: $MODULE_AUTHOR
Size: $PACKAGE_SIZE
Created: $(date)

Installation:
Place this .cmod file in your CHTL project's 'module' directory.

Usage in CHTL:
Import @Chtl from $MODULE_NAME;
EOF

    echo -e "${GREEN}Package info created: $INFO_OUTPUT${NC}"
    
    # Show package contents
    echo -e "${YELLOW}Package contents:${NC}"
    tar -tzf "$OUTPUT_FILE" | head -20
    if [ $(tar -tzf "$OUTPUT_FILE" | wc -l) -gt 20 ]; then
        echo "... and $(($(tar -tzf "$OUTPUT_FILE" | wc -l) - 20)) more files"
    fi
    
else
    echo -e "${RED}ERROR: Failed to create CMOD package${NC}"
    exit 1
fi

echo -e "${BLUE}CMOD packaging completed successfully!${NC}"