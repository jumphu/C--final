/*=========================================================================================================
 * 简单的自由落体测试 - 隔离问题
 *=========================================================================================================*/

#include <iostream>
#include <iomanip>
#include "physicalWorld.h"
#include "shapes.h"

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  简单自由落体测试" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setTimeStep(1.0 / 60.0);
    world.setInclineAngle(0.0);  // 水平地面
    
    // 地面在 Y=0
    world.ground.setYLevel(0.0);
    
    // 创建一个简单的球，初始高度 Y=10
    Circle* ball = new Circle(1.0, 1.0, 0.0, 10.0);
    ball->setName("Ball");
    ball->setVelocity(0.0, 0.0);  // 初始速度为0
    
    world.addDynamicShape(ball);
    
    std::cout << "初始状态：" << std::endl;
    double x, y, vx, vy;
    ball->getCentre(x, y);
    ball->getVelocity(vx, vy);
    std::cout << "  位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "  底部Y: " << ball->getBottom() << std::endl;
    std::cout << "  速度: (" << vx << ", " << vy << ")" << std::endl;
    std::cout << "  地面Y: " << world.ground.getYLevel() << "\n" << std::endl;
    
    // 理论计算：从10米高度下落到地面（底部=1米）需要的时间
    // h = 9米，t = sqrt(2h/g) = sqrt(18/10) ≈ 1.34秒
    // 需要约 80 帧
    
    std::cout << "运行物理模拟...\n" << std::endl;
    
    for (int i = 0; i <= 120; ++i) {  // 2秒
        world.update(world.dynamicShapeList, world.ground);
        
        if (i % 20 == 0 || (i >= 70 && i <= 90 && i % 5 == 0)) {  // 关键帧附近更频繁输出
            ball->getCentre(x, y);
            ball->getVelocity(vx, vy);
            
            std::cout << std::fixed << std::setprecision(4);
            std::cout << "帧 " << std::setw(3) << i << " (" << std::setw(5) << (i * world.getTimeStep()) << "s): "
                      << "Y=" << std::setw(7) << y
                      << ", 底部=" << std::setw(7) << ball->getBottom()
                      << ", vy=" << std::setw(8) << vy
                      << ", 被支撑=" << (ball->getIsSupported() ? "是" : "否");
            
            // 检查是否与地面碰撞
            bool collided = ball->HasCollidedWithGround(world.ground.getYLevel());
            std::cout << ", 与地面碰撞=" << (collided ? "是" : "否");
            
            // 检查距离地面的距离
            double distance = ball->getBottom() - world.ground.getYLevel();
            std::cout << ", 距离=" << std::setw(7) << distance << "m";
            
            std::cout << std::endl;
        }
    }
    
    std::cout << "\n最终分析：" << std::endl;
    ball->getCentre(x, y);
    ball->getVelocity(vx, vy);
    
    std::cout << "  最终位置: (" << x << ", " << y << ")" << std::endl;
    std::cout << "  最终底部Y: " << ball->getBottom() << std::endl;
    std::cout << "  最终速度: (" << vx << ", " << vy << ")" << std::endl;
    std::cout << "  地面Y: " << world.ground.getYLevel() << std::endl;
    std::cout << "  距离地面: " << (ball->getBottom() - world.ground.getYLevel()) << " m" << std::endl;
    
    bool on_ground = ball->HasCollidedWithGround(world.ground.getYLevel());
    std::cout << "\n结果: " << (on_ground ? "? 物体在地面上" : "? 物体没有到达地面") << std::endl;
    
    if (std::abs(vy) < 0.1) {
        std::cout << "       ? 速度接近0" << std::endl;
    } else {
        std::cout << "       ? 速度还很大 (" << vy << " m/s)" << std::endl;
    }
    
    delete ball;
    
    return 0;
}
