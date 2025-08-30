#!/bin/bash
# Linux Universal Build Script
set -e

echo "🔨 CHTL Linux Universal Build"
echo "============================"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
cd "$PROJECT_ROOT"

echo "🔧 Building CHTL compiler..."
bash "$SCRIPT_DIR/build_release.sh"

echo "📦 Packaging modules..."
bash "$SCRIPT_DIR/package_modules.sh"

echo "🔌 Building VSCode extension..."
bash "$SCRIPT_DIR/build_vscode_extension.sh"

echo "🎊 Universal build completed!"