#include "../include/physicalWorld.h"
#include <iostream>
#include <cmath>

/*=========================================================================================================
 * 简化调试测试 - 检查支撑状态和力的施加
 *=========================================================================================================*/

int main() {
    std::cout << "========== 斜面滑动调试测试 ==========" << std::endl;
    
    double angle = 30.0;
    double g = 10.0;
    double friction = 0.2;
    
    PhysicalWorld world;
    world.setGravity(g);
    world.setInclineAngle(angle);
    world.setTimeStep(1.0 / 60.0);
    
    world.ground.setYLevel(0.0);
    world.ground.setFriction(friction);
    
    Shape* ball = world.placeDynamicShapeByType("Circle", "Ball", 0.0, 0.0, 1.0, 5.0);
    ball->setFraction(friction);
    ball->setVelocity(0.0, 0.0);
    ball->setRestitution(0.0);
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(ball);
    
    std::cout << "\n初始设置:" << std::endl;
    double x, y, vx, vy, f;
    ball->getCentre(x, y);
    ball->getVelocity(vx, vy);
    ball->getFraction(f);
    std::cout << "位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "速度: (" << vx << ", " << vy << ")" << std::endl;
    std::cout << "摩擦系数: " << f << std::endl;
    std::cout << "地面摩擦系数: " << world.ground.getFriction() << std::endl;
    std::cout << "被支撑: " << (ball->getIsSupported() ? "是" : "否") << std::endl;
    std::cout << "底部Y: " << ball->getBottom() << ", 地面Y: " << world.ground.getYLevel() << std::endl;
    
    world.start();
    
    // 先稳定几帧
    for (int i = 0; i < 5; i++) {
        world.update(shapeList, world.ground);
    }
    
    std::cout << "\n稳定后 (5帧):" << std::endl;
    ball->getCentre(x, y);
    ball->getVelocity(vx, vy);
    std::cout << "位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "速度: (" << vx << ", " << vy << ")" << std::endl;
    std::cout << "被支撑: " << (ball->getIsSupported() ? "是" : "否") << std::endl;
    
    // 运行60帧 (1秒)
    for (int i = 0; i < 60; i++) {
        world.update(shapeList, world.ground);
    }
    
    std::cout << "\n1秒后 (60帧):" << std::endl;
    ball->getCentre(x, y);
    ball->getVelocity(vx, vy);
    double speed = sqrt(vx * vx + vy * vy);
    std::cout << "位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "速度: (" << vx << ", " << vy << "), 大小: " << speed << std::endl;
    std::cout << "被支撑: " << (ball->getIsSupported() ? "是" : "否") << std::endl;
    
    std::cout << "\n理论分析 (30°斜面, μ=0.2):" << std::endl;
    double angleRad = angle * 3.14159265 / 180.0;
    double a_theory = g * (sin(angleRad) - friction * cos(angleRad));
    double v_theory = a_theory * 1.0;
    std::cout << "  理论加速度: a = g*(sin(θ) - μ*cos(θ)) = 10*(0.5 - 0.2*0.866) = " << a_theory << " m/s?" << std::endl;
    std::cout << "  理论速度(1秒): v = a*t = " << v_theory << " m/s" << std::endl;
    std::cout << "  实际速度: " << fabs(vx) << " m/s" << std::endl;
    std::cout << "  误差: " << fabs(fabs(vx) - v_theory) / v_theory * 100.0 << "%" << std::endl;
    
    // 无摩擦对比
    std::cout << "\n对比：无摩擦情况" << std::endl;
    double a_no_friction = g * sin(angleRad);
    double v_no_friction = a_no_friction * 1.0;
    std::cout << "  理论加速度: a = g*sin(θ) = 10*0.5 = " << a_no_friction << " m/s?" << std::endl;
    std::cout << "  理论速度(1秒): " << v_no_friction << " m/s" << std::endl;
    
    return 0;
}
