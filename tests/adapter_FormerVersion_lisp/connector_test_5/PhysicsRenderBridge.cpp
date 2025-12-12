#include "PhysicsRenderBridge.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>

PhysicsRenderBridge::PhysicsRenderBridge(PhysicalWorld& physics_world) 
    : physics_world_(physics_world), pixels_per_meter_(100.0), world_offset_(400, 300), currently_dragging_("") {
    
    button_states_.start_clicked = false;
    button_states_.pause_clicked = false;
    button_states_.stop_clicked = false;
    button_states_.start_hovered = false;
    button_states_.pause_hovered = false;
    button_states_.stop_hovered = false;
    
    music_state_.is_playing = false;
    music_state_.is_hovering = false;
    music_state_.position = sf::Vector2f(320, 80);
    music_state_.radius = 60.0f;
}

void PhysicsRenderBridge::initialize(double ppm, sf::Vector2f offset) {
    pixels_per_meter_ = ppm;
    world_offset_ = offset;
    render_objects_.clear();
    currently_dragging_ = "";
}

sf::Vector2f PhysicsRenderBridge::physicsToScreen(double physics_x, double physics_y) {
    float screen_x = static_cast<float>(physics_x * pixels_per_meter_ + world_offset_.x);
    float screen_y = static_cast<float>(-physics_y * pixels_per_meter_ + world_offset_.y);
    return sf::Vector2f(screen_x, screen_y);
}

sf::Vector2f PhysicsRenderBridge::screenToPhysics(float screen_x, float screen_y) {
    double physics_x = (screen_x - world_offset_.x) / pixels_per_meter_;
    double physics_y = -(screen_y - world_offset_.y) / pixels_per_meter_;
    return sf::Vector2f(static_cast<float>(physics_x), static_cast<float>(physics_y));
}

void PhysicsRenderBridge::syncPhysicsToRender() {
    // 同步动态形状
    for (Shape* shape : physics_world_.dynamicShapeList) {
        std::string name = shape->getName();
        if (render_objects_.find(name) == render_objects_.end()) {
            createRenderShape(name, shape->getType());
        }
        updateRenderStateFromShape(shape, render_objects_[name]);
    }
    
    // 同步静态形状
    for (Shape* shape : physics_world_.staticShapeList) {
        std::string name = shape->getName();
        if (render_objects_.find(name) == render_objects_.end()) {
            createRenderShape(name, shape->getType(), sf::Color(150, 150, 150));
        }
        updateRenderStateFromShape(shape, render_objects_[name]);
    }
    
    // 清理不存在的渲染对象
    std::vector<std::string> to_remove;
    for (const auto& pair : render_objects_) {
        bool found = false;
        for (Shape* shape : physics_world_.dynamicShapeList) {
            if (shape->getName() == pair.first) {
                found = true;
                break;
            }
        }
        if (!found) {
            for (Shape* shape : physics_world_.staticShapeList) {
                if (shape->getName() == pair.first) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            to_remove.push_back(pair.first);
        }
    }
    
    for (const std::string& name : to_remove) {
        render_objects_.erase(name);
    }
}

void PhysicsRenderBridge::syncInputToPhysics(const std::vector<UserInput>& user_inputs) {
    for (const auto& input : user_inputs) {
        applyUserInputToPhysics(input);
    }
}

void PhysicsRenderBridge::updateButtonStates(const ButtonStates& button_states) {
    button_states_ = button_states;
}

void PhysicsRenderBridge::updateMusicState(const MusicState& music_state) {
    music_state_ = music_state;
}

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

void PhysicsRenderBridge::applyUserInputToPhysics(const UserInput& input) {
    switch (input.type) {
        case UserInput::MOUSE_DRAG_START:
        case UserInput::MOUSE_DRAG_UPDATE:
        case UserInput::MOUSE_DRAG_END:
            handleMouseDrag(input);
            break;
            
        case UserInput::BUTTON_CLICK:
            if (input.parameter_name == "start") {
                physics_world_.start();
            } else if (input.parameter_name == "pause") {
                physics_world_.Pause();
            } else if (input.parameter_name == "continue") {
                physics_world_.Continue();
            } else if (input.parameter_name == "stop") {
                physics_world_.Stop();
            } else if (input.parameter_name == "reset") {
                physics_world_.clearAllShapes();
                render_objects_.clear();
            }
            break;
            
        case UserInput::PARAMETER_CHANGE:
            handleParameterChange(input);
            break;
            
        case UserInput::CREATE_SHAPE:
            handleShapeCreation(input);
            break;
            
        case UserInput::DELETE_SHAPE:
            if (!input.shape_name.empty()) {
                Shape* shape = physics_world_.findShapeByName(input.shape_name);
                if (shape) {
                    physics_world_.removeDynamicShape(shape);
                    physics_world_.removeStaticShape(shape);
                }
            }
            break;
            
        case UserInput::MUSIC_TOGGLE:
            music_state_.is_playing = !music_state_.is_playing;
            break;
    }
}

void PhysicsRenderBridge::handleButtonClick(const std::string& button_name) {
    if (button_name == "start") {
        physics_world_.start();
    }
    else if (button_name == "pause") {
        if (physics_world_.getIsPaused()) {
            physics_world_.Continue();
        } else {
            physics_world_.Pause();
        }
    }
    else if (button_name == "stop") {
        physics_world_.Stop();
    }
}

void PhysicsRenderBridge::handleMouseDrag(const UserInput& input) {
    sf::Vector2f physics_pos = screenToPhysics(input.mouse_position.x, input.mouse_position.y);
    
    if (input.type == UserInput::MOUSE_DRAG_START) {
        currently_dragging_ = findShapeAtPosition(input.mouse_position);
        if (!currently_dragging_.empty()) {
            Shape* shape = physics_world_.findShapeByName(currently_dragging_);
            if (shape) {
                drag_offset_ = sf::Vector2f(
                    static_cast<float>(physics_pos.x - shape->getX()),
                    static_cast<float>(physics_pos.y - shape->getY())
                );
            }
        }
    }
    else if (input.type == UserInput::MOUSE_DRAG_UPDATE && !currently_dragging_.empty()) {
        Shape* shape = physics_world_.findShapeByName(currently_dragging_);
        if (shape) {
            shape->setPosition(
                physics_pos.x - drag_offset_.x,
                physics_pos.y - drag_offset_.y
            );
            shape->setVelocity(0, 0);
        }
    }
    else if (input.type == UserInput::MOUSE_DRAG_END) {
        currently_dragging_ = "";
    }
}

void PhysicsRenderBridge::handleParameterChange(const UserInput& input) {
    if (input.parameter_name == "gravity") {
        physics_world_.setGravity(input.parameter_value);
    } else if (input.parameter_name == "incline") {
        physics_world_.setInclineAngle(input.parameter_value);
    } else if (input.parameter_name == "friction") {
        physics_world_.StaticFriction = input.parameter_value;
    } else if (input.parameter_name == "timestep") {
        physics_world_.setTimeStep(input.parameter_value);
    }
}

void PhysicsRenderBridge::handleShapeCreation(const UserInput& input) {
    sf::Vector2f physics_pos = screenToPhysics(input.mouse_position.x, input.mouse_position.y);
    
    Shape* new_shape = physics_world_.placeDynamicShapeByType(
        input.shape_type,
        "user_created_" + std::to_string(rand() % 1000),
        physics_pos.x, physics_pos.y,
        1.0, 0.5
    );
    
    if (new_shape) {
        createRenderShape(new_shape->getName(), input.shape_type);
    }
}

const std::unordered_map<std::string, RenderState>& PhysicsRenderBridge::getRenderStates() const {
    return render_objects_;
}

const ButtonStates& PhysicsRenderBridge::getButtonStates() const {
    return button_states_;
}

const MusicState& PhysicsRenderBridge::getMusicState() const {
    return music_state_;
}

void PhysicsRenderBridge::createRenderShape(const std::string& name, const std::string& type, sf::Color color) {
    RenderState render_state;
    render_state.name = name;
    render_state.type = type;
    render_state.color = (color == sf::Color::White) ? getColorForShapeType(type) : color;
    render_state.is_selected = false;
    
    if (type == "circle" || type == "ball") {
        render_state.size = sf::Vector2f(30, 30);
    } else if (type == "rectangle" || type == "box") {
        render_state.size = sf::Vector2f(50, 30);
    } else {
        render_state.size = sf::Vector2f(40, 40);
    }
    
    render_objects_[name] = render_state;
}

void PhysicsRenderBridge::removeRenderShape(const std::string& name) {
    render_objects_.erase(name);
}

double PhysicsRenderBridge::getGravity() const {
    return physics_world_.gravity;
}

double PhysicsRenderBridge::getInclineAngle() const {
    return physics_world_.inclineAngle;
}

double PhysicsRenderBridge::getFriction() const {
    return physics_world_.StaticFriction;
}

double PhysicsRenderBridge::getTimeStep() const {
    return physics_world_.timeStep;
}

bool PhysicsRenderBridge::isPaused() const {
    return physics_world_.getIsPaused();
}

std::string PhysicsRenderBridge::findShapeAtPosition(sf::Vector2f screen_pos, float tolerance) {
    sf::Vector2f physics_pos = screenToPhysics(screen_pos.x, screen_pos.y);
    
    for (Shape* shape : physics_world_.dynamicShapeList) {
        double dx = shape->getX() - physics_pos.x;
        double dy = shape->getY() - physics_pos.y;
        double distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance * pixels_per_meter_ < tolerance) {
            return shape->getName();
        }
    }
    
    return "";
}

sf::Color PhysicsRenderBridge::getColorForShapeType(const std::string& type) {
    if (type == "circle" || type == "ball") return sf::Color::Red;
    if (type == "rectangle" || type == "box") return sf::Color::Blue;
    if (type == "triangle") return sf::Color::Green;
    if (type == "pendulum") return sf::Color::Yellow;
    return sf::Color::Magenta;
}

void PhysicsRenderBridge::updateRenderStateFromShape(Shape* shape, RenderState& render_state) {
    sf::Vector2f screen_pos = physicsToScreen(shape->getX(), shape->getY());
    render_state.position = screen_pos;
}