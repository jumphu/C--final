#include "../include/physicalWorld.h"
#include "../include/shapes.h"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

// 辅助函数：比较两个double值是否近似相等
bool approxEqual(double a, double b, double epsilon = 1e-3) {
    return fabs(a - b) < epsilon;
}

// 辅助函数：打印分隔线
void printSeparator(const string& title = "") {
    cout << "\n";
    cout << "========================================" << endl;
    if (!title.empty()) {
        cout << "  " << title << endl;
        cout << "========================================" << endl;
    }
}

// 辅助函数：打印形状信息
void printShapeInfo(const string& name, Shape& shape) {
    double x, y, vx, vy, r;
    shape.getCentre(x, y);
    shape.getVelocity(vx, vy);
    shape.getRestitution(r);
    
    cout << name << ":" << endl;
    cout << "  位置: (" << fixed << setprecision(2) << x << ", " << y << ")" << endl;
    cout << "  速度: (" << vx << ", " << vy << ")" << endl;
    cout << "  质量: " << shape.getMass() << endl;
    cout << "  弹性系数: " << r << endl;
}

/*=========================================================================================================
 * 测试1：Circle vs Circle 碰撞
 *=========================================================================================================*/
void testCircleVsCircleCollision() {
    printSeparator("测试1: Circle vs Circle 碰撞");
    
    cout << "\n场景：两个圆形正面对撞" << endl;
    cout << "圆1从左向右运动，圆2从右向左运动\n" << endl;
    
    // 创建物理世界
    PhysicalWorld world;
    world.setGravity(0.0);  // 关闭重力，只测试碰撞
    
    // 创建两个圆形
    Circle* circle1 = new Circle(1.0, 2.0, 0.0, 0.0);   // 质量1，半径2，位置(0,0)
    Circle* circle2 = new Circle(1.0, 2.0, 6.0, 0.0);   // 质量1，半径2，位置(6,0)
    
    // 设置速度：相向运动
    circle1->setVelocity(5.0, 0.0);   // 向右
    circle2->setVelocity(-5.0, 0.0);  // 向左
    
    // 设置弹性系数
    circle1->setRestitution(1.0);  // 完全弹性
    circle2->setRestitution(1.0);
    
    cout << "碰撞前状态：" << endl;
    printShapeInfo("圆1", *circle1);
    printShapeInfo("圆2", *circle2);
    
    // 执行碰撞检测和响应
    if (circle1->check_collision(*circle2)) {
        cout << "\n✓ 检测到碰撞！" << endl;
        world.Collisions(*circle1, *circle2);
    }
    
    cout << "\n碰撞后状态：" << endl;
    printShapeInfo("圆1", *circle1);
    printShapeInfo("圆2", *circle2);
    
    // 验证：完全弹性碰撞，相等质量，速度应该交换
    double v1x, v1y, v2x, v2y;
    circle1->getVelocity(v1x, v1y);
    circle2->getVelocity(v2x, v2y);
    
    cout << "\n预期结果：速度交换" << endl;
    cout << "圆1速度应该约为 (-5, 0)，实际: (" << v1x << ", " << v1y << ")" << endl;
    cout << "圆2速度应该约为 (5, 0)，实际: (" << v2x << ", " << v2y << ")" << endl;
    
    if (approxEqual(v1x, -5.0) && approxEqual(v2x, 5.0)) {
        cout << "✓ 测试通过！" << endl;
    } else {
        cout << "✗ 测试失败！" << endl;
    }
    
    delete circle1;
    delete circle2;
}

/*=========================================================================================================
 * 测试2：不同质量的 Circle vs Circle 碰撞
 *=========================================================================================================*/
void testCircleVsCircleDifferentMass() {
    printSeparator("测试2: 不同质量的圆形碰撞");
    
    cout << "\n场景：重球撞击轻球" << endl;
    cout << "重球(质量10)向右运动，轻球(质量1)静止\n" << endl;
    
    PhysicalWorld world;
    world.setGravity(0.0);
    
    // 重球
    Circle* heavyBall = new Circle(10.0, 2.0, 0.0, 0.0);
    heavyBall->setVelocity(10.0, 0.0);
    heavyBall->setRestitution(0.8);
    
    // 轻球
    Circle* lightBall = new Circle(1.0, 2.0, 6.0, 0.0);
    lightBall->setVelocity(0.0, 0.0);
    lightBall->setRestitution(0.8);
    
    cout << "碰撞前状态：" << endl;
    printShapeInfo("重球", *heavyBall);
    printShapeInfo("轻球", *lightBall);
    
    if (heavyBall->check_collision(*lightBall)) {
        cout << "\n✓ 检测到碰撞！" << endl;
        world.Collisions(*heavyBall, *lightBall);
    }
    
    cout << "\n碰撞后状态：" << endl;
    printShapeInfo("重球", *heavyBall);
    printShapeInfo("轻球", *lightBall);
    
    double v1x, v1y, v2x, v2y;
    heavyBall->getVelocity(v1x, v1y);
    lightBall->getVelocity(v2x, v2y);
    
    cout << "\n观察：" << endl;
    cout << "重球速度变化小，轻球速度变化大" << endl;
    cout << "重球速度: " << v1x << " (应该略小于10)" << endl;
    cout << "轻球速度: " << v2x << " (应该远大于10)" << endl;
    
    if (v1x > 0 && v1x < 10.0 && v2x > 10.0) {
        cout << "✓ 物理行为正确！" << endl;
    }
    
    delete heavyBall;
    delete lightBall;
}

/*=========================================================================================================
 * 测试3：AABB vs AABB 碰撞
 *=========================================================================================================*/
void testAABBVsAABBCollision() {
    printSeparator("测试3: AABB vs AABB 碰撞");
    
    cout << "\n场景：两个矩形水平碰撞" << endl;
    cout << "矩形1从左向右运动，矩形2静止\n" << endl;
    
    PhysicalWorld world;
    world.setGravity(0.0);
    
    // 移动的矩形
    AABB* box1 = new AABB(1.0, 4.0, 4.0, 0.0, 0.0);  // 质量1，4x4，位置(0,0)
    box1->setVelocity(8.0, 0.0);
    box1->setRestitution(0.9);
    
    // 静止的矩形
    AABB* box2 = new AABB(1.0, 4.0, 4.0, 6.0, 0.0);  // 质量1，4x4，位置(6,0)
    box2->setVelocity(0.0, 0.0);
    box2->setRestitution(0.9);
    
    cout << "碰撞前状态：" << endl;
    printShapeInfo("矩形1", *box1);
    printShapeInfo("矩形2", *box2);
    
    if (box1->check_collision(*box2)) {
        cout << "\n✓ 检测到碰撞！" << endl;
        world.Collisions(*box1, *box2);
    }
    
    cout << "\n碰撞后状态：" << endl;
    printShapeInfo("矩形1", *box1);
    printShapeInfo("矩形2", *box2);
    
    double v1x, v1y, v2x, v2y;
    box1->getVelocity(v1x, v1y);
    box2->getVelocity(v2x, v2y);
    
    cout << "\n预期：矩形1减速或停止，矩形2加速" << endl;
    cout << "矩形1速度: " << v1x << endl;
    cout << "矩形2速度: " << v2x << endl;
    
    if (v1x < 8.0 && v2x > 0) {
        cout << "✓ 碰撞响应正确！" << endl;
    }
    
    delete box1;
    delete box2;
}

/*=========================================================================================================
 * 测试4：Circle vs AABB 碰撞
 *=========================================================================================================*/
void testCircleVsAABBCollision() {
    printSeparator("测试4: Circle vs AABB 碰撞");
    
    cout << "\n场景：圆形撞击矩形" << endl;
    cout << "圆形从左向右运动，矩形静止\n" << endl;
    
    PhysicalWorld world;
    world.setGravity(0.0);
    
    // 移动的圆形
    Circle* circle = new Circle(2.0, 2.0, 0.0, 0.0);  // 质量2，半径2，位置(0,0)
    circle->setVelocity(6.0, 0.0);
    circle->setRestitution(0.7);
    
    // 静止的矩形
    AABB* box = new AABB(3.0, 4.0, 4.0, 8.0, 0.0);  // 质量3，4x4，位置(8,0)
    box->setVelocity(0.0, 0.0);
    box->setRestitution(0.7);
    
    cout << "碰撞前状态：" << endl;
    printShapeInfo("圆形", *circle);
    printShapeInfo("矩形", *box);
    
    if (circle->check_collision(*box)) {
        cout << "\n✓ 检测到碰撞！" << endl;
        world.Collisions(*circle, *box);
    }
    
    cout << "\n碰撞后状态：" << endl;
    printShapeInfo("圆形", *circle);
    printShapeInfo("矩形", *box);
    
    double v1x, v1y, v2x, v2y;
    circle->getVelocity(v1x, v1y);
    box->getVelocity(v2x, v2y);
    
    cout << "\n观察：动量守恒和能量部分守恒" << endl;
    cout << "圆形速度: " << v1x << endl;
    cout << "矩形速度: " << v2x << endl;
    
    // 检查动量方向
    if (v1x < 6.0 && v2x > 0) {
        cout << "✓ 碰撞物理正确！" << endl;
    }
    
    delete circle;
    delete box;
}

/*=========================================================================================================
 * 测试5：不同弹性系数的碰撞
 *=========================================================================================================*/
void testDifferentRestitution() {
    printSeparator("测试5: 不同弹性系数的碰撞对比");
    
    cout << "\n对比：完全弹性 vs 部分弹性 vs 完全非弹性\n" << endl;
    
    PhysicalWorld world;
    world.setGravity(0.0);
    
    // 测试三种情况
    struct TestCase {
        double restitution;
        string description;
    };
    
    TestCase cases[] = {
        {1.0, "完全弹性 (e=1.0)"},
        {0.5, "部分弹性 (e=0.5)"},
        {0.0, "完全非弹性 (e=0.0)"}
    };
    
    for (int i = 0; i < 3; i++) {
        cout << "\n--- " << cases[i].description << " ---" << endl;
        
        Circle* ball1 = new Circle(1.0, 2.0, 0.0, 0.0);
        Circle* ball2 = new Circle(1.0, 2.0, 6.0, 0.0);
        
        ball1->setVelocity(10.0, 0.0);
        ball2->setVelocity(-10.0, 0.0);
        
        ball1->setRestitution(cases[i].restitution);
        ball2->setRestitution(cases[i].restitution);
        
        cout << "碰撞前：球1速度=" << 10.0 << ", 球2速度=" << -10.0 << endl;
        
        if (ball1->check_collision(*ball2)) {
            world.Collisions(*ball1, *ball2);
        }
        
        double v1x, v1y, v2x, v2y;
        ball1->getVelocity(v1x, v1y);
        ball2->getVelocity(v2x, v2y);
        
        cout << "碰撞后：球1速度=" << v1x << ", 球2速度=" << v2x << endl;
        cout << "速度变化：球1=" << (v1x - 10.0) << ", 球2=" << (v2x + 10.0) << endl;
        
        delete ball1;
        delete ball2;
    }
    
    cout << "\n✓ 弹性系数影响测试完成！" << endl;
}

/*=========================================================================================================
 * 测试6：连续碰撞模拟
 *=========================================================================================================*/
void testMultipleCollisions() {
    printSeparator("测试6: 连续碰撞模拟");
    
    cout << "\n场景：三个球的连锁碰撞" << endl;
    cout << "球1撞击球2，球2再撞击球3\n" << endl;
    
    PhysicalWorld world;
    world.setGravity(0.0);
    Ground ground(0.0, 0.0);
    
    // 创建三个球
    vector<Shape*> balls;
    
    Circle* ball1 = new Circle(1.0, 2.0, 0.0, 5.0);
    ball1->setVelocity(20.0, 0.0);
    ball1->setRestitution(0.9);
    balls.push_back(ball1);
    
    Circle* ball2 = new Circle(1.0, 2.0, 8.0, 5.0);
    ball2->setVelocity(0.0, 0.0);
    ball2->setRestitution(0.9);
    balls.push_back(ball2);
    
    Circle* ball3 = new Circle(1.0, 2.0, 16.0, 5.0);
    ball3->setVelocity(0.0, 0.0);
    ball3->setRestitution(0.9);
    balls.push_back(ball3);
    
    cout << "初始状态：" << endl;
    for (size_t i = 0; i < balls.size(); i++) {
        cout << "球" << (i+1) << ": ";
        double vx, vy;
        balls[i]->getVelocity(vx, vy);
        cout << "速度=" << vx << endl;
    }
    
    // 模拟几个时间步
    cout << "\n开始模拟..." << endl;
    for (int step = 0; step < 5; step++) {
        cout << "\n时间步 " << (step+1) << ":" << endl;
        world.update(balls, 0.1, ground);
        
        for (size_t i = 0; i < balls.size(); i++) {
            double x, y, vx, vy;
            balls[i]->getCentre(x, y);
            balls[i]->getVelocity(vx, vy);
            cout << "  球" << (i+1) << ": 位置=(" << x << ", " << y 
                 << "), 速度=" << vx << endl;
        }
    }
    
    cout << "\n✓ 连续碰撞模拟完成！" << endl;
    
    for (auto ball : balls) {
        delete ball;
    }
}

/*=========================================================================================================
 * 主函数
 
int main() {
    cout << "\n";
    cout << "╔════════════════════════════════════════╗" << endl;
    cout << "║   物理世界碰撞测试套件                 ║" << endl;
    cout << "║   测试不同类型形状的碰撞响应           ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    
    try {
        // 测试1：Circle vs Circle
        testCircleVsCircleCollision();
        
        // 测试2：不同质量的碰撞
        testCircleVsCircleDifferentMass();
        
        // 测试3：AABB vs AABB
        testAABBVsAABBCollision();
        
        // 测试4：Circle vs AABB
        testCircleVsAABBCollision();
        
        // 测试5：不同弹性系数
        testDifferentRestitution();
        
        // 测试6：连续碰撞
        testMultipleCollisions();
        
        printSeparator();
        cout << "\n✓✓✓ 所有碰撞测试完成！ ✓✓✓\n" << endl;
        
        return 0;
        
    } catch (const exception& e) {
        cerr << "\n✗ 测试失败: " << e.what() << endl;
        return 1;
    }
}
*=========================================================================================================*/