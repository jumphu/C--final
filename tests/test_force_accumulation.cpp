#include "../include/shapes.h"
#include <iostream>
#include <iomanip>

void print_force_info(const Shape& shape) {
    double x, y, vx, vy, fx, fy;
    shape.getCentre(x, y);
    shape.getVelocity(vx, vy);
    shape.getTotalForce(fx, fy);
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "位置: (" << x << ", " << y << "), "
              << "速度: (" << vx << ", " << vy << "), "
              << "合力: (" << fx << ", " << fy << ")" << std::endl;
}

int main() {
    std::cout << "=== 合力累加测试程序 ===" << std::endl;
    
    // 创建一个圆形物体
    Circle circle(1.0, 5.0, 0.0, 10.0);  // 质量1kg，半径5m，初始位置(0, 10)，初始速度为0
    circle.setVelocity(0.0, 0.0);
    
    std::cout << "\n初始状态:" << std::endl;
    print_force_info(circle);
    
    // 测试1: 累加多个力
    std::cout << "\n=== 测试1: 累加多个力 ===" << std::endl;
    circle.clearTotalForce();  // 确保合力为0
    
    std::cout << "施加力 (10, 0) N..." << std::endl;
    circle.applyForce(10.0, 0.0);
    print_force_info(circle);
    
    std::cout << "再施加力 (0, 5) N..." << std::endl;
    circle.applyForce(0.0, 5.0);
    print_force_info(circle);
    
    std::cout << "再施加力 (-3, 2) N..." << std::endl;
    circle.applyForce(-3.0, 2.0);
    print_force_info(circle);
    
    // 测试2: 应用累加的合力
    std::cout << "\n=== 测试2: 应用累加的合力 ===" << std::endl;
    double deltaTime = 1.0;  // 时间步长1秒
    std::cout << "应用合力 (deltaTime = " << deltaTime << " s)..." << std::endl;
    circle.applyTotalForce(deltaTime);
    print_force_info(circle);
    std::cout << "注意: 合力应用后自动清零" << std::endl;
    
    // 测试3: 重力累加
    std::cout << "\n=== 测试3: 重力累加 ===" << std::endl;
    circle.clearTotalForce();
    circle.setVelocity(0.0, 0.0);  // 重置速度
    
    std::cout << "初始状态:" << std::endl;
    print_force_info(circle);
    
    std::cout << "应用重力 (g = 9.8 m/s?)..." << std::endl;
    circle.applyGravity(9.8);
    print_force_info(circle);
    
    std::cout << "再施加一个向上的力 (0, 15) N..." << std::endl;
    circle.applyForce(0.0, 15.0);
    print_force_info(circle);
    
    std::cout << "应用合力 (deltaTime = " << deltaTime << " s)..." << std::endl;
    circle.applyTotalForce(deltaTime);
    print_force_info(circle);
    
    // 测试4: 模拟多时间步长
    std::cout << "\n=== 测试4: 模拟物理更新（自由落体） ===" << std::endl;
    Circle ball(2.0, 1.0, 0.0, 50.0);  // 质量2kg，半径1m，位置(0, 50)
    ball.setVelocity(0.0, 0.0);
    
    deltaTime = 0.1;  // 0.1秒时间步长
    std::cout << "时间步长: " << deltaTime << " s" << std::endl;
    std::cout << "\n时间\t高度\t速度Y\t合力Y" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    for (int i = 0; i <= 10; ++i) {
        double x, y, vx, vy, fx, fy;
        ball.getCentre(x, y);
        ball.getVelocity(vx, vy);
        ball.getTotalForce(fx, fy);
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << (i * deltaTime) << "\t" << y << "\t" << vy << "\t" << fy << std::endl;
        
        // 每个时间步
        ball.clearTotalForce();           // 清空上一步的力
        ball.applyGravity(9.8);          // 累加重力
        ball.applyTotalForce(deltaTime);  // 应用力到速度
        ball.update(deltaTime);           // 更新位置
    }
    
    // 测试5: 使用 addToTotalForce 方法
    std::cout << "\n=== 测试5: 使用 addToTotalForce 方法 ===" << std::endl;
    Circle obj(1.0, 1.0, 0.0, 0.0);
    obj.setVelocity(0.0, 0.0);
    obj.clearTotalForce();
    
    std::cout << "直接添加力到累加器:" << std::endl;
    obj.addToTotalForce(5.0, 3.0);
    print_force_info(obj);
    
    obj.addToTotalForce(-2.0, 1.0);
    print_force_info(obj);
    
    std::cout << "\n应用合力并更新位置 (deltaTime = 1.0 s)..." << std::endl;
    obj.applyTotalForce(1.0);
    obj.update(1.0);
    print_force_info(obj);
    
    std::cout << "\n=== 测试完成 ===" << std::endl;
    return 0;
}
