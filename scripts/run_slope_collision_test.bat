@echo off
REM ============================================
REM 斜面碰撞测试脚本
REM ============================================

echo.
echo ============================================
echo   斜面碰撞测试
echo ============================================
echo.

set COMPILER=g++
set CFLAGS=-std=c++11 -Iinclude
set SOURCES=src/physicalWorld.cpp src/shapes.cpp

echo [1/2] 编译 test_slope_collision.exe...
%COMPILER% %CFLAGS% -o tests/test_slope_collision.exe tests/test_slope_collision.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    pause
    exit /b 1
)
echo    [OK] 编译成功
echo.

echo [2/2] 运行测试...
echo.
tests\test_slope_collision.exe > tests\output_slope_collision_full.txt 2>&1
echo.

echo ============================================
echo   测试完成
echo ============================================
echo.
echo 输出已保存到: tests\output_slope_collision_full.txt
echo.

echo 现在运行分析报告...
echo.
%COMPILER% %CFLAGS% -o tests/test_slope_collision_analysis.exe tests/test_slope_collision_analysis.cpp %SOURCES%
tests\test_slope_collision_analysis.exe
echo.

echo 完整测试报告请查看: docs\slope_collision_test_report.md
echo.

pause
