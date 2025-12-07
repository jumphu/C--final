//头文件开始
//头文件保护，防止重复包含，如果没有定义ADDITIONAL_BUTTONS_H，则定义它
#ifndef ADDITIONAL_BUTTONS_H

//定义ADDITIONAL_BUTTONS_H宏，表示该头文件已被包含
#define ADDITIONAL_BUTTONS_H

#include <graphics.h>
#include <windows.h>

//定义按钮的颜色常量
//定义按钮的颜色常量为浅灰色
const COLORREF BUTTON_COLOR = RGB(220, 220, 220);
//定义按钮悬停时的颜色常量为浅蓝色
const COLORREF HOVER_COLOR = RGB(180, 200, 255);
//定义文字颜色常量为红色
const COLORREF TEXT_COLOR = RGB(255, 0, 0);


// AdditionalButtons类，用于管理四个附加按钮的状态和行为
class AdditionalButtons {

//public区域，对外公开的函数和接口
public:
    //构造函数，初始化成员变量
    AdditionalButtons();
    //初始化按钮的位置和尺寸
    void initButtons(int windowWidth, int windowHeight, int margin = 20);
    //绘制按钮
    void drawButtons();
    //处理鼠标输入，更新按钮状态
    void handleMouseInput();
    //检查按钮状态并触发相应操作
    void checkButtonStates();

    //获取按钮状态的公有接口，供其他开发者使用
    //获取所选颜色
    const char* getColorChoice() const;
    //获取重力系数
    float getGravityCoefficient() const;
    //获取摩擦力系数
    float getFrictionCoefficient() const;
    //获取速度值
    float getSpeed() const;

//私有区域，仅供类内部使用的函数和变量
private:
    //检查鼠标是否在按钮范围内
    bool isInButton(int mx, int my, int x, int y, int w, int h) const;
    //重置所有按钮的点击状态
    void resetButtonStates();
    //显示颜色选项
    void showColorOptions();
    //显示输入框，提示用户输入数值
    void showInputBox(const char* prompt, float& value);

    //按钮的位置和尺寸
    //按钮的X坐标，颜色按钮的Y坐标，重力系数按钮的Y坐标，摩擦力系数按钮的Y坐标，速度按钮的Y坐标，按钮宽度，按钮高度
    int btnX, colorBtnY, gravityBtnY, frictionBtnY, speedBtnY, btnW, btnH;
    //各个按钮的点击状态状态
    bool isColorClicked, isGravityClicked, isFrictionClicked, isSpeedClicked;
    //各个按钮的悬停状态
    bool isColorHovered, isGravityHovered, isFrictionHovered, isSpeedHovered;
    
    //存储用户选择的颜色和输入的数值
    char colorChoice[10]; // Store the selected color
    //储存用户输入的重力系数、摩擦力系数和速度值
    float gravityCoefficient;
    float frictionCoefficient;
    float speed;
};

//头文件结束
#endif 
