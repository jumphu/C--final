/**
 * ============================================================
 * 文件: PhysicsRenderBridge.cpp (适配版)
 * 关键实现: 处理队友接口的适配和转换
 * ============================================================
 */

#include "PhysicsRenderBridge.h"
#include <typeinfo>
#include <cmath>

// ==================== 物理世界适配器实现 ====================

/**
 * DefaultPhysicalWorldAdapter - 默认物理世界适配器
 * 假设第一部分同学使用shapes.h中的类，但没有统一的PhysicalWorld类
 */
class DefaultPhysicalWorldAdapter : public IPhysicalWorldAdapter {
private:
    std::vector<std::unique_ptr<Shape>> shapes_;
    std::vector<Shape*> ground_shapes_;
    double gravity_ = 9.8;
    double ground_friction_ = 0.1;
    bool is_paused_ = false;
    
public:
    std::vector<Shape*> getAllShapes() const override {
        std::vector<Shape*> result;
        for (const auto& shape : shapes_) {
            result.push_back(shape.get());
        }
        for (const auto& shape : ground_shapes_) {
            result.push_back(shape);
        }
        return result;
    }
    
    Shape* findShapeByName(const std::string& name) const override {
        for (const auto& shape : shapes_) {
            if (shape->getName() == name) return shape.get();
        }
        for (const auto& shape : ground_shapes_) {
            if (shape->getName() == name) return shape;
        }
        return nullptr;
    }
    
    // 其他接口实现...
    
    Shape* createBall(const std::string& name, double x, double y, 
                      double radius, double mass, COLORREF color) override {
        auto ball = std::make_unique<Circle>(mass, radius, x, y);
        ball->setName(name);
        shapes_.push_back(std::move(ball));
        return shapes_.back().get();
    }
    
    Shape* createBlock(const std::string& name, double x, double y,
                       double width, double height, double mass, COLORREF color) override {
        auto block = std::make_unique<AABB>(mass, width, height, x, y);
        block->setName(name);
        shapes_.push_back(std::move(block));
        return shapes_.back().get();
    }
    
    // 物理更新
    void update(double deltaTime) override {
        if (is_paused_) return;
        
        for (auto& shape : shapes_) {
            // 应用重力
            shape->applyGravity(gravity_);
            
            // 应用摩擦力（简化处理）
            if (shape->getIsSupported()) {
                // 实际应该根据支撑面的摩擦系数计算
                shape->applyFriction(shape->getMass() * gravity_, ground_friction_, 
                                     ground_friction_ * 1.2, 0);
            }
            
            // 更新位置
            shape->update(deltaTime);
        }
    }
};

// ==================== 构造函数 ====================

PhysicsRenderBridge::PhysicsRenderBridge(
    std::unique_ptr<IPhysicalWorldAdapter> adapter, 
    Renderer* renderer) 
    : physics_adapter_(std::move(adapter))
    , renderer_(renderer)
    , pixels_per_meter_(100.0)
    , screen_height_(600) {
    
    // 初始化按钮状态
    memset(&button_states_, 0, sizeof(button_states_));
    strcpy(button_states_.color_choice, "red");
    strcpy(button_states_.color_choice2, "blue");
    button_states_.gravity_coefficient = 9.8f;
    button_states_.friction_coefficient = 0.1f;
    button_states_.speed_value = 1.0f;
    
    dragging_shape_.clear();
}

// ==================== 坐标转换 ====================

POINT PhysicsRenderBridge::physicsToScreen(double physics_x, double physics_y) const {
    int screen_x = static_cast<int>(physics_x * pixels_per_meter_ + world_offset_.x);
    int screen_y = static_cast<int>(world_offset_.y - physics_y * pixels_per_meter_);
    return {screen_x, screen_y};
}

POINT PhysicsRenderBridge::screenToPhysics(int screen_x, int screen_y) const {
    double physics_x = (screen_x - world_offset_.x) / pixels_per_meter_;
    double physics_y = (world_offset_.y - screen_y) / pixels_per_meter_;
    return {static_cast<int>(physics_x), static_cast<int>(physics_y)};
}

// ==================== 核心同步方法 ====================

void PhysicsRenderBridge::syncPhysicsToRender() {
    // 清空当前渲染对象
    render_objects_.clear();
    
    // 获取所有物理形状
    auto shapes = physics_adapter_->getAllShapes();
    
    for (Shape* shape : shapes) {
        // 转换为渲染对象
        RenderObject render_obj = shapeToRenderObject(shape);
        
        // 存储到映射表
        render_objects_[shape->getName()] = render_obj;
    }
}

RenderObject PhysicsRenderBridge::shapeToRenderObject(Shape* shape) const {
    RenderObject obj;
    obj.name = shape->getName();
    
    // 获取位置和速度
    double x, y, vx, vy;
    shape->getCentre(x, y);
    shape->getVelocity(vx, vy);
    
    // 根据形状类型设置数据
    std::string type = shape->getType();
    
    if (type == "Circle") {
        obj.type = RenderObject::BALL;
        Circle* circle = asCircle(shape);
        if (circle) {
            obj.ball.x = x;
            obj.ball.y = y;
            obj.ball.radius = circle->getRadius();
            obj.ball.vx = vx;
            obj.ball.vy = vy;
            obj.ball.mass = shape->getMass();
        }
        obj.color = RED;  // 默认颜色
    }
    else if (type == "AABB") {
        obj.type = RenderObject::BLOCK;
        AABB* aabb = asAABB(shape);
        if (aabb) {
            obj.block.cx = x;
            obj.block.cy = y;
            obj.block.width = aabb->getWidth();
            obj.block.height = aabb->getHeight();
            obj.block.angle = 0;  // AABB不旋转
            obj.block.vx = vx;
            obj.block.vy = vy;
            obj.block.mass = shape->getMass();
        }
        obj.color = BLUE;
    }
    else if (type == "Slope") {
        obj.type = RenderObject::RAMP;
        Slope* slope = asSlope(shape);
        if (slope) {
            // 计算斜坡起点和终点
            double length = slope->getLength();
            double angle = slope->getAngle();
            
            obj.ramp.x1 = x - length * cos(angle) / 2;
            obj.ramp.y1 = y - length * sin(angle) / 2;
            obj.ramp.x2 = x + length * cos(angle) / 2;
            obj.ramp.y2 = y + length * sin(angle) / 2;
            obj.ramp.mu = 0.1;  // 默认摩擦系数
        }
        obj.color = GREEN;
    }
    else if (type == "Ground") {
        obj.type = RenderObject::GROUND;
        obj.color = RGB(100, 100, 100);  // 灰色
    }
    else if (type == "Wall") {
        obj.type = RenderObject::WALL;
        obj.color = RGB(150, 150, 150);
    }
    
    return obj;
}

// ==================== 类型检测和向下转型 ====================

Circle* PhysicsRenderBridge::asCircle(Shape* shape) const {
    return (shape && shape->getType() == "Circle") ? 
           static_cast<Circle*>(shape) : nullptr;
}

AABB* PhysicsRenderBridge::asAABB(Shape* shape) const {
    return (shape && shape->getType() == "AABB") ? 
           static_cast<AABB*>(shape) : nullptr;
}

Slope* PhysicsRenderBridge::asSlope(Shape* shape) const {
    return (shape && shape->getType() == "Slope") ? 
           static_cast<Slope*>(shape) : nullptr;
}

Ground* PhysicsRenderBridge::asGround(Shape* shape) const {
    return (shape && shape->getType() == "Ground") ? 
           static_cast<Ground*>(shape) : nullptr;
}

Wall* PhysicsRenderBridge::asWall(Shape* shape) const {
    return (shape && shape->getType() == "Wall") ? 
           static_cast<Wall*>(shape) : nullptr;
}

// ==================== 按钮状态同步 ====================

void PhysicsRenderBridge::updateButtonStates() {
    // 同步不同按钮系统的状态
    syncFromAllButtons();  // 使用ALL_BUTTONS_H作为统一接口
    
    // 根据按钮状态执行相应操作
    if (button_states_.start_clicked) {
        physics_adapter_->start();
    }
    if (button_states_.pause_clicked) {
        physics_adapter_->pause();
    }
    if (button_states_.stop_clicked) {
        physics_adapter_->stop();
    }
    
    // 更新物理参数
    physics_adapter_->setGravity(button_states_.gravity_coefficient);
    physics_adapter_->setGroundFriction(button_states_.friction_coefficient);
}

void PhysicsRenderBridge::syncFromAllButtons() {
    // 使用ALL_BUTTONS_H中的函数获取状态
    button_states_.start_clicked = getStartButtonState();
    button_states_.pause_clicked = getPauseButtonState();
    button_states_.stop_clicked = getStopButtonState();
    
    // 获取附加按钮参数
    strcpy(button_states_.color_choice, getColor());
    button_states_.gravity_coefficient = getGrav();
    button_states_.friction_coefficient = getFric();
    button_states_.speed_value = getSpeedVal();
}

// ==================== 输入处理 ====================

void PhysicsRenderBridge::syncInputToPhysics(const std::vector<UserInput>& inputs) {
    for (const auto& input : inputs) {
        switch (input.type) {
            case UserInput::MOUSE_DRAG_START:
            case UserInput::MOUSE_DRAG_UPDATE:
            case UserInput::MOUSE_DRAG_END:
                handleMouseDrag(input);
                break;
                
            case UserInput::CREATE_SHAPE:
                createShapeAtPosition(input.shape_type, input.mouse_position);
                break;
                
            case UserInput::BUTTON_CLICK:
                handleButtonClick(input);
                break;
                
            case UserInput::PARAMETER_CHANGE:
                handleParameterChange(input);
                break;
        }
    }
}

// ==================== 形状查找 ====================

std::string PhysicsRenderBridge::findShapeAtPosition(POINT screen_pos, int tolerance) {
    POINT physics_pos = screenToPhysics(screen_pos.x, screen_pos.y);
    
    // 查找最近的形状
    double min_distance = tolerance * tolerance / (pixels_per_meter_ * pixels_per_meter_);
    std::string closest_shape;
    
    auto shapes = physics_adapter_->getAllShapes();
    for (Shape* shape : shapes) {
        double x, y;
        shape->getCentre(x, y);
        
        double dx = x - physics_pos.x;
        double dy = y - physics_pos.y;
        double distance = dx * dx + dy * dy;
        
        if (distance < min_distance) {
            min_distance = distance;
            closest_shape = shape->getName();
        }
    }
    
    return closest_shape;
}
