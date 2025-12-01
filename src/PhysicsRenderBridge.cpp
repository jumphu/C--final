#include "PhysicsRenderBridge.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>

// 坐标转换函数 - 注意EasyX的Y轴方向
POINT PhysicsRenderBridge::physicsToScreen(double physics_x, double physics_y) {
    int screen_x = static_cast<int>(physics_x * pixels_per_meter_ + world_offset_.x);
    int screen_y = static_cast<int>(-physics_y * pixels_per_meter_ + world_offset_.y); // Y轴翻转
    return {screen_x, screen_y};
}

POINT PhysicsRenderBridge::screenToPhysics(int screen_x, int screen_y) {
    double physics_x = (screen_x - world_offset_.x) / pixels_per_meter_;
    double physics_y = -(screen_y - world_offset_.y) / pixels_per_meter_; // Y轴翻转
    return {static_cast<int>(physics_x), static_cast<int>(physics_y)};
}

// 形状绘制函数
void PhysicsRenderBridge::renderShapes() {
    // 设置绘制属性
    setlinecolor(BLACK);
    setlinestyle(PS_SOLID, 2);
    
    for (const auto& pair : render_objects_) {
        const RenderState& state = pair.second;
        
        if (state.type == "circle" || state.type == "ball") {
            // 绘制圆形[citation:2]
            setfillcolor(state.color);
            fillcircle(state.position.x, state.position.y, state.size.cx / 2);
        } else if (state.type == "rectangle" || type == "box") {
            // 绘制矩形[citation:2]
            setfillcolor(state.color);
            fillrectangle(state.position.x - state.size.cx / 2, 
                         state.position.y - state.size.cy / 2,
                         state.position.x + state.size.cx / 2, 
                         state.position.y + state.size.cy / 2);
        }
    }
}

// 按钮事件处理
void PhysicsRenderBridge::handleButtonEvents() {
    if (button_states_.start_clicked) {
        handleButtonClick("start");
        button_states_.start_clicked = false;
    }
    
    if (button_states_.pause_clicked) {
        handleButtonClick("pause");
        button_states_.pause_clicked = false;
    }
    
    if (button_states_.stop_clicked) {
        handleButtonClick("stop");
        button_states_.stop_clicked = false;
    }
}

// 颜色映射函数
COLORREF PhysicsRenderBridge::getColorForShapeType(const std::string& type) {
    if (type == "circle" || type == "ball") return RED;
    if (type == "rectangle" || type == "box") return BLUE;
    if (type == "triangle") return GREEN;
    if (type == "pendulum") return YELLOW;
    return MAGENTA;
}