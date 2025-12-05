#include "allbottums.h"    
#include <iostream>

// 判断鼠标是否在按钮范围内
bool isInButton(int mx, int my, int x, int y, int w, int h) {
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
    //鼠标坐标在左界以右，右界以左，上界以下，下界以上
}

// 定义3个全局变量来记录按钮状态，当用户点击后会变为true，之后再重置为false
bool model1Clicked = false;
bool model2Clicked = false;
bool model3Clicked = false;

// 定义3个全局变量来表示按钮悬停状态，鼠标悬停在按钮上对应的变量就变为true，鼠标移开就变为false
bool model1Hovered = false;
bool model2Hovered = false;
bool model3Hovered = false;

// 重置所有按钮点击状态，重新设置为未点击状态
void resetButtonStates() {
    model1Clicked = false;
    model2Clicked = false;
    model3Clicked = false;
}

// 用来获取按钮状态的函数，可以通过调用这些函数来将按钮接入到其他程序
bool getmodel1ButtonState() { 
    return model1Clicked; 
}

bool getmodel2ButtonState() { 
    return model2Clicked; 
}

bool getmodel3ButtonState()  { 
    return model3Clicked; 
}

// 绘制所有按钮
//参数：按钮的x坐标，model1按钮的y坐标，model2按钮的y坐标，model3按钮的y坐标，按钮宽度，按钮高度
void drawButtons(int btnX, int model1BtnY, int model2BtnY, int model3BtnY, int btnW, int btnH) {
    
    // 绘制model1按钮
    if (model1Hovered) //如果鼠标悬停在按钮上方，则model1Hovered就是true，则执行下面的语句
        setfillcolor(RGB(180, 200, 255));  // 按钮填充为浅蓝色
    else  //model1Hovered是false，表明鼠标没有悬停在按钮上方，则按钮填充为浅灰色
        setfillcolor(RGB(220, 220, 220));  // 按钮填充为浅灰色
    solidrectangle(btnX, model1BtnY, btnX + btnW, model1BtnY + btnH);
    //绘制一个实心的矩形。
    //变量：矩形左上角x坐标，矩形左上角y坐标，矩形右下角x坐标，矩形右下角y坐标
    
    // 绘制model2按钮
    if (model2Hovered)
        setfillcolor(RGB(180, 200, 255));
    else
        setfillcolor(RGB(220, 220, 220));
    solidrectangle(btnX, model2BtnY, btnX + btnW, model2BtnY + btnH);
    
    // 绘制model3按钮
    if (model3Hovered)
        setfillcolor(RGB(180, 200, 255));
    else
        setfillcolor(RGB(220, 220, 220));
    solidrectangle(btnX, model3BtnY, btnX + btnW, model3BtnY + btnH);
    
    // 绘制按钮文字
    settextcolor(RGB(255, 0, 0)); //设置字体颜色为红色
    setbkmode(TRANSPARENT);  //设置字体背景为透明
    settextstyle(18, 0, "Consolas");  //设置字体高度为18，宽度为0，字体为Consolas
                                      //使用Consolas等宽字体避免因字母数量不同导致单词长度差异太大
    
    //计算按钮文字的位置，并使其居中
    //（200 - 50）/2 = 75，（60 - 18）/2 = 21
    outtextxy(btnX + 75, model1BtnY + 21, "Model 1");
    outtextxy(btnX + 75, model2BtnY + 21, "Model 2");
    outtextxy(btnX + 75, model3BtnY + 21, "Model 3");
}

// 处理所有鼠标输入并更新按钮状态
//参数：按钮的x坐标，model1按钮的y坐标，model2按钮的y坐标，model3按钮的y坐标，按钮的宽度，按钮的高度
void handleMouseInput(int btnX, int model1BtnY, int model2BtnY, int model3BtnY, int btnW, int btnH) {
    ExMessage msg;   // 定义一个ExMessage类型的变量msg
    while (peekmessage(&msg, EM_MOUSE)) {  
        // 处理鼠标移动
        if (msg.message == WM_MOUSEMOVE) { //检测鼠标是否移动
            //分别检测鼠标是否在3个按钮上
            model1Hovered = isInButton(msg.x, msg.y, btnX, model1BtnY, btnW, btnH);
            model2Hovered = isInButton(msg.x, msg.y, btnX, model2BtnY, btnW, btnH);
            model3Hovered = isInButton(msg.x, msg.y, btnX, model3BtnY, btnW, btnH);
        }
        
        // 处理鼠标点击
        if (msg.message == WM_LBUTTONDOWN) {  //检测鼠标是否移动
            if (model1Hovered) { //如果鼠标在按钮上，就为true，执行下面的语句
                model1Clicked = true;  //将按钮状态设置为被点击
            }
            if (model2Hovered) {
                model2Clicked = true;
            }
            if (model3Hovered) {
                model3Clicked = true;
            }
        }
    }
}

// 按钮状态检查函数
void checkButtonStates() {
    if (getmodel1ButtonState()) {
        resetButtonStates();
        //调用resetButtonStates函数，重置model1按钮点击状态为false
        // 这里接model1的代码，从这里启动第一个场景
    }

    if (getmodel2ButtonState()) {
         resetButtonStates();
        //调用resetButtonStates函数，重置model2按钮点击状态为false
        // 这里接model2的代码，从这里启动第二个场景
    }

    if (getmodel3ButtonState()) {
        resetButtonStates();
        ////调用resetButtonStates函数，重置model3按钮点击状态为false
        // 这里接model3的代码，从这里启动第三个场景
    }
}

//初始化函数
//参数：按钮的x坐标，model1按钮的y坐标，model2按钮的y坐标，model3按钮的y坐标，按钮宽度，按钮高度，边距
void initButtons(int& btnX, int& model1BtnY, int& model2BtnY, int& model3BtnY, int& btnW, int& btnH, int margin) {
    btnW = 200;
    btnH = 60;
    
    // 三个按钮的Y坐标
    model1BtnY = 300 - btnH - margin;
    model2BtnY = 300;
    model3BtnY = 300 + btnH + margin;
    btnX = (800 - btnW) / 2;  
}
//最后会返回3个bool值，如果为true，则被点击，false则没有被触发，后续可以通过检测这个bool值来判断选择了哪个场景
