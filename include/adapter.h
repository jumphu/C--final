// adapter.h - 最小可行物理可视化适配器
#pragma once

/***************************************************************
 * 文件说明：物理引擎与可视化系统的连接适配器
 * 核心职责：
 * 1. 同步UI参数到物理引擎
 * 2. 同步物理物体状态到可视化系统
 * 3. 处理用户交互事件
 * 4. 协调物理更新和图形渲染时序
 * 
 * 注意：这是最小可行版本，专注于核心连接功能
 * 需要团队确认的关键决策点已在代码中用【待确认】标出
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

// 场景模式枚举
enum SceneMode {
    SCENE_SINGLE_OBJECT,     // 单物体场景
    SCENE_TWO_OBJECTS,       // 双物体场景
    SCENE_SPHERE_CREATION,   // 球体生成
    SCENE_TWO_STARS,         // 双星系统
    SCENE_SOLAR_SYS,         // 太阳系
    SCENE_PENDULUM,          // 钟摆
    SCENE_NONE               // 无场景
};

// 物理对象类型（与shapes.h对应）
enum PhysicsObjectType {
    OBJ_CIRCLE,    // 圆形
    OBJ_AABB,      // 轴对齐矩形
    OBJ_SLOPE,     // 斜坡
    OBJ_GROUND,    // 地面
    OBJ_WALL,      // 墙壁
    OBJ_PENDULUM,  // 钟摆
    OBJ_GENERIC    // 通用形状
};

// 对象连接信息结构
// 【待确认】决策点1：如何唯一标识物理物体？
// 当前方案：适配器维护自己的ID映射表
struct ObjectConnection {
    int adapterId;          // 适配器分配的ID
    Shape* physicsObject;   // 指向物理引擎中物体的指针
    PhysicsObjectType type; // 物体类型
    
    // 上一次同步的状态（用于变化检测）
    double lastX, lastY;
    double lastVx, lastVy;
    double lastAngle;
    
    // 物理属性缓存
    double radius;      // 圆形半径
    double width, height; // 矩形宽高
    double length;      // 斜坡长度
    double slopeAngle;  // 斜坡角度
    
    // 可视化属性
    COLORREF color;
    bool isVisible;
    
    ObjectConnection() 
        : adapterId(-1), physicsObject(nullptr), type(OBJ_GENERIC),
          lastX(0), lastY(0), lastVx(0), lastVy(0), lastAngle(0),
          radius(0), width(0), height(0), length(0), slopeAngle(0),
          color(RGB(0, 0, 0)), isVisible(true) {}
    
    // 从物理对象更新状态
    void updateFromPhysics();
    
    // 转换为可视化数据
    BallData getBallData() const;
    BlockData getBlockData() const;
    RampData getRampData() const;
};

// 主适配器类
class PhysicsVisualAdapter {
private:
    // 核心组件指针
    Renderer* renderer;                          // 可视化渲染器
    PhysicalWorld* physicsWorld;                 // 物理世界引擎
    DigitalRainBackgroundIntegrated* background; // 背景效果
    MusicPlayer* musicPlayer;                    // 音乐播放器
    
    // 对象管理
    std::unordered_map<int, ObjectConnection> objectConnections; // ID->连接信息
    int nextObjectId;                                            // 下一个可用的ID
    
    // UI状态
    SceneMode currentScene;
    bool isSimulationRunning;
    bool isSimulationPaused;
    
    // UI参数缓存
    struct {
        float gravity = 9.8f;
        float friction = 0.1f;
        float timeScale = 1.0f;
    } uiParams;
    
    // Button Layout Cache
    struct {
        int startX, pauseX, stopX, btnY, btnW, btnH;
        int sceneBtnX, s1Y, s2Y, s3Y, s4Y, sW, sH;
    } btnLayout;
    
    // 交互状态
    int draggedObjectId;      // 当前被拖拽的物体ID
    bool isDragging;          // 是否正在拖拽
    
    // 时间管理
    double lastUpdateTime;    // 上一次更新时间
    double accumulatedTime;   // 累积时间（用于固定时间步长）
    
    // ==================== 内部辅助方法 ====================
    
    // 同步单个物理对象到可视化
    void syncPhysicsObjectToVisual(int adapterId, ObjectConnection& conn);
    
    // 处理UI参数变化
    void handleParameterChanges();
    
    // 处理按钮点击
    void handleButtonClicks();
    
    // 初始化场景
    void initializeScene(SceneMode scene);
    
    // 清理场景
    void cleanupScene();
    
    // 创建物理物体（包装函数）
    int createPhysicsObject(PhysicsObjectType type, 
                           double x, double y,
                           double param1, double param2, // 类型特定参数
                           double mass,
                           COLORREF color,
                           bool isDynamic = true);
    
    // 查找屏幕位置的物体
    int findObjectAtScreen(int screenX, int screenY) const;
    
public:
    // ==================== 公共接口 ====================
    
    // 构造函数和析构函数
    PhysicsVisualAdapter(Renderer* rendererPtr);
    ~PhysicsVisualAdapter();
    
    // 初始化适配器
    // 参数：屏幕宽度、高度
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
    
    // ==================== 调试信息 ====================
    
    // 打印调试信息
    void printDebugInfo() const;
    
    // ==================== 清理资源 ====================
    
    // 清理所有资源
    void cleanup();
};