@echo off
rem Release build script for CHTL compiler

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

echo "Building Release version..."
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo "Build failed!"
    cd ..
    exit /b %errorlevel%
)

echo "Copying executable to root directory..."
copy Release\chtl.exe ..\chtl.exe

cd ..
echo "Release build complete!"
