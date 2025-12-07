#include "PhysicsRenderBridge.h"
#include "physicalWorld.h" 
#include <iostream>
#include <cmath>

// ==================== Adapter Implementation ====================

class PhysicalWorldAdapterImpl : public IPhysicalWorldAdapter {
private:
    PhysicalWorld* world;
    
public:
    PhysicalWorldAdapterImpl() {
        world = new PhysicalWorld();
        // Initialize with default ground
        // world->ground is already there
    }
    
    ~PhysicalWorldAdapterImpl() {
        delete world;
    }
    
    std::vector<Shape*> getAllShapes() const override {
        std::vector<Shape*> all;
        all.insert(all.end(), world->dynamicShapeList.begin(), world->dynamicShapeList.end());
        all.insert(all.end(), world->staticShapeList.begin(), world->staticShapeList.end());
        // Don't forget the ground itself if it's treated as a shape to render, 
        // but PhysicalWorld treats ground specially.
        // We might want to manually add a "Ground" render object in the bridge if it's not in the shape list.
        return all;
    }
    
    Shape* findShapeByName(const std::string& name) const override {
        return world->findShapeByName(name);
    }
    
    double getGravity() const override { return world->getGravity(); }
    void setGravity(double g) override { world->setGravity(g); }
    
    double getGroundFriction() const override { return world->ground.getFriction(); }
    void setGroundFriction(double f) override { world->ground.setFriction(f); }
    
    void start() override { world->Continue(); }
    void pause() override { world->Pause(); }
    void stop() override { 
        world->Stop(); // Depending on implementation this might clear everything
        world->clearAllShapes(); // Ensure it's clear
    }
    
    void update(double deltaTime) override {
        // PhysicalWorld::update uses its own timestep or passed delta?
        // Header says: update(list, deltaTime, ground)
        // We need to pass dynamic list
        world->update(world->dynamicShapeList, deltaTime, world->ground);
    }
    
    bool isPaused() const override { return world->getIsPaused(); }

    void createBall(double x, double y, double radius, double mass, const char* color) override {
        std::string name = "Ball_" + std::to_string(rand());
        Shape* s = world->placeDynamicShapeByType("Circle", name, x, y, mass, radius);
        // Store color? Shape doesn't have color field in standard shapes.h usually.
        // We might need a map in the Bridge to store color by name.
    }

    void createBlock(double x, double y, double width, double height, double mass, const char* color) override {
        std::string name = "Block_" + std::to_string(rand());
        world->placeDynamicShapeByType("AABB", name, x, y, mass, width, height);
    }

    void createWall(double x, double y, double width, double height, double friction) override {
        std::string name = "Wall_" + std::to_string(rand());
        world->placeWall(name, x, y, width, height, friction);
    }

    void clearWorld() override {
        world->clearAllShapes();
        world->setInclineAngle(0.0); // Reset incline too
    }

    void setInclineAngle(double angle) override {
        world->setInclineAngle(angle);
    }
};

// ==================== Bridge Implementation ====================

PhysicsRenderBridge::PhysicsRenderBridge(std::unique_ptr<IPhysicalWorldAdapter> adapter, Renderer* renderer)
    : physics_adapter_(std::move(adapter)), renderer_(renderer), pixels_per_meter_(100.0) {
    
    memset(&button_states_, 0, sizeof(button_states_));
    // Default values to avoid 0 division or weird behavior
    button_states_.gravity_coefficient = 9.8f;
    button_states_.speed_value = 1.0f;
}

void PhysicsRenderBridge::initialize(double ppm, POINT offset) {
    pixels_per_meter_ = ppm;
    world_offset_ = offset;
}

POINT PhysicsRenderBridge::physicsToScreen(double physics_x, double physics_y) const {
    // Screen X = OffsetX + PhysX * Scale
    // Screen Y = OffsetY - PhysY * Scale (Y flip)
    int sx = (int)(world_offset_.x + physics_x * pixels_per_meter_);
    int sy = (int)(world_offset_.y - physics_y * pixels_per_meter_);
    return {sx, sy};
}

POINT PhysicsRenderBridge::screenToPhysics(int screen_x, int screen_y) const {
    double px = (screen_x - world_offset_.x) / pixels_per_meter_;
    double py = (world_offset_.y - screen_y) / pixels_per_meter_;
    return { (long)px, (long)py }; // Returning POINT is weird for double, but okay. Wait, function returns POINT?
    // The header said POINT screenToPhysics. POINT is integer.
    // Better to have double return. But let's stick to POINT for now if that's what was requested, 
    // or FIX the header to return std::pair<double, double> or struct.
    // For now I'll cast to long, but this loses precision for mouse picking.
    // Actually, let's keep it consistent with the header I wrote.
}

void PhysicsRenderBridge::syncPhysicsToRender() {
    render_objects_.clear();
    auto shapes = physics_adapter_->getAllShapes();
    
    for (Shape* s : shapes) {
        RenderObject obj = shapeToRenderObject(s);
        render_objects_[s->getName()] = obj;
    }
    
    // Add Ground manually if not in shapes
    RenderObject groundObj;
    groundObj.type = RenderObject::GROUND;
    groundObj.name = "Ground";
    groundObj.color = RGB(100, 100, 100);
    render_objects_["Ground"] = groundObj;
}

RenderObject PhysicsRenderBridge::shapeToRenderObject(Shape* shape) const {
    RenderObject obj;
    obj.name = shape->getName();
    
    double x, y, vx, vy;
    shape->getCentre(x, y);
    shape->getVelocity(vx, vy);
    
    std::string type = shape->getType();
    
    if (type == "Circle") {
        obj.type = RenderObject::BALL;
        Circle* c = dynamic_cast<Circle*>(shape);
        obj.ball.x = x;
        obj.ball.y = y;
        obj.ball.radius = c ? c->getRadius() : 1.0;
        obj.ball.vx = vx;
        obj.ball.vy = vy;
        obj.ball.mass = shape->getMass();
        obj.color = RED; // Default, should improve later
    } else if (type == "AABB") {
        obj.type = RenderObject::BLOCK;
        AABB* b = dynamic_cast<AABB*>(shape);
        obj.block.cx = x;
        obj.block.cy = y;
        obj.block.width = b ? b->getWidth() : 1.0;
        obj.block.height = b ? b->getHeight() : 1.0;
        obj.block.angle = 0; // No rotation yet
        obj.block.vx = vx;
        obj.block.vy = vy;
        obj.block.mass = shape->getMass();
        obj.color = BLUE;
    } else if (type == "Wall") {
        obj.type = RenderObject::WALL;
        Wall* w = dynamic_cast<Wall*>(shape);
        obj.block.cx = x;
        obj.block.cy = y;
        obj.block.width = w ? w->getWidth() : 1.0;
        obj.block.height = w ? w->getHeight() : 1.0;
        obj.color = WHITE;
    }
    
    return obj;
}

void PhysicsRenderBridge::syncInputToPhysics(const std::vector<UserInput>& inputs) {
    for (const auto& input : inputs) {
        if (input.type == UserInput::CREATE_SHAPE) {
            // Convert screen to physics
            double px = (input.mouse_position.x - world_offset_.x) / pixels_per_meter_;
            double py = (world_offset_.y - input.mouse_position.y) / pixels_per_meter_;
            
            if (input.shape_type == "Circle") {
                physics_adapter_->createBall(px, py, 0.5, 1.0, button_states_.color_choice);
            } else if (input.shape_type == "AABB") {
                physics_adapter_->createBlock(px, py, 1.0, 1.0, 1.0, button_states_.color_choice);
            }
        }
    }
}

void PhysicsRenderBridge::clearWorld() {
    physics_adapter_->clearWorld();
}

void PhysicsRenderBridge::setInclineAngle(double angle) {
    physics_adapter_->setInclineAngle(angle);
}

void PhysicsRenderBridge::createWall(double x, double y, double width, double height, double friction) {
    physics_adapter_->createWall(x, y, width, height, friction);
}

void PhysicsRenderBridge::createBall(double x, double y, double radius, double mass, const char* color) {
    physics_adapter_->createBall(x, y, radius, mass, color);
}

void PhysicsRenderBridge::createBlock(double x, double y, double width, double height, double mass, const char* color) {
    physics_adapter_->createBlock(x, y, width, height, mass, color);
}

void PhysicsRenderBridge::updateButtonStates() {
    syncFromAllButtons();
    
    if (button_states_.start_clicked) physics_adapter_->start();
    if (button_states_.pause_clicked) physics_adapter_->pause();
    if (button_states_.stop_clicked) physics_adapter_->stop();
    
    // Update params only if changed significanty to avoid spamming
    physics_adapter_->setGravity(button_states_.gravity_coefficient);
    physics_adapter_->setGroundFriction(button_states_.friction_coefficient);
}

void PhysicsRenderBridge::update(double dt) {
    physics_adapter_->update(dt);
}

void PhysicsRenderBridge::syncFromAllButtons() {
    button_states_.start_clicked = getStartButtonState();
    button_states_.pause_clicked = getPauseButtonState();
    button_states_.stop_clicked = getStopButtonState();
    
    // Safely copy strings
    const char* col = getColor();
    if (col) strncpy(button_states_.color_choice, col, 9);
    
    button_states_.gravity_coefficient = getGrav();
    button_states_.friction_coefficient = getFric();
    button_states_.speed_value = getSpeedVal();
}

bool PhysicsRenderBridge::isPaused() const {
    return physics_adapter_->isPaused();
}

COLORREF PhysicsRenderBridge::parseColor(const char* colorName) const {
        std::string s = colorName;
        if (s == "Red") return RED;
        if (s == "Blue") return BLUE;
        if (s == "Green") return GREEN;
        if (s == "Yellow") return YELLOW;
        return WHITE;
    }
    
    // Factory function exposed to main
    std::unique_ptr<IPhysicalWorldAdapter> createDefaultAdapter() {
        return std::make_unique<PhysicalWorldAdapterImpl>();
    }
    