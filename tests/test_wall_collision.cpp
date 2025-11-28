/*=========================================================================================================
 * 测试程序：测试小球和滑块的撞墙效果（改进版）
 * 
 * 测试场景：
 * 1. 小球撞向左侧墙壁 - 水平碰撞
 * 2. 小球撞向右侧墙壁 - 水平碰撞  
 * 3. 滑块撞向墙壁 - 水平碰撞
 * 4. 小球多次弹跳测试
 *=========================================================================================================*/

#include "physicalWorld.h"
#include "shapes.h"
#include <iostream>
#include <iomanip>
#include <cmath>

void printTestHeader(const std::string& testName) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  " << testName << std::endl;
    std::cout << "========================================" << std::endl;
}

void printShapeState(const std::string& name, Shape* shape) {
    double x, y, vx, vy;
    shape->getCentre(x, y);
    shape->getVelocity(vx, vy);
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << name << " - "
              << "位置: (" << std::setw(7) << x << ", " << std::setw(7) << y << "), "
              << "速度: (" << std::setw(7) << vx << ", " << std::setw(7) << vy << ")"
              << std::endl;
}

/*=========================================================================================================
 * 测试1：小球撞向左侧墙壁（水平碰撞）
 *=========================================================================================================*/
void test_circle_hits_left_wall() {
    printTestHeader("测试1：小球撞向左侧墙壁");
    
    PhysicalWorld world;
    world.setGravity(0.0);  // 关闭重力
    world.start();
    
    // 创建左侧墙壁
    Wall* leftWall = world.placeWall("LeftWall", -50.0, 0.0, 10.0, 200.0, 0.0);
    std::cout << "墙壁位置: x = -50, 左边缘 = " << leftWall->getLeft() << ", 右边缘 = " << leftWall->getRight() << std::endl;
    
    // 创建小球，放在墙壁附近
    Circle* ball = new Circle(1.0, 5.0, -30.0, 0.0);  // 距离墙壁15单位
    ball->setName("Ball");
    ball->setVelocity(-10.0, 0.0);  // 向左运动
    ball->setRestitution(0.8);
    world.addDynamicShape(ball);
    
    std::cout << "小球半径: " << ball->getRadius() << std::endl;
    std::cout << "小球初始速度: vx = -10.0 m/s（向左）" << std::endl;
    std::cout << "弹性系数: 0.8\n" << std::endl;
    
    std::cout << "模拟过程：" << std::endl;
    std::cout << "初始状态: "; printShapeState("Ball", ball);
    
    // 模拟直到发生碰撞
    bool collided = false;
    for (int i = 0; i < 50; i++) {
        // 每帧更新
        ball->setCentre(ball->mass_centre[0] + ball->velocity[0] * 0.1, 
                       ball->mass_centre[1] + ball->velocity[1] * 0.1);
        
        // 检测碰撞
        if (ball->check_collision(*leftWall)) {
            if (!collided) {
                std::cout << "\n[碰撞发生！Frame " << i << "]" << std::endl;
                std::cout << "碰撞前: "; printShapeState("Ball", ball);
                
                // 处理碰撞
                world.handleWallCollision(*ball, *leftWall);
                
                std::cout << "碰撞后: "; printShapeState("Ball", ball);
                collided = true;
            }
        }
        
        // 打印每10帧的状态
        if (i % 10 == 0 && i > 0) {
            std::cout << "Frame " << std::setw(2) << i << ": "; 
            printShapeState("Ball", ball);
        }
    }
    
    // 验证结果
    double vx, vy;
    ball->getVelocity(vx, vy);
    std::cout << "\n结果验证：" << std::endl;
    if (vx > 0 && collided) {
        std::cout << "  ? 小球成功反弹（vx > 0）" << std::endl;
    } else if (!collided) {
        std::cout << "  ? 未检测到碰撞" << std::endl;
    } else {
        std::cout << "  ? 速度未正确反向（vx = " << vx << "）" << std::endl;
    }
    
    delete ball;
}

/*=========================================================================================================
 * 测试2：小球撞向右侧墙壁
 *=========================================================================================================*/
void test_circle_hits_right_wall() {
    printTestHeader("测试2：小球撞向右侧墙壁");
    
    PhysicalWorld world;
    world.setGravity(0.0);
    world.start();
    
    Wall* rightWall = world.placeWall("RightWall", 50.0, 0.0, 10.0, 200.0, 0.0);
    std::cout << "墙壁位置: x = 50, 左边缘 = " << rightWall->getLeft() << ", 右边缘 = " << rightWall->getRight() << std::endl;
    
    Circle* ball = new Circle(1.0, 5.0, 30.0, 0.0);
    ball->setName("Ball");
    ball->setVelocity(10.0, 0.0);  // 向右运动
    ball->setRestitution(0.8);
    world.addDynamicShape(ball);
    
    std::cout << "小球初始速度: vx = 10.0 m/s（向右）\n" << std::endl;
    std::cout << "初始状态: "; printShapeState("Ball", ball);
    
    bool collided = false;
    for (int i = 0; i < 50; i++) {
        ball->setCentre(ball->mass_centre[0] + ball->velocity[0] * 0.1, 
                       ball->mass_centre[1] + ball->velocity[1] * 0.1);
        
        if (ball->check_collision(*rightWall)) {
            if (!collided) {
                std::cout << "\n[碰撞发生！Frame " << i << "]" << std::endl;
                std::cout << "碰撞前: "; printShapeState("Ball", ball);
                world.handleWallCollision(*ball, *rightWall);
                std::cout << "碰撞后: "; printShapeState("Ball", ball);
                collided = true;
            }
        }
        
        if (i % 10 == 0 && i > 0) {
            std::cout << "Frame " << std::setw(2) << i << ": "; 
            printShapeState("Ball", ball);
        }
    }
    
    double vx, vy;
    ball->getVelocity(vx, vy);
    std::cout << "\n结果验证：" << std::endl;
    if (vx < 0 && collided) {
        std::cout << "  ? 小球成功反弹（vx < 0）" << std::endl;
    } else if (!collided) {
        std::cout << "  ? 未检测到碰撞" << std::endl;
    } else {
        std::cout << "  ? 速度未正确反向（vx = " << vx << "）" << std::endl;
    }
    
    delete ball;
}

/*=========================================================================================================
 * 测试3：滑块撞向墙壁
 *=========================================================================================================*/
void test_aabb_hits_wall() {
    printTestHeader("测试3：滑块（AABB）撞向墙壁");
    
    PhysicalWorld world;
    world.setGravity(0.0);
    world.start();
    
    Wall* wall = world.placeWall("Wall", 50.0, 0.0, 10.0, 200.0, 0.0);
    std::cout << "墙壁位置: x = 50" << std::endl;
    
    AABB* box = new AABB(1.0, 10.0, 10.0, 20.0, 0.0);
    box->setName("Box");
    box->setVelocity(15.0, 0.0);
    box->setRestitution(0.6);
    world.addDynamicShape(box);
    
    std::cout << "滑块尺寸: " << box->getWidth() << " x " << box->getHeight() << std::endl;
    std::cout << "滑块初始速度: vx = 15.0 m/s\n" << std::endl;
    std::cout << "初始状态: "; printShapeState("Box", box);
    
    bool collided = false;
    for (int i = 0; i < 50; i++) {
        box->setCentre(box->mass_centre[0] + box->velocity[0] * 0.1, 
                      box->mass_centre[1] + box->velocity[1] * 0.1);
        
        if (box->check_collision(*wall)) {
            if (!collided) {
                std::cout << "\n[碰撞发生！Frame " << i << "]" << std::endl;
                std::cout << "碰撞前: "; printShapeState("Box", box);
                world.handleWallCollision(*box, *wall);
                std::cout << "碰撞后: "; printShapeState("Box", box);
                collided = true;
            }
        }
        
        if (i % 10 == 0 && i > 0) {
            std::cout << "Frame " << std::setw(2) << i << ": "; 
            printShapeState("Box", box);
        }
    }
    
    double vx, vy;
    box->getVelocity(vx, vy);
    std::cout << "\n结果验证：" << std::endl;
    if (vx < 0 && collided) {
        std::cout << "  ? 滑块成功反弹（vx < 0）" << std::endl;
    } else if (!collided) {
        std::cout << "  ? 未检测到碰撞" << std::endl;
    } else {
        std::cout << "  ? 速度未正确反向（vx = " << vx << "）" << std::endl;
    }
    
    delete box;
}

/*=========================================================================================================
 * 测试4：小球在两墙之间多次弹跳
 *=========================================================================================================*/
void test_multiple_bounces() {
    printTestHeader("测试4：小球在两墙之间多次弹跳");
    
    PhysicalWorld world;
    world.setGravity(0.0);
    world.start();
    
    Wall* leftWall = world.placeWall("LeftWall", -50.0, 0.0, 5.0, 200.0, 0.0);
    Wall* rightWall = world.placeWall("RightWall", 50.0, 0.0, 5.0, 200.0, 0.0);
    
    std::cout << "左墙: x = -50 (左边缘: " << leftWall->getLeft() << ")" << std::endl;
    std::cout << "右墙: x = 50 (右边缘: " << rightWall->getRight() << ")" << std::endl;
    
    Circle* ball = new Circle(1.0, 3.0, 0.0, 0.0);
    ball->setName("Ball");
    ball->setVelocity(8.0, 0.0);
    ball->setRestitution(0.9);
    world.addDynamicShape(ball);
    
    std::cout << "小球半径: " << ball->getRadius() << std::endl;
    std::cout << "初始速度: vx = 8.0 m/s" << std::endl;
    std::cout << "弹性系数: 0.9\n" << std::endl;
    
    int bounceCount = 0;
    double lastVx = 8.0;
    
    for (int i = 0; i < 200; i++) {
        // 手动更新位置
        ball->setCentre(ball->mass_centre[0] + ball->velocity[0] * 0.05, 
                       ball->mass_centre[1]);
        
        // 检测并处理碰撞
        bool collisionThisFrame = false;
        
        if (ball->check_collision(*leftWall)) {
            world.handleWallCollision(*ball, *leftWall);
            collisionThisFrame = true;
        }
        
        if (ball->check_collision(*rightWall)) {
            world.handleWallCollision(*ball, *rightWall);
            collisionThisFrame = true;
        }
        
        // 检测速度反向
        double vx, vy;
        ball->getVelocity(vx, vy);
        
        if ((lastVx > 0 && vx < 0) || (lastVx < 0 && vx > 0)) {
            bounceCount++;
            std::cout << "[弹跳 " << bounceCount << "] Frame " << i 
                     << " - 位置: x = " << std::fixed << std::setprecision(2) << ball->mass_centre[0]
                     << ", 速度: vx = " << vx << std::endl;
            
            if (bounceCount >= 5) break;  // 记录5次弹跳后停止
        }
        
        lastVx = vx;
    }
    
    std::cout << "\n结果统计：" << std::endl;
    std::cout << "  总弹跳次数: " << bounceCount << std::endl;
    
    if (bounceCount >= 3) {
        std::cout << "  ? 成功实现多次弹跳" << std::endl;
    } else {
        std::cout << "  ??  弹跳次数: " << bounceCount << " (期望 >= 3)" << std::endl;
    }
    
    delete ball;
}

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "  墙壁碰撞效果测试程序" << std::endl;
    std::cout << "  测试小球和滑块的撞墙效果" << std::endl;
    std::cout << "============================================" << std::endl;
    
    try {
        test_circle_hits_left_wall();
        test_circle_hits_right_wall();
        test_aabb_hits_wall();
        test_multiple_bounces();
        
        std::cout << "\n============================================" << std::endl;
        std::cout << "  ? 所有墙壁碰撞测试完成！" << std::endl;
        std::cout << "============================================" << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n? 测试失败: " << e.what() << std::endl;
        return 1;
    }
}
