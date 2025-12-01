@echo off
echo === Project-C EasyX 编译演示 ===
echo.
echo 使用的真正EasyX头文件来自:
echo "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\VS\include\graphics.h"
echo.
echo 要编译EasyX程序，需要:
echo 1. 安装Visual Studio 2022 Community版本
echo 2. 包含EasyX库支持
echo 3. 使用Windows图形API (GDI+)
echo.
echo 演示代码位置: examples\simple_easyx_demo.cpp
echo 代码只有18行，展示了:
echo - 创建形状对象 (Circle, AABB)
echo - 打开图形窗口 (800x600)  
echo - 绘制填充图形 (黄色圆形, 红色矩形)
echo - 显示文本信息
echo - 等待用户输入后关闭
echo.
echo 编译命令示例:
echo cl /EHsc /I"include" src\shapes.cpp examples\simple_easyx_demo.cpp /Fe:bin\demo.exe /link graphics.lib
echo.
echo 项目结构符合现代C++标准:
echo - include\project-C\shapes.h   (形状处理库)
echo - include\easyx\graphics.h     (真正的EasyX头文件)
echo - src\shapes.cpp               (形状类实现)
echo - examples\simple_easyx_demo.cpp (18行演示代码)
echo.
pause