# CHTL Dependencies Installation Script for Windows
# This script installs required dependencies for CHTL development on Windows

param(
    [switch]$All,
    [switch]$Chocolatey,
    [switch]$Vcpkg,
    [switch]$Manual,
    [switch]$SkipVS,
    [switch]$Help
)

if ($Help) {
    Write-Host "CHTL Dependencies Installation Script for Windows" -ForegroundColor Blue
    Write-Host "=================================================="
    Write-Host ""
    Write-Host "Usage: .\install_dependencies.ps1 [options]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -All         Install all dependencies using best available method"
    Write-Host "  -Chocolatey  Use Chocolatey package manager"
    Write-Host "  -Vcpkg       Use vcpkg for C++ libraries"
    Write-Host "  -Manual      Show manual installation instructions"
    Write-Host "  -SkipVS      Skip Visual Studio installation"
    Write-Host "  -Help        Show this help message"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\install_dependencies.ps1 -All"
    Write-Host "  .\install_dependencies.ps1 -Chocolatey"
    Write-Host "  .\install_dependencies.ps1 -Manual"
    exit 0
}

# Require Administrator privileges for installation
if (-NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Write-Host "This script requires Administrator privileges to install software." -ForegroundColor Red
    Write-Host "Please run PowerShell as Administrator and try again." -ForegroundColor Yellow
    exit 1
}

# Colors for output
function Write-Success { param($Message) Write-Host "✓ $Message" -ForegroundColor Green }
function Write-Warning { param($Message) Write-Host "⚠ $Message" -ForegroundColor Yellow }
function Write-Error { param($Message) Write-Host "✗ $Message" -ForegroundColor Red }
function Write-Info { param($Message) Write-Host "ℹ $Message" -ForegroundColor Cyan }
function Write-Step { param($Message) Write-Host "➤ $Message" -ForegroundColor Magenta }

Write-Host "CHTL Dependencies Installation Script for Windows" -ForegroundColor Blue
Write-Host "=================================================="
Write-Host ""

# Default to all if no specific method chosen
if (-not ($Chocolatey -or $Vcpkg -or $Manual)) {
    $All = $true
}

# Check if Chocolatey is available
function Test-Chocolatey {
    try {
        $chocoVersion = choco --version 2>$null
        return $LASTEXITCODE -eq 0
    } catch {
        return $false
    }
}

# Install Chocolatey if needed
function Install-Chocolatey {
    Write-Step "Installing Chocolatey package manager..."
    
    try {
        Set-ExecutionPolicy Bypass -Scope Process -Force
        [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
        Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
        
        # Refresh PATH
        $env:PATH = [System.Environment]::GetEnvironmentVariable("PATH", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("PATH", "User")
        
        if (Test-Chocolatey) {
            Write-Success "Chocolatey installed successfully"
            return $true
        } else {
            Write-Error "Chocolatey installation failed"
            return $false
        }
    } catch {
        Write-Error "Failed to install Chocolatey: $($_.Exception.Message)"
        return $false
    }
}

# Install dependencies using Chocolatey
function Install-WithChocolatey {
    Write-Step "Installing dependencies with Chocolatey..."
    
    # Check if Chocolatey is available
    if (-not (Test-Chocolatey)) {
        Write-Warning "Chocolatey not found. Installing Chocolatey first..."
        if (-not (Install-Chocolatey)) {
            Write-Error "Cannot proceed without Chocolatey"
            return $false
        }
    }
    
    $packages = @(
        "cmake",
        "git",
        "mingw"
    )
    
    if (-not $SkipVS) {
        # Note: Visual Studio Community is a large download, user might prefer manual installation
        Write-Warning "Visual Studio installation via Chocolatey will download ~3GB"
        $response = Read-Host "Install Visual Studio Community via Chocolatey? (y/N)"
        if ($response -eq 'y' -or $response -eq 'Y') {
            $packages += "visualstudio2019community"
            $packages += "visualstudio2019-workload-nativedesktop"
        }
    }
    
    foreach ($package in $packages) {
        Write-Step "Installing $package..."
        try {
            choco install $package -y
            if ($LASTEXITCODE -eq 0) {
                Write-Success "$package installed"
            } else {
                Write-Warning "$package installation may have failed"
            }
        } catch {
            Write-Error "Failed to install $package: $($_.Exception.Message)"
        }
    }
    
    # Install optional packages
    Write-Step "Installing optional development tools..."
    $optionalPackages = @(
        "vscode",
        "notepadplusplus",
        "7zip"
    )
    
    foreach ($package in $optionalPackages) {
        try {
            choco install $package -y
            if ($LASTEXITCODE -eq 0) {
                Write-Success "$package installed"
            }
        } catch {
            Write-Warning "Optional package $package failed to install"
        }
    }
    
    return $true
}

# Install C++ libraries using vcpkg
function Install-WithVcpkg {
    Write-Step "Setting up vcpkg for C++ libraries..."
    
    $vcpkgDir = "C:\vcpkg"
    
    # Clone vcpkg if not exists
    if (-not (Test-Path $vcpkgDir)) {
        Write-Step "Cloning vcpkg..."
        try {
            git clone https://github.com/Microsoft/vcpkg.git $vcpkgDir
            if ($LASTEXITCODE -ne 0) {
                throw "Git clone failed"
            }
        } catch {
            Write-Error "Failed to clone vcpkg: $($_.Exception.Message)"
            return $false
        }
    } else {
        Write-Info "vcpkg directory already exists"
    }
    
    # Bootstrap vcpkg
    Write-Step "Bootstrapping vcpkg..."
    Push-Location $vcpkgDir
    
    try {
        if (-not (Test-Path "vcpkg.exe")) {
            .\bootstrap-vcpkg.bat
            if ($LASTEXITCODE -ne 0) {
                throw "Bootstrap failed"
            }
        }
        
        # Integrate vcpkg with Visual Studio
        .\vcpkg.exe integrate install
        
        # Install C++ dependencies
        Write-Step "Installing C++ libraries..."
        $cppPackages = @(
            "gtest",
            "antlr4"
        )
        
        foreach ($package in $cppPackages) {
            Write-Step "Installing $package..."
            .\vcpkg.exe install "${package}:x64-windows"
        }
        
        Write-Success "vcpkg setup completed"
        Write-Info "vcpkg installed at: $vcpkgDir"
        
    } catch {
        Write-Error "vcpkg setup failed: $($_.Exception.Message)"
        return $false
    } finally {
        Pop-Location
    }
    
    return $true
}

# Show manual installation instructions
function Show-ManualInstructions {
    Write-Host "Manual Installation Instructions" -ForegroundColor Blue
    Write-Host "=================================" -ForegroundColor Blue
    Write-Host ""
    
    Write-Host "Required Components:" -ForegroundColor Yellow
    Write-Host ""
    
    Write-Host "1. Visual Studio 2017+ or Build Tools" -ForegroundColor Cyan
    Write-Host "   Download from: https://visualstudio.microsoft.com/downloads/"
    Write-Host "   - Install 'Desktop development with C++' workload"
    Write-Host "   - Ensure C++17 support is included"
    Write-Host ""
    
    Write-Host "2. CMake 3.15+" -ForegroundColor Cyan
    Write-Host "   Download from: https://cmake.org/download/"
    Write-Host "   - Choose 'Windows x64 Installer'"
    Write-Host "   - Select 'Add CMake to PATH' during installation"
    Write-Host ""
    
    Write-Host "3. Git for Windows" -ForegroundColor Cyan
    Write-Host "   Download from: https://git-scm.com/download/win"
    Write-Host "   - Use default installation options"
    Write-Host ""
    
    Write-Host "Optional Components:" -ForegroundColor Yellow
    Write-Host ""
    
    Write-Host "4. MinGW-w64 (alternative to Visual Studio)" -ForegroundColor Cyan
    Write-Host "   Download from: https://www.mingw-w64.org/downloads/"
    Write-Host "   - Or use MSYS2: https://www.msys2.org/"
    Write-Host ""
    
    Write-Host "5. ANTLR4 C++ Runtime" -ForegroundColor Cyan
    Write-Host "   Download from: https://www.antlr.org/download.html"
    Write-Host "   - Get the C++ runtime source"
    Write-Host "   - Build and install according to documentation"
    Write-Host ""
    
    Write-Host "6. Development Tools" -ForegroundColor Cyan
    Write-Host "   - Visual Studio Code: https://code.visualstudio.com/"
    Write-Host "   - Windows SDK (for debugging tools)"
    Write-Host "   - Intel Inspector (optional, for advanced debugging)"
    Write-Host ""
    
    Write-Host "Installation Verification:" -ForegroundColor Yellow
    Write-Host "After installation, run: .\scripts\check_environment.ps1"
    Write-Host ""
}

# Main installation logic
if ($Manual) {
    Show-ManualInstructions
    exit 0
}

Write-Info "Starting CHTL dependencies installation for Windows..."
Write-Warning "This script requires Administrator privileges and internet connection."
Write-Host ""

$success = $true

if ($All -or $Chocolatey) {
    if (-not (Install-WithChocolatey)) {
        $success = $false
    }
}

if ($All -or $Vcpkg) {
    if (-not (Install-WithVcpkg)) {
        Write-Warning "vcpkg installation failed, but other components may still work"
    }
}

# Refresh environment variables
Write-Step "Refreshing environment variables..."
$env:PATH = [System.Environment]::GetEnvironmentVariable("PATH", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("PATH", "User")

# Final verification
Write-Host ""
Write-Step "Verifying installation..."

# Check if key components are now available
$components = @{
    "cmake" = "CMake"
    "git" = "Git"
}

foreach ($cmd in $components.Keys) {
    try {
        $version = & $cmd --version 2>$null
        if ($LASTEXITCODE -eq 0) {
            Write-Success "$($components[$cmd]) is available"
        } else {
            Write-Warning "$($components[$cmd]) not found in PATH (may need restart)"
        }
    } catch {
        Write-Warning "$($components[$cmd]) not found in PATH"
    }
}

Write-Host ""
Write-Host "="*60 -ForegroundColor Blue
Write-Host "Installation Summary" -ForegroundColor Blue
Write-Host "="*60 -ForegroundColor Blue

if ($success) {
    Write-Success "Dependencies installation completed!"
    Write-Host ""
    Write-Info "Next steps:"
    Write-Info "1. Restart your terminal/PowerShell session"
    Write-Info "2. Run: .\scripts\check_environment.ps1"
    Write-Info "3. If everything looks good, build CHTL:"
    Write-Info "   .\scripts\build_release.ps1"
} else {
    Write-Warning "Installation completed with some issues"
    Write-Info "Please check the error messages above and install missing components manually"
    Write-Info "Run .\install_dependencies.ps1 -Manual for detailed instructions"
}

Write-Host ""
Write-Info "For troubleshooting, see: docs/core/项目环境依赖文档.md"