#!/bin/bash
# Cross-Platform Universal Build Script
# Detects platform and calls appropriate platform-specific script

echo "🔨 CHTL Universal Build Script (Cross-Platform)"
echo "==============================================="

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Detect platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="linux"
    echo "🐧 Detected platform: Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macos"
    echo "🍎 Detected platform: macOS"
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    PLATFORM="windows"
    echo "🪟 Detected platform: Windows (Git Bash/Cygwin)"
else
    echo "❌ Unsupported platform: $OSTYPE"
    echo "Please run the platform-specific script manually:"
    echo "  - Windows: scripts/windows/universal_build.bat"
    echo "  - Linux: scripts/linux/universal_build.sh"
    echo "  - macOS: scripts/macos/universal_build.sh"
    exit 1
fi

# Call platform-specific script
if [ "$PLATFORM" == "windows" ]; then
    # For Windows, we need to call the .bat file
    echo "🚀 Please run: scripts/windows/universal_build.bat"
    echo "Note: This bash script cannot directly execute Windows batch files."
    exit 1
else
    PLATFORM_SCRIPT="$SCRIPT_DIR/$PLATFORM/universal_build.sh"
    
    if [ -f "$PLATFORM_SCRIPT" ]; then
        echo "🚀 Calling platform-specific build script..."
        bash "$PLATFORM_SCRIPT"
    else
        echo "❌ Platform-specific script not found: $PLATFORM_SCRIPT"
        exit 1
    fi
fi