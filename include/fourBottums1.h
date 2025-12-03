#ifndef ADD_BTN_H
#define ADD_BTN_H

#include <graphics.h>
#include <windows.h>

//函数声明
void initBtns(int w,int h);
void drawBtns();
void updBtns();
void colorSelect();
void numInput(const char* t,float* val);
void saveScr();
void restoreScr();

//接口
char* getColor();
float getGrav();
float getFric();
float getSpeedVal();

#endif
