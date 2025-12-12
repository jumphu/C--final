/**
 * ============================================================
 * 文件: PhysicsRenderBridge.cpp
 * 描述: EasyX版本接口桥梁模块的实现文件
 * 作用: 实现PhysicsRenderBridge.h中声明的所有接口方法
 * 关键技术: 
 *   1. EasyX坐标系与物理坐标系的转换算法
 *   2. 状态同步和事件路由机制
 *   3. 与物理世界和渲染系统的数据适配
 * ============================================================
 */

#include "PhysicsRenderBridge.h"
#include <cmath>      // 数学函数: sqrt, sin, cos等
#include <algorithm>  // STL算法: remove_if等
#include <cstdlib>    // 随机数: rand()

// 假设的窗口高度，实际应从Renderer获取或配置
const int SCREEN_HEIGHT = 600;

// ==================== 构造函数和初始化 ====================

/**
 * 构造函数实现
 * 作用: 初始化所有成员变量到默认状态
 * 注意: 这里只是初始化，实际坐标参数在initialize()中设置
 */
PhysicsRenderBridge::PhysicsRenderBridge(PhysicalWorld& physics_world) 
    : physics_world_(physics_world), 
      pixels_per_meter_(100.0), 
      currently_dragging_("") 
{
    // 初始化按钮状态为false
    button_states_.start_clicked = false;
    button_states_.pause_clicked = false;
    button_states_.stop_clicked = false;
    button_states_.start_hovered = false;
    button_states_.pause_hovered = false;
    button_states_.stop_hovered = false;
    
    // 初始化音乐状态
    music_state_.is_playing = false;
    music_state_.is_hovering = false;
    music_state_.position = {320, 80};  // POINT初始化
    music_state_.radius = 60;
}

/**
 * 初始化函数实现
 * 作用: 设置坐标转换参数，清空所有状态
 * 注意: 必须在开始模拟前调用，或重置模拟时调用
 */
void PhysicsRenderBridge::initialize(double ppm, POINT offset) {
    pixels_per_meter_ = ppm;        // 设置像素与米的比例
    world_offset_ = offset;         // 设置世界坐标原点在屏幕的位置
    render_objects_.clear();        // 清空渲染对象缓存
    currently_dragging_ = "";       // 重置拖动状态
}

// ==================== 坐标转换实现 ====================

/**
 * 物理坐标转屏幕坐标实现
 * 关键算法解释:
 *   物理坐标系: 原点在中心，Y轴向上，单位是米
 *   EasyX坐标系: 原点在左上角，Y轴向下，单位是像素
 *   转换公式:
 *     screen_x = physics_x * ppm + offset_x  (X轴方向一致)
 *     screen_y = offset_y - physics_y * ppm  (Y轴方向相反)
 * 注意: 这里假设offset是屏幕中心，实际可能需要调整
 */
POINT PhysicsRenderBridge::physicsToScreen(double physics_x, double physics_y) {
    int screen_x = static_cast<int>(physics_x * pixels_per_meter_ + world_offset_.x + 0.5);
    int screen_y = static_cast<int>(world_offset_.y - physics_y * pixels_per_meter_ + 0.5);
    return {screen_x, screen_y};
}

/**
 * 屏幕坐标转物理坐标实现
 * 关键算法: 坐标转换的逆运算
 * 转换公式:
 *   physics_x = (screen_x - offset_x) / ppm
 *   physics_y = (offset_y - screen_y) / ppm
 * 使用场景: 鼠标交互时将点击位置转换为物理坐标
 */
POINT PhysicsRenderBridge::screenToPhysics(int screen_x, int screen_y) {
    double physics_x = (screen_x - world_offset_.x) / pixels_per_meter_;
    double physics_y = (world_offset_.y - screen_y) / pixels_per_meter_;
    return {static_cast<int>(physics_x), static_cast<int>(physics_y)};
}

// ==================== 核心同步方法实现 ====================

/**
 * 物理到渲染同步实现
 * 算法详细步骤:
 *   1. 遍历物理世界动态形状列表，为每个形状创建或更新渲染状态
 *   2. 遍历物理世界静态形状列表，用灰色区分静态物体
 *   3. 比较渲染缓存和物理世界，删除不存在于物理世界的渲染对象
 * 时间复杂度: O(n+m+k)，n=动态形状数，m=静态形状数，k=渲染对象数
 */
void PhysicsRenderBridge::syncPhysicsToRender() {
    // 步骤1: 同步动态形状(受物理影响的物体)
    for (Shape* shape : physics_world_.dynamicShapeList) {
        std::string name = shape->getName(); // 假设Shape有getName()方法
        if (render_objects_.find(name) == render_objects_.end()) {
            // 新形状: 创建渲染状态
            createRenderShape(name, shape->getType()); // 假设Shape有getType()方法
        }
        // 更新现有形状的渲染状态
        updateRenderStateFromShape(shape, render_objects_[name]);
    }
    
    // 步骤2: 同步静态形状(不受物理影响的物体，如墙壁、地面)
    for (Shape* shape : physics_world_.staticShapeList) {
        std::string name = shape->getName();
        if (render_objects_.find(name) == render_objects_.end()) {
            // 静态物体用灰色区分
            createRenderShape(name, shape->getType(), RGB(150, 150, 150));
        }
        updateRenderStateFromShape(shape, render_objects_[name]);
    }
    
    // 步骤3: 清理已从物理世界删除的形状
    std::vector<std::string> to_remove;
    for (const auto& pair : render_objects_) {
        const std::string& name = pair.first;
        bool found = false;
        
        // 在动态形状中查找
        for (Shape* shape : physics_world_.dynamicShapeList) {
            if (shape->getName() == name) {
                found = true;
                break;
            }
        }
        
        // 在静态形状中查找
        if (!found) {
            for (Shape* shape : physics_world_.staticShapeList) {
                if (shape->getName() == name) {
                    found = true;
                    break;
                }
            }
        }
        
        // 如果物理世界中没有这个形状，标记为待删除
        if (!found) {
            to_remove.push_back(name);
        }
    }
    
    // 执行删除操作
    for (const std::string& name : to_remove) {
        render_objects_.erase(name);
    }
}

/**
 * 输入到物理同步实现
 * 作用: 将用户输入事件分发给对应的处理函数
 * 设计模式: 命令模式，每个输入事件封装为一个命令
 */
void PhysicsRenderBridge::syncInputToPhysics(const std::vector<UserInput>& user_inputs) {
    for (const auto& input : user_inputs) {
        applyUserInputToPhysics(input);
    }
}

// ==================== 事件处理方法实现 ====================

/**
 * 应用用户输入到物理世界实现
 * 关键: 根据事件类型进行分发处理
 * 设计思路: 统一事件处理接口，简化物理世界的交互逻辑
 */
void PhysicsRenderBridge::applyUserInputToPhysics(const UserInput& input) {
    switch (input.type) {
        case UserInput::MOUSE_DRAG_START:
        case UserInput::MOUSE_DRAG_UPDATE:
        case UserInput::MOUSE_DRAG_END:
            handleMouseDrag(input);
            break;
            
        case UserInput::BUTTON_CLICK:
            // 按钮事件由handleButtonEvents()统一处理，这里保持兼容
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
                    // 渲染对象会在下次syncPhysicsToRender时自动清理
                }
            }
            break;
            
        case UserInput::MUSIC_TOGGLE:
            // 切换音乐播放状态(实际播放逻辑在第二部分)
            music_state_.is_playing = !music_state_.is_playing;
            // 这里可以添加音乐控制逻辑，如播放/暂停音频文件
            break;
    }
}

/**
 * 处理鼠标拖动事件实现
 * 算法步骤:
 *   1. DRAG_START: 查找被点击的形状，计算拖动偏移量
 *   2. DRAG_UPDATE: 更新被拖动形状的位置，重置其速度
 *   3. DRAG_END: 清除拖动状态
 */
void PhysicsRenderBridge::handleMouseDrag(const UserInput& input) {
    POINT physics_pos = screenToPhysics(input.mouse_position.x, input.mouse_position.y);
    
    if (input.type == UserInput::MOUSE_DRAG_START) {
        // 查找点击的形状
        currently_dragging_ = findShapeAtPosition(input.mouse_position);
        if (!currently_dragging_.empty()) {
            // 计算拖动偏移量(鼠标位置与形状中心的差值)
            Shape* shape = physics_world_.findShapeByName(currently_dragging_);
            if (shape) {
                drag_offset_.x = physics_pos.x - static_cast<int>(shape->getX());
                drag_offset_.y = physics_pos.y - static_cast<int>(shape->getY());
            }
        }
    }
    else if (input.type == UserInput::MOUSE_DRAG_UPDATE && !currently_dragging_.empty()) {
        // 更新被拖动形状的位置
        Shape* shape = physics_world_.findShapeByName(currently_dragging_);
        if (shape) {
            // 设置新位置(考虑拖动偏移量)
            double new_x = physics_pos.x - drag_offset_.x;
            double new_y = physics_pos.y - drag_offset_.y;
            shape->setPosition(new_x, new_y);
            
            // 重置速度，因为用户拖动覆盖了物理运动
            shape->setVelocity(0, 0);
        }
    }
    else if (input.type == UserInput::MOUSE_DRAG_END) {
        // 结束拖动
        currently_dragging_ = "";
    }
}

/**
 * 处理参数改变事件实现
 * 作用: 更新物理世界的参数设置
 * 支持参数: 重力、倾斜角度、摩擦系数、时间步长
 */
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

/**
 * 处理形状创建事件实现
 * 作用: 在鼠标位置创建新形状
 * 使用场景: 右键点击创建新物体
 */
void PhysicsRenderBridge::handleShapeCreation(const UserInput& input) {
    POINT physics_pos = screenToPhysics(input.mouse_position.x, input.mouse_position.y);
    
    // 使用物理世界的简化接口创建形状
    Shape* new_shape = physics_world_.placeDynamicShapeByType(
        input.shape_type,  // 形状类型
        "user_created_" + std::to_string(rand() % 1000), // 生成唯一名称
        physics_pos.x, physics_pos.y, // 位置
        1.0,   // 默认质量1kg
        0.5    // 默认尺寸0.5米
    );
    
    if (new_shape) {
        // 创建对应的渲染状态
        createRenderShape(new_shape->getName(), input.shape_type);
    }
}

// ==================== 其他方法实现 ====================

/**
 * 根据形状类型获取颜色实现
 * 颜色映射方案:
 *   - 圆形/球体: 红色，表示运动物体
 *   - 矩形/盒子: 蓝色，表示刚体
 *   - 三角形: 绿色，表示特殊形状
 *   - 钟摆: 黄色，表示摆动系统
 *   - 其他: 紫色，表示未知或自定义形状
 */
COLORREF PhysicsRenderBridge::getColorForShapeType(const std::string& type) {
    if (type == "circle" || type == "ball") return RED;
    if (type == "rectangle" || type == "box") return BLUE;
    if (type == "triangle") return GREEN;
    if (type == "pendulum") return YELLOW;
    return MAGENTA; // 默认颜色
}

/**
 * 从Shape对象更新渲染状态实现
 * 关键转换步骤:
 *   1. 坐标转换: 物理坐标 → 屏幕坐标
 *   2. 尺寸转换: 物理尺寸 → 像素尺寸
 *   3. 类型映射: 物理类型 → 渲染类型
 * 注意: 这里使用了假设的Shape接口方法，需要根据实际实现调整
 */
void PhysicsRenderBridge::updateRenderStateFromShape(Shape* shape, RenderState& render_state) {
    // 步骤1: 转换位置坐标
    POINT screen_pos = physicsToScreen(shape->getX(), shape->getY());
    render_state.position = screen_pos;
    
    // 步骤2: 转换尺寸(这里需要根据实际Shape类型调整)
    // 假设圆形有getRadius()，矩形有getWidth()/getHeight()
    if (render_state.type == "circle" || render_state.type == "ball") {
        // 对于圆形，size表示直径
        int diameter = static_cast<int>(shape->getRadius() * 2 * pixels_per_meter_ + 0.5);
        render_state.size = {diameter, diameter};
    } else if (render_state.type == "rectangle" || render_state.type == "box") {
        // 对于矩形，size表示宽度和高度
        int width = static_cast<int>(shape->getWidth() * pixels_per_meter_ + 0.5);
        int height = static_cast<int>(shape->getHeight() * pixels_per_meter_ + 0.5);
        render_state.size = {width, height};
    }
    
    // 步骤3: 更新其他属性(如旋转角度等)
    // render_state.rotation = shape->getRotation();
}