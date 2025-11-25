#include "../include/physicalWorld.h"
#include "../include/shapes.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

using namespace std;

/*=========================================================================================================
 * 全面的碰撞测试套件
 * 测试物理引擎的碰撞检测、碰撞响应、分离机制
 *=========================================================================================================*/

// 颜色输出（用于美化输出）
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// 测试统计
int totalTests = 0;
int passedTests = 0;
int failedTests = 0;

// 辅助函数：比较浮点数
bool approxEqual(double a, double b, double epsilon = 0.1) {
    return fabs(a - b) < epsilon;
}

// 辅助函数：打印测试结果
void printTestResult(const string& testName, bool passed, const string& message = "") {
    totalTests++;
    if (passed) {
        passedTests++;
        cout << GREEN << "? PASS" << RESET << " - " << testName;
        if (!message.empty()) cout << " (" << message << ")";
        cout << endl;
    } else {
        failedTests++;
        cout << RED << "? FAIL" << RESET << " - " << testName;
        if (!message.empty()) cout << " (" << message << ")";
        cout << endl;
    }
}

// 辅助函数：打印分隔线
void printSeparator(const string& title = "") {
    cout << "\n" << CYAN << "========================================" << RESET << endl;
    if (!title.empty()) {
        cout << CYAN << "  " << title << RESET << endl;
        cout << CYAN << "========================================" << RESET << endl;
    }
}

// 辅助函数：打印形状信息
void printShapeInfo(const string& name, Shape& shape) {
    double x, y, vx, vy;
    shape.getCentre(x, y);
    shape.getVelocity(vx, vy);
    
    cout << "  " << name << ":" << endl;
    cout << "    位置: (" << fixed << setprecision(2) << x << ", " << y << ")" << endl;
    cout << "    速度: (" << vx << ", " << vy << ")" << endl;
    cout << "    质量: " << shape.getMass() << endl;
}

/*=========================================================================================================
 * 第一部分：基础碰撞检测测试
 *=========================================================================================================*/

void testCollisionDetection() {
    printSeparator("第一部分：碰撞检测测试");
    
    // 测试1：Circle-Circle 碰撞检测
    {
        Circle c1(1.0, 1.0, 0.0, 0.0);  // 半径1，中心(0,0)
        Circle c2(1.0, 1.0, 1.5, 0.0);  // 半径1，中心(1.5,0)
        
        bool colliding = c1.check_collision(c2);
        printTestResult("Circle-Circle 碰撞检测（重叠）", colliding, "距离1.5 < 半径和2.0");
    }
    
    // 测试2：Circle-Circle 无碰撞
    {
        Circle c1(1.0, 1.0, 0.0, 0.0);
        Circle c2(1.0, 1.0, 3.0, 0.0);
        
        bool colliding = c1.check_collision(c2);
        printTestResult("Circle-Circle 无碰撞", !colliding, "距离3.0 > 半径和2.0");
    }
    
    // 测试3：AABB-AABB 碰撞检测
    {
        AABB a1(1.0, 2.0, 2.0, 0.0, 0.0);   // 宽2高2，中心(0,0)
        AABB a2(1.0, 2.0, 2.0, 1.5, 0.0);   // 宽2高2，中心(1.5,0)
        
        bool colliding = a1.check_collision(a2);
        printTestResult("AABB-AABB 碰撞检测（重叠）", colliding, "X方向重叠0.5");
    }
    
    // 测试4：AABB-AABB 无碰撞
    {
        AABB a1(1.0, 2.0, 2.0, 0.0, 0.0);
        AABB a2(1.0, 2.0, 2.0, 3.0, 0.0);
        
        bool colliding = a1.check_collision(a2);
        printTestResult("AABB-AABB 无碰撞", !colliding, "X方向无重叠");
    }
    
    // 测试5：Circle-AABB 碰撞检测
    {
        Circle c(1.0, 1.0, 0.0, 0.0);       // 半径1，中心(0,0)
        AABB a(1.0, 2.0, 2.0, 2.0, 0.0);    // 宽2高2，中心(2,0)
        
        bool colliding = c.check_collision(a);
        printTestResult("Circle-AABB 碰撞检测（边缘接触）", colliding);
    }
}

/*=========================================================================================================
 * 第二部分：弹性碰撞测试
 *=========================================================================================================*/

void testElasticCollisions() {
    printSeparator("第二部分：弹性碰撞测试");
    
    PhysicalWorld world;
    
    // 测试1：完全弹性碰撞（e=1.0）
    {
        Circle c1(1.0, 1.0, 0.0, 0.0);
        Circle c2(1.0, 1.0, 1.9, 0.0);
        
        c1.setVelocity(10.0, 0.0);
        c2.setVelocity(-10.0, 0.0);
        c1.setRestitution(1.0);
        c2.setRestitution(1.0);
        
        cout << "\n测试：完全弹性碰撞（相同质量，相向运动）" << endl;
        printShapeInfo("碰撞前 球1", c1);
        printShapeInfo("碰撞前 球2", c2);
        
        world.resolveCollision(c1, c2);
        
        printShapeInfo("碰撞后 球1", c1);
        printShapeInfo("碰撞后 球2", c2);
        
        double v1x, v1y, v2x, v2y;
        c1.getVelocity(v1x, v1y);
        c2.getVelocity(v2x, v2y);
        
        bool speedExchanged = approxEqual(v1x, -10.0) && approxEqual(v2x, 10.0);
        printTestResult("速度交换", speedExchanged, "v1: " + to_string(v1x) + ", v2: " + to_string(v2x));
    }
    
    // 测试2：不同质量弹性碰撞
    {
        Circle heavy(10.0, 1.0, 0.0, 0.0);
        Circle light(1.0, 1.0, 1.9, 0.0);
        
        heavy.setVelocity(5.0, 0.0);
        light.setVelocity(0.0, 0.0);
        heavy.setRestitution(1.0);
        light.setRestitution(1.0);
        
        cout << "\n测试：重球撞轻球（质量比10:1）" << endl;
        printShapeInfo("碰撞前 重球", heavy);
        printShapeInfo("碰撞前 轻球", light);
        
        world.resolveCollision(heavy, light);
        
        printShapeInfo("碰撞后 重球", heavy);
        printShapeInfo("碰撞后 轻球", light);
        
        double vHeavy, vLight, vy;
        heavy.getVelocity(vHeavy, vy);
        light.getVelocity(vLight, vy);
        
        bool heavySlowsDown = vHeavy < 5.0 && vHeavy > 4.0;
        bool lightSpeedsUp = vLight > 8.0;
        
        printTestResult("重球减速（4-5）", heavySlowsDown, "v_heavy: " + to_string(vHeavy));
        printTestResult("轻球加速（>8）", lightSpeedsUp, "v_light: " + to_string(vLight));
    }
    
    // 测试3：固定墙壁碰撞（无穷大质量）
    {
        Circle ball(1.0, 1.0, 0.0, 0.0);
        AABB wall(INFINITY, 2.0, 10.0, 1.9, 0.0);
        
        ball.setVelocity(10.0, 0.0);
        wall.setVelocity(0.0, 0.0);
        ball.setRestitution(1.0);
        wall.setRestitution(1.0);
        
        cout << "\n测试：球撞固定墙壁" << endl;
        printShapeInfo("碰撞前 球", ball);
        
        world.resolveCollision(ball, wall);
        
        printShapeInfo("碰撞后 球", ball);
        printShapeInfo("碰撞后 墙", wall);
        
        double vBall, vWall, vy;
        ball.getVelocity(vBall, vy);
        wall.getVelocity(vWall, vy);
        
        bool ballBounces = approxEqual(vBall, -10.0, 0.5);
        bool wallStays = approxEqual(vWall, 0.0);
        
        printTestResult("球反弹（v≈-10）", ballBounces, "v_ball: " + to_string(vBall));
        printTestResult("墙不动（v=0）", wallStays, "v_wall: " + to_string(vWall));
    }
}

/*=========================================================================================================
 * 第三部分：非弹性碰撞测试
 *=========================================================================================================*/

void testInelasticCollisions() {
    printSeparator("第三部分：非弹性碰撞测试");
    
    PhysicalWorld world;
    
    // 测试1：完全非弹性碰撞（e=0）
    {
        Circle c1(1.0, 1.0, 0.0, 0.0);
        Circle c2(1.0, 1.0, 1.9, 0.0);
        
        c1.setVelocity(10.0, 0.0);
        c2.setVelocity(-10.0, 0.0);
        c1.setRestitution(0.0);
        c2.setRestitution(0.0);
        
        cout << "\n测试：完全非弹性碰撞（e=0）" << endl;
        printShapeInfo("碰撞前 球1", c1);
        printShapeInfo("碰撞前 球2", c2);
        
        world.resolveCollision(c1, c2);
        
        printShapeInfo("碰撞后 球1", c1);
        printShapeInfo("碰撞后 球2", c2);
        
        double v1x, v2x, vy;
        c1.getVelocity(v1x, vy);
        c2.getVelocity(v2x, vy);
        
        bool velocitiesSimilar = approxEqual(v1x, v2x, 1.0);
        bool nearZero = fabs(v1x) < 2.0 && fabs(v2x) < 2.0;
        
        printTestResult("速度趋于相等", velocitiesSimilar, "v1: " + to_string(v1x) + ", v2: " + to_string(v2x));
        printTestResult("速度接近零", nearZero);
    }
    
    // 测试2：部分弹性碰撞（e=0.5）
    {
        Circle c1(1.0, 1.0, 0.0, 0.0);
        Circle c2(1.0, 1.0, 1.9, 0.0);
        
        c1.setVelocity(10.0, 0.0);
        c2.setVelocity(0.0, 0.0);
        c1.setRestitution(0.5);
        c2.setRestitution(0.5);
        
        cout << "\n测试：部分弹性碰撞（e=0.5）" << endl;
        double initialKE = 0.5 * 1.0 * 10.0 * 10.0;  // 初始动能
        
        world.resolveCollision(c1, c2);
        
        double v1x, v2x, vy;
        c1.getVelocity(v1x, vy);
        c2.getVelocity(v2x, vy);
        
        double finalKE = 0.5 * 1.0 * v1x * v1x + 0.5 * 1.0 * v2x * v2x;
        
        bool energyLost = finalKE < initialKE;
        
        printTestResult("能量损失", energyLost, 
                       "初始KE: " + to_string(initialKE) + ", 最终KE: " + to_string(finalKE));
    }
}

/*=========================================================================================================
 * 第四部分：动量守恒测试
 *=========================================================================================================*/

void testMomentumConservation() {
    printSeparator("第四部分：动量守恒测试");
    
    PhysicalWorld world;
    
    // 测试1：正面碰撞动量守恒
    {
        Circle c1(2.0, 1.0, 0.0, 0.0);
        Circle c2(3.0, 1.0, 1.9, 0.0);
        
        c1.setVelocity(10.0, 0.0);
        c2.setVelocity(-5.0, 0.0);
        c1.setRestitution(0.8);
        c2.setRestitution(0.8);
        
        cout << "\n测试：动量守恒验证" << endl;
        
        double p_before = 2.0 * 10.0 + 3.0 * (-5.0);
        cout << "  碰撞前总动量: " << p_before << " kg·m/s" << endl;
        
        world.resolveCollision(c1, c2);
        
        double v1x, v2x, vy;
        c1.getVelocity(v1x, vy);
        c2.getVelocity(v2x, vy);
        
        double p_after = 2.0 * v1x + 3.0 * v2x;
        cout << "  碰撞后总动量: " << p_after << " kg·m/s" << endl;
        cout << "  动量差异: " << fabs(p_after - p_before) << endl;
        
        bool momentumConserved = approxEqual(p_before, p_after, 0.5);
        printTestResult("动量守恒", momentumConserved);
    }
    
    // 测试2：斜向碰撞动量守恒
    {
        Circle c1(1.0, 1.0, 0.0, 0.0);
        Circle c2(1.0, 1.0, 1.9, 0.0);
        
        c1.setVelocity(10.0, 5.0);   // 斜向
        c2.setVelocity(-5.0, -3.0);  // 斜向
        c1.setRestitution(1.0);
        c2.setRestitution(1.0);
        
        cout << "\n测试：斜向碰撞动量守恒" << endl;
        
        double px_before = 1.0 * 10.0 + 1.0 * (-5.0);
        double py_before = 1.0 * 5.0 + 1.0 * (-3.0);
        
        world.resolveCollision(c1, c2);
        
        double v1x, v1y, v2x, v2y;
        c1.getVelocity(v1x, v1y);
        c2.getVelocity(v2x, v2y);
        
        double px_after = 1.0 * v1x + 1.0 * v2x;
        double py_after = 1.0 * v1y + 1.0 * v2y;
        
        bool xMomentumConserved = approxEqual(px_before, px_after, 0.5);
        bool yMomentumConserved = approxEqual(py_before, py_after, 0.5);
        
        printTestResult("X方向动量守恒", xMomentumConserved, 
                       "前: " + to_string(px_before) + ", 后: " + to_string(px_after));
        printTestResult("Y方向动量守恒", yMomentumConserved,
                       "前: " + to_string(py_before) + ", 后: " + to_string(py_after));
    }
}

/*=========================================================================================================
 * 第五部分：重叠分离测试
 *=========================================================================================================*/

void testOverlapSeparation() {
    printSeparator("第五部分：重叠分离测试");
    
    PhysicalWorld world;
    
    // 测试1：Circle-Circle 分离
    {
        Circle c1(1.0, 1.0, 0.0, 0.0);
        Circle c2(1.0, 1.0, 1.5, 0.0);  // 重叠0.5
        
        cout << "\n测试：Circle-Circle 重叠分离" << endl;
        cout << "  初始距离: 1.5 (重叠0.5)" << endl;
        
        c1.setVelocity(1.0, 0.0);
        c2.setVelocity(-1.0, 0.0);
        
        world.resolveCollision(c1, c2);
        
        double x1, y1, x2, y2;
        c1.getCentre(x1, y1);
        c2.getCentre(x2, y2);
        
        double distance = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
        cout << "  分离后距离: " << distance << endl;
        
        bool separated = distance >= 1.9;  // 应该接近2.0
        printTestResult("物体已分离", separated, "距离: " + to_string(distance));
    }
    
    // 测试2：AABB-AABB 分离
    {
        AABB a1(1.0, 2.0, 2.0, 0.0, 0.0);
        AABB a2(1.0, 2.0, 2.0, 1.5, 0.0);  // X方向重叠0.5
        
        cout << "\n测试：AABB-AABB 重叠分离" << endl;
        
        a1.setVelocity(1.0, 0.0);
        a2.setVelocity(-1.0, 0.0);
        
        world.resolveCollision(a1, a2);
        
        double x1, y1, x2, y2;
        a1.getCentre(x1, y1);
        a2.getCentre(x2, y2);
        
        double xDistance = fabs(x2 - x1);
        bool separated = xDistance >= 1.9;  // 应该接近2.0（宽度和）
        
        printTestResult("物体已分离", separated, "X距离: " + to_string(xDistance));
    }
    
    // 测试3：质量比例分离
    {
        Circle heavy(10.0, 1.0, 0.0, 0.0);
        Circle light(1.0, 1.0, 1.5, 0.0);
        
        cout << "\n测试：质量比例分离（10:1）" << endl;
        
        double x1_before, y1, x2_before, y2;
        heavy.getCentre(x1_before, y1);
        light.getCentre(x2_before, y2);
        
        world.resolveCollision(heavy, light);
        
        double x1_after, x2_after;
        heavy.getCentre(x1_after, y1);
        light.getCentre(x2_after, y2);
        
        double heavyMove = fabs(x1_after - x1_before);
        double lightMove = fabs(x2_after - x2_before);
        
        cout << "  重球移动: " << heavyMove << endl;
        cout << "  轻球移动: " << lightMove << endl;
        
        bool heavyMovesLess = heavyMove < lightMove;
        printTestResult("重球移动更少", heavyMovesLess, 
                       "比例: " + to_string(lightMove / heavyMove));
    }
}

/*=========================================================================================================
 * 第六部分：多物体碰撞测试
 *=========================================================================================================*/

void testMultipleCollisions() {
    printSeparator("第六部分：多物体碰撞测试");
    
    PhysicalWorld world;
    Ground ground(0.0, 0.3);
    
    // 测试：连续碰撞
    {
        vector<Shape*> shapes;
        
        // ? 调整初始位置，让球刚好接触
        Circle* c1 = new Circle(1.0, 0.5, 0.0, 2.0);     // 半径0.5，中心x=0
        Circle* c2 = new Circle(1.0, 0.5, 1.0, 2.0);     // 半径0.5，中心x=1.0（距离刚好1.0）
        Circle* c3 = new Circle(1.0, 0.5, 2.0, 2.0);     // 半径0.5，中心x=2.0
        
        c1->setVelocity(5.0, 0.0);   // 速度5 m/s
        c2->setVelocity(0.0, 0.0);
        c3->setVelocity(0.0, 0.0);
        
        c1->setRestitution(1.0);  // 完全弹性
        c2->setRestitution(1.0);
        c3->setRestitution(1.0);
        
        shapes.push_back(c1);
        shapes.push_back(c2);
        shapes.push_back(c3);
        
        cout << "\n测试：三球连续碰撞（优化版）" << endl;
        cout << "  初始：球1(v=5.0) -> 球2(v=0) -> 球3(v=0)" << endl;
        cout << "  球间距: 1.0 (半径0.5 + 0.5)" << endl;
        
        // ? 直接碰撞（不需要时间步长）
        cout << "\n  第一次碰撞：球1 -> 球2" << endl;
        world.resolveCollision(*c1, *c2);
        
        double v1, v2, v3, vy;
        c1->getVelocity(v1, vy);
        c2->getVelocity(v2, vy);
        c3->getVelocity(v3, vy);
        cout << "  碰撞后：v1=" << v1 << ", v2=" << v2 << ", v3=" << v3 << endl;
        
        cout << "\n  第二次碰撞：球2 -> 球3" << endl;
        world.resolveCollision(*c2, *c3);
        
        c1->getVelocity(v1, vy);
        c2->getVelocity(v2, vy);
        c3->getVelocity(v3, vy);
        cout << "  碰撞后：v1=" << v1 << ", v2=" << v2 << ", v3=" << v3 << endl;
        
        bool energyTransferred = v3 > 3.0;  // 理论上v3应该接近5.0
        printTestResult("能量传递到球3", energyTransferred, "v3: " + to_string(v3));
        
        // 清理
        for (auto shape : shapes) delete shape;
    }
}

/*=========================================================================================================
 * 第七部分：边界情况测试
 *=========================================================================================================*/

void testEdgeCases() {
    printSeparator("第七部分：边界情况测试");
    
    PhysicalWorld world;
    
    // 测试1：零速度碰撞
    {
        Circle c1(1.0, 1.0, 0.0, 0.0);
        Circle c2(1.0, 1.0, 1.5, 0.0);
        
        c1.setVelocity(0.0, 0.0);
        c2.setVelocity(0.0, 0.0);
        
        world.resolveCollision(c1, c2);
        
        double v1, v2, vy;
        c1.getVelocity(v1, vy);
        c2.getVelocity(v2, vy);
        
        bool staysStatic = approxEqual(v1, 0.0) && approxEqual(v2, 0.0);
        printTestResult("零速度碰撞保持静止", staysStatic);
    }
    
    // 测试2：相同方向运动（追赶）
    {
        Circle c1(1.0, 1.0, 0.0, 0.0);
        Circle c2(1.0, 1.0, 1.9, 0.0);
        
        c1.setVelocity(10.0, 0.0);
        c2.setVelocity(5.0, 0.0);
        
        cout << "\n测试：追赶碰撞（快球追慢球）" << endl;
        
        world.resolveCollision(c1, c2);
        
        double v1, v2, vy;
        c1.getVelocity(v1, vy);
        c2.getVelocity(v2, vy);
        
        bool c1SlowsDown = v1 < 10.0;
        bool c2SpeedsUp = v2 > 5.0;
        
        printTestResult("快球减速", c1SlowsDown, "v1: " + to_string(v1));
        printTestResult("慢球加速", c2SpeedsUp, "v2: " + to_string(v2));
    }
    
    // 测试3：刚好接触（距离等于半径和）
    {
        Circle c1(1.0, 1.0, 0.0, 0.0);
        Circle c2(1.0, 1.0, 2.0, 0.0);  // 刚好接触
        
        c1.setVelocity(1.0, 0.0);
        c2.setVelocity(0.0, 0.0);
        
        bool colliding = c1.check_collision(c2);
        printTestResult("刚好接触时碰撞检测", !colliding, "距离=2.0（边界情况）");
    }
}

/*=========================================================================================================
 * 主函数
 *=========================================================================================================*/

int main() {
    cout << "\n";
    cout << MAGENTA << "╔════════════════════════════════════════╗" << RESET << endl;
    cout << MAGENTA << "║     全面的碰撞测试套件 v2.0            ║" << RESET << endl;
    cout << MAGENTA << "║   测试物理引擎的碰撞系统               ║" << RESET << endl;
    cout << MAGENTA << "╚════════════════════════════════════════╝" << RESET << endl;
    
    try {
        testCollisionDetection();
        testElasticCollisions();
        testInelasticCollisions();
        testMomentumConservation();
        testOverlapSeparation();
        testMultipleCollisions();
        testEdgeCases();
        
        // 打印测试统计
        printSeparator();
        cout << "\n" << YELLOW << "测试统计：" << RESET << endl;
        cout << "  总测试数: " << totalTests << endl;
        cout << "  " << GREEN << "? 通过: " << passedTests << RESET << endl;
        cout << "  " << RED << "? 失败: " << failedTests << RESET << endl;
        
        double passRate = (totalTests > 0) ? (100.0 * passedTests / totalTests) : 0.0;
        cout << "  通过率: " << fixed << setprecision(1) << passRate << "%" << endl;
        
        if (failedTests == 0) {
            cout << "\n" << GREEN << "?? 恭喜！所有测试通过！" << RESET << endl;
        } else {
            cout << "\n" << YELLOW << "??  有 " << failedTests << " 个测试失败，请检查。" << RESET << endl;
        }
        
        return (failedTests == 0) ? 0 : 1;
        
    } catch (const exception& e) {
        cerr << "\n" << RED << "? 测试失败: " << e.what() << RESET << endl;
        return 1;
    }
}
