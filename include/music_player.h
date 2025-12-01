#pragma once

class MusicPlayer {
private:
    int posX, posY;           // 按钮位置
    int radius;               // 按钮半径  
    bool isPlaying;           // 播放状态
    bool isHovering;          // 悬停状态
    
public:
    // 构造函数 - 设置按钮位置和大小
    MusicPlayer(int x = 320, int y = 80, int r = 60);
    
    // 绘制按钮（在主项目的绘制循环中调用）
    void Draw();
    
    // 处理鼠标输入（在主项目的消息循环中调用）
    // 返回true表示处理了该输入，false表示没有处理
    bool HandleMouseInput(int mouseX, int mouseY, int messageType);
    
    // 获取当前播放状态
    bool IsPlaying();
    
    // 设置按钮位置（如果需要动态调整）
    void SetPosition(int x, int y);
    
    // 清理资源（程序退出时调用）
    void Cleanup();
};
