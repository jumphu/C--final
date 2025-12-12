// PhysicsRenderBridge.cpp
#include "connector.h" //"PhysicsRenderBridge.h"
#include <cmath>

PhysicsRenderBridge::PhysicsRenderBridge() 
    : pixels_per_meter_(100.0), world_offset_(400, 300) {
}

void PhysicsRenderBridge::initialize(double ppm, sf::Vector2f offset) {
    pixels_per_meter_ = ppm;
    world_offset_ = offset;
    physics_objects_.clear();
    render_objects_.clear();
    
    // 默认配置
    current_config_.gravity = 9.8;
    current_config_.friction = 0.1;
    current_config_.elasticity = 0.8;
    current_config_.time_step = 0.016; // ~60 FPS
    current_config_.pause_simulation = false;
}

sf::Vector2f PhysicsRenderBridge::physicsToScreen(double physics_x, double physics_y) {
    float screen_x = static_cast<float>(physics_x * pixels_per_meter_ + world_offset_.x);
    float screen_y = static_cast<float>(-physics_y * pixels_per_meter_ + world_offset_.y); // Y轴翻转
    return sf::Vector2f(screen_x, screen_y);
}

sf::Vector2f PhysicsRenderBridge::screenToPhysics(float screen_x, float screen_y) {
    double physics_x = (screen_x - world_offset_.x) / pixels_per_meter_;
    double physics_y = -(screen_y - world_offset_.y) / pixels_per_meter_; // Y轴翻转
    return sf::Vector2f(static_cast<float>(physics_x), static_cast<float>(physics_y));
}

void PhysicsRenderBridge::syncPhysicsToRender(const std::vector<PhysicsState>& physics_update) {
    for (const auto& physics_state : physics_update) {
        // 查找对应的渲染对象
        for (auto& render_pair : render_objects_) {
            if (render_pair.first == physics_state.shape_type) { // 这里用shape_type作为ID，实际应该用唯一ID
                RenderState& render_state = render_pair.second;
                
                // 更新位置和旋转
                sf::Vector2f screen_pos = physicsToScreen(physics_state.pos_x, physics_state.pos_y);
                render_state.position = screen_pos;
                render_state.rotation = static_cast<float>(physics_state.angle * 180.0 / M_PI); // 弧度转角度
                
                // 更新尺寸
                if (physics_state.shape_type == 0) { // 圆形
                    render_state.size = sf::Vector2f(
                        static_cast<float>(physics_state.radius * 2 * pixels_per_meter_),
                        static_cast<float>(physics_state.radius * 2 * pixels_per_meter_)
                    );
                } else if (physics_state.shape_type == 1) { // 矩形
                    render_state.size = sf::Vector2f(
                        static_cast<float>(physics_state.width * pixels_per_meter_),
                        static_cast<float>(physics_state.height * pixels_per_meter_)
                    );
                }
                break;
            }
        }
    }
}

std::vector<PhysicsState> PhysicsRenderBridge::syncInputToPhysics(const std::vector<UserInput>& user_inputs) {
    std::vector<PhysicsState> physics_updates;
    
    for (const auto& input : user_inputs) {
        switch (input.type) {
            case UserInput::MOUSE_DRAG_START:
            case UserInput::MOUSE_DRAG_UPDATE:
                handleMouseDrag(input);
                break;
                
            case UserInput::BUTTON_CLICK:
                // 处理按钮点击，如重置、暂停等
                if (input.parameter_name == "reset") {
                    // 重置逻辑
                } else if (input.parameter_name == "pause") {
                    current_config_.pause_simulation = !current_config_.pause_simulation;
                }
                break;
                
            case UserInput::PARAMETER_CHANGE:
                // 更新物理参数
                if (input.parameter_name == "gravity") {
                    current_config_.gravity = input.parameter_value;
                } else if (input.parameter_name == "friction") {
                    current_config_.friction = input.parameter_value;
                } else if (input.parameter_name == "elasticity") {
                    current_config_.elasticity = input.parameter_value;
                }
                break;
        }
    }
    
    // 返回需要更新的物理状态
    for (const auto& physics_pair : physics_objects_) {
        physics_updates.push_back(physics_pair.second);
    }
    
    return physics_updates;
}

const std::vector<std::pair<int, RenderState>>& PhysicsRenderBridge::getRenderStates() const {
    return render_objects_;
}

void PhysicsRenderBridge::updatePhysicsConfig(const PhysicsConfig& config) {
    current_config_ = config;
}

PhysicsConfig PhysicsRenderBridge::getCurrentConfig() const {
    return current_config_;
}

int PhysicsRenderBridge::createObject(const PhysicsState& physics_state, const RenderState& render_state) {
    int new_id = generateObjectId();
    physics_objects_.emplace_back(new_id, physics_state);
    render_objects_.emplace_back(new_id, render_state);
    return new_id;
}

void PhysicsRenderBridge::removeObject(int object_id) {
    // 从物理对象列表中移除
    physics_objects_.erase(
        std::remove_if(physics_objects_.begin(), physics_objects_.end(),
                      [object_id](const auto& pair) { return pair.first == object_id; }),
        physics_objects_.end()
    );
    
    // 从渲染对象列表中移除
    render_objects_.erase(
        std::remove_if(render_objects_.begin(), render_objects_.end(),
                      [object_id](const auto& pair) { return pair.first == object_id; }),
        render_objects_.end()
    );
}

void PhysicsRenderBridge::handleMouseDrag(const UserInput& input) {
    // 处理鼠标拖动逻辑
    for (auto& physics_pair : physics_objects_) {
        if (physics_pair.first == input.object_id) {
            PhysicsState& physics_state = physics_pair.second;
            
            // 将屏幕坐标转换为物理坐标
            sf::Vector2f physics_pos = screenToPhysics(input.mouse_position.x, input.mouse_position.y);
            
            // 更新物理对象位置
            physics_state.pos_x = physics_pos.x;
            physics_state.pos_y = physics_pos.y;
            
            // 如果是拖动开始或更新，设置速度为0
            if (input.type == UserInput::MOUSE_DRAG_START || 
                input.type == UserInput::MOUSE_DRAG_UPDATE) {
                physics_state.velocity_x = 0;
                physics_state.velocity_y = 0;
            }
            break;
        }
    }
}

int PhysicsRenderBridge::generateObjectId() {
    static int next_id = 1;
    return next_id++;
}