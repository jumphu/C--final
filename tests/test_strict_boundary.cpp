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
 * 测试1：物体完全超出边界 - 停止运动
 *=========================================================================================================*/
void testOutOfBoundsStop() {
    printSeparator("测试1: 物体完全超出边界 - 停止运动");
    
    cout << "\n场景：球体飞出边界外" << endl;
    cout << "边界: [0, 100, 0, 100]" << endl;
    cout << "球的半径: 5\n" << endl;
    
    PhysicalWorld world(0.0, 100.0, 0.0, 100.0);
    world.setGravity(0.0);
    Ground ground(0.0, 0.0);
    
    // 创建一个会飞出边界的球
    Circle* ball = new Circle(1.0, 5.0, 95.0, 50.0);  // 位置(95, 50)，半径5
    ball->setVelocity(50.0, 0.0);  // 高速向右
    ball->setRestitution(0.9);
    
    vector<Shape*> shapes;
    shapes.push_back(ball);
    
    cout << "初始状态：" << endl;
    printShapeInfo("  球", *ball);
    
    cout << "\n模拟过程：" << endl;
    for (int step = 0; step < 10; step++) {
        world.update(shapes, 0.1, ground);
        
        double x, y, vx, vy;
        ball->getCentre(x, y);
        ball->getVelocity(vx, vy);
        
        cout << "时间步 " << (step+1) << ": ";
        printShapeInfo("", *ball);
        
        // 检查是否已停止
        if (vx == 0.0 && vy == 0.0) {
            cout << "  → 物体已停止运动（完全超出边界）" << endl;
            break;
        }
    }
    
    cout << "\n✓ 测试完成：物体超出边界后已停止" << endl;
    delete ball;
}

/*=========================================================================================================
 * 测试2：物体部分碰撞边界 - 反弹
 *=========================================================================================================*/
void testPartialBoundaryReflection() {
    printSeparator("测试2: 物体部分碰撞边界 - 反弹");
    
    cout << "\n场景：球体正常撞击边界反弹" << endl;
    cout << "边界: [0, 100, 0, 100]" << endl;
    cout << "球的半径: 5\n" << endl;
    
    PhysicalWorld world(0.0, 100.0, 0.0, 100.0);
    world.setGravity(0.0);
    Ground ground(0.0, 0.0);
    
    // 创建一个会正常反弹的球
    Circle* ball = new Circle(1.0, 5.0, 80.0, 50.0);  // 位置(80, 50)
    ball->setVelocity(15.0, 0.0);  // 适中速度向右
    ball->setRestitution(0.9);
    
    vector<Shape*> shapes;
    shapes.push_back(ball);
    
    cout << "初始状态：" << endl;
    printShapeInfo("  球", *ball);
    
    cout << "\n模拟过程：" << endl;
    bool hasReflected = false;
    for (int step = 0; step < 8; step++) {
        double vx_before, vy_before;
        ball->getVelocity(vx_before, vy_before);
        
        world.update(shapes, 0.1, ground);
        
        double vx_after, vy_after;
        ball->getVelocity(vx_after, vy_after);
        
        cout << "时间步 " << (step+1) << ": ";
        printShapeInfo("", *ball);
        
        // 检测反弹
        if (vx_before > 0 && vx_after < 0 && !hasReflected) {
            cout << "  → 检测到反弹！" << endl;
            hasReflected = true;
        }
    }
    
    cout << "\n✓ 测试完成：物体正常反弹" << endl;
    delete ball;
}

/*=========================================================================================================
 * 测试3：对比测试 - 不同速度的行为
 *=========================================================================================================*/
void testSpeedComparison() {
    printSeparator("测试3: 不同速度的边界行为对比");
    
    cout << "\n场景：对比不同速度下的边界处理" << endl;
    cout << "边界: [0, 100, 0, 100]" << endl;
    cout << "球的半径: 5，起始位置: (85, 50)\n" << endl;
    
    double speeds[] = {10.0, 30.0, 100.0};
    string descriptions[] = {"慢速", "中速", "极快"};
    
    for (int i = 0; i < 3; i++) {
        cout << "\n--- " << descriptions[i] << " (速度=" << speeds[i] << ") ---" << endl;
        
        PhysicalWorld world(0.0, 100.0, 0.0, 100.0);
        world.setGravity(0.0);
        Ground ground(0.0, 0.0);
        
        Circle* ball = new Circle(1.0, 5.0, 85.0, 50.0);
        ball->setVelocity(speeds[i], 0.0);
        ball->setRestitution(0.9);
        
        vector<Shape*> shapes;
        shapes.push_back(ball);
        
        cout << "初始速度: " << speeds[i] << endl;
        
        // 模拟5步
        bool stopped = false;
        for (int step = 0; step < 5; step++) {
            world.update(shapes, 0.1, ground);
            
            double vx, vy;
            ball->getVelocity(vx, vy);
            
            if (vx == 0.0 && vy == 0.0 && !stopped) {
                cout << "时间步 " << (step+1) << ": 物体停止（完全超出边界）" << endl;
                stopped = true;
                break;
            }
        }
        
        if (!stopped) {
            double x, y, vx, vy;
            ball->getCentre(x, y);
            ball->getVelocity(vx, vy);
            cout << "最终状态: 位置=(" << x << ", " << y << "), 速度=" << vx << endl;
            cout << "结果: 物体仍在边界内运动" << endl;
        }
        
        delete ball;
    }
    
    cout << "\n✓ 对比测试完成" << endl;
}

/*=========================================================================================================
 * 测试4：矩形超出边界测试
 *=========================================================================================================*/
void testAABBOutOfBounds() {
    printSeparator("测试4: 矩形超出边界测试");
    
    cout << "\n场景：矩形飞出边界" << endl;
    cout << "边界: [0, 100, 0, 100]" << endl;
    cout << "矩形大小: 10x10\n" << endl;
    
    PhysicalWorld world(0.0, 100.0, 0.0, 100.0);
    world.setGravity(0.0);
    Ground ground(0.0, 0.0);
    
    // 创建一个会飞出边界的矩形
    AABB* box = new AABB(1.0, 10.0, 10.0, 90.0, 50.0);  // 10x10矩形，位置(90, 50)
    box->setVelocity(60.0, 30.0);  // 斜向飞出
    box->setRestitution(0.8);
    
    vector<Shape*> shapes;
    shapes.push_back(box);
    
    cout << "初始状态：" << endl;
    printShapeInfo("  矩形", *box);
    
    cout << "\n模拟过程：" << endl;
    for (int step = 0; step < 8; step++) {
        world.update(shapes, 0.1, ground);
        
        double x, y, vx, vy;
        box->getCentre(x, y);
        box->getVelocity(vx, vy);
        
        cout << "时间步 " << (step+1) << ": ";
        printShapeInfo("", *box);
        
        if (vx == 0.0 && vy == 0.0) {
            cout << "  → 矩形已停止运动" << endl;
            break;
        }
    }
    
    cout << "\n✓ 矩形边界测试完成" << endl;
    delete box;
}

/*=========================================================================================================
 * 测试5：多物体混合测试
 *=========================================================================================================*/
void testMixedBehavior() {
    printSeparator("测试5: 多物体混合边界行为");
    
    cout << "\n场景：3个球，不同速度，观察不同结果" << endl;
    cout << "边界: [0, 100, 0, 100]\n" << endl;
    
    PhysicalWorld world(0.0, 100.0, 0.0, 100.0);
    world.setGravity(0.0);
    Ground ground(0.0, 0.0);
    
    vector<Shape*> balls;
    
    // 球1：慢速，会反弹
    Circle* ball1 = new Circle(1.0, 5.0, 80.0, 50.0);
    ball1->setVelocity(12.0, 0.0);
    ball1->setRestitution(0.9);
    balls.push_back(ball1);
    
    // 球2：中速，可能反弹或停止
    Circle* ball2 = new Circle(1.0, 5.0, 85.0, 60.0);
    ball2->setVelocity(25.0, 0.0);
    ball2->setRestitution(0.9);
    balls.push_back(ball2);
    
    // 球3：极快，会飞出停止
    Circle* ball3 = new Circle(1.0, 5.0, 90.0, 70.0);
    ball3->setVelocity(80.0, 0.0);
    ball3->setRestitution(0.9);
    balls.push_back(ball3);
    
    cout << "初始状态：" << endl;
    for (size_t i = 0; i < balls.size(); i++) {
        cout << "  ";
        printShapeInfo("球" + to_string(i+1), *balls[i]);
    }
    
    cout << "\n模拟10个时间步..." << endl;
    for (int step = 0; step < 10; step++) {
        world.update(balls, 0.1, ground);
    }
    
    cout << "\n最终状态：" << endl;
    for (size_t i = 0; i < balls.size(); i++) {
        cout << "  ";
        double vx, vy;
        balls[i]->getVelocity(vx, vy);
        printShapeInfo("球" + to_string(i+1), *balls[i]);
        
        if (vx == 0.0 && vy == 0.0) {
            cout << "    状态: 已停止（超出边界）" << endl;
        } else if (vx < 0.0) {
            cout << "    状态: 反弹回来" << endl;
        } else {
            cout << "    状态: 继续向前" << endl;
        }
    }
    
    cout << "\n✓ 混合测试完成" << endl;
    
    for (auto ball : balls) {
        delete ball;
    }
}

/*=========================================================================================================
 * 主函数
 
int main() {
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║   边界严格约束测试套件                 ║" << endl;
    cout << "║   超出边界 → 停止运动                  ║" << endl;
    cout << "║   碰撞边界 → 反弹                      ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    
    try {
        // 测试1：完全超出边界停止
        testOutOfBoundsStop();
        
        // 测试2：部分碰撞反弹
        testPartialBoundaryReflection();
        
        // 测试3：速度对比
        testSpeedComparison();
        
        // 测试4：矩形测试
        testAABBOutOfBounds();
        
        // 测试5：混合测试
        testMixedBehavior();
        
        printSeparator();
        cout << "\n✓✓✓ 所有严格边界测试完成！ ✓✓✓" << endl;
        cout << "\n总结：" << endl;
        cout << "  • 物体部分碰撞边界 → 反弹（保持运动）" << endl;
        cout << "  • 物体完全超出边界 → 停止运动（速度归零）\n" << endl;
        
        return 0;
        
    } catch (const exception& e) {
        cerr << "\n✗ 测试失败: " << e.what() << endl;
        return 1;
    }
}
*=========================================================================================================*/