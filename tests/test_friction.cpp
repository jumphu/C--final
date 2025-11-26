/*=========================================================================================================
 * 摩擦力测试程序
 * 
 * 测试场景：
 * 1. 小球在地面上滑动 - 测试基本摩擦力
 * 2. 滑块在地面上滑动 - 测试AABB摩擦力
 * 3. 不同摩擦系数对比 - 测试摩擦系数影响
 * 4. 小球在运动平台上 - 测试相对摩擦力
 * 5. 堆叠物体摩擦力 - 测试正压力累积
 * 6. 摩擦力停止效果 - 测试速度衰减到零
 *=========================================================================================================*/

#include "physicalWorld.h"
#include "shapes.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

void printTestHeader(const std::string& testName) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  " << testName << std::endl;
    std::cout << "========================================" << std::endl;
}

void printShapeState(const std::string& name, Shape* shape, int frame = -1) {
    double x, y, vx, vy;
    shape->getCentre(x, y);
    shape->getVelocity(vx, vy);
    
    std::cout << std::fixed << std::setprecision(3);
    if (frame >= 0) {
        std::cout << "[Frame " << std::setw(3) << frame << "] ";
    }
    std::cout << name << " - "
              << "位置: (" << std::setw(7) << x << ", " << std::setw(7) << y << "), "
              << "速度: (" << std::setw(7) << vx << ", " << std::setw(7) << vy << ")"
              << std::endl;
}

/*=========================================================================================================
 * 测试1：小球在地面上滑动 - 测试基本摩擦力
 *=========================================================================================================*/
void test_ball_sliding_on_ground() {
    printTestHeader("测试1：小球在地面上滑动（基本摩擦力）");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.ground.setFriction(0.3);  // 设置地面摩擦系数为0.3
    world.start();
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  重力: " << world.getGravity() << " m/s?" << std::endl;
    std::cout << "  地面摩擦系数: " << world.ground.getFriction() << std::endl;
    std::cout << "  地面高度: " << world.ground.getYLevel() << std::endl;
    
    // 创建小球，初始水平速度10 m/s
    Circle* ball = new Circle(1.0, 5.0, 0.0, 10.0);  // 质量1kg, 半径5, 在地面上
    ball->setName("SlidingBall");
    ball->setVelocity(10.0, 0.0);  // 初始水平速度10 m/s
    ball->setRestitution(0.1);     // 低弹性，避免弹跳
    world.addDynamicShape(ball);
    world.placeDynamicShape(*ball, 0.0, world.ground.getYLevel());
    
    std::cout << "\n小球初始状态：" << std::endl;
    std::cout << "  质量: " << ball->getMass() << " kg" << std::endl;
    std::cout << "  半径: " << ball->getRadius() << " m" << std::endl;
    std::cout << "  初始速度: 10.0 m/s (水平)" << std::endl;
    std::cout << "  弹性系数: 0.1" << std::endl;
    
    std::cout << "\n模拟过程（每10帧显示一次）：" << std::endl;
    printShapeState("Ball", ball, 0);
    
    double initialSpeed = 10.0;
    int stopFrame = -1;
    
    // 模拟100帧
    for (int i = 1; i <= 100; i++) {
        world.update(world.dynamicShapeList, world.ground);
        
        // 每10帧打印一次
        if (i % 10 == 0) {
            printShapeState("Ball", ball, i);
        }
        
        // 检测停止
        double vx, vy;
        ball->getVelocity(vx, vy);
        if (std::abs(vx) < 0.1 && stopFrame < 0) {
            stopFrame = i;
            std::cout << "\n>>> 小球在第 " << stopFrame << " 帧停止（速度 < 0.1 m/s）" << std::endl;
            printShapeState("Ball", ball, stopFrame);
        }
    }
    
    // 最终状态
    std::cout << "\n最终状态：" << std::endl;
    printShapeState("Ball", ball, 100);
    
    double vx, vy;
    ball->getVelocity(vx, vy);
    double finalSpeed = std::abs(vx);
    
    std::cout << "\n结果验证：" << std::endl;
    std::cout << "  初始速度: " << initialSpeed << " m/s" << std::endl;
    std::cout << "  最终速度: " << finalSpeed << " m/s" << std::endl;
    std::cout << "  速度降低: " << (initialSpeed - finalSpeed) << " m/s" << std::endl;
    
    if (finalSpeed < initialSpeed * 0.5) {
        std::cout << "  ? 摩擦力正常工作（速度降低超过50%）" << std::endl;
    } else {
        std::cout << "  ??  摩擦力可能不足（速度降低不到50%）" << std::endl;
    }
    
    if (stopFrame > 0) {
        std::cout << "  ? 小球在第 " << stopFrame << " 帧停止" << std::endl;
    } else {
        std::cout << "  ??  小球在100帧内未完全停止" << std::endl;
    }
    
    delete ball;
}

/*=========================================================================================================
 * 测试2：滑块在地面上滑动
 *=========================================================================================================*/
void test_box_sliding_on_ground() {
    printTestHeader("测试2：滑块（AABB）在地面上滑动");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.ground.setFriction(0.4);
    world.start();
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  地面摩擦系数: " << world.ground.getFriction() << std::endl;
    
    // 创建滑块
    AABB* box = new AABB(2.0, 10.0, 10.0, 0.0, 15.0);
    box->setName("SlidingBox");
    box->setVelocity(15.0, 0.0);
    box->setRestitution(0.1);
    world.addDynamicShape(box);
    world.placeDynamicShape(*box, 0.0, world.ground.getYLevel());
    
    std::cout << "\n滑块初始状态：" << std::endl;
    std::cout << "  质量: " << box->getMass() << " kg" << std::endl;
    std::cout << "  尺寸: " << box->getWidth() << " x " << box->getHeight() << " m" << std::endl;
    std::cout << "  初始速度: 15.0 m/s" << std::endl;
    
    std::cout << "\n模拟过程：" << std::endl;
    printShapeState("Box", box, 0);
    
    double initialSpeed = 15.0;
    int stopFrame = -1;
    
    for (int i = 1; i <= 100; i++) {
        world.update(world.dynamicShapeList, world.ground);
        
        if (i % 10 == 0) {
            printShapeState("Box", box, i);
        }
        
        double vx, vy;
        box->getVelocity(vx, vy);
        if (std::abs(vx) < 0.1 && stopFrame < 0) {
            stopFrame = i;
            std::cout << "\n>>> 滑块在第 " << stopFrame << " 帧停止" << std::endl;
        }
    }
    
    double vx, vy;
    box->getVelocity(vx, vy);
    double finalSpeed = std::abs(vx);
    
    std::cout << "\n结果验证：" << std::endl;
    std::cout << "  初始速度: " << initialSpeed << " m/s" << std::endl;
    std::cout << "  最终速度: " << finalSpeed << " m/s" << std::endl;
    
    if (finalSpeed < initialSpeed * 0.3) {
        std::cout << "  ? AABB摩擦力正常工作" << std::endl;
    } else {
        std::cout << "  ??  AABB摩擦力可能不足" << std::endl;
    }
    
    delete box;
}

/*=========================================================================================================
 * 测试3：不同摩擦系数对比
 *=========================================================================================================*/
void test_different_friction_coefficients() {
    printTestHeader("测试3：不同摩擦系数对比");
    
    double frictions[] = {0.0, 0.2, 0.4, 0.6, 0.8};
    
    std::cout << "测试5个不同的摩擦系数，观察速度衰减\n" << std::endl;
    std::cout << std::setw(12) << "摩擦系数" 
              << std::setw(15) << "初始速度" 
              << std::setw(15) << "50帧后速度" 
              << std::setw(15) << "速度保留率" << std::endl;
    std::cout << std::string(57, '-') << std::endl;
    
    for (double friction : frictions) {
        PhysicalWorld world;
        world.setGravity(9.8);
        world.ground.setFriction(friction);
        world.start();
        
        Circle* ball = new Circle(1.0, 5.0, 0.0, 10.0);
        ball->setVelocity(10.0, 0.0);
        ball->setRestitution(0.0);
        world.addDynamicShape(ball);
        world.placeDynamicShape(*ball, 0.0, world.ground.getYLevel());
        
        double initialSpeed = 10.0;
        
        // 模拟50帧
        for (int i = 0; i < 50; i++) {
            world.update(world.dynamicShapeList, world.ground);
        }
        
        double vx, vy;
        ball->getVelocity(vx, vy);
        double finalSpeed = std::abs(vx);
        double retentionRate = (finalSpeed / initialSpeed) * 100.0;
        
        std::cout << std::fixed << std::setprecision(1);
        std::cout << std::setw(12) << friction 
                  << std::setw(15) << initialSpeed 
                  << std::setw(15) << finalSpeed 
                  << std::setw(14) << retentionRate << "%" << std::endl;
        
        delete ball;
    }
    
    std::cout << "\n结果分析：" << std::endl;
    std::cout << "  摩擦系数越大，速度衰减越快" << std::endl;
    std::cout << "  摩擦系数 = 0.0 时，速度应该几乎不变（无摩擦）" << std::endl;
    std::cout << "  摩擦系数 = 0.8 时，速度应该显著降低" << std::endl;
}

/*=========================================================================================================
 * 测试4：小球在运动平台上 - 测试相对摩擦力
 *=========================================================================================================*/
void test_ball_on_moving_platform() {
    printTestHeader("测试4：小球在运动平台上（相对摩擦力）");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.start();
    
    // 创建平台（大质量的滑块作为平台）
    AABB* platform = new AABB(100.0, 50.0, 10.0, 0.0, 20.0);
    platform->setName("Platform");
    platform->setVelocity(5.0, 0.0);  // 平台向右移动 5 m/s
    platform->setRestitution(0.0);
    platform->fraction = 0.5;  // 平台摩擦系数 0.5
    world.addDynamicShape(platform);
    world.placeDynamicShape(*platform, 0.0, world.ground.getYLevel());
    
    // 创建小球，放在平台上
    Circle* ball = new Circle(1.0, 5.0, 0.0, 30.0);
    ball->setName("Ball");
    ball->setVelocity(0.0, 0.0);  // 小球初始静止
    ball->setRestitution(0.3);
    world.addDynamicShape(ball);
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  平台质量: " << platform->getMass() << " kg（重物）" << std::endl;
    std::cout << "  平台初始速度: 5.0 m/s（向右）" << std::endl;
    std::cout << "  平台摩擦系数: 0.5" << std::endl;
    std::cout << "  小球质量: " << ball->getMass() << " kg" << std::endl;
    std::cout << "  小球初始速度: 0.0 m/s（静止）" << std::endl;
    
    std::cout << "\n模拟过程：" << std::endl;
    std::cout << "Frame   平台速度    小球速度    相对速度" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (int i = 0; i <= 50; i++) {
        if (i > 0) {
            world.update(world.dynamicShapeList, world.ground);
        }
        
        if (i % 10 == 0) {
            double platformVx, platformVy, ballVx, ballVy;
            platform->getVelocity(platformVx, platformVy);
            ball->getVelocity(ballVx, ballVy);
            double relativeVx = ballVx - platformVx;
            
            std::cout << std::fixed << std::setprecision(3);
            std::cout << std::setw(5) << i 
                      << std::setw(12) << platformVx 
                      << std::setw(12) << ballVx 
                      << std::setw(12) << relativeVx << std::endl;
        }
    }
    
    double platformVx, platformVy, ballVx, ballVy;
    platform->getVelocity(platformVx, platformVy);
    ball->getVelocity(ballVx, ballVy);
    double relativeVx = ballVx - platformVx;
    
    std::cout << "\n结果验证：" << std::endl;
    std::cout << "  最终平台速度: " << platformVx << " m/s" << std::endl;
    std::cout << "  最终小球速度: " << ballVx << " m/s" << std::endl;
    std::cout << "  最终相对速度: " << relativeVx << " m/s" << std::endl;
    
    if (std::abs(relativeVx) < 1.0) {
        std::cout << "  ? 相对摩擦力正常工作（相对速度接近0）" << std::endl;
        std::cout << "  ? 小球被平台摩擦力加速到接近平台速度" << std::endl;
    } else {
        std::cout << "  ??  相对摩擦力可能不足" << std::endl;
    }
    
    delete ball;
    delete platform;
}

/*=========================================================================================================
 * 测试5：堆叠物体摩擦力 - 测试正压力累积
 *=========================================================================================================*/
void test_stacked_objects_friction() {
    printTestHeader("测试5：堆叠物体摩擦力（正压力累积）");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.ground.setFriction(0.4);
    world.start();
    
    std::cout << "场景设置：3个小球垂直堆叠，最下面的球有水平速度" << std::endl;
    std::cout << "  地面摩擦系数: 0.4" << std::endl;
    std::cout << "  每个球质量: 1.0 kg" << std::endl;
    
    // 创建底部小球（有初始速度）
    Circle* ball1 = new Circle(1.0, 5.0, 0.0, 10.0);
    ball1->setName("Ball1_Bottom");
    ball1->setVelocity(8.0, 0.0);
    ball1->setRestitution(0.1);
    world.addDynamicShape(ball1);
    world.placeDynamicShape(*ball1, 0.0, world.ground.getYLevel());
    
    // 创建中间小球
    Circle* ball2 = new Circle(1.0, 5.0, 0.0, 20.0);
    ball2->setName("Ball2_Middle");
    ball2->setVelocity(8.0, 0.0);
    ball2->setRestitution(0.1);
    world.addDynamicShape(ball2);
    
    // 创建顶部小球
    Circle* ball3 = new Circle(1.0, 5.0, 0.0, 30.0);
    ball3->setName("Ball3_Top");
    ball3->setVelocity(8.0, 0.0);
    ball3->setRestitution(0.1);
    world.addDynamicShape(ball3);
    
    std::cout << "\n初始状态：所有球初始速度 8.0 m/s" << std::endl;
    printShapeState("Ball1", ball1, 0);
    printShapeState("Ball2", ball2, 0);
    printShapeState("Ball3", ball3, 0);
    
    std::cout << "\n模拟过程（每20帧显示一次）：" << std::endl;
    
    for (int i = 1; i <= 80; i++) {
        world.update(world.dynamicShapeList, world.ground);
        
        if (i % 20 == 0) {
            std::cout << "\n--- Frame " << i << " ---" << std::endl;
            printShapeState("Ball1", ball1);
            printShapeState("Ball2", ball2);
            printShapeState("Ball3", ball3);
            
            // 打印正压力信息
            double fx1, fy1, fx2, fy2, fx3, fy3;
            ball1->getNormalForce(fx1, fy1);
            ball2->getNormalForce(fx2, fy2);
            ball3->getNormalForce(fx3, fy3);
            
            std::cout << "  正压力: Ball1=" << std::abs(fy1) 
                     << ", Ball2=" << std::abs(fy2) 
                     << ", Ball3=" << std::abs(fy3) << std::endl;
        }
    }
    
    double vx1, vy1, vx2, vy2, vx3, vy3;
    ball1->getVelocity(vx1, vy1);
    ball2->getVelocity(vx2, vy2);
    ball3->getVelocity(vx3, vy3);
    
    std::cout << "\n结果验证：" << std::endl;
    std::cout << "  Ball1最终速度: " << vx1 << " m/s（承受3个球的重量）" << std::endl;
    std::cout << "  Ball2最终速度: " << vx2 << " m/s（承受2个球的重量）" << std::endl;
    std::cout << "  Ball3最终速度: " << vx3 << " m/s（只承受自身重量）" << std::endl;
    
    // 理论上，底部球因为承受更大正压力，应该减速更快
    if (vx1 < vx2 && vx2 < vx3) {
        std::cout << "  ? 正压力累积正确（底部球减速最快）" << std::endl;
    } else {
        std::cout << "  ??  正压力累积可能有问题" << std::endl;
    }
    
    delete ball1;
    delete ball2;
    delete ball3;
}

/*=========================================================================================================
 * 测试6：摩擦力停止效果 - 测试速度衰减到零
 *=========================================================================================================*/
void test_friction_stopping() {
    printTestHeader("测试6：摩擦力停止效果");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.ground.setFriction(0.5);
    world.start();
    
    std::cout << "测试不同初始速度下的停止时间" << std::endl;
    std::cout << "  地面摩擦系数: 0.5（较大）\n" << std::endl;
    
    std::cout << std::setw(15) << "初始速度" 
              << std::setw(15) << "停止帧数" 
              << std::setw(15) << "理论停止时间" << std::endl;
    std::cout << std::string(45, '-') << std::endl;
    
    double initialSpeeds[] = {5.0, 10.0, 15.0, 20.0};
    
    for (double v0 : initialSpeeds) {
        PhysicalWorld testWorld;
        testWorld.setGravity(9.8);
        testWorld.ground.setFriction(0.5);
        testWorld.start();
        
        Circle* ball = new Circle(1.0, 5.0, 0.0, 10.0);
        ball->setVelocity(v0, 0.0);
        ball->setRestitution(0.0);
        testWorld.addDynamicShape(ball);
        testWorld.placeDynamicShape(*ball, 0.0, testWorld.ground.getYLevel());
        
        int stopFrame = -1;
        
        // 模拟直到停止
        for (int i = 1; i <= 200; i++) {
            testWorld.update(testWorld.dynamicShapeList, testWorld.ground);
            
            double vx, vy;
            ball->getVelocity(vx, vy);
            
            if (std::abs(vx) < 0.05 && stopFrame < 0) {
                stopFrame = i;
                break;
            }
        }
        
        // 理论停止时间 = v0 / (μ * g)
        double theoreticalTime = v0 / (0.5 * 9.8);
        
        std::cout << std::fixed << std::setprecision(1);
        std::cout << std::setw(15) << v0 
                  << std::setw(15) << stopFrame 
                  << std::setw(14) << theoreticalTime << " s" << std::endl;
        
        delete ball;
    }
    
    std::cout << "\n结果分析：" << std::endl;
    std::cout << "  初始速度越大，停止时间越长（线性关系）" << std::endl;
    std::cout << "  理论停止时间 = v0 / (μ * g)" << std::endl;
    std::cout << "  实际停止帧数应与理论时间成正比" << std::endl;
}

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "     摩擦力测试程序" << std::endl;
    std::cout << "============================================" << std::endl;
    
    try {
        test_ball_sliding_on_ground();
        test_box_sliding_on_ground();
        test_different_friction_coefficients();
        test_ball_on_moving_platform();
        test_stacked_objects_friction();
        test_friction_stopping();
        
        std::cout << "\n============================================" << std::endl;
        std::cout << "  ? 所有摩擦力测试完成！" << std::endl;
        std::cout << "============================================" << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n? 测试失败: " << e.what() << std::endl;
        return 1;
    }
}
