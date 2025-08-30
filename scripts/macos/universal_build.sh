#!/bin/bash
# Universal Build Script for macOS
# Build compiler, package modules, and build VSCode extension

echo "🔨 CHTL Universal Build Script (macOS)"
echo "======================================"

# Get script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$PROJECT_ROOT"

echo ""
echo "🏗️ Step 1: Building CHTL Compiler (Release)..."
echo "================================================"
bash "$SCRIPT_DIR/build_release.sh"
if [ $? -ne 0 ]; then
    echo "❌ Compiler build failed!"
    exit 1
fi

echo ""
echo "📦 Step 2: Packaging CMOD modules..."
echo "===================================="
bash "$SCRIPT_DIR/package_cmod.sh"
if [ $? -ne 0 ]; then
    echo "❌ CMOD packaging failed!"
    exit 1
fi

echo ""
echo "⚡ Step 3: Packaging CJMOD modules..."
echo "====================================="
bash "$SCRIPT_DIR/package_cjmod.sh"
if [ $? -ne 0 ]; then
    echo "❌ CJMOD packaging failed!"
    exit 1
fi

echo ""
echo "🧩 Step 4: Building VSCode Extension..."
echo "======================================="
bash "$SCRIPT_DIR/build_vscode_extension.sh"
if [ $? -ne 0 ]; then
    echo "❌ VSCode extension build failed!"
    exit 1
fi

echo ""
echo "🎉 Universal Build Completed Successfully!"
echo "========================================="
echo "✅ Compiler built and ready"
echo "✅ Modules packaged"
echo "✅ VSCode extension built"
echo ""
echo "📁 Output locations:"
echo "  - Compiler: build/Release/"
echo "  - Modules: packages/"
echo "  - VSCode Extension: vscode-extension/chtl-extension-*.vsix"