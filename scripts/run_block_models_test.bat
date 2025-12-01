@echo off
REM ============================================
REM 板块模型测试脚本
REM ============================================

echo.
echo ============================================
echo   板块模型测试
echo ============================================
echo.

set COMPILER=g++
set CFLAGS=-std=c++11 -Iinclude
set SOURCES=src/physicalWorld.cpp src/shapes.cpp

echo [1/2] 编译 test_block_models.exe...
%COMPILER% %CFLAGS% -o tests/test_block_models.exe tests/test_block_models.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    pause
    exit /b 1
)
echo    [OK] 编译成功
echo.

echo [2/2] 运行测试...
echo.
tests\test_block_models.exe
echo.

echo ============================================
echo   测试完成
echo ============================================
echo.
echo 查看完整输出: tests\output_block_models.txt
echo.

pause
