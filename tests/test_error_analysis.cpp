#include "../include/shapes.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

struct TestResult {
    double deltaTime;
    double stopTime;
    double stopDistance;
    double timeError;
    double distanceError;
};

TestResult runTest(double deltaTime) {
    // 参数
    double mass = 1.0;
    double initialVelocity = 10.0;
    double mu = 0.3;
    double g = 9.8;
    
    // 理论值
    double stopTime_theory = initialVelocity / (mu * g);
    double stopDistance_theory = (initialVelocity * initialVelocity) / (2 * mu * g);
    
    // 模拟
    Circle ball(mass, 1.0, 0.0, 0.0);
    ball.setVelocity(initialVelocity, 0.0);
    
    double normalForce = mass * g;
    double currentTime = 0.0;
    double simulationStopTime = 0.0;
    double simulationStopDistance = 0.0;
    
    for (int i = 0; i < 10000; ++i) {
        currentTime = i * deltaTime;
        
        double x, y, vx, vy;
        ball.getCentre(x, y);
        ball.getVelocity(vx, vy);
        
        if (std::abs(vx) < 0.01 && simulationStopTime == 0.0) {
            simulationStopTime = currentTime;
            simulationStopDistance = x;
            break;
        }
        
        ball.clearTotalForce();
        ball.applyFriction(normalForce, mu);
        ball.applyTotalForce(deltaTime);
        ball.update(deltaTime);
    }
    
    TestResult result;
    result.deltaTime = deltaTime;
    result.stopTime = simulationStopTime;
    result.stopDistance = simulationStopDistance;
    result.timeError = std::abs(simulationStopTime - stopTime_theory) / stopTime_theory * 100.0;
    result.distanceError = std::abs(simulationStopDistance - stopDistance_theory) / stopDistance_theory * 100.0;
    
    return result;
}

int main() {
    std::cout << "=== 时间步长对误差的影响 ===" << std::endl;
    
    // 理论值
    double v0 = 10.0;
    double mu = 0.3;
    double g = 9.8;
    double stopTime_theory = v0 / (mu * g);
    double stopDistance_theory = v0 * v0 / (2 * mu * g);
    
    std::cout << "\n【理论值】" << std::endl;
    std::cout << "停止时间: " << stopTime_theory << " s" << std::endl;
    std::cout << "停止距离: " << stopDistance_theory << " m" << std::endl;
    
    // 测试不同的时间步长
    std::vector<double> timeSteps = {
        0.5, 0.2, 0.1, 0.05, 0.02, 0.01, 0.005, 0.001
    };
    
    std::vector<TestResult> results;
    
    std::cout << "\n【不同时间步长的测试结果】" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Δt(s)\t\t停止距离(m)\t距离误差(%)\t理论误差(%)\t误差比" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    double firstError = 0.0;
    
    for (double dt : timeSteps) {
        TestResult result = runTest(dt);
        results.push_back(result);
        
        // 理论误差估算：?a·t·Δt / s_theory
        double a = -mu * g;
        double theoretical_error = std::abs(0.5 * a * stopTime_theory * dt) / stopDistance_theory * 100.0;
        
        if (firstError == 0.0) {
            firstError = result.distanceError;
        }
        
        double errorRatio = firstError / result.distanceError;
        
        std::cout << dt << "\t\t"
                  << result.stopDistance << "\t\t"
                  << result.distanceError << "\t\t"
                  << theoretical_error << "\t\t"
                  << errorRatio << std::endl;
    }
    
    // 验证线性关系
    std::cout << "\n【误差与时间步长的关系】" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "当 Δt 减半时，误差应该也减半（线性关系）" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (size_t i = 1; i < results.size(); ++i) {
        double ratio_dt = results[i-1].deltaTime / results[i].deltaTime;
        double ratio_error = results[i-1].distanceError / results[i].distanceError;
        
        std::cout << "Δt 比值: " << ratio_dt 
                  << " → 误差比值: " << ratio_error 
                  << " (期望: " << ratio_dt << ")" << std::endl;
    }
    
    // 速度 vs 位置误差对比
    std::cout << "\n【速度误差 vs 位置误差】" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    double dt_test = 0.1;
    Circle testBall(1.0, 1.0, 0.0, 0.0);
    testBall.setVelocity(10.0, 0.0);
    
    double normalForce = 1.0 * g;
    
    std::cout << "\nΔt = " << dt_test << " s" << std::endl;
    std::cout << "\n时间(s)\t速度误差(%)\t位置误差(%)\t误差比(位置/速度)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (int i = 0; i <= 30; i += 5) {
        double t = i * dt_test;
        
        // 理论值
        double v_theory = std::max(0.0, 10.0 - mu * g * t);
        double x_theory;
        if (t < stopTime_theory) {
            x_theory = 10.0 * t - 0.5 * mu * g * t * t;
        } else {
            x_theory = stopDistance_theory;
        }
        
        // 模拟值
        double x, y, vx, vy;
        testBall.getCentre(x, y);
        testBall.getVelocity(vx, vy);
        
        // 误差
        double v_error = (v_theory > 0.01) ? std::abs(vx - v_theory) / v_theory * 100.0 : 0.0;
        double x_error = (x_theory > 0.01) ? std::abs(x - x_theory) / x_theory * 100.0 : 0.0;
        double error_ratio = (v_error > 0.001) ? x_error / v_error : 0.0;
        
        std::cout << t << "\t"
                  << v_error << "\t\t"
                  << x_error << "\t\t"
                  << (v_error > 0.001 ? std::to_string(error_ratio) : "∞ (速度完美)") << std::endl;
        
        // 更新
        testBall.clearTotalForce();
        testBall.applyFriction(normalForce, mu);
        testBall.applyTotalForce(dt_test);
        testBall.update(dt_test);
    }
    
    // 关键洞察
    std::cout << "\n【关键洞察】" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "\n1. 速度误差 = 0% (对于恒定加速度)" << std::endl;
    std::cout << "   原因: v(t+Δt) = v(t) + a·Δt 在恒定加速度下是精确的" << std::endl;
    
    std::cout << "\n2. 位置误差 ∝ Δt (线性关系)" << std::endl;
    std::cout << "   原因: 每步缺失 ?a·Δt?，累积 n 步得到 ?a·t·Δt" << std::endl;
    
    std::cout << "\n3. 位置误差 >> 速度误差" << std::endl;
    std::cout << "   原因: 位置是速度的积分，误差会累积" << std::endl;
    
    std::cout << "\n4. 改进方法:" << std::endl;
    std::cout << "   - 减小 Δt: 简单但计算量增加" << std::endl;
    std::cout << "   - 使用中点法: x += v_mid·Δt, 其中 v_mid = v + ?a·Δt" << std::endl;
    std::cout << "   - 使用 Verlet: x += v·Δt + ?a·Δt?" << std::endl;
    std::cout << "   - 使用 RK4: 四阶精度，误差 O(Δt?)" << std::endl;
    
    // 数学公式
    std::cout << "\n【数学解释】" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "\n欧拉法位置误差推导:" << std::endl;
    std::cout << "  x_exact = x? + v?·t + ?a·t?" << std::endl;
    std::cout << "  x_euler = x? + v?·t + ?a·t? - ?a·t·Δt" << std::endl;
    std::cout << "  误差 = ?a·t·Δt" << std::endl;
    std::cout << "\n对于我们的例子:" << std::endl;
    std::cout << "  a = -2.94 m/s?, t = 3.4 s, Δt = 0.1 s" << std::endl;
    std::cout << "  误差 = ? × (-2.94) × 3.4 × 0.1 = -0.4998 m" << std::endl;
    std::cout << "  相对误差 = 0.4998 / 17.0068 × 100% = 2.94%" << std::endl;
    
    std::cout << "\n=== 测试完成 ===" << std::endl;
    return 0;
}
