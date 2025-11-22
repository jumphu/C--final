#include "../include/shapes.h"
#include <iostream>
#include <iomanip>
#include <cassert>

using namespace std;

// 测试辅助函数：比较两个double值是否近似相等
bool approxEqual(double a, double b, double epsilon = 1e-6) {
    return fabs(a - b) < epsilon;
}

// 测试Circle的getter和setter方法
void testCircleGettersSetters() {
    cout << "=== 测试 Circle 的 Getter 和 Setter ===" << endl;
    
    // 1. 测试默认构造函数
    cout << "\n1. 测试默认构造函数:" << endl;
    Circle c1;
    double x, y, vx, vy;
    
    c1.getCentre(x, y);
    c1.getVelocity(vx, vy);
    
    cout << "   质量: " << c1.getMass() << " (期望: 1.0)" << endl;
    cout << "   半径: " << c1.getRadius() << " (期望: 1.0)" << endl;
    cout << "   质心: (" << x << ", " << y << ") (期望: (0, 0))" << endl;
    cout << "   速度: (" << vx << ", " << vy << ") (期望: (0, 0))" << endl;
    
    assert(approxEqual(c1.getMass(), 1.0));
    assert(approxEqual(c1.getRadius(), 1.0));
    assert(approxEqual(x, 0.0) && approxEqual(y, 0.0));
    assert(approxEqual(vx, 0.0) && approxEqual(vy, 0.0));
    cout << "   ? 默认构造函数测试通过" << endl;
    
    // 2. 测试完整构造函数
    cout << "\n2. 测试完整构造函数 Circle(2.5, 3.0, 10.0, 20.0, 1.5, -2.0):" << endl;
    Circle c2(2.5, 3.0, 10.0, 20.0, 1.5, -2.0);
    
    c2.getCentre(x, y);
    c2.getVelocity(vx, vy);
    
    cout << "   质量: " << c2.getMass() << " (期望: 2.5)" << endl;
    cout << "   半径: " << c2.getRadius() << " (期望: 3.0)" << endl;
    cout << "   质心: (" << x << ", " << y << ") (期望: (10, 20))" << endl;
    cout << "   速度: (" << vx << ", " << vy << ") (期望: (1.5, -2))" << endl;
    
    assert(approxEqual(c2.getMass(), 2.5));
    assert(approxEqual(c2.getRadius(), 3.0));
    assert(approxEqual(x, 10.0) && approxEqual(y, 20.0));
    assert(approxEqual(vx, 1.5) && approxEqual(vy, -2.0));
    cout << "   ? 完整构造函数测试通过" << endl;
    
    // 3. 测试 setter 方法
    cout << "\n3. 测试 Setter 方法:" << endl;
    Circle c3;
    
    c3.setMass(5.0);
    c3.setCentre(15.0, 25.0);
    c3.setVelocity(3.0, 4.0);
    
    c3.getCentre(x, y);
    c3.getVelocity(vx, vy);
    
    cout << "   设置后的质量: " << c3.getMass() << " (期望: 5.0)" << endl;
    cout << "   设置后的质心: (" << x << ", " << y << ") (期望: (15, 25))" << endl;
    cout << "   设置后的速度: (" << vx << ", " << vy << ") (期望: (3, 4))" << endl;
    
    assert(approxEqual(c3.getMass(), 5.0));
    assert(approxEqual(x, 15.0) && approxEqual(y, 25.0));
    assert(approxEqual(vx, 3.0) && approxEqual(vy, 4.0));
    cout << "   ? Setter 方法测试通过" << endl;
    
    // 4. 测试 move 方法
    cout << "\n4. 测试 move 方法:" << endl;
    Circle c4(1.0, 2.0, 10.0, 10.0);
    c4.move(5.0, -3.0);
    
    c4.getCentre(x, y);
    cout << "   移动后的质心: (" << x << ", " << y << ") (期望: (15, 7))" << endl;
    
    assert(approxEqual(x, 15.0) && approxEqual(y, 7.0));
    cout << "   ? move 方法测试通过" << endl;
    
    cout << "\n??? Circle 所有测试通过！ ???\n" << endl;
}

// 测试AABB的getter和setter方法
void testAABBGettersSetters() {
    cout << "=== 测试 AABB 的 Getter 和 Setter ===" << endl;
    
    // 1. 测试默认构造函数
    cout << "\n1. 测试默认构造函数:" << endl;
    AABB a1;
    double x, y, vx, vy;
    
    a1.getCentre(x, y);
    a1.getVelocity(vx, vy);
    
    cout << "   质量: " << a1.getMass() << " (期望: 1.0)" << endl;
    cout << "   宽度: " << a1.getWidth() << " (期望: 1.0)" << endl;
    cout << "   高度: " << a1.getHeight() << " (期望: 1.0)" << endl;
    cout << "   质心: (" << x << ", " << y << ") (期望: (0, 0))" << endl;
    cout << "   速度: (" << vx << ", " << vy << ") (期望: (0, 0))" << endl;
    
    assert(approxEqual(a1.getMass(), 1.0));
    assert(approxEqual(a1.getWidth(), 1.0));
    assert(approxEqual(a1.getHeight(), 1.0));
    assert(approxEqual(x, 0.0) && approxEqual(y, 0.0));
    assert(approxEqual(vx, 0.0) && approxEqual(vy, 0.0));
    cout << "   ? 默认构造函数测试通过" << endl;
    
    // 2. 测试完整构造函数
    cout << "\n2. 测试完整构造函数 AABB(3.0, 4.0, 5.0, 10.0, 20.0, 2.0, -1.0):" << endl;
    AABB a2(3.0, 4.0, 5.0, 10.0, 20.0, 2.0, -1.0);
    
    a2.getCentre(x, y);
    a2.getVelocity(vx, vy);
    
    cout << "   质量: " << a2.getMass() << " (期望: 3.0)" << endl;
    cout << "   宽度: " << a2.getWidth() << " (期望: 4.0)" << endl;
    cout << "   高度: " << a2.getHeight() << " (期望: 5.0)" << endl;
    cout << "   质心: (" << x << ", " << y << ") (期望: (10, 20))" << endl;
    cout << "   速度: (" << vx << ", " << vy << ") (期望: (2, -1))" << endl;
    
    assert(approxEqual(a2.getMass(), 3.0));
    assert(approxEqual(a2.getWidth(), 4.0));
    assert(approxEqual(a2.getHeight(), 5.0));
    assert(approxEqual(x, 10.0) && approxEqual(y, 20.0));
    assert(approxEqual(vx, 2.0) && approxEqual(vy, -1.0));
    cout << "   ? 完整构造函数测试通过" << endl;
    
    // 3. 测试 setter 方法
    cout << "\n3. 测试 Setter 方法:" << endl;
    AABB a3;
    
    a3.setMass(7.5);
    a3.setCentre(30.0, 40.0);
    a3.setVelocity(-2.5, 3.5);
    
    a3.getCentre(x, y);
    a3.getVelocity(vx, vy);
    
    cout << "   设置后的质量: " << a3.getMass() << " (期望: 7.5)" << endl;
    cout << "   设置后的质心: (" << x << ", " << y << ") (期望: (30, 40))" << endl;
    cout << "   设置后的速度: (" << vx << ", " << vy << ") (期望: (-2.5, 3.5))" << endl;
    
    assert(approxEqual(a3.getMass(), 7.5));
    assert(approxEqual(x, 30.0) && approxEqual(y, 40.0));
    assert(approxEqual(vx, -2.5) && approxEqual(vy, 3.5));
    cout << "   ? Setter 方法测试通过" << endl;
    
    // 4. 测试 move 方法
    cout << "\n4. 测试 move 方法:" << endl;
    AABB a4(1.0, 10.0, 8.0, 50.0, 60.0);
    a4.move(-10.0, 5.0);
    
    a4.getCentre(x, y);
    cout << "   移动后的质心: (" << x << ", " << y << ") (期望: (40, 65))" << endl;
    
    assert(approxEqual(x, 40.0) && approxEqual(y, 65.0));
    cout << "   ? move 方法测试通过" << endl;
    
    // 5. 测试双参数构造函数
    cout << "\n5. 测试双参数构造函数 AABB(6.0, 8.0):" << endl;
    AABB a5(6.0, 8.0);
    
    cout << "   宽度: " << a5.getWidth() << " (期望: 6.0)" << endl;
    cout << "   高度: " << a5.getHeight() << " (期望: 8.0)" << endl;
    cout << "   质量: " << a5.getMass() << " (期望: 1.0)" << endl;
    
    assert(approxEqual(a5.getWidth(), 6.0));
    assert(approxEqual(a5.getHeight(), 8.0));
    assert(approxEqual(a5.getMass(), 1.0));
    cout << "   ? 双参数构造函数测试通过" << endl;
    
    cout << "\n??? AABB 所有测试通过！ ???\n" << endl;
}

// 测试碰撞检测
void testCollisionDetection() {
    cout << "=== 测试碰撞检测 ===" << endl;
    
    // 1. Circle vs Circle - 碰撞
    cout << "\n1. 测试 Circle vs Circle (碰撞):" << endl;
    Circle c1(1.0, 2.0, 0.0, 0.0);   // 半径2，中心(0,0)
    Circle c2(1.0, 2.0, 3.0, 0.0);   // 半径2，中心(3,0)
    
    bool collision1 = c1.check_collision(c2);
    cout << "   圆1中心(0,0)半径2 vs 圆2中心(3,0)半径2" << endl;
    cout << "   距离=3, 半径和=4, 碰撞: " << (collision1 ? "是" : "否") << " (期望: 是)" << endl;
    assert(collision1 == true);
    cout << "   ? Circle-Circle 碰撞测试通过" << endl;
    
    // 2. Circle vs Circle - 不碰撞
    cout << "\n2. 测试 Circle vs Circle (不碰撞):" << endl;
    Circle c3(1.0, 2.0, 0.0, 0.0);   // 半径2，中心(0,0)
    Circle c4(1.0, 2.0, 10.0, 0.0);  // 半径2，中心(10,0)
    
    bool collision2 = c3.check_collision(c4);
    cout << "   圆1中心(0,0)半径2 vs 圆2中心(10,0)半径2" << endl;
    cout << "   距离=10, 半径和=4, 碰撞: " << (collision2 ? "是" : "否") << " (期望: 否)" << endl;
    assert(collision2 == false);
    cout << "   ? Circle-Circle 不碰撞测试通过" << endl;
    
    // 3. AABB vs AABB - 碰撞
    cout << "\n3. 测试 AABB vs AABB (碰撞):" << endl;
    AABB a1(1.0, 4.0, 4.0, 0.0, 0.0);  // 宽4高4，中心(0,0)
    AABB a2(1.0, 4.0, 4.0, 3.0, 0.0);  // 宽4高4，中心(3,0)
    
    bool collision3 = a1.check_collision(a2);
    cout << "   矩形1中心(0,0)宽4高4 vs 矩形2中心(3,0)宽4高4" << endl;
    cout << "   碰撞: " << (collision3 ? "是" : "否") << " (期望: 是)" << endl;
    assert(collision3 == true);
    cout << "   ? AABB-AABB 碰撞测试通过" << endl;
    
    // 4. AABB vs AABB - 不碰撞
    cout << "\n4. 测试 AABB vs AABB (不碰撞):" << endl;
    AABB a3(1.0, 4.0, 4.0, 0.0, 0.0);   // 宽4高4，中心(0,0)
    AABB a4(1.0, 4.0, 4.0, 10.0, 0.0);  // 宽4高4，中心(10,0)
    
    bool collision4 = a3.check_collision(a4);
    cout << "   矩形1中心(0,0)宽4高4 vs 矩形2中心(10,0)宽4高4" << endl;
    cout << "   碰撞: " << (collision4 ? "是" : "否") << " (期望: 否)" << endl;
    assert(collision4 == false);
    cout << "   ? AABB-AABB 不碰撞测试通过" << endl;
    
    // 5. Circle vs AABB - 碰撞
    cout << "\n5. 测试 Circle vs AABB (碰撞):" << endl;
    Circle c5(1.0, 2.0, 0.0, 0.0);     // 半径2，中心(0,0)
    AABB a5(1.0, 4.0, 4.0, 3.0, 0.0);  // 宽4高4，中心(3,0)
    
    bool collision5 = c5.check_collision(a5);
    cout << "   圆中心(0,0)半径2 vs 矩形中心(3,0)宽4高4" << endl;
    cout << "   碰撞: " << (collision5 ? "是" : "否") << " (期望: 是)" << endl;
    assert(collision5 == true);
    cout << "   ? Circle-AABB 碰撞测试通过" << endl;
    
    cout << "\n??? 所有碰撞检测测试通过！ ???\n" << endl;
}

// 主函数
int main() {
    cout << "\n";
    cout << "╔════════════════════════════════════════════╗" << endl;
    cout << "║   Shapes 库测试程序                        ║" << endl;
    cout << "║   测试 Circle 和 AABB 的 Get/Set 方法      ║" << endl;
    cout << "╚════════════════════════════════════════════╝" << endl;
    cout << "\n";
    
    try {
        // 测试 Circle
        testCircleGettersSetters();
        
        // 测试 AABB
        testAABBGettersSetters();
        
        // 测试碰撞检测
        testCollisionDetection();
        
        cout << "\n";
        cout << "╔════════════════════════════════════════════╗" << endl;
        cout << "║   ?? 所有测试通过！                        ║" << endl;
        cout << "╚════════════════════════════════════════════╝" << endl;
        cout << "\n";
        
        return 0;
        
    } catch (const exception& e) {
        cerr << "\n? 测试失败: " << e.what() << endl;
        return 1;
    }
}
