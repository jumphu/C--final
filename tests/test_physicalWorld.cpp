#include "../include/physicalWorld.h"
#include <iostream>
#include <iomanip>
#include <cassert>

/*=========================================================================================================
 * 测试工具函数
 *=========================================================================================================*/

// 打印分隔线
void printSeparator(const std::string& title) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

// 打印测试结果
void printTestResult(const std::string& testName, bool passed) {
    std::cout << "[" << (passed ? "? PASS" : "? FAIL") << "] " << testName << std::endl;
}

// 检查浮点数近似相等
bool almostEqual(double a, double b, double epsilon = 0.01) {
    return std::abs(a - b) < epsilon;
}

/*=========================================================================================================
 * 测试1：基础物理世界创建和配置
 *=========================================================================================================*/
void test_BasicWorldCreation() {
    printSeparator("测试1：基础物理世界创建和配置");
    
    // 创建物理世界
    PhysicalWorld world;
    
    // 测试默认参数
    bool test1 = almostEqual(world.getGravity(), 9.8);
    printTestResult("默认重力加速度为 9.8", test1);
    
    bool test2 = almostEqual(world.getInclineAngle(), 0.0);
    printTestResult("默认倾斜角度为 0", test2);
    
    bool test3 = almostEqual(world.getTimeStep(), 1.0/60.0);
    printTestResult("默认时间步长为 1/60", test3);
    
    // 测试设置参数
    world.setGravity(10.0);
    bool test4 = almostEqual(world.getGravity(), 10.0);
    printTestResult("设置重力加速度为 10.0", test4);
    
    world.setInclineAngle(30.0);
    bool test5 = almostEqual(world.getInclineAngle(), 30.0);
    printTestResult("设置倾斜角度为 30°", test5);
    
    // 测试边界条件
    world.setInclineAngle(100.0); // 超出范围
    bool test6 = almostEqual(world.getInclineAngle(), 30.0); // 应该保持不变
    printTestResult("倾斜角度范围限制 (-90° 到 90°)", test6);
    
    std::cout << "\n总体结果: " << (test1 && test2 && test3 && test4 && test5 && test6 ? "? 全部通过" : "? 部分失败") << std::endl;
}

/*=========================================================================================================
 * 测试2：形状创建和管理
 *=========================================================================================================*/
void test_ShapeCreationAndManagement() {
    printSeparator("测试2：形状创建和管理");
    
    PhysicalWorld world;
    
    // 创建圆形
    Shape* circle = world.placeDynamicShapeByType("Circle", "TestCircle", 100.0, 100.0, 1.0, 10.0);
    bool test1 = (circle != nullptr);
    printTestResult("创建圆形", test1);
    
    // 创建矩形
    Shape* box = world.placeDynamicShapeByType("AABB", "TestBox", 200.0, 100.0, 2.0, 20.0, 15.0);
    bool test2 = (box != nullptr);
    printTestResult("创建矩形", test2);
    
    // 创建墙壁
    Wall* wall = world.placeWall("TestWall", 300.0, 50.0, 10.0, 100.0, 0.5);
    bool test3 = (wall != nullptr);
    printTestResult("创建墙壁", test3);
    
    // 测试形状数量
    bool test4 = (world.getDynamicShapeCount() == 2);
    printTestResult("动态形状数量为 2", test4);
    
    bool test5 = (world.getStaticShapeCount() == 1);
    printTestResult("静态形状数量为 1", test5);
    
    // 测试名称查找
    Shape* foundCircle = world.findShapeByName("TestCircle");
    bool test6 = (foundCircle == circle);
    printTestResult("通过名称查找形状", test6);
    
    // 测试类型查找
    auto circles = world.findShapesByType("Circle");
    bool test7 = (circles.size() == 1);
    printTestResult("通过类型查找形状", test7);
    
    std::cout << "\n总体结果: " << (test1 && test2 && test3 && test4 && test5 && test6 && test7 ? "? 全部通过" : "? 部分失败") << std::endl;
}

/*=========================================================================================================
 * 测试3：自由落体模拟
 *=========================================================================================================*/
void test_FreeFall() {
    printSeparator("测试3：自由落体模拟");
    
    PhysicalWorld world;
    world.setGravity(10.0); // 简化计算
    
    Ground ground(0.0, 0.0);
    
    // 创建一个圆形，位置 (0, 100)
    Shape* ball = world.placeDynamicShapeByType("Circle", "Ball", 0.0, 100.0, 1.0, 5.0);
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(ball);
    
    std::cout << "\n初始状态:" << std::endl;
    double x, y, vx, vy;
    ball->getCentre(x, y);
    ball->getVelocity(vx, vy);
    std::cout << "  位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "  速度: (" << vx << ", " << vy << ")" << std::endl;
    
    // 模拟 1 秒（60 帧）
    for (int i = 0; i < 60; i++) {
        world.update(shapeList, ground);
    }
    
    std::cout << "\n1 秒后状态:" << std::endl;
    ball->getCentre(x, y);
    ball->getVelocity(vx, vy);
    std::cout << "  位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "  速度: (" << vx << ", " << vy << ")" << std::endl;
    
    // 验证：应该落到地面
    bool test1 = (y <= 10.0); // 圆形半径为 5，应该在地面附近
    printTestResult("物体落到地面", test1);
    
    // 验证：垂直速度应该接近 0（着地后）
    bool test2 = almostEqual(vy, 0.0, 1.0);
    printTestResult("着地后垂直速度接近 0", test2);
    
    std::cout << "\n总体结果: " << (test1 && test2 ? "? 全部通过" : "? 部分失败") << std::endl;
}

/*=========================================================================================================
 * 测试4：倾斜平面滑动
 *=========================================================================================================*/
void test_InclinedPlaneSliding() {
    printSeparator("测试4：倾斜平面滑动");
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(30.0); // 30° 斜坡
    
    Ground ground(0.0, 0.1); // 低摩擦
    
    // 创建一个盒子
    Shape* box = world.placeDynamicShapeByType("AABB", "Box", 0.0, 20.0, 1.0, 10.0, 10.0);
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(box);
    
    std::cout << "\n初始状态 (30° 斜坡, 摩擦系数 0.1):" << std::endl;
    double x, y, vx, vy;
    box->getCentre(x, y);
    box->getVelocity(vx, vy);
    std::cout << "  位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "  速度: (" << vx << ", " << vy << ")" << std::endl;
    
    double initialX = x;
    
    // 模拟 2 秒
    for (int i = 0; i < 120; i++) {
        world.update(shapeList, ground);
    }
    
    std::cout << "\n2 秒后状态:" << std::endl;
    box->getCentre(x, y);
    box->getVelocity(vx, vy);
    std::cout << "  位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "  速度: (" << vx << ", " << vy << ")" << std::endl;
    
    // 验证：物体应该沿斜面向下滑动（x 坐标增加）
    bool test1 = (x > initialX);
    printTestResult("物体沿斜面向下滑动", test1);
    
    // 验证：物体有水平速度
    bool test2 = (std::abs(vx) > 0.1);
    printTestResult("物体具有水平速度", test2);
    
    std::cout << "\n总体结果: " << (test1 && test2 ? "? 全部通过" : "? 部分失败") << std::endl;
}

/*=========================================================================================================
 * 测试5：碰撞检测
 *=========================================================================================================*/
void test_CollisionDetection() {
    printSeparator("测试5：碰撞检测");
    
    PhysicalWorld world;
    world.setGravity(10.0);
    
    Ground ground(0.0, 0.0);
    
    // 创建两个圆形，让它们相向运动
    Shape* ball1 = world.placeDynamicShapeByType("Circle", "Ball1", 0.0, 50.0, 1.0, 5.0);
    Shape* ball2 = world.placeDynamicShapeByType("Circle", "Ball2", 100.0, 50.0, 1.0, 5.0);
    
    // 设置初始速度（相向运动）
    ball1->setVelocity(50.0, 0.0);  // 向右
    ball2->setVelocity(-50.0, 0.0); // 向左
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(ball1);
    shapeList.push_back(ball2);
    
    std::cout << "\n初始状态:" << std::endl;
    double x1, y1, vx1, vy1, x2, y2, vx2, vy2;
    ball1->getCentre(x1, y1);
    ball1->getVelocity(vx1, vy1);
    ball2->getCentre(x2, y2);
    ball2->getVelocity(vx2, vy2);
    std::cout << "  球1: 位置=(" << x1 << ", " << y1 << "), 速度=(" << vx1 << ", " << vy1 << ")" << std::endl;
    std::cout << "  球2: 位置=(" << x2 << ", " << y2 << "), 速度=(" << vx2 << ", " << vy2 << ")" << std::endl;
    
    double initialDistance = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    std::cout << "  初始距离: " << initialDistance << std::endl;
    
    // 模拟直到碰撞（最多 2 秒）
    int frame = 0;
    bool collided = false;
    for (int i = 0; i < 120; i++) {
        world.update(shapeList, ground);
        
        ball1->getCentre(x1, y1);
        ball2->getCentre(x2, y2);
        double distance = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        
        if (distance < 15.0 && !collided) { // 两球半径之和为 10，加上一些缓冲
            collided = true;
            frame = i;
            break;
        }
    }
    
    std::cout << "\n碰撞检测:" << std::endl;
    std::cout << "  碰撞发生在第 " << frame << " 帧" << std::endl;
    
    ball1->getCentre(x1, y1);
    ball1->getVelocity(vx1, vy1);
    ball2->getCentre(x2, y2);
    ball2->getVelocity(vx2, vy2);
    std::cout << "  球1: 位置=(" << x1 << ", " << y1 << "), 速度=(" << vx1 << ", " << vy1 << ")" << std::endl;
    std::cout << "  球2: 位置=(" << x2 << ", " << y2 << "), 速度=(" << vx2 << ", " << vy2 << ")" << std::endl;
    
    bool test1 = collided;
    printTestResult("检测到碰撞", test1);
    
    // 验证：碰撞后速度应该反向（弹性碰撞）
    bool test2 = (vx1 < 0 && vx2 > 0); // 球1向左，球2向右
    printTestResult("碰撞后速度反向", test2);
    
    std::cout << "\n总体结果: " << (test1 && test2 ? "? 全部通过" : "? 部分失败") << std::endl;
}

/*=========================================================================================================
 * 测试6：堆叠物体（支撑关系和正压力）
 *=========================================================================================================*/
void test_StackedObjects() {
    printSeparator("测试6：堆叠物体（支撑关系和正压力）");
    
    PhysicalWorld world;
    world.setGravity(10.0);
    
    Ground ground(0.0, 0.8); // 高摩擦，防止滑动
    
    // 创建三个盒子，垂直堆叠
    Shape* box1 = world.placeDynamicShapeByType("AABB", "Box1", 0.0, 10.0, 2.0, 20.0, 10.0);
    Shape* box2 = world.placeDynamicShapeByType("AABB", "Box2", 0.0, 25.0, 1.5, 18.0, 10.0);
    Shape* box3 = world.placeDynamicShapeByType("AABB", "Box3", 0.0, 40.0, 1.0, 16.0, 10.0);
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(box1);
    shapeList.push_back(box2);
    shapeList.push_back(box3);
    
    std::cout << "\n初始质量:" << std::endl;
    std::cout << "  Box1: " << box1->getMass() << " kg" << std::endl;
    std::cout << "  Box2: " << box2->getMass() << " kg" << std::endl;
    std::cout << "  Box3: " << box3->getMass() << " kg" << std::endl;
    
    // 模拟 1 秒，让物体稳定
    for (int i = 0; i < 60; i++) {
        world.update(shapeList, ground);
    }
    
    std::cout << "\n稳定后支撑关系:" << std::endl;
    bool test1 = box1->getIsSupported();
    std::cout << "  Box1 被支撑: " << (test1 ? "是 (地面)" : "否") << std::endl;
    
    bool test2 = box2->getIsSupported();
    std::cout << "  Box2 被支撑: " << (test2 ? "是" : "否");
    if (test2) {
        std::cout << " (支撑物: " << (box2->getSupporter() == box1 ? "Box1" : "其他") << ")";
    }
    std::cout << std::endl;
    
    bool test3 = box3->getIsSupported();
    std::cout << "  Box3 被支撑: " << (test3 ? "是" : "否");
    if (test3) {
        std::cout << " (支撑物: " << (box3->getSupporter() == box2 ? "Box2" : "其他") << ")";
    }
    std::cout << std::endl;
    
    // 检查正压力
    std::cout << "\n正压力计算:" << std::endl;
    double fx1, fy1, fx2, fy2, fx3, fy3;
    box1->getNormalForce(fx1, fy1);
    box2->getNormalForce(fx2, fy2);
    box3->getNormalForce(fx3, fy3);
    
    std::cout << "  Box1 正压力: " << -fy1 << " N (应约等于 " << (box1->getMass() + box2->getMass() + box3->getMass()) * 10.0 << " N)" << std::endl;
    std::cout << "  Box2 正压力: " << -fy2 << " N (应约等于 " << (box2->getMass() + box3->getMass()) * 10.0 << " N)" << std::endl;
    std::cout << "  Box3 正压力: " << -fy3 << " N (应约等于 " << box3->getMass() * 10.0 << " N)" << std::endl;
    
    // 验证正压力递增
    bool test4 = (std::abs(fy1) > std::abs(fy2)) && (std::abs(fy2) > std::abs(fy3));
    printTestResult("正压力从下到上递减", test4);
    
    std::cout << "\n总体结果: " << (test1 && test2 && test3 && test4 ? "? 全部通过" : "? 部分失败") << std::endl;
}

/*=========================================================================================================
 * 测试7：坐标转换（倾斜显示）
 *=========================================================================================================*/
void test_CoordinateTransformation() {
    printSeparator("测试7：坐标转换（倾斜显示）");
    
    PhysicalWorld world;
    world.setInclineAngle(30.0);
    
    // 测试点 (100, 100)
    double x = 100.0, y = 100.0;
    auto displayCoords = world.inclineToStandard(x, y);
    
    std::cout << "\n坐标转换 (30° 旋转):" << std::endl;
    std::cout << "  物理坐标: (" << x << ", " << y << ")" << std::endl;
    std::cout << "  显示坐标: (" << displayCoords[0] << ", " << displayCoords[1] << ")" << std::endl;
    
    // 验证：旋转 30° 后的坐标
    double expectedX = x * std::cos(30.0 * 3.14159 / 180.0) - y * std::sin(30.0 * 3.14159 / 180.0);
    double expectedY = x * std::sin(30.0 * 3.14159 / 180.0) + y * std::cos(30.0 * 3.14159 / 180.0);
    
    bool test1 = almostEqual(displayCoords[0], expectedX, 0.1);
    bool test2 = almostEqual(displayCoords[1], expectedY, 0.1);
    
    printTestResult("X 坐标转换正确", test1);
    printTestResult("Y 坐标转换正确", test2);
    
    // 测试 0° 时应该返回原坐标
    world.setInclineAngle(0.0);
    auto displayCoords2 = world.inclineToStandard(x, y);
    bool test3 = almostEqual(displayCoords2[0], x) && almostEqual(displayCoords2[1], y);
    printTestResult("0° 时返回原坐标", test3);
    
    std::cout << "\n总体结果: " << (test1 && test2 && test3 ? "? 全部通过" : "? 部分失败") << std::endl;
}

/*=========================================================================================================
 * 测试8：暂停和恢复
 *=========================================================================================================*/
void test_PauseAndResume() {
    printSeparator("测试8：暂停和恢复");
    
    PhysicalWorld world;
    world.setGravity(10.0);
    
    Ground ground(0.0, 0.0);
    
    Shape* ball = world.placeDynamicShapeByType("Circle", "Ball", 0.0, 100.0, 1.0, 5.0);
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(ball);
    
    // 模拟 30 帧
    for (int i = 0; i < 30; i++) {
        world.update(shapeList, ground);
    }
    
    double x1, y1, vx1, vy1;
    ball->getCentre(x1, y1);
    ball->getVelocity(vx1, vy1);
    
    std::cout << "\n30 帧后状态:" << std::endl;
    std::cout << "  位置: (" << x1 << ", " << y1 << ")" << std::endl;
    std::cout << "  速度: (" << vx1 << ", " << vy1 << ")" << std::endl;
    
    // 暂停
    world.Pause();
    std::cout << "\n暂停..." << std::endl;
    
    // 尝试更新（应该不起作用）
    for (int i = 0; i < 30; i++) {
        world.update(shapeList, ground);
    }
    
    double x2, y2, vx2, vy2;
    ball->getCentre(x2, y2);
    ball->getVelocity(vx2, vy2);
    
    bool test1 = almostEqual(x1, x2) && almostEqual(y1, y2);
    printTestResult("暂停后位置不变", test1);
    
    // 继续
    world.Continue();
    std::cout << "\n继续..." << std::endl;
    
    // 再模拟 30 帧
    for (int i = 0; i < 30; i++) {
        world.update(shapeList, ground);
    }
    
    double x3, y3, vx3, vy3;
    ball->getCentre(x3, y3);
    ball->getVelocity(vx3, vy3);
    
    std::cout << "\n继续后 30 帧状态:" << std::endl;
    std::cout << "  位置: (" << x3 << ", " << y3 << ")" << std::endl;
    std::cout << "  速度: (" << vx3 << ", " << vy3 << ")" << std::endl;
    
    bool test2 = (y3 < y1); // 继续后应该继续下落
    printTestResult("继续后物理模拟恢复", test2);
    
    std::cout << "\n总体结果: " << (test1 && test2 ? "? 全部通过" : "? 部分失败") << std::endl;
}

/*=========================================================================================================
 * 主测试函数
 *=========================================================================================================*/
int main() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                        物理引擎测试套件 v1.0                                  ║\n";
    std::cout << "║                        Physical Engine Test Suite                             ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝\n";
    
    try {
        // 运行所有测试
        test_BasicWorldCreation();
        test_ShapeCreationAndManagement();
        test_FreeFall();
        test_InclinedPlaneSliding();
        test_CollisionDetection();
        test_StackedObjects();
        test_CoordinateTransformation();
        test_PauseAndResume();
        
        printSeparator("测试完成");
        std::cout << "\n? 所有测试执行完毕！" << std::endl;
        std::cout << "  请检查每个测试的具体结果。" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n? 测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n";
    return 0;
}
