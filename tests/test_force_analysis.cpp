#include "../include/physicalWorld.h"
#include <iostream>
#include <cmath>
#include <iomanip>

int main() {
    std::cout << "========== 力的详细分析 ==========" << std::endl;
    
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
    
    std::cout << "\n理论分析:" << std::endl;
    double angleRad = angle * 3.14159265 / 180.0;
    double F_parallel = g * sin(angleRad);
    double N = g * cos(angleRad);
    double F_friction = friction * N;
    double F_net = F_parallel - F_friction;
    
    std::cout << "  驱动力: " << F_parallel << " N" << std::endl;
    std::cout << "  正压力: " << N << " N" << std::endl;
    std::cout << "  摩擦力: " << F_friction << " N" << std::endl;
    std::cout << "  净力: " << F_net << " N" << std::endl;
    std::cout << "  理论加速度: " << F_net << " m/s?" << std::endl;
    
    world.start();
    
    // 稳定
    for (int i = 0; i < 5; i++) {
        world.update(shapeList, world.ground);
    }
    
    ball->setVelocity(0.0, 0.0);
    
    std::cout << "\n详细力分析 (前5帧):" << std::endl;
    std::cout << std::setw(6) << "帧"
              << std::setw(12) << "Vx前(m/s)"
              << std::setw(12) << "Fx(N)"
              << std::setw(12) << "ax(m/s?)"
              << std::setw(12) << "ΔVx预期"
              << std::setw(12) << "Vx后(m/s)"
              << std::setw(12) << "ΔVx实际" << std::endl;
    std::cout << std::string(84, '-') << std::endl;
    
    for (int i = 1; i <= 5; i++) {
        double vx_before, vy_before;
        ball->getVelocity(vx_before, vy_before);
        
        // 清空力
        ball->clearTotalForce();
        
        // 施加驱动力
        ball->addToTotalForce(F_parallel, 0.0);
        
        // 施加摩擦力（只有速度不为0时）
        if (fabs(vx_before) > 1e-6) {
            double friction_force = -F_friction * (vx_before / fabs(vx_before));
            ball->addToTotalForce(friction_force, 0.0);
        }
        
        // 获取总力
        double fx, fy;
        ball->getTotalForce(fx, fy);
        double ax = fx / ball->getMass();
        double deltaVx_expected = ax * world.getTimeStep();
        
        // 现在用world.update()实际运行
        ball->setVelocity(vx_before, vy_before);  // 恢复速度
        world.update(shapeList, world.ground);
        
        double vx_after, vy_after;
        ball->getVelocity(vx_after, vy_after);
        double deltaVx_actual = vx_after - vx_before;
        
        std::cout << std::setw(6) << i
                  << std::setw(12) << std::fixed << std::setprecision(6) << vx_before
                  << std::setw(12) << fx
                  << std::setw(12) << ax
                  << std::setw(12) << deltaVx_expected
                  << std::setw(12) << vx_after
                  << std::setw(12) << deltaVx_actual << std::endl;
    }
    
    std::cout << "\n观察：如果ΔVx实际 < ΔVx预期，说明摩擦力被过度施加！" << std::endl;
    
    return 0;
}
