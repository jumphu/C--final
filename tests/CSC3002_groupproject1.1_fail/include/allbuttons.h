#ifndef ALL_BUTTONS_H
#define ALL_BUTTONS_H

#include <graphics.h>
#include <windows.h>

// ==========================================
// 1. 控制按钮 (Start/Pause/Stop) - 对应 threebuttons.cpp
// ==========================================
extern bool isStartClicked;
extern bool isPauseClicked;
extern bool isStopClicked;
extern bool isStartHovered;
extern bool isPauseHovered;
extern bool isStopHovered;

void resetCtrlButtons(); // 原 resetButtonStates
bool getStartButtonState();
bool getPauseButtonState();
bool getStopButtonState();

void drawCtrlButtons(int startBtnX, int pauseBtnX, int stopBtnX, int btnY, int btnW, int btnH);
void handleCtrlMouse(int startBtnX, int pauseBtnX, int stopBtnX, int btnY, int btnW, int btnH);
void checkCtrlButtonStates();
void initCtrlButtons(int& startBtnX, int& pauseBtnX, int& stopBtnX, int& btnY, int& btnW, int& btnH);


// ==========================================
// 2. 场景选择按钮1 (Model 1/2/3) - 对应 choose.cpp
// ==========================================
extern bool model1Clicked;
extern bool model2Clicked;
extern bool model3Clicked;
extern bool model1Hovered;
extern bool model2Hovered;
extern bool model3Hovered;

void resetChooseButtons(); // 原 resetButtonStates
bool getmodel1ButtonState();
bool getmodel2ButtonState();
bool getmodel3ButtonState();

void drawChooseButtons(int btnX, int model1BtnY, int model2BtnY, int model3BtnY, int btnW, int btnH);
void handleChooseMouse(int btnX, int model1BtnY, int model2BtnY, int model3BtnY, int btnW, int btnH);
void checkChooseButtonStates();
void initChooseButtons(int& btnX, int& model1BtnY, int& model2BtnY, int& model3BtnY, int& btnW, int& btnH, int margin);


// ==========================================
// 3. 场景选择按钮2 (Space System) - 对应 spacebottom.cpp
// ==========================================
extern bool sphere_creationClicked;
extern bool two_starsClicked;
extern bool solar_sysClicked;
extern bool sphere_creationHovered;
extern bool two_starsHovered;
extern bool solar_sysHovered;

void resetSpaceButtons(); // 原 resetButtonStates
bool getSphereCreationButtonState();
bool getTwoStarsButtonState();
bool getSolarSysButtonState();

void drawSpaceButtons(int btnX, int sphere_creationBtnY, int two_starsBtnY, int solar_sysBtnY, int btnW, int btnH);
void handleSpaceMouse(int btnX, int sphere_creationBtnY, int two_starsBtnY, int solar_sysBtnY, int btnW, int btnH);
void checkSpaceButtonStates();
void initSpaceButtons(int& btnX, int& sphere_creationBtnY, int& two_starsBtnY, int& solar_sysBtnY, int& btnW, int& btnH, int margin = 20);


// ==========================================
// 4. 参数调节 (单物体场景) - 对应 fourButtons1.cpp
// ==========================================
void initBtns(int w, int h);
void drawBtns();
void updBtns();
void colorSelect();
void numInput(const char* t, float* val);
void saveScr();
void restoreScr();

char* getColor();
float getGrav();
float getFric();
float getSpeedVal();


// ==========================================
// 5. 参数调节 (双物体场景) - 对应 fourbuttons2_fixed.cpp
// ==========================================
void initBtns2(int w, int h);
void drawBtns2();
void updBtns2();
void colorSelect2(int idx);
void numInput2(const char* t, float* val);
void saveScr2();
void restoreScr2();

char* getColor1();
float getRadius1();
float getMass1();
float getSpeed1();

char* getColor2();
float getRadius2();
float getMass2();
float getSpeed2();

#endif