@echo off
echo === CHTL Core Build Script (No ANTLR Dependencies) ===

REM Check directory
if not exist "src" (
    echo ERROR: Please run from CHTL project root directory
    pause
    exit /b 1
)

REM Backup and use core configuration
if exist "CMakeLists.txt" (
    copy CMakeLists.txt CMakeLists_backup.txt > nul
)

copy CMakeLists_Core.txt CMakeLists.txt > nul
echo Using core-only configuration (no ANTLR dependencies)

REM Create build directory
if not exist "build_core" (
    mkdir build_core
)

cd build_core

echo.
echo Configuring CMake for core build...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed
    cd ..
    if exist "CMakeLists_backup.txt" (
        copy CMakeLists_backup.txt CMakeLists.txt > nul
    )
    pause
    exit /b 1
)

echo.
echo Building core library...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed
    cd ..
    if exist "CMakeLists_backup.txt" (
        copy CMakeLists_backup.txt CMakeLists.txt > nul
    )
    pause
    exit /b 1
)

echo.
echo SUCCESS: CHTL core library built!
echo Output: build_core\Release\chtl_core.lib

cd ..

REM Restore original
if exist "CMakeLists_backup.txt" (
    copy CMakeLists_backup.txt CMakeLists.txt > nul
    del CMakeLists_backup.txt > nul
)

echo.
echo Test compilation:
echo cl windows_test_fixed.cpp /I"src" /std:c++17 build_core\Release\chtl_core.lib
echo.
pause