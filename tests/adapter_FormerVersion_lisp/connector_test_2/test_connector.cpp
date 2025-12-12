// test_connector.cpp
#include <iostream>
#include <vector>
#include "connector.h"

int main() {
    PhysicsRenderer connector;

    // 测试世界参数同步
    PhysicsWorldParams worldParams;
    worldParams.gravity = 9.8f;
    worldParams.timeStep = 0.02f;
    worldParams.groundLevel = -2.0f;
    worldParams.groundFriction = 0.4f;
    
    connector.syncWorldParamsToPhysics(worldParams);
    PhysicsWorldParams currentParams = connector.getCurrentWorldParams();
    std::cout << "世界参数 - 重力: " << currentParams.gravity 
              << ", 地面摩擦: " << currentParams.groundFriction << std::endl;

    // 测试物体参数
    PhysicsObjectParams circleParams;
    circleParams.shapeType = "circle";
    circleParams.mass = 2.0f;
    circleParams.friction = 0.3f;
    circleParams.restitution = 0.8f;
    circleParams.radius = 1.0f;
    circleParams.position[0] = 0.0f;
    circleParams.position[1] = 5.0f;
    circleParams.velocity[0] = 2.0f;
    circleParams.velocity[1] = 0.0f;

    // 测试物理状态到渲染的转换
    PhysicsStateData physicsState;
    physicsState.position[0] = 2.5f;
    physicsState.position[1] = 1.8f;
    physicsState.velocity[0] = 1.5f;
    physicsState.velocity[1] = 0.0f;
    physicsState.isSupported = true;
    physicsState.isMoving = true;
    physicsState.kineticEnergy = 2.25f;

    RenderData renderObj = connector.physicsToRender(physicsState, circleParams);
    std::cout << "物理坐标(" << physicsState.position[0] << "," << physicsState.position[1] << ") -> ";
    std::cout << "屏幕坐标(" << renderObj.screenX << "," << renderObj.screenY << ")" << std::endl;
    std::cout << "状态文本: " << renderObj.statusText << std::endl;

    // 测试用户输入转换
    PhysicsObjectParams draggedObj = connector.inputToPhysics(300, 200, circleParams);
    std::cout << "屏幕点击(300,200) -> 物理坐标(" << draggedObj.position[0] << "," << draggedObj.position[1] << ")" << std::endl;

    // 测试可调节参数列表
    std::vector<std::string> params = connector.getAdjustableParameters();
    std::cout << "可调节参数: ";
    for (const auto& param : params) {
        std::cout << param << " ";
    }
    std::cout << std::endl;

    return 0;
}