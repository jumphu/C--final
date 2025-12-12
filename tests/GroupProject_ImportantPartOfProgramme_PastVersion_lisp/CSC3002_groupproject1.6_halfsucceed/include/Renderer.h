#pragma once
#include <graphics.h>
#include <windows.h>
#include <string>
#include <vector>
#include <cmath>

struct BallData {
    double x;     // m, world coords (center)
    double y;     // m
    double radius;// m
    double vx;    // m/s
    double vy;    // m/s
    double mass;  // kg
    COLORREF color;
};

struct RampData {
    double x1, y1; // meters, top point
    double x2, y2; // meters, bottom point
    double mu;     // friction coefficient
};

struct BlockData {
    double cx, cy;   // center in meters
    double width;    // m
    double height;   // m
    double angle;    // radians (counter-clockwise)
    double mass;
    double vx, vy;
    COLORREF color;
};

class Renderer {
public:
    Renderer(int width_px, int height_px, double scale_px_per_m);
    ~Renderer();

    void BeginFrame();
    void EndFrame();

    // draw helpers
    void Clear(COLORREF color = WHITE);
    void DrawText(const std::string& s, int x, int y, int fontsize = 16);
    void DrawButtonPlaceholder(int x, int y, int w, int h, const std::string& label);

    // world drawing
    void DrawBall(const BallData& b);
    void DrawRamp(const RampData& r);
    void DrawBlock(const BlockData& blk);

    // coord transform
    int WorldToScreenX(double wx) const;
    int WorldToScreenY(double wy) const;
    double ScreenToWorldX(int sx) const;
    double ScreenToWorldY(int sy) const;

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    double GetScale() const { return scale; }

private:
    int width, height;
    double scale; // pixels per meter

    // helper for polygon rotated rectangle
    void DrawRotatedRect(double cx, double cy, double w, double h, double angle, COLORREF color);
};
