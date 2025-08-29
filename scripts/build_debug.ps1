# CHTL Debug Build Script for Windows
# This script builds CHTL in debug mode with debugging symbols

param(
    [switch]$Help,
    [string]$Generator = "Visual Studio 16 2019",
    [string]$Platform = "x64",
    [string]$BuildDir = "build_debug",
    [switch]$EnableSanitizers,
    [switch]$EnableCoverage
)

# Show usage if help requested
if ($Help) {
    Write-Host "CHTL Debug Build Script for Windows" -ForegroundColor Blue
    Write-Host "===================================="
    Write-Host ""
    Write-Host "Usage: .\build_debug.ps1 [options]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -Generator         CMake generator (default: 'Visual Studio 16 2019')"
    Write-Host "  -Platform          Build platform (default: 'x64')"
    Write-Host "  -BuildDir          Build directory (default: 'build_debug')"
    Write-Host "  -EnableSanitizers  Enable AddressSanitizer (MSVC 2019+ only)"
    Write-Host "  -EnableCoverage    Enable code coverage"
    Write-Host "  -Help              Show this help message"
    exit 0
}

# Colors for output
function Write-Success { param($Message) Write-Host $Message -ForegroundColor Green }
function Write-Warning { param($Message) Write-Host $Message -ForegroundColor Yellow }
function Write-Error { param($Message) Write-Host $Message -ForegroundColor Red }
function Write-Info { param($Message) Write-Host $Message -ForegroundColor Cyan }

Write-Host "CHTL Debug Build Script for Windows" -ForegroundColor Blue
Write-Host "===================================="

# Check dependencies
Write-Warning "Checking dependencies..."

# Check CMake
try {
    $cmakeVersion = cmake --version 2>$null
    if ($LASTEXITCODE -eq 0) {
        Write-Success "✓ CMake found: $($cmakeVersion[0])"
    } else {
        Write-Error "✗ CMake not found"
        exit 1
    }
} catch {
    Write-Error "✗ CMake not found or not in PATH"
    exit 1
}

# Check for debugging tools
$debugToolsInfo = @()

# Check for Visual Studio Debugger
if (Get-Command "devenv.exe" -ErrorAction SilentlyContinue) {
    Write-Success "✓ Visual Studio IDE found"
    $debugToolsInfo += "Visual Studio Debugger"
}

# Check for standalone debuggers
if (Get-Command "cdb.exe" -ErrorAction SilentlyContinue) {
    Write-Success "✓ Windows Debugger (CDB) found"
    $debugToolsInfo += "CDB (Console Debugger)"
}

# Check for Intel Inspector (optional)
if (Get-Command "inspxe-cl.exe" -ErrorAction SilentlyContinue) {
    Write-Success "✓ Intel Inspector found"
    $debugToolsInfo += "Intel Inspector"
}

# Check for Application Verifier
$appVerifierPath = "${env:SystemRoot}\System32\appverif.exe"
if (Test-Path $appVerifierPath) {
    Write-Success "✓ Application Verifier found"
    $debugToolsInfo += "Application Verifier"
}

if ($debugToolsInfo.Count -eq 0) {
    Write-Warning "⚠ No debugging tools found (optional but recommended)"
} else {
    Write-Info "Available debugging tools: $($debugToolsInfo -join ', ')"
}

Write-Success "Dependencies check passed"

# Clean previous build
if (Test-Path $BuildDir) {
    Write-Warning "Cleaning previous debug build..."
    Remove-Item -Recurse -Force $BuildDir
}

# Create build directory
Write-Warning "Creating debug build directory..."
New-Item -ItemType Directory -Path $BuildDir | Out-Null
Set-Location $BuildDir

# Configure with CMake for debug
Write-Warning "Configuring with CMake for debug..."

$cmakeArgs = @(
    ".."
    "-G"
    $Generator
    "-DCMAKE_BUILD_TYPE=Debug"
    "-DCMAKE_CXX_STANDARD=17"
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DENABLE_TESTING=ON"
    "-DENABLE_DEBUGGING=ON"
)

if ($Platform) {
    $cmakeArgs += "-A"
    $cmakeArgs += $Platform
}

# Debug flags
if ($Generator -like "*Visual Studio*") {
    $debugFlags = "/Zi /Od /DEBUG /DDEBUG"
    
    # Enable AddressSanitizer if requested (VS 2019+)
    if ($EnableSanitizers) {
        Write-Info "Enabling AddressSanitizer..."
        $debugFlags += " /fsanitize=address"
    }
    
    $cmakeArgs += "-DCMAKE_CXX_FLAGS_DEBUG=$debugFlags"
    $cmakeArgs += "-DCMAKE_C_FLAGS_DEBUG=$debugFlags"
} else {
    $debugFlags = "-g -O0 -Wall -Wextra -DDEBUG"
    
    # Enable sanitizers for GCC/Clang
    if ($EnableSanitizers) {
        Write-Info "Enabling sanitizers..."
        $debugFlags += " -fsanitize=address -fsanitize=undefined"
    }
    
    $cmakeArgs += "-DCMAKE_CXX_FLAGS_DEBUG=$debugFlags"
    $cmakeArgs += "-DCMAKE_C_FLAGS_DEBUG=$debugFlags"
}

# Enable coverage if requested
if ($EnableCoverage) {
    Write-Info "Enabling code coverage..."
    if ($Generator -like "*Visual Studio*") {
        # Use Visual Studio Code Coverage
        $cmakeArgs += "-DENABLE_CODE_COVERAGE=ON"
    } else {
        # Use gcov for GCC
        $cmakeArgs += "-DCMAKE_CXX_FLAGS_DEBUG=$debugFlags --coverage"
        $cmakeArgs += "-DCMAKE_EXE_LINKER_FLAGS_DEBUG=--coverage"
    }
}

try {
    & cmake @cmakeArgs
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed"
    }
} catch {
    Write-Error "ERROR: CMake configuration failed"
    Set-Location ..
    exit 1
}

# Build
Write-Warning "Building CHTL in debug mode..."
try {
    if ($Generator -like "*Visual Studio*") {
        & cmake --build . --config Debug --parallel
    } else {
        & cmake --build . --parallel
    }
    
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }
} catch {
    Write-Error "ERROR: Build failed"
    Set-Location ..
    exit 1
}

# Run tests if available
$testExecutable = ""
if ($Generator -like "*Visual Studio*") {
    $testExecutable = "Debug\chtl_tests.exe"
} else {
    $testExecutable = "chtl_tests.exe"
}

if (Test-Path $testExecutable) {
    Write-Warning "Running debug tests..."
    try {
        & .\$testExecutable
        if ($LASTEXITCODE -eq 0) {
            Write-Success "✓ Debug tests passed"
        } else {
            Write-Warning "⚠ Some tests failed (this is normal in debug mode)"
        }
    } catch {
        Write-Warning "⚠ Could not run tests"
    }
}

# Find the main executable
$executable = ""
if ($Generator -like "*Visual Studio*") {
    $executable = "src\Debug\chtl.exe"
} else {
    $executable = "src\chtl.exe"
}

Write-Success "Debug build completed successfully!"

if (Test-Path $executable) {
    Write-Info "Debug executable location: $(Resolve-Path $executable)"
} else {
    Write-Warning "Main executable not found at expected location"
}

# Create debug helper scripts
Write-Warning "Creating debug helper scripts..."

# Visual Studio debug script
if ($Generator -like "*Visual Studio*" -and (Get-Command "devenv.exe" -ErrorAction SilentlyContinue)) {
    $vsDebugScript = @"
@echo off
echo Starting CHTL with Visual Studio Debugger...
echo Usage: Set breakpoints in Visual Studio, then run with arguments
devenv.exe $(Split-Path -Parent (Get-Location))\CHTL.sln
"@
    $vsDebugScript | Out-File -FilePath "debug_with_vs.bat" -Encoding ASCII
    Write-Success "✓ Created debug_with_vs.bat"
}

# CDB debug script
if (Get-Command "cdb.exe" -ErrorAction SilentlyContinue) {
    $cdbScript = @"
@echo off
if "%~1"=="" (
    echo Usage: %0 ^<input.chtl^> [output.html]
    exit /b 1
)

echo Starting CHTL with CDB debugger...
echo Common CDB commands:
echo   g          - Go (continue execution)
echo   bp main    - Set breakpoint at main
echo   bl         - List breakpoints
echo   k          - Show call stack
echo   q          - Quit

cdb.exe $executable %*
"@
    $cdbScript | Out-File -FilePath "debug_with_cdb.bat" -Encoding ASCII
    Write-Success "✓ Created debug_with_cdb.bat"
}

# Application Verifier script
if (Test-Path $appVerifierPath) {
    $appVerifScript = @"
@echo off
echo Enabling Application Verifier for CHTL...
appverif.exe /verify $executable
echo Application Verifier enabled. Run your program normally.
echo To disable: appverif.exe /reset $executable
pause
"@
    $appVerifScript | Out-File -FilePath "enable_app_verifier.bat" -Encoding ASCII
    Write-Success "✓ Created enable_app_verifier.bat"
}

# PowerShell debug session script
$psDebugScript = @"
# CHTL Debug Session Helper
# This script provides debugging utilities for CHTL development

Write-Host "CHTL Debug Session Helper" -ForegroundColor Blue
Write-Host "========================="

`$executable = "$executable"

function Start-CHTLDebug {
    param([string]`$InputFile, [string]`$OutputFile = "output.html")
    
    if (-not (Test-Path `$InputFile)) {
        Write-Error "Input file not found: `$InputFile"
        return
    }
    
    Write-Host "Starting CHTL debug session..." -ForegroundColor Yellow
    Write-Host "Input: `$InputFile" -ForegroundColor Cyan
    Write-Host "Output: `$OutputFile" -ForegroundColor Cyan
    
    & `$executable `$InputFile -o `$OutputFile --debug
}

function Show-CHTLHelp {
    Write-Host "Available debug functions:" -ForegroundColor Green
    Write-Host "  Start-CHTLDebug <input.chtl> [output.html]  - Run CHTL with debug output"
    Write-Host "  Get-CHTLVersion                             - Show CHTL version info"
    Write-Host "  Test-CHTLSyntax <file>                     - Test CHTL syntax only"
}

function Get-CHTLVersion {
    & `$executable --version
}

function Test-CHTLSyntax {
    param([string]`$InputFile)
    & `$executable `$InputFile --syntax-only
}

# Show help on startup
Show-CHTLHelp
"@
$psDebugScript | Out-File -FilePath "debug_session.ps1" -Encoding UTF8
Write-Success "✓ Created debug_session.ps1"

# Create debug info
$gitVersion = ""
try {
    $gitVersion = git describe --tags --always 2>$null
    if ($LASTEXITCODE -ne 0) { $gitVersion = "dev" }
} catch {
    $gitVersion = "dev"
}

$debugInfo = @"
CHTL Debug Build Information (Windows)
======================================
Build Date: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Build Type: Debug
Generator: $Generator
Platform: $Platform
Compiler: $(if ($Generator -like "*Visual Studio*") { "MSVC" } else { "GCC" })
Debug Flags: $(if ($Generator -like "*Visual Studio*") { "/Zi /Od /DEBUG" } else { "-g -O0 -Wall -Wextra" })
Sanitizers: $(if ($EnableSanitizers) { "Enabled" } else { "Disabled" })
Coverage: $(if ($EnableCoverage) { "Enabled" } else { "Disabled" })
Binary: $(if (Test-Path $executable) { Resolve-Path $executable } else { "Not found" })
Version: $gitVersion

Debug Tools Available:
$($debugToolsInfo -join "`n")

Debug Usage:
$executable input.chtl -o output.html --debug

Debugging Scripts:
$(if (Test-Path "debug_with_vs.bat") { "- debug_with_vs.bat (Visual Studio)" } else { "" })
$(if (Test-Path "debug_with_cdb.bat") { "- debug_with_cdb.bat (Console Debugger)" } else { "" })
$(if (Test-Path "enable_app_verifier.bat") { "- enable_app_verifier.bat (App Verifier)" } else { "" })
- debug_session.ps1 (PowerShell helper)

Compile Commands Database:
$(if (Test-Path "compile_commands.json") { "compile_commands.json generated for IDE integration" } else { "Not generated" })
"@

$debugInfo | Out-File -FilePath "CHTL_DEBUG_INFO.txt" -Encoding UTF8

Write-Success "Debug information saved to CHTL_DEBUG_INFO.txt"

# Return to original directory
Set-Location ..

Write-Host ""
Write-Success "✓ Debug build process completed!"
Write-Info "Debug Tips for Windows:"
Write-Info "- Use Visual Studio for best debugging experience"
Write-Info "- Enable Application Verifier for heap corruption detection"
Write-Info "- Use Performance Toolkit for advanced profiling"
if (Test-Path "$BuildDir\compile_commands.json") {
    Write-Info "- compile_commands.json available for IDE integration"
}
Write-Info ""
Write-Info "Build directory: $BuildDir"
Write-Info "To start debugging: cd $BuildDir && .\debug_session.ps1"