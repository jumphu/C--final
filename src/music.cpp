#include <music_player.h>
#include <graphics.h>
#include <windows.h>
#include <mmsystem.h>
#include <cmath>
#pragma comment(lib, "winmm.lib")
#include <stdio.h>


const char* musicFile = "音乐路径.mp3";  // 找到对应的文件
//定义了一个常量字符指针musicFile来储存文件路径

 //播放音乐的函数
void playMusic() {
    mciSendString("close mymusic", NULL, 0, NULL);  // 先关闭之前可能打开的音乐 
    char cmd[256];  // 创建命令字符串缓冲区
    sprintf(cmd, "open \"%s\" alias mymusic", musicFile);  // 使用全局的musicFile
    if (mciSendString(cmd, NULL, 0, NULL) == 0) {  
        mciSendString("play mymusic", NULL, 0, NULL);  // 播放音乐,执行打开命令，如果为0就是成功，反之失败
    }
}

// 音乐停止的函数
void stopMusic() {
    mciSendString("stop mymusic", NULL, 0, NULL);  // 停止播放
    //参数：关掉mymusic，不关心命令执行的返回信息，返回缓冲区大小为0，不需要回调通知
    mciSendString("close mymusic", NULL, 0, NULL);  // 关闭音乐文件
}

// 绘制喇叭图标
static void drawSpeakerIcon(int cx, int cy, int size, bool isPlaying) { 
    //喇叭主体
    setfillcolor(isPlaying ? RGB(70, 130, 180) : RGB(150, 150, 150)); //设置正在播放时填充蓝色，停止播放时填充灰色
    setlinecolor(RGB(0, 0, 0));  // 设置喇叭边框颜色为黑色
    setlinestyle(PS_SOLID, 2);  // 设置线条样式，实线，线条宽度为2
    
    // 定义喇叭主体的三角形四个顶点坐标
    POINT triangle[4] = {
        {cx - size/2, cy - size/3},  // 左上角
        {cx + size/2, cy},           // 右侧中点
        {cx - size/2, cy + size/3},  // 左下角
        {cx - size/2, cy - size/3}   // 回到左上角（闭合）
    };
    fillpolygon(triangle, 4);  // 绘制填充的三角形
    
    // 绘制矩形喇叭口
    setfillcolor(isPlaying ? RGB(100, 160, 210) : RGB(180, 180, 180));  // 设置喇叭口填充颜色
    //如果正在播放，则isPlaying为true，则填充为浅蓝色，否则填充为浅灰色
    solidrectangle(cx + size/2 - 5, cy - size/4, cx + size/2 + size/3, cy + size/4);
    //参数：矩形左上角x坐标，矩形左上角y坐标，矩形右下角x坐标，矩形右下角y坐标
    
    // 绘制声波
    if (isPlaying) {  //如果正在播放，则isPlaying为true，则执行下面语句
        setlinecolor(RGB(70, 130, 180));  // 设置声波颜色为蓝色
        setlinestyle(PS_SOLID, 3);  // 设置线条样式，实线，线条宽度为3
        
        // 绘制声波弧线
        for (int i = 0; i < 3; i++) {  //绘制3条声波弧线
            arc(cx + size/2 + size/3 + i*8, cy - size/2 + i*5,  
                cx + size/2 + size/3 + 30 + i*15, cy + size/2 - i*5,
                2.356, -0.785); //135度到-45度弧度，模拟声波
            //参数：矩形左上角x坐标，矩形左上角y坐标，矩形右下角x坐标，矩形右下角y坐标，起始点x坐标，起始点y坐标，结束点x坐标，结束点y坐标
        }
    }
}

// 判断鼠标是否在圆形按钮范围内
static bool isInCircleButton(int mx, int my, int cx, int cy, int radius) {
    //定义一个bool类型的函数isInCircleButton，参数为鼠标x坐标mx，鼠标y坐标my，圆心x坐标cx，圆心y坐标cy，圆形区域半径radius
    int dx = mx - cx;  //计算鼠标x坐标与圆心的水平距离
    int dy = my - cy;  //计算鼠标y坐标与圆心的垂直距离
    return (dx * dx + dy * dy <= radius * radius);  //// 判断距离平方是否小于半径平方
}

MusicPlayer::MusicPlayer(int x, int y, int r) 
    : posX(x), posY(y), radius(r), isPlaying(false), isHovering(false) {
}
  //初始化，把x，y，r赋值给成员变量posX，posY，radius，并将播放状态和悬停状态初始化为false

void MusicPlayer::Draw() {
    // 绘制按钮
    if (isPlaying) {  //如果正在播放，则isPlaying为true，则执行下面语句
        setfillcolor(RGB(200, 230, 255));  // 播放时的填充浅蓝色
    } else {
        setfillcolor(RGB(240, 240, 240));  // 停止时填充浅灰色
    }
    
    // 边框
    if (isHovering) {  //如果鼠标悬停在按钮上方，则isHovering就是true，则执行下面的语句
        setlinecolor(RGB(100, 100, 255));  // 设置边框颜色为深蓝色
        setlinestyle(PS_SOLID, 4);  // 设置边框为实线，宽度为4
    } else {
        setlinecolor(RGB(0, 0, 0)); // 设置边框颜色为黑色
        setlinestyle(PS_SOLID, 3);  // 设置边框为实线，宽度为3
    }
    
    fillcircle(posX, posY, radius);  // 绘制填充圆形按钮背景
    
    // 绘制喇叭图标
    drawSpeakerIcon(posX, posY, 80, isPlaying);  // 喇叭图标大小固定为80
}

//处理鼠标数据
//参数：鼠标x坐标mouseX，鼠标y坐标mouseY，消息类型messageType
bool MusicPlayer::HandleMouseInput(int mouseX, int mouseY, int messageType) {

//明确了一个名为HandleMouseInput的方法
    bool isMouseInButton = isInCircleButton(mouseX, mouseY, posX, posY, radius);
    // 调用isInCircleButton判断鼠标是否在按钮范围内，并赋值给名为isMouseInButton的bool变量

    switch (messageType) {
        case WM_MOUSEMOVE:
            // 如果鼠标数据是移动，则执行下面的语句
            isHovering = isMouseInButton;
            return isMouseInButton;  // 返回true表示这个移动事件与按钮相关
            
        case WM_LBUTTONDOWN:
            // 如果鼠标数据是左键按下，则执行下面的语句
            if (isMouseInButton) {  //如果鼠标在按钮范围内点击，则执行下面的语句
                // 切换播放状态
                if (isPlaying) {  //如果正在播放
                    stopMusic();  // 停止音乐
                    isPlaying = false;  // 更新播放状态，调整为false
                } else {
                    playMusic();  // 播放音乐
                    isPlaying = true;  // 更新播放状态，调整为true
                }
                return true;  // true表示点击已被处理
            }
            return false; // 点击未在按钮范围内，返回false
            
        default:
            return false; 
        //忽略其他鼠标信息
    }
}

//定义了一个名为IsPlaying的类方法
bool MusicPlayer::IsPlaying() {  //获取当前音乐的播放状态
    return isPlaying;   //将播放状态返回给isPlaying
}

//定义一个名为SetPosition的类方法
void MusicPlayer::SetPosition(int x, int y) {
    posX = x; //把x赋值给成员变量posX，即按钮中心的x坐标
    posY = y; //把y赋值给成员变量posY，即按钮中心的y坐标
}

//定义一个名为Cleanup的类方法
void MusicPlayer::Cleanup() {
    if (isPlaying) {  //如果正在播放音乐，则执行下面的语句
        stopMusic();  //停止音乐
    }
}
