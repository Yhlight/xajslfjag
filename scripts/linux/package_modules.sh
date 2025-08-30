#!/bin/bash
# CHTL Module Packaging Script for Linux
# Package CMOD and CJMOD modules

echo "================================================"
echo "CHTL Module Packaging for Linux"
echo "================================================"

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$PROJECT_ROOT"

# Check if src directory exists
if [ ! -d "src" ]; then
    echo "Error: src directory not found"
    exit 1
fi

# Create packages directory
mkdir -p packages

echo "Packaging CMOD modules..."
./scripts/package_cmod.sh

if [ $? -ne 0 ]; then
    echo "CMOD packaging failed"
    exit 1
fi

echo "Packaging CJMOD modules..."
./scripts/package_cjmod.sh

if [ $? -ne 0 ]; then
    echo "CJMOD packaging failed"
    exit 1
fi

echo "================================================"
echo "Module Packaging Completed Successfully"
echo "================================================"
echo "Packaged modules location: packages/"
ls -la packages/*.cmod packages/*.cjmod 2>/dev/null || echo "No packaged modules found"
echo "================================================"