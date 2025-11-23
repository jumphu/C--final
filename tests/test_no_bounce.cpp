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
 * 测试1：物体穿过边界后停止
 *=========================================================================================================*/
void testPassThroughBoundary() {
    printSeparator("测试1: 物体穿过边界后停止");
    
    cout << "\n场景：物体以不同速度飞向边界" << endl;
    cout << "边界: [0, 100, 0, 100]" << endl;
    cout << "预期：物体穿过边界后立即停止\n" << endl;
    
    PhysicalWorld world(0.0, 100.0, 0.0, 100.0);
    world.setGravity(0.0);
    Ground ground(0.0, 0.0);
    
    // 测试不同速度
    double speeds[] = {10.0, 30.0, 80.0};
    string names[] = {"慢速球", "中速球", "快速球"};
    
    for (int i = 0; i < 3; i++) {
        cout << "\n--- " << names[i] << " (速度=" << speeds[i] << ") ---" << endl;
        
        Circle* ball = new Circle(1.0, 5.0, 85.0, 50.0);
        ball->setVelocity(speeds[i], 0.0);
        ball->setRestitution(0.9);
        
        vector<Shape*> shapes;
        shapes.push_back(ball);
        
        cout << "初始: ";
        printShapeInfo("", *ball);
        
        // 模拟直到停止
        for (int step = 0; step < 15; step++) {
            double vx_before, vy_before;
            ball->getVelocity(vx_before, vy_before);
            
            world.update(shapes, 0.1, ground);
            
            double vx_after, vy_after;
            ball->getVelocity(vx_after, vy_after);
            
            // 检测是否停止
            if (vx_before != 0.0 && vx_after == 0.0) {
                cout << "时间步 " << (step+1) << ": ";
                printShapeInfo("", *ball);
                cout << "  → 已停止（超出边界）" << endl;
                break;
            }
            
            // 显示关键时刻
            if (step == 0 || step == 2 || step == 5) {
                cout << "时间步 " << (step+1) << ": ";
                printShapeInfo("", *ball);
            }
        }
        
        delete ball;
    }
    
    cout << "\n✓ 测试完成：无反弹，只有停止" << endl;
}

/*=========================================================================================================
 * 测试2：边界内自由运动
 *=========================================================================================================*/
void testFreeMovementInside() {
    printSeparator("测试2: 边界内自由运动");
    
    cout << "\n场景：物体在边界内自由运动" << endl;
    cout << "边界: [0, 100, 0, 100]" << endl;
    cout << "预期：物体不受边界影响，自由运动\n" << endl;
    
    PhysicalWorld world(0.0, 100.0, 0.0, 100.0);
    world.setGravity(0.0);
    Ground ground(0.0, 0.0);
    
    Circle* ball = new Circle(1.0, 5.0, 30.0, 50.0);  // 位置在边界内部
    ball->setVelocity(15.0, 10.0);  // 斜向运动
    ball->setRestitution(0.9);
    
    vector<Shape*> shapes;
    shapes.push_back(ball);
    
    cout << "初始状态：" << endl;
    printShapeInfo("  球", *ball);
    
    cout << "\n运动轨迹：" << endl;
    for (int step = 0; step < 8; step++) {
        world.update(shapes, 0.1, ground);
        
        if (step % 2 == 0) {
            cout << "时间步 " << (step+1) << ": ";
            printShapeInfo("", *ball);
        }
    }
    
    double vx, vy;
    ball->getVelocity(vx, vy);
    
    if (vx != 0.0 || vy != 0.0) {
        cout << "\n✓ 测试通过：物体在边界内持续运动，无反弹" << endl;
    } else {
        cout << "\n✗ 意外：物体停止了" << endl;
    }
    
    delete ball;
}

/*=========================================================================================================
 * 测试3：对比有/无边界处理
 *=========================================================================================================*/
void testBoundaryComparison() {
    printSeparator("测试3: 边界处理对比");
    
    cout << "\n场景：对比接近边界和远离边界的物体" << endl;
    cout << "边界: [0, 100, 0, 100]\n" << endl;
    
    PhysicalWorld world(0.0, 100.0, 0.0, 100.0);
    world.setGravity(0.0);
    Ground ground(0.0, 0.0);
    
    // 球1：靠近边界
    Circle* ball1 = new Circle(1.0, 5.0, 85.0, 50.0);
    ball1->setVelocity(25.0, 0.0);
    
    // 球2：远离边界
    Circle* ball2 = new Circle(1.0, 5.0, 30.0, 50.0);
    ball2->setVelocity(25.0, 0.0);
    
    vector<Shape*> shapes;
    shapes.push_back(ball1);
    shapes.push_back(ball2);
    
    cout << "初始状态：" << endl;
    printShapeInfo("  球1 (靠近边界)", *ball1);
    printShapeInfo("  球2 (远离边界)", *ball2);
    
    cout << "\n模拟5个时间步..." << endl;
    for (int step = 0; step < 5; step++) {
        world.update(shapes, 0.1, ground);
    }
    
    cout << "\n最终状态：" << endl;
    double v1x, v1y, v2x, v2y;
    ball1->getVelocity(v1x, v1y);
    ball2->getVelocity(v2x, v2y);
    
    printShapeInfo("  球1", *ball1);
    if (v1x == 0.0) cout << "    → 已停止（超出边界）" << endl;
    
    printShapeInfo("  球2", *ball2);
    if (v2x != 0.0) cout << "    → 持续运动（在边界内）" << endl;
    
    cout << "\n✓ 对比测试完成" << endl;
    
    delete ball1;
    delete ball2;
}

/*=========================================================================================================
 * 测试4：多方向超出边界
 *=========================================================================================================*/
void testMultiDirectionExit() {
    printSeparator("测试4: 多方向超出边界");
    
    cout << "\n场景：物体从四个方向飞出边界" << endl;
    cout << "边界: [0, 100, 0, 100]\n" << endl;
    
    PhysicalWorld world(0.0, 100.0, 0.0, 100.0);
    world.setGravity(0.0);
    Ground ground(0.0, 0.0);
    
    struct TestCase {
        double x, y, vx, vy;
        string direction;
    };
    
    TestCase cases[] = {
        {10.0, 50.0, -50.0, 0.0, "向左"},
        {90.0, 50.0, 50.0, 0.0, "向右"},
        {50.0, 10.0, 0.0, -50.0, "向下"},
        {50.0, 90.0, 0.0, 50.0, "向上"}
    };
    
    for (int i = 0; i < 4; i++) {
        cout << "\n--- " << cases[i].direction << "飞出 ---" << endl;
        
        Circle* ball = new Circle(1.0, 5.0, cases[i].x, cases[i].y);
        ball->setVelocity(cases[i].vx, cases[i].vy);
        
        vector<Shape*> shapes;
        shapes.push_back(ball);
        
        cout << "初始: ";
        printShapeInfo("", *ball);
        
        // 模拟
        bool stopped = false;
        for (int step = 0; step < 10; step++) {
            double vx_before, vy_before;
            ball->getVelocity(vx_before, vy_before);
            
            world.update(shapes, 0.1, ground);
            
            double vx_after, vy_after;
            ball->getVelocity(vx_after, vy_after);
            
            if ((vx_before != 0.0 || vy_before != 0.0) && 
                vx_after == 0.0 && vy_after == 0.0) {
                cout << "时间步 " << (step+1) << ": ";
                printShapeInfo("", *ball);
                cout << "  → 已停止" << endl;
                stopped = true;
                break;
            }
        }
        
        if (!stopped) {
            cout << "未超出边界，继续运动" << endl;
        }
        
        delete ball;
    }
    
    cout << "\n✓ 多方向测试完成" << endl;
}

/*=========================================================================================================
 * 测试5：矩形穿越边界
 *=========================================================================================================*/
void testAABBPassThrough() {
    printSeparator("测试5: 矩形穿越边界");
    
    cout << "\n场景：矩形飞出边界" << endl;
    cout << "边界: [0, 100, 0, 100]\n" << endl;
    
    PhysicalWorld world(0.0, 100.0, 0.0, 100.0);
    world.setGravity(0.0);
    Ground ground(0.0, 0.0);
    
    AABB* box = new AABB(1.0, 10.0, 10.0, 85.0, 50.0);
    box->setVelocity(40.0, 0.0);
    
    vector<Shape*> shapes;
    shapes.push_back(box);
    
    cout << "初始状态：" << endl;
    printShapeInfo("  矩形", *box);
    
    cout << "\n模拟过程：" << endl;
    for (int step = 0; step < 8; step++) {
        double vx_before, vy_before;
        box->getVelocity(vx_before, vy_before);
        
        world.update(shapes, 0.1, ground);
        
        double vx_after, vy_after;
        box->getVelocity(vx_after, vy_after);
        
        cout << "时间步 " << (step+1) << ": ";
        printShapeInfo("", *box);
        
        if (vx_before != 0.0 && vx_after == 0.0) {
            cout << "  → 已停止（超出边界）" << endl;
            break;
        }
    }
    
    cout << "\n✓ 矩形测试完成" << endl;
    delete box;
}

/*=========================================================================================================
 * 主函数
 
int main() {
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║   无反弹边界系统测试                   ║" << endl;
    cout << "║   物体超出边界 → 直接停止              ║" << endl;
    cout << "║   边界内运动 → 不受影响                ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    
    try {
        // 测试1：穿过边界停止
        testPassThroughBoundary();
        
        // 测试2：边界内自由运动
        testFreeMovementInside();
        
        // 测试3：边界处理对比
        testBoundaryComparison();
        
        // 测试4：多方向超出
        testMultiDirectionExit();
        
        // 测试5：矩形测试
        testAABBPassThrough();
        
        printSeparator();
        cout << "\n✓✓✓ 所有无反弹测试完成！ ✓✓✓" << endl;
        cout << "\n新的边界行为：" << endl;
        cout << "  • 物体在边界内 → 自由运动，不受影响" << endl;
        cout << "  • 物体完全超出边界 → 立即停止，速度归零" << endl;
        cout << "  • 没有反弹效果\n" << endl;
        
        return 0;
        
    } catch (const exception& e) {
        cerr << "\n✗ 测试失败: " << e.what() << endl;
        return 1;
    }
}
*=========================================================================================================*/