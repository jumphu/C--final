/*=========================================================================================================
 * 堆叠物体摩擦力测试 - 修复版
 * 
 * 修复内容：
 * 1. 让物体先静止堆叠
 * 2. 准确计算堆叠位置
 * 3. 等待稳定后再测试摩擦力
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
 * 测试：正确的堆叠物体摩擦力测试
 *=========================================================================================================*/
void test_stacked_objects_friction_fixed() {
    printTestHeader("堆叠物体摩擦力测试（修复版）");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.ground.setFriction(0.4);
    world.start();
    
    std::cout << "修复方案：" << std::endl;
    std::cout << "  1. 物体初始静止" << std::endl;
    std::cout << "  2. 准确计算堆叠位置" << std::endl;
    std::cout << "  3. 等待稳定后给整体速度" << std::endl;
    std::cout << "  4. 弹性系数设为0（避免反弹）\n" << std::endl;
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  地面摩擦系数: 0.4" << std::endl;
    std::cout << "  每个球质量: 1.0 kg" << std::endl;
    std::cout << "  每个球半径: 5.0 m\n" << std::endl;
    
    // 创建底部小球（在地面上）
    Circle* ball1 = new Circle(1.0, 5.0, 0.0, 0.0);
    ball1->setName("Ball1_Bottom");
    ball1->setVelocity(0.0, 0.0);  // 初始静止
    ball1->setRestitution(0.0);    // 无弹性
    world.addDynamicShape(ball1);
    world.placeDynamicShape(*ball1, 0.0, world.ground.getYLevel());
    
    // 创建中间小球（精确堆叠在Ball1上）
    Circle* ball2 = new Circle(1.0, 5.0, 0.0, 0.0);
    ball2->setName("Ball2_Middle");
    ball2->setVelocity(0.0, 0.0);  // 初始静止
    ball2->setRestitution(0.0);
    world.addDynamicShape(ball2);
    
    // 计算Ball2的准确位置：Ball1中心 + Ball1半径 + Ball2半径
    double ball1_y = ball1->mass_centre[1];  // 5.0
    double ball2_y = ball1_y + ball1->getRadius() + ball2->getRadius();  // 5 + 5 + 5 = 15
    ball2->setCentre(0.0, ball2_y);
    
    // 创建顶部小球（精确堆叠在Ball2上）
    Circle* ball3 = new Circle(1.0, 5.0, 0.0, 0.0);
    ball3->setName("Ball3_Top");
    ball3->setVelocity(0.0, 0.0);  // 初始静止
    ball3->setRestitution(0.0);
    world.addDynamicShape(ball3);
    
    // 计算Ball3的准确位置
    double ball3_y = ball2_y + ball2->getRadius() + ball3->getRadius();  // 15 + 5 + 5 = 25
    ball3->setCentre(0.0, ball3_y);
    
    std::cout << "初始位置（理论值）：" << std::endl;
    std::cout << "  Ball1: y = " << ball1_y << " (地面上)" << std::endl;
    std::cout << "  Ball2: y = " << ball2_y << " (Ball1上)" << std::endl;
    std::cout << "  Ball3: y = " << ball3_y << " (Ball2上)" << std::endl;
    
    std::cout << "\n第一阶段：让物体自然堆叠（50帧静止模拟）" << std::endl;
    
    // 第一阶段：让物体静止堆叠50帧
    for (int i = 1; i <= 50; i++) {
        world.update(world.dynamicShapeList, world.ground);
        
        if (i == 10 || i == 30 || i == 50) {
            std::cout << "\n--- Frame " << i << " ---" << std::endl;
            printShapeState("Ball1", ball1);
            printShapeState("Ball2", ball2);
            printShapeState("Ball3", ball3);
            
            // 检查支撑关系
            std::cout << "  支撑关系: ";
            std::cout << "Ball1->" << (ball1->getSupporter() ? ball1->getSupporter()->getName() : "地面");
            std::cout << ", Ball2->" << (ball2->getSupporter() ? ball2->getSupporter()->getName() : "无");
            std::cout << ", Ball3->" << (ball3->getSupporter() ? ball3->getSupporter()->getName() : "无");
            std::cout << std::endl;
            
            // 打印正压力
            double fx1, fy1, fx2, fy2, fx3, fy3;
            ball1->getNormalForce(fx1, fy1);
            ball2->getNormalForce(fx2, fy2);
            ball3->getNormalForce(fx3, fy3);
            
            std::cout << "  正压力: Ball1=" << std::fixed << std::setprecision(1) << std::abs(fy1) 
                     << "N, Ball2=" << std::abs(fy2) 
                     << "N, Ball3=" << std::abs(fy3) << "N" << std::endl;
        }
    }
    
    // 验证堆叠是否成功
    bool isStacked = false;
    if (ball2->getSupporter() == ball1 && ball3->getSupporter() == ball2) {
        std::cout << "\n? 堆叠成功！支撑关系正确建立" << std::endl;
        isStacked = true;
    } else {
        std::cout << "\n??  堆叠可能失败" << std::endl;
        std::cout << "  Ball2支撑者: " << (ball2->getSupporter() ? ball2->getSupporter()->getName() : "无") << std::endl;
        std::cout << "  Ball3支撑者: " << (ball3->getSupporter() ? ball3->getSupporter()->getName() : "无") << std::endl;
    }
    
    // 检查正压力
    double fx1, fy1, fx2, fy2, fx3, fy3;
    ball1->getNormalForce(fx1, fy1);
    ball2->getNormalForce(fx2, fy2);
    ball3->getNormalForce(fx3, fy3);
    
    double expectedF1 = 3.0 * 1.0 * 9.8;  // 29.4 N
    double expectedF2 = 2.0 * 1.0 * 9.8;  // 19.6 N
    double expectedF3 = 1.0 * 1.0 * 9.8;  // 9.8 N
    
    std::cout << "\n正压力验证：" << std::endl;
    std::cout << "  Ball1: " << std::abs(fy1) << " N (期望: " << expectedF1 << " N)" << std::endl;
    std::cout << "  Ball2: " << std::abs(fy2) << " N (期望: " << expectedF2 << " N)" << std::endl;
    std::cout << "  Ball3: " << std::abs(fy3) << " N (期望: " << expectedF3 << " N)" << std::endl;
    
    bool normalForceCorrect = true;
    if (std::abs(std::abs(fy1) - expectedF1) < 1.0) {
        std::cout << "  ? Ball1正压力正确" << std::endl;
    } else {
        std::cout << "  ? Ball1正压力错误" << std::endl;
        normalForceCorrect = false;
    }
    
    if (std::abs(std::abs(fy2) - expectedF2) < 1.0) {
        std::cout << "  ? Ball2正压力正确" << std::endl;
    } else {
        std::cout << "  ? Ball2正压力错误" << std::endl;
        normalForceCorrect = false;
    }
    
    if (std::abs(std::abs(fy3) - expectedF3) < 1.0) {
        std::cout << "  ? Ball3正压力正确" << std::endl;
    } else {
        std::cout << "  ? Ball3正压力错误" << std::endl;
        normalForceCorrect = false;
    }
    
    // 第二阶段：给整个堆叠一个水平速度，测试摩擦力
    std::cout << "\n第二阶段：给整个堆叠施加水平速度 8.0 m/s" << std::endl;
    
    ball1->setVelocity(8.0, 0.0);
    ball2->setVelocity(8.0, 0.0);
    ball3->setVelocity(8.0, 0.0);
    
    std::cout << "\n模拟过程（每20帧显示一次）：" << std::endl;
    
    for (int i = 1; i <= 80; i++) {
        world.update(world.dynamicShapeList, world.ground);
        
        if (i % 20 == 0) {
            std::cout << "\n--- Frame " << i << " ---" << std::endl;
            printShapeState("Ball1", ball1);
            printShapeState("Ball2", ball2);
            printShapeState("Ball3", ball3);
            
            ball1->getNormalForce(fx1, fy1);
            ball2->getNormalForce(fx2, fy2);
            ball3->getNormalForce(fx3, fy3);
            
            std::cout << "  正压力: Ball1=" << std::abs(fy1) 
                     << ", Ball2=" << std::abs(fy2) 
                     << ", Ball3=" << std::abs(fy3) << std::endl;
        }
    }
    
    // 最终结果
    double vx1, vy1, vx2, vy2, vx3, vy3;
    ball1->getVelocity(vx1, vy1);
    ball2->getVelocity(vx2, vy2);
    ball3->getVelocity(vx3, vy3);
    
    std::cout << "\n最终结果验证：" << std::endl;
    std::cout << "  Ball1最终速度: " << vx1 << " m/s" << std::endl;
    std::cout << "  Ball2最终速度: " << vx2 << " m/s" << std::endl;
    std::cout << "  Ball3最终速度: " << vx3 << " m/s" << std::endl;
    
    // 理论上，底部球因为承受更大正压力，应该减速更快
    if (isStacked && normalForceCorrect) {
        if (vx1 < vx2 && vx2 < vx3) {
            std::cout << "\n? 测试通过！" << std::endl;
            std::cout << "  ? 堆叠成功" << std::endl;
            std::cout << "  ? 正压力累积正确" << std::endl;
            std::cout << "  ? 底部球减速最快（承受最大正压力）" << std::endl;
        } else {
            std::cout << "\n??  速度关系不符合预期" << std::endl;
            std::cout << "  期望: vx1 < vx2 < vx3" << std::endl;
            std::cout << "  实际: " << vx1 << " < " << vx2 << " < " << vx3 << std::endl;
        }
    } else {
        std::cout << "\n??  测试未完全通过" << std::endl;
        if (!isStacked) {
            std::cout << "  ? 堆叠失败" << std::endl;
        }
        if (!normalForceCorrect) {
            std::cout << "  ? 正压力计算错误" << std::endl;
        }
    }
    
    delete ball1;
    delete ball2;
    delete ball3;
}

/*=========================================================================================================
 * 测试：两个球堆叠（简化版）
 *=========================================================================================================*/
void test_two_balls_stacked() {
    printTestHeader("两个球堆叠测试（简化版）");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.ground.setFriction(0.5);
    world.start();
    
    std::cout << "场景：两个球堆叠，测试正压力" << std::endl;
    std::cout << "  地面摩擦系数: 0.5" << std::endl;
    std::cout << "  球质量: 1.0 kg\n" << std::endl;
    
    // 底部球
    Circle* ball1 = new Circle(1.0, 5.0, 0.0, 0.0);
    ball1->setName("Ball1");
    ball1->setVelocity(0.0, 0.0);
    ball1->setRestitution(0.0);
    world.addDynamicShape(ball1);
    world.placeDynamicShape(*ball1, 0.0, world.ground.getYLevel());
    
    // 顶部球（精确堆叠）
    Circle* ball2 = new Circle(1.0, 5.0, 0.0, 0.0);
    ball2->setName("Ball2");
    ball2->setVelocity(0.0, 0.0);
    ball2->setRestitution(0.0);
    world.addDynamicShape(ball2);
    
    double ball2_y = ball1->mass_centre[1] + ball1->getRadius() + ball2->getRadius();
    ball2->setCentre(0.0, ball2_y);
    
    std::cout << "让物体稳定30帧..." << std::endl;
    
    // 稳定阶段
    for (int i = 0; i < 30; i++) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    // 检查结果
    double fx1, fy1, fx2, fy2;
    ball1->getNormalForce(fx1, fy1);
    ball2->getNormalForce(fx2, fy2);
    
    std::cout << "\n结果：" << std::endl;
    std::cout << "  Ball1正压力: " << std::abs(fy1) << " N (期望: " << 2.0*9.8 << " N)" << std::endl;
    std::cout << "  Ball2正压力: " << std::abs(fy2) << " N (期望: " << 1.0*9.8 << " N)" << std::endl;
    
    std::cout << "  Ball2支撑者: " << (ball2->getSupporter() ? ball2->getSupporter()->getName() : "无") << std::endl;
    
    if (ball2->getSupporter() == ball1) {
        std::cout << "\n? 支撑关系正确" << std::endl;
    } else {
        std::cout << "\n? 支撑关系错误" << std::endl;
    }
    
    if (std::abs(std::abs(fy1) - 19.6) < 1.0 && std::abs(std::abs(fy2) - 9.8) < 1.0) {
        std::cout << "? 正压力累积正确" << std::endl;
    } else {
        std::cout << "? 正压力累积错误" << std::endl;
    }
    
    delete ball1;
    delete ball2;
}

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "  堆叠物体摩擦力测试（修复版）" << std::endl;
    std::cout << "============================================" << std::endl;
    
    try {
        test_two_balls_stacked();
        test_stacked_objects_friction_fixed();
        
        std::cout << "\n============================================" << std::endl;
        std::cout << "  测试完成" << std::endl;
        std::cout << "============================================" << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n? 测试失败: " << e.what() << std::endl;
        return 1;
    }
}
