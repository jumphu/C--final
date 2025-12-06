#pragma once
// 基于你原始 background1.cpp，但移除了 BeginBatchDraw()/EndBatchDraw()
// 以便与 Renderer 的批量绘制协作。

#ifndef DIGITAL_RAIN_BACKGROUND_INTEGRATED_H
#define DIGITAL_RAIN_BACKGROUND_INTEGRATED_H

#include <graphics.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH_INT 800
#define SCREEN_HEIGHT_INT 600
#define DEF_RAIN_NUM_INT 53

class DigitalRainBackgroundIntegrated {
private:
    int rainPos[DEF_RAIN_NUM_INT];
    char rainArr[DEF_RAIN_NUM_INT][10];
    char getRandomChar();
    void initPos();
    void initRain();
public:
    DigitalRainBackgroundIntegrated();
    // 在 renderer.BeginFrame() 与 EndFrame() 之间被调用
    void UpdateAndDraw();
};

#endif

