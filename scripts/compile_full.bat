@echo off
setlocal

REM Enable Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64
if %errorlevel% neq 0 (
    echo "Could not find Community VsDevCmd.bat. Trying Enterprise..."
    call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=x64
)

REM Set include and lib paths
set INCLUDE=%INCLUDE%;%cd%\include;%cd%\include\easyx
set LIB=%LIB%;%cd%\lib

REM Create bin directory
if not exist bin mkdir bin

REM Source files to compile
REM Excluded:
REM src\threebottums.cpp (duplicate of control_buttons.cpp)
REM src\music_player.cpp (duplicate of music.cpp)
REM src\menu.cpp (has main)
REM src\background1.cpp (missing DigitalRainBackground.h)
REM src\main_console_demo.cpp (has main)
REM src\main_lisp (folder?)

set SOURCES=src\main.cpp src\PhysicsRenderBridge.cpp src\control_buttons.cpp src\additional_buttons.cpp src\fourBottums1.cpp src\fourBottums2.cpp src\physicalWorld.cpp src\shapes.cpp src\Renderer.cpp src\music.cpp

REM Compile
echo Compiling...
cl /EHsc /nologo /std:c++17 %SOURCES% /Fe:bin\ProjectC.exe /link user32.lib gdi32.lib shell32.lib winmm.lib EasyXa.lib

if %errorlevel% equ 0 (
    echo.
    echo Build successful!
    echo Run bin\ProjectC.exe to start.
) else (
    echo.
    echo Build failed.
)

