#include "allbottums.h"    
#include <iostream>

// 判断鼠标是否在按钮范围内
bool isInButton(int mx, int my, int x, int y, int w, int h) {
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
    //鼠标坐标在左界以右，右界以左，上界以下，下界以上
}

// 定义3个全局变量来记录按钮状态，当用户点击后会变为true，之后再重置为false
bool sphere_creationClicked = false;
bool two_starsClicked = false;
bool solar_sysClicked = false;

// 定义3个全局变量来表示按钮悬停状态，鼠标悬停在按钮上对应的变量就变为true，鼠标移开就变为false
bool sphere_creationHovered = false;
bool two_starsHovered = false;
bool solar_sysHovered = false;

// 重置所有按钮点击状态，重新设置为未点击状态
void resetButtonStates() {
    sphere_creationClicked = false;
    two_starsClicked = false;
    solar_sysClicked = false;
}

// 用来获取按钮状态的函数，可以通过调用这些函数来将按钮接入到其他程序
bool getsphere_creationButtonState() { 
    return sphere_creationClicked; 
}

bool gettwo_starsButtonState() { 
    return two_starsClicked; 
}

bool getsolar_sysButtonState()  { 
    return solar_sysClicked; 
}

// 绘制所有按钮
//参数：按钮的x坐标，sphere_creation按钮的y坐标，two_stars按钮的y坐标，solar_sys按钮的y坐标，按钮宽度，按钮高度
void drawButtons(int btnX, int sphere_creationBtnY, int two_starsBtnY, int solar_sysBtnY, int btnW, int btnH) {
    
    // 绘制sphere_creation按钮
    if (sphere_creationHovered) //如果鼠标悬停在按钮上方，则sphere_creationHovered就是true，则执行下面的语句
        setfillcolor(RGB(180, 200, 255));  // 按钮填充为浅蓝色
    else  //sphere_creationHovered是false，表明鼠标没有悬停在按钮上方，则按钮填充为浅灰色
        setfillcolor(RGB(220, 220, 220));  // 按钮填充为浅灰色
    solidrectangle(btnX, sphere_creationBtnY, btnX + btnW, sphere_creationBtnY + btnH);
    //绘制一个实心的矩形。
    //变量：矩形左上角x坐标，矩形左上角y坐标，矩形右下角x坐标，矩形右下角y坐标
    
    // 绘制two_stars按钮
    if (two_starsHovered)
        setfillcolor(RGB(180, 200, 255));
    else
        setfillcolor(RGB(220, 220, 220));
    solidrectangle(btnX, two_starsBtnY, btnX + btnW, two_starsBtnY + btnH);
    
    // 绘制solar_sys按钮
    if (solar_sysHovered)
        setfillcolor(RGB(180, 200, 255));
    else
        setfillcolor(RGB(220, 220, 220));
    solidrectangle(btnX, solar_sysBtnY, btnX + btnW, solar_sysBtnY + btnH);
    
    // 绘制按钮文字
    settextcolor(RGB(255, 0, 0)); //设置字体颜色为红色
    setbkmode(TRANSPARENT);  //设置字体背景为透明
    settextstyle(18, 0, "Consolas");  //设置字体高度为18，宽度为0，字体为Consolas
                                      //使用Consolas等宽字体避免因字母数量不同导致单词长度差异太大
    
    //计算按钮文字的位置，并使其居中
    //（200 - 50）/2 = 75，（60 - 18）/2 = 21
    outtextxy(btnX + 75, sphere_creationBtnY + 21, "Sphere Creation");
    outtextxy(btnX + 75, two_starsBtnY + 21, "Two Stars");
    outtextxy(btnX + 75, solar_sysBtnY + 21, "Solar System");
}

// 处理所有鼠标输入并更新按钮状态
//参数：按钮的x坐标，sphere_creation按钮的y坐标，two_stars按钮的y坐标，solar_sys按钮的y坐标，按钮的宽度，按钮的高度
void handleMouseInput(int btnX, int sphere_creationBtnY, int two_starsBtnY, int solar_sysBtnY, int btnW, int btnH) {
    ExMessage msg;   // 定义一个ExMessage类型的变量msg
    while (peekmessage(&msg, EM_MOUSE)) {  
        // 处理鼠标移动
        if (msg.message == WM_MOUSEMOVE) { //检测鼠标是否移动
            //分别检测鼠标是否在3个按钮上
            sphere_creationHovered = isInButton(msg.x, msg.y, btnX, sphere_creationBtnY, btnW, btnH);
            two_starsHovered = isInButton(msg.x, msg.y, btnX, two_starsBtnY, btnW, btnH);
            solar_sysHovered = isInButton(msg.x, msg.y, btnX, solar_sysBtnY, btnW, btnH);
        }
        
        // 处理鼠标点击
        if (msg.message == WM_LBUTTONDOWN) {  //检测鼠标是否移动
            if (sphere_creationHovered) { //如果鼠标在按钮上，就为true，执行下面的语句
                sphere_creationClicked = true;  //将按钮状态设置为被点击
            }
            if (two_starsHovered) {
                two_starsClicked = true;
            }
            if (solar_sysHovered) {
                solar_sysClicked = true;
            }
        }
    }
}

// 按钮状态检查函数
void checkButtonStates() {
    if (getsphere_creationButtonState()) {
        resetButtonStates();
        //调用resetButtonStates函数，重置sphere_creation按钮点击状态为false
        // 这里接sphere_creation的代码，从这里启动第一个场景
    }

    if (gettwo_starsButtonState()) {
         resetButtonStates();
        //调用resetButtonStates函数，重置two_stars按钮点击状态为false
        // 这里接two_stars的代码，从这里启动第二个场景
    }

    if (getsolar_sysButtonState()) {
        resetButtonStates();
        ////调用resetButtonStates函数，重置solar_sys按钮点击状态为false
        // 这里接solar_sys的代码，从这里启动第三个场景
    }
}

//初始化函数
//参数：按钮的x坐标，sphere_creation按钮的y坐标，two_stars按钮的y坐标，solar_sys按钮的y坐标，按钮宽度，按钮高度，边距
void initButtons(int& btnX, int& sphere_creationBtnY, int& two_starsBtnY, int& solar_sysBtnY, int& btnW, int& btnH, int margin) {
    btnW = 200;
    btnH = 60;
    
    // 三个按钮的Y坐标
    sphere_creationBtnY = 600 - btnH - margin;
    two_starsBtnY = 600;
    solar_sysBtnY = 600 + btnH + margin;
    btnX = (1920 - btnW) / 2;  
}
//最后会返回3个bool值，如果为true，则被点击，false则没有被触发，后续可以通过检测这个bool值来判断选择了哪个场景
