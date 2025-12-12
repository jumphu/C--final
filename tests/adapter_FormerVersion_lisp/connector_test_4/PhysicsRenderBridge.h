#ifndef PHYSICS_RENDER_BRIDGE_H
#define PHYSICS_RENDER_BRIDGE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "PhysicalWorld.h"

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
    sf::Vector2f position;
    float radius;
};

// 渲染状态结构
struct RenderState {
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Color color;
    float rotation;
    bool is_selected;
    std::string name;
    std::string type;
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
    sf::Vector2f mouse_position;
    std::string shape_name;
    std::string parameter_name;
    double parameter_value;
    std::string shape_type;
};

class PhysicsRenderBridge {
private:
    PhysicalWorld& physics_world_;
    double pixels_per_meter_;
    sf::Vector2f world_offset_;
    std::unordered_map<std::string, RenderState> render_objects_;
    ButtonStates button_states_;
    MusicState music_state_;
    std::string currently_dragging_;
    sf::Vector2f drag_offset_;

public:
    PhysicsRenderBridge(PhysicalWorld& physics_world);
    void initialize(double ppm = 100.0, sf::Vector2f offset = sf::Vector2f(400, 300));
    
    // 坐标转换
    sf::Vector2f physicsToScreen(double physics_x, double physics_y);
    sf::Vector2f screenToPhysics(float screen_x, float screen_y);
    
    // 核心同步函数
    void syncPhysicsToRender();
    void syncInputToPhysics(const std::vector<UserInput>& user_inputs);
    
    // 按钮和音乐状态更新
    void updateButtonStates(const ButtonStates& button_states);
    void updateMusicState(const MusicState& music_state);
    void handleButtonEvents();
    
    // 获取状态
    const std::unordered_map<std::string, RenderState>& getRenderStates() const;
    const ButtonStates& getButtonStates() const;
    const MusicState& getMusicState() const;
    
    // 形状管理
    void createRenderShape(const std::string& name, const std::string& type, sf::Color color = sf::Color::White);
    void removeRenderShape(const std::string& name);
    
    // 物理参数获取
    double getGravity() const;
    double getInclineAngle() const;
    double getFriction() const;
    double getTimeStep() const;
    bool isPaused() const;
    
    // 交互功能
    std::string findShapeAtPosition(sf::Vector2f screen_pos, float tolerance = 10.0f);

private:
    void applyUserInputToPhysics(const UserInput& input);
    void handleMouseDrag(const UserInput& input);
    void handleParameterChange(const UserInput& input);
    void handleShapeCreation(const UserInput& input);
    void handleButtonClick(const std::string& button_name);
    sf::Color getColorForShapeType(const std::string& type);
    void updateRenderStateFromShape(Shape* shape, RenderState& render_state);
};

#endif