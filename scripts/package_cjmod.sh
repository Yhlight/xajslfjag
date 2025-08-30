#!/bin/bash
# CJMOD Packaging Script
# Package CJMOD modules according to CHTL syntax documentation

echo "⚡ CJMOD Module Packaging Script"
echo "================================="

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

echo "🔍 Scanning for CJMOD modules..."

# Find all potential CJMOD modules
for module_dir in */; do
    if [ -d "$module_dir" ]; then
        module_name=$(basename "$module_dir")
        echo ""
        echo "📋 Processing module: $module_name"
        
        # Check for CJMOD structure
        cjmod_path=""
        
        # Look for CJMOD folder variants
        for variant in "CJMOD" "cjmod" "CJmod"; do
            if [ -d "$module_dir$variant" ]; then
                cjmod_path="$module_dir$variant"
                break
            fi
        done
        
        if [ -n "$cjmod_path" ]; then
            echo "  ✓ Found CJMOD structure: $cjmod_path"
            
            # Validate CJMOD structure according to syntax documentation
            if [ -d "$cjmod_path/$module_name/src" ] && [ -d "$cjmod_path/$module_name/info" ]; then
                echo "  ✓ Valid CJMOD structure detected"
                
                # Check for C++ source files
                cpp_files=$(find "$cjmod_path/$module_name/src" -name "*.cpp" -o -name "*.h" | wc -l)
                if [ $cpp_files -gt 0 ]; then
                    echo "  ✓ C++ source files found: $cpp_files files"
                    
                    # Check same-name constraint
                    info_file="$cjmod_path/$module_name/info/$module_name.chtl"
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
                            
                            # Compile C++ sources first
                            echo "  🔧 Compiling C++ sources..."
                            src_dir="$cjmod_path/$module_name/src"
                            
                            # Simple compilation (in real implementation, this should be more sophisticated)
                            cd "$src_dir"
                            cpp_compiled=true
                            
                            for cpp_file in *.cpp; do
                                if [ -f "$cpp_file" ]; then
                                    echo "    Compiling: $cpp_file"
                                    g++ -std=c++17 -c "$cpp_file" -o "${cpp_file%.cpp}.o" 2>/dev/null
                                    
                                    if [ $? -ne 0 ]; then
                                        echo "    ⚠️ Compilation warning for $cpp_file"
                                        cpp_compiled=false
                                    fi
                                fi
                            done
                            
                            cd "$PROJECT_ROOT/src_new/Module"
                            
                            if [ "$cpp_compiled" = true ]; then
                                echo "  ✓ C++ compilation completed"
                            else
                                echo "  ⚠️ C++ compilation had warnings"
                            fi
                            
                            # Create CJMOD package
                            package_name="${module_name}-${module_version}.cjmod"
                            package_path="../../packaged_modules/$package_name"
                            
                            echo "  📦 Creating CJMOD package: $package_name"
                            
                            # Create temporary directory for packaging
                            temp_dir="/tmp/cjmod_package_$$"
                            mkdir -p "$temp_dir"
                            
                            # Copy CJMOD structure
                            cp -r "$cjmod_path/$module_name" "$temp_dir/"
                            
                            # Create ZIP package (CJMOD format)
                            cd "$temp_dir"
                            zip -r "$PROJECT_ROOT/packaged_modules/$package_name" "$module_name"
                            
                            if [ $? -eq 0 ]; then
                                echo "  ✅ CJMOD package created successfully"
                            else
                                echo "  ❌ CJMOD packaging failed"
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
                    echo "  ❌ No C++ source files found"
                fi
            else
                echo "  ❌ Invalid CJMOD structure"
            fi
        else
            echo "  ⚠️ No CJMOD structure found in $module_name"
        fi
    fi
done

echo ""
echo "📊 CJMOD Packaging Summary"
echo "================================="
packaged_count=$(ls -1 ../../packaged_modules/*.cjmod 2>/dev/null | wc -l)
echo "Packaged CJMOD modules: $packaged_count"

if [ $packaged_count -gt 0 ]; then
    echo ""
    echo "📦 Packaged modules:"
    ls -la ../../packaged_modules/*.cjmod 2>/dev/null
fi

echo ""
echo "✅ CJMOD packaging completed"
echo "================================="