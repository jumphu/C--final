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
REM Excluded: src\main.cpp (console test), src\menu.cpp (old entry)
set SOURCES=src\main_graphical.cpp src\adapter.cpp src\background_integrated.cpp src\fourBottums1.cpp src\fourbuttons2_fixed.cpp src\music_fixed.cpp src\physicalWorld.cpp src\Renderer.cpp src\shapes.cpp src\spacebottum.cpp src\threebottums.cpp

REM Compile
echo Compiling...
cl /EHsc /nologo /std:c++17 %SOURCES% /Fe:bin\ProjectC_final.exe /link user32.lib gdi32.lib shell32.lib winmm.lib EasyXa.lib

if %errorlevel% equ 0 (
    echo.
    echo Build successful!
    echo Run bin\ProjectC_final.exe to start.
) else (
    echo.
    echo Build failed.
)

