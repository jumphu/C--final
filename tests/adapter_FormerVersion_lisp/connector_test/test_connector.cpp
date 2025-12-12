// test_connector.cpp
#include <iostream>
#include "connector.h"

int main() {
    PhysicsRenderer connector;
    
    // 测试物理坐标转屏幕坐标
    PhysicsData physObj;
    physObj.x = 2.5f;
    physObj.y = 1.8f;
    physObj.radius = 0.5f;
    
    RenderData renderObj = connector.physicsToRender(physObj);
    std::cout << "物理坐标(" << physObj.x << "," << physObj.y << ") → ";
    std::cout << "屏幕坐标(" << renderObj.screenX << "," << renderObj.screenY << ")" << std::endl;
    
    // 测试屏幕坐标转物理坐标
    PhysicsData draggedObj = connector.inputToPhysics(300, 200);
    std::cout << "屏幕点击(300,200) → 物理坐标(" << draggedObj.x << "," << draggedObj.y << ")" << std::endl;
    
    return 0;
}