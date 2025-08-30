#!/bin/bash

# CHTL Unified Package Script
# This script handles packaging of CMOD, CJMOD, and mixed modules

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${BLUE}CHTL Unified Package Script${NC}"
echo "============================"

# Function to show usage
show_usage() {
    echo "Usage: $0 <module_directory> [output_directory]"
    echo ""
    echo "Arguments:"
    echo "  module_directory    Path to the module directory"
    echo "  output_directory    Output directory for package files (default: current directory)"
    echo ""
    echo "Supported Module Types:"
    echo "  1. CMOD only        - Contains only CHTL modules"
    echo "  2. CJMOD only       - Contains only C++ extensions"
    echo "  3. Mixed (CMOD+CJMOD) - Contains both CHTL modules and C++ extensions"
    echo ""
    echo "Mixed Module Structure:"
    echo "  ModuleName/"
    echo "  ├── CMOD/           # CHTL modules"
    echo "  │   └── SubModule/"
    echo "  │       ├── src/"
    echo "  │       └── info/"
    echo "  ├── CJMOD/          # C++ extensions"
    echo "  │   └── SubModule/"
    echo "  │       ├── src/"
    echo "  │       └── info/"
    echo "  └── info/"
    echo "      └── ModuleName.chtl  # Main module info"
    echo ""
    echo "Examples:"
    echo "  $0 MyModule                    # Package to current directory"
    echo "  $0 MyModule output/            # Package to output/ directory"
    echo "  $0 MyModule --analyze          # Analyze module structure only"
    exit 1
}

# Function to analyze module structure
analyze_module() {
    local module_dir="$1"
    local module_name=$(basename "$module_dir")
    
    echo -e "${CYAN}Analyzing module structure: $module_name${NC}"
    echo "============================================"
    
    # Check for different module types
    local has_cmod=false
    local has_cjmod=false
    local has_mixed=false
    local module_type="unknown"
    
    # Check for mixed structure (CMOD + CJMOD folders)
    if [ -d "$module_dir/CMOD" ] || [ -d "$module_dir/cmod" ] || [ -d "$module_dir/Cmod" ]; then
        has_mixed=true
        has_cmod=true
        echo -e "${GREEN}✓ Found CMOD directory in mixed structure${NC}"
    fi
    
    if [ -d "$module_dir/CJMOD" ] || [ -d "$module_dir/cjmod" ] || [ -d "$module_dir/CJmod" ]; then
        has_mixed=true
        has_cjmod=true
        echo -e "${GREEN}✓ Found CJMOD directory in mixed structure${NC}"
    fi
    
    # Check for standalone CMOD structure
    if [ "$has_mixed" = false ] && [ -d "$module_dir/src" ] && [ -d "$module_dir/info" ]; then
        # Check if it contains C++ files (CJMOD) or CHTL files (CMOD)
        local cpp_files=$(find "$module_dir/src" -name "*.cpp" -o -name "*.h" -o -name "*.hpp" 2>/dev/null | wc -l)
        local chtl_files=$(find "$module_dir/src" -name "*.chtl" 2>/dev/null | wc -l)
        
        if [ $cpp_files -gt 0 ] && [ $chtl_files -eq 0 ]; then
            has_cjmod=true
            module_type="cjmod"
            echo -e "${GREEN}✓ Standalone CJMOD structure detected${NC}"
        elif [ $chtl_files -gt 0 ] && [ $cpp_files -eq 0 ]; then
            has_cmod=true
            module_type="cmod"
            echo -e "${GREEN}✓ Standalone CMOD structure detected${NC}"
        else
            echo -e "${YELLOW}⚠ Mixed file types in standalone structure${NC}"
        fi
    fi
    
    if [ "$has_mixed" = true ]; then
        module_type="mixed"
    fi
    
    echo ""
    echo "Module Analysis Results:"
    echo "  Type: $module_type"
    echo "  Has CMOD: $has_cmod"
    echo "  Has CJMOD: $has_cjmod"
    echo "  Mixed Structure: $has_mixed"
    
    if [ "$has_cmod" = true ]; then
        if [ "$has_mixed" = true ]; then
            local cmod_dir="$module_dir/CMOD"
            [ ! -d "$cmod_dir" ] && cmod_dir="$module_dir/cmod"
            [ ! -d "$cmod_dir" ] && cmod_dir="$module_dir/Cmod"
            local cmod_files=$(find "$cmod_dir" -name "*.chtl" 2>/dev/null | wc -l)
        else
            local cmod_files=$(find "$module_dir/src" -name "*.chtl" 2>/dev/null | wc -l)
        fi
        echo "  CHTL Files: $cmod_files"
    fi
    
    if [ "$has_cjmod" = true ]; then
        if [ "$has_mixed" = true ]; then
            local cjmod_dir="$module_dir/CJMOD"
            [ ! -d "$cjmod_dir" ] && cjmod_dir="$module_dir/cjmod"
            [ ! -d "$cjmod_dir" ] && cjmod_dir="$module_dir/CJmod"
            local cpp_files=$(find "$cjmod_dir" -name "*.cpp" -o -name "*.h" -o -name "*.hpp" 2>/dev/null | wc -l)
        else
            local cpp_files=$(find "$module_dir/src" -name "*.cpp" -o -name "*.h" -o -name "*.hpp" 2>/dev/null | wc -l)
        fi
        echo "  C++ Files: $cpp_files"
    fi
    
    echo ""
    return 0
}

# Check arguments
if [ $# -lt 1 ] || [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    show_usage
fi

MODULE_DIR="$1"
OUTPUT_DIR="${2:-.}"

# Special analyze mode
if [ "$2" = "--analyze" ] || [ "$1" = "--analyze" ]; then
    if [ "$1" = "--analyze" ]; then
        echo -e "${RED}ERROR: Module directory required before --analyze${NC}"
        exit 1
    fi
    analyze_module "$MODULE_DIR"
    exit 0
fi

# Validate module directory
if [ ! -d "$MODULE_DIR" ]; then
    echo -e "${RED}ERROR: Module directory '$MODULE_DIR' does not exist${NC}"
    exit 1
fi

MODULE_NAME=$(basename "$MODULE_DIR")

# Analyze module structure
analyze_module "$MODULE_DIR"

# Determine packaging strategy
echo -e "${YELLOW}Determining packaging strategy...${NC}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PACKAGES_CREATED=()

# Check for mixed structure
if [ -d "$MODULE_DIR/CMOD" ] || [ -d "$MODULE_DIR/cmod" ] || [ -d "$MODULE_DIR/Cmod" ]; then
    # Find CMOD directory (case-insensitive)
    CMOD_DIR=""
    for dir in "CMOD" "cmod" "Cmod"; do
        if [ -d "$MODULE_DIR/$dir" ]; then
            CMOD_DIR="$MODULE_DIR/$dir"
            break
        fi
    done
    
    if [ -n "$CMOD_DIR" ]; then
        echo -e "${YELLOW}Packaging CMOD components...${NC}"
        
        # Package each CMOD submodule
        for submodule in "$CMOD_DIR"/*; do
            if [ -d "$submodule" ] && [ -d "$submodule/src" ] && [ -d "$submodule/info" ]; then
                submodule_name=$(basename "$submodule")
                echo -e "${CYAN}  Packaging CMOD submodule: $submodule_name${NC}"
                
                if [ -x "$SCRIPT_DIR/package_cmod.sh" ]; then
                    "$SCRIPT_DIR/package_cmod.sh" "$submodule" "$OUTPUT_DIR"
                    PACKAGES_CREATED+=("$submodule_name.cmod")
                else
                    echo -e "${RED}ERROR: package_cmod.sh not found or not executable${NC}"
                    exit 1
                fi
            fi
        done
    fi
fi

if [ -d "$MODULE_DIR/CJMOD" ] || [ -d "$MODULE_DIR/cjmod" ] || [ -d "$MODULE_DIR/CJmod" ]; then
    # Find CJMOD directory (case-insensitive)
    CJMOD_DIR=""
    for dir in "CJMOD" "cjmod" "CJmod"; do
        if [ -d "$MODULE_DIR/$dir" ]; then
            CJMOD_DIR="$MODULE_DIR/$dir"
            break
        fi
    done
    
    if [ -n "$CJMOD_DIR" ]; then
        echo -e "${YELLOW}Packaging CJMOD components...${NC}"
        
        # Package each CJMOD submodule
        for submodule in "$CJMOD_DIR"/*; do
            if [ -d "$submodule" ] && [ -d "$submodule/src" ] && [ -d "$submodule/info" ]; then
                submodule_name=$(basename "$submodule")
                echo -e "${CYAN}  Packaging CJMOD submodule: $submodule_name${NC}"
                
                if [ -x "$SCRIPT_DIR/package_cjmod.sh" ]; then
                    "$SCRIPT_DIR/package_cjmod.sh" "$submodule" "$OUTPUT_DIR"
                    PACKAGES_CREATED+=("$submodule_name.cjmod")
                else
                    echo -e "${RED}ERROR: package_cjmod.sh not found or not executable${NC}"
                    exit 1
                fi
            fi
        done
    fi
fi

# Check for standalone module
if [ ${#PACKAGES_CREATED[@]} -eq 0 ]; then
    # Check if it's a standalone CMOD or CJMOD
    if [ -d "$MODULE_DIR/src" ] && [ -d "$MODULE_DIR/info" ]; then
        cpp_files=$(find "$MODULE_DIR/src" -name "*.cpp" -o -name "*.h" -o -name "*.hpp" 2>/dev/null | wc -l)
        chtl_files=$(find "$MODULE_DIR/src" -name "*.chtl" 2>/dev/null | wc -l)
        
        if [ $cpp_files -gt 0 ] && [ $chtl_files -eq 0 ]; then
            echo -e "${YELLOW}Packaging standalone CJMOD...${NC}"
            if [ -x "$SCRIPT_DIR/package_cjmod.sh" ]; then
                "$SCRIPT_DIR/package_cjmod.sh" "$MODULE_DIR" "$OUTPUT_DIR"
                PACKAGES_CREATED+=("$MODULE_NAME.cjmod")
            else
                echo -e "${RED}ERROR: package_cjmod.sh not found or not executable${NC}"
                exit 1
            fi
        elif [ $chtl_files -gt 0 ]; then
            echo -e "${YELLOW}Packaging standalone CMOD...${NC}"
            if [ -x "$SCRIPT_DIR/package_cmod.sh" ]; then
                "$SCRIPT_DIR/package_cmod.sh" "$MODULE_DIR" "$OUTPUT_DIR"
                PACKAGES_CREATED+=("$MODULE_NAME.cmod")
            else
                echo -e "${RED}ERROR: package_cmod.sh not found or not executable${NC}"
                exit 1
            fi
        else
            echo -e "${RED}ERROR: Module contains mixed file types or no recognizable content${NC}"
            exit 1
        fi
    else
        echo -e "${RED}ERROR: No recognizable module structure found${NC}"
        exit 1
    fi
fi

# Create unified package if multiple packages were created
if [ ${#PACKAGES_CREATED[@]} -gt 1 ]; then
    echo -e "${YELLOW}Creating unified package...${NC}"
    
    UNIFIED_PACKAGE="$OUTPUT_DIR/$MODULE_NAME-unified.chtlpkg"
    TEMP_UNIFIED_DIR="/tmp/unified_package_$$"
    
    mkdir -p "$TEMP_UNIFIED_DIR"
    
    # Copy all created packages
    for package in "${PACKAGES_CREATED[@]}"; do
        package_path="$OUTPUT_DIR/$package"
        if [ -f "$package_path" ]; then
            cp "$package_path" "$TEMP_UNIFIED_DIR/"
        fi
    done
    
    # Create unified metadata
    cat > "$TEMP_UNIFIED_DIR/UNIFIED_METADATA" << EOF
CHTL Unified Package Metadata
=============================
Package: $MODULE_NAME-unified
Created: $(date -u +"%Y-%m-%dT%H:%M:%SZ")
Components: ${#PACKAGES_CREATED[@]}

Included Packages:
$(printf "  - %s\n" "${PACKAGES_CREATED[@]}")

Installation Instructions:
1. Extract this .chtlpkg file
2. Install individual .cmod files to CHTL module directory
3. Build and install .cjmod files to CHTL JS cjmod directory

Usage:
Import @Chtl from $MODULE_NAME;     # For CMOD components
Import @CJmod from $MODULE_NAME;    # For CJMOD components
EOF
    
    # Create the unified package
    cd "$TEMP_UNIFIED_DIR"
    tar -czf "$UNIFIED_PACKAGE" ./*
    
    # Clean up
    rm -rf "$TEMP_UNIFIED_DIR"
    
    if [ -f "$UNIFIED_PACKAGE" ]; then
        echo -e "${GREEN}Unified package created: $UNIFIED_PACKAGE${NC}"
        PACKAGES_CREATED+=("$MODULE_NAME-unified.chtlpkg")
    fi
fi

# Summary
echo ""
echo -e "${GREEN}Packaging Summary${NC}"
echo "=================="
echo "Module: $MODULE_NAME"
echo "Packages Created: ${#PACKAGES_CREATED[@]}"

for package in "${PACKAGES_CREATED[@]}"; do
    package_path="$OUTPUT_DIR/$package"
    if [ -f "$package_path" ]; then
        size=$(du -h "$package_path" | cut -f1)
        echo "  - $package ($size)"
    fi
done

echo ""
echo -e "${BLUE}Unified packaging completed successfully!${NC}"