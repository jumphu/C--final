#ifndef ADD_BTN2_H
#define ADD_BTN2_H

#include <graphics.h>
#include <windows.h>

//函数声明
void initBtns2(int w,int h);
void drawBtns2();
void updBtns2();
void colorSelect2();
void numInput2(const char* t,float* val);
void saveScr2();
void restoreScr2();

//接口
char* getColor1();
float getRadius1();
float getMass1();
float getSpeed1();
char* getColor2();
float getRadius2();
float getMass2();
float getSpeed2();

#endif
