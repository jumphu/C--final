@echo off
echo === 编译 Project-C 演示程序 ===
echo.

REM 设置Visual Studio环境
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64

echo 正在编译简化演示程序...
cl /EHsc /nologo examples\simple_demo.cpp /Fe:bin\simple_demo.exe

if exist "bin\simple_demo.exe" (
    echo 编译成功！可执行文件: bin\simple_demo.exe
    echo.
    echo 运行演示程序:
    bin\simple_demo.exe
) else (
    echo 编译失败。
    echo.
    echo 请检查:
    echo 1. Visual Studio 2022 Community 是否正确安装
    echo 2. Windows SDK 是否安装
    echo 3. C++ 开发工具是否安装
)

echo.
pause