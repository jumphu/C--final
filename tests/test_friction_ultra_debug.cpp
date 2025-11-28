/*=========================================================================================================
 * 超级详细的摩擦力调试 - 打印每一步的计算
 *=========================================================================================================*/

#include "../include/physicalWorld.h"
#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    std::cout << "========== 超详细摩擦力调试 ==========" << std::endl;
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);
    world.setTimeStep(1.0 / 60.0);
    world.ground.setYLevel(0.0);
    world.ground.setFriction(0.3);
    
    Circle ball(1.0, 5.0, 0.0, 5.0);
    ball.setVelocity(10.0, 0.0);
    ball.setRestitution(0.0);
    ball.setName("Ball");
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(&ball);
    
    world.start();
    
    // 稳定化
    for (int i = 0; i < 30; i++) {
        world.update(shapeList, world.ground);
    }
    
    // 重置速度
    ball.setVelocity(10.0, 0.0);
    
    std::cout << "\n开始详细跟踪第1帧的计算：\n" << std::endl;
    
    // 手动执行一帧的计算，打印每一步
    double deltaTime = world.getTimeStep();
    std::cout << "deltaTime = " << deltaTime << " s" << std::endl;
    
    // 获取初始状态
    double x, y, vx, vy;
    ball.getCentre(x, y);
    ball.getVelocity(vx, vy);
    std::cout << "初始位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "初始速度: (" << vx << ", " << vy << ")" << std::endl;
    
    // 模拟物理世界的更新过程
    // 1. 清空力
    ball.clearTotalForce();
    double fx, fy;
    ball.getTotalForce(fx, fy);
    std::cout << "\n1. 清空力后: totalForce = (" << fx << ", " << fy << ")" << std::endl;
    
    // 2. 施加重力sin分量（horizontal时为0）
    const double PI = 3.14159265358979323846;
    double angleRad = 0.0 * PI / 180.0;
    double gravityForce = ball.getMass() * world.getGravity() * std::sin(angleRad);
    ball.addToTotalForce(gravityForce, 0.0);
    ball.getTotalForce(fx, fy);
    std::cout << "2. 施加重力sin分量后: gravityForce = " << gravityForce << ", totalForce = (" << fx << ", " << fy << ")" << std::endl;
    
    // 3. 计算正压力
    double normalForce = ball.getMass() * world.getGravity();
    std::cout << "3. 正压力: N = m * g = " << ball.getMass() << " * " << world.getGravity() << " = " << normalForce << " N" << std::endl;
    
    // 3.5 检查物体的 normalforce[1] 值
    double nfx, nfy;
    ball.getNormalForce(nfx, nfy);
    std::cout << "3.5 物体的 normalforce 数组: (" << nfx << ", " << nfy << ")" << std::endl;
    double actualNormalForce = ball.getMass() * world.getGravity() + std::abs(nfy);
    std::cout << "3.6 实际使用的正压力: N = m*g + |normalforce[1]| = " << actualNormalForce << " N" << std::endl;
    
    // 4. 计算摩擦力
    double frictionCoeff = world.ground.getFriction();
    double frictionMag = frictionCoeff * normalForce;
    std::cout << "4. 摩擦力大小: f = μ * N = " << frictionCoeff << " * " << normalForce << " = " << frictionMag << " N" << std::endl;
    
    // 5. 摩擦力方向
    double speed = std::sqrt(vx * vx + vy * vy);
    double dirX = -vx / speed;
    std::cout << "5. 速度大小: " << speed << " m/s, 摩擦力方向: " << dirX << std::endl;
    
    // 6. 施加摩擦力
    double frictionX = frictionMag * dirX;
    ball.addToTotalForce(frictionX, 0.0);
    ball.getTotalForce(fx, fy);
    std::cout << "6. 施加摩擦力后: frictionForce = " << frictionX << ", totalForce = (" << fx << ", " << fy << ")" << std::endl;
    
    // 7. 计算加速度
    double ax = fx / ball.getMass();
    std::cout << "7. 加速度: a = F/m = " << fx << " / " << ball.getMass() << " = " << ax << " m/s?" << std::endl;
    
    // 8. 计算速度变化
    double deltaVx = ax * deltaTime;
    std::cout << "8. 速度变化: Δv = a * dt = " << ax << " * " << deltaTime << " = " << deltaVx << " m/s" << std::endl;
    
    // 9. 计算新速度
    double newVx = vx + deltaVx;
    std::cout << "9. 新速度: v_new = v_old + Δv = " << vx << " + " << deltaVx << " = " << newVx << " m/s" << std::endl;
    
    // 现在执行实际的更新
    std::cout << "\n执行实际更新：" << std::endl;
    ball.setVelocity(10.0, 0.0);  // 重置
    world.update(shapeList, world.ground);
    ball.getVelocity(vx, vy);
    std::cout << "实际新速度: (" << vx << ", " << vy << ")" << std::endl;
    
    std::cout << "\n预期速度变化: -0.05 m/s" << std::endl;
    std::cout << "实际速度变化: " << (vx - 10.0) << " m/s" << std::endl;
    std::cout << "倍数差异: " << (vx - 10.0) / (-0.05) << "x" << std::endl;
    
    return 0;
}
