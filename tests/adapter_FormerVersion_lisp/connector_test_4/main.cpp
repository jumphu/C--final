#include <SFML/Graphics.hpp>
#include "PhysicsRenderBridge.h"

int main() {
    // 创建物理世界
    PhysicalWorld physics_world;
    
    // 创建接口桥梁
    PhysicsRenderBridge bridge(physics_world);
    bridge.initialize(100.0f, sf::Vector2f(400, 300));
    
    // 创建窗口
    sf::RenderWindow window(sf::VideoMode(800, 600), "Physics Demo");
    
    while (window.isOpen()) {
        // 处理事件
        std::vector<UserInput> user_inputs;
        sf::Event event;
        
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            // 处理鼠标点击
            if (event.type == sf::Event::MouseButtonPressed) {
                // 检查音乐按钮点击
                sf::Vector2f mouse_pos(event.mouseButton.x, event.mouseButton.y);
                const MusicState& music = bridge.getMusicState();
                float distance = std::sqrt(
                    std::pow(mouse_pos.x - music.position.x, 2) + 
                    std::pow(mouse_pos.y - music.position.y, 2)
                );
                
                if (distance <= music.radius) {
                    UserInput music_event;
                    music_event.type = UserInput::MUSIC_TOGGLE;
                    music_event.mouse_position = mouse_pos;
                    user_inputs.push_back(music_event);
                }
            }
        }
        
        // 更新按钮状态（这里需要从第二部分获取实际状态）
        ButtonStates button_states;
        // button_states = 从第二部分获取按钮状态;
        bridge.updateButtonStates(button_states);
        
        // 更新音乐状态
        MusicState music_state;
        // music_state = 从第二部分获取音乐状态;
        bridge.updateMusicState(music_state);
        
        // 处理按钮事件
        bridge.handleButtonEvents();
        
        // 同步用户输入到物理世界
        bridge.syncInputToPhysics(user_inputs);
        
        // 如果未暂停，更新物理世界
        if (!bridge.isPaused()) {
            physics_world.update(physics_world.dynamicShapeList, physics_world.ground);
        }
        
        // 同步物理世界到渲染
        bridge.syncPhysicsToRender();
        
        // 渲染
        window.clear(sf::Color::White);
        
        // 渲染物理形状
        const auto& render_states = bridge.getRenderStates();
        for (const auto& pair : render_states) {
            const RenderState& state = pair.second;
            
            if (state.type == "circle" || state.type == "ball") {
                sf::CircleShape circle(state.size.x / 2);
                circle.setPosition(state.position - sf::Vector2f(state.size.x / 2, state.size.y / 2));
                circle.setFillColor(state.color);
                window.draw(circle);
            } else if (state.type == "rectangle" || state.type == "box") {
                sf::RectangleShape rect(state.size);
                rect.setPosition(state.position - state.size / 2.0f);
                rect.setFillColor(state.color);
                window.draw(rect);
            }
        }
        
        // 渲染按钮（示例）
        const auto& btn_states = bridge.getButtonStates();
        
        // 开始按钮
        sf::RectangleShape start_btn(sf::Vector2f(100, 40));
        start_btn.setPosition(50, 50);
        start_btn.setFillColor(btn_states.start_hovered ? sf::Color(200, 200, 200) : sf::Color(150, 150, 150));
        window.draw(start_btn);
        
        // 暂停按钮
        sf::RectangleShape pause_btn(sf::Vector2f(100, 40));
        pause_btn.setPosition(50, 100);
        pause_btn.setFillColor(btn_states.pause_hovered ? sf::Color(200, 200, 200) : sf::Color(150, 150, 150));
        window.draw(pause_btn);
        
        // 停止按钮
        sf::RectangleShape stop_btn(sf::Vector2f(100, 40));
        stop_btn.setPosition(50, 150);
        stop_btn.setFillColor(btn_states.stop_hovered ? sf::Color(200, 200, 200) : sf::Color(150, 150, 150));
        window.draw(stop_btn);
        
        // 渲染音乐按钮
        const auto& music_states = bridge.getMusicState();
        sf::CircleShape music_btn(music_states.radius);
        music_btn.setPosition(music_states.position - sf::Vector2f(music_states.radius, music_states.radius));
        music_btn.setFillColor(music_states.is_playing ? sf::Color::Green : sf::Color::Red);
        window.draw(music_btn);
        
        window.display();
    }
    
    return 0;
}