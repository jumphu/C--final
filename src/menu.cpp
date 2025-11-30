#include <graphics.h>
#include <windows.h>
#include <string>
#include "buttons.h"     // 引入按钮系统
#include "renderer.h"    // 引入绘图系统（你之前做的小球、斜坡等）

// 页面模式
enum PageState {
    PAGE_MENU,       // 选择模型（小球碰撞 / 斜坡滚下）
    PAGE_PARAM,      // 输入参数页面（这里只表示界面）
    PAGE_DEMO        // 演示页面（调用 renderer）
};

PageState currentPage = PAGE_MENU;

// ------------------------- 工具函数 -------------------------
void drawTitle(const char* text, int y, int size = 36) {
    settextstyle(size, 0, "Consolas");
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);

    int tw = textwidth(text);
    outtextxy((getwidth() - tw) / 2, y, text);
}

void drawOptionBox(int x, int y, int w, int h, const char* text, bool hover) {
    setfillcolor(hover ? RGB(200, 230, 255) : RGB(230, 230, 230));
    solidrectangle(x, y, x + w, y + h);

    settextstyle(24, 0, "Consolas");
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);

    int tw = textwidth(text);
    int th = textheight(text);
    outtextxy(x + (w - tw) / 2, y + (h - th) / 2, text);
}

// ------------------------- 页面一：模型选择 -------------------------
void drawMenuPage() {
    cleardevice();
    drawTitle("Select Simulation Model", 50);

    static bool hover1 = false, hover2 = false;

    int boxW = 320, boxH = 80;
    int x = (getwidth() - boxW) / 2;
    int y1 = 150, y2 = 260;

    // 鼠标检测
    ExMessage msg;
    while (peekmessage(&msg, EM_MOUSE)) {
        hover1 = (msg.x >= x && msg.x <= x + boxW && msg.y >= y1 && msg.y <= y1 + boxH);
        hover2 = (msg.x >= x && msg.x <= x + boxW && msg.y >= y2 && msg.y <= y2 + boxH);

        if (msg.message == WM_LBUTTONDOWN) {
            if (hover1) currentPage = PAGE_PARAM;  // 小球碰撞
            if (hover2) currentPage = PAGE_PARAM;  // 小球斜坡
        }
    }

    drawOptionBox(x, y1, boxW, boxH, "Ball Collision", hover1);
    drawOptionBox(x, y2, boxW, boxH, "Ball on Slope", hover2);
}

// ------------------------- 页面二：参数设置页面（展示用） -------------------------
void drawParameterPage() {
    cleardevice();
    drawTitle("Parameter Setup Page", 50, 32);

    settextstyle(20, 0, "Consolas");
    outtextxy(80, 150, "（你们组的参数输入模块会放在这里）");

    outtextxy(80, 200, "点击 Start 进入演示页面");
}

// ------------------------- 页面三：演示页面（集成按钮 + renderer） -------------------------
void drawDemoPage() {
    static bool firstEnter = true;

    // 按钮区域参数
    static int btnX, startY, pauseY, stopY, btnW, btnH;

    if (firstEnter) {
        initButtons(btnX, startY, pauseY, stopY, btnW, btnH);
        firstEnter = false;
    }

    // 绘图
    cleardevice();
    drawTitle("Simulation Running", 20, 28);

    // 绘制物体（renderer.cpp 负责）
    drawScene();

    // 绘制按钮
    drawButtons(btnX, startY, pauseY, stopY, btnW, btnH);
    handleMouseInput(btnX, startY, pauseY, stopY, btnW, btnH);

    // 根据按钮状态触发动作（你们组的物理模块会接这里）
    if (getStartButtonState()) {
        // 这里让物体重新开始
        resetSimulation();
        resetButtonStates();
    }
    if (getPauseButtonState()) {
        pauseSimulation();
        resetButtonStates();
    }
    if (getStopButtonState()) {
        stopSimulation();
        resetButtonStates();
    }
}

// ------------------------- 主渲染循环 -------------------------
int main() {
    initgraph(800, 600);
    setbkcolor(WHITE);
    cleardevice();

    while (true) {
        switch (currentPage) {
        case PAGE_MENU:
            drawMenuPage();
            break;
        case PAGE_PARAM:
            drawParameterPage();
            break;
        case PAGE_DEMO:
            drawDemoPage();
            break;
        }

        Sleep(10);
    }

    closegraph();
    return 0;
}