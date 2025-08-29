# CHTL Environment Checker for Windows
# This script checks if the Windows development environment is properly set up

param(
    [switch]$Detailed,
    [switch]$Help
)

if ($Help) {
    Write-Host "CHTL Environment Checker for Windows" -ForegroundColor Blue
    Write-Host "====================================="
    Write-Host ""
    Write-Host "Usage: .\check_environment.ps1 [options]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -Detailed    Show detailed information about each component"
    Write-Host "  -Help        Show this help message"
    exit 0
}

# Colors for output
function Write-Success { param($Message) Write-Host "✓ $Message" -ForegroundColor Green }
function Write-Warning { param($Message) Write-Host "⚠ $Message" -ForegroundColor Yellow }
function Write-Error { param($Message) Write-Host "✗ $Message" -ForegroundColor Red }
function Write-Info { param($Message) Write-Host "ℹ $Message" -ForegroundColor Cyan }

Write-Host "CHTL Environment Checker for Windows" -ForegroundColor Blue
Write-Host "====================================="
Write-Host ""

$allGood = $true

# Check PowerShell version
Write-Host "Checking PowerShell..." -ForegroundColor Cyan
if ($PSVersionTable.PSVersion.Major -ge 5) {
    Write-Success "PowerShell $($PSVersionTable.PSVersion) found"
    if ($Detailed) {
        Write-Info "  Edition: $($PSVersionTable.PSEdition)"
        Write-Info "  Platform: $($PSVersionTable.Platform)"
    }
} else {
    Write-Error "PowerShell version too old (need 5.0+, found $($PSVersionTable.PSVersion))"
    $allGood = $false
}

# Check Windows version
Write-Host "`nChecking Windows version..." -ForegroundColor Cyan
$osInfo = Get-CimInstance -ClassName Win32_OperatingSystem
$windowsVersion = [System.Environment]::OSVersion.Version

if ($windowsVersion.Major -ge 10) {
    Write-Success "Windows $($osInfo.Caption) (Build $($osInfo.BuildNumber))"
    if ($Detailed) {
        Write-Info "  Architecture: $($osInfo.OSArchitecture)"
        Write-Info "  Total Memory: $([math]::Round($osInfo.TotalVisibleMemorySize / 1MB, 2))GB"
    }
} elseif ($windowsVersion.Major -eq 6 -and $windowsVersion.Minor -ge 1) {
    Write-Warning "Windows $($osInfo.Caption) - supported but Windows 10+ recommended"
} else {
    Write-Error "Windows version too old - Windows 7+ required"
    $allGood = $false
}

# Check C++ compiler
Write-Host "`nChecking C++ compilers..." -ForegroundColor Cyan

# Check for Visual Studio
$vsFound = $false
$vsVersions = @()

# Check registry for VS installations
$vsKeys = @(
    "HKLM:\SOFTWARE\Microsoft\VisualStudio\SxS\VS7",
    "HKLM:\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\SxS\VS7"
)

foreach ($key in $vsKeys) {
    try {
        $vsEntries = Get-ItemProperty -Path $key -ErrorAction SilentlyContinue
        if ($vsEntries) {
            foreach ($property in $vsEntries.PSObject.Properties) {
                if ($property.Name -match "^\d+\.\d+$" -and $property.Value) {
                    $version = $property.Name
                    $path = $property.Value
                    if (Test-Path $path) {
                        $vsVersions += @{ Version = $version; Path = $path }
                        $vsFound = $true
                    }
                }
            }
        }
    } catch {
        # Continue checking
    }
}

# Check for VS 2017+ using vswhere
try {
    $vswherePath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vswherePath) {
        $vsInstances = & $vswherePath -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -format json | ConvertFrom-Json
        if ($vsInstances) {
            foreach ($instance in $vsInstances) {
                $vsVersions += @{ 
                    Version = $instance.catalog.productSemanticVersion; 
                    Path = $instance.installationPath;
                    DisplayName = $instance.displayName
                }
                $vsFound = $true
            }
        }
    }
} catch {
    # vswhere not available or failed
}

if ($vsFound) {
    foreach ($vs in $vsVersions) {
        Write-Success "Visual Studio found: $($vs.DisplayName)"
        if ($Detailed) {
            Write-Info "  Version: $($vs.Version)"
            Write-Info "  Path: $($vs.Path)"
        }
    }
} else {
    Write-Warning "Visual Studio not found"
}

# Check for MinGW
$mingwFound = $false
try {
    $gccVersion = gcc --version 2>$null
    if ($LASTEXITCODE -eq 0) {
        Write-Success "MinGW GCC found: $($gccVersion[0])"
        $mingwFound = $true
        
        if ($Detailed) {
            $gccPath = (Get-Command gcc -ErrorAction SilentlyContinue).Source
            Write-Info "  Path: $gccPath"
        }
    }
} catch {
    # GCC not found
}

if (-not $mingwFound) {
    Write-Warning "MinGW not found (optional)"
}

if (-not $vsFound -and -not $mingwFound) {
    Write-Error "No C++ compiler found - install Visual Studio or MinGW"
    $allGood = $false
}

# Check CMake
Write-Host "`nChecking CMake..." -ForegroundColor Cyan
try {
    $cmakeVersion = cmake --version 2>$null
    if ($LASTEXITCODE -eq 0) {
        $versionLine = $cmakeVersion[0]
        Write-Success "CMake found: $versionLine"
        
        # Check version
        $versionMatch = [regex]::Match($versionLine, "(\d+\.\d+\.\d+)")
        if ($versionMatch.Success) {
            $version = [version]$versionMatch.Groups[1].Value
            if ($version -lt [version]"3.15.0") {
                Write-Warning "CMake version may be too old (need 3.15+, found $version)"
            }
        }
        
        if ($Detailed) {
            $cmakePath = (Get-Command cmake -ErrorAction SilentlyContinue).Source
            Write-Info "  Path: $cmakePath"
        }
    } else {
        Write-Error "CMake not found or not working"
        $allGood = $false
    }
} catch {
    Write-Error "CMake not found in PATH"
    Write-Info "Download from: https://cmake.org/download/"
    $allGood = $false
}

# Check Git
Write-Host "`nChecking Git..." -ForegroundColor Cyan
try {
    $gitVersion = git --version 2>$null
    if ($LASTEXITCODE -eq 0) {
        Write-Success "Git found: $gitVersion"
        
        if ($Detailed) {
            $gitPath = (Get-Command git -ErrorAction SilentlyContinue).Source
            Write-Info "  Path: $gitPath"
        }
    } else {
        Write-Warning "Git not found (recommended for development)"
    }
} catch {
    Write-Warning "Git not found in PATH"
}

# Check optional dependencies
Write-Host "`nChecking optional dependencies..." -ForegroundColor Cyan

# Check for ANTLR4
$antlrFound = $false
try {
    # Check if antlr4 is in PATH
    $antlrOutput = antlr4 2>$null
    if ($LASTEXITCODE -eq 0) {
        Write-Success "ANTLR4 command-line tool found"
        $antlrFound = $true
    }
} catch {
    # Not found via command
}

# Check for ANTLR4 runtime in common locations
$antlrPaths = @(
    "${env:ProgramFiles}\antlr4",
    "${env:ProgramFiles(x86)}\antlr4",
    "C:\antlr4"
)

foreach ($path in $antlrPaths) {
    if (Test-Path "$path\include\antlr4-runtime") {
        Write-Success "ANTLR4 C++ runtime found at $path"
        $antlrFound = $true
        break
    }
}

if (-not $antlrFound) {
    Write-Warning "ANTLR4 not found (optional but recommended)"
    if ($Detailed) {
        Write-Info "  Used for CSS and JavaScript parsing"
        Write-Info "  Download from: https://www.antlr.org/"
    }
}

# Check for debugging tools
Write-Host "`nChecking debugging tools..." -ForegroundColor Cyan

# Check for Windows Debugger
if (Get-Command "cdb.exe" -ErrorAction SilentlyContinue) {
    Write-Success "Windows Debugger (CDB) found"
} else {
    Write-Warning "Windows Debugger not found (install Windows SDK)"
}

# Check for Application Verifier
$appVerifierPath = "${env:SystemRoot}\System32\appverif.exe"
if (Test-Path $appVerifierPath) {
    Write-Success "Application Verifier found"
} else {
    Write-Warning "Application Verifier not found (part of Windows SDK)"
}

# Check system resources
Write-Host "`nChecking system resources..." -ForegroundColor Cyan

# Check available memory
$totalMemoryGB = [math]::Round($osInfo.TotalVisibleMemorySize / 1MB, 2)
$freeMemoryGB = [math]::Round($osInfo.FreePhysicalMemory / 1MB, 2)

if ($totalMemoryGB -ge 8) {
    Write-Success "Total memory: ${totalMemoryGB}GB (sufficient)"
} elseif ($totalMemoryGB -ge 4) {
    Write-Warning "Total memory: ${totalMemoryGB}GB (minimum, 8GB+ recommended)"
} else {
    Write-Error "Total memory: ${totalMemoryGB}GB (insufficient, 4GB+ required)"
    $allGood = $false
}

if ($Detailed) {
    Write-Info "  Free memory: ${freeMemoryGB}GB"
}

# Check available disk space
$systemDrive = Get-CimInstance -ClassName Win32_LogicalDisk | Where-Object { $_.DriveType -eq 3 -and $_.DeviceID -eq $env:SystemDrive }
if ($systemDrive) {
    $freeSpaceGB = [math]::Round($systemDrive.FreeSpace / 1GB, 2)
    
    if ($freeSpaceGB -ge 10) {
        Write-Success "Free disk space: ${freeSpaceGB}GB (sufficient)"
    } elseif ($freeSpaceGB -ge 5) {
        Write-Warning "Free disk space: ${freeSpaceGB}GB (low, 10GB+ recommended)"
    } else {
        Write-Error "Free disk space: ${freeSpaceGB}GB (insufficient, 5GB+ required)"
        $allGood = $false
    }
}

# Check CPU
$cpu = Get-CimInstance -ClassName Win32_Processor | Select-Object -First 1
Write-Info "CPU: $($cpu.Name) ($($cpu.NumberOfCores) cores, $($cpu.NumberOfLogicalProcessors) threads)"

# Final summary
Write-Host "`n" + "="*50 -ForegroundColor Blue
Write-Host "Environment Check Summary" -ForegroundColor Blue
Write-Host "="*50 -ForegroundColor Blue

if ($allGood) {
    Write-Success "All required components found - environment is ready for CHTL development!"
} else {
    Write-Error "Some required components are missing - please install them before building CHTL"
}

Write-Host ""
Write-Info "To build CHTL:"
Write-Info "  Release build: .\scripts\build_release.ps1"
Write-Info "  Debug build:   .\scripts\build_debug.ps1"
Write-Info "  Clean build:   .\scripts\clean_build.ps1"

Write-Host ""
Write-Info "For help with any component, run: .\scripts\check_environment.ps1 -Detailed"