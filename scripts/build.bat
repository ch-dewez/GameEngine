@echo off

:: Get the directory where the script is located
set "SCRIPT_DIR=%~dp0"
set "ROOT_DIR=%SCRIPT_DIR%.."

:: Default configuration
set "CONFIG=Debug"
set "RUN_AFTER_BUILD=false"

:: Process command line arguments
:arg_loop
if "%1" == "" goto arg_done
if "%1" == "--config" (
    set "CONFIG=%2"
    shift
    shift
    goto arg_loop
)
if "%1" == "--run" (
    set "RUN_AFTER_BUILD=true"
    shift
    goto arg_loop
)
echo Unknown option: %1
exit /b 1
:arg_done

:: Build the project
cd "%ROOT_DIR%\build"
cmake --build . --config %CONFIG%

if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b 1
)

echo Build completed successfully!

:: Run the game if --run was specified
if "%RUN_AFTER_BUILD%" == "true" (
    echo Running the game...
    call "%SCRIPT_DIR%\Run.bat" "--config" "%CONFIG%"
)
