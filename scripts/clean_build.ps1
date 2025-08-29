# CHTL Build Clean Script for Windows PowerShell
# This script cleans all build artifacts and temporary files

param(
    [switch]$All,
    [switch]$Build,
    [switch]$Temp,
    [switch]$Packages,
    [switch]$Cache,
    [switch]$DryRun,
    [switch]$Help
)

# Colors for output
function Write-Success { param($Message) Write-Host $Message -ForegroundColor Green }
function Write-Warning { param($Message) Write-Host $Message -ForegroundColor Yellow }
function Write-Error { param($Message) Write-Host $Message -ForegroundColor Red }
function Write-Info { param($Message) Write-Host $Message -ForegroundColor Cyan }

if ($Help) {
    Write-Host "CHTL Build Clean Script for Windows PowerShell" -ForegroundColor Blue
    Write-Host "=============================================="
    Write-Host ""
    Write-Host "Usage: .\clean_build.ps1 [options]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -All         Clean everything (build dirs, temp files, packages)"
    Write-Host "  -Build       Clean only build directories"
    Write-Host "  -Temp        Clean only temporary files"
    Write-Host "  -Packages    Clean only package files (.cmod, .cjmod)"
    Write-Host "  -Cache       Clean only CMake cache and generated files"
    Write-Host "  -DryRun      Show what would be deleted without actually deleting"
    Write-Host "  -Help        Show this help message"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\clean_build.ps1 -All          # Clean everything"
    Write-Host "  .\clean_build.ps1 -Build        # Clean only build directories"
    Write-Host "  .\clean_build.ps1 -DryRun       # Preview what would be cleaned"
    exit 0
}

Write-Host "CHTL Build Clean Script for Windows PowerShell" -ForegroundColor Blue
Write-Host "=============================================="

# If no options specified, clean build directories by default
if (-not ($Build -or $Temp -or $Packages -or $Cache -or $All)) {
    $Build = $true
}

# If -All specified, enable everything
if ($All) {
    $Build = $true
    $Temp = $true
    $Packages = $true
    $Cache = $true
}

if ($DryRun) {
    Write-Warning "Running in dry-run mode - no files will be deleted"
    Write-Host ""
}

# Track what was cleaned
$CleanedItems = @()
$TotalSizeFreed = 0

# Function to safely remove files/directories
function Remove-ItemSafely {
    param(
        [string]$Path,
        [string]$Description
    )
    
    if (Test-Path $Path) {
        # Calculate size before deletion
        try {
            $size = 0
            if (Test-Path $Path -PathType Container) {
                $size = (Get-ChildItem $Path -Recurse -ErrorAction SilentlyContinue | 
                        Measure-Object -Property Length -Sum -ErrorAction SilentlyContinue).Sum
            } else {
                $size = (Get-Item $Path -ErrorAction SilentlyContinue).Length
            }
            
            if ($size -eq $null) { $size = 0 }
            $script:TotalSizeFreed += $size
        } catch {
            # Size calculation failed, continue anyway
        }
        
        if ($DryRun) {
            Write-Warning "[DRY RUN] Would remove: $Path ($Description)"
        } else {
            Write-Warning "Removing: $Path ($Description)"
            try {
                Remove-Item -Path $Path -Recurse -Force -ErrorAction Stop
            } catch {
                Write-Error "Failed to remove: $Path - $($_.Exception.Message)"
            }
        }
        $script:CleanedItems += $Description
    }
}

# Function to find and remove files by pattern
function Remove-FilesByPattern {
    param(
        [string]$Pattern,
        [string]$Description,
        [string]$BaseDir = "."
    )
    
    $foundFiles = Get-ChildItem -Path $BaseDir -Filter $Pattern -ErrorAction SilentlyContinue
    
    foreach ($file in $foundFiles) {
        Remove-ItemSafely -Path $file.FullName -Description "$Description ($($file.Name))"
    }
}

Write-Warning "Starting cleanup process..."

# Clean build directories
if ($Build) {
    Write-Info "Cleaning build directories..."
    
    Remove-ItemSafely -Path "build" -Description "Main build directory"
    Remove-ItemSafely -Path "build_debug" -Description "Debug build directory"
    Remove-ItemSafely -Path "build_release" -Description "Release build directory"
    Remove-ItemSafely -Path "build_test" -Description "Test build directory"
    Remove-ItemSafely -Path "bin" -Description "Binary output directory"
    Remove-ItemSafely -Path "lib" -Description "Library output directory"
    Remove-ItemSafely -Path "obj" -Description "Object files directory"
    Remove-ItemSafely -Path "out" -Description "Output directory"
    Remove-ItemSafely -Path "x64" -Description "Visual Studio x64 directory"
    Remove-ItemSafely -Path "Win32" -Description "Visual Studio Win32 directory"
    Remove-ItemSafely -Path ".vs" -Description "Visual Studio cache directory"
    
    # Find any other build-like directories
    Get-ChildItem -Directory -Filter "build_*" | ForEach-Object {
        Remove-ItemSafely -Path $_.FullName -Description "Build directory"
    }
}

# Clean temporary files
if ($Temp) {
    Write-Info "Cleaning temporary files..."
    
    Remove-ItemSafely -Path "temp" -Description "Temporary directory"
    Remove-ItemSafely -Path "tmp" -Description "Temporary directory"
    
    # Windows-specific temporary files
    Remove-FilesByPattern -Pattern "*.tmp" -Description "Temporary file"
    Remove-FilesByPattern -Pattern "*.temp" -Description "Temporary file"
    Remove-FilesByPattern -Pattern "*.bak" -Description "Backup file"
    Remove-FilesByPattern -Pattern "*.log" -Description "Log file"
    Remove-FilesByPattern -Pattern "*.pid" -Description "Process ID file"
    
    # Visual Studio temporary files
    Remove-FilesByPattern -Pattern "*.pch" -Description "Precompiled header"
    Remove-FilesByPattern -Pattern "*.idb" -Description "Incremental database"
    Remove-FilesByPattern -Pattern "*.pdb" -Description "Program database"
    Remove-FilesByPattern -Pattern "*.ilk" -Description "Incremental linker file"
    Remove-FilesByPattern -Pattern "*.exp" -Description "Export file"
    Remove-FilesByPattern -Pattern "*.exe.manifest" -Description "Manifest file"
    
    # Backup files
    Remove-FilesByPattern -Pattern "*~" -Description "Backup file"
    Remove-FilesByPattern -Pattern "*.orig" -Description "Original file"
    Remove-FilesByPattern -Pattern "*.rej" -Description "Reject file"
    
    # Test output files
    Remove-FilesByPattern -Pattern "test_*" -Description "Test output file"
    Remove-FilesByPattern -Pattern "*_test_*" -Description "Test output file"
    Remove-FilesByPattern -Pattern "*_demo_*" -Description "Demo output file"
}

# Clean package files
if ($Packages) {
    Write-Info "Cleaning package files..."
    
    Remove-FilesByPattern -Pattern "*.cmod" -Description "CMOD package"
    Remove-FilesByPattern -Pattern "*.cjmod" -Description "CJMOD package"
    Remove-FilesByPattern -Pattern "*.chtlpkg" -Description "CHTL unified package"
    Remove-FilesByPattern -Pattern "*.cmod.info" -Description "CMOD package info"
    Remove-FilesByPattern -Pattern "*.cjmod.info" -Description "CJMOD package info"
    
    # Package temporary directories
    Get-ChildItem -Directory -Filter "*_package_*" | ForEach-Object {
        Remove-ItemSafely -Path $_.FullName -Description "Package temporary directory"
    }
}

# Clean CMake cache and generated files
if ($Cache) {
    Write-Info "Cleaning CMake cache and generated files..."
    
    Remove-ItemSafely -Path "CMakeCache.txt" -Description "CMake cache file"
    Remove-ItemSafely -Path "CMakeFiles" -Description "CMake files directory"
    Remove-ItemSafely -Path "cmake_install.cmake" -Description "CMake install script"
    Remove-ItemSafely -Path "Makefile" -Description "Generated Makefile"
    Remove-ItemSafely -Path "compile_commands.json" -Description "Compile commands database"
    Remove-ItemSafely -Path "CTestTestfile.cmake" -Description "CTest file"
    Remove-ItemSafely -Path "Testing" -Description "CTest directory"
    
    # Visual Studio generated files
    Remove-FilesByPattern -Pattern "*.sln" -Description "Visual Studio solution file"
    Remove-FilesByPattern -Pattern "*.vcxproj" -Description "Visual Studio project file"
    Remove-FilesByPattern -Pattern "*.vcxproj.filters" -Description "Visual Studio filters file"
    Remove-FilesByPattern -Pattern "*.vcxproj.user" -Description "Visual Studio user file"
    
    # Find CMake generated files
    Remove-FilesByPattern -Pattern "*.cmake" -Description "CMake generated file"
}

# Clean IDE and editor files
if ($Temp) {
    Write-Info "Cleaning IDE and editor files..."
    
    Remove-ItemSafely -Path ".vscode" -Description "VS Code directory"
    Remove-ItemSafely -Path ".idea" -Description "IntelliJ IDEA directory"
    Remove-ItemSafely -Path ".clangd" -Description "Clangd cache directory"
    Remove-ItemSafely -Path ".ccls-cache" -Description "CCLS cache directory"
    Remove-ItemSafely -Path ".cquery_cache" -Description "Cquery cache directory"
    
    Remove-FilesByPattern -Pattern "*.code-workspace" -Description "VS Code workspace"
    
    # Windows-specific editor files
    Remove-FilesByPattern -Pattern "*.suo" -Description "Visual Studio user options"
    Remove-FilesByPattern -Pattern "*.user" -Description "Visual Studio user files"
    Remove-FilesByPattern -Pattern "*.ncb" -Description "Visual Studio IntelliSense database"
    Remove-FilesByPattern -Pattern "*.aps" -Description "Visual Studio resource file"
    Remove-ItemSafely -Path "ipch" -Description "Visual Studio precompiled headers"
}

# Calculate and display summary
Write-Host ""
Write-Success "Cleanup Summary"
Write-Host "==============="

if ($CleanedItems.Count -eq 0) {
    Write-Success "No items to clean"
} else {
    Write-Host "Items cleaned: $($CleanedItems.Count)"
    
    # Convert bytes to human readable
    if ($TotalSizeFreed -gt 0) {
        $sizeGB = [math]::Round($TotalSizeFreed / 1GB, 2)
        $sizeMB = [math]::Round($TotalSizeFreed / 1MB, 2)
        $sizeKB = [math]::Round($TotalSizeFreed / 1KB, 2)
        
        if ($sizeGB -gt 1) {
            Write-Host "Space freed: ${sizeGB}GB"
        } elseif ($sizeMB -gt 1) {
            Write-Host "Space freed: ${sizeMB}MB"
        } elseif ($sizeKB -gt 1) {
            Write-Host "Space freed: ${sizeKB}KB"
        } else {
            Write-Host "Space freed: ${TotalSizeFreed}B"
        }
    }
    
    # Show cleaned categories (unique)
    $uniqueCategories = $CleanedItems | Sort-Object | Get-Unique
    if ($uniqueCategories.Count -gt 0) {
        Write-Host ""
        Write-Host "Categories cleaned:"
        foreach ($category in $uniqueCategories) {
            Write-Host "  - $category"
        }
    }
}

if ($DryRun) {
    Write-Host ""
    Write-Warning "This was a dry run. To actually clean files, run without -DryRun"
}

Write-Host ""
Write-Success "Cleanup completed!"