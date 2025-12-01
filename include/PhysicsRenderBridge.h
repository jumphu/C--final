#ifndef PHYSICS_RENDER_BRIDGE_H
#define PHYSICS_RENDER_BRIDGE_H

#include <graphics.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "PhysicalWorld.h"

// 使用EasyX兼容的类型替代SFML类型
struct RenderState {
    POINT position;      // 使用POINT替代sf::Vector2f
    SIZE size;          
    COLORREF color;      // 使用COLORREF替代sf::Color
    std::string name;    
    std::string type;    
};

// 按钮状态结构
struct ButtonStates {
    bool start_clicked;
    bool pause_clicked; 
    bool stop_clicked;
    bool start_hovered;
    bool pause_hovered;
    bool stop_hovered;
};

// 音乐播放器状态
struct MusicState {
    bool is_playing;
    bool is_hovering;
    POINT position;
    int radius;
};

// 用户交互事件
struct UserInput {
    enum InputType {
        MOUSE_DRAG_START,
        MOUSE_DRAG_UPDATE, 
        MOUSE_DRAG_END,
        BUTTON_CLICK,
        PARAMETER_CHANGE,
        CREATE_SHAPE,
        DELETE_SHAPE,
        MUSIC_TOGGLE
    };
    
    InputType type;
    POINT mouse_position;  // 使用POINT
    std::string shape_name;
    std::string parameter_name;
    double parameter_value;
    std::string shape_type;
};

class PhysicsRenderBridge {
private:
    PhysicalWorld& physics_world_;
    double pixels_per_meter_;
    POINT world_offset_;  // 使用POINT替代sf::Vector2f
    
    std::unordered_map<std::string, RenderState> render_objects_;
    ButtonStates button_states_;
    MusicState music_state_;
    std::string currently_dragging_;
    POINT drag_offset_;

public:
    PhysicsRenderBridge(PhysicalWorld& physics_world);
    void initialize(double ppm = 100.0, POINT offset = {400, 300});
    
    // 坐标转换函数
    POINT physicsToScreen(double physics_x, double physics_y);
    POINT screenToPhysics(int screen_x, int screen_y);
    
    // 核心同步函数
    void syncPhysicsToRender();
    void syncInputToPhysics(const std::vector<UserInput>& user_inputs);
    
    // 状态更新
    void updateButtonStates(const ButtonStates& button_states);
    void updateMusicState(const MusicState& music_state);
    void handleButtonEvents();
    
    // 状态获取
    const std::unordered_map<std::string, RenderState>& getRenderStates() const;
    const ButtonStates& getButtonStates() const;
    const MusicState& getMusicState() const;
    
    // 形状管理
    void createRenderShape(const std::string& name, const std::string& type, COLORREF color = RGB(255, 255, 255));
    void removeRenderShape(const std::string& name);
    
    // 物理参数获取
    double getGravity() const;
    double getInclineAngle() const;
    double getFriction() const;
    double getTimeStep() const;
    bool isPaused() const;
    
    // 交互功能
    std::string findShapeAtPosition(POINT screen_pos, int tolerance = 10);

private:
    void applyUserInputToPhysics(const UserInput& input);
    void handleMouseDrag(const UserInput& input);
    void handleParameterChange(const UserInput& input);
    void handleShapeCreation(const UserInput& input);
    void handleButtonClick(const std::string& button_name);
    COLORREF getColorForShapeType(const std::string& type);
    void updateRenderStateFromShape(Shape* shape, RenderState& render_state);
};

#endif