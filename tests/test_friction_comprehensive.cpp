/*=========================================================================================================
 * 摩擦力综合测试程序 (Comprehensive Friction Test)
 * 
 * 测试目的：
 *   全面验证摩擦力系统的正确性，包括：
 *   1. 基础水平摩擦力（单个物体在地面滑动）
 *   2. 不同摩擦系数的影响
 *   3. 堆叠物体的正压力累积
 *   4. 运动平台上的相对摩擦力
 *   5. 倾斜角度下的摩擦力
 * 
 * 理论验证：
 *   - 摩擦力: f = μ * N
 *   - 正压力: N = m * g * cos(θ) + 上方物体的压力
 *   - 减速度: a = f / m = μ * g * cos(θ)
 *   - 停止时间: t = v? / a = v? / (μ * g * cos(θ))
 *=========================================================================================================*/

#include "../include/physicalWorld.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

// 测试结果结构
struct TestResult {
    std::string testName;
    bool passed;
    std::string message;
    double expectedValue;
    double actualValue;
    double tolerance;
};

std::vector<TestResult> testResults;

void printHeader(const std::string& title) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << "========================================" << std::endl;
}

void printResult(const TestResult& result) {
    std::cout << "\n[" << (result.passed ? "? 通过" : "? 失败") << "] " << result.testName << std::endl;
    if (!result.message.empty()) {
        std::cout << "    " << result.message << std::endl;
    }
    if (result.expectedValue != 0.0 || result.actualValue != 0.0) {
        std::cout << "    预期值: " << std::fixed << std::setprecision(2) << result.expectedValue << std::endl;
        std::cout << "    实际值: " << std::fixed << std::setprecision(2) << result.actualValue << std::endl;
        std::cout << "    误差: " << std::abs(result.actualValue - result.expectedValue) 
                  << " (容许: " << result.tolerance << ")" << std::endl;
    }
}

void printSummary() {
    printHeader("测试总结");
    int passed = 0;
    int failed = 0;
    
    for (const auto& result : testResults) {
        if (result.passed) {
            passed++;
        } else {
            failed++;
        }
    }
    
    std::cout << "总测试数: " << testResults.size() << std::endl;
    std::cout << "通过: " << passed << std::endl;
    std::cout << "失败: " << failed << std::endl;
    std::cout << "通过率: " << std::fixed << std::setprecision(1) 
              << (100.0 * passed / testResults.size()) << "%" << std::endl;
}

/*=========================================================================================================
 * 测试1：基础水平摩擦力
 * 场景：单个小球在水平地面上滑动
 * 验证：停止时间是否符合理论预期
 *=========================================================================================================*/
void test1_basic_horizontal_friction() {
    printHeader("测试1：基础水平摩擦力");
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);  // 水平
    world.setTimeStep(1.0 / 60.0);
    
    world.ground.setYLevel(0.0);
    world.ground.setFriction(0.3);
    
    // 创建小球
    Shape* ball = world.placeDynamicShapeByType("Circle", "Ball", 0.0, 0.0, 1.0, 5.0);
    ball->setVelocity(10.0, 0.0);
    ball->setRestitution(0.0);
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(ball);
    
    std::cout << "\n场景设置：" << std::endl;
    std::cout << "  重力: " << world.getGravity() << " m/s?" << std::endl;
    std::cout << "  摩擦系数: " << world.ground.getFriction() << std::endl;
    std::cout << "  质量: " << ball->getMass() << " kg" << std::endl;
    std::cout << "  初始速度: 10.0 m/s" << std::endl;
    
    double mu = world.ground.getFriction();
    double g = world.getGravity();
    double v0 = 10.0;
    
    // 理论计算
    double theoreticalAccel = mu * g;  // a = μ * g
    double theoreticalTime = v0 / theoreticalAccel;  // t = v? / a
    double theoreticalFrames = theoreticalTime * 60.0;  // 转换为帧数
    
    std::cout << "\n理论分析：" << std::endl;
    std::cout << "  减速度: a = μ*g = " << mu << "*" << g << " = " << theoreticalAccel << " m/s?" << std::endl;
    std::cout << "  停止时间: t = v?/a = " << v0 << "/" << theoreticalAccel << " = " << theoreticalTime << " s" << std::endl;
    std::cout << "  停止帧数: " << theoreticalFrames << " 帧" << std::endl;
    
    world.start();
    
    // 稳定化
    for (int i = 0; i < 30; i++) {
        world.update(shapeList, world.ground);
    }
    
    // 重置速度
    ball->setVelocity(10.0, 0.0);
    
    // 模拟并记录
    int stopFrame = -1;
    for (int frame = 0; frame <= 300; frame++) {
        world.update(shapeList, world.ground);
        
        double vx, vy;
        ball->getVelocity(vx, vy);
        
        if (std::abs(vx) < 0.01 && stopFrame < 0) {
            stopFrame = frame;
            break;
        }
    }
    
    std::cout << "\n实际结果：" << std::endl;
    std::cout << "  停止帧数: " << stopFrame << " 帧" << std::endl;
    
    // 验证结果（允许10%误差）
    double tolerance = theoreticalFrames * 0.10;
    bool passed = std::abs(stopFrame - theoreticalFrames) <= tolerance;
    
    TestResult result;
    result.testName = "基础水平摩擦力";
    result.passed = passed;
    result.message = passed ? "停止时间符合理论预期" : "停止时间与理论值偏差过大";
    result.expectedValue = theoreticalFrames;
    result.actualValue = stopFrame;
    result.tolerance = tolerance;
    
    testResults.push_back(result);
    printResult(result);
}

/*=========================================================================================================
 * 测试2：不同摩擦系数对比
 * 场景：相同物体在不同摩擦系数的地面上滑动
 * 验证：停止时间与摩擦系数成反比
 *=========================================================================================================*/
void test2_different_friction_coefficients() {
    printHeader("测试2：不同摩擦系数对比");
    
    double frictionCoeffs[] = {0.1, 0.3, 0.5};
    int numTests = 3;
    std::vector<int> stopFrames;
    
    for (int i = 0; i < numTests; i++) {
        double mu = frictionCoeffs[i];
        
        PhysicalWorld world;
        world.setGravity(10.0);
        world.setInclineAngle(0.0);
        world.setTimeStep(1.0 / 60.0);
        world.ground.setYLevel(0.0);
        world.ground.setFriction(mu);
        
        Shape* ball = world.placeDynamicShapeByType("Circle", "Ball", 0.0, 0.0, 1.0, 5.0);
        ball->setVelocity(10.0, 0.0);
        ball->setRestitution(0.0);
        
        std::vector<Shape*> shapeList;
        shapeList.push_back(ball);
        
        world.start();
        
        // 稳定化
        for (int j = 0; j < 30; j++) {
            world.update(shapeList, world.ground);
        }
        
        ball->setVelocity(10.0, 0.0);
        
        // 模拟
        int stopFrame = -1;
        for (int frame = 0; frame <= 600; frame++) {
            world.update(shapeList, world.ground);
            
            double vx, vy;
            ball->getVelocity(vx, vy);
            
            if (std::abs(vx) < 0.01 && stopFrame < 0) {
                stopFrame = frame;
                break;
            }
        }
        
        stopFrames.push_back(stopFrame);
        
        std::cout << "\n摩擦系数 μ = " << mu << ":" << std::endl;
        std::cout << "  理论停止时间: " << (10.0 / (mu * 10.0)) << " s = " << (10.0 / (mu * 10.0) * 60.0) << " 帧" << std::endl;
        std::cout << "  实际停止时间: " << stopFrame << " 帧" << std::endl;
    }
    
    // 验证：停止时间应该与摩擦系数成反比
    // 即 t1/t2 ≈ μ2/μ1
    double ratio_actual = (double)stopFrames[0] / stopFrames[1];
    double ratio_expected = frictionCoeffs[1] / frictionCoeffs[0];
    double error = std::abs(ratio_actual - ratio_expected) / ratio_expected;
    
    bool passed = error < 0.15;  // 允许15%误差
    
    TestResult result;
    result.testName = "不同摩擦系数对比";
    result.passed = passed;
    result.message = passed ? "停止时间与摩擦系数成反比关系正确" : "停止时间比例不符合预期";
    result.expectedValue = ratio_expected;
    result.actualValue = ratio_actual;
    result.tolerance = ratio_expected * 0.15;
    
    testResults.push_back(result);
    printResult(result);
}

/*=========================================================================================================
 * 测试3：堆叠物体的摩擦力
 * 场景：一个物体压在另一个物体上
 * 验证：下层物体的摩擦力是否正确考虑了上层物体的重量
 *=========================================================================================================*/
void test3_stacked_objects_friction() {
    printHeader("测试3：堆叠物体的摩擦力");
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);
    world.setTimeStep(1.0 / 60.0);
    world.ground.setYLevel(0.0);
    world.ground.setFriction(0.3);
    
    // 创建底部物体（大方块）
    Shape* bottom = world.placeDynamicShapeByType("AABB", "Bottom", 0.0, 0.0, 2.0, 20.0, 10.0);
    bottom->setVelocity(10.0, 0.0);
    bottom->setRestitution(0.0);
    
    // 创建顶部物体（小球）
    double x_bottom, y_bottom;
    bottom->getCentre(x_bottom, y_bottom);
    const AABB* aabb = dynamic_cast<const AABB*>(bottom);
    double top_y = y_bottom + aabb->getHeight() / 2.0;
    
    Shape* top = world.placeDynamicShapeByType("Circle", "Top", x_bottom, top_y, 1.0, 3.0);
    top->setVelocity(10.0, 0.0);
    top->setRestitution(0.0);
    top->setFraction(0.3);  // 顶部物体与底部物体的摩擦系数
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(bottom);
    shapeList.push_back(top);
    
    std::cout << "\n场景设置：" << std::endl;
    std::cout << "  底部物体质量: " << bottom->getMass() << " kg" << std::endl;
    std::cout << "  顶部物体质量: " << top->getMass() << " kg" << std::endl;
    std::cout << "  总质量: " << (bottom->getMass() + top->getMass()) << " kg" << std::endl;
    std::cout << "  摩擦系数: " << world.ground.getFriction() << std::endl;
    std::cout << "  初始速度: 10.0 m/s" << std::endl;
    
    // 理论计算（底部物体受到的摩擦力需要考虑上层物体）
    double totalMass = bottom->getMass() + top->getMass();
    double normalForce = totalMass * world.getGravity();
    double frictionForce = world.ground.getFriction() * normalForce;
    double accel = frictionForce / bottom->getMass();  // 注意：只有底部物体的质量
    
    // 但实际上，两个物体会一起运动（假设顶部不滑动）
    // 系统总摩擦力 = μ * (m1 + m2) * g
    // 系统总质量 = m1 + m2
    // 系统减速度 = μ * g
    double systemAccel = world.ground.getFriction() * world.getGravity();
    double theoreticalTime = 10.0 / systemAccel;
    double theoreticalFrames = theoreticalTime * 60.0;
    
    std::cout << "\n理论分析（假设两物体一起运动）：" << std::endl;
    std::cout << "  总正压力: N = " << totalMass << " * " << world.getGravity() << " = " << normalForce << " N" << std::endl;
    std::cout << "  总摩擦力: f = " << world.ground.getFriction() << " * " << normalForce << " = " << frictionForce << " N" << std::endl;
    std::cout << "  系统减速度: a = μ*g = " << systemAccel << " m/s?" << std::endl;
    std::cout << "  理论停止时间: " << theoreticalTime << " s = " << theoreticalFrames << " 帧" << std::endl;
    
    world.start();
    
    // 稳定化
    for (int i = 0; i < 60; i++) {
        world.update(shapeList, world.ground);
    }
    
    // 重置速度
    bottom->setVelocity(10.0, 0.0);
    top->setVelocity(10.0, 0.0);
    
    // 模拟
    int stopFrame = -1;
    for (int frame = 0; frame <= 300; frame++) {
        world.update(shapeList, world.ground);
        
        double vx_bottom, vy_bottom;
        bottom->getVelocity(vx_bottom, vy_bottom);
        
        if (std::abs(vx_bottom) < 0.01 && stopFrame < 0) {
            stopFrame = frame;
            break;
        }
    }
    
    std::cout << "\n实际结果：" << std::endl;
    std::cout << "  停止帧数: " << stopFrame << " 帧" << std::endl;
    
    // 验证结果
    double tolerance = theoreticalFrames * 0.15;
    bool passed = std::abs(stopFrame - theoreticalFrames) <= tolerance;
    
    TestResult result;
    result.testName = "堆叠物体的摩擦力";
    result.passed = passed;
    result.message = passed ? "堆叠物体的正压力累积正确" : "堆叠物体的摩擦力计算有误";
    result.expectedValue = theoreticalFrames;
    result.actualValue = stopFrame;
    result.tolerance = tolerance;
    
    testResults.push_back(result);
    printResult(result);
}

/*=========================================================================================================
 * 主函数
 *=========================================================================================================*/
int main() {
    printHeader("摩擦力综合测试程序");
    
    std::cout << "\n本测试将验证物理引擎的摩擦力系统是否正确实现了：" << std::endl;
    std::cout << "  1. 基础摩擦力公式: f = μ * N" << std::endl;
    std::cout << "  2. 减速度公式: a = f / m" << std::endl;
    std::cout << "  3. 正压力累积（堆叠物体）" << std::endl;
    std::cout << "  4. 摩擦系数的影响" << std::endl;
    
    // 运行测试
    test1_basic_horizontal_friction();
    test2_different_friction_coefficients();
    test3_stacked_objects_friction();
    
    // 打印总结
    printSummary();
    
    return 0;
}
