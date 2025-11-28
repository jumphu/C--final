#pragma once
#include <graphics.h>

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void BeginFrame();
    void EndFrame();

    void DrawBall(int x, int y, int radius, COLORREF color);

private:
    int width, height;
};