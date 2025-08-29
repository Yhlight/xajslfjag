@echo off
echo === CHTL Windows构建脚本 (修正版) ===

REM 设置编码为UTF-8
chcp 65001 > nul

REM 检查当前目录
if not exist "src" (
    echo 错误: 请在CHTL项目根目录运行此脚本
    echo 当前目录应包含 src 文件夹
    pause
    exit /b 1
)

REM 备份原始CMakeLists.txt
if exist "CMakeLists.txt" (
    copy CMakeLists.txt CMakeLists_backup.txt > nul
    echo 已备份原始 CMakeLists.txt
)

REM 使用修复版配置
copy CMakeLists_Windows_Fixed.txt CMakeLists.txt > nul
echo 使用Windows修复版配置文件

REM 创建构建目录
if not exist "build_windows" (
    mkdir build_windows
    echo 创建构建目录: build_windows
)

cd build_windows

echo.
echo 配置CMake项目 (Visual Studio 2022)...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo 错误: CMake配置失败
    cd ..
    echo 恢复原始配置文件...
    if exist "CMakeLists_backup.txt" (
        copy CMakeLists_backup.txt CMakeLists.txt > nul
    )
    pause
    exit /b 1
)

echo.
echo 构建项目...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo 错误: 项目构建失败
    cd ..
    echo 恢复原始配置文件...
    if exist "CMakeLists_backup.txt" (
        copy CMakeLists_backup.txt CMakeLists.txt > nul
    )
    pause
    exit /b 1
)

echo.
echo ✓ CHTL Windows构建完成!
echo 输出文件位置: build_windows\Release\chtl_core.lib

cd ..

REM 恢复原始配置文件
if exist "CMakeLists_backup.txt" (
    copy CMakeLists_backup.txt CMakeLists.txt > nul
    del CMakeLists_backup.txt > nul
    echo 已恢复原始配置文件
)

echo.
echo 现在可以编译和测试CHTL功能了
echo.
echo 编译测试程序:
echo cl windows_test_fixed.cpp /I"src" /std:c++17 build_windows\Release\chtl_core.lib
echo.
pause