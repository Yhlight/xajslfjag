@echo off
REM Universal Build Script for Windows
REM Build compiler, package modules, and build VSCode extension

echo 🔨 CHTL Universal Build Script (Windows)
echo ========================================

REM Get script directory and project root
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%\..\..\

cd /d "%PROJECT_ROOT%"

echo.
echo 🏗️ Step 1: Building CHTL Compiler (Release)...
echo ================================================
call "%SCRIPT_DIR%\build_release.bat"
if errorlevel 1 (
    echo ❌ Compiler build failed!
    exit /b 1
)

echo.
echo 📦 Step 2: Packaging CMOD modules...
echo ====================================
call "%SCRIPT_DIR%\package_cmod.bat"
if errorlevel 1 (
    echo ❌ CMOD packaging failed!
    exit /b 1
)

echo.
echo ⚡ Step 3: Packaging CJMOD modules...
echo =====================================
call "%SCRIPT_DIR%\package_cjmod.bat"
if errorlevel 1 (
    echo ❌ CJMOD packaging failed!
    exit /b 1
)

echo.
echo 🧩 Step 4: Building VSCode Extension...
echo =======================================
call "%SCRIPT_DIR%\build_vscode_extension.bat"
if errorlevel 1 (
    echo ❌ VSCode extension build failed!
    exit /b 1
)

echo.
echo 🎉 Universal Build Completed Successfully!
echo =========================================
echo ✅ Compiler built and ready
echo ✅ Modules packaged
echo ✅ VSCode extension built
echo.
echo 📁 Output locations:
echo   - Compiler: build\Release\
echo   - Modules: packages\
echo   - VSCode Extension: vscode-extension\chtl-extension-*.vsix

pause