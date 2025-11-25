#include "../include/physicalWorld.h"
#include "../include/shapes.h"
#include <iostream>
#include <iomanip>

using namespace std;

/*=========================================================================================================
 * timeStep 功能测试
 * 测试 PhysicalWorld 的两种 update() 模式
 *=========================================================================================================*/

int main() {
    cout << "\n========================================" << endl;
    cout << "  timeStep 功能测试" << endl;
    cout << "========================================\n" << endl;
    
    PhysicalWorld world;
    Ground ground(0.0, 0.1);
    
    // 测试1：获取默认时间步长
    cout << "测试1：默认时间步长" << endl;
    cout << "  默认 timeStep = " << fixed << setprecision(6) << world.getTimeStep() << " 秒" << endl;
    cout << "  对应 FPS = " << (1.0 / world.getTimeStep()) << endl;
    
    // 测试2：设置时间步长
    cout << "\n测试2：设置时间步长" << endl;
    world.setTimeStep(0.01);
    cout << "  设置后 timeStep = " << world.getTimeStep() << " 秒" << endl;
    cout << "  对应 FPS = " << (1.0 / world.getTimeStep()) << endl;
    
    // 测试3：使用模式1（成员变量 timeStep）
    cout << "\n测试3：使用模式1（简洁模式）" << endl;
    {
        vector<Shape*> shapes;
        Circle* ball = new Circle(1.0, 0.5, 5.0, 10.0);
        ball->setVelocity(10.0, 0.0);
        shapes.push_back(ball);
        
        world.setTimeStep(0.01);
        cout << "  时间步长: " << world.getTimeStep() << "s" << endl;
        cout << "  初始位置: ";
        double x, y;
        ball->getCentre(x, y);
        cout << "(" << x << ", " << y << ")" << endl;
        
        // 模拟10帧
        for (int i = 0; i < 10; i++) {
            world.update(shapes, ground);  // ? 使用成员变量 timeStep
        }
        
        ball->getCentre(x, y);
        cout << "  最终位置: (" << x << ", " << y << ")" << endl;
        cout << "  移动距离: " << (x - 5.0) << " 米" << endl;
        cout << "  预期距离: " << (10.0 * 0.01 * 10) << " 米" << endl;
        
        delete ball;
    }
    
    // 测试4：使用模式2（参数 deltaTime）
    cout << "\n测试4：使用模式2（灵活模式）" << endl;
    {
        vector<Shape*> shapes;
        Circle* ball = new Circle(1.0, 0.5, 5.0, 10.0);
        ball->setVelocity(10.0, 0.0);
        shapes.push_back(ball);
        
        cout << "  自定义时间步长: 0.001s" << endl;
        cout << "  初始位置: ";
        double x, y;
        ball->getCentre(x, y);
        cout << "(" << x << ", " << y << ")" << endl;
        
        // 模拟100帧（使用自定义时间步长）
        for (int i = 0; i < 100; i++) {
            world.update(shapes, 0.001, ground);  // ? 使用参数 deltaTime
        }
        
        ball->getCentre(x, y);
        cout << "  最终位置: (" << x << ", " << y << ")" << endl;
        cout << "  移动距离: " << (x - 5.0) << " 米" << endl;
        cout << "  预期距离: " << (10.0 * 0.001 * 100) << " 米" << endl;
        
        delete ball;
    }
    
    // 测试5：混合使用
    cout << "\n测试5：混合使用" << endl;
    {
        vector<Shape*> shapes;
        Circle* ball = new Circle(1.0, 0.5, 0.0, 10.0);
        ball->setVelocity(5.0, 0.0);
        shapes.push_back(ball);
        
        world.setTimeStep(0.1);  // 设置默认时间步长
        
        cout << "  阶段1：使用默认时间步长 (0.1s)" << endl;
        double x, y;
        ball->getCentre(x, y);
        cout << "    初始: x = " << x << endl;
        
        for (int i = 0; i < 5; i++) {
            world.update(shapes, ground);  // 使用 0.1s
        }
        
        ball->getCentre(x, y);
        cout << "    5帧后: x = " << x << " (移动 " << x << " 米)" << endl;
        
        cout << "  阶段2：临时使用高精度 (0.001s)" << endl;
        for (int i = 0; i < 100; i++) {
            world.update(shapes, 0.001, ground);  // 临时使用 0.001s
        }
        
        ball->getCentre(x, y);
        cout << "    100帧后: x = " << x << endl;
        
        cout << "  阶段3：恢复默认时间步长" << endl;
        for (int i = 0; i < 5; i++) {
            world.update(shapes, ground);  // 又回到 0.1s
        }
        
        ball->getCentre(x, y);
        cout << "    5帧后: x = " << x << endl;
        cout << "    总移动距离: " << x << " 米" << endl;
        
        delete ball;
    }
    
    // 测试6：防御性编程（负数时间步长）
    cout << "\n测试6：防御性编程测试" << endl;
    world.setTimeStep(-0.01);
    cout << "  尝试设置负数 timeStep (-0.01)" << endl;
    cout << "  实际 timeStep = " << world.getTimeStep() << " 秒" << endl;
    if (world.getTimeStep() > 0) {
        cout << "  ? 通过：负数被拒绝，保持原值" << endl;
    } else {
        cout << "  ? 失败：接受了负数" << endl;
    }
    
    world.setTimeStep(0.0);
    cout << "\n  尝试设置零 timeStep (0.0)" << endl;
    cout << "  实际 timeStep = " << world.getTimeStep() << " 秒" << endl;
    if (world.getTimeStep() > 0) {
        cout << "  ? 通过：零被拒绝，保持原值" << endl;
    } else {
        cout << "  ? 失败：接受了零" << endl;
    }
    
    cout << "\n========================================" << endl;
    cout << "  所有测试完成！" << endl;
    cout << "========================================" << endl;
    
    return 0;
}
