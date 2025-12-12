#include "allbottums.h"    
#include <iostream>

// 【重要修改】static 关键字防止与其他文件的同名函数冲突
static bool isInButton(int mx, int my, int x, int y, int w, int h) {
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
}

bool isStartClicked = false;
bool isPauseClicked = false;
bool isStopClicked = false;

bool isStartHovered = false;
bool isPauseHovered = false;
bool isStopHovered = false;

// 改名：resetButtonStates -> resetCtrlButtons
void resetCtrlButtons() {
    isStartClicked = false;
    isPauseClicked = false;
    isStopClicked = false;
}

bool getStartButtonState() { return isStartClicked; }
bool getPauseButtonState() { return isPauseClicked; }
bool getStopButtonState()  { return isStopClicked; }

// 改名：drawButtons -> drawCtrlButtons
void drawCtrlButtons(int startBtnX, int pauseBtnX, int stopBtnX, int btnY, int btnW, int btnH) {
    // 绘制开始按钮
    setfillcolor(isStartHovered ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(startBtnX, btnY, startBtnX + btnW, btnY + btnH);
    
    // 绘制暂停按钮
    setfillcolor(isPauseHovered ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(pauseBtnX, btnY, pauseBtnX + btnW, btnY + btnH);
    
    // 绘制停止按钮
    setfillcolor(isStopHovered ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(stopBtnX, btnY, stopBtnX + btnW, btnY + btnH);
    
    // 绘制文字
    settextcolor(RGB(255, 0, 0)); 
    setbkmode(TRANSPARENT); 
    settextstyle(18, 0, "Consolas");
    
    outtextxy(startBtnX + 35, btnY + 11, "Start");
    outtextxy(pauseBtnX + 35, btnY + 11, "Pause");
    outtextxy(stopBtnX + 35, btnY + 11, "Stop");
}

// 【修改】不再自己读取消息，直接使用传入的 mx, my, isDown
void handleCtrlMouse(int mx, int my, bool isDown, int startBtnX, int pauseBtnX, int stopBtnX, int btnY, int btnW, int btnH) {
    // 1. 更新悬停状态 (Hover)
    isStartHovered = isInButton(mx, my, startBtnX, btnY, btnW, btnH);
    isPauseHovered = isInButton(mx, my, pauseBtnX, btnY, btnW, btnH);
    isStopHovered  = isInButton(mx, my, stopBtnX, btnY, btnW, btnH);

    // 2. 如果发生了点击 (isDown == true)，则更新点击状态
    if (isDown) {
        if (isStartHovered) isStartClicked = true;
        if (isPauseHovered) isPauseClicked = true;
        if (isStopHovered)  isStopClicked = true;
    }
}

// 改名：checkButtonStates -> checkCtrlButtonStates
void checkCtrlButtonStates() {
    if (getStartButtonState()) resetCtrlButtons();
    if (getPauseButtonState()) resetCtrlButtons();
    if (getStopButtonState()) resetCtrlButtons();
}

// 改名：initButtons -> initCtrlButtons
void initCtrlButtons(int& startBtnX, int& pauseBtnX, int& stopBtnX, int& btnY, int& btnW, int& btnH) {
    btnW = 120;
    btnH = 40;
    startBtnX = 140;
    pauseBtnX = 340;
    stopBtnX = 540;
    btnY = 20;  
}