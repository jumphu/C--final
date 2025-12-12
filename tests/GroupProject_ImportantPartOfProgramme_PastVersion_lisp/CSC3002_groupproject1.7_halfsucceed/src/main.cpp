// main.cpp
#include <graphics.h>
#include <conio.h>     // 用于 _kbhit (虽然这里主要用 EasyX 消息)
#include <iostream>
#include "adapter.h"   // 你的适配器
#include "Renderer.h"  // 渲染器

// 屏幕尺寸配置
const int SCREEN_WIDTH = 1000;  // 可以根据你们的布局调整，spacebottum 里似乎假设了 1920？
const int SCREEN_HEIGHT = 800;
const double PIXELS_PER_METER = 20.0; // 1米 = 20像素

int main() {
    // 1. 初始化控制台（用于调试输出）
    // EasyX 的 initgraph 配合 EX_SHOWCONSOLE 可以同时显示图形和黑框
    // 注意：Renderer构造函数里已经调了 initgraph，所以这里其实不需要显式调用，
    // 但为了逻辑清晰，我们把窗口创建权交给 Renderer
    
    // 2. 创建核心模块
    // 创建渲染器：负责画图
    Renderer renderer(SCREEN_WIDTH, SCREEN_HEIGHT, PIXELS_PER_METER);
    
    // 创建适配器：负责连接物理和图形
    PhysicsVisualAdapter adapter(&renderer);
    
    // 3. 初始化适配器
    // 这里会初始化物理世界、UI位置、背景等
    if (!adapter.initialize(SCREEN_WIDTH, SCREEN_HEIGHT)) {
        std::cerr << "初始化失败！" << std::endl;
        return -1;
    }

    std::cout << "程序启动成功！按 ESC 退出。" << std::endl;

    // 4. 游戏主循环 (Game Loop)
    // 核心逻辑： 处理输入 -> 更新逻辑 -> 绘制画面 -> 帧率控制
    
    const int TARGET_FPS = 60;
    const int FRAME_DELAY = 1000 / TARGET_FPS; // 每帧约 16ms
    
    bool running = true;
    
    while (running) {
        // 记录帧开始时间
        DWORD startTime = GetTickCount();
        
        // --- A. 输入处理 (Input) ---
        ExMessage msg;
        while (peekmessage(&msg)) {
            // 1. 鼠标移动消息
            if (msg.message == WM_MOUSEMOVE) {
                adapter.onMouseMove(msg.x, msg.y);
            }
            // 2. 鼠标点击消息 (左键按下)
            else if (msg.message == WM_LBUTTONDOWN) {
                adapter.onMouseClick(msg.x, msg.y, true);
            }
            // 3. 鼠标松开消息 (结束拖拽)
            else if (msg.message == WM_LBUTTONUP) {
                adapter.onMouseDragEnd(msg.x, msg.y);
            }
            // 4. 键盘消息
            else if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_ESCAPE) {
                    running = false; // 按 ESC 退出主循环
                } else {
                    adapter.onKeyPress((char)msg.vkcode);
                }
            }
        }
        
        // 处理持续性的输入状态（如拖拽更新）
        // 如果正在拖拽，获取当前鼠标位置并更新物体
        if (adapter.getIsDragging()) {
             POINT pt;
             GetCursorPos(&pt); // 获取屏幕绝对坐标
             ScreenToClient(GetHWnd(), &pt); // 转换为窗口相对坐标
             adapter.onMouseDragUpdate(pt.x, pt.y);
        }

        // --- B. 逻辑更新 (Update) ---
        // 传入两帧之间的时间间隔 (秒)
        // 这里为了稳定性，传入固定的 1/60 秒，物理引擎内部也是按这个步长算的
        adapter.updateFrame(1.0f / TARGET_FPS);
        
        // --- C. 画面渲染 (Render) ---
        adapter.renderFrame();
        
        // --- D. 帧率控制 (Sleep) ---
        DWORD frameTime = GetTickCount() - startTime;
        if (frameTime < FRAME_DELAY) {
            Sleep(FRAME_DELAY - frameTime);
        }
    }

    // 5. 清理资源
    // 析构函数会自动调用 cleanup
    return 0;
}