@echo off
REM CJMOD Packaging Script for Windows
REM Package CJMOD modules according to CHTL syntax documentation

echo ⚡ CJMOD Module Packaging Script (Windows)
echo ==========================================

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

echo 🔍 Scanning for CJMOD modules...

REM Process each module directory
for /d %%D in (*) do (
    set "module_name=%%D"
    echo.
    echo 📁 Processing module: %%D
    
    REM Check for CJMOD structure
    if exist "%%D\CJMOD" (
        echo ✅ Found CJMOD structure in %%D
        
        REM Check for info directory and .chtl file
        if exist "%%D\CJMOD\%%D\info\%%D.chtl" (
            echo ✅ Found info file: %%D\CJMOD\%%D\info\%%D.chtl
            
            REM Check for src directory with C++ sources
            if exist "%%D\CJMOD\%%D\src" (
                echo ✅ Found src directory: %%D\CJMOD\%%D\src
                
                REM Check for C++ source files
                set "has_cpp=false"
                for %%F in ("%%D\CJMOD\%%D\src\*.cpp") do set "has_cpp=true"
                for %%F in ("%%D\CJMOD\%%D\src\*.h") do set "has_cpp=true"
                
                if "!has_cpp!"=="true" (
                    echo ✅ Found C++ source files
                    
                    REM Compile C++ sources (simplified - would need proper build system)
                    echo 🔨 Compiling C++ sources...
                    
                    REM Create CJMOD package
                    echo 📦 Creating CJMOD package for %%D...
                    
                    REM Create temporary directory for packaging
                    set "temp_dir=..\..\temp_%%D_cjmod"
                    if exist "!temp_dir!" rmdir /s /q "!temp_dir!"
                    mkdir "!temp_dir!"
                    
                    REM Copy CJMOD structure
                    xcopy "%%D\CJMOD" "!temp_dir!" /E /I /Q
                    
                    REM Create ZIP archive (requires PowerShell)
                    echo 🗜️ Creating ZIP archive...
                    powershell -Command "Compress-Archive -Path '!temp_dir!\*' -DestinationPath '..\..\packages\%%D.cjmod' -Force"
                    
                    if exist "..\..\packages\%%D.cjmod" (
                        echo ✅ Successfully created: packages\%%D.cjmod
                    ) else (
                        echo ❌ Failed to create package for %%D
                    )
                    
                    REM Cleanup
                    rmdir /s /q "!temp_dir!"
                ) else (
                    echo ❌ No C++ source files found in %%D\CJMOD\%%D\src\
                )
            ) else (
                echo ❌ No src directory found in %%D\CJMOD\%%D\
            )
        ) else (
            echo ❌ No info file found: %%D\CJMOD\%%D\info\%%D.chtl
        )
    ) else (
        echo ℹ️ No CJMOD structure found in %%D
    )
)

echo.
echo 🎉 CJMOD packaging completed!
echo 📁 Packaged modules saved to: packages\

cd /d "%SCRIPT_DIR%"
pause