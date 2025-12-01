#include "../include/physicalWorld.h"
#include <iostream>

/*=========================================================================================================
 * 快速测试 - 验证基本功能
 *=========================================================================================================*/

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   物理引擎快速测试" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 1. 创建物理世界
    std::cout << "1. 创建物理世界..." << std::endl;
    PhysicalWorld world;
    world.setGravity(10.0);
    std::cout << "   ? 重力设置为: " << world.getGravity() << " m/s?\n" << std::endl;
    
    // 2. 创建地面
    std::cout << "2. 创建地面..." << std::endl;
    Ground ground(0.0, 0.3);
    std::cout << "   ? 地面 Y 坐标: " << ground.getYLevel() << std::endl;
    std::cout << "   ? 摩擦系数: " << ground.getFriction() << "\n" << std::endl;
    
    // 3. 创建形状
    std::cout << "3. 创建形状..." << std::endl;
    Shape* ball = world.placeDynamicShapeByType("Circle", "Ball", 0.0, 100.0, 1.0, 5.0);
    Shape* box = world.placeDynamicShapeByType("AABB", "Box", 50.0, 100.0, 2.0, 10.0, 10.0);
    Wall* wall = world.placeWall("Wall", 100.0, 25.0, 5.0, 50.0, 0.5);
    
    std::cout << "   ? 创建圆形: " << ball->getName() << std::endl;
    std::cout << "   ? 创建矩形: " << box->getName() << std::endl;
    std::cout << "   ? 创建墙壁: " << wall->getName() << std::endl;
    std::cout << "   动态形状数: " << world.getDynamicShapeCount() << std::endl;
    std::cout << "   静态形状数: " << world.getStaticShapeCount() << "\n" << std::endl;
    
    // 4. 运行模拟
    std::cout << "4. 运行物理模拟 (60 帧)..." << std::endl;
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(ball);
    shapeList.push_back(box);
    
    double x, y, vx, vy;
    
    // 初始状态
    ball->getCentre(x, y);
    std::cout << "   初始: Ball 位置 (" << x << ", " << y << ")" << std::endl;
    
    // 模拟 60 帧 (1 秒)
    world.start();
    for (int i = 0; i < 60; i++) {
        world.update(shapeList, ground);
    }
    
    // 最终状态
    ball->getCentre(x, y);
    ball->getVelocity(vx, vy);
    std::cout << "   1秒后: Ball 位置 (" << x << ", " << y << "), 速度 (" << vx << ", " << vy << ")" << std::endl;
    
    box->getCentre(x, y);
    box->getVelocity(vx, vy);
    std::cout << "   1秒后: Box 位置 (" << x << ", " << y << "), 速度 (" << vx << ", " << vy << ")\n" << std::endl;
    
    // 5. 测试倾斜
    std::cout << "5. 测试倾斜系统..." << std::endl;
    world.setInclineAngle(30.0);
    std::cout << "   ? 设置倾斜角度: " << world.getInclineAngle() << "°" << std::endl;
    
    auto displayCoords = world.inclineToStandard(100.0, 100.0);
    std::cout << "   坐标转换: (100, 100) → (" 
              << displayCoords[0] << ", " << displayCoords[1] << ")\n" << std::endl;
    
    // 6. 测试暂停/恢复
    std::cout << "6. 测试暂停/恢复..." << std::endl;
    ball->setCentre(0.0, 50.0);
    ball->setVelocity(0.0, 0.0);
    
    for (int i = 0; i < 30; i++) {
        world.update(shapeList, ground);
    }
    
    ball->getCentre(x, y);
    double y_before_pause = y;
    std::cout << "   暂停前: Y = " << y << std::endl;
    
    world.Pause();
    for (int i = 0; i < 30; i++) {
        world.update(shapeList, ground);
    }
    
    ball->getCentre(x, y);
    std::cout << "   暂停中: Y = " << y << " (应该不变)" << std::endl;
    
    world.Continue();
    for (int i = 0; i < 30; i++) {
        world.update(shapeList, ground);
    }
    
    ball->getCentre(x, y);
    std::cout << "   恢复后: Y = " << y << " (应该继续变化)\n" << std::endl;
    
    // 7. 打印所有形状信息
    std::cout << "7. 所有形状信息:" << std::endl;
    world.printAllShapes();
    
    // 完成
    std::cout << "\n========================================" << std::endl;
    std::cout << "   ? 所有测试完成！" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    return 0;
}
