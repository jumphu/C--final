#ifndef PHYSICS_RENDER_BRIDGE_H
#define PHYSICS_RENDER_BRIDGE_H

#include <graphics.h>
#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>

// 队友的头文件
#include "shapes.h"        
#include "Renderer.h"      
#include "allbottums.h"   // Correct filename
#include "music.h"   

// ==================== 新的数据结构（适配Renderer）====================

struct RenderObject {
    enum Type { BALL, RAMP, BLOCK, GROUND, WALL } type;
    
    union {
        struct {
            double x, y;     
            double radius;   
            double vx, vy;   
            double mass;     
        } ball;
        
        struct {
            double x1, y1;   
            double x2, y2;   
            double mu;       
        } ramp;
        
        struct {
            double cx, cy;   
            double width;    
            double height;   
            double angle;    
            double vx, vy;   
            double mass;     
        } block;
    };
    
    COLORREF color;          
    std::string name;        
};

struct UserInput {
    enum Type {
        MOUSE_DRAG_START,
        MOUSE_DRAG_UPDATE,
        MOUSE_DRAG_END,
        CREATE_SHAPE,
        BUTTON_CLICK,
        PARAMETER_CHANGE
    } type;
    
    POINT mouse_position;
    std::string shape_type; // For CREATE_SHAPE
};

class IPhysicalWorldAdapter {
public:
    virtual ~IPhysicalWorldAdapter() = default;
    
    virtual std::vector<Shape*> getAllShapes() const = 0;
    virtual Shape* findShapeByName(const std::string& name) const = 0;
    
    virtual double getGravity() const = 0;
    virtual void setGravity(double g) = 0;
    virtual double getGroundFriction() const = 0;
    virtual void setGroundFriction(double f) = 0;
    
    virtual void start() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void update(double deltaTime) = 0;
    virtual bool isPaused() const = 0;

    virtual void createBall(double x, double y, double radius, double mass, const char* color) = 0;
    virtual void createBlock(double x, double y, double width, double height, double mass, const char* color) = 0;
};

struct UnifiedButtonStates {
    bool start_clicked;
    bool pause_clicked;
    bool stop_clicked;
    
    char color_choice[10];
    float gravity_coefficient;
    float friction_coefficient;
    float speed_value;
    
    // Additional inputs
    char color_choice2[10];
    float radius_value2;
    float mass_value2;
    float speed_value2;
};

class PhysicsRenderBridge {
private:
    std::unique_ptr<IPhysicalWorldAdapter> physics_adapter_;
    Renderer* renderer_;
    std::unordered_map<std::string, RenderObject> render_objects_;
    UnifiedButtonStates button_states_;
    
    double pixels_per_meter_;
    POINT world_offset_;
    
public:
    PhysicsRenderBridge(std::unique_ptr<IPhysicalWorldAdapter> adapter, Renderer* renderer);
    
    void initialize(double ppm = 100.0, POINT offset = {400, 300});
    
    POINT physicsToScreen(double physics_x, double physics_y) const;
    POINT screenToPhysics(int screen_x, int screen_y) const;
    
    void syncPhysicsToRender();
    void syncInputToPhysics(const std::vector<UserInput>& inputs);
    void updateButtonStates();
    void update(double dt);
    
    const std::unordered_map<std::string, RenderObject>& getRenderObjects() const { return render_objects_; }
    
    bool isPaused() const;

private:
    RenderObject shapeToRenderObject(Shape* shape) const;
    
    void syncFromAllButtons();
    COLORREF parseColor(const char* colorName) const;
};

#endif // PHYSICS_RENDER_BRIDGE_H