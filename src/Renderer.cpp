#include "Renderer.h"
#include <sstream>

Renderer::Renderer(int width_px, int height_px, double scale_px_per_m)
    : width(width_px), height(height_px), scale(scale_px_per_m)
{
    // open window and keep console for debug
    initgraph(width, height, EX_SHOWCONSOLE);
    setbkcolor(WHITE);
    cleardevice();
    BeginBatchDraw();
}

Renderer::~Renderer() {
    EndBatchDraw();
    closegraph();
}

void Renderer::BeginFrame() {
    cleardevice();
}

void Renderer::EndFrame() {
    FlushBatchDraw();
    Sleep(16); // ~60 FPS
}

void Renderer::Clear(COLORREF color) {
    setbkcolor(color);
    cleardevice();
}

void Renderer::DrawText(const std::string& s, int x, int y, int fontsize) {
    settextstyle(fontsize, 0, "Consolas");
    outtextxy(x, y, s.c_str());
}

void Renderer::DrawButtonPlaceholder(int x, int y, int w, int h, const std::string& label) {
    // simple button look (placeholder for real UI)
    setlinecolor(BLACK);
    setfillcolor(RGB(240, 240, 240));
    solidroundrect(x, y, x + w, y + h, 6, 6);
    settextstyle(16, 0, "Consolas");
    int tw = textwidth(label.c_str());
    int th = textheight(label.c_str());
    int tx = x + (w - tw) / 2;
    int ty = y + (h - th) / 2;
    outtextxy(tx, ty, label.c_str());
}

void Renderer::DrawBall(const BallData& b) {
    int sx = WorldToScreenX(b.x);
    int sy = WorldToScreenY(b.y);
    int rpx = static_cast<int>(b.radius * scale + 0.5);
    setfillcolor(b.color);
    solidcircle(sx, sy, rpx);
    // outline
    setlinecolor(BLACK);
    circle(sx, sy, rpx);
}

void Renderer::DrawRamp(const RampData& r) {
    int x1 = WorldToScreenX(r.x1);
    int y1 = WorldToScreenY(r.y1);
    int x2 = WorldToScreenX(r.x2);
    int y2 = WorldToScreenY(r.y2);

    // Draw thicker ramp as polygon (small thickness)
    double dx = r.x2 - r.x1;
    double dy = r.y2 - r.y1;
    double len = sqrt(dx * dx + dy * dy);
    if (len < 1e-6) return;
    double nx = -dy / len; // normal
    double ny = dx / len;
    double t = 0.06; // thickness in meters
    POINT poly[4];
    poly[0].x = WorldToScreenX(r.x1 + nx * t);
    poly[0].y = WorldToScreenY(r.y1 + ny * t);
    poly[1].x = WorldToScreenX(r.x2 + nx * t);
    poly[1].y = WorldToScreenY(r.y2 + ny * t);
    poly[2].x = WorldToScreenX(r.x2 - nx * t);
    poly[2].y = WorldToScreenY(r.y2 - ny * t);
    poly[3].x = WorldToScreenX(r.x1 - nx * t);
    poly[3].y = WorldToScreenY(r.y1 - ny * t);

    setfillcolor(RGB(200, 160, 110));
    setlinecolor(BLACK);
    fillpolygon(poly, 4);
    polygon(poly, 4);
}

void Renderer::DrawBlock(const BlockData& blk) {
    DrawRotatedRect(blk.cx, blk.cy, blk.width, blk.height, blk.angle, blk.color);
}

int Renderer::WorldToScreenX(double wx) const {
    return static_cast<int>(width / 2.0 + wx * scale + 0.5);
}

int Renderer::WorldToScreenY(double wy) const {
    // world origin at (width/2, height-200); Y axis up
    return static_cast<int>((height - 200.0) - wy * scale + 0.5);
}

double Renderer::ScreenToWorldX(int sx) const {
    return (sx - width / 2.0) / scale;
}

double Renderer::ScreenToWorldY(int sy) const {
    return ((height - 200.0) - sy) / scale;
}

void Renderer::DrawRotatedRect(double cx, double cy, double w, double h, double angle, COLORREF color) {
    // compute 4 corners and draw filled polygon
    double hw = w / 2.0;
    double hh = h / 2.0;
    double c = cos(angle), s = sin(angle);
    double corners[4][2] = {
        { -hw, -hh},
        {  hw, -hh},
        {  hw,  hh},
        { -hw,  hh}
    };
    POINT poly[4];
    for (int i = 0; i < 4; ++i) {
        double rx = corners[i][0] * c - corners[i][1] * s + cx;
        double ry = corners[i][0] * s + corners[i][1] * c + cy;
        poly[i].x = WorldToScreenX(rx);
        poly[i].y = WorldToScreenY(ry);
    }
    setfillcolor(color);
    fillpolygon(poly, 4);
    setlinecolor(BLACK);
    polygon(poly, 4);
}