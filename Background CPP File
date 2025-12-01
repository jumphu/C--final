#include "DigitalRainBackground.h"
//包含头文件

// 生成随机字符 '0' 或 '1'
char DigitalRainBackground::getRandomChar() {
    return (rand() % 2) ?  '1' : '0';
    //利用随机函数random() % 2 生成随机的true/false，如果是true，后面的三元运算符输出1，否则输出0
}

// 初始化每条雨的位置
void DigitalRainBackground::initPos() {
    for (int i = 0; i < DEF_RAIN_NUM; i++) {
        //利用for循环语句生成雨滴，直到数量达到预设数量100条
        rainPos[i] = rand() % SCREEN_HEIGHT;
        //利用随机函数random()，随机生成每条雨滴的初始y坐标，范围在0到屏幕高度之间
    }  
}

// 初始化每条雨的字符
void DigitalRainBackground::initRain() {
    //遍历每条雨
    for (int i = 0; i < DEF_RAIN_NUM; i++) {
        //从0开始，如果数量未达到预设数量100条，则i+1，继续生成雨滴
        //对每条雨添加字符
        for (int j = 0; j < 10; j++) {
            //从0开始，如果字符数量未达到10，则j+1，继续生成字符
            rainArr[i][j] = getRandomChar();
            //调用之前定义的getRandomChar()函数，为每条雨滴生成10个随机字符 '0' 或 '1'
        }
    }
}

// 构造函数
DigitalRainBackground::DigitalRainBackground() {
    srand(time(NULL)); 
    //利用time(NULL)来使用当前时间，通过srand函数来将其设为随机数种子，确保每次生成的随机数序列不同，因为时间是不会重复的
    initPos();
    //调用initPos()函数来初始化每条雨滴的位置
    initRain();
    //调用initRain()函数来初始化每条雨滴的字符
}


// 更新并绘制雨滴
void DigitalRainBackground::UpdateAndDraw() {

    BeginBatchDraw();
    //用来修改原来存在的闪烁问题，使动画更流畅
    //将逐个绘制的过程合并为一个批次进行绘制，减少屏幕刷新次数，从而减少闪烁现象，提高动画的流畅度
    
    //绘制雨滴
    for (int i = 0; i < DEF_RAIN_NUM; i++) {
        //遍历每条雨滴

        for (int j = 0; j < 10; j++) {
            //遍历雨滴的10个字符
            
            //计算颜色，实现渐变效果
            int g = 255 - j * 25; //根据字符位置计算绿色分量，越往上颜色越暗
            settextcolor(RGB(0, g, 0));
            //将计算得到的绿色分量g应用到文本颜色中，形成从亮绿色到暗绿色的渐变效果

            outtextxy(i * 15, rainPos[i] - j * 15, rainArr[i][j]);
            //利用outtextxy函数在屏幕上绘制字符，第一个参数是x坐标，第二个参数是y坐标，第三个参数是要绘制的字符，即第i条雨滴的第j个字符
            //x坐标是雨滴数i乘15，使得每条雨滴之间间隔15
            //y坐标通过rainPos[i] - j * 15计算得到，rainPos[i]是雨滴的当前y坐标，j * 15是每个字符之间的垂直间隔
        }
    }
    
    //更新雨滴位置，使其实现下落效果
    for (int i = 0; i < DEF_RAIN_NUM; i++) {
        //遍历每条雨滴
        rainPos[i] += 15;
        //每次更新时，将每条雨滴的y坐标增加15个像素，模拟出雨滴向下落的效果

        //边界检测
        if (rainPos[i] > SCREEN_HEIGHT + 50) { 
            //如果雨滴的y坐标超过屏幕高度+50，则雨滴已经完全落出屏幕之外，执行下面的语句 
            rainPos[i] = 0;    //重置操作
            //将雨滴的y坐标重置为0，使其重新从屏幕顶部开始下落
            for (int j = 0; j < 10; j++) {
                //为重置的雨滴重新生成10个随机字符
                rainArr[i][j] = getRandomChar();
            }
        }
    }

    EndBatchDraw();
    //结束批量绘制，更新屏幕显示内容

}
