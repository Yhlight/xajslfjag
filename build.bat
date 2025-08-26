@echo off
REM CHTL编译器构建脚本 (Windows)

setlocal enabledelayedexpansion

REM 设置编码为UTF-8
chcp 65001 > nul

REM 构建类型（默认为Release）
set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

echo CHTL编译器构建脚本
echo 构建类型: %BUILD_TYPE%

REM 创建构建目录
set BUILD_DIR=build-%BUILD_TYPE%
if not exist %BUILD_DIR% mkdir %BUILD_DIR%

REM 进入构建目录
cd %BUILD_DIR%

REM 检测Visual Studio
where cl >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误: 未检测到Visual Studio编译器！
    echo 请在Visual Studio命令提示符中运行此脚本。
    exit /b 1
)

REM 运行CMake
echo 配置项目...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if %errorlevel% neq 0 (
    echo CMake配置失败！
    exit /b 1
)

REM 编译项目
echo 编译项目...
cmake --build . --config %BUILD_TYPE% --parallel

if %errorlevel% neq 0 (
    echo 编译失败！
    exit /b 1
)

echo 编译成功！
echo 可执行文件位置: %BUILD_DIR%\bin\%BUILD_TYPE%\chtl.exe

REM 创建模块目录
if not exist bin\%BUILD_TYPE%\module\CMOD mkdir bin\%BUILD_TYPE%\module\CMOD
if not exist bin\%BUILD_TYPE%\module\CJMOD mkdir bin\%BUILD_TYPE%\module\CJMOD

echo 构建完成！