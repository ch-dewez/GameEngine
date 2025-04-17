@echo off

:: Get the directory where the script is located
set "SCRIPT_DIR=%~dp0"
set "ROOT_DIR=%SCRIPT_DIR%.."

:: Create build directory
if not exist "%ROOT_DIR%\build" mkdir "%ROOT_DIR%\build"
cd "%ROOT_DIR%\build"

:: Generate build files
cmake ..

if %ERRORLEVEL% neq 0 (
    echo Failed to generate CMake project files!
    exit /b 1
)

echo CMake project files generated successfully!