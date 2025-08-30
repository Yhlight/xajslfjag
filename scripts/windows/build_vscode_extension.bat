@echo off
REM VSCode Extension Build Script for Windows

echo ================================================
echo CHTL VSCode Extension Build for Windows
echo ================================================

cd /d "%~dp0..\.."

REM Check if vscode-extension directory exists
if not exist "vscode-extension" (
    echo Error: vscode-extension directory not found
    exit /b 1
)

cd vscode-extension

echo Installing Node.js dependencies...
call npm install

if %ERRORLEVEL% neq 0 (
    echo npm install failed
    exit /b 1
)

echo Compiling TypeScript...
call npm run compile

if %ERRORLEVEL% neq 0 (
    echo TypeScript compilation failed
    exit /b 1
)

echo Copying compiler binaries...
if not exist "bin" mkdir bin
if not exist "modules" mkdir modules

REM Copy compiler binary
if exist "..\src\build_release\Release\chtl_compiler.exe" (
    copy "..\src\build_release\Release\chtl_compiler.exe" bin\
    echo   - Compiler binary copied
) else if exist "..\src\build\Release\chtl_compiler.exe" (
    copy "..\src\build\Release\chtl_compiler.exe" bin\
    echo   - Compiler binary copied (from build)
) else (
    echo   Warning: Compiler binary not found
)

REM Copy official modules
if exist "..\src\Module" (
    xcopy "..\src\Module\*" modules\ /E /I /Y
    echo   - Official modules copied
) else (
    echo   Warning: Official modules not found
)

echo Creating VSIX package...
call npx vsce package

if %ERRORLEVEL% neq 0 (
    echo VSCode extension packaging failed
    exit /b 1
)

REM Move VSIX to project root
for %%f in (*.vsix) do (
    move "%%f" "..\%%f"
    echo Extension package: ..\%%f
)

echo ================================================
echo VSCode Extension Build Completed Successfully
echo ================================================
echo Extension features:
echo   - CHTL syntax highlighting
echo   - Auto-completion with official modules
echo   - Compile and preview commands
echo   - Live server with real-time reload
echo   - Built-in compiler and modules
echo ================================================

pause