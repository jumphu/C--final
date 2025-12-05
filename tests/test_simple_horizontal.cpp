// 简单测试：验证统一的重力处理函数
#include "../include/physicalWorld.h"
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "========== 测试统一重力处理函数 ==========" << std::endl;
    std::cout << "\n测试场景：单个球在水平地面滑动（倾斜角=0）\n" << std::endl;
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);  // 水平
    world.setTimeStep(1.0 / 60.0);
    
    world.ground.setYLevel(0.0);
    world.ground.setFriction(0.3);
    
    // 手动创建球
    Circle ball(1.0, 5.0, 0.0, 5.0);  // 质量1kg, 半径5m, 位置(0, 5)
    ball.setVelocity(10.0, 0.0);
    ball.setRestitution(0.0);
    ball.setName("TestBall");
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(&ball);
    
    world.start();
    
    std::cout << "初始状态：" << std::endl;
    std::cout << "  位置: (0, 5)" << std::endl;
    std::cout << "  速度: (10, 0) m/s" << std::endl;
    std::cout << "  摩擦系数: 0.3" << std::endl;
    std::cout << "  倾斜角度: 0°" << std::endl;
    
    std::cout << "\n开始模拟（显示前10帧）：\n" << std::endl;
    std::cout << std::setw(6) << "帧"
              << std::setw(12) << "速度(m/s)"
              << std::setw(12) << "位置X(m)" << std::endl;
    std::cout << std::string(30, '-') << std::endl;
    
    for (int frame = 0; frame <= 200; frame++) {
        world.update(shapeList, world.ground);
        
        double x, y, vx, vy;
        ball.getCentre(x, y);
        ball.getVelocity(vx, vy);
        
        if (frame <= 10 || frame % 30 == 0) {
            std::cout << std::setw(6) << frame
                      << std::setw(12) << std::fixed << std::setprecision(3) << vx
                      << std::setw(12) << x << std::endl;
        }
        
        // 检测停止
        if (std::abs(vx) < 0.01) {
            std::cout << "\n? 物体在第 " << frame << " 帧停止" << std::endl;
            std::cout << "  理论停止时间: v?/(μ*g) = 10/(0.3*10) ≈ 3.33s = 200帧" << std::endl;
            std::cout << "  实际停止时间: " << frame << " 帧" << std::endl;
            
            if (frame >= 180 && frame <= 220) {
                std::cout << "\n?? 测试通过！水平摩擦力工作正常" << std::endl;
            } else {
                std::cout << "\n?? 测试失败！停止时间不符合预期" << std::endl;
            }
            break;
        }
    }
    
    return 0;
}
