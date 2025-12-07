#include "allbuttons.h"    
#include <iostream>

// Helper: Check if mouse is in button
static bool isMouseInButton(int mx, int my, int x, int y, int w, int h) {
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
}

// Global variables definitions (declared extern in header)
bool sphere_creationClicked = false;
bool two_starsClicked = false;
bool solar_sysClicked = false;

bool sphere_creationHovered = false;
bool two_starsHovered = false;
bool solar_sysHovered = false;

// Local reset function
static void resetSceneButtons() {
    sphere_creationClicked = false;
    two_starsClicked = false;
    solar_sysClicked = false;
}

// Getter implementations
bool getSphereCreationButtonState() { 
    return sphere_creationClicked; 
}

bool getTwoStarsButtonState() { 
    return two_starsClicked; 
}

bool getSolarSysButtonState() { 
    return solar_sysClicked; 
}

// Draw function
void drawSceneModelButtons(int btnX, int sphere_creationBtnY, int two_starsBtnY, int solar_sysBtnY, int btnW, int btnH) {
    
    // Sphere Creation Button
    if (sphere_creationHovered) 
        setfillcolor(RGB(180, 200, 255));
    else  
        setfillcolor(RGB(220, 220, 220));
    solidrectangle(btnX, sphere_creationBtnY, btnX + btnW, sphere_creationBtnY + btnH);
    
    // Two Stars Button
    if (two_starsHovered)
        setfillcolor(RGB(180, 200, 255));
    else
        setfillcolor(RGB(220, 220, 220));
    solidrectangle(btnX, two_starsBtnY, btnX + btnW, two_starsBtnY + btnH);
    
    // Solar System Button
    if (solar_sysHovered)
        setfillcolor(RGB(180, 200, 255));
    else
        setfillcolor(RGB(220, 220, 220));
    solidrectangle(btnX, solar_sysBtnY, btnX + btnW, solar_sysBtnY + btnH);
    
    // Text
    settextcolor(RGB(255, 0, 0)); 
    setbkmode(TRANSPARENT);  
    settextstyle(18, 0, "Consolas");
    
    // Centering text manually approx
    outtextxy(btnX + (btnW-100)/2, sphere_creationBtnY + 20, "Slope Scene");
    outtextxy(btnX + (btnW-140)/2, two_starsBtnY + 20, "Ball Collision");
    outtextxy(btnX + (btnW-120)/2, solar_sysBtnY + 20, "Stacking Demo");
}

// Input handling
void handleSceneModelMouseInput(int btnX, int sphere_creationBtnY, int two_starsBtnY, int solar_sysBtnY, int btnW, int btnH) {
    ExMessage msg;
    while (peekmessage(&msg, EM_MOUSE)) {  
        if (msg.message == WM_MOUSEMOVE) {
            sphere_creationHovered = isMouseInButton(msg.x, msg.y, btnX, sphere_creationBtnY, btnW, btnH);
            two_starsHovered = isMouseInButton(msg.x, msg.y, btnX, two_starsBtnY, btnW, btnH);
            solar_sysHovered = isMouseInButton(msg.x, msg.y, btnX, solar_sysBtnY, btnW, btnH);
        }
        
        if (msg.message == WM_LBUTTONDOWN) {
            if (sphere_creationHovered) sphere_creationClicked = true;
            if (two_starsHovered) two_starsClicked = true;
            if (solar_sysHovered) solar_sysClicked = true;
        }
    }
}

// State checking
void checkSceneModelButtonStates() {
    if (getSphereCreationButtonState()) {
        resetSceneButtons();
    }
    if (getTwoStarsButtonState()) {
         resetSceneButtons();
    }
    if (getSolarSysButtonState()) {
        resetSceneButtons();
    }
}

// Init
void initSceneModelButtons(int& btnX, int& sphere_creationBtnY, int& two_starsBtnY, int& solar_sysBtnY, int& btnW, int& btnH, int margin) {
    btnW = 200;
    btnH = 60;
    
    // Layout logic
    sphere_creationBtnY = 600 - btnH - margin;
    two_starsBtnY = 600;
    solar_sysBtnY = 600 + btnH + margin;
    btnX = 1000; // Fixed X for now
}
