/*=========================================================================================================
 * 斜面碰撞测试 - 测试物体在斜面上的碰撞行为
 * 
 * 测试场景：
 * 1. 两个球在斜面上相向滑动并碰撞
 * 2. 球从斜面顶部滚下并与静止球碰撞
 * 3. 多个球在斜面上连续碰撞
 * 4. 球在斜面上碰撞后的速度变化验证
 *=========================================================================================================*/

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include "physicalWorld.h"
#include "shapes.h"

void printSeparator(char c = '=', int length = 80) {
    std::cout << std::string(length, c) << std::endl;
}

void printSubSeparator() {
    printSeparator('-', 60);
}

void printBallState(const Circle* ball, const std::string& label = "") {
    double x, y, vx, vy;
    ball->getCentre(x, y);
    ball->getVelocity(vx, vy);
    
    if (!label.empty()) {
        std::cout << label << ": ";
    }
    std::cout << ball->getName() 
              << " - 位置: (" << std::fixed << std::setprecision(2) << x << ", " << y << ")"
              << ", 速度: (" << vx << ", " << vy << ")"
              << ", 速率: " << ball->getSpeed() << " m/s" << std::endl;
}

// 测试1：两个球在斜面上相向滑动并碰撞
void test_head_on_collision_on_slope() {
    printSeparator();
    std::cout << "测试1：斜面上的正面碰撞" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(30.0);  // 30度斜面
    world.ground.setFriction(0.1, 0.15);  // 动摩擦系数0.1，静摩擦系数0.15
    world.setTimeStep(1.0 / 60.0);
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  倾斜角度: 30°" << std::endl;
    std::cout << "  重力加速度: 10 m/s?" << std::endl;
    std::cout << "  动摩擦系数: 0.1" << std::endl;
    std::cout << "  静摩擦系数: 0.15" << std::endl;
    std::cout << "  恢复系数: 0.8" << std::endl;
    
    // 创建两个球，一个向右滑动，一个向左滑动
    Circle* ball1 = new Circle(1.0, 2.0, 0.0, 5.0, 5.0, 0.0);  // 向右运动
    Circle* ball2 = new Circle(1.0, 2.0, 20.0, 5.0, -5.0, 0.0); // 向左运动
    
    ball1->setName("Ball1");
    ball2->setName("Ball2");
    ball1->setRestitution(0.8);
    ball2->setRestitution(0.8);
    
    world.addDynamicShape(ball1);
    world.addDynamicShape(ball2);
    
    // 放置在地面上
    world.placeShapeOnGround(*ball1, world.ground);
    world.placeShapeOnGround(*ball2, world.ground);
    
    std::cout << "\n初始状态：" << std::endl;
    printBallState(ball1);
    printBallState(ball2);
    
    double x1, y1, vx1, vy1, x2, y2, vx2, vy2;
    ball1->getCentre(x1, y1);
    ball2->getCentre(x2, y2);
    std::cout << "初始距离: " << std::abs(x2 - x1) << " m" << std::endl;
    
    // 运行模拟直到碰撞
    bool collided = false;
    int collision_frame = -1;
    double pre_collision_ke1 = 0, pre_collision_ke2 = 0;
    double post_collision_ke1 = 0, post_collision_ke2 = 0;
    
    std::cout << "\n模拟过程：" << std::endl;
    for (int i = 0; i < 300; ++i) {
        world.update(world.dynamicShapeList, world.ground);
        
        ball1->getCentre(x1, y1);
        ball2->getCentre(x2, y2);
        double distance = std::abs(x2 - x1) - ball1->getRadius() - ball2->getRadius();
        
        // 检测碰撞（距离小于0表示重叠）
        if (distance <= 0 && !collided) {
            collided = true;
            collision_frame = i;
            
            // 记录碰撞前的动能
            pre_collision_ke1 = ball1->getKineticEnergy();
            pre_collision_ke2 = ball2->getKineticEnergy();
            
            std::cout << "\n[碰撞发生！] 第 " << i << " 帧" << std::endl;
            std::cout << "碰撞前状态：" << std::endl;
            printBallState(ball1, "  ");
            printBallState(ball2, "  ");
            std::cout << "  总动能: " << (pre_collision_ke1 + pre_collision_ke2) << " J" << std::endl;
        }
        
        // 碰撞后的第一帧
        if (collided && i == collision_frame + 1) {
            post_collision_ke1 = ball1->getKineticEnergy();
            post_collision_ke2 = ball2->getKineticEnergy();
            
            std::cout << "\n碰撞后状态：" << std::endl;
            printBallState(ball1, "  ");
            printBallState(ball2, "  ");
            std::cout << "  总动能: " << (post_collision_ke1 + post_collision_ke2) << " J" << std::endl;
            break;
        }
        
        // 每30帧显示一次状态（碰撞前）
        if (!collided && i % 30 == 0) {
            std::cout << "第 " << i << " 帧: 距离 = " << distance << " m" << std::endl;
        }
    }
    
    // 结果验证
    printSubSeparator();
    std::cout << "结果验证：" << std::endl;
    
    if (collided) {
        std::cout << "? 检测到碰撞" << std::endl;
        
        ball1->getVelocity(vx1, vy1);
        ball2->getVelocity(vx2, vy2);
        
        // 检查速度是否反向
        if (vx1 < 0) {
            std::cout << "? Ball1 速度反向（向左）" << std::endl;
        } else {
            std::cout << "? Ball1 速度未反向（应该向左）" << std::endl;
        }
        
        if (vx2 > 0) {
            std::cout << "? Ball2 速度反向（向右）" << std::endl;
        } else {
            std::cout << "? Ball2 速度未反向（应该向右）" << std::endl;
        }
        
        // 能量守恒检查（考虑恢复系数）
        double total_ke_before = pre_collision_ke1 + pre_collision_ke2;
        double total_ke_after = post_collision_ke1 + post_collision_ke2;
        double energy_loss_ratio = (total_ke_before - total_ke_after) / total_ke_before;
        
        std::cout << "\n能量分析：" << std::endl;
        std::cout << "  碰撞前总动能: " << total_ke_before << " J" << std::endl;
        std::cout << "  碰撞后总动能: " << total_ke_after << " J" << std::endl;
        std::cout << "  能量损失: " << (total_ke_before - total_ke_after) << " J (" 
                  << (energy_loss_ratio * 100) << "%)" << std::endl;
        std::cout << "  理论能量损失（e=0.8）: ≈36%" << std::endl;
        
    } else {
        std::cout << "? 未检测到碰撞" << std::endl;
    }
    
    delete ball1;
    delete ball2;
}

// 测试2：球从斜面顶部滚下并与静止球碰撞
void test_rolling_collision_on_slope() {
    printSeparator();
    std::cout << "测试2：滚动球与静止球碰撞" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(30.0);
    world.ground.setFriction(0.05, 0.1);  // 较小摩擦力
    world.setTimeStep(1.0 / 60.0);
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  一个球从斜面上方滚下，撞击静止的球" << std::endl;
    std::cout << "  倾斜角度: 30°" << std::endl;
    std::cout << "  摩擦系数: 0.05（动）, 0.1（静）" << std::endl;
    
    // 运动的球（从上方开始）
    Circle* moving_ball = new Circle(1.0, 2.0, 0.0, 5.0, 0.0, 0.0);
    moving_ball->setName("MovingBall");
    moving_ball->setRestitution(0.9);
    
    // 静止的球（在下方）
    Circle* static_ball = new Circle(1.0, 2.0, 15.0, 5.0, 0.0, 0.0);
    static_ball->setName("StaticBall");
    static_ball->setRestitution(0.9);
    
    world.addDynamicShape(moving_ball);
    world.addDynamicShape(static_ball);
    
    world.placeShapeOnGround(*moving_ball, world.ground);
    world.placeShapeOnGround(*static_ball, world.ground);
    
    std::cout << "\n初始状态：" << std::endl;
    printBallState(moving_ball);
    printBallState(static_ball);
    
    // 运行模拟
    bool collided = false;
    int collision_frame = -1;
    
    std::cout << "\n模拟过程（每20帧显示）：" << std::endl;
    std::cout << std::left << std::setw(8) << "帧数" 
              << std::setw(15) << "运动球速度" 
              << std::setw(15) << "静止球速度" 
              << std::setw(12) << "距离" << std::endl;
    printSubSeparator();
    
    for (int i = 0; i < 300; ++i) {
        world.update(world.dynamicShapeList, world.ground);
        
        double x1, y1, x2, y2;
        moving_ball->getCentre(x1, y1);
        static_ball->getCentre(x2, y2);
        double distance = std::abs(x2 - x1) - moving_ball->getRadius() - static_ball->getRadius();
        
        if (i % 20 == 0) {
            std::cout << std::left << std::setw(8) << i
                      << std::setw(15) << moving_ball->getSpeed()
                      << std::setw(15) << static_ball->getSpeed()
                      << std::setw(12) << distance << std::endl;
        }
        
        // 检测碰撞
        if (distance <= 0 && !collided) {
            collided = true;
            collision_frame = i;
            
            std::cout << "\n[碰撞！] 第 " << i << " 帧" << std::endl;
            printBallState(moving_ball, "碰撞前 ");
            printBallState(static_ball, "碰撞前 ");
        }
        
        if (collided && i == collision_frame + 5) {
            std::cout << "\n碰撞后 5 帧：" << std::endl;
            printBallState(moving_ball);
            printBallState(static_ball);
            break;
        }
    }
    
    // 结果验证
    printSubSeparator();
    std::cout << "结果验证：" << std::endl;
    
    if (collided) {
        std::cout << "? 碰撞发生在第 " << collision_frame << " 帧" << std::endl;
        
        double vx_moving = moving_ball->velocity[0];
        double vx_static = static_ball->velocity[0];
        
        if (vx_static > 0) {
            std::cout << "? 静止球被推动（vx = " << vx_static << " m/s）" << std::endl;
        } else {
            std::cout << "? 静止球未被推动" << std::endl;
        }
        
        if (vx_moving > 0 && vx_moving < vx_static) {
            std::cout << "? 运动球减速（vx = " << vx_moving << " m/s）" << std::endl;
        }
        
        // 动量守恒检查
        double total_momentum = moving_ball->getMass() * vx_moving + static_ball->getMass() * vx_static;
        std::cout << "\n动量分析：" << std::endl;
        std::cout << "  碰撞后总动量: " << total_momentum << " kg·m/s" << std::endl;
        
    } else {
        std::cout << "? 未检测到碰撞（可能运动球速度太慢）" << std::endl;
    }
    
    delete moving_ball;
    delete static_ball;
}

// 测试3：多球连续碰撞（多米诺效应）
void test_domino_collision_on_slope() {
    printSeparator();
    std::cout << "测试3：斜面上的多米诺碰撞效应" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(25.0);  // 25度斜面
    world.ground.setFriction(0.1, 0.15);
    world.setTimeStep(1.0 / 60.0);
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  5个球排成一列，最上方的球滑下撞击其他球" << std::endl;
    std::cout << "  倾斜角度: 25°" << std::endl;
    
    // 创建5个球
    const int num_balls = 5;
    std::vector<Circle*> balls;
    
    for (int i = 0; i < num_balls; ++i) {
        double x_pos = 5.0 + i * 6.0;  // 间隔6米
        Circle* ball = new Circle(1.0, 2.0, x_pos, 5.0, 0.0, 0.0);
        ball->setName("Ball" + std::to_string(i + 1));
        ball->setRestitution(0.85);
        balls.push_back(ball);
        world.addDynamicShape(ball);
        world.placeShapeOnGround(*ball, world.ground);
    }
    
    // 给第一个球一个初速度
    balls[0]->setVelocity(8.0, 0.0);
    
    std::cout << "\n初始状态：" << std::endl;
    for (auto ball : balls) {
        printBallState(ball);
    }
    
    // 运行模拟
    std::cout << "\n模拟过程（每30帧显示）：" << std::endl;
    std::vector<int> collision_frames;
    std::vector<bool> has_collided(num_balls - 1, false);
    
    for (int i = 0; i < 600; ++i) {
        world.update(world.dynamicShapeList, world.ground);
        
        // 检测每对相邻球的碰撞
        for (int j = 0; j < num_balls - 1; ++j) {
            double x1, y1, x2, y2;
            balls[j]->getCentre(x1, y1);
            balls[j + 1]->getCentre(x2, y2);
            double distance = std::abs(x2 - x1) - balls[j]->getRadius() - balls[j + 1]->getRadius();
            
            if (distance <= 0 && !has_collided[j]) {
                has_collided[j] = true;
                collision_frames.push_back(i);
                std::cout << "\n[碰撞 " << (j + 1) << "!] 第 " << i << " 帧: " 
                          << balls[j]->getName() << " 撞击 " << balls[j + 1]->getName() << std::endl;
            }
        }
        
        // 定期显示所有球的速度
        if (i % 30 == 0) {
            std::cout << "\n第 " << i << " 帧状态：" << std::endl;
            for (auto ball : balls) {
                std::cout << "  " << ball->getName() << " 速度: " << ball->getSpeed() << " m/s" << std::endl;
            }
        }
        
        // 如果所有碰撞都发生了，再等一会儿然后退出
        bool all_collided = true;
        for (bool c : has_collided) {
            if (!c) all_collided = false;
        }
        if (all_collided && i > collision_frames.back() + 60) {
            break;
        }
    }
    
    // 结果验证
    printSubSeparator();
    std::cout << "\n结果验证：" << std::endl;
    
    int num_collisions = 0;
    for (bool c : has_collided) {
        if (c) num_collisions++;
    }
    
    std::cout << "发生的碰撞次数: " << num_collisions << " / " << (num_balls - 1) << std::endl;
    
    if (num_collisions == num_balls - 1) {
        std::cout << "? 多米诺效应完整传递" << std::endl;
    } else {
        std::cout << "? 多米诺效应未完全传递" << std::endl;
    }
    
    std::cout << "\n最终速度：" << std::endl;
    for (auto ball : balls) {
        std::cout << "  " << ball->getName() << ": " << ball->getSpeed() << " m/s" << std::endl;
    }
    
    // 清理
    for (auto ball : balls) {
        delete ball;
    }
}

// 测试4：碰撞后速度验证（理论vs实际）
void test_collision_velocity_verification() {
    printSeparator();
    std::cout << "测试4：碰撞速度理论验证" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(20.0);
    world.ground.setFriction(0.0);  // 无摩擦，简化计算
    world.setTimeStep(1.0 / 120.0); // 更小的时间步长以提高精度
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  无摩擦斜面，两个相同质量的球弹性碰撞" << std::endl;
    std::cout << "  倾斜角度: 20°" << std::endl;
    std::cout << "  恢复系数: 1.0（完全弹性）" << std::endl;
    
    double m1 = 1.0, m2 = 1.0;
    double v1_initial = 5.0, v2_initial = -3.0;
    double e = 1.0;  // 恢复系数
    
    Circle* ball1 = new Circle(m1, 2.0, 0.0, 5.0, v1_initial, 0.0);
    Circle* ball2 = new Circle(m2, 2.0, 15.0, 5.0, v2_initial, 0.0);
    
    ball1->setName("Ball1");
    ball2->setName("Ball2");
    ball1->setRestitution(e);
    ball2->setRestitution(e);
    
    world.addDynamicShape(ball1);
    world.addDynamicShape(ball2);
    
    world.placeShapeOnGround(*ball1, world.ground);
    world.placeShapeOnGround(*ball2, world.ground);
    
    // 理论计算（一维弹性碰撞）
    double v1_theory = ((m1 - e*m2)*v1_initial + m2*(1 + e)*v2_initial) / (m1 + m2);
    double v2_theory = ((m2 - e*m1)*v2_initial + m1*(1 + e)*v1_initial) / (m1 + m2);
    
    std::cout << "\n理论计算：" << std::endl;
    std::cout << "  碰撞前: Ball1 = " << v1_initial << " m/s, Ball2 = " << v2_initial << " m/s" << std::endl;
    std::cout << "  碰撞后: Ball1 = " << v1_theory << " m/s, Ball2 = " << v2_theory << " m/s" << std::endl;
    
    std::cout << "\n初始状态：" << std::endl;
    printBallState(ball1);
    printBallState(ball2);
    
    // 运行模拟直到碰撞
    bool collided = false;
    double v1_actual = 0, v2_actual = 0;
    
    for (int i = 0; i < 600; ++i) {
        world.update(world.dynamicShapeList, world.ground);
        
        double x1, y1, x2, y2;
        ball1->getCentre(x1, y1);
        ball2->getCentre(x2, y2);
        double distance = std::abs(x2 - x1) - ball1->getRadius() - ball2->getRadius();
        
        if (distance <= 0 && !collided) {
            collided = true;
            std::cout << "\n[碰撞！] 第 " << i << " 帧" << std::endl;
        }
        
        if (collided && i % 10 == 0) {
            v1_actual = ball1->velocity[0];
            v2_actual = ball2->velocity[0];
            
            // 检查速度是否稳定
            if (std::abs(v1_actual - v1_theory) < 0.1 && std::abs(v2_actual - v2_theory) < 0.1) {
                break;
            }
        }
    }
    
    // 结果比较
    printSubSeparator();
    std::cout << "\n实际测量：" << std::endl;
    printBallState(ball1);
    printBallState(ball2);
    
    v1_actual = ball1->velocity[0];
    v2_actual = ball2->velocity[0];
    
    std::cout << "\n速度对比：" << std::endl;
    std::cout << "  Ball1: 理论 = " << v1_theory << " m/s, 实际 = " << v1_actual 
              << " m/s, 误差 = " << std::abs(v1_actual - v1_theory) << " m/s ("
              << (std::abs(v1_actual - v1_theory) / std::abs(v1_theory) * 100) << "%)" << std::endl;
    std::cout << "  Ball2: 理论 = " << v2_theory << " m/s, 实际 = " << v2_actual 
              << " m/s, 误差 = " << std::abs(v2_actual - v2_theory) << " m/s ("
              << (std::abs(v2_actual - v2_theory) / std::abs(v2_theory) * 100) << "%)" << std::endl;
    
    // 动量守恒检查
    double momentum_before = m1 * v1_initial + m2 * v2_initial;
    double momentum_after = m1 * v1_actual + m2 * v2_actual;
    std::cout << "\n守恒定律验证：" << std::endl;
    std::cout << "  碰撞前总动量: " << momentum_before << " kg·m/s" << std::endl;
    std::cout << "  碰撞后总动量: " << momentum_after << " kg·m/s" << std::endl;
    std::cout << "  动量误差: " << std::abs(momentum_after - momentum_before) << " kg·m/s" << std::endl;
    
    // 能量守恒检查
    double ke_before = 0.5 * m1 * v1_initial * v1_initial + 0.5 * m2 * v2_initial * v2_initial;
    double ke_after = 0.5 * m1 * v1_actual * v1_actual + 0.5 * m2 * v2_actual * v2_actual;
    std::cout << "  碰撞前总动能: " << ke_before << " J" << std::endl;
    std::cout << "  碰撞后总动能: " << ke_after << " J" << std::endl;
    std::cout << "  能量误差: " << std::abs(ke_after - ke_before) << " J" << std::endl;
    
    delete ball1;
    delete ball2;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   斜面碰撞测试套件" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n本测试验证物体在倾斜平面上的碰撞行为" << std::endl;
    std::cout << "包括速度变化、能量守恒、动量守恒等" << std::endl;
    std::cout << std::endl;
    
    test_head_on_collision_on_slope();
    std::cout << "\n\n";
    
    test_rolling_collision_on_slope();
    std::cout << "\n\n";
    
    test_domino_collision_on_slope();
    std::cout << "\n\n";
    
    test_collision_velocity_verification();
    
    printSeparator();
    std::cout << "所有斜面碰撞测试完成！" << std::endl;
    printSeparator();
    
    return 0;
}
