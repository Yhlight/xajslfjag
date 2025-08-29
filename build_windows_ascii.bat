@echo off
echo === CHTL Windows Build Script ===

REM Check if we are in correct directory
if not exist "src" (
    echo ERROR: Please run this script from CHTL project root directory
    echo Current directory should contain 'src' folder
    pause
    exit /b 1
)

REM Backup original CMakeLists.txt
if exist "CMakeLists.txt" (
    copy CMakeLists.txt CMakeLists_backup.txt > nul
    echo Backed up original CMakeLists.txt
)

REM Use Windows fixed configuration
copy CMakeLists_Windows_Fixed.txt CMakeLists.txt > nul
echo Using Windows fixed configuration file

REM Create build directory
if not exist "build_windows" (
    mkdir build_windows
    echo Created build directory: build_windows
)

cd build_windows

echo.
echo Configuring CMake project (Visual Studio 2022)...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed
    cd ..
    echo Restoring original configuration...
    if exist "CMakeLists_backup.txt" (
        copy CMakeLists_backup.txt CMakeLists.txt > nul
    )
    pause
    exit /b 1
)

echo.
echo Building project...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo ERROR: Project build failed
    cd ..
    echo Restoring original configuration...
    if exist "CMakeLists_backup.txt" (
        copy CMakeLists_backup.txt CMakeLists.txt > nul
    )
    pause
    exit /b 1
)

echo.
echo SUCCESS: CHTL Windows build completed!
echo Output file location: build_windows\Release\chtl_core.lib

cd ..

REM Restore original configuration
if exist "CMakeLists_backup.txt" (
    copy CMakeLists_backup.txt CMakeLists.txt > nul
    del CMakeLists_backup.txt > nul
    echo Restored original configuration file
)

echo.
echo Now you can compile and test CHTL functionality
echo.
echo Compile test program:
echo cl windows_test_fixed.cpp /I"src" /std:c++17 build_windows\Release\chtl_core.lib
echo.
pause