#include "../include/shapes.h"
#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    std::cout << "=== 水平摩擦力测试 - 理论 vs 模拟 ===" << std::endl;
    
    // 测试参数
    double mass = 1.0;              // 质量 1 kg
    double initialVelocity = 10.0;  // 初始速度 10 m/s
    double mu = 0.3;                // 摩擦系数 0.3
    double g = 9.8;                 // 重力加速度 9.8 m/s?
    double deltaTime = 0.1;         // 时间步长 0.1 s
    
    // 理论计算
    double a_theory = -mu * g;      // 理论加速度 a = -μg
    double frictionForce = mu * mass * g;  // 摩擦力大小 f = μmg
    double stopTime_theory = initialVelocity / (mu * g);  // 停止时间 t = v?/(μg)
    double stopDistance_theory = (initialVelocity * initialVelocity) / (2 * mu * g);  // 停止距离 s = v??/(2μg)
    
    std::cout << "\n【理论计算】" << std::endl;
    std::cout << "质量: " << mass << " kg" << std::endl;
    std::cout << "初始速度: " << initialVelocity << " m/s" << std::endl;
    std::cout << "摩擦系数: " << mu << std::endl;
    std::cout << "重力加速度: " << g << " m/s?" << std::endl;
    std::cout << "时间步长: " << deltaTime << " s" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "理论加速度: " << a_theory << " m/s?" << std::endl;
    std::cout << "摩擦力大小: " << frictionForce << " N" << std::endl;
    std::cout << "理论停止时间: " << stopTime_theory << " s" << std::endl;
    std::cout << "理论停止距离: " << stopDistance_theory << " m" << std::endl;
    
    // 模拟测试
    std::cout << "\n【模拟测试 - 显式欧拉法】" << std::endl;
    Circle ball(mass, 1.0, 0.0, 0.0);
    ball.setVelocity(initialVelocity, 0.0);
    ball.setFraction(mu);
    
    double normalForce = mass * g;
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\n时间(s)\t位置(m)\t速度(m/s)\t理论速度\t速度误差(%)\t理论位置\t位置误差(%)" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
    
    double simulationStopTime = 0.0;
    double simulationStopDistance = 0.0;
    double maxVelocityError = 0.0;
    double maxPositionError = 0.0;
    
    for (int i = 0; i <= 40; ++i) {
        double currentTime = i * deltaTime;
        
        // 获取模拟值
        double x, y, vx, vy;
        ball.getCentre(x, y);
        ball.getVelocity(vx, vy);
        
        // 计算理论值
        double v_theory = std::max(0.0, initialVelocity + a_theory * currentTime);
        double x_theory;
        
        if (currentTime < stopTime_theory) {
            // 还在运动：s = v?t + ?at?
            x_theory = initialVelocity * currentTime + 0.5 * a_theory * currentTime * currentTime;
        } else {
            // 已停止：保持停止距离
            x_theory = stopDistance_theory;
        }
        
        // 计算误差
        double velocity_error = 0.0;
        double position_error = 0.0;
        
        if (v_theory > 0.01) {
            velocity_error = std::abs(vx - v_theory) / v_theory * 100.0;
            maxVelocityError = std::max(maxVelocityError, velocity_error);
        }
        
        if (x_theory > 0.01) {
            position_error = std::abs(x - x_theory) / x_theory * 100.0;
            maxPositionError = std::max(maxPositionError, position_error);
        }
        
        // 输出结果
        std::cout << currentTime << "\t"
                  << x << "\t"
                  << vx << "\t\t"
                  << v_theory << "\t\t"
                  << velocity_error << "\t\t"
                  << x_theory << "\t\t"
                  << position_error << std::endl;
        
        // 物理更新
        ball.clearTotalForce();
        ball.applyFriction(normalForce, mu);
        ball.applyTotalForce(deltaTime);
        ball.update(deltaTime);
        
        // 记录停止时间和距离
        if (std::abs(vx) < 0.01 && simulationStopTime == 0.0) {
            simulationStopTime = currentTime;
            simulationStopDistance = x;
        }
        
        // 如果速度接近零，再多运行几步验证停止状态
        if (std::abs(vx) < 0.001 && i > 30) {
            break;
        }
    }
    
    // 结果对比
    std::cout << "\n【结果对比】" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    std::cout << std::setw(30) << "项目" << std::setw(15) << "理论值" << std::setw(15) << "模拟值" << std::setw(10) << "误差(%)" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    std::cout << std::setw(30) << "停止时间 (s)" 
              << std::setw(15) << stopTime_theory 
              << std::setw(15) << simulationStopTime 
              << std::setw(10) << (std::abs(simulationStopTime - stopTime_theory) / stopTime_theory * 100.0) << std::endl;
    std::cout << std::setw(30) << "停止距离 (m)" 
              << std::setw(15) << stopDistance_theory 
              << std::setw(15) << simulationStopDistance 
              << std::setw(10) << (std::abs(simulationStopDistance - stopDistance_theory) / stopDistance_theory * 100.0) << std::endl;
    std::cout << std::setw(30) << "最大速度误差 (%)" 
              << std::setw(15) << "-" 
              << std::setw(15) << "-"
              << std::setw(10) << maxVelocityError << std::endl;
    std::cout << std::setw(30) << "最大位置误差 (%)" 
              << std::setw(15) << "-" 
              << std::setw(15) << "-"
              << std::setw(10) << maxPositionError << std::endl;
    
    // 验证结论
    std::cout << "\n【验证结论】" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    double time_error = std::abs(simulationStopTime - stopTime_theory) / stopTime_theory * 100.0;
    double distance_error = std::abs(simulationStopDistance - stopDistance_theory) / stopDistance_theory * 100.0;
    
    bool time_passed = time_error < 1.0;  // 时间误差小于1%
    bool distance_passed = distance_error < 5.0;  // 距离误差小于5%（考虑数值误差）
    
    std::cout << "停止时间验证: " << (time_passed ? "? 通过" : "? 失败") 
              << " (误差: " << time_error << "%, 阈值: 1%)" << std::endl;
    std::cout << "停止距离验证: " << (distance_passed ? "? 通过" : "? 失败") 
              << " (误差: " << distance_error << "%, 阈值: 5%)" << std::endl;
    
    if (time_passed && distance_passed) {
        std::cout << "\n??? 模拟结果与理论计算吻合良好！" << std::endl;
    } else {
        std::cout << "\n??? 模拟结果与理论计算存在显著偏差！" << std::endl;
    }
    
    // 误差分析
    std::cout << "\n【误差分析】" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "数值积分方法: 显式欧拉法 (Forward Euler)" << std::endl;
    std::cout << "  v(t+Δt) = v(t) + a·Δt" << std::endl;
    std::cout << "  x(t+Δt) = x(t) + v(t)·Δt" << std::endl;
    std::cout << "\n误差来源:" << std::endl;
    std::cout << "1. 欧拉法是一阶精度方法，局部截断误差 O(Δt?)" << std::endl;
    std::cout << "2. 全局误差累积为 O(Δt)" << std::endl;
    std::cout << "3. 位置更新使用当前速度，而非平均速度" << std::endl;
    std::cout << "\n改进方法:" << std::endl;
    std::cout << "1. 减小时间步长 Δt" << std::endl;
    std::cout << "2. 使用中点法或 Verlet 积分" << std::endl;
    std::cout << "3. 使用 Runge-Kutta 方法 (RK4)" << std::endl;
    
    // 理论误差估算
    double theoretical_error_rate = (a_theory * deltaTime * deltaTime) / (2 * stopDistance_theory) * 100;
    std::cout << "\n理论误差估计: ~" << std::abs(theoretical_error_rate * stopTime_theory / deltaTime) 
              << "% (基于局部截断误差累积)" << std::endl;
    std::cout << "实际观测误差: " << distance_error << "%" << std::endl;
    
    // 物理公式总结
    std::cout << "\n【物理公式】" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "摩擦力:     f = μN = μmg" << std::endl;
    std::cout << "加速度:     a = -f/m = -μg" << std::endl;
    std::cout << "速度:       v(t) = v? + at = v? - μgt" << std::endl;
    std::cout << "位移:       s(t) = v?t + ?at? = v?t - ?μgt?" << std::endl;
    std::cout << "停止时间:   t = v?/(μg)" << std::endl;
    std::cout << "停止距离:   s = v??/(2μg)" << std::endl;
    
    std::cout << "\n=== 测试完成 ===" << std::endl;
    return 0;
}
