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
void drawButtons(int btnX, int startBtnY, int pauseBtnY, int stopBtnY, int btnW, int btnH);
void handleMouseInput(int btnX, int startBtnY, int pauseBtnY, int stopBtnY, int btnW, int btnH);
void checkButtonStates();
void initButtons(int& btnX, int& startBtnY, int& pauseBtnY, int& stopBtnY, int& btnW, int& btnH, int margin);

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


#endif
