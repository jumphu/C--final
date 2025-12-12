#include "allbottums.h"    
#include <iostream>

// 【重要修改】static 关键字
static bool isInButton(int mx, int my, int x, int y, int w, int h) {
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
}

bool model1Clicked = false;
bool model2Clicked = false;
bool solarSysClicked_M3 = false;
bool sphereCreationClicked_M3 = false;
bool twoStarsClicked_M3 = false;

bool model1Hovered = false;
bool model2Hovered = false;
bool solarSysHovered_M3 = false;
bool sphereCreationHovered_M3 = false;
bool twoStarsHovered_M3 = false;

// 改名：resetButtonStates -> resetChooseButtons
void resetChooseButtons() {
    model1Clicked = false;
    model2Clicked = false;
    solarSysClicked_M3 = false;
    sphereCreationClicked_M3 = false;
    twoStarsClicked_M3 = false;
}

bool getmodel1ButtonState() { return model1Clicked; }
bool getmodel2ButtonState() { return model2Clicked; }
bool getSolarSysButtonState_M3() { return solarSysClicked_M3; }
bool getSphereCreationButtonState_M3() { return sphereCreationClicked_M3; }
bool getTwoStarsButtonState_M3() { return twoStarsClicked_M3; }

// 改名：drawButtons -> drawChooseButtons
void drawChooseButtons(int btnX, int model1BtnY, int model2BtnY, int solarSysBtnY, int sphereCreationBtnY, int twoStarsBtnY, int btnW, int btnH) {
    // Model 1
    setfillcolor(model1Hovered ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(btnX, model1BtnY, btnX + btnW, model1BtnY + btnH);
    
    // Model 2
    setfillcolor(model2Hovered ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(btnX, model2BtnY, btnX + btnW, model2BtnY + btnH);
    
    // Solar System
    setfillcolor(solarSysHovered_M3 ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(btnX, solarSysBtnY, btnX + btnW, solarSysBtnY + btnH);
    
    // Sphere Creation
    setfillcolor(sphereCreationHovered_M3 ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(btnX, sphereCreationBtnY, btnX + btnW, sphereCreationBtnY + btnH);
    
    // Two Stars
    setfillcolor(twoStarsHovered_M3 ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(btnX, twoStarsBtnY, btnX + btnW, twoStarsBtnY + btnH);
    
    settextcolor(RGB(255, 0, 0));
    setbkmode(TRANSPARENT);
    settextstyle(18, 0, "Consolas");
    
    outtextxy(btnX + 60, model1BtnY + 21, "Model 1");
    outtextxy(btnX + 60, model2BtnY + 21, "Model 2");
    outtextxy(btnX + 40, solarSysBtnY + 21, "Model 3-Solar");
    outtextxy(btnX + 40, sphereCreationBtnY + 21, "Model 3-Sphere");
    outtextxy(btnX + 40, twoStarsBtnY + 21, "Model 3-Stars");
}

// 【修改】直接使用参数
void handleChooseMouse(int mx, int my, bool isDown, int btnX, int model1BtnY, int model2BtnY, int solarSysBtnY, int sphereCreationBtnY, int twoStarsBtnY, int btnW, int btnH) {
    // 1. 更新悬停状态
    model1Hovered = isInButton(mx, my, btnX, model1BtnY, btnW, btnH);
    model2Hovered = isInButton(mx, my, btnX, model2BtnY, btnW, btnH);
    solarSysHovered_M3 = isInButton(mx, my, btnX, solarSysBtnY, btnW, btnH);
    sphereCreationHovered_M3 = isInButton(mx, my, btnX, sphereCreationBtnY, btnW, btnH);
    twoStarsHovered_M3 = isInButton(mx, my, btnX, twoStarsBtnY, btnW, btnH);

    // 2. 处理点击
    if (isDown) {
        if (model1Hovered) model1Clicked = true;
        if (model2Hovered) model2Clicked = true;
        if (solarSysHovered_M3) solarSysClicked_M3 = true;
        if (sphereCreationHovered_M3) sphereCreationClicked_M3 = true;
        if (twoStarsHovered_M3) twoStarsClicked_M3 = true;
    }
}

// 改名：checkButtonStates -> checkChooseButtonStates
void checkChooseButtonStates() {
    if (getmodel1ButtonState()) resetChooseButtons();
    if (getmodel2ButtonState()) resetChooseButtons();
    if (getSolarSysButtonState_M3()) resetChooseButtons();
    if (getSphereCreationButtonState_M3()) resetChooseButtons();
    if (getTwoStarsButtonState_M3()) resetChooseButtons();
}

// src/choose.cpp 的末尾部分

void initChooseButtons(int& btnX, int& model1BtnY, int& model2BtnY, int& solarSysBtnY, int& sphereCreationBtnY, int& twoStarsBtnY, int& btnW, int& btnH, int margin) {
    btnW = 200;
    btnH = 50; // 稍微变小一点，放得下5个按钮
    
    // 1. 靠左显示
    btnX = 30;  
    
    // 2. 放在顶部控制按钮(Y=20~60)的下方
    // 从 Y=100 开始向下排列
    model1BtnY = 100;
    model2BtnY = model1BtnY + btnH + margin;
    solarSysBtnY = model2BtnY + btnH + margin;
    sphereCreationBtnY = solarSysBtnY + btnH + margin;
    twoStarsBtnY = sphereCreationBtnY + btnH + margin;
}