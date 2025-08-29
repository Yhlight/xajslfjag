@echo off
echo === CHTL Windows构建脚本 ===

REM 设置编码为UTF-8
chcp 65001 > nul

REM 创建构建目录
if not exist "build_windows" (
    mkdir build_windows
    echo 创建构建目录: build_windows
)

cd build_windows

echo.
echo 配置CMake项目 (Windows)...
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release -f ../CMakeLists_Windows.txt
if %ERRORLEVEL% neq 0 (
    echo 错误: CMake配置失败
    pause
    exit /b 1
)

echo.
echo 构建项目...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo 错误: 项目构建失败
    pause
    exit /b 1
)

echo.
echo ✓ CHTL Windows构建完成!
echo 输出文件位置: build_windows\Release\chtl_core.lib

cd ..
echo.
echo 现在可以测试CHTL功能了
pause