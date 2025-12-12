#include <graphics.h>
#include <conio.h>
#include <iostream>
#include "PhysicsRenderBridge.h"

// 简单的按钮实现[citation:2]
class EasyXButton {
private:
    int x, y, width, height;
    std::string label;
    bool isHovered;
    bool isClicked;
    
public:
    EasyXButton(int x, int y, int w, int h, std::string text) 
        : x(x), y(y), width(w), height(h), label(text), isHovered(false), isClicked(false) {}
    
    void update(const MOUSEMSG& mouse) {
        // 检测悬停
        isHovered = (mouse.x >= x && mouse.x <= x + width && 
                    mouse.y >= y && mouse.y <= y + height);
        
        // 检测点击
        if (isHovered && mouse.mkLButton) {
            isClicked = true;
        } else {
            isClicked = false;
        }
        
        // 绘制按钮[citation:2]
        if (isClicked) {
            setfillcolor(RGB(100, 100, 100));
        } else if (isHovered) {
            setfillcolor(RGB(200, 200, 200));
        } else {
            setfillcolor(RGB(150, 150, 150));
        }
        
        fillrectangle(x, y, x + width, y + height);
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);
        outtextxy(x + 10, y + (height - textheight(label.c_str())) / 2, label.c_str());
    }
    
    bool isButtonClicked() const { return isClicked; }
    bool isButtonHovered() const { return isHovered; }
};

int main() {
    // 初始化图形窗口[citation:2]
    initgraph(800, 600, EX_SHOWCONSOLE);
    setbkcolor(WHITE);
    cleardevice();
    
    // 创建物理世界和接口桥梁
    PhysicalWorld physics_world;
    PhysicsRenderBridge bridge(physics_world);
    bridge.initialize(100.0, {400, 300});
    
    // 创建UI元素
    EasyXButton startButton(50, 50, 100, 40, "Start");
    EasyXButton pauseButton(50, 100, 100, 40, "Pause"); 
    EasyXButton stopButton(50, 150, 100, 40, "Stop");
    
    // 主循环
    while (true) {
        // 处理消息
        if (MouseHit()) {
            MOUSEMSG mouse = GetMouseMsg();
            
            // 更新按钮状态
            startButton.update(mouse);
            pauseButton.update(mouse);
            stopButton.update(mouse);
             
            // 处理用户输入
            std::vector<UserInput> user_inputs;
            
            // 处理按钮点击
            if (startButton.isButtonClicked()) {
                UserInput start_event;
                start_event.type = UserInput::BUTTON_CLICK;
                start_event.parameter_name = "start";
                user_inputs.push_back(start_event);
            }
            
            // 处理鼠标拖动等其它事件...
        }
        
        // 更新按钮状态
        ButtonStates button_states;
        button_states.start_clicked = startButton.isButtonClicked();
        button_states.pause_clicked = pauseButton.isButtonClicked();
        button_states.stop_clicked = stopButton.isButtonClicked();
        button_states.start_hovered = startButton.isButtonHovered();
        button_states.pause_hovered = pauseButton.isButtonHovered();
        button_states.stop_hovered = stopButton.isButtonHovered();
        
        bridge.updateButtonStates(button_states);
        bridge.handleButtonEvents();
        
        // 更新物理世界
        if (!bridge.isPaused()) {
            physics_world.update(physics_world.dynamicShapeList, physics_world.ground);
        }
        
        // 同步和渲染
        bridge.syncPhysicsToRender();
        
        // 清除并重绘
        cleardevice();
        
        // 渲染物理形状
        bridge.renderShapes();
        
        // 渲染UI
        startButton.update(GetMouseMsg());
        pauseButton.update(GetMouseMsg());
        stopButton.update(GetMouseMsg());
        
        // 控制帧率
        Sleep(10);
    }
    
    closegraph();
    return 0;
}