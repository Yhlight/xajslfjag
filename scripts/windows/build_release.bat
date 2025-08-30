@echo off
REM CHTL Compiler Release Build Script for Windows
REM Build the complete CHTL compiler in release mode

echo ================================================
echo CHTL Compiler Release Build for Windows
echo ================================================

cd /d "%~dp0..\.."

REM Check if src_new directory exists
if not exist "src_new" (
    echo Error: src_new directory not found
    exit /b 1
)

cd src_new

REM Create build directory
if not exist "build_release" mkdir build_release
cd build_release

echo Configuring CMake for Release build...
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019" -A x64

if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed
    exit /b 1
)

echo Building CHTL Compiler...
cmake --build . --config Release --parallel 4

if %ERRORLEVEL% neq 0 (
    echo Build failed
    exit /b 1
)

echo ================================================
echo CHTL Compiler Release Build Completed Successfully
echo ================================================
echo Binaries location: %CD%\Release\
echo.
echo Built components:
echo   - chtl_compiler.exe
echo   - CHTL Core Libraries
echo   - ANTLR4 Integration
echo   - Official Modules Support
echo.
echo Ready for distribution!
echo ================================================

pause