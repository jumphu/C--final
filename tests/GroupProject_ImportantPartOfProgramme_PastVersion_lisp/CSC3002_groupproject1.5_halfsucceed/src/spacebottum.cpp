#include "allbottums.h"    
#include <iostream>

// 【重要修改】static 关键字
static bool isInButton(int mx, int my, int x, int y, int w, int h) {
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
}

bool sphere_creationClicked = false;
bool two_starsClicked = false;
bool solar_sysClicked = false;
bool sphere_creationHovered = false;
bool two_starsHovered = false;
bool solar_sysHovered = false;

// 改名：resetButtonStates -> resetSpaceButtons
void resetSpaceButtons() {
    sphere_creationClicked = false;
    two_starsClicked = false;
    solar_sysClicked = false;
}

bool getSphereCreationButtonState() { return sphere_creationClicked; }
bool getTwoStarsButtonState() { return two_starsClicked; }
bool getSolarSysButtonState()  { return solar_sysClicked; }

// 改名：drawButtons -> drawSpaceButtons
void drawSpaceButtons(int btnX, int sphere_creationBtnY, int two_starsBtnY, int solar_sysBtnY, int btnW, int btnH) {
    setfillcolor(sphere_creationHovered ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(btnX, sphere_creationBtnY, btnX + btnW, sphere_creationBtnY + btnH);
    
    setfillcolor(two_starsHovered ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(btnX, two_starsBtnY, btnX + btnW, two_starsBtnY + btnH);
    
    setfillcolor(solar_sysHovered ? RGB(180, 200, 255) : RGB(220, 220, 220));
    solidrectangle(btnX, solar_sysBtnY, btnX + btnW, solar_sysBtnY + btnH);
    
    settextcolor(RGB(255, 0, 0));
    setbkmode(TRANSPARENT); 
    settextstyle(18, 0, "Consolas");
    
    outtextxy(btnX + 75, sphere_creationBtnY + 21, "Sphere Creation");
    outtextxy(btnX + 75, two_starsBtnY + 21, "Two Stars");
    outtextxy(btnX + 75, solar_sysBtnY + 21, "Solar System");
}

// 改名：handleMouseInput -> handleSpaceMouse
void handleSpaceMouse(int btnX, int sphere_creationBtnY, int two_starsBtnY, int solar_sysBtnY, int btnW, int btnH) {
    ExMessage msg;   
    while (peekmessage(&msg, EM_MOUSE)) {  
        if (msg.message == WM_MOUSEMOVE) { 
            sphere_creationHovered = isInButton(msg.x, msg.y, btnX, sphere_creationBtnY, btnW, btnH);
            two_starsHovered = isInButton(msg.x, msg.y, btnX, two_starsBtnY, btnW, btnH);
            solar_sysHovered = isInButton(msg.x, msg.y, btnX, solar_sysBtnY, btnW, btnH);
        }
        if (msg.message == WM_LBUTTONDOWN) {  
            if (sphere_creationHovered) sphere_creationClicked = true;
            if (two_starsHovered) two_starsClicked = true;
            if (solar_sysHovered) solar_sysClicked = true;
        }
    }
}

// 改名：checkButtonStates -> checkSpaceButtonStates
void checkSpaceButtonStates() {
    if (getSphereCreationButtonState()) resetSpaceButtons();
    if (getTwoStarsButtonState()) resetSpaceButtons();
    if (getSolarSysButtonState()) resetSpaceButtons();
}

// 改名：initButtons -> initSpaceButtons
void initSpaceButtons(int& btnX, int& sphere_creationBtnY, int& two_starsBtnY, int& solar_sysBtnY, int& btnW, int& btnH, int margin) {
    btnW = 200;
    btnH = 60;
    // 注意：这里的坐标如果是写死的，Adapter初始化时可能会覆盖，或者按这里的逻辑走
    sphere_creationBtnY = 600 - btnH - margin;
    two_starsBtnY = 600;
    solar_sysBtnY = 600 + btnH + margin;
    btnX = (1920 - btnW) / 2;  
}