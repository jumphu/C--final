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
void colorSelect2(int idx); // <--- 修正：带参数 idx
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
