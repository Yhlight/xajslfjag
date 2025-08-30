@echo off
REM CMOD Packaging Script for Windows
REM Package CMOD modules according to CHTL syntax documentation

echo 📦 CMOD Module Packaging Script (Windows)
echo =========================================

REM Get script directory and project root
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%\..\..\

cd /d "%PROJECT_ROOT%"

REM Check if Module directory exists
if not exist "src\Module" (
    echo ❌ Module directory not found: src\Module
    exit /b 1
)

cd src\Module

REM Create output directory for packaged modules
if not exist "..\..\packages" mkdir "..\..\packages"

echo 🔍 Scanning for CMOD modules...

REM Process each module directory
for /d %%D in (*) do (
    set "module_name=%%D"
    echo.
    echo 📁 Processing module: %%D
    
    REM Check for CMOD structure
    if exist "%%D\CMOD" (
        echo ✅ Found CMOD structure in %%D
        
        REM Check for info directory and .chtl file
        if exist "%%D\CMOD\%%D\info\%%D.chtl" (
            echo ✅ Found info file: %%D\CMOD\%%D\info\%%D.chtl
            
            REM Check for src directory
            if exist "%%D\CMOD\%%D\src" (
                echo ✅ Found src directory: %%D\CMOD\%%D\src
                
                REM Create CMOD package
                echo 📦 Creating CMOD package for %%D...
                
                REM Create temporary directory for packaging
                set "temp_dir=..\..\temp_%%D_cmod"
                if exist "!temp_dir!" rmdir /s /q "!temp_dir!"
                mkdir "!temp_dir!"
                
                REM Copy CMOD structure
                xcopy "%%D\CMOD" "!temp_dir!" /E /I /Q
                
                REM Create ZIP archive (requires PowerShell)
                echo 🗜️ Creating ZIP archive...
                powershell -Command "Compress-Archive -Path '!temp_dir!\*' -DestinationPath '..\..\packages\%%D.cmod' -Force"
                
                if exist "..\..\packages\%%D.cmod" (
                    echo ✅ Successfully created: packages\%%D.cmod
                ) else (
                    echo ❌ Failed to create package for %%D
                )
                
                REM Cleanup
                rmdir /s /q "!temp_dir!"
            ) else (
                echo ❌ No src directory found in %%D\CMOD\%%D\
            )
        ) else (
            echo ❌ No info file found: %%D\CMOD\%%D\info\%%D.chtl
        )
    ) else (
        echo ℹ️ No CMOD structure found in %%D
    )
)

echo.
echo 🎉 CMOD packaging completed!
echo 📁 Packaged modules saved to: packages\

cd /d "%SCRIPT_DIR%"
pause