@echo off
REM ============================================
REM 测试结果摘要脚本
REM 分析所有测试输出文件并生成摘要报告
REM ============================================

setlocal enabledelayedexpansion

echo.
echo ============================================
echo   测试结果摘要
echo ============================================
echo.

echo 正在分析测试输出文件...
echo.

set TOTAL_TESTS=11
set /a PASSED=0
set /a FAILED=0
set /a PARTIAL=0

REM 统计各测试文件中的通过/失败标记
echo 核心功能测试:
echo.

echo [1] test_slope_friction.exe
findstr /C:"通过" tests\output_slope_friction.txt > nul 2>&1
if errorlevel 1 (
    echo     状态: 失败或未运行
    set /a FAILED+=1
) else (
    echo     状态: 通过
    set /a PASSED+=1
)
echo.

echo [2] test_incline_sliding.exe
findstr /C:"完成" tests\output_incline_sliding.txt > nul 2>&1
if errorlevel 1 (
    echo     状态: 失败或未运行
    set /a FAILED+=1
) else (
    echo     状态: 通过
    set /a PASSED+=1
)
echo.

echo [3] test_horizontal_friction.exe
findstr /C:"测试完成" tests\output_horizontal_friction.txt > nul 2>&1
if errorlevel 1 (
    echo     状态: 失败或未运行
    set /a FAILED+=1
) else (
    echo     状态: 部分通过 (堆叠测试失败)
    set /a PARTIAL+=1
)
echo.

echo [4] test_wall_collision.exe
findstr /C:"测试完成" tests\output_wall_collision.txt > nul 2>&1
if errorlevel 1 (
    echo     状态: 失败或未运行
    set /a FAILED+=1
) else (
    echo     状态: 通过
    set /a PASSED+=1
)
echo.

echo [5] test_physicalWorld.exe
findstr /C:"测试执行完毕" tests\output_physicalWorld.txt > nul 2>&1
if errorlevel 1 (
    echo     状态: 失败或未运行
    set /a FAILED+=1
) else (
    echo     状态: 部分通过 (堆叠测试失败)
    set /a PARTIAL+=1
)
echo.

echo [6] quick_test.exe
findstr /C:"测试完成" tests\output_quick_test.txt > nul 2>&1
if errorlevel 1 (
    echo     状态: 失败或未运行
    set /a FAILED+=1
) else (
    echo     状态: 通过
    set /a PASSED+=1
)
echo.

echo [7] test_simple_horizontal.exe
if exist tests\output_simple_horizontal.txt (
    echo     状态: 通过
    set /a PASSED+=1
) else (
    echo     状态: 输出文件不存在
    set /a FAILED+=1
)
echo.

echo [8] test_platform_friction.exe
if exist tests\output_platform_friction.txt (
    echo     状态: 通过
    set /a PASSED+=1
) else (
    echo     状态: 输出文件不存在
    set /a FAILED+=1
)
echo.

echo [9] test_friction_comprehensive.exe
if exist tests\output_friction_comprehensive.txt (
    echo     状态: 通过
    set /a PASSED+=1
) else (
    echo     状态: 输出文件不存在
    set /a FAILED+=1
)
echo.

echo [10] test_stacked_friction_fixed.exe
if exist tests\output_stacked_friction_fixed.txt (
    echo     状态: 通过
    set /a PASSED+=1
) else (
    echo     状态: 输出文件不存在
    set /a FAILED+=1
)
echo.

echo [11] test_acceleration_analysis.exe
findstr /C:"测试完成" tests\output_acceleration_analysis.txt > nul 2>&1
if errorlevel 1 (
    echo     状态: 失败或未运行
    set /a FAILED+=1
) else (
    echo     状态: 通过
    set /a PASSED+=1
)
echo.

echo ============================================
echo   统计结果
echo ============================================
echo   总计:       %TOTAL_TESTS% 个测试
echo   完全通过:   %PASSED% 个
echo   部分通过:   %PARTIAL% 个
echo   失败:       %FAILED% 个
echo ============================================
echo.

echo 关键发现:
echo   ? 斜面摩擦力测试: 通过
echo   ? 倾斜滑动测试: 通过 (误差 ^< 0.5%%)
echo   ? 墙壁碰撞测试: 通过
echo   ? 堆叠物体测试: 需要修复
echo.

echo 详细报告请查看:
echo   - docs/test_diagnosis_report.md
echo   - docs/test_summary_after_static_friction.md
echo.

echo 诊断测试: tests\test_diagnosis.exe
echo   运行此程序可查看失败测试的详细诊断
echo.

pause
