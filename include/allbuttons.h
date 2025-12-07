#ifndef ALL_BUTTONS_H
#define ALL_BUTTONS_H

#include <graphics.h>
#include <windows.h>

//////////////////////
// three buttons (start/pause/stop)
extern bool isStartClicked;
extern bool isPauseClicked;
extern bool isStopClicked;
extern bool isStartHovered;
extern bool isPauseHovered;
extern bool isStopHovered;

bool isInButton(int mx, int my, int x, int y, int w, int h);
void resetButtonStates();
bool getStartButtonState();
bool getPauseButtonState();
bool getStopButtonState();

void drawButtons(int startBtnX, int pauseBtnX, int stopBtnX, int btnY, int btnW, int btnH);
void handleMouseInput(int startBtnX, int pauseBtnX, int stopBtnX, int btnY, int btnW, int btnH);
void checkButtonStates();
void initButtons(int& startBtnX, int& pauseBtnX, int& stopBtnX, int& btnY, int& btnW, int& btnH);

// single-object scene (4 buttons)
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

// two-object scene (8 buttons)
void initBtns2(int w, int h);
void drawBtns2();
void updBtns2();
void colorSelect2(int idx); // <--- ÐÞÕý£º´ø²ÎÊý idx
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


// 三个按钮点击/悬停状态
extern bool model1Clicked;
extern bool model2Clicked;
extern bool model3Clicked;

extern bool model1Hovered;
extern bool model2Hovered;
extern bool model3Hovered;

// 判断鼠标是否在按钮范围内
bool isInButton(int mx, int my, int x, int y, int w, int h);

// 重置按钮状态
void resetButtonStates();

// 获取按钮状态
bool getmodel1ButtonState();
bool getmodel2ButtonState();
bool getmodel3ButtonState();

// 绘制按钮
void drawButtons(int btnX, int model1BtnY, int model2BtnY, int model3BtnY, int btnW, int btnH);

// 处理鼠标输入
void handleMouseInput(int btnX, int model1BtnY, int model2BtnY, int model3BtnY, int btnW, int btnH);

// 按钮状态检查
void checkButtonStates();

// 初始化按钮
void initButtons(int& btnX, int& model1BtnY, int& model2BtnY, int& model3BtnY, int& btnW, int& btnH, int margin);

//新加内容：
extern bool sphere_creationClicked;
extern bool two_starsClicked;
extern bool solar_sysClicked;
extern bool block_slopeClicked;

extern bool sphere_creationHovered;
extern bool two_starsHovered;
extern bool solar_sysHovered;
extern bool block_slopeHovered;

bool getSphereCreationButtonState();
bool getTwoStarsButtonState();
bool getSolarSysButtonState();
bool getBlockSlopeButtonState();

void drawSceneModelButtons(int btnX, int sphere_creationBtnY, int two_starsBtnY, int solar_sysBtnY, int block_slopeBtnY, int btnW, int btnH);
void handleSceneModelMouseInput(int btnX, int sphere_creationBtnY, int two_starsBtnY, int solar_sysBtnY, int block_slopeBtnY, int btnW, int btnH);
void checkSceneModelButtonStates();
void initSceneModelButtons(int& btnX, int& sphere_creationBtnY, int& two_starsBtnY, int& solar_sysBtnY, int& block_slopeBtnY, int& btnW, int& btnH, int margin = 20);

#endif



