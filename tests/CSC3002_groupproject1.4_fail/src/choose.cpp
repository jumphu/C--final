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

// 改名：handleMouseInput -> handleChooseMouse
void handleChooseMouse(int btnX, int model1BtnY, int model2BtnY, int model3BtnY, int btnW, int btnH) {
    ExMessage msg;   
    while (peekmessage(&msg, EM_MOUSE)) {  
        if (msg.message == WM_MOUSEMOVE) { 
            model1Hovered = isInButton(msg.x, msg.y, btnX, model1BtnY, btnW, btnH);
            model2Hovered = isInButton(msg.x, msg.y, btnX, model2BtnY, btnW, btnH);
            model3Hovered = isInButton(msg.x, msg.y, btnX, model3BtnY, btnW, btnH);
        }
        if (msg.message == WM_LBUTTONDOWN) {  
            if (model1Hovered) model1Clicked = true;
            if (model2Hovered) model2Clicked = true;
            if (model3Hovered) model3Clicked = true;
        }
    }
}

// 改名：checkButtonStates -> checkChooseButtonStates
void checkChooseButtonStates() {
    if (getmodel1ButtonState()) resetChooseButtons();
    if (getmodel2ButtonState()) resetChooseButtons();
    if (getmodel3ButtonState()) resetChooseButtons();
}

// 改名：initButtons -> initChooseButtons
void initChooseButtons(int& btnX, int& model1BtnY, int& model2BtnY, int& model3BtnY, int& btnW, int& btnH, int margin) {
    btnW = 200;
    btnH = 60;
    model1BtnY = 300 - btnH - margin;
    model2BtnY = 300;
    model3BtnY = 300 + btnH + margin;
    btnX = (800 - btnW) / 2;  
}