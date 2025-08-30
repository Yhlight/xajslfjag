# CHTL Release Build Script for Windows
# This script builds CHTL in release mode with optimizations on Windows

param(
    [switch]$Help,
    [string]$Generator = "Visual Studio 16 2019",
    [string]$Platform = "x64",
    [string]$BuildDir = "build_release"
)

# Show usage if help requested
if ($Help) {
    Write-Host "CHTL Release Build Script for Windows" -ForegroundColor Blue
    Write-Host "======================================"
    Write-Host ""
    Write-Host "Usage: .\build_release.ps1 [options]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -Generator    CMake generator (default: 'Visual Studio 16 2019')"
    Write-Host "  -Platform     Build platform (default: 'x64')"
    Write-Host "  -BuildDir     Build directory (default: 'build_release')"
    Write-Host "  -Help         Show this help message"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\build_release.ps1"
    Write-Host "  .\build_release.ps1 -Generator 'Visual Studio 17 2022'"
    Write-Host "  .\build_release.ps1 -Generator 'MinGW Makefiles'"
    exit 0
}

# Colors for output
function Write-Success { param($Message) Write-Host $Message -ForegroundColor Green }
function Write-Warning { param($Message) Write-Host $Message -ForegroundColor Yellow }
function Write-Error { param($Message) Write-Host $Message -ForegroundColor Red }
function Write-Info { param($Message) Write-Host $Message -ForegroundColor Cyan }

Write-Host "CHTL Release Build Script for Windows" -ForegroundColor Blue
Write-Host "======================================"

# Check PowerShell version
if ($PSVersionTable.PSVersion.Major -lt 5) {
    Write-Error "ERROR: PowerShell 5.0 or higher is required"
    exit 1
}

# Check dependencies
Write-Warning "Checking dependencies..."

# Check CMake
try {
    $cmakeVersion = cmake --version 2>$null
    if ($LASTEXITCODE -eq 0) {
        Write-Success "✓ CMake found: $($cmakeVersion[0])"
        
        # Extract version number
        $versionMatch = [regex]::Match($cmakeVersion[0], "(\d+\.\d+)")
        if ($versionMatch.Success) {
            $version = [version]$versionMatch.Groups[1].Value
            if ($version -lt [version]"3.15") {
                Write-Error "✗ CMake version too old (need 3.15+, found $version)"
                exit 1
            }
        }
    } else {
        Write-Error "✗ CMake not found. Please install CMake 3.15+"
        Write-Info "Download from: https://cmake.org/download/"
        exit 1
    }
} catch {
    Write-Error "✗ CMake not found or not in PATH"
    exit 1
}

# Check compiler
$compilerFound = $false

# Check for Visual Studio
if ($Generator -like "*Visual Studio*") {
    $vsInstances = Get-ChildItem "HKLM:\SOFTWARE\Microsoft\VisualStudio" -ErrorAction SilentlyContinue
    if ($vsInstances) {
        Write-Success "✓ Visual Studio detected"
        $compilerFound = $true
    } else {
        # Check for Build Tools
        $buildTools = Get-ChildItem "HKLM:\SOFTWARE\Microsoft\VisualStudio\*\Setup\Reboot" -ErrorAction SilentlyContinue
        if ($buildTools) {
            Write-Success "✓ Visual Studio Build Tools detected"
            $compilerFound = $true
        }
    }
}

# Check for MinGW
if (-not $compilerFound -or $Generator -like "*MinGW*") {
    try {
        $gccVersion = gcc --version 2>$null
        if ($LASTEXITCODE -eq 0) {
            Write-Success "✓ MinGW GCC found: $($gccVersion[0])"
            $compilerFound = $true
            if ($Generator -like "*Visual Studio*") {
                Write-Warning "Both Visual Studio and MinGW found. Using Visual Studio."
            }
        }
    } catch {
        # MinGW not found, continue
    }
}

if (-not $compilerFound) {
    Write-Error "✗ No suitable C++ compiler found"
    Write-Info "Please install:"
    Write-Info "  - Visual Studio 2017+ with C++ tools, or"
    Write-Info "  - Visual Studio Build Tools, or"
    Write-Info "  - MinGW-w64"
    exit 1
}

Write-Success "Dependencies check passed"

# Clean previous build
if (Test-Path $BuildDir) {
    Write-Warning "Cleaning previous build..."
    Remove-Item -Recurse -Force $BuildDir
}

# Create build directory
Write-Warning "Creating build directory..."
New-Item -ItemType Directory -Path $BuildDir | Out-Null
Set-Location $BuildDir

# Configure with CMake
Write-Warning "Configuring with CMake..."
$cmakeArgs = @(
    ".."
    "-G"
    $Generator
    "-DCMAKE_BUILD_TYPE=Release"
    "-DCMAKE_CXX_STANDARD=17"
    "-DBUILD_SHARED_LIBS=ON"
    "-DENABLE_TESTING=OFF"
    "-DENABLE_BENCHMARKS=OFF"
)

if ($Platform) {
    $cmakeArgs += "-A"
    $cmakeArgs += $Platform
}

# Add Windows-specific optimizations
if ($Generator -like "*Visual Studio*") {
    $cmakeArgs += "-DCMAKE_CXX_FLAGS_RELEASE=/O2 /DNDEBUG /MP"
} else {
    $cmakeArgs += "-DCMAKE_CXX_FLAGS_RELEASE=-O3 -DNDEBUG -march=native"
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
Write-Warning "Building CHTL..."
try {
    if ($Generator -like "*Visual Studio*") {
        # Use MSBuild for Visual Studio
        & cmake --build . --config Release --parallel
    } else {
        # Use make for MinGW
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

# Run basic tests if available
$testExecutable = ""
if ($Generator -like "*Visual Studio*") {
    $testExecutable = "Release\chtl_tests.exe"
} else {
    $testExecutable = "chtl_tests.exe"
}

if (Test-Path $testExecutable) {
    Write-Warning "Running basic tests..."
    try {
        & .\$testExecutable
        if ($LASTEXITCODE -eq 0) {
            Write-Success "✓ Tests passed"
        } else {
            Write-Warning "⚠ Some tests failed"
        }
    } catch {
        Write-Warning "⚠ Could not run tests"
    }
}

# Find the main executable
$executable = ""
if ($Generator -like "*Visual Studio*") {
    $executable = "src\Release\chtl.exe"
} else {
    $executable = "src\chtl.exe"
}

Write-Success "Release build completed successfully!"

if (Test-Path $executable) {
    Write-Info "Executable location: $(Resolve-Path $executable)"
} else {
    Write-Warning "Main executable not found at expected location"
}

# Create package info
Write-Warning "Creating package info..."

$gitVersion = ""
try {
    $gitVersion = git describe --tags --always 2>$null
    if ($LASTEXITCODE -ne 0) { $gitVersion = "dev" }
} catch {
    $gitVersion = "dev"
}

$packageInfo = @"
CHTL Release Build Information (Windows)
========================================
Build Date: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Build Type: Release
Generator: $Generator
Platform: $Platform
Compiler: $(if ($Generator -like "*Visual Studio*") { "MSVC" } else { "GCC" })
Optimization: $(if ($Generator -like "*Visual Studio*") { "/O2" } else { "-O3 -march=native" })
Binary: $(if (Test-Path $executable) { Resolve-Path $executable } else { "Not found" })
Version: $gitVersion

Installation:
Copy the executable and required DLLs to your desired location.

Usage:
chtl.exe input.chtl -o output.html

Dependencies:
Make sure Visual C++ Redistributable is installed if using MSVC build.
"@

$packageInfo | Out-File -FilePath "CHTL_RELEASE_INFO.txt" -Encoding UTF8

Write-Success "Build information saved to CHTL_RELEASE_INFO.txt"

# Return to original directory
Set-Location ..

Write-Host ""
Write-Success "✓ Release build process completed!"
Write-Info "Build directory: $BuildDir"
Write-Info "To clean build files, run: .\scripts\clean_build.ps1"