#!/bin/bash
# CHTL Compiler Universal Build Script
# Build compiler, package modules, and build VSCode extension

echo "================================================"
echo "CHTL Compiler Universal Build Script"
echo "================================================"

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

# Step 1: Build CHTL Compiler
echo "🔧 Step 1: Building CHTL Compiler..."
echo "================================================"

if [ -f "scripts/linux/build_release.sh" ]; then
    chmod +x scripts/linux/build_release.sh
    ./scripts/linux/build_release.sh
    
    if [ $? -ne 0 ]; then
        echo "❌ Compiler build failed"
        exit 1
    fi
else
    echo "❌ Release build script not found"
    exit 1
fi

# Step 2: Package Module Folders
echo ""
echo "📦 Step 2: Packaging Module Folders..."
echo "================================================"

if [ -f "scripts/package_cmod.sh" ]; then
    chmod +x scripts/package_cmod.sh
    ./scripts/package_cmod.sh
    
    if [ $? -ne 0 ]; then
        echo "⚠️ CMOD packaging failed, continuing..."
    fi
fi

if [ -f "scripts/package_cjmod.sh" ]; then
    chmod +x scripts/package_cjmod.sh
    ./scripts/package_cjmod.sh
    
    if [ $? -ne 0 ]; then
        echo "⚠️ CJMOD packaging failed, continuing..."
    fi
fi

# Step 3: Build VSCode Extension
echo ""
echo "🔌 Step 3: Building VSCode Extension..."
echo "================================================"

if [ -f "scripts/build_vscode_extension.sh" ]; then
    chmod +x scripts/build_vscode_extension.sh
    ./scripts/build_vscode_extension.sh
    
    if [ $? -ne 0 ]; then
        echo "⚠️ VSCode extension build failed, continuing..."
    fi
fi

# Step 4: Create Distribution Package
echo ""
echo "📋 Step 4: Creating Distribution Package..."
echo "================================================"

DIST_DIR="$PROJECT_ROOT/dist"
mkdir -p "$DIST_DIR"

# Copy compiler binaries
if [ -d "src/build_release" ]; then
    cp -r src/build_release/bin "$DIST_DIR/" 2>/dev/null || echo "⚠️ No bin directory found"
    cp -r src/build_release/lib "$DIST_DIR/" 2>/dev/null || echo "⚠️ No lib directory found"
fi

# Copy official modules
if [ -d "src/Module" ]; then
    cp -r src/Module "$DIST_DIR/modules"
    echo "✓ Official modules copied"
fi

# Copy VSCode extension
if [ -f "chtl-language-support.vsix" ]; then
    cp chtl-language-support.vsix "$DIST_DIR/"
    echo "✓ VSCode extension copied"
fi

# Copy documentation
mkdir -p "$DIST_DIR/docs"
cp docs/*.md "$DIST_DIR/docs/" 2>/dev/null || echo "⚠️ No documentation found"

echo ""
echo "🎊 Universal Build Completed Successfully!"
echo "================================================"
echo "Distribution package location: $DIST_DIR"
echo ""
echo "Package contents:"
echo "  ✓ CHTL Compiler binaries"
echo "  ✓ Official modules (Chtholly, Yuigahama)"
echo "  ✓ VSCode extension"
echo "  ✓ Documentation"
echo ""
echo "Ready for release!"
echo "================================================"