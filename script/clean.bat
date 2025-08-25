@echo off
rem Cleans the build artifacts for the CHTL compiler

echo "Cleaning build directory..."
if exist build (
    rmdir /s /q build
)

echo "Cleaning executables from root..."
if exist chtl.exe (
    del chtl.exe
)
if exist chtl_debug.exe (
    del chtl_debug.exe
)

echo "Cleanup complete!"
