/*=========================================================================================================
 * 摩擦力测试程序
 * 测试物体在地面上滑行时的摩擦力效果
 *=========================================================================================================*/

#include "../include/shapes.h"
#include "../include/physicalWorld.h"
#include <iostream>
#include <iomanip>
#include <cmath>

// 理论计算：匀减速直线运动的滑行距离
// v? = v?? - 2as
// s = v?? / (2a)
// 其中 a = μg
double calculateTheoreticalDistance(double v0, double friction, double g = 9.8) {
    double a = friction * g;  // 摩擦力加速度
    return (v0 * v0) / (2.0 * a);
}

// 理论计算：停止时间
// v = v? - at
// t = v? / a
double calculateTheoreticalTime(double v0, double friction, double g = 9.8) {
    double a = friction * g;
    return v0 / a;
}

void printTestHeader(const std::string& testName) {
    std::cout << "\n";
    std::cout << "========================================" << std::endl;
    std::cout << "  " << testName << std::endl;
    std::cout << "========================================" << std::endl;
}

void printSimulationStep(int step, double time, double x, double vx, double fx) {
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "步数: " << std::setw(4) << step 
              << " | 时间: " << std::setw(6) << time << "s"
              << " | 位置: " << std::setw(8) << x << "m"
              << " | 速度: " << std::setw(7) << vx << "m/s"
              << " | 摩擦力: " << std::setw(7) << fx << "N"
              << std::endl;
}

void printResults(double theoretical, double simulated, const std::string& unit) {
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "理论值: " << std::fixed << std::setprecision(4) << theoretical << " " << unit << std::endl;
    std::cout << "模拟值: " << simulated << " " << unit << std::endl;
    
    double error = std::abs(theoretical - simulated);
    double errorPercent = (error / theoretical) * 100.0;
    
    std::cout << "误  差: " << error << " " << unit 
              << " (" << std::setprecision(2) << errorPercent << "%)" << std::endl;
    
    if (errorPercent < 5.0) {
        std::cout << "? 测试通过！误差在5%以内" << std::endl;
    } else if (errorPercent < 10.0) {
        std::cout << "??  警告：误差在5-10%之间" << std::endl;
    } else {
        std::cout << "? 测试失败！误差超过10%" << std::endl;
    }
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
}

/*=========================================================================================================
 * 测试1：基础摩擦力测试
 * 一个AABB物体在地面上以初速度开始滑行，直到停止
 *=========================================================================================================*/
void test_basic_friction() {
    printTestHeader("测试1：基础摩擦力测试");
    
    // 测试参数
    const double mass = 1.0;           // 质量：1 kg
    const double initialVelocity = 10.0; // 初速度：10 m/s
    const double friction = 0.3;       // 摩擦系数：0.3
    const double dt = 0.01;            // 时间步长：0.01秒
    const double g = 9.8;              // 重力加速度
    
    std::cout << "\n?? 测试参数：" << std::endl;
    std::cout << "  质量 m = " << mass << " kg" << std::endl;
    std::cout << "  初速度 v? = " << initialVelocity << " m/s" << std::endl;
    std::cout << "  摩擦系数 μ = " << friction << std::endl;
    std::cout << "  重力加速度 g = " << g << " m/s?" << std::endl;
    std::cout << "  时间步长 Δt = " << dt << " s" << std::endl;
    
    // 理论计算
    double theoreticalDistance = calculateTheoreticalDistance(initialVelocity, friction, g);
    double theoreticalTime = calculateTheoreticalTime(initialVelocity, friction, g);
    
    std::cout << "\n?? 理论计算：" << std::endl;
    std::cout << "  加速度 a = μg = " << friction * g << " m/s?" << std::endl;
    std::cout << "  滑行距离 s = v??/(2a) = " << theoreticalDistance << " m" << std::endl;
    std::cout << "  停止时间 t = v?/a = " << theoreticalTime << " s" << std::endl;
    
    // 创建物理世界和物体
    PhysicalWorld world;
    world.setGravity(g);
    
    Ground ground(0.0, friction);  // 地面在y=0，摩擦系数0.3
    AABB box(mass, 1.0, 1.0, 0.0, 0.5);  // 1kg，1m×1m，位置(0, 0.5)
    box.setVelocity(initialVelocity, 0.0);  // 初速度10 m/s向右
    
    // 将物体放在地面上
    world.placeShapeOnGround(box, ground);
    
    std::cout << "\n?? 开始模拟..." << std::endl;
    std::cout << "\n" << std::string(80, '-') << std::endl;
    
    double time = 0.0;
    int step = 0;
    double startX, startY;
    box.getCentre(startX, startY);
    
    // 打印初始状态
    double vx, vy;
    box.getVelocity(vx, vy);
    printSimulationStep(step, time, startX, vx, 0.0);
    
    // 模拟循环
    while (box.isMoving() && time < theoreticalTime * 2.0) {
        // 重置支撑状态
        box.resetSupportStatus();
        
        // 清空合力
        box.clearTotalForce();
        
        // 检查是否在地面上
        if (ground.check_collision(box)) {
            box.setIsSupported(true);
            
            // 应用摩擦力
            double normalForce = mass * g;  // 正压力 = 重力
            box.applyFriction(normalForce, friction);
        }
        
        // 应用合力
        box.applyTotalForce(dt);
        
        // 更新位置
        box.update(dt);
        
        // 确保物体在地面上
        world.placeShapeOnGround(box, ground);
        
        time += dt;
        step++;
        
        // 每0.1秒打印一次
        if (step % 10 == 0) {
            double x, y;
            box.getCentre(x, y);
            box.getVelocity(vx, vy);
            
            double fx, fy;
            box.getTotalForce(fx, fy);
            
            printSimulationStep(step, time, x, vx, fx);
        }
    }
    
    // 最终状态
    double finalX, finalY;
    box.getCentre(finalX, finalY);
    box.getVelocity(vx, vy);
    
    std::cout << std::string(80, '-') << std::endl;
    printSimulationStep(step, time, finalX, vx, 0.0);
    
    // 计算实际滑行距离
    double simulatedDistance = finalX - startX;
    
    // 显示结果
    std::cout << "\n?? 结果对比：" << std::endl;
    printResults(theoreticalDistance, simulatedDistance, "m");
    printResults(theoreticalTime, time, "s");
}

/*=========================================================================================================
 * 测试2：不同摩擦系数测试
 * 测试不同摩擦系数下的滑行距离
 *=========================================================================================================*/
void test_different_friction_coefficients() {
    printTestHeader("测试2：不同摩擦系数测试");
    
    const double mass = 1.0;
    const double initialVelocity = 10.0;
    const double g = 9.8;
    const double dt = 0.01;
    
    double frictions[] = {0.1, 0.2, 0.3, 0.4, 0.5};
    int numTests = sizeof(frictions) / sizeof(frictions[0]);
    
    std::cout << "\n测试条件：" << std::endl;
    std::cout << "  质量 = " << mass << " kg" << std::endl;
    std::cout << "  初速度 = " << initialVelocity << " m/s" << std::endl;
    std::cout << "\n" << std::string(100, '=') << std::endl;
    std::cout << std::setw(10) << "摩擦系数" 
              << std::setw(20) << "理论距离(m)" 
              << std::setw(20) << "模拟距离(m)" 
              << std::setw(15) << "误差(%)" 
              << std::setw(10) << "结果" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
    
    for (int i = 0; i < numTests; i++) {
        double friction = frictions[i];
        
        // 理论计算
        double theoreticalDistance = calculateTheoreticalDistance(initialVelocity, friction, g);
        
        // 模拟
        PhysicalWorld world;
        world.setGravity(g);
        
        Ground ground(0.0, friction);
        AABB box(mass, 1.0, 1.0, 0.0, 0.5);
        box.setVelocity(initialVelocity, 0.0);
        
        world.placeShapeOnGround(box, ground);
        
        double startX, startY;
        box.getCentre(startX, startY);
        
        double time = 0.0;
        double maxTime = calculateTheoreticalTime(initialVelocity, friction, g) * 2.0;
        
        while (box.isMoving() && time < maxTime) {
            box.resetSupportStatus();
            box.clearTotalForce();
            
            if (ground.check_collision(box)) {
                box.setIsSupported(true);
                double normalForce = mass * g;
                box.applyFriction(normalForce, friction);
            }
            
            box.applyTotalForce(dt);
            box.update(dt);
            world.placeShapeOnGround(box, ground);
            
            time += dt;
        }
        
        double finalX, finalY;
        box.getCentre(finalX, finalY);
        double simulatedDistance = finalX - startX;
        
        double error = std::abs(theoreticalDistance - simulatedDistance);
        double errorPercent = (error / theoreticalDistance) * 100.0;
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << std::setw(10) << friction
                  << std::setw(20) << theoreticalDistance
                  << std::setw(20) << simulatedDistance
                  << std::setw(15) << errorPercent;
        
        if (errorPercent < 5.0) {
            std::cout << std::setw(10) << "? 通过";
        } else {
            std::cout << std::setw(10) << "? 失败";
        }
        std::cout << std::endl;
    }
    
    std::cout << std::string(100, '=') << std::endl;
}

/*=========================================================================================================
 * 测试3：不同初速度测试
 * 测试不同初速度下的滑行距离
 *=========================================================================================================*/
void test_different_initial_velocities() {
    printTestHeader("测试3：不同初速度测试");
    
    const double mass = 1.0;
    const double friction = 0.3;
    const double g = 9.8;
    const double dt = 0.01;
    
    double velocities[] = {5.0, 10.0, 15.0, 20.0, 25.0};
    int numTests = sizeof(velocities) / sizeof(velocities[0]);
    
    std::cout << "\n测试条件：" << std::endl;
    std::cout << "  质量 = " << mass << " kg" << std::endl;
    std::cout << "  摩擦系数 = " << friction << std::endl;
    std::cout << "\n" << std::string(100, '=') << std::endl;
    std::cout << std::setw(15) << "初速度(m/s)" 
              << std::setw(20) << "理论距离(m)" 
              << std::setw(20) << "模拟距离(m)" 
              << std::setw(15) << "误差(%)" 
              << std::setw(10) << "结果" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
    
    for (int i = 0; i < numTests; i++) {
        double v0 = velocities[i];
        
        // 理论计算
        double theoreticalDistance = calculateTheoreticalDistance(v0, friction, g);
        
        // 模拟
        PhysicalWorld world;
        world.setGravity(g);
        
        Ground ground(0.0, friction);
        AABB box(mass, 1.0, 1.0, 0.0, 0.5);
        box.setVelocity(v0, 0.0);
        
        world.placeShapeOnGround(box, ground);
        
        double startX, startY;
        box.getCentre(startX, startY);
        
        double time = 0.0;
        double maxTime = calculateTheoreticalTime(v0, friction, g) * 2.0;
        
        while (box.isMoving() && time < maxTime) {
            box.resetSupportStatus();
            box.clearTotalForce();
            
            if (ground.check_collision(box)) {
                box.setIsSupported(true);
                double normalForce = mass * g;
                box.applyFriction(normalForce, friction);
            }
            
            box.applyTotalForce(dt);
            box.update(dt);
            world.placeShapeOnGround(box, ground);
            
            time += dt;
        }
        
        double finalX, finalY;
        box.getCentre(finalX, finalY);
        double simulatedDistance = finalX - startX;
        
        double error = std::abs(theoreticalDistance - simulatedDistance);
        double errorPercent = (error / theoreticalDistance) * 100.0;
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << std::setw(15) << v0
                  << std::setw(20) << theoreticalDistance
                  << std::setw(20) << simulatedDistance
                  << std::setw(15) << errorPercent;
        
        if (errorPercent < 5.0) {
            std::cout << std::setw(10) << "? 通过";
        } else {
            std::cout << std::setw(10) << "? 失败";
        }
        std::cout << std::endl;
    }
    
    std::cout << std::string(100, '=') << std::endl;
}

/*=========================================================================================================
 * 主函数
 *=========================================================================================================*/
int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                          ║" << std::endl;
    std::cout << "║       摩 擦 力 物 理 模 拟 测 试         ║" << std::endl;
    std::cout << "║                                          ║" << std::endl;
    std::cout << "║  测试物体在摩擦力作用下的滑行行为        ║" << std::endl;
    std::cout << "║                                          ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════╝" << std::endl;
    
    try {
        // 运行所有测试
        test_basic_friction();
        test_different_friction_coefficients();
        test_different_initial_velocities();
        
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "? 所有测试完成！" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n? 测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
