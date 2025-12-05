@echo off
echo ========================================
echo ±‡“Îƒ¶≤¡¡¶ª¨––≤‚ ‘
echo ========================================

g++ -o tests\test_friction_sliding.exe tests\test_friction_sliding.cpp src\physicalWorld.cpp src\shapes.cpp -Iinclude -std=c++11

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo ±‡“Î≥…π¶£¨ø™ º‘À––≤‚ ‘...
    echo ========================================
    echo.
    tests\test_friction_sliding.exe
) else (
    echo.
    echo ========================================
    echo ±‡“Î ß∞‹£°
    echo ========================================
    pause
)
