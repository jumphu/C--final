#ifndef BUTTONS_H  // 防止多重包含
#define BUTTONS_H

#include <graphics.h>  // EasyX库的核心头文件
#include <windows.h>   // 用于ExMessage等Windows API

// 声明全局变量（使用extern，避免在头文件中定义）
extern bool isStartClicked;
extern bool isPauseClicked;
extern bool isStopClicked;
extern bool isStartHovered;
extern bool isPauseHovered;
extern bool isStopHovered;

// 函数声明
bool isInButton(int mx, int my, int x, int y, int w, int h);
void resetButtonStates();
bool getStartButtonState();
bool getPauseButtonState();
bool getStopButtonState();
void drawButtons(int btnX, int startBtnY, int pauseBtnY, int stopBtnY, int btnW, int btnH);
void handleMouseInput(int btnX, int startBtnY, int pauseBtnY, int stopBtnY, int btnW, int btnH);
void checkButtonStates();
void initButtons(int& btnX, int& startBtnY, int& pauseBtnY, int& stopBtnY, int& btnW, int& btnH, int margin = 20);

#endif  // BUTTONS_H
