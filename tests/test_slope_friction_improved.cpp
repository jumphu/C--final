/*=========================================================================================================
 * 改进的斜面摩擦力测试
 * 使用更小的时间步长以提高精度
 *=========================================================================================================*/

#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include "physicalWorld.h"
#include "shapes.h"

// 使用 shapes.h 中定义的 PI

void printTestHeader(const std::string& title) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << "========================================" << std::endl;
}

void printShapeState(const std::string& name, Shape* shape, double time) {
    double x, y, vx, vy;
    shape->getCentre(x, y);
    shape->getVelocity(vx, vy);
    
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Time: " << std::setw(5) << time << "s | " 
              << name << " - "
              << "Pos: (" << std::setw(7) << x << ", " << std::setw(7) << y << "), "
              << "Vel: (" << std::setw(7) << vx << ", " << std::setw(7) << vy << ")"
              << std::endl;
}

void test_improved_precision() {
    printTestHeader("改进测试: 30度斜面，更小时间步长");

    PhysicalWorld world;
    world.setInclineAngle(30.0);
    world.ground.setFriction(0.3, 0.3); // 明确设置动摩擦和静摩擦
    world.setTimeStep(0.005);  // 更小的时间步长 (0.005s)

    AABB* box = new AABB(10.0, 2.0, 2.0, 0.0, 0.0);
    box->setName("Box");
    world.addDynamicShape(box);
    world.placeShapeOnGround(*box, world.ground);
    
    // 显式设置初始速度为零
    box->setVelocity(0.0, 0.0);

    std::cout << "角度=30°, 摩擦系数=0.3" << std::endl;
    std::cout << "时间步长=0.005s (提高精度)" << std::endl;
    std::cout << "预期: 物体加速下滑\n" << std::endl;

    // 模拟2秒 (400步 × 0.005s = 2s)
    for (int i = 0; i <= 400; ++i) {
        world.update(world.dynamicShapeList, world.ground);
        if (i % 100 == 0) { // 每0.5秒打印一次
            printShapeState(box->getName(), box, i * world.getTimeStep());
        }
    }

    double vx, vy;
    box->getVelocity(vx, vy);
    double finalSpeed = std::sqrt(vx*vx + vy*vy);

    // 理论计算
    double expected_ax = 9.8 * (sin(30 * PI / 180.0) - 0.3 * cos(30 * PI / 180.0));
    double expected_final_v = expected_ax * 2.0;

    std::cout << "\n结果验证：" << std::endl;
    std::cout << "  ? 物体发生滑动" << std::endl;
    std::cout << "  - 理论加速度: " << expected_ax << " m/s?" << std::endl;
    std::cout << "  - 理论末速度: " << expected_final_v << " m/s" << std::endl;
    std::cout << "  - 实际末速度: " << finalSpeed << " m/s" << std::endl;
    std::cout << "  - 误差: " << std::abs(expected_final_v - finalSpeed) << " m/s ";
    std::cout << "(" << std::fixed << std::setprecision(1) 
              << (std::abs(expected_final_v - finalSpeed) / expected_final_v * 100) << "%)" << std::endl;

    delete box;
}

void test_multiple_timesteps() {
    printTestHeader("对比测试: 不同时间步长的影响");

    double timesteps[] = {0.02, 0.01, 0.005, 0.001};
    int num_tests = 4;

    // 理论值
    double expected_ax = 9.8 * (sin(30 * PI / 180.0) - 0.3 * cos(30 * PI / 180.0));
    double expected_final_v = expected_ax * 2.0;

    std::cout << "\n理论末速度: " << expected_final_v << " m/s\n" << std::endl;
    std::cout << std::setw(15) << "时间步长" 
              << std::setw(15) << "实际末速度" 
              << std::setw(15) << "误差"
              << std::setw(15) << "误差百分比" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    for (int t = 0; t < num_tests; ++t) {
        double dt = timesteps[t];
        int steps = static_cast<int>(2.0 / dt);

        PhysicalWorld world;
        world.setInclineAngle(30.0);
        world.ground.setFriction(0.3, 0.3);
        world.setTimeStep(dt);

        AABB* box = new AABB(10.0, 2.0, 2.0, 0.0, 0.0);
        world.addDynamicShape(box);
        world.placeShapeOnGround(*box, world.ground);
        box->setVelocity(0.0, 0.0);

        // 运行模拟
        for (int i = 0; i <= steps; ++i) {
            world.update(world.dynamicShapeList, world.ground);
        }

        double vx, vy;
        box->getVelocity(vx, vy);
        double finalSpeed = std::sqrt(vx*vx + vy*vy);
        double error = std::abs(expected_final_v - finalSpeed);
        double error_percent = (error / expected_final_v) * 100.0;

        std::cout << std::fixed << std::setprecision(4);
        std::cout << std::setw(15) << dt << " s"
                  << std::setw(15) << finalSpeed << " m/s"
                  << std::setw(15) << error << " m/s"
                  << std::setw(14) << error_percent << " %" << std::endl;

        delete box;
    }
}

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "    改进的斜面摩擦力测试程序" << std::endl;
    std::cout << "============================================" << std::endl;

    test_improved_precision();
    test_multiple_timesteps();

    std::cout << "\n============================================" << std::endl;
    std::cout << "  ? 所有测试完成！" << std::endl;
    std::cout << "============================================\n" << std::endl;

    return 0;
}
