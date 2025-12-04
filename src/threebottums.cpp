#include "allbottums.h"    
#include <iostream>

// 判断鼠标是否在按钮范围内
bool isInButton(int mx, int my, int x, int y, int w, int h) {
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
    //鼠标坐标在左界以右，右界以左，上界以下，下界以上
}

// 定义3个全局变量来记录按钮状态，当用户点击后会变为true，之后再重置为false
bool isStartClicked = false;
bool isPauseClicked = false;
bool isStopClicked = false;

// 定义3个全局变量来表示按钮悬停状态，鼠标悬停在按钮上对应的变量就变为true，鼠标移开就变为false
bool isStartHovered = false;
bool isPauseHovered = false;
bool isStopHovered = false;

// 重置所有按钮点击状态，重新设置为未点击状态
void resetButtonStates() {
    isStartClicked = false;
    isPauseClicked = false;
    isStopClicked = false;
}

// 用来获取按钮状态的函数，可以通过调用这些函数来将按钮接入到其他程序
bool getStartButtonState() { 
    return isStartClicked; 
}

bool getPauseButtonState() { 
    return isPauseClicked; 
}

bool getStopButtonState()  { 
    return isStopClicked; 
}

// 绘制所有按钮
//参数：按钮的x坐标，开始按钮的y坐标，暂停按钮的y坐标，停止按钮的y坐标，按钮宽度，按钮高度
void drawButtons(int btnX, int startBtnY, int pauseBtnY, int stopBtnY, int btnW, int btnH) {
    
    // 绘制开始按钮
    if (isStartHovered) //如果鼠标悬停在按钮上方，则isStartHovered就是true，则执行下面的语句
        setfillcolor(RGB(180, 200, 255));  // 按钮填充为浅蓝色
    else  //isStartHovered是false，表明鼠标没有悬停在按钮上方，则按钮填充为浅灰色
        setfillcolor(RGB(220, 220, 220));  // 按钮填充为浅灰色
    solidrectangle(btnX, startBtnY, btnX + btnW, startBtnY + btnH);
    //绘制一个实心的矩形。
    //变量：矩形左上角x坐标，矩形左上角y坐标，矩形右下角x坐标，矩形右下角y坐标
    
    // 绘制暂停按钮
    if (isPauseHovered)
        setfillcolor(RGB(180, 200, 255));
    else
        setfillcolor(RGB(220, 220, 220));
    solidrectangle(btnX, pauseBtnY, btnX + btnW, pauseBtnY + btnH);
    
    // 绘制停止按钮
    if (isStopHovered)
        setfillcolor(RGB(180, 200, 255));
    else
        setfillcolor(RGB(220, 220, 220));
    solidrectangle(btnX, stopBtnY, btnX + btnW, stopBtnY + btnH);
    
    // 绘制按钮文字
    settextcolor(RGB(255, 0, 0)); //设置字体颜色为红色
    setbkmode(TRANSPARENT);  //设置字体背景为透明
    settextstyle(18, 0, "Consolas");  //设置字体高度为18，宽度为0，字体为Consolas
                                      //使用Consolas等宽字体避免因字母数量不同导致单词长度差异太大
    
    //计算按钮文字的位置，并使其居中
    //（200 - 50）/2 = 75，（60 - 18）/2 = 21
    outtextxy(btnX + 75, startBtnY + 21, "Start");
    outtextxy(btnX + 75, pauseBtnY + 21, "Pause");
    outtextxy(btnX + 75, stopBtnY + 21, "Stop");
}

// 处理所有鼠标输入并更新按钮状态
//参数：按钮的x坐标，开始按钮的y坐标，暂停按钮的y坐标，停止按钮的y坐标，按钮的宽度，按钮的高度
void handleMouseInput(int btnX, int startBtnY, int pauseBtnY, int stopBtnY, int btnW, int btnH) {
    ExMessage msg;   // 定义一个ExMessage类型的变量msg
    while (peekmessage(&msg, EM_MOUSE)) {  
        // 处理鼠标移动
        if (msg.message == WM_MOUSEMOVE) { //检测鼠标是否移动
            //分别检测鼠标是否在3个按钮上
            isStartHovered = isInButton(msg.x, msg.y, btnX, startBtnY, btnW, btnH);
            isPauseHovered = isInButton(msg.x, msg.y, btnX, pauseBtnY, btnW, btnH);
            isStopHovered = isInButton(msg.x, msg.y, btnX, stopBtnY, btnW, btnH);
        }
        
        // 处理鼠标点击
        if (msg.message == WM_LBUTTONDOWN) {  //检测鼠标是否移动
            if (isStartHovered) { //如果鼠标在按钮上，就为true，执行下面的语句
                isStartClicked = true;  //将按钮状态设置为被点击
            }
            if (isPauseHovered) {
                isPauseClicked = true;
            }
            if (isStopHovered) {
                isStopClicked = true;
            }
        }
    }
}

// 按钮状态检查函数
void checkButtonStates() {
    if (getStartButtonState()) {
        resetButtonStates();
        //调用resetButtonStates函数，重置开始按钮点击状态为false
        // 这里接开始的代码，从这里启动物理引擎
    }

    if (getPauseButtonState()) {
         resetButtonStates();
        //调用resetButtonStates函数，重置暂停按钮点击状态为false
        // 这里接暂停的代码，从这里暂停物理引擎
    }

    if (getStopButtonState()) {
        resetButtonStates();
        ////调用resetButtonStates函数，重置停止按钮点击状态为false
        // 这里接停止的代码，从这里停止物理引擎
    }
}

//初始化函数
//参数：按钮的x坐标，开始按钮的y坐标，暂停按钮的y坐标，停止按钮的y坐标，按钮宽度，按钮高度，边距
void initButtons(int& btnX, int& startBtnY, int& pauseBtnY, int& stopBtnY, int& btnW, int& btnH, int margin) {
    btnW = 200;
    btnH = 60;
    
    // 三个按钮的Y坐标
    startBtnY = 600 - btnH * 3 - margin * 2;
    pauseBtnY = 600 - btnH * 2 - margin;
    stopBtnY = 600 - btnH - margin;
    btnX = 800 - btnW - margin;  
}
