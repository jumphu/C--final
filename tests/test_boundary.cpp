#include "../include/physicalWorld.h"
#include "../include/shapes.h"
#include <iostream>
#include <iomanip>

using namespace std;

void printSeparator(const string& title = "") {
    cout << "\n========================================" << endl;
    if (!title.empty()) {
        cout << "  " << title << endl;
        cout << "========================================" << endl;
    }
}

void printShapeInfo(const string& name, Shape& shape) {
    double x, y, vx, vy;
    shape.getCentre(x, y);
    shape.getVelocity(vx, vy);
    
    cout << name << ": 位置=(" << fixed << setprecision(2) 
         << x << ", " << y << "), 速度=(" 
         << vx << ", " << vy << ")" << endl;
}

/*=========================================================================================================
 * 测试1：边界反弹测试
 *=========================================================================================================*/
void testBoundaryReflection() {
    printSeparator("测试1: 边界反弹测试");
    
    cout << "\n场景：球体撞击边界反弹" << endl;
    cout << "边界: [-100, 100, -100, 100]\n" << endl;
    
    // 创建物理世界，设置边界
    PhysicalWorld world(-100.0, 100.0, -100.0, 100.0);
    world.setGravity(0.0);  // 关闭重力
    Ground ground(0.0, 0.0);
    
    // 创建一个向右移动的球
    Circle* ball = new Circle(1.0, 5.0, 80.0, 0.0);  // 质量1，半径5，位置(80,0)
    ball->setVelocity(30.0, 0.0);  // 向右30单位/秒
    ball->setRestitution(0.9);  // 90%弹性
    
    vector<Shape*> shapes;
    shapes.push_back(ball);
    
    cout << "初始状态：" << endl;
    printShapeInfo("球", *ball);
    
    // 模拟几个时间步
    cout << "\n开始模拟..." << endl;
    for (int step = 0; step < 10; step++) {
        world.update(shapes, 0.1, ground);
        
        double x, y;
        ball->getCentre(x, y);
        
        // 只在关键时刻打印
        if (step == 0 || step == 2 || step == 5 || step == 9) {
            cout << "时间步 " << (step+1) << ": ";
            printShapeInfo("", *ball);
        }
    }
    
    cout << "\n✓ 边界反弹测试完成！" << endl;
    delete ball;
}

/*=========================================================================================================
 * 测试2：多球边界反弹测试
 *=========================================================================================================*/
void testMultipleBallsBoundary() {
    printSeparator("测试2: 多球边界测试");
    
    cout << "\n场景：多个球在边界内运动和碰撞" << endl;
    cout << "边界: [0, 200, 0, 150]\n" << endl;
    
    PhysicalWorld world(0.0, 200.0, 0.0, 150.0);
    world.setGravity(0.0);
    Ground ground(0.0, 0.0);
    
    vector<Shape*> balls;
    
    // 创建4个球，不同方向运动
    Circle* ball1 = new Circle(1.0, 8.0, 50.0, 75.0);
    ball1->setVelocity(25.0, 15.0);
    ball1->setRestitution(0.95);
    balls.push_back(ball1);
    
    Circle* ball2 = new Circle(1.0, 8.0, 150.0, 75.0);
    ball2->setVelocity(-20.0, 10.0);
    ball2->setRestitution(0.95);
    balls.push_back(ball2);
    
    Circle* ball3 = new Circle(1.0, 8.0, 100.0, 30.0);
    ball3->setVelocity(15.0, 20.0);
    ball3->setRestitution(0.95);
    balls.push_back(ball3);
    
    Circle* ball4 = new Circle(1.0, 8.0, 100.0, 120.0);
    ball4->setVelocity(-10.0, -25.0);
    ball4->setRestitution(0.95);
    balls.push_back(ball4);
    
    cout << "初始状态：" << endl;
    for (size_t i = 0; i < balls.size(); i++) {
        cout << "  ";
        printShapeInfo("球" + to_string(i+1), *balls[i]);
    }
    
    // 模拟
    cout << "\n模拟10个时间步后..." << endl;
    for (int step = 0; step < 10; step++) {
        world.update(balls, 0.1, ground);
    }
    
    cout << "最终状态：" << endl;
    for (size_t i = 0; i < balls.size(); i++) {
        cout << "  ";
        printShapeInfo("球" + to_string(i+1), *balls[i]);
    }
    
    cout << "\n✓ 多球边界测试完成！" << endl;
    
    for (auto ball : balls) {
        delete ball;
    }
}

/*=========================================================================================================
 * 测试3：弹性系数对边界反弹的影响
 *=========================================================================================================*/
void testRestitutionEffect() {
    printSeparator("测试3: 弹性系数影响测试");
    
    cout << "\n场景：不同弹性系数的球撞击边界" << endl;
    cout << "测试弹性系数: 1.0, 0.7, 0.3\n" << endl;
    
    double restitutions[] = {1.0, 0.7, 0.3};
    string names[] = {"完全弹性", "高弹性", "低弹性"};
    
    for (int i = 0; i < 3; i++) {
        cout << "\n--- " << names[i] << " (e=" << restitutions[i] << ") ---" << endl;
        
        PhysicalWorld world(0.0, 100.0, 0.0, 100.0);
        world.setGravity(0.0);
        Ground ground(0.0, 0.0);
        
        Circle* ball = new Circle(1.0, 5.0, 10.0, 50.0);
        ball->setVelocity(40.0, 0.0);  // 向右
        ball->setRestitution(restitutions[i]);
        
        vector<Shape*> shapes;
        shapes.push_back(ball);
        
        double vx_initial, vy_initial;
        ball->getVelocity(vx_initial, vy_initial);
        cout << "初始速度: " << vx_initial << endl;
        
        // 让球撞击右边界
        for (int step = 0; step < 5; step++) {
            world.update(shapes, 0.1, ground);
        }
        
        double vx_after, vy_after;
        ball->getVelocity(vx_after, vy_after);
        cout << "反弹后速度: " << vx_after << endl;
        cout << "速度保留率: " << (abs(vx_after) / abs(vx_initial)) * 100 << "%" << endl;
        
        delete ball;
    }
    
    cout << "\n✓ 弹性系数影响测试完成！" << endl;
}

/*=========================================================================================================
 * 测试4：摩擦力测试
 *=========================================================================================================*/
void testFrictionEffect() {
    printSeparator("测试4: 摩擦力影响测试");
    
    cout << "\n场景：带摩擦力的物体碰撞" << endl;
    cout << "两个球碰撞，一个有摩擦力，一个无摩擦力\n" << endl;
    
    PhysicalWorld world(0.0, 200.0, 0.0, 100.0);
    world.setGravity(0.0);
    Ground ground(0.0, 0.0);
    
    // 无摩擦球
    Circle* ball1 = new Circle(1.0, 10.0, 50.0, 50.0);
    ball1->setVelocity(20.0, 0.0);
    ball1->setRestitution(0.9);
    ball1->setFraction(0.0);  // 无摩擦
    
    // 有摩擦球
    Circle* ball2 = new Circle(1.0, 10.0, 100.0, 50.0);
    ball2->setVelocity(0.0, 0.0);
    ball2->setRestitution(0.9);
    ball2->setFraction(0.5);  // 50%摩擦
    
    vector<Shape*> balls;
    balls.push_back(ball1);
    balls.push_back(ball2);
    
    cout << "碰撞前：" << endl;
    printShapeInfo("  无摩擦球", *ball1);
    printShapeInfo("  有摩擦球", *ball2);
    
    // 模拟碰撞
    for (int step = 0; step < 5; step++) {
        world.update(balls, 0.1, ground);
    }
    
    cout << "\n碰撞后：" << endl;
    printShapeInfo("  无摩擦球", *ball1);
    printShapeInfo("  有摩擦球", *ball2);
    
    cout << "\n观察：摩擦力会影响碰撞后的速度分配" << endl;
    cout << "✓ 摩擦力测试完成！" << endl;
    
    delete ball1;
    delete ball2;
}

/*=========================================================================================================
 * 测试5：矩形边界测试
 *=========================================================================================================*/
void testAABBBoundary() {
    printSeparator("测试5: 矩形边界测试");
    
    cout << "\n场景：矩形在边界内运动" << endl;
    cout << "边界: [0, 150, 0, 100]\n" << endl;
    
    PhysicalWorld world(0.0, 150.0, 0.0, 100.0);
    world.setGravity(0.0);
    Ground ground(0.0, 0.0);
    
    // 创建一个旋转运动的矩形
    AABB* box = new AABB(1.0, 15.0, 10.0, 75.0, 50.0);
    box->setVelocity(25.0, 20.0);
    box->setRestitution(0.8);
    
    vector<Shape*> shapes;
    shapes.push_back(box);
    
    cout << "初始状态：" << endl;
    printShapeInfo("矩形", *box);
    
    cout << "\n模拟轨迹：" << endl;
    for (int step = 0; step < 15; step++) {
        world.update(shapes, 0.1, ground);
        
        if (step % 3 == 0) {
            cout << "时间步 " << (step+1) << ": ";
            printShapeInfo("", *box);
        }
    }
    
    cout << "\n✓ 矩形边界测试完成！" << endl;
    delete box;
}

/*=========================================================================================================
 * 主函数
 
int main() {
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║   物理世界边界系统测试套件             ║" << endl;
    cout << "║   测试边界碰撞、摩擦力和弹性           ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    
    try {
        // 测试1：基本边界反弹
        testBoundaryReflection();
        
        // 测试2：多球边界
        testMultipleBallsBoundary();
        
        // 测试3：弹性系数影响
        testRestitutionEffect();
        
        // 测试4：摩擦力影响
        testFrictionEffect();
        
        // 测试5：矩形边界
        testAABBBoundary();
        
        printSeparator();
        cout << "\n✓✓✓ 所有边界测试完成！ ✓✓✓\n" << endl;
        
        return 0;
        
    } catch (const exception& e) {
        cerr << "\n✗ 测试失败: " << e.what() << endl;
        return 1;
    }
}
*=========================================================================================================*/