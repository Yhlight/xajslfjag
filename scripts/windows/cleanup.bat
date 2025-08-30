@echo off
REM Repository Cleanup Script for Windows

echo ================================================
echo CHTL Repository Cleanup for Windows
echo ================================================

cd /d "%~dp0..\.."

echo Cleaning build artifacts...
for /d /r . %%d in (build*) do @if exist "%%d" rd /s /q "%%d"
del /s /q *.o 2>nul
del /s /q *.obj 2>nul
del /s /q *.pdb 2>nul
del /s /q *.ilk 2>nul
del /s /q CMakeCache.txt 2>nul

echo Cleaning temporary files...
del /s /q *.tmp 2>nul
del /s /q *.log 2>nul
del /s /q *.bak 2>nul
del /s /q Thumbs.db 2>nul

echo Cleaning test artifacts...
del /s /q *test*.exe 2>nul
del /s /q *verification*.exe 2>nul

echo ================================================
echo Repository Cleanup Completed
echo ================================================
echo Repository is now clean and ready for distribution
echo ================================================

pause