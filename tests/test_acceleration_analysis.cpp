/*=========================================================================================================
 * 详细分析测试 - 每帧输出速度和加速度
 *=========================================================================================================*/

#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include "physicalWorld.h"
#include "shapes.h"

void test_detailed_analysis() {
    std::cout << "========================================" << std::endl;
    std::cout << "  详细分析: 每帧速度和加速度" << std::endl;
    std::cout << "========================================\n" << std::endl;

    PhysicalWorld world;
    world.setInclineAngle(30.0);
    world.ground.setFriction(0.3, 0.3);
    world.setTimeStep(0.02);  // 使用标准时间步长

    AABB* box = new AABB(10.0, 2.0, 2.0, 0.0, 0.0);
    world.addDynamicShape(box);
    world.placeShapeOnGround(*box, world.ground);
    box->setVelocity(0.0, 0.0);

    // 理论加速度
    const double PI = 3.14159265358979323846;
    double expected_ax = 9.8 * (sin(30 * PI / 180.0) - 0.3 * cos(30 * PI / 180.0));
    
    std::cout << "理论加速度: " << expected_ax << " m/s?\n" << std::endl;
    std::cout << std::setw(8) << "帧数" 
              << std::setw(12) << "时间(s)"
              << std::setw(12) << "速度(m/s)"
              << std::setw(12) << "Δv(m/s)"
              << std::setw(15) << "实际a(m/s?)"
              << std::setw(15) << "误差(%)" << std::endl;
    std::cout << std::string(75, '-') << std::endl;

    double prev_vx = 0.0;
    
    // 只模拟前50帧（1秒）
    for (int i = 0; i <= 50; ++i) {
        double vx, vy;
        box->getVelocity(vx, vy);
        
        double delta_v = vx - prev_vx;
        double actual_a = (i > 0) ? (delta_v / world.getTimeStep()) : 0.0;
        double error_percent = (i > 0) ? ((expected_ax - actual_a) / expected_ax * 100.0) : 0.0;
        
        if (i % 5 == 0 || i < 5) {  // 打印前5帧和每5帧
            std::cout << std::fixed << std::setprecision(4);
            std::cout << std::setw(8) << i
                      << std::setw(12) << i * world.getTimeStep()
                      << std::setw(12) << vx
                      << std::setw(12) << delta_v
                      << std::setw(15) << actual_a
                      << std::setw(15) << error_percent << std::endl;
        }
        
        prev_vx = vx;
        world.update(world.dynamicShapeList, world.ground);
    }

    std::cout << "\n观察:" << std::endl;
    std::cout << "1. 第一帧的加速度是否正常？" << std::endl;
    std::cout << "2. 后续帧的加速度是否稳定？" << std::endl;
    std::cout << "3. 是否有异常的加速度波动？" << std::endl;

    delete box;
}

int main() {
    test_detailed_analysis();
    return 0;
}
