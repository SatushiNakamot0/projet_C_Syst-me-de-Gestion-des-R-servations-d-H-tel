@echo off
echo ========================================
echo Hotel Reservation System - One-Click Setup
echo ========================================
echo.

REM Check if build directory exists and clean it
if exist build (
    echo Cleaning existing build directory...
    rmdir /s /q build
    if %errorlevel% neq 0 (
        echo ERROR: Failed to clean build directory
        pause
        exit /b 1
    )
)

REM Create new build directory
echo Creating build directory...
mkdir build
if %errorlevel% neq 0 (
    echo ERROR: Failed to create build directory
    pause
    exit /b 1
)

cd build
if %errorlevel% neq 0 (
    echo ERROR: Failed to enter build directory
    pause
    exit /b 1
)

REM Configure with CMake
echo.
echo Configuring project with CMake...
cmake -G "MinGW Makefiles" ..
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed
    echo Make sure you have MSYS2/MinGW installed and in PATH
    pause
    exit /b 1
)

REM Build the project
echo.
echo Building project...
cmake --build .
if %errorlevel% neq 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo ========================================
echo BUILD SUCCESSFUL!
echo ========================================
echo.
echo You can now run the application:
echo   hotel_app.exe
echo.
echo Press any key to start the application...
pause > nul

REM Run the application
hotel_app.exe
if %errorlevel% neq 0 (
    echo.
    echo ERROR: Application failed to start
    pause
    exit /b 1
)

echo.
echo Application finished. Press any key to exit...
pause > nul
