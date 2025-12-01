@echo off
REM 物理引擎测试编译和运行脚本 (Windows)

setlocal

echo ========================================
echo    物理引擎测试脚本
echo ========================================
echo.

REM 检查是否安装了 g++
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo [错误] 未找到 g++ 编译器！
    echo 请安装 MinGW 或其他 C++ 编译器。
    pause
    exit /b 1
)

echo [信息] 编译器检测: 
g++ --version | findstr /C:"g++"
echo.

REM 创建构建目录
if not exist build mkdir build

REM 显示菜单
:menu
echo 请选择操作:
echo   1. 编译完整测试
echo   2. 编译快速测试
echo   3. 编译所有测试
echo   4. 运行完整测试
echo   5. 运行快速测试
echo   6. 编译并运行完整测试
echo   7. 编译并运行快速测试
echo   8. 清理编译文件
echo   9. 退出
echo.

set /p choice="输入选项 (1-9): "

if "%choice%"=="1" goto compile_full
if "%choice%"=="2" goto compile_quick
if "%choice%"=="3" goto compile_all
if "%choice%"=="4" goto run_full
if "%choice%"=="5" goto run_quick
if "%choice%"=="6" goto build_run_full
if "%choice%"=="7" goto build_run_quick
if "%choice%"=="8" goto clean
if "%choice%"=="9" goto end

echo [错误] 无效选项！
echo.
goto menu

:compile_full
echo.
echo [编译] 完整测试套件...
g++ -std=c++11 -Wall -I include tests/test_physicalWorld.cpp src/physicalWorld.cpp src/shapes.cpp -o build/test_engine.exe
if %errorlevel% equ 0 (
    echo [成功] 编译完成: build/test_engine.exe
) else (
    echo [失败] 编译出错！
)
echo.
pause
goto menu

:compile_quick
echo.
echo [编译] 快速测试...
g++ -std=c++11 -Wall -I include tests/quick_test.cpp src/physicalWorld.cpp src/shapes.cpp -o build/quick_test.exe
if %errorlevel% equ 0 (
    echo [成功] 编译完成: build/quick_test.exe
) else (
    echo [失败] 编译出错！
)
echo.
pause
goto menu

:compile_all
echo.
echo [编译] 所有测试...
call :compile_full
call :compile_quick
goto menu

:run_full
echo.
if not exist build/test_engine.exe (
    echo [错误] 找不到 build/test_engine.exe，请先编译！
    pause
    goto menu
)
echo [运行] 完整测试套件...
echo.
build\test_engine.exe
echo.
pause
goto menu

:run_quick
echo.
if not exist build/quick_test.exe (
    echo [错误] 找不到 build/quick_test.exe，请先编译！
    pause
    goto menu
)
echo [运行] 快速测试...
echo.
build\quick_test.exe
echo.
pause
goto menu

:build_run_full
call :compile_full
if %errorlevel% equ 0 (
    call :run_full
)
goto menu

:build_run_quick
call :compile_quick
if %errorlevel% equ 0 (
    call :run_quick
)
goto menu

:clean
echo.
echo [清理] 删除编译文件...
if exist build rmdir /s /q build
echo [完成] 清理完成！
echo.
pause
goto menu

:end
echo.
echo 再见！
endlocal
