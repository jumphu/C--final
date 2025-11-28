/*=========================================================================================================
 * 测试程序：测试斜面上的摩擦力效果
 * 
 * 测试场景：
 * 1. 物体在斜面上，当重力分力小于最大静摩擦力时，应保持静止。
 * 2. 物体在斜面上，当重力分力大于最大静摩擦力时，应开始加速下滑。
 * 3. 物体在下滑过程中，受到的动摩擦力应使其加速度小于无摩擦时的加速度。
 *=========================================================================================================*/

#include "physicalWorld.h"
#include "shapes.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cassert>

// PI for calculations
// const double PI = 3.14159265358979323846;

void printTestHeader(const std::string& testName) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  " << testName << std::endl;
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

/*=========================================================================================================
 * 测试1：静摩擦力足够，物体保持静止
 * 
 * 条件: 
 * - 斜面角度: 15度
 * - 摩擦系数: 0.3 (μ)
 * - 重力: 9.8 m/s^2
 * 
 * 计算:
 * - 重力下滑分力: F_slide = mg * sin(15°) ≈ mg * 0.259
 * - 最大静摩擦力: F_friction_max = μ * N = μ * mg * cos(15°) ≈ 0.3 * mg * 0.966 = mg * 0.289
 * - 结论: F_slide < F_friction_max，物体应保持静止。
 *=========================================================================================================*/
void test_static_friction_holds() {
    printTestHeader("测试1: 静摩擦力足够，物体静止");

    PhysicalWorld world;
    world.setInclineAngle(15.0); // 设置15度斜面
    world.ground.setFriction(0.3);
    world.setTimeStep(0.02);

    AABB* box = new AABB(10.0, 2.0, 2.0, 0.0, 0.0);
    box->setName("Box");
    world.addDynamicShape(box);
    world.placeShapeOnGround(*box, world.ground);

    std::cout << "角度=15°, 摩擦系数=0.3" << std::endl;
    std::cout << "预期: 物体保持静止\n" << std::endl;

    // 模拟2秒
    for (int i = 0; i <= 100; ++i) {
        world.update(world.dynamicShapeList, world.ground);
        if (i % 25 == 0) { // 每0.5秒打印一次状态
            printShapeState(box->getName(), box, i * world.getTimeStep());
        }
    }

    double vx, vy;
    box->getVelocity(vx, vy);
    std::cout << "\n结果验证：" << std::endl;
    if (std::abs(vx) < 1e-3 && std::abs(vy) < 1e-3) {
        std::cout << "  ? 成功: 物体保持静止。" << std::endl;
    } else {
        std::cout << "  ?? 失败: 物体发生移动。" << std::endl;
    }

    delete box;
}

/*=========================================================================================================
 * 测试2：静摩擦力不足，物体加速下滑
 * 
 * 条件: 
 * - 斜面角度: 30度
 * - 摩擦系数: 0.3 (μ)
 * - 重力: 9.8 m/s^2
 * 
 * 计算:
 * - 重力下滑分力: F_slide = mg * sin(30°) = mg * 0.5
 * - 最大静摩擦力: F_friction_max = μ * mg * cos(30°) ≈ 0.3 * mg * 0.866 = mg * 0.26
 * - 结论: F_slide > F_friction_max，物体应加速下滑。
 * - 预期加速度: a = g * (sin(30°) - μ * cos(30°)) ≈ 9.8 * (0.5 - 0.26) = 2.35 m/s^2
 *=========================================================================================================*/
void test_friction_insufficient_and_slides() {
    printTestHeader("测试2: 静摩擦力不足，物体加速下滑");

    PhysicalWorld world;
    world.setInclineAngle(30.0); // 设置30度斜面
    world.ground.setFriction(0.3);
    world.setTimeStep(0.02);

    AABB* box = new AABB(10.0, 2.0, 2.0, 0.0, 0.0);
    box->setName("Box");
    world.addDynamicShape(box);
    world.placeShapeOnGround(*box, world.ground);

    std::cout << "角度=30°, 摩擦系数=0.3" << std::endl;
    std::cout << "预期: 物体加速下滑\n" << std::endl;

    // 模拟2秒
    for (int i = 0; i <= 100; ++i) {
        world.update(world.dynamicShapeList, world.ground);
        if (i % 25 == 0) { // 每0.5秒打印一次状态
            printShapeState(box->getName(), box, i * world.getTimeStep());
        }
    }

    double vx, vy;
    box->getVelocity(vx, vy);
    double finalSpeed = std::sqrt(vx*vx + vy*vy);

    // 理论末速度 v = a*t = 2.35 * 2 = 4.7 m/s
    // 由于坐标系旋转，速度会分解到vx和vy
    double expected_ax = 9.8 * (sin(30 * PI / 180.0) - 0.3 * cos(30 * PI / 180.0));
    double expected_final_v = expected_ax * 2.0;


    std::cout << "\n结果验证：" << std::endl;
    if (finalSpeed > 1.0) {
        std::cout << "  ? 成功: 物体发生滑动。" << std::endl;
        std::cout << "  - 理论末速度(沿斜面): " << expected_final_v << " m/s" << std::endl;
        std::cout << "  - 实际末速度(矢量和): " << finalSpeed << " m/s" << std::endl;
    } else {
        std::cout << "  ?? 失败: 物体没有按预期滑动。" << std::endl;
    }

    delete box;
}


int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "         斜面摩擦力测试程序" << std::endl;
    std::cout << "============================================" << std::endl;
    
    try {
        test_static_friction_holds();
        test_friction_insufficient_and_slides();
        
        std::cout << "\n============================================" << std::endl;
        std::cout << "  ? 所有斜面摩擦力测试完成！" << std::endl;
        std::cout << "============================================" << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n? 测试失败: " << e.what() << std::endl;
        return 1;
    }
}
