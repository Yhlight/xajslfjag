#!/bin/bash
# CMOD Packaging Script
# Package CMOD modules according to CHTL syntax documentation

echo "📦 CMOD Module Packaging Script"
echo "================================"

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

# Check if Module directory exists
if [ ! -d "src_new/Module" ]; then
    echo "❌ Module directory not found: src_new/Module"
    exit 1
fi

cd src_new/Module

# Create output directory for packaged modules
mkdir -p "../../packaged_modules"

echo "🔍 Scanning for CMOD modules..."

# Find all potential CMOD modules
for module_dir in */; do
    if [ -d "$module_dir" ]; then
        module_name=$(basename "$module_dir")
        echo ""
        echo "📋 Processing module: $module_name"
        
        # Check for CMOD structure
        cmod_path=""
        
        # Look for CMOD folder variants
        for variant in "CMOD" "cmod" "Cmod"; do
            if [ -d "$module_dir$variant" ]; then
                cmod_path="$module_dir$variant"
                break
            fi
        done
        
        if [ -n "$cmod_path" ]; then
            echo "  ✓ Found CMOD structure: $cmod_path"
            
            # Validate CMOD structure according to syntax documentation
            if [ -d "$cmod_path/$module_name/src" ] && [ -d "$cmod_path/$module_name/info" ]; then
                echo "  ✓ Valid CMOD structure detected"
                
                # Check same-name constraint
                info_file="$cmod_path/$module_name/info/$module_name.chtl"
                if [ -f "$info_file" ]; then
                    echo "  ✓ Same-name constraint satisfied"
                    
                    # Parse module info
                    if grep -q "\[Info\]" "$info_file"; then
                        echo "  ✓ [Info] block found"
                        
                        # Extract module information
                        module_version=$(grep -o 'version = "[^"]*"' "$info_file" | sed 's/version = "\([^"]*\)"/\1/')
                        module_desc=$(grep -o 'description = "[^"]*"' "$info_file" | sed 's/description = "\([^"]*\)"/\1/')
                        
                        echo "    Name: $module_name"
                        echo "    Version: $module_version"
                        echo "    Description: $module_desc"
                        
                        # Create CMOD package
                        package_name="${module_name}-${module_version}.cmod"
                        package_path="../../packaged_modules/$package_name"
                        
                        echo "  📦 Creating CMOD package: $package_name"
                        
                        # Create temporary directory for packaging
                        temp_dir="/tmp/cmod_package_$$"
                        mkdir -p "$temp_dir"
                        
                        # Copy CMOD structure
                        cp -r "$cmod_path/$module_name" "$temp_dir/"
                        
                        # Create ZIP package (CMOD format)
                        cd "$temp_dir"
                        zip -r "$PROJECT_ROOT/packaged_modules/$package_name" "$module_name"
                        
                        if [ $? -eq 0 ]; then
                            echo "  ✅ CMOD package created successfully"
                        else
                            echo "  ❌ CMOD packaging failed"
                        fi
                        
                        # Cleanup
                        rm -rf "$temp_dir"
                        cd "$PROJECT_ROOT/src_new/Module"
                        
                    else
                        echo "  ❌ [Info] block not found in $info_file"
                    fi
                else
                    echo "  ❌ Info file not found: $info_file"
                fi
            else
                echo "  ❌ Invalid CMOD structure"
            fi
        else
            echo "  ⚠️ No CMOD structure found in $module_name"
        fi
    fi
done

echo ""
echo "📊 CMOD Packaging Summary"
echo "================================"
packaged_count=$(ls -1 ../../packaged_modules/*.cmod 2>/dev/null | wc -l)
echo "Packaged CMOD modules: $packaged_count"

if [ $packaged_count -gt 0 ]; then
    echo ""
    echo "📦 Packaged modules:"
    ls -la ../../packaged_modules/*.cmod 2>/dev/null
fi

echo ""
echo "✅ CMOD packaging completed"
echo "================================"