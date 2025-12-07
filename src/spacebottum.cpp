#include "allbuttons.h"    
#include <iostream>

// Helper: Check if mouse is in button
static bool isMouseInButton(int mx, int my, int x, int y, int w, int h) {
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
}

// Global variables definitions
bool sphere_creationClicked = false;
bool two_starsClicked = false;
bool solar_sysClicked = false;
bool block_slopeClicked = false;
bool block_on_boardClicked = false;

bool sphere_creationHovered = false;
bool two_starsHovered = false;
bool solar_sysHovered = false;
bool block_slopeHovered = false;
bool block_on_boardHovered = false;

// Local reset function
static void resetSceneButtons() {
    sphere_creationClicked = false;
    two_starsClicked = false;
    solar_sysClicked = false;
    block_slopeClicked = false;
    block_on_boardClicked = false;
}

// Getter implementations
bool getSphereCreationButtonState() { return sphere_creationClicked; }
bool getTwoStarsButtonState() { return two_starsClicked; }
bool getSolarSysButtonState() { return solar_sysClicked; }
bool getBlockSlopeButtonState() { return block_slopeClicked; }
bool getBlockOnBoardButtonState() { return block_on_boardClicked; }

// Draw function
void drawSceneModelButtons(int btnX, int sphere_creationBtnY, int two_starsBtnY, int solar_sysBtnY, int block_slopeBtnY, int block_on_boardBtnY, int btnW, int btnH) {
    
    // Helper lambda for drawing a single button
    auto drawBtn = [&](int y, bool hovered, const char* text) {
        if (hovered) setfillcolor(RGB(180, 200, 255));
        else setfillcolor(RGB(220, 220, 220));
        solidrectangle(btnX, y, btnX + btnW, y + btnH);
        settextcolor(RGB(255, 0, 0)); 
        setbkmode(TRANSPARENT);  
        settextstyle(18, 0, "Consolas");
        int tw = textwidth(text);
        int th = textheight(text);
        outtextxy(btnX + (btnW-tw)/2, y + (btnH-th)/2, text);
    };

    drawBtn(sphere_creationBtnY, sphere_creationHovered, "Slope Scene");
    drawBtn(two_starsBtnY, two_starsHovered, "Ball Collision");
    drawBtn(solar_sysBtnY, solar_sysHovered, "Stacking Demo");
    drawBtn(block_slopeBtnY, block_slopeHovered, "Slope Left");
    drawBtn(block_on_boardBtnY, block_on_boardHovered, "Block on Board");
}

// Input handling
void handleSceneModelMouseInput(int btnX, int sphere_creationBtnY, int two_starsBtnY, int solar_sysBtnY, int block_slopeBtnY, int block_on_boardBtnY, int btnW, int btnH) {
    ExMessage msg;
    while (peekmessage(&msg, EM_MOUSE)) {  
        if (msg.message == WM_MOUSEMOVE) {
            sphere_creationHovered = isMouseInButton(msg.x, msg.y, btnX, sphere_creationBtnY, btnW, btnH);
            two_starsHovered = isMouseInButton(msg.x, msg.y, btnX, two_starsBtnY, btnW, btnH);
            solar_sysHovered = isMouseInButton(msg.x, msg.y, btnX, solar_sysBtnY, btnW, btnH);
            block_slopeHovered = isMouseInButton(msg.x, msg.y, btnX, block_slopeBtnY, btnW, btnH);
            block_on_boardHovered = isMouseInButton(msg.x, msg.y, btnX, block_on_boardBtnY, btnW, btnH);
        }
        
        if (msg.message == WM_LBUTTONDOWN) {
            if (sphere_creationHovered) sphere_creationClicked = true;
            if (two_starsHovered) two_starsClicked = true;
            if (solar_sysHovered) solar_sysClicked = true;
            if (block_slopeHovered) block_slopeClicked = true;
            if (block_on_boardHovered) block_on_boardClicked = true;
        }
    }
}

// State checking
void checkSceneModelButtonStates() {
    if (getSphereCreationButtonState() || getTwoStarsButtonState() || getSolarSysButtonState() || getBlockSlopeButtonState() || getBlockOnBoardButtonState()) {
        resetSceneButtons();
    }
}

// Init
void initSceneModelButtons(int& btnX, int& sphere_creationBtnY, int& two_starsBtnY, int& solar_sysBtnY, int& block_slopeBtnY, int& block_on_boardBtnY, int& btnW, int& btnH, int margin) {
    btnW = 200;
    btnH = 50; 
    
    int startY = 450; 
    sphere_creationBtnY = startY;
    two_starsBtnY = startY + btnH + margin;
    solar_sysBtnY = startY + 2 * (btnH + margin);
    block_slopeBtnY = startY + 3 * (btnH + margin);
    block_on_boardBtnY = startY + 4 * (btnH + margin); // 450 + 280 = 730. Bottom at 780. Fits.
    
    btnX = 1000; 
}
