@echo off
echo Compiling Runtime Terror...
echo.

REM Try common compiler locations
set COMPILER_FOUND=0

REM Check for g++ in PATH
where g++ >nul 2>&1
if %ERRORLEVEL% == 0 (
    echo Found g++ in PATH
    set COMPILER=g++
    set COMPILER_FOUND=1
    goto :compile
)

REM Check for clang
where clang++ >nul 2>&1
if %ERRORLEVEL% == 0 (
    echo Found clang++ in PATH
    set COMPILER=clang++
    set COMPILER_FOUND=1
    goto :compile
)

REM Check for MSVC cl.exe
where cl >nul 2>&1
if %ERRORLEVEL% == 0 (
    echo Found MSVC cl.exe
    set COMPILER=cl
    set COMPILER_FOUND=1
    goto :compile
)

echo No C++ compiler found in PATH!
echo Please install MinGW-w64 or Visual Studio Build Tools
echo.
echo MinGW-w64: https://www.mingw-w64.org/downloads/
echo Visual Studio: https://visualstudio.microsoft.com/downloads/
pause
exit /b 1

:compile
echo Using compiler: %COMPILER%
echo.

REM Try to compile with raylib (assuming it's in lib folder)
%COMPILER% -o RuntimeTerror.exe src/main.cpp src/player.cpp src/tiles.cpp src/rewind.cpp -Ilib -Llib -lraylib -lopengl32 -lgdi32 -lwinmm

if %ERRORLEVEL% == 0 (
    echo.
    echo Compilation successful!
    echo Run: RuntimeTerror.exe
    pause
) else (
    echo.
    echo Compilation failed. You may need to install raylib first.
    echo Download from: https://www.raylib.com/works.html
    pause
)
