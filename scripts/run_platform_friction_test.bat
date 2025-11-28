@echo off
echo ============================================
echo   ±‡“Î≤¢‘À––∞ÂøÈƒ¶≤¡¡¶≤‚ ‘
echo ============================================
echo.

REM »∑±£ƒø¬º¥Ê‘⁄
if not exist "bin" mkdir bin
if not exist "obj" mkdir obj

echo [1/3] ±‡“Î shapes.cpp...
g++ -std=c++11 -Iinclude -c src/shapes.cpp -o obj/shapes.o
if errorlevel 1 (
    echo ¥ÌŒÛ: shapes.cpp ±‡“Î ß∞‹
    pause
    exit /b 1
)

echo [2/3] ±‡“Î physicalWorld.cpp...
g++ -std=c++11 -Iinclude -c src/physicalWorld.cpp -o obj/physicalWorld.o
if errorlevel 1 (
    echo ¥ÌŒÛ: physicalWorld.cpp ±‡“Î ß∞‹
    pause
    exit /b 1
)

echo [3/3] ±‡“Î≤¢¡¥Ω” test_platform_friction.cpp...
g++ -std=c++11 -Iinclude tests/test_platform_friction.cpp obj/shapes.o obj/physicalWorld.o -o bin/test_platform.exe
if errorlevel 1 (
    echo ¥ÌŒÛ: test_platform_friction.cpp ±‡“Î ß∞‹
    pause
    exit /b 1
)

echo.
echo ============================================
echo   ±‡“Î≥…π¶£°‘À––≤‚ ‘...
echo ============================================
echo.

bin\test_platform.exe

echo.
echo ============================================
echo   ≤‚ ‘ÕÍ≥…
echo ============================================
pause
