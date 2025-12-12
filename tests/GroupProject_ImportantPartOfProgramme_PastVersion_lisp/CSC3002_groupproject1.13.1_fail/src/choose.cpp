#include "allbottums.h"    
#include <iostream>

// 【重要修改】static 关键字
static bool isInButton(int mx, int my, int x, int y, int w, int h) {
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
}

bool model1Clicked = false;
bool model2Clicked = false;
bool model3Clicked = false;
bool model1Hovered = false;
bool model2Hovered = false;
bool model3Hovered = false;

// 改名：resetButtonStates -> resetChooseButtons
void resetChooseButtons() {
    model1Clicked = false;
    model2Clicked = false;
    model3Clicked = false;
}

bool getmodel1ButtonState() { return model1Clicked; }
bool getmodel2ButtonState() { return model2Clicked; }
bool getmodel3ButtonState() { return model3Clicked; }

// 改名：drawButtons -> drawChooseButtons
void drawChooseButtons(int btnX, int model1BtnY, int model2BtnY, int model3BtnY, int btnW, int btnH) {
    setfillcolor(model1Hovered ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(btnX, model1BtnY, btnX + btnW, model1BtnY + btnH);
    
    setfillcolor(model2Hovered ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(btnX, model2BtnY, btnX + btnW, model2BtnY + btnH);
    
    setfillcolor(model3Hovered ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(btnX, model3BtnY, btnX + btnW, model3BtnY + btnH);
    
    settextcolor(RGB(255, 0, 0));
    setbkmode(TRANSPARENT);
    settextstyle(18, 0, "Consolas");
    
    outtextxy(btnX + 75, model1BtnY + 21, "Model 1");
    outtextxy(btnX + 75, model2BtnY + 21, "Model 2");
    outtextxy(btnX + 75, model3BtnY + 21, "Model 3");
}

// 【修改】直接使用参数
void handleChooseMouse(int mx, int my, bool isDown, int btnX, int model1BtnY, int model2BtnY, int model3BtnY, int btnW, int btnH) {
    // 1. 更新悬停状态
    model1Hovered = isInButton(mx, my, btnX, model1BtnY, btnW, btnH);
    model2Hovered = isInButton(mx, my, btnX, model2BtnY, btnW, btnH);
    model3Hovered = isInButton(mx, my, btnX, model3BtnY, btnW, btnH);

    // 2. 处理点击
    if (isDown) {
        if (model1Hovered) model1Clicked = true;
        if (model2Hovered) model2Clicked = true;
        if (model3Hovered) model3Clicked = true;
    }
}

// 改名：checkButtonStates -> checkChooseButtonStates
void checkChooseButtonStates() {
    if (getmodel1ButtonState()) resetChooseButtons();
    if (getmodel2ButtonState()) resetChooseButtons();
    if (getmodel3ButtonState()) resetChooseButtons();
}

// src/choose.cpp 的末尾部分

void initChooseButtons(int& btnX, int& model1BtnY, int& model2BtnY, int& model3BtnY, int& btnW, int& btnH, int margin) {
    btnW = 200;
    btnH = 60;
    
    // 1. 靠左显示
    btnX = 30;  
    
    // 2. 放在顶部控制按钮(Y=20~60)的下方
    // 从 Y=100 开始向下排列
    model1BtnY = 100;
    model2BtnY = model1BtnY + btnH + margin;
    model3BtnY = model2BtnY + btnH + margin;
}