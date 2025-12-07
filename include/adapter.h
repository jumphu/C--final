// adapter.h - 最小可行物理可视化适配器（修正版）
#pragma once

/***************************************************************
 * 文件说明：物理引擎与可视化系统的连接适配器
 * 修正内容：
 * 1. 修复时间步长获取错误（使用physicsWorld->timeStep而非getTimeStep()）
 * 2. 添加按钮位置结构体，用于管理多个UI系统的位置信息
 * 3. 完善UI系统集成，支持5个独立的按钮系统
 * 4. 修正拖拽时的物理状态管理
 * 5. 优化场景切换和参数同步逻辑
 * 
 * 核心职责：
 * 1. 同步UI参数到物理引擎
 * 2. 同步物理物体状态到可视化系统
 * 3. 处理用户交互事件（鼠标、键盘、按钮）
 * 4. 协调物理更新和图形渲染时序
 * 
 * 架构说明：
 * - 适配器作为模型（物理引擎）和视图（可视化系统）之间的控制器
 * - 使用固定时间步长进行稳定的物理模拟
 * - 支持多场景切换和动态物体管理
 * - 与5个独立的UI按钮系统集成
 ***************************************************************/

#include "Renderer.h"
#include "allbuttons.h"
#include "background_integrated.h"
#include "music.h"
#include "shapes.h"
#include "physicalWorld.h"

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>
#include <functional>

// 场景模式枚举
enum SceneMode {
    SCENE_SINGLE_OBJECT,     // 单物体场景（斜坡上的球）
    SCENE_TWO_OBJECTS,       // 双物体场景（碰撞演示）
    SCENE_SPHERE_CREATION,   // 球体生成场景（多球体）
    SCENE_TWO_STARS,         // 双星系统场景
    SCENE_SOLAR_SYS,         // 太阳系场景
    SCENE_PENDULUM,          // 钟摆场景（待实现）
    SCENE_NONE               // 无场景
};

// 物理对象类型（与shapes.h对应）
enum PhysicsObjectType {
    OBJ_CIRCLE,    // 圆形（球体）
    OBJ_AABB,      // 轴对齐矩形（方块）
    OBJ_SLOPE,     // 斜坡
    OBJ_GROUND,    // 地面
    OBJ_WALL,      // 墙壁
    OBJ_PENDULUM,  // 钟摆
    OBJ_GENERIC    // 通用形状
};

// 对象连接信息结构
// 说明：维护物理引擎物体与可视化物体之间的映射关系
// 每个连接包含物理状态、可视化属性和同步信息
struct ObjectConnection {
    int adapterId;          // 适配器分配的唯一ID（用于内部管理）
    Shape* physicsObject;   // 指向物理引擎中物体的指针
    PhysicsObjectType type; // 物体类型
    
    // 上一次同步的状态（用于变化检测和增量更新）
    double lastX, lastY;    // 上一次的位置
    double lastVx, lastVy;  // 上一次的速度
    double lastAngle;       // 上一次的角度（对于旋转物体）
    
    // 物理属性缓存（减少动态类型转换开销）
    double radius;          // 圆形半径
    double width, height;   // 矩形宽高
    double length;         // 斜坡长度
    double slopeAngle;     // 斜坡角度
    
    // 可视化属性
    COLORREF color;        // 物体颜色
    bool isVisible;        // 是否可见
    
    // 构造函数：初始化所有成员变量
    ObjectConnection() 
        : adapterId(-1), physicsObject(nullptr), type(OBJ_GENERIC),
          lastX(0), lastY(0), lastVx(0), lastVy(0), lastAngle(0),
          radius(0), width(0), height(0), length(0), slopeAngle(0),
          color(RGB(0, 0, 0)), isVisible(true) {}
    
    // 从物理对象更新状态（从物理引擎读取最新状态）
    void updateFromPhysics();
    
    // 转换为可视化数据结构（供渲染器使用）
    BallData getBallData() const;    // 转换为球体数据
    BlockData getBlockData() const;  // 转换为方块数据
    RampData getRampData() const;    // 转换为斜坡数据
};

// 主适配器类
class PhysicsVisualAdapter {
private:
    // ==================== 核心组件指针 ====================
    Renderer* renderer;                          // 可视化渲染器（负责绘图）
    PhysicalWorld* physicsWorld;                 // 物理世界引擎（负责计算）
    DigitalRainBackgroundIntegrated* background; // 背景效果
    MusicPlayer* musicPlayer;                    // 音乐播放器
    
    // ==================== 对象管理 ====================
    std::unordered_map<int, ObjectConnection> objectConnections; // ID->连接信息的映射表
    int nextObjectId;                                            // 下一个可用的ID
    
    // ==================== UI状态 ====================
    SceneMode currentScene;      // 当前场景模式
    bool isSimulationRunning;    // 模拟是否正在运行
    bool isSimulationPaused;     // 模拟是否已暂停
    
    // ==================== UI参数缓存 ====================
    // 说明：缓存UI参数，避免频繁从UI系统获取，同时检测变化
    struct {
        float gravity = 9.8f;    // 重力加速度（m/s²）
        float friction = 0.1f;   // 摩擦系数
        float timeScale = 1.0f;  // 时间缩放因子
    } uiParams;
    
    // ==================== 交互状态 ====================
    int draggedObjectId;         // 当前被拖拽的物体ID
    bool isDragging;             // 是否正在拖拽
    
    // ==================== 时间管理 ====================
    // 说明：使用固定时间步长进行物理模拟，累积多余时间
    double lastUpdateTime;       // 上一次更新时间
    double accumulatedTime;      // 累积时间（用于固定时间步长）
    
    // ==================== UI按钮位置管理 ====================
    // 说明：管理5个独立UI系统的按钮位置信息
    struct ButtonPositions {
        // 控制按钮（threebottums.cpp）：开始/暂停/停止
        int startBtnX, pauseBtnX, stopBtnX, btnY, btnW, btnH;
        
        // 场景选择按钮1（choose.cpp）：Model 1/2/3
        int chooseBtnX, model1BtnY, model2BtnY, model3BtnY, chooseBtnW, chooseBtnH;
        
        // 场景选择按钮2（spacebottum.cpp）：行星体系场景
        int spaceBtnX, sphereCreationBtnY, twoStarsBtnY, solarSysBtnY, spaceBtnW, spaceBtnH;
    };
    
    ButtonPositions buttonPos;   // 按钮位置信息
    
    // ==================== 内部辅助方法 ====================
    
    // 同步单个物理对象到可视化
    void syncPhysicsObjectToVisual(int adapterId, ObjectConnection& conn);
    
    // 处理UI参数变化（从按钮系统读取并同步到物理引擎）
    void handleParameterChanges();
    
    // 处理按钮点击（检查所有按钮系统状态）
    void handleButtonClicks();
    
    // 初始化场景（根据场景类型创建物体）
    void initializeScene(SceneMode scene);
    
    // 清理场景（释放场景资源）
    void cleanupScene();
    
    // 创建物理物体（包装函数，简化物体创建）
    // 参数说明：
    //   type: 物体类型
    //   x, y: 初始位置
    //   param1, param2: 类型特定参数（半径、宽高等）
    //   mass: 质量
    //   color: 颜色
    //   isDynamic: 是否为动态物体
    int createPhysicsObject(PhysicsObjectType type, 
                           double x, double y,
                           double param1, double param2,
                           double mass,
                           COLORREF color,
                           bool isDynamic = true);
    
    // 查找屏幕位置的物体（用于鼠标交互）
    int findObjectAtScreen(int screenX, int screenY) const;
    
    // 颜色字符串转换为COLORREF
    COLORREF stringToColor(const std::string& colorStr) const;
    
    // 应用UI参数到物理世界
    void applyUIParametersToPhysics();
    
public:
    // ==================== 公共接口 ====================
    
    // 构造函数和析构函数
    PhysicsVisualAdapter(Renderer* rendererPtr);
    ~PhysicsVisualAdapter();
    
    // 初始化适配器
    // 参数：screenWidth - 屏幕宽度（像素）
    //       screenHeight - 屏幕高度（像素）
    // 返回：初始化是否成功
    bool initialize(int screenWidth, int screenHeight);
    
    // 主更新函数（每帧调用）
    // 参数：deltaTime - 距上一帧的时间（秒）
    void updateFrame(float deltaTime);
    
    // 渲染函数（每帧调用）
    void renderFrame();
    
    // ==================== 用户输入处理 ====================
    
    // 鼠标点击事件
    void onMouseClick(int screenX, int screenY, bool leftButton = true);
    
    // 鼠标拖拽开始
    void onMouseDragStart(int screenX, int screenY);
    
    // 鼠标拖拽更新
    void onMouseDragUpdate(int screenX, int screenY);
    
    // 鼠标拖拽结束
    void onMouseDragEnd(int screenX, int screenY);
    
    // 鼠标移动事件
    void onMouseMove(int screenX, int screenY);
    
    // 键盘按键事件
    void onKeyPress(char key);
    
    // ==================== 场景控制 ====================
    
    // 切换场景
    void switchScene(SceneMode newScene);
    
    // 开始模拟
    void startSimulation();
    
    // 暂停模拟
    void pauseSimulation();
    
    // 停止模拟
    void stopSimulation();
    
    // 重置模拟
    void resetSimulation();
    
    // ==================== 参数设置 ====================
    
    // 设置重力
    void setGravity(float gravity);
    
    // 设置摩擦系数
    void setFriction(float friction);
    
    // 设置时间缩放
    void setTimeScale(float scale);
    
    // ==================== 状态获取 ====================
    
    // 获取当前场景
    SceneMode getCurrentScene() const { return currentScene; }
    
    // 获取模拟状态
    bool isRunning() const { return isSimulationRunning; }
    bool isPaused() const { return isSimulationPaused; }
    
    // 获取物体数量
    int getObjectCount() const { return static_cast<int>(objectConnections.size()); }
    
    // 是否正在拖拽（公开用于主循环）
    bool getIsDragging() const { return isDragging; }
    
    // ==================== 调试信息 ====================
    
    // 打印调试信息
    void printDebugInfo() const;
    
    // ==================== 清理资源 ====================
    
    // 清理所有资源
    void cleanup();
};
