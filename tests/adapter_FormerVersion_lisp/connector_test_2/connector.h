// connector.h
#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <vector>
#include <string>

// 物理世界参数
struct PhysicsWorldParams {
    float gravity;           // 重力加速度
    float timeStep;          // 时间步长
    float bounds[4];         // 边界 [左, 右, 下, 上]
    float groundLevel;       // 地面高度
    float groundFriction;    // 地面摩擦系数
};

// 物体物理参数
struct PhysicsObjectParams {
    std::string shapeType;   // 形状类型: "circle", "rectangle", "slope"
    float mass;              // 质量
    float friction;          // 摩擦系数
    float restitution;       // 弹性系数
    
    // 形状特定参数
    union {
        struct { float radius; };                    // 圆形
        struct { float width, height; };            // 矩形
        struct { float angle, length; };            // 斜坡
    };
    
    // 初始状态
    float position[2];       // 初始位置 (x, y)
    float velocity[2];       // 初始速度 (vx, vy)
};

// 物理状态数据（从物理系统获取）
struct PhysicsStateData {
    float position[2];       // 当前位置
    float velocity[2];       // 当前速度
    float totalForce[2];     // 总受力
    float normalForce[2];    // 法向力
    bool isSupported;        // 是否被支撑
    bool isMoving;           // 是否在运动
    float kineticEnergy;     // 动能
    float momentum[2];       // 动量
};

// 渲染数据
struct RenderData {
    int screenX, screenY;    // 屏幕坐标
    int screenSize;          // 屏幕上的大小
    bool isVisible;          // 是否可见
    std::string shapeType;   // 形状类型
    float width, height;     // 矩形尺寸（屏幕坐标）
    float angle;             // 旋转角度
    int color[3];            // 颜色 [R, G, B]
    std::string statusText;  // 状态文本显示
};

// 用户输入参数
struct UserInputParams {
    // 全局参数调节
    PhysicsWorldParams worldParams;
    
    // 物体创建参数
    PhysicsObjectParams objectCreationParams;
    
    // 交互控制
    struct {
        int mouseX, mouseY;          // 鼠标位置
        bool isDragging;             // 是否正在拖拽
        int selectedObjectIndex;     // 选中的物体索引
        bool pauseSimulation;        // 暂停模拟
        bool resetSimulation;        // 重置模拟
    } control;
};

class PhysicsRenderer {
public:
    PhysicsRenderer();
    
    // 坐标转换
    RenderData physicsToRender(const PhysicsStateData& physics, const PhysicsObjectParams& objParams);
    PhysicsObjectParams inputToPhysics(int mouseX, int mouseY, const PhysicsObjectParams& templateParams);
    
    // 参数同步
    void syncWorldParamsToPhysics(const PhysicsWorldParams& params);
    PhysicsWorldParams getCurrentWorldParams() const;
    
    void syncObjectParamsToPhysics(int index, const PhysicsObjectParams& params);
    PhysicsObjectParams getCurrentObjectParams(int index) const;
    
    // 状态同步
    void syncPhysicsStateToRender(const std::vector<PhysicsStateData>& physicsStates,
                                const std::vector<PhysicsObjectParams>& objectParams,
                                std::vector<RenderData>& renderObjects);
    
    void syncUserInputToPhysics(const UserInputParams& input,
                              std::vector<PhysicsObjectParams>& objectParams);
    
    // 工具函数
    std::vector<std::string> getAdjustableParameters() const;
    void setScaleFactor(float scale);
    float getScaleFactor() const;

private:
    float scaleFactor;
    PhysicsWorldParams currentWorldParams;
    
    // 形状特定的渲染计算
    void calculateCircleRenderData(const PhysicsStateData& physics, const PhysicsObjectParams& params, RenderData& render);
    void calculateRectangleRenderData(const PhysicsStateData& physics, const PhysicsObjectParams& params, RenderData& render);
    void calculateSlopeRenderData(const PhysicsStateData& physics, const PhysicsObjectParams& params, RenderData& render);
    
    // 状态颜色计算
    void calculateStatusColor(const PhysicsStateData& physics, int color[3]);
    std::string generateStatusText(const PhysicsStateData& physics);
};

#endif