/**
 * ============================================================
 * 文件: PhysicsRenderBridge.h (适配版)
 * 描述: 适配队友接口的桥梁模块
 * 更新内容:
 *   1. 适配shapes.h中的Shape类接口
 *   2. 统一多个按钮系统的接口
 *   3. 适配Renderer的数据结构
 *   4. 添加向下转型功能
 * ============================================================
 */

#ifndef PHYSICS_RENDER_BRIDGE_H
#define PHYSICS_RENDER_BRIDGE_H

#include <graphics.h>
#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

// 队友的头文件
#include "shapes.h"        // 第一部分：物理形状定义
#include "Renderer.h"      // 第二部分：渲染器
#include "ALL_BUTTONS_H"   // 统一使用这个按钮头文件
#include "MusicPlayer.h"   // 使用其中一个音乐播放器

// ==================== 新的数据结构（适配Renderer）====================

/**
 * RenderObject - 渲染对象通用结构
 * 作用: 统一存储不同类型的渲染数据，供Renderer使用
 * 设计: 使用枚举区分类型，避免复杂的继承
 */
struct RenderObject {
    enum Type { BALL, RAMP, BLOCK, GROUND, WALL } type;
    
    union {
        struct {
            double x, y;     // 质心位置(米)
            double radius;   // 半径(米)
            double vx, vy;   // 速度(m/s)
            double mass;     // 质量(kg)
        } ball;
        
        struct {
            double x1, y1;   // 起点(米)
            double x2, y2;   // 终点(米)
            double mu;       // 摩擦系数
        } ramp;
        
        struct {
            double cx, cy;   // 中心位置(米)
            double width;    // 宽度(米)
            double height;   // 高度(米)
            double angle;    // 旋转角度(弧度)
            double vx, vy;   // 速度(m/s)
            double mass;     // 质量(kg)
        } block;
    };
    
    COLORREF color;          // 颜色
    std::string name;        // 对象名称
};

/**
 * PhysicalWorldAdapter - 物理世界适配器接口
 * 作用: 为不同的物理世界实现提供统一接口
 * 设计模式: 适配器模式
 */
class IPhysicalWorldAdapter {
public:
    virtual ~IPhysicalWorldAdapter() = default;
    
    // 形状管理
    virtual std::vector<Shape*> getAllShapes() const = 0;
    virtual Shape* findShapeByName(const std::string& name) const = 0;
    virtual bool removeShape(const std::string& name) = 0;
    
    // 物理参数
    virtual double getGravity() const = 0;
    virtual void setGravity(double g) = 0;
    virtual double getGroundFriction() const = 0;
    virtual void setGroundFriction(double f) = 0;
    
    // 模拟控制
    virtual void start() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void stop() = 0;
    virtual void clear() = 0;
    virtual void update(double deltaTime) = 0;
    
    // 形状创建（简化接口）
    virtual Shape* createBall(const std::string& name, double x, double y, 
                              double radius, double mass, COLORREF color) = 0;
    virtual Shape* createBlock(const std::string& name, double x, double y,
                               double width, double height, double mass, COLORREF color) = 0;
};

// ==================== 统一按钮状态结构 ====================

/**
 * UnifiedButtonStates - 统一所有按钮系统的状态
 * 作用: 合并多个按钮头文件的接口
 */
struct UnifiedButtonStates {
    // 主控制按钮（来自Buttons.h）
    bool start_clicked;
    bool pause_clicked;
    bool stop_clicked;
    bool start_hovered;
    bool pause_hovered;
    bool stop_hovered;
    
    // 附加按钮1（来自AdditionalButtons.h）
    bool color_clicked;
    bool gravity_clicked;
    bool friction_clicked;
    bool speed_clicked;
    
    // 附加按钮2（来自ALL_BUTTONS_H的第二组）
    bool color2_clicked;
    bool radius2_clicked;
    bool mass2_clicked;
    bool speed2_clicked;
    
    // 参数值
    char color_choice[10];
    float gravity_coefficient;
    float friction_coefficient;
    float speed_value;
    
    // 第二组参数
    char color_choice2[10];
    float radius_value2;
    float mass_value2;
    float speed_value2;
};

// ==================== 核心桥梁类 ====================

class PhysicsRenderBridge {
private:
    // 物理世界适配器（而非直接引用）
    std::unique_ptr<IPhysicalWorldAdapter> physics_adapter_;
    
    // 渲染器引用
    Renderer* renderer_;
    
    // 状态存储
    std::unordered_map<std::string, RenderObject> render_objects_;
    UnifiedButtonStates button_states_;
    
    // 交互状态
    std::string dragging_shape_;
    POINT drag_offset_;
    
    // 坐标转换参数
    double pixels_per_meter_;
    POINT world_offset_;
    int screen_height_;
    
public:
    // ========== 构造函数和初始化 ==========
    PhysicsRenderBridge(std::unique_ptr<IPhysicalWorldAdapter> adapter, Renderer* renderer);
    void initialize(double ppm = 100.0, POINT offset = {400, 300});
    
    // ========== 坐标转换接口 ==========
    POINT physicsToScreen(double physics_x, double physics_y) const;
    POINT screenToPhysics(int screen_x, int screen_y) const;
    
    // ========== 核心同步接口 ==========
    void syncPhysicsToRender();
    void syncInputToPhysics(const std::vector<UserInput>& inputs);
    
    // ========== 按钮和状态管理 ==========
    void updateButtonStates();
    void updateMusicState(bool is_playing, bool is_hovering, POINT position, int radius);
    
    // ========== 渲染数据获取 ==========
    const std::unordered_map<std::string, RenderObject>& getRenderObjects() const { return render_objects_; }
    const UnifiedButtonStates& getButtonStates() const { return button_states_; }
    
    // ========== 物理参数接口 ==========
    double getGravity() const;
    double getFriction() const;
    bool isPaused() const;
    
    // ========== 形状管理 ==========
    std::string findShapeAtPosition(POINT screen_pos, int tolerance = 10);
    bool createShapeAtPosition(const std::string& type, POINT screen_pos, 
                              const std::string& name = "");
    
private:
    // ========== 私有辅助方法 ==========
    
    // 形状转换方法
    RenderObject shapeToRenderObject(Shape* shape) const;
    
    // 类型检测和向下转型
    Circle* asCircle(Shape* shape) const;
    AABB* asAABB(Shape* shape) const;
    Slope* asSlope(Shape* shape) const;
    Ground* asGround(Shape* shape) const;
    Wall* asWall(Shape* shape) const;
    
    // 事件处理方法
    void handleMouseDrag(const UserInput& input);
    void handleButtonClick(const UserInput& input);
    void handleParameterChange(const UserInput& input);
    
    // 按钮状态同步
    void syncFromButtonsH();      // 从Buttons.h同步
    void syncFromAdditionalButtons(); // 从AdditionalButtons.h同步
    void syncFromAllButtons();    // 从ALL_BUTTONS_H同步
};

#endif // PHYSICS_RENDER_BRIDGE_H