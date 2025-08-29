#!/bin/bash

# CHTL Build Clean Script
# This script cleans all build artifacts and temporary files

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}CHTL Build Clean Script${NC}"
echo "========================"

# Function to show usage
show_usage() {
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -a, --all         Clean everything (build dirs, temp files, packages)"
    echo "  -b, --build       Clean only build directories"
    echo "  -t, --temp        Clean only temporary files"
    echo "  -p, --packages    Clean only package files (.cmod, .cjmod)"
    echo "  -c, --cache       Clean only CMake cache and generated files"
    echo "  --dry-run         Show what would be deleted without actually deleting"
    echo "  -h, --help        Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 --all          # Clean everything"
    echo "  $0 --build        # Clean only build directories"
    echo "  $0 --dry-run      # Preview what would be cleaned"
    exit 1
}

# Default options
CLEAN_BUILD=false
CLEAN_TEMP=false
CLEAN_PACKAGES=false
CLEAN_CACHE=false
DRY_RUN=false

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -a|--all)
            CLEAN_BUILD=true
            CLEAN_TEMP=true
            CLEAN_PACKAGES=true
            CLEAN_CACHE=true
            shift
            ;;
        -b|--build)
            CLEAN_BUILD=true
            shift
            ;;
        -t|--temp)
            CLEAN_TEMP=true
            shift
            ;;
        -p|--packages)
            CLEAN_PACKAGES=true
            shift
            ;;
        -c|--cache)
            CLEAN_CACHE=true
            shift
            ;;
        --dry-run)
            DRY_RUN=true
            shift
            ;;
        -h|--help)
            show_usage
            ;;
        *)
            echo -e "${RED}ERROR: Unknown option: $1${NC}"
            show_usage
            ;;
    esac
done

# If no options specified, clean build directories by default
if [ "$CLEAN_BUILD" = false ] && [ "$CLEAN_TEMP" = false ] && [ "$CLEAN_PACKAGES" = false ] && [ "$CLEAN_CACHE" = false ]; then
    CLEAN_BUILD=true
fi

# Dry run prefix
DRY_PREFIX=""
if [ "$DRY_RUN" = true ]; then
    DRY_PREFIX="[DRY RUN] "
    echo -e "${YELLOW}Running in dry-run mode - no files will be deleted${NC}"
    echo ""
fi

# Track what was cleaned
CLEANED_ITEMS=()
TOTAL_SIZE_FREED=0

# Function to safely remove files/directories
safe_remove() {
    local path="$1"
    local description="$2"
    
    if [ -e "$path" ]; then
        # Calculate size before deletion
        if [ -d "$path" ]; then
            local size=$(du -sb "$path" 2>/dev/null | cut -f1 || echo "0")
        else
            local size=$(stat -c%s "$path" 2>/dev/null || echo "0")
        fi
        
        TOTAL_SIZE_FREED=$((TOTAL_SIZE_FREED + size))
        
        if [ "$DRY_RUN" = true ]; then
            echo -e "${YELLOW}${DRY_PREFIX}Would remove: $path ($description)${NC}"
        else
            echo -e "${YELLOW}Removing: $path ($description)${NC}"
            rm -rf "$path"
        fi
        CLEANED_ITEMS+=("$description")
    fi
}

# Function to find and remove files by pattern
find_and_remove() {
    local pattern="$1"
    local description="$2"
    local base_dir="${3:-.}"
    
    local found_files=$(find "$base_dir" -name "$pattern" 2>/dev/null)
    
    if [ -n "$found_files" ]; then
        echo -e "${YELLOW}Found files matching '$pattern':${NC}"
        echo "$found_files" | while read file; do
            if [ -n "$file" ]; then
                safe_remove "$file" "$description"
            fi
        done
    fi
}

echo -e "${YELLOW}Starting cleanup process...${NC}"

# Clean build directories
if [ "$CLEAN_BUILD" = true ]; then
    echo -e "${CYAN}Cleaning build directories...${NC}"
    
    safe_remove "build" "Main build directory"
    safe_remove "build_debug" "Debug build directory"
    safe_remove "build_release" "Release build directory"
    safe_remove "build_test" "Test build directory"
    safe_remove "bin" "Binary output directory"
    safe_remove "lib" "Library output directory"
    safe_remove "obj" "Object files directory"
    safe_remove "out" "Output directory"
    
    # Find any other build-like directories
    find_and_remove "build_*" "Build directory"
fi

# Clean temporary files
if [ "$CLEAN_TEMP" = true ]; then
    echo -e "${CYAN}Cleaning temporary files...${NC}"
    
    safe_remove "temp" "Temporary directory"
    safe_remove "tmp" "Temporary directory"
    
    # Find temporary files
    find_and_remove "*.tmp" "Temporary file"
    find_and_remove "*.temp" "Temporary file"
    find_and_remove ".*~" "Backup file"
    find_and_remove "*~" "Backup file"
    find_and_remove "*.bak" "Backup file"
    find_and_remove "*.swp" "Vim swap file"
    find_and_remove "*.swo" "Vim swap file"
    find_and_remove ".*.swp" "Vim swap file"
    find_and_remove "core" "Core dump file"
    find_and_remove "core.*" "Core dump file"
    find_and_remove "*.log" "Log file"
    find_and_remove "*.pid" "Process ID file"
    
    # Clean test output files
    find_and_remove "test_*" "Test output file"
    find_and_remove "*_test_*" "Test output file"
    find_and_remove "*_demo_*" "Demo output file"
fi

# Clean package files
if [ "$CLEAN_PACKAGES" = true ]; then
    echo -e "${CYAN}Cleaning package files...${NC}"
    
    find_and_remove "*.cmod" "CMOD package"
    find_and_remove "*.cjmod" "CJMOD package"
    find_and_remove "*.chtlpkg" "CHTL unified package"
    find_and_remove "*.cmod.info" "CMOD package info"
    find_and_remove "*.cjmod.info" "CJMOD package info"
    
    # Clean package temporary directories
    find_and_remove "*_package_*" "Package temporary directory"
fi

# Clean CMake cache and generated files
if [ "$CLEAN_CACHE" = true ]; then
    echo -e "${CYAN}Cleaning CMake cache and generated files...${NC}"
    
    safe_remove "CMakeCache.txt" "CMake cache file"
    safe_remove "CMakeFiles" "CMake files directory"
    safe_remove "cmake_install.cmake" "CMake install script"
    safe_remove "Makefile" "Generated Makefile"
    safe_remove "compile_commands.json" "Compile commands database"
    safe_remove "CTestTestfile.cmake" "CTest file"
    safe_remove "Testing" "CTest directory"
    
    # Find CMake generated files
    find_and_remove "*.cmake" "CMake generated file"
    find_and_remove "*Config.cmake" "CMake config file"
    find_and_remove "*ConfigVersion.cmake" "CMake config version file"
fi

# Clean IDE and editor files
if [ "$CLEAN_TEMP" = true ]; then
    echo -e "${CYAN}Cleaning IDE and editor files...${NC}"
    
    safe_remove ".vscode" "VS Code directory"
    safe_remove ".idea" "IntelliJ IDEA directory"
    safe_remove "*.code-workspace" "VS Code workspace"
    safe_remove ".clangd" "Clangd cache directory"
    safe_remove ".ccls-cache" "CCLS cache directory"
    safe_remove ".cquery_cache" "Cquery cache directory"
fi

# Calculate and display summary
echo ""
echo -e "${GREEN}Cleanup Summary${NC}"
echo "==============="

if [ ${#CLEANED_ITEMS[@]} -eq 0 ]; then
    echo -e "${GREEN}No items to clean${NC}"
else
    echo "Items cleaned: ${#CLEANED_ITEMS[@]}"
    
    # Convert bytes to human readable
    if [ $TOTAL_SIZE_FREED -gt 0 ]; then
        if [ $TOTAL_SIZE_FREED -gt 1073741824 ]; then
            # GB
            size_gb=$((TOTAL_SIZE_FREED / 1073741824))
            echo "Space freed: ${size_gb}GB"
        elif [ $TOTAL_SIZE_FREED -gt 1048576 ]; then
            # MB
            size_mb=$((TOTAL_SIZE_FREED / 1048576))
            echo "Space freed: ${size_mb}MB"
        elif [ $TOTAL_SIZE_FREED -gt 1024 ]; then
            # KB
            size_kb=$((TOTAL_SIZE_FREED / 1024))
            echo "Space freed: ${size_kb}KB"
        else
            echo "Space freed: ${TOTAL_SIZE_FREED}B"
        fi
    fi
    
    # Show cleaned categories
    echo ""
    echo "Categories cleaned:"
    for item in "${CLEANED_ITEMS[@]}"; do
        echo "  - $item"
    done
fi

if [ "$DRY_RUN" = true ]; then
    echo ""
    echo -e "${YELLOW}This was a dry run. To actually clean files, run without --dry-run${NC}"
fi

echo ""
echo -e "${BLUE}Cleanup completed!${NC}"