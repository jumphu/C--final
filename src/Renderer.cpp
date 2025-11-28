#include "Renderer.h"

Renderer::Renderer(int width, int height) : width(width), height(height) {
    initgraph(width, height, EX_SHOWCONSOLE); // 打开窗口并允许控制台显示
}

Renderer::~Renderer() {
    closegraph();
}

void Renderer::BeginFrame() {
    // 使用 EasyX 双缓冲避免闪烁
    BeginBatchDraw();
    cleardevice();
}

void Renderer::EndFrame() {
    FlushBatchDraw();
    EndBatchDraw();
}

void Renderer::DrawBall(int x, int y, int radius, COLORREF color) {
    setfillcolor(color);
    solidcircle(x, y, radius);
}