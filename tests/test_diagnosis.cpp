/*=========================================================================================================
 * 诊断测试 - 检查 test_physicalWorld 中失败的测试
 * 
 * 失败的测试：
 * 1. 测试3：自由落体模拟 - 物体没有落到地面
 * 2. 测试4：倾斜平面滑动 - 物体没有沿斜面滑动
 * 3. 测试5：碰撞检测 - 碰撞后速度没有反向
 * 4. 测试6：堆叠物体 - 支撑关系和正压力计算不正确
 *=========================================================================================================*/

#include <iostream>
#include <iomanip>
#include <cmath>
#include "physicalWorld.h"
#include "shapes.h"

void printSeparator() {
    std::cout << "\n========================================\n";
}

// 诊断测试3：自由落体
void diagnose_free_fall() {
    printSeparator();
    std::cout << "诊断测试3：自由落体模拟\n";
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setTimeStep(1.0 / 60.0);
    
    // 创建地面
    world.ground.setYLevel(0.0);
    
    // 创建小球，初始位置在 Y=105
    Circle* ball = new Circle(1.0, 5.0, 0.0, 105.0);
    ball->setName("Ball");
    world.addDynamicShape(ball);
    
    std::cout << "初始状态：" << std::endl;
    double x, y, vx, vy;
    ball->getCentre(x, y);
    ball->getVelocity(vx, vy);
    std::cout << "  位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "  速度: (" << vx << ", " << vy << ")" << std::endl;
    std::cout << "  底部Y: " << ball->getBottom() << std::endl;
    std::cout << "  地面Y: " << world.ground.getYLevel() << std::endl;
    std::cout << "  是否被支撑: " << (ball->getIsSupported() ? "是" : "否") << std::endl;
    
    // 运行 60 帧（1秒）
    for (int i = 0; i < 60; ++i) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    std::cout << "\n1秒后状态：" << std::endl;
    ball->getCentre(x, y);
    ball->getVelocity(vx, vy);
    std::cout << "  位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "  速度: (" << vx << ", " << vy << ")" << std::endl;
    std::cout << "  底部Y: " << ball->getBottom() << std::endl;
    std::cout << "  是否被支撑: " << (ball->getIsSupported() ? "是" : "否") << std::endl;
    
    // 检查是否碰撞地面
    bool hasCollided = ball->HasCollidedWithGround(world.ground.getYLevel());
    std::cout << "  HasCollidedWithGround: " << (hasCollided ? "是" : "否") << std::endl;
    
    // 分析
    std::cout << "\n分析：" << std::endl;
    if (ball->getBottom() <= world.ground.getYLevel()) {
        std::cout << "  物体底部已到达地面 ?" << std::endl;
    } else {
        std::cout << "  物体底部还没到地面 ?" << std::endl;
        std::cout << "  距离: " << (ball->getBottom() - world.ground.getYLevel()) << " m" << std::endl;
    }
    
    if (std::abs(vy) < 0.1) {
        std::cout << "  垂直速度接近0 ?" << std::endl;
    } else {
        std::cout << "  垂直速度还很大 ? (" << vy << " m/s)" << std::endl;
    }
    
    delete ball;
}

// 诊断测试4：倾斜平面滑动
void diagnose_incline_sliding() {
    printSeparator();
    std::cout << "诊断测试4：倾斜平面滑动\n";
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(30.0);
    world.ground.setFriction(0.1);
    world.setTimeStep(1.0 / 60.0);
    
    // 创建小球
    Circle* ball = new Circle(1.0, 5.0, 0.0, 25.0);
    ball->setName("Ball");
    world.addDynamicShape(ball);
    world.placeShapeOnGround(*ball, world.ground);
    
    std::cout << "初始状态 (30° 斜坡, 摩擦系数 0.1)：" << std::endl;
    double x, y, vx, vy;
    ball->getCentre(x, y);
    ball->getVelocity(vx, vy);
    std::cout << "  位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "  速度: (" << vx << ", " << vy << ")" << std::endl;
    std::cout << "  是否被支撑: " << (ball->getIsSupported() ? "是" : "否") << std::endl;
    
    // 运行 120 帧（2秒）
    for (int i = 0; i < 120; ++i) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    std::cout << "\n2秒后状态：" << std::endl;
    ball->getCentre(x, y);
    ball->getVelocity(vx, vy);
    std::cout << "  位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "  速度: (" << vx << ", " << vy << ")" << std::endl;
    std::cout << "  是否被支撑: " << (ball->getIsSupported() ? "是" : "否") << std::endl;
    
    // 理论计算
    const double PI = 3.14159265358979323846;
    double angle_rad = 30.0 * PI / 180.0;
    double expected_a = 10.0 * (std::sin(angle_rad) - 0.1 * std::cos(angle_rad));
    double expected_v = expected_a * 2.0;
    
    std::cout << "\n理论分析：" << std::endl;
    std::cout << "  理论加速度: " << expected_a << " m/s?" << std::endl;
    std::cout << "  理论末速度: " << expected_v << " m/s" << std::endl;
    std::cout << "  实际水平速度: " << vx << " m/s" << std::endl;
    
    if (std::abs(vx) > 1.0) {
        std::cout << "  物体沿斜面滑动 ?" << std::endl;
    } else {
        std::cout << "  物体没有沿斜面滑动 ?" << std::endl;
    }
    
    delete ball;
}

// 诊断测试5：碰撞检测
void diagnose_collision() {
    printSeparator();
    std::cout << "诊断测试5：碰撞检测\n";
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setTimeStep(1.0 / 60.0);
    
    // 创建两个小球，相向运动
    Circle* ball1 = new Circle(1.0, 5.0, 0.0, 55.0, 50.0, 0.0);
    Circle* ball2 = new Circle(1.0, 5.0, 100.0, 55.0, -50.0, 0.0);
    ball1->setName("Ball1");
    ball2->setName("Ball2");
    world.addDynamicShape(ball1);
    world.addDynamicShape(ball2);
    
    std::cout << "初始状态：" << std::endl;
    double x1, y1, vx1, vy1, x2, y2, vx2, vy2;
    ball1->getCentre(x1, y1);
    ball1->getVelocity(vx1, vy1);
    ball2->getCentre(x2, y2);
    ball2->getVelocity(vx2, vy2);
    std::cout << "  Ball1: 位置=(" << x1 << ", " << y1 << "), 速度=(" << vx1 << ", " << vy1 << ")" << std::endl;
    std::cout << "  Ball2: 位置=(" << x2 << ", " << y2 << "), 速度=(" << vx2 << ", " << vy2 << ")" << std::endl;
    
    // 运行直到碰撞
    bool collided = false;
    int collision_frame = -1;
    for (int i = 0; i < 200; ++i) {
        world.update(world.dynamicShapeList, world.ground);
        
        ball1->getCentre(x1, y1);
        ball2->getCentre(x2, y2);
        double distance = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        
        if (distance < (ball1->getRadius() + ball2->getRadius()) && !collided) {
            collided = true;
            collision_frame = i;
            std::cout << "\n碰撞发生在第 " << i << " 帧" << std::endl;
            break;
        }
    }
    
    ball1->getCentre(x1, y1);
    ball1->getVelocity(vx1, vy1);
    ball2->getCentre(x2, y2);
    ball2->getVelocity(vx2, vy2);
    
    std::cout << "\n碰撞后状态：" << std::endl;
    std::cout << "  Ball1: 位置=(" << x1 << ", " << y1 << "), 速度=(" << vx1 << ", " << vy1 << ")" << std::endl;
    std::cout << "  Ball2: 位置=(" << x2 << ", " << y2 << "), 速度=(" << vx2 << ", " << vy2 << ")" << std::endl;
    
    std::cout << "\n分析：" << std::endl;
    if (collided) {
        std::cout << "  检测到碰撞 ?" << std::endl;
    } else {
        std::cout << "  没有检测到碰撞 ?" << std::endl;
    }
    
    if (vx1 < 0 && vx2 > 0) {
        std::cout << "  碰撞后速度反向 ?" << std::endl;
    } else {
        std::cout << "  碰撞后速度没有反向 ?" << std::endl;
        std::cout << "  Ball1 vx: " << vx1 << " (应该<0)" << std::endl;
        std::cout << "  Ball2 vx: " << vx2 << " (应该>0)" << std::endl;
    }
    
    delete ball1;
    delete ball2;
}

// 诊断测试6：堆叠物体
void diagnose_stacking() {
    printSeparator();
    std::cout << "诊断测试6：堆叠物体\n";
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setTimeStep(1.0 / 60.0);
    
    // 创建三个堆叠的盒子
    AABB* box1 = new AABB(2.0, 10.0, 10.0, 0.0, 10.0);
    AABB* box2 = new AABB(1.5, 10.0, 10.0, 0.0, 30.0);
    AABB* box3 = new AABB(1.0, 10.0, 10.0, 0.0, 50.0);
    
    box1->setName("Box1");
    box2->setName("Box2");
    box3->setName("Box3");
    
    world.addDynamicShape(box1);
    world.addDynamicShape(box2);
    world.addDynamicShape(box3);
    
    std::cout << "初始质量：" << std::endl;
    std::cout << "  Box1: " << box1->getMass() << " kg" << std::endl;
    std::cout << "  Box2: " << box2->getMass() << " kg" << std::endl;
    std::cout << "  Box3: " << box3->getMass() << " kg" << std::endl;
    
    // 运行一段时间让物体稳定
    for (int i = 0; i < 300; ++i) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    std::cout << "\n稳定后支撑关系：" << std::endl;
    std::cout << "  Box1 被支撑: " << (box1->getIsSupported() ? "是" : "否");
    if (box1->getSupporter()) {
        std::cout << " (支撑物: " << box1->getSupporter()->getName() << ")";
    }
    std::cout << std::endl;
    
    std::cout << "  Box2 被支撑: " << (box2->getIsSupported() ? "是" : "否");
    if (box2->getSupporter()) {
        std::cout << " (支撑物: " << box2->getSupporter()->getName() << ")";
    }
    std::cout << std::endl;
    
    std::cout << "  Box3 被支撑: " << (box3->getIsSupported() ? "是" : "否");
    if (box3->getSupporter()) {
        std::cout << " (支撑物: " << box3->getSupporter()->getName() << ")";
    }
    std::cout << std::endl;
    
    double fx1, fy1, fx2, fy2, fx3, fy3;
    box1->getNormalForce(fx1, fy1);
    box2->getNormalForce(fx2, fy2);
    box3->getNormalForce(fx3, fy3);
    
    std::cout << "\n正压力计算：" << std::endl;
    std::cout << "  Box1 正压力: " << std::abs(fy1) << " N (应约等于 " << (2.0 + 1.5 + 1.0) * 10 << " N)" << std::endl;
    std::cout << "  Box2 正压力: " << std::abs(fy2) << " N (应约等于 " << (1.5 + 1.0) * 10 << " N)" << std::endl;
    std::cout << "  Box3 正压力: " << std::abs(fy3) << " N (应约等于 " << 1.0 * 10 << " N)" << std::endl;
    
    std::cout << "\n位置：" << std::endl;
    double x1, y1, x2, y2, x3, y3;
    box1->getCentre(x1, y1);
    box2->getCentre(x2, y2);
    box3->getCentre(x3, y3);
    std::cout << "  Box1: (" << x1 << ", " << y1 << "), 底部: " << box1->getBottom() << ", 顶部: " << box1->getTop() << std::endl;
    std::cout << "  Box2: (" << x2 << ", " << y2 << "), 底部: " << box2->getBottom() << ", 顶部: " << box2->getTop() << std::endl;
    std::cout << "  Box3: (" << x3 << ", " << y3 << "), 底部: " << box3->getBottom() << ", 顶部: " << box3->getTop() << std::endl;
    
    delete box1;
    delete box2;
    delete box3;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  test_physicalWorld 失败测试诊断" << std::endl;
    std::cout << "========================================" << std::endl;
    
    diagnose_free_fall();
    diagnose_incline_sliding();
    diagnose_collision();
    diagnose_stacking();
    
    printSeparator();
    std::cout << "诊断完成" << std::endl;
    printSeparator();
    
    return 0;
}
