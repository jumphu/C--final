/*=========================================================================================================
 * 板块摩擦力测试程序
 * 
 * 测试场景：
 * 1. 静止小球在运动平台上 - 测试平台加速小球
 * 2. 运动小球在静止平台上 - 测试平台减速小球
 * 3. 不同速度方向 - 测试相对摩擦力
 * 4. 不同摩擦系数 - 测试摩擦系数影响
 * 5. 多个物体在同一平台 - 测试多物体交互
 * 6. 堆叠物体在运动平台 - 测试复合场景
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
 * 测试1：静止小球在运动平台上 - 测试平台加速小球
 *=========================================================================================================*/
void test_ball_on_moving_platform() {
    printTestHeader("测试1：静止小球在运动平台上（平台加速小球）");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.ground.setFriction(0.3);
    world.start();
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  重力: 9.8 m/s?" << std::endl;
    std::cout << "  地面摩擦系数: 0.3" << std::endl;
    
    // 创建平台（AABB，大质量，有初始速度）
    AABB* platform = new AABB(100.0, 40.0, 8.0, 0.0, 0.0);
    platform->setName("Platform");
    platform->setVelocity(10.0, 0.0);  // 平台向右移动 10 m/s
    platform->setRestitution(0.0);
    platform->fraction = 0.6;  // 平台摩擦系数 0.6
    world.addDynamicShape(platform);
    world.placeDynamicShape(*platform, 0.0, world.ground.getYLevel());
    
    // 创建小球，放在平台上，初始静止
    Circle* ball = new Circle(1.0, 3.0, 0.0, 0.0);
    ball->setName("Ball");
    ball->setVelocity(0.0, 0.0);  // 小球初始静止
    ball->setRestitution(0.2);
    world.addDynamicShape(ball);
    
    // 将球准确放置在平台上
    double platformY = platform->mass_centre[1];
    double ballY = platformY + platform->getHeight()/2 + ball->getRadius();
    ball->setCentre(0.0, ballY);
    
    std::cout << "\n初始状态：" << std::endl;
    std::cout << "  平台质量: " << platform->getMass() << " kg" << std::endl;
    std::cout << "  平台初始速度: 10.0 m/s（向右）" << std::endl;
    std::cout << "  平台摩擦系数: 0.6" << std::endl;
    std::cout << "  小球质量: " << ball->getMass() << " kg" << std::endl;
    std::cout << "  小球初始速度: 0.0 m/s（静止）" << std::endl;
    
    std::cout << "\n理论分析：" << std::endl;
    std::cout << "  摩擦力 = μ × m × g = 0.6 × 1.0 × 9.8 = 5.88 N" << std::endl;
    std::cout << "  加速度 = F/m = 5.88/1.0 = 5.88 m/s?" << std::endl;
    std::cout << "  经过约1.7秒，小球应接近平台速度" << std::endl;
    
    // 第一阶段：让物体稳定堆叠（30帧）
    std::cout << "\n第一阶段：稳定堆叠（30帧）" << std::endl;
    for (int i = 0; i < 30; i++) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    // 检查支撑关系
    if (ball->getSupporter() == platform) {
        std::cout << "? 小球成功放置在平台上" << std::endl;
    } else {
        std::cout << "??  小球未正确放置在平台上" << std::endl;
        std::cout << "  支撑者: " << (ball->getSupporter() ? ball->getSupporter()->getName() : "无") << std::endl;
    }
    
    std::cout << "\n第二阶段：观察摩擦力加速过程" << std::endl;
    std::cout << "\nFrame   平台速度    小球速度    相对速度    加速度" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    double lastBallVx = 0.0;
    
    for (int i = 1; i <= 60; i++) {
        world.update(world.dynamicShapeList, world.ground);
        
        if (i % 10 == 0) {
            double platformVx, platformVy, ballVx, ballVy;
            platform->getVelocity(platformVx, platformVy);
            ball->getVelocity(ballVx, ballVy);
            double relativeVx = ballVx - platformVx;
            
            // 计算加速度（速度变化率）
            double acceleration = (ballVx - lastBallVx) / 10.0;  // 10帧的平均加速度
            lastBallVx = ballVx;
            
            std::cout << std::fixed << std::setprecision(3);
            std::cout << std::setw(5) << i 
                      << std::setw(12) << platformVx 
                      << std::setw(12) << ballVx 
                      << std::setw(12) << relativeVx
                      << std::setw(12) << acceleration << std::endl;
        }
    }
    
    double platformVx, platformVy, ballVx, ballVy;
    platform->getVelocity(platformVx, platformVy);
    ball->getVelocity(ballVx, ballVy);
    double relativeVx = ballVx - platformVx;
    
    std::cout << "\n最终结果验证：" << std::endl;
    std::cout << "  平台速度: " << platformVx << " m/s" << std::endl;
    std::cout << "  小球速度: " << ballVx << " m/s" << std::endl;
    std::cout << "  相对速度: " << relativeVx << " m/s" << std::endl;
    std::cout << "  速度比: " << (ballVx / platformVx * 100.0) << "%" << std::endl;
    
    if (ballVx > 5.0 && std::abs(relativeVx) < 3.0) {
        std::cout << "\n? 测试通过！" << std::endl;
        std::cout << "  ? 小球被平台摩擦力加速" << std::endl;
        std::cout << "  ? 相对速度明显减小" << std::endl;
    } else {
        std::cout << "\n??  测试未完全通过" << std::endl;
        if (ballVx < 5.0) {
            std::cout << "  ? 小球加速不足" << std::endl;
        }
        if (std::abs(relativeVx) >= 3.0) {
            std::cout << "  ? 相对速度仍然较大" << std::endl;
        }
    }
    
    delete ball;
    delete platform;
}

/*=========================================================================================================
 * 测试2：运动小球在静止平台上 - 测试平台减速小球
 *=========================================================================================================*/
void test_moving_ball_on_static_platform() {
    printTestHeader("测试2：运动小球在静止平台上（平台减速小球）");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.ground.setFriction(0.2);
    world.start();
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  静止平台，小球初始有速度" << std::endl;
    
    // 创建平台（AABB，超大质量，静止）
    AABB* platform = new AABB(1000.0, 50.0, 10.0, 0.0, 0.0);
    platform->setName("Platform");
    platform->setVelocity(0.0, 0.0);  // 平台静止
    platform->setRestitution(0.0);
    platform->fraction = 0.5;  // 平台摩擦系数 0.5
    world.addDynamicShape(platform);
    world.placeDynamicShape(*platform, 0.0, world.ground.getYLevel());
    
    // 创建小球，有初始速度
    Circle* ball = new Circle(1.0, 3.0, 0.0, 0.0);
    ball->setName("Ball");
    ball->setVelocity(15.0, 0.0);  // 小球初始速度 15 m/s
    ball->setRestitution(0.1);
    world.addDynamicShape(ball);
    
    // 将球准确放置在平台上
    double platformY = platform->mass_centre[1];
    double ballY = platformY + platform->getHeight()/2 + ball->getRadius();
    ball->setCentre(0.0, ballY);
    
    std::cout << "\n初始状态：" << std::endl;
    std::cout << "  平台速度: 0.0 m/s（静止）" << std::endl;
    std::cout << "  平台摩擦系数: 0.5" << std::endl;
    std::cout << "  小球速度: 15.0 m/s（向右）" << std::endl;
    
    std::cout << "\n理论分析：" << std::endl;
    std::cout << "  摩擦力 = μ × m × g = 0.5 × 1.0 × 9.8 = 4.9 N" << std::endl;
    std::cout << "  减速度 = F/m = 4.9/1.0 = 4.9 m/s?" << std::endl;
    std::cout << "  停止时间 = v0/a = 15.0/4.9 ≈ 3.1 秒" << std::endl;
    
    // 稳定阶段
    std::cout << "\n稳定堆叠（30帧）..." << std::endl;
    for (int i = 0; i < 30; i++) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    std::cout << "\n观察小球减速过程（每10帧）：" << std::endl;
    printShapeState("Ball", ball, 0);
    
    int stopFrame = -1;
    
    for (int i = 1; i <= 100; i++) {
        world.update(world.dynamicShapeList, world.ground);
        
        if (i % 10 == 0) {
            printShapeState("Ball", ball, i);
        }
        
        double vx, vy;
        ball->getVelocity(vx, vy);
        if (std::abs(vx) < 0.5 && stopFrame < 0) {
            stopFrame = i;
            std::cout << "\n>>> 小球在第 " << stopFrame << " 帧减速到接近静止" << std::endl;
        }
    }
    
    double ballVx, ballVy;
    ball->getVelocity(ballVx, ballVy);
    
    std::cout << "\n最终结果：" << std::endl;
    std::cout << "  初始速度: 15.0 m/s" << std::endl;
    std::cout << "  最终速度: " << ballVx << " m/s" << std::endl;
    std::cout << "  速度降低: " << (15.0 - ballVx) << " m/s" << std::endl;
    
    if (ballVx < 5.0) {
        std::cout << "\n? 测试通过！平台摩擦力使小球减速" << std::endl;
    } else {
        std::cout << "\n??  小球减速不足" << std::endl;
    }
    
    delete ball;
    delete platform;
}

/*=========================================================================================================
 * 测试3：不同速度方向 - 测试相对摩擦力
 *=========================================================================================================*/
void test_opposite_direction_velocities() {
    printTestHeader("测试3：不同速度方向（相对摩擦力）");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.ground.setFriction(0.2);
    world.start();
    
    std::cout << "场景设置：平台向右，小球向左（相反方向）\n" << std::endl;
    
    // 创建平台（向右移动）
    AABB* platform = new AABB(50.0, 40.0, 8.0, 0.0, 0.0);
    platform->setName("Platform");
    platform->setVelocity(5.0, 0.0);  // 向右 5 m/s
    platform->setRestitution(0.0);
    platform->fraction = 0.7;
    world.addDynamicShape(platform);
    world.placeDynamicShape(*platform, 0.0, world.ground.getYLevel());
    
    // 创建小球（向左移动）
    Circle* ball = new Circle(1.0, 3.0, 0.0, 0.0);
    ball->setName("Ball");
    ball->setVelocity(-8.0, 0.0);  // 向左 8 m/s
    ball->setRestitution(0.1);
    world.addDynamicShape(ball);
    
    double platformY = platform->mass_centre[1];
    double ballY = platformY + platform->getHeight()/2 + ball->getRadius();
    ball->setCentre(0.0, ballY);
    
    std::cout << "初始状态：" << std::endl;
    std::cout << "  平台速度: +5.0 m/s（向右）" << std::endl;
    std::cout << "  小球速度: -8.0 m/s（向左）" << std::endl;
    std::cout << "  相对速度: -13.0 m/s" << std::endl;
    std::cout << "  平台摩擦系数: 0.7\n" << std::endl;
    
    // 稳定
    for (int i = 0; i < 30; i++) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    std::cout << "Frame   平台速度    小球速度    相对速度" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (int i = 1; i <= 50; i++) {
        world.update(world.dynamicShapeList, world.ground);
        
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
    std::cout << "  初始相对速度: -13.0 m/s" << std::endl;
    std::cout << "  最终相对速度: " << relativeVx << " m/s" << std::endl;
    
    if (std::abs(relativeVx) < 8.0) {
        std::cout << "\n? 测试通过！相对摩擦力使相对速度减小" << std::endl;
    } else {
        std::cout << "\n??  相对速度减小不明显" << std::endl;
    }
    
    delete ball;
    delete platform;
}

/*=========================================================================================================
 * 测试4：不同摩擦系数 - 测试摩擦系数影响
 *=========================================================================================================*/
void test_different_platform_friction() {
    printTestHeader("测试4：不同摩擦系数（摩擦系数影响）");
    
    double frictions[] = {0.1, 0.3, 0.5, 0.7, 0.9};
    
    std::cout << "测试5个不同的平台摩擦系数\n" << std::endl;
    std::cout << std::setw(12) << "摩擦系数" 
              << std::setw(15) << "小球初速" 
              << std::setw(15) << "50帧后速度" 
              << std::setw(15) << "加速比例" << std::endl;
    std::cout << std::string(57, '-') << std::endl;
    
    for (double friction : frictions) {
        PhysicalWorld world;
        world.setGravity(9.8);
        world.ground.setFriction(0.2);
        world.start();
        
        // 平台速度10 m/s
        AABB* platform = new AABB(100.0, 40.0, 8.0, 0.0, 0.0);
        platform->setVelocity(10.0, 0.0);
        platform->setRestitution(0.0);
        platform->fraction = friction;
        world.addDynamicShape(platform);
        world.placeDynamicShape(*platform, 0.0, world.ground.getYLevel());
        
        // 小球初始静止
        Circle* ball = new Circle(1.0, 3.0, 0.0, 0.0);
        ball->setVelocity(0.0, 0.0);
        ball->setRestitution(0.1);
        world.addDynamicShape(ball);
        
        double platformY = platform->mass_centre[1];
        double ballY = platformY + platform->getHeight()/2 + ball->getRadius();
        ball->setCentre(0.0, ballY);
        
        // 稳定
        for (int i = 0; i < 30; i++) {
            world.update(world.dynamicShapeList, world.ground);
        }
        
        // 模拟50帧
        for (int i = 0; i < 50; i++) {
            world.update(world.dynamicShapeList, world.ground);
        }
        
        double ballVx, ballVy;
        ball->getVelocity(ballVx, ballVy);
        double accelerationRatio = (ballVx / 10.0) * 100.0;
        
        std::cout << std::fixed << std::setprecision(1);
        std::cout << std::setw(12) << friction 
                  << std::setw(15) << 0.0 
                  << std::setw(15) << ballVx 
                  << std::setw(14) << accelerationRatio << "%" << std::endl;
        
        delete ball;
        delete platform;
    }
    
    std::cout << "\n结果分析：" << std::endl;
    std::cout << "  摩擦系数越大，小球加速越快" << std::endl;
    std::cout << "  摩擦系数 = 0.1: 加速较慢" << std::endl;
    std::cout << "  摩擦系数 = 0.9: 加速很快" << std::endl;
}

/*=========================================================================================================
 * 测试5：多个物体在同一平台 - 测试多物体交互
 *=========================================================================================================*/
void test_multiple_objects_on_platform() {
    printTestHeader("测试5：多个物体在同一平台（多物体交互）");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.ground.setFriction(0.2);
    world.start();
    
    std::cout << "场景设置：1个平台，3个小球\n" << std::endl;
    
    // 创建平台
    AABB* platform = new AABB(80.0, 60.0, 10.0, 0.0, 0.0);
    platform->setName("Platform");
    platform->setVelocity(8.0, 0.0);
    platform->setRestitution(0.0);
    platform->fraction = 0.5;
    world.addDynamicShape(platform);
    world.placeDynamicShape(*platform, 0.0, world.ground.getYLevel());
    
    // 创建3个小球，不同初始速度
    Circle* ball1 = new Circle(1.0, 2.5, 0.0, 0.0);
    ball1->setName("Ball1");
    ball1->setVelocity(0.0, 0.0);  // 静止
    ball1->setRestitution(0.1);
    world.addDynamicShape(ball1);
    
    Circle* ball2 = new Circle(1.0, 2.5, 0.0, 0.0);
    ball2->setName("Ball2");
    ball2->setVelocity(4.0, 0.0);  // 4 m/s
    ball2->setRestitution(0.1);
    world.addDynamicShape(ball2);
    
    Circle* ball3 = new Circle(1.0, 2.5, 0.0, 0.0);
    ball3->setName("Ball3");
    ball3->setVelocity(12.0, 0.0);  // 12 m/s
    ball3->setRestitution(0.1);
    world.addDynamicShape(ball3);
    
    // 放置小球
    double platformY = platform->mass_centre[1];
    double ballY = platformY + platform->getHeight()/2 + ball1->getRadius();
    ball1->setCentre(-15.0, ballY);
    ball2->setCentre(0.0, ballY);
    ball3->setCentre(15.0, ballY);
    
    std::cout << "初始状态：" << std::endl;
    std::cout << "  平台速度: 8.0 m/s" << std::endl;
    std::cout << "  Ball1速度: 0.0 m/s（需要加速）" << std::endl;
    std::cout << "  Ball2速度: 4.0 m/s（需要加速）" << std::endl;
    std::cout << "  Ball3速度: 12.0 m/s（需要减速）\n" << std::endl;
    
    // 稳定
    for (int i = 0; i < 30; i++) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    std::cout << "Frame   Ball1速度   Ball2速度   Ball3速度   平台速度" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (int i = 1; i <= 60; i++) {
        world.update(world.dynamicShapeList, world.ground);
        
        if (i % 15 == 0) {
            double v1x, v1y, v2x, v2y, v3x, v3y, pvx, pvy;
            ball1->getVelocity(v1x, v1y);
            ball2->getVelocity(v2x, v2y);
            ball3->getVelocity(v3x, v3y);
            platform->getVelocity(pvx, pvy);
            
            std::cout << std::fixed << std::setprecision(3);
            std::cout << std::setw(5) << i 
                      << std::setw(12) << v1x 
                      << std::setw(12) << v2x 
                      << std::setw(12) << v3x
                      << std::setw(12) << pvx << std::endl;
        }
    }
    
    double v1x, v1y, v2x, v2y, v3x, v3y;
    ball1->getVelocity(v1x, v1y);
    ball2->getVelocity(v2x, v2y);
    ball3->getVelocity(v3x, v3y);
    
    std::cout << "\n结果验证：" << std::endl;
    std::cout << "  Ball1: 0.0 → " << v1x << " m/s（应增加）" << std::endl;
    std::cout << "  Ball2: 4.0 → " << v2x << " m/s（应增加）" << std::endl;
    std::cout << "  Ball3: 12.0 → " << v3x << " m/s（应减少）" << std::endl;
    
    bool test1 = v1x > 3.0;
    bool test2 = v2x > 4.0;
    bool test3 = v3x < 12.0;
    
    if (test1 && test2 && test3) {
        std::cout << "\n? 测试通过！所有球都向平台速度靠拢" << std::endl;
    } else {
        std::cout << "\n??  部分球的速度变化不符合预期" << std::endl;
    }
    
    delete ball1;
    delete ball2;
    delete ball3;
    delete platform;
}

/*=========================================================================================================
 * 测试6：堆叠物体在运动平台 - 测试复合场景
 *=========================================================================================================*/
void test_stacked_objects_on_platform() {
    printTestHeader("测试6：堆叠物体在运动平台（复合场景）");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.ground.setFriction(0.3);
    world.start();
    
    std::cout << "场景设置：2个球堆叠在运动平台上\n" << std::endl;
    
    // 创建平台
    AABB* platform = new AABB(100.0, 50.0, 10.0, 0.0, 0.0);
    platform->setName("Platform");
    platform->setVelocity(6.0, 0.0);
    platform->setRestitution(0.0);
    platform->fraction = 0.6;
    world.addDynamicShape(platform);
    world.placeDynamicShape(*platform, 0.0, world.ground.getYLevel());
    
    // 创建底部小球
    Circle* ball1 = new Circle(1.0, 4.0, 0.0, 0.0);
    ball1->setName("Ball1_Bottom");
    ball1->setVelocity(0.0, 0.0);
    ball1->setRestitution(0.0);
    world.addDynamicShape(ball1);
    
    // 创建顶部小球
    Circle* ball2 = new Circle(1.0, 4.0, 0.0, 0.0);
    ball2->setName("Ball2_Top");
    ball2->setVelocity(0.0, 0.0);
    ball2->setRestitution(0.0);
    world.addDynamicShape(ball2);
    
    // 放置球（精确堆叠）
    double platformY = platform->mass_centre[1];
    double ball1Y = platformY + platform->getHeight()/2 + ball1->getRadius();
    double ball2Y = ball1Y + ball1->getRadius() + ball2->getRadius();
    
    ball1->setCentre(0.0, ball1Y);
    ball2->setCentre(0.0, ball2Y);
    
    std::cout << "初始状态：" << std::endl;
    std::cout << "  平台速度: 6.0 m/s" << std::endl;
    std::cout << "  Ball1速度: 0.0 m/s" << std::endl;
    std::cout << "  Ball2速度: 0.0 m/s\n" << std::endl;
    
    // 稳定
    std::cout << "稳定阶段（50帧）..." << std::endl;
    for (int i = 0; i < 50; i++) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    // 检查支撑关系
    bool ball1OnPlatform = (ball1->getSupporter() == platform);
    bool ball2OnBall1 = (ball2->getSupporter() == ball1);
    
    std::cout << "\n支撑关系：" << std::endl;
    std::cout << "  Ball1 -> " << (ball1->getSupporter() ? ball1->getSupporter()->getName() : "无") << std::endl;
    std::cout << "  Ball2 -> " << (ball2->getSupporter() ? ball2->getSupporter()->getName() : "无") << std::endl;
    
    if (ball1OnPlatform && ball2OnBall1) {
        std::cout << "? 堆叠成功\n" << std::endl;
    } else {
        std::cout << "??  堆叠可能失败\n" << std::endl;
    }
    
    std::cout << "观察过程：" << std::endl;
    std::cout << "Frame   平台速度    Ball1速度   Ball2速度" << std::endl;
    std::cout << std::string(55, '-') << std::endl;
    
    for (int i = 1; i <= 60; i++) {
        world.update(world.dynamicShapeList, world.ground);
        
        if (i % 15 == 0) {
            double pvx, pvy, v1x, v1y, v2x, v2y;
            platform->getVelocity(pvx, pvy);
            ball1->getVelocity(v1x, v1y);
            ball2->getVelocity(v2x, v2y);
            
            std::cout << std::fixed << std::setprecision(3);
            std::cout << std::setw(5) << i 
                      << std::setw(12) << pvx 
                      << std::setw(12) << v1x 
                      << std::setw(12) << v2x << std::endl;
        }
    }
    
    double v1x, v1y, v2x, v2y;
    ball1->getVelocity(v1x, v1y);
    ball2->getVelocity(v2x, v2y);
    
    std::cout << "\n结果验证：" << std::endl;
    std::cout << "  Ball1最终速度: " << v1x << " m/s" << std::endl;
    std::cout << "  Ball2最终速度: " << v2x << " m/s" << std::endl;
    
    if (v1x > 2.0 && v2x > 1.0) {
        std::cout << "\n? 测试通过！堆叠的球都被平台加速" << std::endl;
    } else {
        std::cout << "\n??  堆叠的球加速不足" << std::endl;
    }
    
    delete ball1;
    delete ball2;
    delete platform;
}

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "     板块摩擦力测试程序" << std::endl;
    std::cout << "============================================" << std::endl;
    
    try {
        test_ball_on_moving_platform();
        test_moving_ball_on_static_platform();
        test_opposite_direction_velocities();
        test_different_platform_friction();
        test_multiple_objects_on_platform();
        test_stacked_objects_on_platform();
        
        std::cout << "\n============================================" << std::endl;
        std::cout << "  ? 所有板块摩擦力测试完成！" << std::endl;
        std::cout << "============================================" << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n? 测试失败: " << e.what() << std::endl;
        return 1;
    }
}
