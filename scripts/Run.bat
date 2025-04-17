@echo off

:: Get the directory where the script is located
set "SCRIPT_DIR=%~dp0"
:: Get the root directory (one level up from scripts)
set "ROOT_DIR=%SCRIPT_DIR%.."

:: Default configuration
set "CONFIG=debug"

:: Process command line arguments
:arg_loop
if "%1" == "" goto arg_done
if "%1" == "--config" (
    set "CONFIG=%2"
    shift
    shift
    goto arg_loop
)
echo Unknown option: %1
exit /b 1
:arg_done

:: Run the game
echo Running the game in %CONFIG% configuration...
"%ROOT_DIR%\build\bin\Game.exe"

if %ERRORLEVEL% neq 0 (
    echo Game execution failed!
    exit /b 1
)
