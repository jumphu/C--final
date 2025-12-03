#ifndef ALL_BUTTONS_H
#define ALL_BUTTONS_H

#include <graphics.h>
#include <windows.h>

// 全局变量（外部声明）
extern bool isStartClicked;
extern bool isPauseClicked;
extern bool isStopClicked;
extern bool isStartHovered;
extern bool isPauseHovered;
extern bool isStopHovered;

// 函数声明
bool isInButton(int mx, int my, int x, int y, int w, int h);
void resetButtonStates();
bool getStartButtonState();
bool getPauseButtonState();
bool getStopButtonState();
void drawButtons(int btnX, int startBtnY, int pauseBtnY, int stopBtnY, int btnW, int btnH);
void handleMouseInput(int btnX, int startBtnY, int pauseBtnY, int stopBtnY, int btnW, int btnH);
void checkButtonStates();
void initButtons(int& btnX, int& startBtnY, int& pauseBtnY, int& stopBtnY, int& btnW, int& btnH, int margin);


void initBtns(int w,int h);
void drawBtns();
void updBtns();
void colorSelect();
void numInput(const char* t,float* val);
void saveScr();
void restoreScr();

// 接口
char* getColor();
float getGrav();
float getFric();
float getSpeedVal();

void initBtns2(int w,int h);
void drawBtns2();
void updBtns2();
void colorSelect2();
void numInput2(const char* t,float* val);
void saveScr2();
void restoreScr2();

// 接口
char* getColor1();
float getRadius1();
float getMass1();
float getSpeed1();

char* getColor2();
float getRadius2();
float getMass2();
float getSpeed2();

#endif
