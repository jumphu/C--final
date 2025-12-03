//防止头文件被重复包含，起到保护作用
#ifndef DIGITAL_RAIN_BACKGROUND_H
//定义了一个宏名为DIGITAL_RAIN_BACKGROUND_H的宏
#define DIGITAL_RAIN_BACKGROUND_H

//需要包含以下头文件
#include <graphics.h>
#include <stdlib.h>
#include <time.h>

//定义3个常量，包括屏幕宽度、高度和默认雨滴数量
#define SCREEN_WIDTH 1366   //设置屏幕宽度为1366像素
#define SCREEN_HEIGHT 768   //设置屏幕高度为768像素
#define DEF_RAIN_NUM 100     //设置雨滴数量为100条

class DigitalRainBackground {
//私有成员变量和函数
private:
    //每条雨滴的y坐标
    int rainPos[DEF_RAIN_NUM]; 
    //每条雨滴的字符 
    char rainArr[DEF_RAIN_NUM][10]; 
    //生成字符 '0' 或 '1'
    char getRandomChar(); 
    //初始化雨滴位置
    void initPos();
    //初始化雨滴字符
    void initRain();

//公共成员函数
public:
    //构造函数，不返回值，而是构造对象本身，自动调用，只在创建对象时调用一次
    DigitalRainBackground();
    //更新并绘制雨滴
    void UpdateAndDraw();
};

#endif
//结束头文件
