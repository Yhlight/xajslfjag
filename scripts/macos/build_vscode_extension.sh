#!/bin/bash
# VSCode Extension Build Script for macOS

echo "================================================"
echo "CHTL VSCode Extension Build for macOS"
echo "================================================"

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$PROJECT_ROOT"

# Check if vscode-extension directory exists
if [ ! -d "vscode-extension" ]; then
    echo "Error: vscode-extension directory not found"
    exit 1
fi

cd vscode-extension

echo "Installing Node.js dependencies..."
npm install

if [ $? -ne 0 ]; then
    echo "npm install failed"
    exit 1
fi

echo "Compiling TypeScript..."
npm run compile

if [ $? -ne 0 ]; then
    echo "TypeScript compilation failed"
    exit 1
fi

echo "Copying compiler binaries..."
mkdir -p bin
mkdir -p modules

# Copy compiler binary
if [ -f "../src/build_release/chtl_compiler" ]; then
    cp "../src/build_release/chtl_compiler" bin/
    echo "  ✓ Compiler binary copied (Release)"
elif [ -f "../src/build_debug/chtl_compiler" ]; then
    cp "../src/build_debug/chtl_compiler" bin/
    echo "  ✓ Compiler binary copied (Debug)"
elif [ -f "../src/build/chtl_compiler" ]; then
    cp "../src/build/chtl_compiler" bin/
    echo "  ✓ Compiler binary copied"
else
    echo "  ⚠️ Compiler binary not found, extension will work without compilation"
fi

# Copy official modules
if [ -d "../src/Module" ]; then
    cp -r ../src/Module/* modules/
    echo "  ✓ Official modules copied"
    
    # List copied modules
    echo "  📋 Included modules:"
    for module in modules/*/; do
        if [ -d "$module" ]; then
            module_name=$(basename "$module")
            echo "    - $module_name"
        fi
    done
else
    echo "  ⚠️ Official modules directory not found"
fi

echo "Creating VSIX package..."

# Install vsce if not available
if ! command -v vsce &> /dev/null; then
    echo "Installing vsce globally..."
    npm install -g vsce
fi

# Create VSIX package
vsce package

if [ $? -eq 0 ]; then
    echo "✅ VSCode extension package created successfully"
    
    # Move VSIX to project root
    vsix_file=$(ls -1 *.vsix 2>/dev/null | head -1)
    if [ -n "$vsix_file" ]; then
        mv "$vsix_file" "../"
        echo "  📦 Extension package: ../$vsix_file"
    fi
    
    echo ""
    echo "🎊 VSCode Extension Build Completed for macOS!"
    echo "================================="
    echo "Extension features:"
    echo "  ✓ CHTL syntax highlighting"
    echo "  ✓ Auto-completion with official modules"
    echo "  ✓ Compile and preview commands"
    echo "  ✓ Live server with real-time reload"
    echo "  ✓ Right-click context menu"
    echo "  ✓ Keyboard shortcuts (Ctrl+Shift+C/P)"
    echo "  ✓ Built-in compiler and modules"
    echo ""
    echo "Installation: code --install-extension $vsix_file"
    echo "================================="
    
else
    echo "❌ VSCode extension packaging failed"
    exit 1
fi