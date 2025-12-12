// main.cpp 示例用法
#include "PhysicsRenderBridge.h"
#include <SFML/Graphics.hpp>

int main() {
    // 创建接口桥梁
    PhysicsRenderBridge bridge;
    bridge.initialize(100.0f, sf::Vector2f(400, 300));
    
    // 创建物理对象示例
    PhysicsState ball_physics;
    ball_physics.pos_x = 0.0;
    ball_physics.pos_y = 2.0;
    ball_physics.shape_type = 0; // 圆形
    ball_physics.radius = 0.5;
    
    RenderState ball_render;
    ball_render.color = sf::Color::Red;
    
    int ball_id = bridge.createObject(ball_physics, ball_render);
    
    // 主循环示例
    sf::RenderWindow window(sf::VideoMode(800, 600), "Physics Demo");
    
    while (window.isOpen()) {
        sf::Event event;
        std::vector<UserInput> user_inputs;
        
        // 处理事件
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // 这里添加更多事件处理...
        }
        
        // 从物理引擎获取更新
        std::vector<PhysicsState> physics_updates; // 这里应该从第一部分获取
        
        // 同步物理到渲染
        bridge.syncPhysicsToRender(physics_updates);
        
        // 获取渲染状态并绘制
        const auto& render_states = bridge.getRenderStates();
        
        window.clear();
        // 这里根据render_states绘制对象...
        window.display();
    }
    
    return 0;
}