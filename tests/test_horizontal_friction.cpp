/*=========================================================================================================
 * 水平摩擦力测试 (Horizontal Friction Test)
 * 
 * 测试目的：
 *   验证当倾斜角度为0（水平）时，摩擦力系统是否正常工作
 *   
 * 测试场景：
 *   1. 基础水平摩擦 - 单个物体在水平地面上滑动
 *   2. 不同摩擦系数对比 - 验证摩擦力大小与摩擦系数的关系
 *   3. 堆叠物体的水平摩擦 - 验证正压力累积
 *   4. 运动平台上的摩擦 - 验证相对摩擦力
 * 
 * 预期行为：
 *   - 倾斜角=0时，applyGravitySin应该被调用但不施加力（sin(0)=0）
 *   - 摩擦力只与正压力和摩擦系数有关
 *   - 物体应该在摩擦力作用下减速直至停止
 *=========================================================================================================*/

#include "../include/physicalWorld.h"
#include <iostream>
#include <iomanip>
#include <cmath>

void printHeader(const std::string& title) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << "========================================" << std::endl;
}

void printResult(const std::string& description, bool passed) {
    std::cout << description << ": " << (passed ? "? 通过" : "? 失败") << std::endl;
}

/*=========================================================================================================
 * 测试1：基础水平摩擦
 * 场景：单个小球在水平地面上滑动
 *=========================================================================================================*/
void test_basic_horizontal_friction() {
    printHeader("测试1：基础水平摩擦");
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);  // ← 水平！
    world.setTimeStep(1.0 / 60.0);
    
    world.ground.setYLevel(0.0);
    world.ground.setFriction(0.3);  // 摩擦系数0.3
    
    // 创建一个小球，初始速度10 m/s
    Shape* ball = world.placeDynamicShapeByType("Circle", "Ball", 0.0, 0.0, 1.0, 5.0);
    ball->setVelocity(10.0, 0.0);
    ball->setRestitution(0.0);
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(ball);
    
    std::cout << "\n场景设置：" << std::endl;
    std::cout << "  倾斜角度: 0° (水平)" << std::endl;
    std::cout << "  重力加速度: 10 m/s?" << std::endl;
    std::cout << "  摩擦系数: 0.3" << std::endl;
    std::cout << "  小球质量: 1 kg" << std::endl;
    std::cout << "  初始速度: 10 m/s" << std::endl;
    
    std::cout << "\n理论分析：" << std::endl;
    std::cout << "  正压力 N = m*g = 1*10 = 10 N" << std::endl;
    std::cout << "  摩擦力 f = μ*N = 0.3*10 = 3 N" << std::endl;
    std::cout << "  减速度 a = f/m = 3/1 = 3 m/s?" << std::endl;
    std::cout << "  理论停止时间 = v?/a = 10/3 ≈ 3.33 s" << std::endl;
    std::cout << "  理论停止帧数 = 3.33*60 ≈ 200 帧" << std::endl;
    
    world.start();
    
    // 稳定堆叠
    for (int i = 0; i < 30; i++) {
        world.update(shapeList, world.ground);
    }
    
    // 重置速度
    ball->setVelocity(10.0, 0.0);
    
    std::cout << "\n模拟过程（每20帧显示一次）：" << std::endl;
    std::cout << std::setw(8) << "帧数" 
              << std::setw(12) << "速度(m/s)" 
              << std::setw(12) << "位置(m)"
              << std::setw(12) << "Y位置(m)" << std::endl;
    std::cout << std::string(44, '-') << std::endl;
    
    int stopFrame = -1;
    double prevVx = 10.0;
    
    for (int frame = 0; frame <= 250; frame++) {
        world.update(shapeList, world.ground);
        
        double x, y, vx, vy;
        ball->getCentre(x, y);
        ball->getVelocity(vx, vy);
        
        if (frame % 20 == 0) {
            std::cout << std::setw(8) << frame
                      << std::setw(12) << std::fixed << std::setprecision(3) << vx
                      << std::setw(12) << x
                      << std::setw(12) << y << std::endl;
        }
        
        // 检测停止
        if (stopFrame == -1 && std::abs(vx) < 0.01) {
            stopFrame = frame;
        }
        
        prevVx = vx;
    }
    
    double finalVx, finalVy, finalX, finalY;
    ball->getVelocity(finalVx, finalVy);
    ball->getCentre(finalX, finalY);
    
    std::cout << "\n结果验证：" << std::endl;
    std::cout << "  实际停止帧数: " << stopFrame << " 帧" << std::endl;
    std::cout << "  理论停止帧数: 200 帧" << std::endl;
    std::cout << "  最终速度: " << finalVx << " m/s" << std::endl;
    std::cout << "  Y位置始终为: " << finalY << " m (应该=5.0)" << std::endl;
    
    bool test1 = (stopFrame >= 180 && stopFrame <= 220);  // 允许±10%误差
    bool test2 = std::abs(finalVx) < 0.1;
    bool test3 = std::abs(finalY - 5.0) < 0.1;  // Y位置应该保持在5.0
    
    printResult("停止时间符合理论", test1);
    printResult("最终速度接近0", test2);
    printResult("Y位置保持不变（无垂直运动）", test3);
    
    if (test1 && test2 && test3) {
        std::cout << "\n? 测试通过！水平摩擦力正常工作" << std::endl;
    } else {
        std::cout << "\n? 测试失败！可能存在问题" << std::endl;
    }
}

/*=========================================================================================================
 * 测试2：摩擦系数对比（水平）
 * 场景：测试不同摩擦系数下的减速效果
 *=========================================================================================================*/
void test_friction_coefficient_comparison() {
    printHeader("测试2：摩擦系数对比（水平）");
    
    std::cout << "\n测试5个不同的摩擦系数，观察停止时间\n" << std::endl;
    
    std::cout << std::setw(15) << "摩擦系数"
              << std::setw(15) << "初始速度"
              << std::setw(15) << "停止帧数"
              << std::setw(15) << "理论帧数" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    double frictions[] = {0.1, 0.2, 0.3, 0.4, 0.5};
    double initialSpeed = 10.0;
    double gravity = 10.0;
    double fps = 60.0;
    
    for (double mu : frictions) {
        PhysicalWorld world;
        world.setGravity(gravity);
        world.setInclineAngle(0.0);  // 水平
        world.setTimeStep(1.0 / fps);
        
        world.ground.setYLevel(0.0);
        world.ground.setFriction(mu);
        
        Shape* ball = world.placeDynamicShapeByType("Circle", "Ball", 0.0, 0.0, 1.0, 5.0);
        ball->setVelocity(initialSpeed, 0.0);
        ball->setRestitution(0.0);
        
        std::vector<Shape*> shapeList;
        shapeList.push_back(ball);
        
        world.start();
        
        // 稳定
        for (int i = 0; i < 30; i++) {
            world.update(shapeList, world.ground);
        }
        ball->setVelocity(initialSpeed, 0.0);
        
        // 模拟直到停止
        int stopFrame = -1;
        for (int frame = 0; frame <= 300; frame++) {
            world.update(shapeList, world.ground);
            
            double vx, vy;
            ball->getVelocity(vx, vy);
            
            if (stopFrame == -1 && std::abs(vx) < 0.01) {
                stopFrame = frame;
                break;
            }
        }
        
        // 理论停止时间: t = v? / (μ*g)
        double theoreticalTime = initialSpeed / (mu * gravity);
        int theoreticalFrames = (int)(theoreticalTime * fps);
        
        std::cout << std::setw(15) << std::fixed << std::setprecision(1) << mu
                  << std::setw(15) << std::setprecision(1) << initialSpeed
                  << std::setw(15) << stopFrame
                  << std::setw(15) << theoreticalFrames << std::endl;
    }
    
    std::cout << "\n结果分析：" << std::endl;
    std::cout << "  摩擦系数越大，停止时间越短（反比关系）" << std::endl;
    std::cout << "  实际停止帧数应该接近理论值" << std::endl;
}

/*=========================================================================================================
 * 测试3：堆叠物体的水平摩擦
 * 场景：两个球堆叠在一起，检验正压力累积
 *=========================================================================================================*/
void test_stacked_horizontal_friction() {
    printHeader("测试3：堆叠物体的水平摩擦");
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);  // 水平
    world.setTimeStep(1.0 / 60.0);
    
    world.ground.setYLevel(0.0);
    world.ground.setFriction(0.3);
    
    // 创建两个球：下面的球和上面的球
    Shape* ball1 = world.placeDynamicShapeByType("Circle", "Bottom", 0.0, 0.0, 1.0, 5.0);
    Shape* ball2 = world.placeDynamicShapeByType("Circle", "Top", 0.0, 15.0, 1.0, 5.0);
    
    ball1->setVelocity(10.0, 0.0);
    ball2->setVelocity(10.0, 0.0);
    ball1->setRestitution(0.0);
    ball2->setRestitution(0.0);
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(ball1);
    shapeList.push_back(ball2);
    
    std::cout << "\n场景设置：" << std::endl;
    std::cout << "  两个球堆叠，各1kg，初速10 m/s" << std::endl;
    std::cout << "  地面摩擦系数: 0.3" << std::endl;
    
    std::cout << "\n理论分析：" << std::endl;
    std::cout << "  Bottom球承受: 自身1kg + Top球1kg = 2kg" << std::endl;
    std::cout << "  Bottom正压力: N = 2*10 = 20 N" << std::endl;
    std::cout << "  Bottom摩擦力: f = 0.3*20 = 6 N" << std::endl;
    std::cout << "  Bottom减速度: a = 6/1 = 6 m/s?" << std::endl;
    std::cout << "  Top球正压力: N = 1*10 = 10 N (在Bottom上)" << std::endl;
    
    world.start();
    
    // 稳定堆叠
    for (int i = 0; i < 60; i++) {
        world.update(shapeList, world.ground);
    }
    
    // 检查堆叠
    bool isStacked = (ball2->getSupporter() == ball1);
    std::cout << "\n堆叠状态: " << (isStacked ? "? 成功堆叠" : "? 未堆叠") << std::endl;
    
    // 重置速度
    ball1->setVelocity(10.0, 0.0);
    ball2->setVelocity(10.0, 0.0);
    
    std::cout << "\n模拟过程（每30帧显示一次）：" << std::endl;
    std::cout << std::setw(8) << "帧数"
              << std::setw(15) << "Bottom速度"
              << std::setw(15) << "Top速度"
              << std::setw(15) << "Bottom正压力" << std::endl;
    std::cout << std::string(53, '-') << std::endl;
    
    int bottomStopFrame = -1;
    int topStopFrame = -1;
    
    for (int frame = 0; frame <= 200; frame++) {
        world.update(shapeList, world.ground);
        
        double v1x, v1y, v2x, v2y;
        ball1->getVelocity(v1x, v1y);
        ball2->getVelocity(v2x, v2y);
        
        double fx, fy;
        ball1->getNormalForce(fx, fy);
        
        if (frame % 30 == 0) {
            std::cout << std::setw(8) << frame
                      << std::setw(15) << std::fixed << std::setprecision(3) << v1x
                      << std::setw(15) << v2x
                      << std::setw(15) << std::abs(fy) << std::endl;
        }
        
        if (bottomStopFrame == -1 && std::abs(v1x) < 0.01) {
            bottomStopFrame = frame;
        }
        if (topStopFrame == -1 && std::abs(v2x) < 0.01) {
            topStopFrame = frame;
        }
    }
    
    std::cout << "\n结果验证：" << std::endl;
    std::cout << "  Bottom停止帧数: " << bottomStopFrame << " (理论≈100帧)" << std::endl;
    std::cout << "  Top停止帧数: " << topStopFrame << std::endl;
    
    // Bottom应该因为承受双倍正压力而更快停止
    bool test1 = (bottomStopFrame >= 80 && bottomStopFrame <= 120);
    bool test2 = isStacked;
    
    printResult("Bottom球停止时间符合理论（承受双倍正压力）", test1);
    printResult("堆叠稳定", test2);
}

/*=========================================================================================================
 * 测试4：水平运动平台上的摩擦
 * 场景：小球在运动平台上，验证相对摩擦力
 *=========================================================================================================*/
void test_horizontal_moving_platform() {
    printHeader("测试4：水平运动平台上的摩擦");
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);  // 水平
    world.setTimeStep(1.0 / 60.0);
    
    world.ground.setYLevel(0.0);
    world.ground.setFriction(0.0);  // 地面无摩擦
    
    // 创建平台（大质量）和小球
    Shape* platform = world.placeDynamicShapeByType("AABB", "Platform", 0.0, 0.0, 100.0, 50.0, 10.0);
    Shape* ball = world.placeDynamicShapeByType("Circle", "Ball", 0.0, 15.0, 1.0, 5.0);
    
    platform->setVelocity(5.0, 0.0);  // 平台向右运动
    ball->setVelocity(0.0, 0.0);      // 小球静止
    platform->setFraction(0.5);       // 平台表面摩擦系数0.5
    platform->setRestitution(0.0);
    ball->setRestitution(0.0);
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(platform);
    shapeList.push_back(ball);
    
    std::cout << "\n场景设置：" << std::endl;
    std::cout << "  平台：100kg，初速5 m/s" << std::endl;
    std::cout << "  小球：1kg，初速0 m/s" << std::endl;
    std::cout << "  平台摩擦系数：0.5" << std::endl;
    
    std::cout << "\n预期行为：" << std::endl;
    std::cout << "  相对摩擦力应该使小球加速，平台减速" << std::endl;
    std::cout << "  最终两者速度趋于一致" << std::endl;
    
    world.start();
    
    // 稳定堆叠
    for (int i = 0; i < 60; i++) {
        world.update(shapeList, world.ground);
    }
    
    // 检查堆叠
    bool isOnPlatform = (ball->getSupporter() == platform);
    std::cout << "\n堆叠状态: " << (isOnPlatform ? "? 小球在平台上" : "? 未堆叠") << std::endl;
    
    // 重置速度
    platform->setVelocity(5.0, 0.0);
    ball->setVelocity(0.0, 0.0);
    
    std::cout << "\n模拟过程（每20帧显示一次）：" << std::endl;
    std::cout << std::setw(8) << "帧数"
              << std::setw(15) << "平台速度"
              << std::setw(15) << "小球速度"
              << std::setw(15) << "相对速度" << std::endl;
    std::cout << std::string(53, '-') << std::endl;
    
    double initialRelVel = 5.0;
    
    for (int frame = 0; frame <= 150; frame++) {
        world.update(shapeList, world.ground);
        
        double vp, vyp, vb, vyb;
        platform->getVelocity(vp, vyp);
        ball->getVelocity(vb, vyb);
        
        double relVel = vb - vp;
        
        if (frame % 20 == 0) {
            std::cout << std::setw(8) << frame
                      << std::setw(15) << std::fixed << std::setprecision(3) << vp
                      << std::setw(15) << vb
                      << std::setw(15) << relVel << std::endl;
        }
    }
    
    double finalVp, finalVb, dummy;
    platform->getVelocity(finalVp, dummy);
    ball->getVelocity(finalVb, dummy);
    double finalRelVel = finalVb - finalVp;
    
    std::cout << "\n结果验证：" << std::endl;
    std::cout << "  初始相对速度: " << initialRelVel << " m/s" << std::endl;
    std::cout << "  最终相对速度: " << finalRelVel << " m/s" << std::endl;
    std::cout << "  平台最终速度: " << finalVp << " m/s" << std::endl;
    std::cout << "  小球最终速度: " << finalVb << " m/s" << std::endl;
    
    bool test1 = std::abs(finalRelVel) < std::abs(initialRelVel);  // 相对速度减小
    bool test2 = finalVb > 0.1;  // 小球被加速
    bool test3 = finalVp < 5.0;  // 平台被减速
    
    printResult("相对速度减小", test1);
    printResult("小球被加速", test2);
    printResult("平台被减速（牛顿第三定律）", test3);
}

/*=========================================================================================================
 * 主函数
 *=========================================================================================================*/
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    水平摩擦力测试套件" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n本测试专门验证倾斜角度为0（水平）时的摩擦力系统" << std::endl;
    std::cout << "确保修复后的代码正确处理水平情况\n" << std::endl;
    
    test_basic_horizontal_friction();
    test_friction_coefficient_comparison();
    test_stacked_horizontal_friction();
    test_horizontal_moving_platform();
    
    std::cout << "\n============================================" << std::endl;
    std::cout << "  ? 所有水平摩擦力测试完成！" << std::endl;
    std::cout << "============================================" << std::endl;
    
    return 0;
}
