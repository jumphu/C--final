/*=========================================================================================================
 * 摩擦力调试测试 - 详细输出每一帧的物理量
 *=========================================================================================================*/

#include "../include/physicalWorld.h"
#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    std::cout << "========== 摩擦力调试测试 ==========" << std::endl;
    std::cout << "\n单个球在水平地面滑动\n" << std::endl;
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);
    world.setTimeStep(1.0 / 60.0);
    
    world.ground.setYLevel(0.0);
    world.ground.setFriction(0.3);
    
    Shape* ball = world.placeDynamicShapeByType("Circle", "Ball", 0.0, 0.0, 1.0, 5.0);
    ball->setVelocity(10.0, 0.0);
    ball->setRestitution(0.0);
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(ball);
    
    std::cout << "理论计算：" << std::endl;
    std::cout << "  质量 m = 1 kg" << std::endl;
    std::cout << "  **世界重力 = " << world.getGravity() << " m/s?**" << std::endl;
    std::cout << "  重力 g = 10 m/s?" << std::endl;
    std::cout << "  摩擦系数 μ = 0.3" << std::endl;
    std::cout << "  初始速度 v? = 10 m/s" << std::endl;
    std::cout << "  **实际时间步长 dt = " << world.getTimeStep() << " s**" << std::endl;
    std::cout << "  正压力 N = m * g = 1 * 10 = 10 N" << std::endl;
    std::cout << "  摩擦力 f = μ * N = 0.3 * 10 = 3 N" << std::endl;
    std::cout << "  减速度 a = f / m = 3 / 1 = 3 m/s?" << std::endl;
    std::cout << "  时间步长 dt = 1/60 s = 0.01667 s" << std::endl;
    std::cout << "  每帧速度变化 Δv = a * dt = 3 * 0.01667 = 0.05 m/s" << std::endl;
    std::cout << "  理论停止时间 t = v? / a = 10 / 3 = 3.333 s" << std::endl;
    std::cout << "  理论停止帧数 = 3.333 * 60 = 200 帧\n" << std::endl;
    
    world.start();
    
    // 稳定化
    for (int i = 0; i < 30; i++) {
        world.update(shapeList, world.ground);
    }
    
    // 重置速度
    ball->setVelocity(10.0, 0.0);
    
    std::cout << "实际模拟（显示前20帧）：\n" << std::endl;
    std::cout << std::setw(6) << "帧"
              << std::setw(12) << "速度(m/s)"
              << std::setw(14) << "速度变化"
              << std::setw(12) << "位置X(m)"
              << std::endl;
    std::cout << std::string(44, '-') << std::endl;
    
    double prevVx = 10.0;
    
    for (int frame = 0; frame <= 25; frame++) {
        double x, y, vx, vy;
        ball->getCentre(x, y);
        ball->getVelocity(vx, vy);
        
        double deltaV = vx - prevVx;
        
        std::cout << std::setw(6) << frame
                  << std::setw(12) << std::fixed << std::setprecision(6) << vx
                  << std::setw(14) << std::setprecision(6) << deltaV
                  << std::setw(12) << std::setprecision(3) << x
                  << std::endl;
        
        prevVx = vx;
        world.update(shapeList, world.ground);
    }
    
    // 继续模拟到停止
    int stopFrame = -1;
    for (int frame = 26; frame <= 250; frame++) {
        double vx, vy;
        ball->getVelocity(vx, vy);
        
        if (std::abs(vx) < 0.01 && stopFrame < 0) {
            stopFrame = frame;
            std::cout << "\n物体在第 " << stopFrame << " 帧停止" << std::endl;
            std::cout << "理论值: 200 帧" << std::endl;
            std::cout << "误差: " << (200 - stopFrame) << " 帧 (" 
                      << std::fixed << std::setprecision(1)
                      << (100.0 * (200 - stopFrame) / 200.0) << "%)" << std::endl;
            break;
        }
        
        world.update(shapeList, world.ground);
    }
    
    return 0;
}
