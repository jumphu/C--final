#include "../include/physicalWorld.h"
#include <iostream>
#include <cmath>
#include <iomanip>

/*=========================================================================================================
 * 详细诊断测试 - 打印每一帧的力、加速度、速度
 *=========================================================================================================*/

int main() {
    std::cout << "========== 摩擦力详细诊断 ==========" << std::endl;
    
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
    double F_parallel = 1.0 * g * sin(angleRad);  // 驱动力
    double N = 1.0 * g * cos(angleRad);            // 正压力
    double F_friction = friction * N;              // 摩擦力大小
    double F_net = F_parallel - F_friction;        // 净力
    double a_theory = F_net;                       // 加速度 (m=1)
    
    std::cout << "  驱动力 F_parallel = m*g*sin(θ) = " << F_parallel << " N" << std::endl;
    std::cout << "  正压力 N = m*g*cos(θ) = " << N << " N" << std::endl;
    std::cout << "  摩擦力 F_friction = μ*N = " << F_friction << " N" << std::endl;
    std::cout << "  净力 F_net = " << F_net << " N" << std::endl;
    std::cout << "  理论加速度 a = " << a_theory << " m/s?" << std::endl;
    
    world.start();
    
    // 先稳定
    for (int i = 0; i < 5; i++) {
        world.update(shapeList, world.ground);
    }
    
    ball->setVelocity(0.0, 0.0);  // 重置速度
    
    std::cout << "\n逐帧分析 (前10帧):" << std::endl;
    std::cout << std::setw(6) << "帧"
              << std::setw(12) << "Vx前(m/s)"
              << std::setw(12) << "Fx(N)"
              << std::setw(12) << "Fy(N)"
              << std::setw(12) << "ax(m/s?)"
              << std::setw(12) << "Vx后(m/s)"
              << std::setw(10) << "支撑" << std::endl;
    std::cout << std::string(76, '-') << std::endl;
    
    for (int i = 1; i <= 10; i++) {
        double vx_before, vy_before;
        ball->getVelocity(vx_before, vy_before);
        
        // 清空力
        ball->clearTotalForce();
        
        // 手动施加力（模拟 applyGravity 的逻辑）
        // 1. 沿斜面的驱动力
        ball->addToTotalForce(F_parallel, 0.0);
        
        // 2. 摩擦力（只有速度不为0时）
        if (fabs(vx_before) > 1e-6) {
            double friction_force = -F_friction * (vx_before / fabs(vx_before));  // 方向与速度相反
            ball->addToTotalForce(friction_force, 0.0);
        }
        
        // 获取总力
        double fx, fy;
        ball->getTotalForce(fx, fy);
        
        // 计算加速度
        double ax = fx / ball->getMass();
        
        // 应用力
        ball->applyTotalForce(world.getTimeStep());
        
        double vx_after, vy_after;
        ball->getVelocity(vx_after, vy_after);
        
        // 更新位置
        ball->update(world.getTimeStep());
        
        std::cout << std::setw(6) << i
                  << std::setw(12) << std::fixed << std::setprecision(6) << vx_before
                  << std::setw(12) << fx
                  << std::setw(12) << fy
                  << std::setw(12) << ax
                  << std::setw(12) << vx_after
                  << std::setw(10) << (ball->getIsSupported() ? "是" : "否") << std::endl;
    }
    
    std::cout << "\n使用world.update()运行10帧（详细输出）:" << std::endl;
    ball->setVelocity(0.0, 0.0);
    
    std::cout << std::setw(6) << "帧"
              << std::setw(12) << "Vx前(m/s)"
              << std::setw(12) << "Vx后(m/s)"
              << std::setw(10) << "支撑" << std::endl;
    std::cout << std::string(46, '-') << std::endl;
    
    for (int i = 1; i <= 10; i++) {
        double vx_before, vy_before;
        ball->getVelocity(vx_before, vy_before);
        
        world.update(shapeList, world.ground);
        
        double vx_after, vy_after;
        ball->getVelocity(vx_after, vy_after);
        
        std::cout << std::setw(6) << i
                  << std::setw(12) << std::fixed << std::setprecision(6) << vx_before
                  << std::setw(12) << vx_after
                  << std::setw(10) << (ball->getIsSupported() ? "是" : "否") << std::endl;
    }
    
    std::cout << "\n使用world.update()运行60帧:" << std::endl;
    ball->setVelocity(0.0, 0.0);
    
    for (int i = 0; i < 60; i++) {
        world.update(shapeList, world.ground);
    }
    
    double vx_final, vy_final;
    ball->getVelocity(vx_final, vy_final);
    
    std::cout << "  最终速度: vx = " << vx_final << " m/s" << std::endl;
    std::cout << "  理论速度: v = a*t = " << a_theory << " * 1.0 = " << a_theory << " m/s" << std::endl;
    std::cout << "  误差: " << fabs(vx_final - a_theory) / a_theory * 100.0 << "%" << std::endl;
    
    return 0;
}
