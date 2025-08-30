@echo off
REM CHTL Module Packaging Script for Windows
REM Package CMOD and CJMOD modules

echo ================================================
echo CHTL Module Packaging for Windows
echo ================================================

cd /d "%~dp0..\.."

REM Check if src directory exists
if not exist "src" (
    echo Error: src directory not found
    exit /b 1
)

REM Create packages directory
if not exist "packages" mkdir packages

echo Packaging CMOD modules...
call scripts\package_cmod.sh

echo Packaging CJMOD modules...
call scripts\package_cjmod.sh

echo ================================================
echo Module Packaging Completed
echo ================================================
echo Packaged modules location: packages\
dir packages\*.cmod packages\*.cjmod
echo ================================================

pause