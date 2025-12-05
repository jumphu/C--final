/*=========================================================================================================
 * 正确的堆叠测试 - 从地面开始逐层堆叠
 *=========================================================================================================*/

#include <iostream>
#include <iomanip>
#include <cmath>
#include "physicalWorld.h"
#include "shapes.h"

void printSeparator() {
    std::cout << "\n========================================\n";
}

void printShapeDetails(Shape* shape, const std::string& indent = "  ") {
    double x, y, vx, vy, fx, fy;
    shape->getCentre(x, y);
    shape->getVelocity(vx, vy);
    shape->getNormalForce(fx, fy);
    
    std::cout << indent << shape->getName() << ":" << std::endl;
    std::cout << indent << "  质量: " << shape->getMass() << " kg" << std::endl;
    std::cout << indent << "  位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << indent << "  底部Y: " << shape->getBottom() << ", 顶部Y: " << shape->getTop() << std::endl;
    std::cout << indent << "  速度: (" << vx << ", " << vy << ")" << std::endl;
    std::cout << indent << "  被支撑: " << (shape->getIsSupported() ? "是" : "否");
    if (shape->getSupporter()) {
        std::cout << " (支撑物: " << shape->getSupporter()->getName() << ")";
    }
    std::cout << std::endl;
    std::cout << indent << "  正压力: " << std::abs(fy) << " N" << std::endl;
}

// 测试1：正确的堆叠 - 从地面开始
void test_correct_stacking() {
    printSeparator();
    std::cout << "测试1：正确的堆叠（从地面开始）\n";
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setTimeStep(1.0 / 60.0);
    world.ground.setYLevel(0.0);
    
    // 每个盒子高度为10m
    // 正确堆叠：Box1 在地面上，Box2 在 Box1 上，Box3 在 Box2 上
    
    // Box1: 质量2kg，底部应该刚好在地面(Y=0)，所以质心在 Y=5
    AABB* box1 = new AABB(2.0, 10.0, 10.0, 0.0, 5.0);
    box1->setName("Box1");
    
    // Box2: 质量1.5kg，底部应该在 Box1 顶部(Y=10)，所以质心在 Y=15
    AABB* box2 = new AABB(1.5, 10.0, 10.0, 0.0, 15.0);
    box2->setName("Box2");
    
    // Box3: 质量1kg，底部应该在 Box2 顶部(Y=20)，所以质心在 Y=25
    AABB* box3 = new AABB(1.0, 10.0, 10.0, 0.0, 25.0);
    box3->setName("Box3");
    
    world.addDynamicShape(box1);
    world.addDynamicShape(box2);
    world.addDynamicShape(box3);
    
    std::cout << "初始设置：" << std::endl;
    std::cout << "  地面Y坐标: " << world.ground.getYLevel() << std::endl;
    std::cout << "  重力加速度: " << world.getGravity() << " m/s?" << std::endl;
    std::cout << "\n初始位置：" << std::endl;
    printShapeDetails(box1);
    printShapeDetails(box2);
    printShapeDetails(box3);
    
    // 运行物理模拟，让物体稳定
    std::cout << "\n运行物理模拟 (5秒，300帧)..." << std::endl;
    for (int i = 0; i < 300; ++i) {
        world.update(world.dynamicShapeList, world.ground);
        
        // 每秒打印一次状态
        if (i % 60 == 0) {
            std::cout << "\n--- 第 " << (i/60) << " 秒 ---" << std::endl;
            std::cout << "Box1 位置: " << box1->mass_centre[1] << ", 速度: " << box1->velocity[1] << std::endl;
            std::cout << "Box2 位置: " << box2->mass_centre[1] << ", 速度: " << box2->velocity[1] << std::endl;
            std::cout << "Box3 位置: " << box3->mass_centre[1] << ", 速度: " << box3->velocity[1] << std::endl;
        }
    }
    
    std::cout << "\n\n稳定后状态：" << std::endl;
    printShapeDetails(box1);
    printShapeDetails(box2);
    printShapeDetails(box3);
    
    // 验证结果
    printSeparator();
    std::cout << "结果验证：\n";
    printSeparator();
    
    // 理论正压力
    double expected_n1 = (2.0 + 1.5 + 1.0) * 10.0;  // Box1 承受所有重量
    double expected_n2 = (1.5 + 1.0) * 10.0;        // Box2 承受自己和 Box3
    double expected_n3 = 1.0 * 10.0;                // Box3 只承受自己
    
    double fx, fy;
    
    // Box1
    box1->getNormalForce(fx, fy);
    std::cout << "\nBox1 验证：" << std::endl;
    std::cout << "  理论正压力: " << expected_n1 << " N" << std::endl;
    std::cout << "  实际正压力: " << std::abs(fy) << " N" << std::endl;
    std::cout << "  误差: " << std::abs(std::abs(fy) - expected_n1) << " N";
    if (std::abs(std::abs(fy) - expected_n1) < 1.0) {
        std::cout << " ?" << std::endl;
    } else {
        std::cout << " ?" << std::endl;
    }
    std::cout << "  被支撑: " << (box1->getIsSupported() ? "是 ?" : "否 ?") << std::endl;
    std::cout << "  底部位置: " << box1->getBottom() << " (应该≈0)" << std::endl;
    
    // Box2
    box2->getNormalForce(fx, fy);
    std::cout << "\nBox2 验证：" << std::endl;
    std::cout << "  理论正压力: " << expected_n2 << " N" << std::endl;
    std::cout << "  实际正压力: " << std::abs(fy) << " N" << std::endl;
    std::cout << "  误差: " << std::abs(std::abs(fy) - expected_n2) << " N";
    if (std::abs(std::abs(fy) - expected_n2) < 1.0) {
        std::cout << " ?" << std::endl;
    } else {
        std::cout << " ?" << std::endl;
    }
    std::cout << "  被支撑: " << (box2->getIsSupported() ? "是 ?" : "否 ?") << std::endl;
    std::cout << "  底部位置: " << box2->getBottom() << " (应该≈10，即Box1顶部)" << std::endl;
    
    // Box3
    box3->getNormalForce(fx, fy);
    std::cout << "\nBox3 验证：" << std::endl;
    std::cout << "  理论正压力: " << expected_n3 << " N" << std::endl;
    std::cout << "  实际正压力: " << std::abs(fy) << " N" << std::endl;
    std::cout << "  误差: " << std::abs(std::abs(fy) - expected_n3) << " N";
    if (std::abs(std::abs(fy) - expected_n3) < 1.0) {
        std::cout << " ?" << std::endl;
    } else {
        std::cout << " ?" << std::endl;
    }
    std::cout << "  被支撑: " << (box3->getIsSupported() ? "是 ?" : "否 ?") << std::endl;
    std::cout << "  底部位置: " << box3->getBottom() << " (应该≈20，即Box2顶部)" << std::endl;
    
    delete box1;
    delete box2;
    delete box3;
}

// 测试2：逐个添加物体
void test_incremental_stacking() {
    printSeparator();
    std::cout << "测试2：逐个添加物体\n";
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setTimeStep(1.0 / 60.0);
    world.ground.setYLevel(0.0);
    
    std::cout << "第1步：只添加 Box1" << std::endl;
    AABB* box1 = new AABB(2.0, 10.0, 10.0, 0.0, 5.0);
    box1->setName("Box1");
    world.addDynamicShape(box1);
    
    // 让 Box1 稳定
    for (int i = 0; i < 60; ++i) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    std::cout << "Box1 稳定后：" << std::endl;
    printShapeDetails(box1, "  ");
    
    std::cout << "\n第2步：添加 Box2" << std::endl;
    AABB* box2 = new AABB(1.5, 10.0, 10.0, 0.0, 15.0);
    box2->setName("Box2");
    world.addDynamicShape(box2);
    
    // 让两个盒子稳定
    for (int i = 0; i < 120; ++i) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    std::cout << "两个盒子稳定后：" << std::endl;
    printShapeDetails(box1, "  ");
    printShapeDetails(box2, "  ");
    
    std::cout << "\n第3步：添加 Box3" << std::endl;
    AABB* box3 = new AABB(1.0, 10.0, 10.0, 0.0, 25.0);
    box3->setName("Box3");
    world.addDynamicShape(box3);
    
    // 让所有盒子稳定
    for (int i = 0; i < 180; ++i) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    std::cout << "所有盒子稳定后：" << std::endl;
    printShapeDetails(box1, "  ");
    printShapeDetails(box2, "  ");
    printShapeDetails(box3, "  ");
    
    delete box1;
    delete box2;
    delete box3;
}

// 测试3：使用 placeShapeOnGround
void test_using_place_on_ground() {
    printSeparator();
    std::cout << "测试3：使用 placeShapeOnGround\n";
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setTimeStep(1.0 / 60.0);
    world.ground.setYLevel(0.0);
    
    // 创建盒子，初始位置可以不精确
    AABB* box1 = new AABB(2.0, 10.0, 10.0, 0.0, 20.0);  // 故意放高一点
    box1->setName("Box1");
    world.addDynamicShape(box1);
    
    std::cout << "Box1 初始位置: Y=" << box1->mass_centre[1] << ", 底部=" << box1->getBottom() << std::endl;
    
    // 使用 placeShapeOnGround
    world.placeShapeOnGround(*box1, world.ground);
    
    std::cout << "placeShapeOnGround 后: Y=" << box1->mass_centre[1] << ", 底部=" << box1->getBottom() << std::endl;
    
    // 运行一段时间
    for (int i = 0; i < 60; ++i) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    std::cout << "\n1秒后：" << std::endl;
    printShapeDetails(box1);
    
    delete box1;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  堆叠物体正确测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    test_correct_stacking();
    test_incremental_stacking();
    test_using_place_on_ground();
    
    printSeparator();
    std::cout << "所有测试完成" << std::endl;
    printSeparator();
    
    return 0;
}
