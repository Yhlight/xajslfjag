@echo off
REM CHTL Compiler Debug Build Script for Windows
REM Build the complete CHTL compiler in debug mode

echo ================================================
echo CHTL Compiler Debug Build for Windows
echo ================================================

cd /d "%~dp0..\.."

REM Check if src directory exists
if not exist "src" (
    echo Error: src directory not found
    exit /b 1
)

cd src

REM Create build directory
if not exist "build_debug" mkdir build_debug
cd build_debug

echo Configuring CMake for Debug build...
cmake .. -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 16 2019" -A x64

if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed
    exit /b 1
)

echo Building CHTL Compiler in Debug mode...
cmake --build . --config Debug --parallel 4

if %ERRORLEVEL% neq 0 (
    echo Build failed
    exit /b 1
)

echo ================================================
echo CHTL Compiler Debug Build Completed Successfully
echo ================================================
echo Binaries location: %CD%\Debug\
echo.
echo Built components:
echo   - chtl_compiler.exe (Debug)
echo   - CHTL Core Libraries (Debug)
echo   - Debug symbols included
echo   - Ready for development and debugging
echo ================================================

pause