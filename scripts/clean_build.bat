@echo off
setlocal enabledelayedexpansion

rem CHTL Build Clean Script for Windows
rem This script cleans all build artifacts and temporary files

set "SCRIPT_NAME=%~nx0"
set "DRY_RUN=false"
set "CLEAN_BUILD=false"
set "CLEAN_TEMP=false" 
set "CLEAN_PACKAGES=false"
set "CLEAN_CACHE=false"
set "CLEAN_ALL=false"

rem Colors (if supported)
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

echo %BLUE%CHTL Build Clean Script for Windows%NC%
echo ========================================

rem Parse command line arguments
:parse_args
if "%~1"=="" goto :args_done
if /i "%~1"=="--help" goto :show_help
if /i "%~1"=="-h" goto :show_help
if /i "%~1"=="--all" set "CLEAN_ALL=true" & goto :next_arg
if /i "%~1"=="--build" set "CLEAN_BUILD=true" & goto :next_arg
if /i "%~1"=="--temp" set "CLEAN_TEMP=true" & goto :next_arg
if /i "%~1"=="--packages" set "CLEAN_PACKAGES=true" & goto :next_arg
if /i "%~1"=="--cache" set "CLEAN_CACHE=true" & goto :next_arg
if /i "%~1"=="--dry-run" set "DRY_RUN=true" & goto :next_arg

echo %RED%ERROR: Unknown option: %~1%NC%
goto :show_help

:next_arg
shift
goto :parse_args

:args_done

rem If no options specified, clean build directories by default
if "%CLEAN_BUILD%"=="false" if "%CLEAN_TEMP%"=="false" if "%CLEAN_PACKAGES%"=="false" if "%CLEAN_CACHE%"=="false" if "%CLEAN_ALL%"=="false" (
    set "CLEAN_BUILD=true"
)

rem If --all specified, enable everything
if "%CLEAN_ALL%"=="true" (
    set "CLEAN_BUILD=true"
    set "CLEAN_TEMP=true"
    set "CLEAN_PACKAGES=true"
    set "CLEAN_CACHE=true"
)

if "%DRY_RUN%"=="true" (
    echo %YELLOW%Running in dry-run mode - no files will be deleted%NC%
    echo.
)

set "ITEMS_CLEANED=0"
set "TOTAL_SIZE_FREED=0"

rem Function to safely remove files/directories
goto :start_cleaning

:safe_remove
set "path=%~1"
set "description=%~2"

if exist "%path%" (
    if "%DRY_RUN%"=="true" (
        echo %YELLOW%[DRY RUN] Would remove: %path% ^(%description%^)%NC%
    ) else (
        echo %YELLOW%Removing: %path% ^(%description%^)%NC%
        if exist "%path%\*" (
            rmdir /s /q "%path%" 2>nul
        ) else (
            del /f /q "%path%" 2>nul
        )
    )
    set /a "ITEMS_CLEANED+=1"
)
goto :eof

:start_cleaning

echo %YELLOW%Starting cleanup process...%NC%

rem Clean build directories
if "%CLEAN_BUILD%"=="true" (
    echo %BLUE%Cleaning build directories...%NC%
    
    call :safe_remove "build" "Main build directory"
    call :safe_remove "build_debug" "Debug build directory"
    call :safe_remove "build_release" "Release build directory"
    call :safe_remove "build_test" "Test build directory"
    call :safe_remove "bin" "Binary output directory"
    call :safe_remove "lib" "Library output directory"
    call :safe_remove "obj" "Object files directory"
    call :safe_remove "out" "Output directory"
    call :safe_remove "x64" "Visual Studio x64 directory"
    call :safe_remove "Win32" "Visual Studio Win32 directory"
    call :safe_remove ".vs" "Visual Studio cache directory"
    
    rem Find any other build-like directories
    for /d %%d in (build_*) do (
        call :safe_remove "%%d" "Build directory"
    )
)

rem Clean temporary files
if "%CLEAN_TEMP%"=="true" (
    echo %BLUE%Cleaning temporary files...%NC%
    
    call :safe_remove "temp" "Temporary directory"
    call :safe_remove "tmp" "Temporary directory"
    
    rem Windows-specific temporary files
    for %%f in (*.tmp *.temp *.bak *.log *.pid) do (
        if exist "%%f" call :safe_remove "%%f" "Temporary file"
    )
    
    rem Visual Studio temporary files
    for %%f in (*.pch *.idb *.pdb *.ilk *.exp *.lib *.exe.manifest) do (
        if exist "%%f" call :safe_remove "%%f" "Visual Studio temporary file"
    )
    
    rem Backup files
    for %%f in (*~ *.orig *.rej) do (
        if exist "%%f" call :safe_remove "%%f" "Backup file"
    )
    
    rem Test output files
    for %%f in (test_* *_test_* *_demo_*) do (
        if exist "%%f" call :safe_remove "%%f" "Test output file"
    )
)

rem Clean package files
if "%CLEAN_PACKAGES%"=="true" (
    echo %BLUE%Cleaning package files...%NC%
    
    for %%f in (*.cmod *.cjmod *.chtlpkg) do (
        if exist "%%f" call :safe_remove "%%f" "Package file"
    )
    
    for %%f in (*.cmod.info *.cjmod.info) do (
        if exist "%%f" call :safe_remove "%%f" "Package info file"
    )
    
    rem Package temporary directories
    for /d %%d in (*_package_*) do (
        call :safe_remove "%%d" "Package temporary directory"
    )
)

rem Clean CMake cache and generated files
if "%CLEAN_CACHE%"=="true" (
    echo %BLUE%Cleaning CMake cache and generated files...%NC%
    
    call :safe_remove "CMakeCache.txt" "CMake cache file"
    call :safe_remove "CMakeFiles" "CMake files directory"
    call :safe_remove "cmake_install.cmake" "CMake install script"
    call :safe_remove "Makefile" "Generated Makefile"
    call :safe_remove "compile_commands.json" "Compile commands database"
    call :safe_remove "CTestTestfile.cmake" "CTest file"
    call :safe_remove "Testing" "CTest directory"
    
    rem Visual Studio generated files
    call :safe_remove "*.sln" "Visual Studio solution file"
    call :safe_remove "*.vcxproj" "Visual Studio project file"
    call :safe_remove "*.vcxproj.filters" "Visual Studio filters file"
    call :safe_remove "*.vcxproj.user" "Visual Studio user file"
    
    rem Find CMake generated files
    for %%f in (*.cmake) do (
        if exist "%%f" call :safe_remove "%%f" "CMake generated file"
    )
)

rem Clean IDE and editor files
if "%CLEAN_TEMP%"=="true" (
    echo %BLUE%Cleaning IDE and editor files...%NC%
    
    call :safe_remove ".vscode" "VS Code directory"
    call :safe_remove ".idea" "IntelliJ IDEA directory"
    call :safe_remove "*.code-workspace" "VS Code workspace"
    call :safe_remove ".clangd" "Clangd cache directory"
    call :safe_remove ".ccls-cache" "CCLS cache directory"
    call :safe_remove ".cquery_cache" "Cquery cache directory"
    
    rem Windows-specific editor files
    call :safe_remove "*.suo" "Visual Studio user options"
    call :safe_remove "*.user" "Visual Studio user files"
    call :safe_remove "*.ncb" "Visual Studio IntelliSense database"
    call :safe_remove "*.aps" "Visual Studio resource file"
    call :safe_remove "ipch" "Visual Studio precompiled headers"
)

echo.
echo %GREEN%Cleanup Summary%NC%
echo ===============

if %ITEMS_CLEANED% equ 0 (
    echo %GREEN%No items to clean%NC%
) else (
    echo Items cleaned: %ITEMS_CLEANED%
)

if "%DRY_RUN%"=="true" (
    echo.
    echo %YELLOW%This was a dry run. To actually clean files, run without --dry-run%NC%
)

echo.
echo %BLUE%Cleanup completed!%NC%
goto :end

:show_help
echo.
echo Usage: %SCRIPT_NAME% [options]
echo.
echo Options:
echo   --all         Clean everything ^(build dirs, temp files, packages^)
echo   --build       Clean only build directories
echo   --temp        Clean only temporary files
echo   --packages    Clean only package files ^(.cmod, .cjmod^)
echo   --cache       Clean only CMake cache and generated files
echo   --dry-run     Show what would be deleted without actually deleting
echo   -h, --help    Show this help message
echo.
echo Examples:
echo   %SCRIPT_NAME% --all          Clean everything
echo   %SCRIPT_NAME% --build        Clean only build directories
echo   %SCRIPT_NAME% --dry-run      Preview what would be cleaned
echo.

:end
endlocal