@echo off
REM ============================================
REM 抛体运动测试脚本
REM ============================================

echo.
echo ============================================
echo   抛体运动测试
echo ============================================
echo.

set COMPILER=g++
set CFLAGS=-std=c++11 -Iinclude
set SOURCES=src/physicalWorld.cpp src/shapes.cpp

echo [1/2] 编译 test_projectile_motion.exe...
%COMPILER% %CFLAGS% -o tests/test_projectile_motion.exe tests/test_projectile_motion.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    pause
    exit /b 1
)
echo    [OK] 编译成功
echo.

echo [2/2] 运行测试...
echo.
tests\test_projectile_motion.exe
echo.

echo ============================================
echo   测试完成
echo ============================================
echo.
echo 查看详细报告: docs\projectile_motion_test_report.md
echo 查看快速总结: tests\PROJECTILE_MOTION_SUMMARY.md
echo.

pause
