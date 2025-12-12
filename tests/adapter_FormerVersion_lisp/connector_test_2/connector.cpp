// connector.cpp
#include "connector.h"
#include <cmath>
#include <sstream>
#include <iomanip>

PhysicsRenderer::PhysicsRenderer() {
    scaleFactor = 100.0f;
    
    // 初始化默认世界参数
    currentWorldParams.gravity = 9.8f;
    currentWorldParams.timeStep = 0.016f; // ~60 FPS
    currentWorldParams.bounds[0] = -10.0f; // left
    currentWorldParams.bounds[1] = 10.0f;  // right
    currentWorldParams.bounds[2] = -10.0f; // bottom
    currentWorldParams.bounds[3] = 10.0f;  // top
    currentWorldParams.groundLevel = -5.0f;
    currentWorldParams.groundFriction = 0.3f;
}

RenderData PhysicsRenderer::physicsToRender(const PhysicsStateData& physics, const PhysicsObjectParams& objParams) {
    RenderData render;
    
    // 基础坐标转换
    render.screenX = static_cast<int>(physics.position[0] * scaleFactor);
    render.screenY = static_cast<int>(physics.position[1] * scaleFactor);
    render.shapeType = objParams.shapeType;
    render.isVisible = true;
    
    // 形状特定的渲染计算
    if (objParams.shapeType == "circle") {
        calculateCircleRenderData(physics, objParams, render);
    } else if (objParams.shapeType == "rectangle") {
        calculateRectangleRenderData(physics, objParams, render);
    } else if (objParams.shapeType == "slope") {
        calculateSlopeRenderData(physics, objParams, render);
    }
    
    // 状态相关的颜色和文本
    calculateStatusColor(physics, render.color);
    render.statusText = generateStatusText(physics);
    
    return render;
}

PhysicsObjectParams PhysicsRenderer::inputToPhysics(int mouseX, int mouseY, const PhysicsObjectParams& templateParams) {
    PhysicsObjectParams physics = templateParams;
    
    // 转换屏幕坐标到物理坐标
    physics.position[0] = mouseX / scaleFactor;
    physics.position[1] = mouseY / scaleFactor;
    
    return physics;
}

void PhysicsRenderer::syncWorldParamsToPhysics(const PhysicsWorldParams& params) {
    currentWorldParams = params;
}

PhysicsWorldParams PhysicsRenderer::getCurrentWorldParams() const {
    return currentWorldParams;
}

void PhysicsRenderer::syncObjectParamsToPhysics(int index, const PhysicsObjectParams& params) {
    // 在实际实现中，这里会更新物理系统中的对应物体
    // 目前只是存储参数
}

PhysicsObjectParams PhysicsRenderer::getCurrentObjectParams(int index) const {
    // 在实际实现中，这里会从物理系统获取物体参数
    // 目前返回默认值
    PhysicsObjectParams params;
    params.shapeType = "circle";
    params.mass = 1.0f;
    params.friction = 0.3f;
    params.restitution = 0.7f;
    params.radius = 0.5f;
    params.position[0] = 0.0f;
    params.position[1] = 0.0f;
    params.velocity[0] = 0.0f;
    params.velocity[1] = 0.0f;
    return params;
}

void PhysicsRenderer::syncPhysicsStateToRender(const std::vector<PhysicsStateData>& physicsStates,
                                             const std::vector<PhysicsObjectParams>& objectParams,
                                             std::vector<RenderData>& renderObjects) {
    renderObjects.clear();
    
    for (size_t i = 0; i < physicsStates.size() && i < objectParams.size(); ++i) {
        RenderData render = physicsToRender(physicsStates[i], objectParams[i]);
        renderObjects.push_back(render);
    }
}

void PhysicsRenderer::syncUserInputToPhysics(const UserInputParams& input,
                                           std::vector<PhysicsObjectParams>& objectParams) {
    // 更新世界参数
    syncWorldParamsToPhysics(input.worldParams);
    
    // 处理物体拖拽
    if (input.control.isDragging && input.control.selectedObjectIndex >= 0 && 
        input.control.selectedObjectIndex < static_cast<int>(objectParams.size())) {
        
        PhysicsObjectParams& obj = objectParams[input.control.selectedObjectIndex];
        obj.position[0] = input.control.mouseX / scaleFactor;
        obj.position[1] = input.control.mouseY / scaleFactor;
        // 拖拽时速度清零
        obj.velocity[0] = 0.0f;
        obj.velocity[1] = 0.0f;
    }
}

std::vector<std::string> PhysicsRenderer::getAdjustableParameters() const {
    return {
        "gravity", "timeStep", "groundLevel", "groundFriction",
        "mass", "friction", "restitution", "radius", "width", "height"
    };
}

void PhysicsRenderer::setScaleFactor(float scale) {
    scaleFactor = scale;
}

float PhysicsRenderer::getScaleFactor() const {
    return scaleFactor;
}

// 私有方法实现
void PhysicsRenderer::calculateCircleRenderData(const PhysicsStateData& physics, const PhysicsObjectParams& params, RenderData& render) {
    render.screenSize = static_cast<int>(params.radius * scaleFactor * 2);
    render.width = params.radius * scaleFactor * 2;
    render.height = params.radius * scaleFactor * 2;
    render.angle = 0.0f;
}

void PhysicsRenderer::calculateRectangleRenderData(const PhysicsStateData& physics, const PhysicsObjectParams& params, RenderData& render) {
    render.screenSize = static_cast<int>(std::max(params.width, params.height) * scaleFactor);
    render.width = params.width * scaleFactor;
    render.height = params.height * scaleFactor;
    render.angle = 0.0f;
}

void PhysicsRenderer::calculateSlopeRenderData(const PhysicsStateData& physics, const PhysicsObjectParams& params, RenderData& render) {
    render.screenSize = static_cast<int>(params.length * scaleFactor);
    render.width = params.length * scaleFactor;
    render.height = 20; // 斜坡厚度
    render.angle = params.angle;
}

void PhysicsRenderer::calculateStatusColor(const PhysicsStateData& physics, int color[3]) {
    // 根据状态设置颜色
    if (!physics.isMoving) {
        color[0] = 100; color[1] = 100; color[2] = 255; // 蓝色 - 静止
    } else if (physics.isSupported) {
        color[0] = 100; color[1] = 255; color[2] = 100; // 绿色 - 运动且被支撑
    } else {
        color[0] = 255; color[1] = 100; color[2] = 100; // 红色 - 空中运动
    }
}

std::string PhysicsRenderer::generateStatusText(const PhysicsStateData& physics) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "v=(" << physics.velocity[0] << "," << physics.velocity[1] << ")";
    
    if (physics.kineticEnergy > 0.1f) {
        ss << " E=" << physics.kineticEnergy;
    }
    
    return ss.str();
}