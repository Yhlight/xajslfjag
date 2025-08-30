#!/bin/bash
# Linux Universal Build Script
# 统一构建脚本 - Linux版本

set -e

echo "🔨 CHTL Linux Universal Build"
echo "============================"

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$PROJECT_ROOT"

echo "📍 Working directory: $(pwd)"

# Build compiler (release)
echo "🔧 Building CHTL compiler (Release)..."
bash "$SCRIPT_DIR/build_release.sh"

# Package modules
echo "📦 Packaging modules..."
bash "$SCRIPT_DIR/package_modules.sh"

# Build VSCode extension
echo "🔌 Building VSCode extension..."
bash "$SCRIPT_DIR/build_vscode_extension.sh"

echo ""
echo "🎊 Universal build completed successfully!"
echo "🚀 Ready for deployment!"