// PhysicsRenderBridge.h
#ifndef PHYSICS_RENDER_BRIDGE_H
#define PHYSICS_RENDER_BRIDGE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

// 物理对象状态 - 从第一部分接收
struct PhysicsState {
    double pos_x, pos_y;        // 位置
    double velocity_x, velocity_y; // 速度
    double acceleration_x, acceleration_y; // 加速度
    double angle, angular_velocity; // 角度和角速度
    double mass;                // 质量
    bool is_static;             // 是否静态物体
    int shape_type;             // 形状类型: 0=圆形, 1=矩形
    double radius;              // 圆形半径
    double width, height;       // 矩形宽高
};

// 渲染对象状态 - 发送到第二部分
struct RenderState {
    sf::Vector2f position;      // 屏幕位置
    sf::Vector2f size;          // 尺寸
    sf::Color color;            // 颜色
    float rotation;             // 旋转角度
    bool is_selected;           // 是否被选中
};

// 用户交互事件 - 从第二部分接收
struct UserInput {
    enum InputType {
        MOUSE_DRAG_START,
        MOUSE_DRAG_UPDATE, 
        MOUSE_DRAG_END,
        BUTTON_CLICK,
        PARAMETER_CHANGE
    };
    
    InputType type;
    sf::Vector2f mouse_position;
    int object_id;              // 操作的物体ID
    std::string parameter_name; // 参数名
    double parameter_value;     // 参数值
};

// 物理参数配置 - 发送到第一部分
struct PhysicsConfig {
    double gravity;
    double friction;
    double elasticity;
    double time_step;
    bool pause_simulation;
};

class PhysicsRenderBridge {
private:
    // 坐标转换参数
    double pixels_per_meter_;
    sf::Vector2f world_offset_;
    
    // 对象映射
    std::vector<std::pair<int, PhysicsState>> physics_objects_;
    std::vector<std::pair<int, RenderState>> render_objects_;
    
    // 配置
    PhysicsConfig current_config_;
    
public:
    PhysicsRenderBridge();
    
    // 初始化接口
    void initialize(double ppm = 100.0, sf::Vector2f offset = sf::Vector2f(400, 300));
    
    // 坐标转换函数
    sf::Vector2f physicsToScreen(double physics_x, double physics_y);
    sf::Vector2f screenToPhysics(float screen_x, float screen_y);
    
    // 数据同步函数
    void syncPhysicsToRender(const std::vector<PhysicsState>& physics_update);
    std::vector<PhysicsState> syncInputToPhysics(const std::vector<UserInput>& user_inputs);
    
    // 获取渲染状态
    const std::vector<std::pair<int, RenderState>>& getRenderStates() const;
    
    // 配置管理
    void updatePhysicsConfig(const PhysicsConfig& config);
    PhysicsConfig getCurrentConfig() const;
    
    // 对象管理
    int createObject(const PhysicsState& physics_state, const RenderState& render_state);
    void removeObject(int object_id);
    
private:
    // 内部辅助函数
    void applyUserInputToPhysics(const UserInput& input, PhysicsState& physics_state);
    void handleMouseDrag(const UserInput& input);
    int generateObjectId();
};

#endif