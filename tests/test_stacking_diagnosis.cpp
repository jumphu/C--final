/*=========================================================================================================
 * 堆叠问题深度诊断
 * 
 * 分析为什么堆叠物体的支撑关系无法正确建立
 *=========================================================================================================*/

#include <iostream>
#include <iomanip>
#include <cmath>
#include "physicalWorld.h"
#include "shapes.h"

void printSeparator(const std::string& title = "") {
    std::cout << "\n========================================";
    if (!title.empty()) {
        std::cout << "\n  " << title;
        std::cout << "\n========================================";
    }
    std::cout << std::endl;
}

void diagnose_stacking_detailed() {
    printSeparator("堆叠问题深度诊断");
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setTimeStep(1.0 / 60.0);
    
    // 创建两个简单的盒子进行测试
    AABB* box1 = new AABB(1.0, 10.0, 10.0, 0.0, 10.0);  // 底部应该在 Y=5
    AABB* box2 = new AABB(1.0, 10.0, 10.0, 0.0, 30.0);  // 底部应该在 Y=25
    
    box1->setName("Box1");
    box2->setName("Box2");
    
    world.addDynamicShape(box1);
    world.addDynamicShape(box2);
    
    std::cout << "\n初始状态：" << std::endl;
    std::cout << "Box1: 质心Y=" << box1->mass_centre[1] 
              << ", 底部Y=" << box1->getBottom() 
              << ", 顶部Y=" << box1->getTop() << std::endl;
    std::cout << "Box2: 质心Y=" << box2->mass_centre[1] 
              << ", 底部Y=" << box2->getBottom() 
              << ", 顶部Y=" << box2->getTop() << std::endl;
    
    // 检查初始碰撞状态
    bool initial_collision = box1->check_collision(*box2);
    std::cout << "\n初始碰撞检测: " << (initial_collision ? "有碰撞" : "无碰撞") << std::endl;
    
    // 运行几帧让物体下落
    std::cout << "\n运行模拟..." << std::endl;
    for (int i = 0; i < 120; ++i) {  // 2秒
        world.update(world.dynamicShapeList, world.ground);
        
        if (i % 30 == 0) {  // 每0.5秒输出一次
            double vx1, vy1, vx2, vy2;
            box1->getVelocity(vx1, vy1);
            box2->getVelocity(vx2, vy2);
            
            std::cout << "\n帧 " << i << " (t=" << (i * world.getTimeStep()) << "s):" << std::endl;
            std::cout << "  Box1: Y=" << box1->mass_centre[1] 
                      << ", 底部=" << box1->getBottom()
                      << ", 顶部=" << box1->getTop()
                      << ", vy=" << vy1
                      << ", 被支撑=" << (box1->getIsSupported() ? "是" : "否");
            if (box1->getSupporter()) {
                std::cout << " (支撑者: " << box1->getSupporter()->getName() << ")";
            }
            std::cout << std::endl;
            
            std::cout << "  Box2: Y=" << box2->mass_centre[1] 
                      << ", 底部=" << box2->getBottom()
                      << ", 顶部=" << box2->getTop()
                      << ", vy=" << vy2
                      << ", 被支撑=" << (box2->getIsSupported() ? "是" : "否");
            if (box2->getSupporter()) {
                std::cout << " (支撑者: " << box2->getSupporter()->getName() << ")";
            }
            std::cout << std::endl;
            
            // 检查碰撞
            bool collision = box1->check_collision(*box2);
            std::cout << "  碰撞检测: " << (collision ? "有碰撞" : "无碰撞") << std::endl;
            
            // 检查位置关系
            double distance = box2->getBottom() - box1->getTop();
            std::cout << "  Box2底部 - Box1顶部 距离: " << distance << " m" << std::endl;
            
            // 检查质心Y坐标差
            double y_diff = box2->mass_centre[1] - box1->mass_centre[1];
            std::cout << "  质心Y坐标差: " << y_diff << " m" << std::endl;
            
            // 检查相对速度
            double relVy = vy2 - vy1;
            std::cout << "  相对垂直速度: " << relVy << " m/s" << std::endl;
            
            // 分析支撑条件
            std::cout << "  支撑条件检查:" << std::endl;
            std::cout << "    1. 碰撞检测: " << (collision ? "?" : "?") << std::endl;
            std::cout << "    2. Box2在Box1上方: " << (y_diff > 0 ? "?" : "?") << std::endl;
            std::cout << "    3. 相对速度 < 0.5: " << (std::abs(relVy) < 0.5 ? "?" : "?") << std::endl;
        }
    }
    
    printSeparator("最终分析");
    
    std::cout << "\n最终状态：" << std::endl;
    std::cout << "Box1:" << std::endl;
    std::cout << "  位置: (" << box1->mass_centre[0] << ", " << box1->mass_centre[1] << ")" << std::endl;
    std::cout << "  底部: " << box1->getBottom() << ", 顶部: " << box1->getTop() << std::endl;
    std::cout << "  被支撑: " << (box1->getIsSupported() ? "是" : "否") << std::endl;
    
    double fx1, fy1;
    box1->getNormalForce(fx1, fy1);
    std::cout << "  正压力: " << std::abs(fy1) << " N (应该≈" << (1.0 + 1.0) * 10 << " N)" << std::endl;
    
    std::cout << "\nBox2:" << std::endl;
    std::cout << "  位置: (" << box2->mass_centre[0] << ", " << box2->mass_centre[1] << ")" << std::endl;
    std::cout << "  底部: " << box2->getBottom() << ", 顶部: " << box2->getTop() << std::endl;
    std::cout << "  被支撑: " << (box2->getIsSupported() ? "是" : "否") << std::endl;
    if (box2->getSupporter()) {
        std::cout << "  支撑者: " << box2->getSupporter()->getName() << std::endl;
    }
    
    double fx2, fy2;
    box2->getNormalForce(fx2, fy2);
    std::cout << "  正压力: " << std::abs(fy2) << " N (应该≈" << 1.0 * 10 << " N)" << std::endl;
    
    // 分析为什么没有建立支撑关系
    std::cout << "\n问题分析：" << std::endl;
    
    bool collision = box1->check_collision(*box2);
    if (!collision) {
        std::cout << "  ? 碰撞检测失败 - 两物体之间可能有间隙" << std::endl;
        double distance = box2->getBottom() - box1->getTop();
        std::cout << "    间隙距离: " << distance << " m" << std::endl;
    } else {
        std::cout << "  ? 碰撞检测成功" << std::endl;
    }
    
    double y_diff = box2->mass_centre[1] - box1->mass_centre[1];
    if (y_diff <= 0) {
        std::cout << "  ? 质心Y坐标检查失败" << std::endl;
        std::cout << "    Box2质心Y - Box1质心Y = " << y_diff << " m" << std::endl;
    } else {
        std::cout << "  ? 质心Y坐标检查通过" << std::endl;
    }
    
    double vx1, vy1, vx2, vy2;
    box1->getVelocity(vx1, vy1);
    box2->getVelocity(vx2, vy2);
    double relVy = vy2 - vy1;
    if (std::abs(relVy) >= 0.5) {
        std::cout << "  ? 相对速度检查失败" << std::endl;
        std::cout << "    相对速度: " << relVy << " m/s (阈值: 0.5 m/s)" << std::endl;
    } else {
        std::cout << "  ? 相对速度检查通过" << std::endl;
    }
    
    // 检查地面碰撞
    bool box1_on_ground = box1->HasCollidedWithGround(world.ground.getYLevel());
    std::cout << "\nBox1与地面碰撞: " << (box1_on_ground ? "是" : "否") << std::endl;
    std::cout << "  Box1底部: " << box1->getBottom() << " m" << std::endl;
    std::cout << "  地面Y: " << world.ground.getYLevel() << " m" << std::endl;
    std::cout << "  距离: " << (box1->getBottom() - world.ground.getYLevel()) << " m" << std::endl;
    
    delete box1;
    delete box2;
}

void test_collision_detection() {
    printSeparator("碰撞检测精度测试");
    
    // 测试不同间隙下的碰撞检测
    std::cout << "\n测试AABB碰撞检测在不同间隙下的表现：\n" << std::endl;
    
    AABB* box1 = new AABB(1.0, 10.0, 10.0, 0.0, 5.0);  // 顶部在 Y=10
    
    double gaps[] = {-1.0, -0.5, -0.1, 0.0, 0.1, 0.5, 1.0, 2.0};
    
    std::cout << std::setw(15) << "间隙(m)" 
              << std::setw(15) << "Box2底部Y" 
              << std::setw(20) << "碰撞检测结果" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (double gap : gaps) {
        double box2_bottom = box1->getTop() + gap;
        double box2_y = box2_bottom + 5.0;  // 质心在底部上方5米
        
        AABB* box2 = new AABB(1.0, 10.0, 10.0, 0.0, box2_y);
        
        bool collision = box1->check_collision(*box2);
        
        std::cout << std::setw(15) << gap
                  << std::setw(15) << box2->getBottom()
                  << std::setw(20) << (collision ? "有碰撞" : "无碰撞") << std::endl;
        
        delete box2;
    }
    
    delete box1;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  堆叠问题深度诊断工具" << std::endl;
    std::cout << "========================================" << std::endl;
    
    test_collision_detection();
    diagnose_stacking_detailed();
    
    return 0;
}
