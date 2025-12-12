#include <SFML/Graphics.hpp>
#include <iostream>
#include "PhysicsRenderBridge.h"

// 简单的按钮类（模拟第二部分按钮功能）
class SFMLButton {
private:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Font font;
    bool isHovered;
    bool isClicked;
    std::string label;
    
public:
    SFMLButton(float x, float y, float width, float height, std::string btnLabel) 
        : label(btnLabel), isHovered(false), isClicked(false) {
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2f(width, height));
        shape.setFillColor(sf::Color(150, 150, 150));
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::Black);
        
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Failed to load font" << std::endl;
        }
        
        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        
        // 居中文本
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(
            x + (width - textBounds.width) / 2,
            y + (height - textBounds.height) / 2 - 5
        );
    }
    
    void update(const sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect buttonBounds = shape.getGlobalBounds();
        
        isHovered = buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        
        if (isHovered && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            isClicked = true;
        } else {
            isClicked = false;
        }
        
        // 更新颜色
        if (isClicked) {
            shape.setFillColor(sf::Color(100, 100, 100));
        } else if (isHovered) {
            shape.setFillColor(sf::Color(200, 200, 200));
        } else {
            shape.setFillColor(sf::Color(150, 150, 150));
        }
    }
    
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }
    
    bool isButtonClicked() const { return isClicked; }
    bool isButtonHovered() const { return isHovered; }
    std::string getLabel() const { return label; }
};

int main() {
    // 创建物理世界
    PhysicalWorld physics_world;
    
    // 创建接口桥梁
    PhysicsRenderBridge bridge(physics_world);
    bridge.initialize(100.0f, sf::Vector2f(400, 300));
    
    // 创建窗口
    sf::RenderWindow window(sf::VideoMode(800, 600), "Physics Demo - SFML");
    
    // 创建按钮
    SFMLButton startButton(50, 50, 100, 40, "Start");
    SFMLButton pauseButton(50, 100, 100, 40, "Pause");
    SFMLButton stopButton(50, 150, 100, 40, "Stop");
    
    // 创建音乐按钮
    sf::CircleShape musicButton(30);
    musicButton.setPosition(50, 210);
    musicButton.setFillColor(sf::Color::Red);
    
    // 音乐按钮文本
    sf::Text musicText;
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
    }
    musicText.setFont(font);
    musicText.setString("Music");
    musicText.setCharacterSize(12);
    musicText.setFillColor(sf::Color::White);
    musicText.setPosition(55, 220);
    
    // 主循环
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        
        // 处理事件
        std::vector<UserInput> user_inputs;
        sf::Event event;
        
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            // 处理鼠标点击
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), 
                                     static_cast<float>(event.mouseButton.y));
                
                // 检查音乐按钮点击
                if (musicButton.getGlobalBounds().contains(mousePos)) {
                    UserInput musicEvent;
                    musicEvent.type = UserInput::MUSIC_TOGGLE;
                    musicEvent.mouse_position = mousePos;
                    user_inputs.push_back(musicEvent);
                }
                
                // 检查形状创建
                if (event.mouseButton.button == sf::Mouse::Right) {
                    UserInput createEvent;
                    createEvent.type = UserInput::CREATE_SHAPE;
                    createEvent.mouse_position = mousePos;
                    createEvent.shape_type = "circle";
                    user_inputs.push_back(createEvent);
                }
            }
            
            // 处理鼠标拖动
            if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), 
                                     static_cast<float>(event.mouseMove.y));
                
                UserInput dragEvent;
                dragEvent.type = UserInput::MOUSE_DRAG_UPDATE;
                dragEvent.mouse_position = mousePos;
                user_inputs.push_back(dragEvent);
            }
            
            if (event.type == sf::Event::MouseButtonReleased) {
                sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), 
                                     static_cast<float>(event.mouseButton.y));
                
                UserInput dragEndEvent;
                dragEndEvent.type = UserInput::MOUSE_DRAG_END;
                dragEndEvent.mouse_position = mousePos;
                user_inputs.push_back(dragEndEvent);
            }
        }
        
        // 更新按钮状态
        startButton.update(window);
        pauseButton.update(window);
        stopButton.update(window);
        
        // 更新音乐按钮悬停状态
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        bool musicHovered = musicButton.getGlobalBounds().contains(
            static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        
        // 创建按钮状态结构
        ButtonStates button_states;
        button_states.start_clicked = startButton.isButtonClicked();
        button_states.pause_clicked = pauseButton.isButtonClicked();
        button_states.stop_clicked = stopButton.isButtonClicked();
        button_states.start_hovered = startButton.isButtonHovered();
        button_states.pause_hovered = pauseButton.isButtonHovered();
        button_states.stop_hovered = stopButton.isButtonHovered();
        
        // 创建音乐状态结构
        MusicState music_state;
        music_state.is_playing = bridge.getMusicState().is_playing;
        music_state.is_hovering = musicHovered;
        music_state.position = musicButton.getPosition() + sf::Vector2f(musicButton.getRadius(), musicButton.getRadius());
        music_state.radius = musicButton.getRadius();
        
        // 更新接口状态
        bridge.updateButtonStates(button_states);
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
        
        // 渲染UI
        startButton.draw(window);
        pauseButton.draw(window);
        stopButton.draw(window);
        
        // 渲染音乐按钮
        musicButton.setFillColor(bridge.getMusicState().is_playing ? sf::Color::Green : sf::Color::Red);
        window.draw(musicButton);
        window.draw(musicText);
        
        // 显示物理参数
        sf::Text paramText;
        paramText.setFont(font);
        paramText.setCharacterSize(14);
        paramText.setFillColor(sf::Color::Black);
        paramText.setPosition(50, 280);
        
        std::string paramStr = "Gravity: " + std::to_string(bridge.getGravity()) + "\n" +
                              "Friction: " + std::to_string(bridge.getFriction()) + "\n" +
                              "Incline: " + std::to_string(bridge.getInclineAngle()) + "\n" +
                              "Paused: " + std::string(bridge.isPaused() ? "Yes" : "No");
        paramText.setString(paramStr);
        window.draw(paramText);
        
        window.display();
    }
    
    return 0;
}