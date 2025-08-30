#!/bin/bash
# Repository Cleanup Script for macOS

echo "================================================"
echo "CHTL Repository Cleanup for macOS"
echo "================================================"

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$PROJECT_ROOT"

echo "🔍 Analyzing repository state..."

# Remove build artifacts
echo ""
echo "🗑️ Removing build artifacts..."
find . -name "build*" -type d -exec rm -rf {} + 2>/dev/null || true
find . -name "*.o" -type f -delete 2>/dev/null || true
find . -name "*.a" -type f -path "./src/build*" -delete 2>/dev/null || true
find . -name "CMakeCache.txt" -delete 2>/dev/null || true
find . -name "CMakeFiles" -type d -exec rm -rf {} + 2>/dev/null || true
find . -name "Makefile" -path "./src/*" -delete 2>/dev/null || true
echo "  ✓ Build artifacts removed"

# Remove temporary files
echo ""
echo "🗑️ Removing temporary files..."
find . -name "*.tmp" -delete 2>/dev/null || true
find . -name "*.log" -delete 2>/dev/null || true
find . -name ".DS_Store" -delete 2>/dev/null || true
find . -name "*.swp" -delete 2>/dev/null || true
find . -name "*.swo" -delete 2>/dev/null || true
echo "  ✓ Temporary files removed"

# Remove test artifacts
echo ""
echo "🗑️ Removing test artifacts..."
find . -name "*test*" -type f -executable -delete 2>/dev/null || true
find . -name "comprehensive_test*" -delete 2>/dev/null || true
find . -name "*verification*" -delete 2>/dev/null || true
echo "  ✓ Test artifacts removed"

echo ""
echo "🎊 macOS cleanup completed!"
echo "================================================"
echo "Repository is clean and ready for development"
echo "================================================"