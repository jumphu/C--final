#include "background_integrated.h"

char DigitalRainBackgroundIntegrated::getRandomChar() {
    return (rand() % 2) ? '1' : '0';
}

void DigitalRainBackgroundIntegrated::initPos() {
    for (int i = 0; i < DEF_RAIN_NUM_INT; i++) {
        rainPos[i] = rand() % SCREEN_HEIGHT_INT;
    }
}

void DigitalRainBackgroundIntegrated::initRain() {
    for (int i = 0; i < DEF_RAIN_NUM_INT; i++) {
        for (int j = 0; j < 10; j++) {
            rainArr[i][j] = getRandomChar();
        }
    }
}

DigitalRainBackgroundIntegrated::DigitalRainBackgroundIntegrated() {
    srand((unsigned int)time(NULL));
    initPos();
    initRain();
}

void DigitalRainBackgroundIntegrated::UpdateAndDraw() {
    // NOTE: 不再调用 BeginBatchDraw/EndBatchDraw
    // 只做绘制与状态更新，调用方负责 batch
    for (int i = 0; i < DEF_RAIN_NUM_INT; i++) {
        for (int j = 0; j < 10; j++) {
            int g = 255 - j * 25;
            settextcolor(RGB(0, g, 0));
            outtextxy(i * 15, rainPos[i] - j * 15, rainArr[i][j]);
        }
    }

    for (int i = 0; i < DEF_RAIN_NUM_INT; i++) {
        rainPos[i] += 15;
        if (rainPos[i] > SCREEN_HEIGHT_INT + 50) {
            rainPos[i] = 0;
            for (int j = 0; j < 10; j++) rainArr[i][j] = getRandomChar();
        }
    }
}
