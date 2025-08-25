@echo off
rem Debug build script for CHTL compiler

if not exist build (
    mkdir build
)

cd build

echo "Configuring CMake..."
cmake .. -G "Visual Studio 17 2022" -A x64

if %errorlevel% neq 0 (
    echo "CMake configuration failed!"
    cd ..
    exit /b %errorlevel%
)

echo "Building Debug version..."
cmake --build . --config Debug

if %errorlevel% neq 0 (
    echo "Build failed!"
    cd ..
    exit /b %errorlevel%
)

echo "Copying executable to root directory..."
copy Debug\chtl.exe ..\chtl_debug.exe

cd ..
echo "Debug build complete!"
