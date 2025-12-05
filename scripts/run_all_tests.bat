@echo off
REM ============================================
REM 运行所有测试的主脚本
REM ============================================

echo.
echo ============================================
echo   运行所有测试套件
echo ============================================
echo.

set PASSED=0
set FAILED=0
set TOTAL=0

REM 编译器设置
set COMPILER=g++
set CFLAGS=-std=c++11 -Iinclude
set SOURCES=src/physicalWorld.cpp src/shapes.cpp

echo [1/11] 编译并运行 test_slope_friction.exe...
%COMPILER% %CFLAGS% -o tests/test_slope_friction.exe tests/test_slope_friction.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    set /a FAILED+=1
) else (
    tests\test_slope_friction.exe > tests\output_slope_friction.txt 2>&1
    echo    [DONE] 输出已保存到 tests\output_slope_friction.txt
    set /a PASSED+=1
)
set /a TOTAL+=1
echo.

echo [2/11] 编译并运行 test_incline_sliding.exe...
%COMPILER% %CFLAGS% -o tests/test_incline_sliding.exe tests/test_incline_sliding.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    set /a FAILED+=1
) else (
    tests\test_incline_sliding.exe > tests\output_incline_sliding.txt 2>&1
    echo    [DONE] 输出已保存到 tests\output_incline_sliding.txt
    set /a PASSED+=1
)
set /a TOTAL+=1
echo.

echo [3/11] 编译并运行 test_horizontal_friction.exe...
%COMPILER% %CFLAGS% -o tests/test_horizontal_friction.exe tests/test_horizontal_friction.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    set /a FAILED+=1
) else (
    tests\test_horizontal_friction.exe > tests\output_horizontal_friction.txt 2>&1
    echo    [DONE] 输出已保存到 tests\output_horizontal_friction.txt
    set /a PASSED+=1
)
set /a TOTAL+=1
echo.

echo [4/11] 编译并运行 test_wall_collision.exe...
%COMPILER% %CFLAGS% -o tests/test_wall_collision.exe tests/test_wall_collision.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    set /a FAILED+=1
) else (
    tests\test_wall_collision.exe > tests\output_wall_collision.txt 2>&1
    echo    [DONE] 输出已保存到 tests\output_wall_collision.txt
    set /a PASSED+=1
)
set /a TOTAL+=1
echo.

echo [5/11] 编译并运行 test_physicalWorld.exe...
%COMPILER% %CFLAGS% -o tests/test_physicalWorld.exe tests/test_physicalWorld.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    set /a FAILED+=1
) else (
    tests\test_physicalWorld.exe > tests\output_physicalWorld.txt 2>&1
    echo    [DONE] 输出已保存到 tests\output_physicalWorld.txt
    set /a PASSED+=1
)
set /a TOTAL+=1
echo.

echo [6/11] 编译并运行 quick_test.exe...
%COMPILER% %CFLAGS% -o tests/quick_test.exe tests/quick_test.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    set /a FAILED+=1
) else (
    tests\quick_test.exe > tests\output_quick_test.txt 2>&1
    echo    [DONE] 输出已保存到 tests\output_quick_test.txt
    set /a PASSED+=1
)
set /a TOTAL+=1
echo.

echo [7/11] 编译并运行 test_simple_horizontal.exe...
%COMPILER% %CFLAGS% -o tests/test_simple_horizontal.exe tests/test_simple_horizontal.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    set /a FAILED+=1
) else (
    tests\test_simple_horizontal.exe > tests\output_simple_horizontal.txt 2>&1
    echo    [DONE] 输出已保存到 tests\output_simple_horizontal.txt
    set /a PASSED+=1
)
set /a TOTAL+=1
echo.

echo [8/11] 编译并运行 test_platform_friction.exe...
%COMPILER% %CFLAGS% -o tests/test_platform_friction.exe tests/test_platform_friction.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    set /a FAILED+=1
) else (
    tests\test_platform_friction.exe > tests\output_platform_friction.txt 2>&1
    echo    [DONE] 输出已保存到 tests\output_platform_friction.txt
    set /a PASSED+=1
)
set /a TOTAL+=1
echo.

echo [9/11] 编译并运行 test_friction_comprehensive.exe...
%COMPILER% %CFLAGS% -o tests/test_friction_comprehensive.exe tests/test_friction_comprehensive.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    set /a FAILED+=1
) else (
    tests\test_friction_comprehensive.exe > tests\output_friction_comprehensive.txt 2>&1
    echo    [DONE] 输出已保存到 tests\output_friction_comprehensive.txt
    set /a PASSED+=1
)
set /a TOTAL+=1
echo.

echo [10/11] 编译并运行 test_stacked_friction_fixed.exe...
%COMPILER% %CFLAGS% -o tests/test_stacked_friction_fixed.exe tests/test_stacked_friction_fixed.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    set /a FAILED+=1
) else (
    tests\test_stacked_friction_fixed.exe > tests\output_stacked_friction_fixed.txt 2>&1
    echo    [DONE] 输出已保存到 tests\output_stacked_friction_fixed.txt
    set /a PASSED+=1
)
set /a TOTAL+=1
echo.

echo [11/11] 编译并运行 test_acceleration_analysis.exe...
%COMPILER% %CFLAGS% -o tests/test_acceleration_analysis.exe tests/test_acceleration_analysis.cpp %SOURCES%
if errorlevel 1 (
    echo    [FAIL] 编译失败
    set /a FAILED+=1
) else (
    tests\test_acceleration_analysis.exe > tests\output_acceleration_analysis.txt 2>&1
    echo    [DONE] 输出已保存到 tests\output_acceleration_analysis.txt
    set /a PASSED+=1
)
set /a TOTAL+=1
echo.

echo ============================================
echo   测试完成
echo ============================================
echo   总计: %TOTAL% 个测试
echo   成功: %PASSED% 个
echo   失败: %FAILED% 个
echo ============================================
echo.
echo 所有输出文件保存在 tests\ 目录下
echo.

pause
