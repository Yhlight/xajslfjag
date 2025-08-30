#!/bin/bash
# Linux Universal Build Script - Build compiler, package modules, and build VSCode extension
# 统一构建脚本 - 构建编译器、打包模块、构建VSCode插件

set -e

echo "🔨 CHTL Linux Universal Build"
echo "============================"

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$PROJECT_ROOT"

echo "📍 Working directory: $(pwd)"

# Step 1: Build compiler (release)
echo "🔧 Step 1/4: Building CHTL compiler (Release)..."
bash "$SCRIPT_DIR/build_release.sh"

# Step 2: Package modules
echo "📦 Step 2/4: Packaging modules..."
bash "$SCRIPT_DIR/package_modules.sh"

# Step 3: Build VSCode extension
echo "🔌 Step 3/4: Building VSCode extension..."
bash "$SCRIPT_DIR/build_vscode_extension.sh"

# Step 4: Final verification
echo "✅ Step 4/4: Final verification..."
if [ -f "src/build_release/chtl-compiler" ]; then
    echo "✅ Compiler build successful"
else
    echo "❌ Compiler build failed"
    exit 1
fi

if [ -d "packages" ] && [ "$(ls -A packages 2>/dev/null)" ]; then
    echo "✅ Module packaging successful"
else
    echo "⚠️  No modules packaged (this is OK if no modules exist)"
fi

if [ -f "vscode-extension/chtl-language-support-0.1.0.vsix" ]; then
    echo "✅ VSCode extension build successful"
else
    echo "⚠️  VSCode extension build incomplete"
fi

echo ""
echo "🎊 Universal build completed successfully!"
echo "📁 Build artifacts:"
echo "   - Compiler: src/build_release/chtl-compiler"
echo "   - Modules: packages/"
echo "   - VSCode Extension: vscode-extension/chtl-language-support-0.1.0.vsix"
echo ""
echo "🚀 Ready for deployment!"