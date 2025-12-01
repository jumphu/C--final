/*=========================================================================================================
 * 板块模型测试 - 测试多个板块之间的摩擦力和相对运动
 * 
 * 测试场景：
 * 1. 双板块模型：上层板块在下层板块上滑动（使用 placeShapeOnShape）
 * 2. 三板块堆叠：验证摩擦力的传递（使用 placeShapeOnShape）
 * 3. 板块碰撞：两个板块相向运动并碰撞
 * 4. 牵引板块：拉动下层板块，上层板块跟随
 * 5. 斜面上的板块：板块在斜面上的相对运动（使用 placeShapeOnShape）
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
    std::cout << std::string(60, '-') << std::endl;
}

void printBlockState(AABB* block, const std::string& label = "") {
    double x, y, vx, vy;
    block->getCentre(x, y);
    block->getVelocity(vx, vy);
    
    if (!label.empty()) {
        std::cout << label << ": ";
    }
    std::cout << block->getName() 
              << " - 位置: (" << std::fixed << std::setprecision(3) << x << ", " << y << ")"
              << ", 速度: (" << vx << ", " << vy << ") m/s"
              << ", 支撑: " << (block->getIsSupported() ? "是" : "否");
    
    if (block->getSupporter() != nullptr) {
        std::cout << " (由 " << block->getSupporter()->getName() << " 支撑)";
    }
    std::cout << std::endl;
}

// 测试1：基础双板块模型（使用 placeShapeOnShape）
void test_basic_two_blocks() {
    printSeparator();
    std::cout << "测试1：基础双板块模型（使用 placeShapeOnShape）" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);  // 水平面
    world.setTimeStep(1.0 / 60.0);
    world.ground.setYLevel(0.0);
    world.ground.setFriction(0.2, 0.3);  // 地面摩擦系数
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  下层板块（静止）在地面上" << std::endl;
    std::cout << "  上层板块（5 m/s）在下层板块上滑动" << std::endl;
    std::cout << "  板块间摩擦系数: 0.3（动）, 0.4（静）" << std::endl;
    std::cout << "  地面摩擦系数: 0.2（动）, 0.3（静）" << std::endl;
    
    // 创建下层板块（较大、较重）
    AABB* bottom_block = new AABB(5.0, 10.0, 2.0, 10.0, 0.0);  // 质量5kg, 10x2m
    bottom_block->setName("BottomBlock");
    bottom_block->setFraction(0.2);
    bottom_block->setStaticFraction(0.3);
    
    // 创建上层板块（较小、较轻）
    AABB* top_block = new AABB(2.0, 6.0, 1.5, 10.0, 3.0, 5.0, 0.0);  // 质量2kg, 6x1.5m, 初速度5m/s
    top_block->setName("TopBlock");
    top_block->setFraction(0.3);
    top_block->setStaticFraction(0.4);
    
    world.addDynamicShape(bottom_block);
    world.addDynamicShape(top_block);
    
    // 放置板块
    world.placeShapeOnGround(*bottom_block, world.ground);
    // 使用 placeShapeOnShape 放置上层板块在下层板块上方
    world.placeShapeOnShape(*top_block, *bottom_block, 0.1);
    
    std::cout << "\n初始状态：" << std::endl;
    printBlockState(bottom_block);
    printBlockState(top_block);
    
    // 运行模拟
    std::cout << "\n模拟过程（每30帧显示）：" << std::endl;
    std::cout << std::left 
              << std::setw(8) << "帧数"
              << std::setw(15) << "下层速度"
              << std::setw(15) << "上层速度"
              << std::setw(15) << "相对速度"
              << std::setw(12) << "状态" << std::endl;
    printSubSeparator();
    
    bool reached_equilibrium = false;
    
    for (int i = 0; i < 300; ++i) {
        world.update(world.dynamicShapeList, world.ground);
        
        if (i % 30 == 0) {
            double vx_bottom = bottom_block->velocity[0];
            double vx_top = top_block->velocity[0];
            double relative_v = vx_top - vx_bottom;
            
            std::string status;
            if (std::abs(relative_v) < 0.01) {
                status = "相对静止";
                if (!reached_equilibrium) {
                    reached_equilibrium = true;
                }
            } else if (relative_v > 0) {
                status = "上层前行";
            } else {
                status = "上层后退";
            }
            
            std::cout << std::left
                      << std::setw(8) << i
                      << std::setw(15) << vx_bottom
                      << std::setw(15) << vx_top
                      << std::setw(15) << relative_v
                      << std::setw(12) << status << std::endl;
        }
        
        // 如果达到平衡并持续一段时间，停止模拟
        if (reached_equilibrium && i > 180) {
            std::cout << "\n系统达到平衡，提前结束模拟（第 " << i << " 帧）" << std::endl;
            break;
        }
    }
    
    // 最终状态
    printSubSeparator();
    std::cout << "\n最终状态：" << std::endl;
    printBlockState(bottom_block);
    printBlockState(top_block);
    
    // 结果分析
    double vx_bottom = bottom_block->velocity[0];
    double vx_top = top_block->velocity[0];
    double relative_v = vx_top - vx_bottom;
    
    std::cout << "\n结果分析：" << std::endl;
    std::cout << "  相对速度: " << relative_v << " m/s" << std::endl;
    
    if (std::abs(relative_v) < 0.01) {
        std::cout << "  ? 两板块达到相对静止状态" << std::endl;
        
        if (std::abs(vx_bottom) < 0.01 && std::abs(vx_top) < 0.01) {
            std::cout << "  ? 系统完全静止（摩擦力足够大）" << std::endl;
        } else {
            std::cout << "  ? 两板块共同运动（摩擦力传递成功）" << std::endl;
        }
    } else {
        std::cout << "  ? 两板块仍在相对运动" << std::endl;
    }
    
    delete bottom_block;
    delete top_block;
}

// 测试2：三板块堆叠模型
void test_three_blocks_stack() {
    printSeparator();
    std::cout << "测试2：三板块堆叠模型" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);
    world.setTimeStep(1.0 / 60.0);
    world.ground.setFriction(0.3, 0.4);
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  三个板块堆叠，最上层板块有初速度" << std::endl;
    std::cout << "  验证摩擦力能否从上到下传递" << std::endl;
    
    // 创建三个板块
    AABB* block1 = new AABB(8.0, 12.0, 2.0, 15.0, 0.0);  // 底层：最重
    block1->setName("Block1");
    block1->setFraction(0.25);
    block1->setStaticFraction(0.35);
    
    AABB* block2 = new AABB(5.0, 10.0, 2.0, 15.0, 0.0);  // 中层
    block2->setName("Block2");
    block2->setFraction(0.3);
    block2->setStaticFraction(0.4);
    
    AABB* block3 = new AABB(2.0, 6.0, 1.5, 15.0, 0.0, 8.0, 0.0);  // 顶层：最轻，8m/s初速度
    block3->setName("Block3");
    block3->setFraction(0.35);
    block3->setStaticFraction(0.45);
    
    world.addDynamicShape(block1);
    world.addDynamicShape(block2);
    world.addDynamicShape(block3);
    
    // 放置板块
    world.placeShapeOnGround(*block1, world.ground);
    // 放置block2在block1上
    double block1_top = block1->getTop();
    block2->setCentre(15.0, block1_top + block2->getHeight() / 2.0 + 0.1);
    block2->setSupporter(block1);
    block2->setIsSupported(true);
    // 放置block3在block2上
    double block2_top = block2->getTop();
    block3->setCentre(15.0, block2_top + block3->getHeight() / 2.0 + 0.1);
    block3->setSupporter(block2);
    block3->setIsSupported(true);
    
    std::cout << "\n初始状态：" << std::endl;
    printBlockState(block1, "  ");
    printBlockState(block2, "  ");
    printBlockState(block3, "  ");
    
    // 运行模拟
    std::cout << "\n模拟过程（每20帧显示）：" << std::endl;
    std::cout << std::left 
              << std::setw(8) << "帧数"
              << std::setw(12) << "V1(m/s)"
              << std::setw(12) << "V2(m/s)"
              << std::setw(12) << "V3(m/s)"
              << std::setw(12) << "V3-V2"
              << std::setw(12) << "V2-V1" << std::endl;
    printSubSeparator();
    
    for (int i = 0; i < 300; ++i) {
        world.update(world.dynamicShapeList, world.ground);
        
        if (i % 20 == 0) {
            double v1 = block1->velocity[0];
            double v2 = block2->velocity[0];
            double v3 = block3->velocity[0];
            
            std::cout << std::fixed << std::setprecision(3)
                      << std::left
                      << std::setw(8) << i
                      << std::setw(12) << v1
                      << std::setw(12) << v2
                      << std::setw(12) << v3
                      << std::setw(12) << (v3 - v2)
                      << std::setw(12) << (v2 - v1) << std::endl;
        }
    }
    
    // 最终状态
    printSubSeparator();
    std::cout << "\n最终状态：" << std::endl;
    printBlockState(block1, "  ");
    printBlockState(block2, "  ");
    printBlockState(block3, "  ");
    
    // 分析相对速度
    double v1 = block1->velocity[0];
    double v2 = block2->velocity[0];
    double v3 = block3->velocity[0];
    
    std::cout << "\n结果分析：" << std::endl;
    std::cout << "  Block3-Block2 相对速度: " << (v3 - v2) << " m/s" << std::endl;
    std::cout << "  Block2-Block1 相对速度: " << (v2 - v1) << " m/s" << std::endl;
    
    bool all_static = std::abs(v3 - v2) < 0.01 && std::abs(v2 - v1) < 0.01;
    
    if (all_static) {
        std::cout << "  ? 所有板块达到相对静止" << std::endl;
        
        if (std::abs(v1) > 0.1) {
            std::cout << "  ? 摩擦力成功从上到下传递（整体运动）" << std::endl;
        } else {
            std::cout << "  ? 系统完全静止（摩擦力平衡）" << std::endl;
        }
    } else {
        std::cout << "  ? 板块间仍有相对运动" << std::endl;
    }
    
    delete block1;
    delete block2;
    delete block3;
}

// 测试3：板块碰撞模型
void test_block_collision() {
    printSeparator();
    std::cout << "测试3：板块碰撞模型" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);
    world.setTimeStep(1.0 / 60.0);
    world.ground.setFriction(0.1, 0.15);  // 较小摩擦
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  两个板块相向运动并碰撞" << std::endl;
    std::cout << "  恢复系数: 0.5（非完全弹性碰撞）" << std::endl;
    
    // 创建两个相向运动的板块
    AABB* block_left = new AABB(3.0, 6.0, 2.0, 5.0, 0.0, 5.0, 0.0);   // 向右运动
    block_left->setName("LeftBlock");
    block_left->setFraction(0.1);
    block_left->setRestitution(0.5);
    
    AABB* block_right = new AABB(3.0, 6.0, 2.0, 25.0, 0.0, -5.0, 0.0);  // 向左运动
    block_right->setName("RightBlock");
    block_right->setFraction(0.1);
    block_right->setRestitution(0.5);
    
    world.addDynamicShape(block_left);
    world.addDynamicShape(block_right);
    
    world.placeShapeOnGround(*block_left, world.ground);
    world.placeShapeOnGround(*block_right, world.ground);
    
    std::cout << "\n初始状态：" << std::endl;
    printBlockState(block_left);
    printBlockState(block_right);
    
    double x_left, x_right, y;
    block_left->getCentre(x_left, y);
    block_right->getCentre(x_right, y);
    std::cout << "初始距离: " << (x_right - x_left) << " m" << std::endl;
    
    // 运行模拟
    std::cout << "\n模拟过程：" << std::endl;
    bool collided = false;
    int collision_frame = -1;
    double pre_momentum = 0, post_momentum = 0;
    double pre_ke = 0, post_ke = 0;
    
    for (int i = 0; i < 300; ++i) {
        world.update(world.dynamicShapeList, world.ground);
        
        block_left->getCentre(x_left, y);
        block_right->getCentre(x_right, y);
        double distance = x_right - x_left - block_left->getWidth()/2 - block_right->getWidth()/2;
        
        if (i % 20 == 0 && !collided) {
            std::cout << "第 " << i << " 帧: 距离 = " << distance << " m" << std::endl;
        }
        
        // 检测碰撞
        if (distance <= 0 && !collided) {
            collided = true;
            collision_frame = i;
            
            // 记录碰撞前的动量和动能
            double v_left = block_left->velocity[0];
            double v_right = block_right->velocity[0];
            double m_left = block_left->getMass();
            double m_right = block_right->getMass();
            
            pre_momentum = m_left * v_left + m_right * v_right;
            pre_ke = 0.5 * m_left * v_left * v_left + 0.5 * m_right * v_right * v_right;
            
            std::cout << "\n[碰撞！] 第 " << i << " 帧" << std::endl;
            std::cout << "碰撞前：" << std::endl;
            printBlockState(block_left, "  ");
            printBlockState(block_right, "  ");
            std::cout << "  总动量: " << pre_momentum << " kg·m/s" << std::endl;
            std::cout << "  总动能: " << pre_ke << " J" << std::endl;
        }
        
        // 碰撞后的状态
        if (collided && i == collision_frame + 5) {
            double v_left = block_left->velocity[0];
            double v_right = block_right->velocity[0];
            double m_left = block_left->getMass();
            double m_right = block_right->getMass();
            
            post_momentum = m_left * v_left + m_right * v_right;
            post_ke = 0.5 * m_left * v_left * v_left + 0.5 * m_right * v_right * v_right;
            
            std::cout << "\n碰撞后（5帧后）：" << std::endl;
            printBlockState(block_left, "  ");
            printBlockState(block_right, "  ");
            std::cout << "  总动量: " << post_momentum << " kg·m/s" << std::endl;
            std::cout << "  总动能: " << post_ke << " J" << std::endl;
            break;
        }
    }
    
    // 结果验证
    printSubSeparator();
    std::cout << "\n结果验证：" << std::endl;
    
    if (collided) {
        std::cout << "? 碰撞发生在第 " << collision_frame << " 帧" << std::endl;
        
        // 检查速度是否反向
        double v_left_final = block_left->velocity[0];
        double v_right_final = block_right->velocity[0];
        
        if (v_left_final < 0 && v_right_final > 0) {
            std::cout << "? 两板块速度反向（符合预期）" << std::endl;
        }
        
        // 动量守恒检查
        double momentum_error = std::abs(post_momentum - pre_momentum) / std::abs(pre_momentum) * 100;
        std::cout << "\n守恒定律验证：" << std::endl;
        std::cout << "  动量误差: " << momentum_error << "%" << std::endl;
        
        if (momentum_error < 5.0) {
            std::cout << "  ? 动量守恒良好" << std::endl;
        }
        
        // 能量损失检查（非弹性碰撞）
        double energy_loss = (pre_ke - post_ke) / pre_ke * 100;
        std::cout << "  能量损失: " << energy_loss << "%" << std::endl;
        std::cout << "  理论损失（e=0.5）: ~75%" << std::endl;
        
    } else {
        std::cout << "? 未检测到碰撞" << std::endl;
    }
    
    delete block_left;
    delete block_right;
}

// 测试4：牵引板块模型
void test_pulling_blocks() {
    printSeparator();
    std::cout << "测试4：牵引板块模型" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);
    world.setTimeStep(1.0 / 60.0);
    world.ground.setFriction(0.2, 0.3);
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  对下层板块施加恒定外力（向右50N）" << std::endl;
    std::cout << "  观察上层板块是否跟随" << std::endl;
    std::cout << "  板块间摩擦系数: 0.5（动）, 0.6（静）" << std::endl;
    
    // 创建板块
    AABB* bottom_block = new AABB(4.0, 10.0, 2.0, 10.0, 0.0);
    bottom_block->setName("BottomBlock");
    bottom_block->setFraction(0.2);
    bottom_block->setStaticFraction(0.3);
    
    AABB* top_block = new AABB(2.0, 6.0, 1.5, 10.0, 0.0);
    top_block->setName("TopBlock");
    top_block->setFraction(0.5);
    top_block->setStaticFraction(0.6);
    
    world.addDynamicShape(bottom_block);
    world.addDynamicShape(top_block);
    
    world.placeShapeOnGround(*bottom_block, world.ground);
    // 放置上层板块
    double bottom_top = bottom_block->getTop();
    top_block->setCentre(10.0, bottom_top + top_block->getHeight() / 2.0 + 0.1);
    top_block->setSupporter(bottom_block);
    top_block->setIsSupported(true);
    
    std::cout << "\n初始状态：" << std::endl;
    printBlockState(bottom_block);
    printBlockState(top_block);
    
    // 运行模拟并施加外力
    std::cout << "\n模拟过程（每20帧显示）：" << std::endl;
    std::cout << std::left 
              << std::setw(8) << "帧数"
              << std::setw(12) << "下层速度"
              << std::setw(12) << "上层速度"
              << std::setw(15) << "相对速度"
              << std::setw(12) << "状态" << std::endl;
    printSubSeparator();
    
    const double pulling_force = 50.0;  // 牵引力
    
    for (int i = 0; i < 240; ++i) {
        // 在更新前施加外力
        bottom_block->applyForce(pulling_force, 0.0);
        
        world.update(world.dynamicShapeList, world.ground);
        
        if (i % 20 == 0) {
            double v_bottom = bottom_block->velocity[0];
            double v_top = top_block->velocity[0];
            double relative_v = v_top - v_bottom;
            
            std::string status;
            if (std::abs(relative_v) < 0.05) {
                status = "共同运动";
            } else if (relative_v < 0) {
                status = "上层滑后";
            } else {
                status = "上层滑前";
            }
            
            std::cout << std::fixed << std::setprecision(3)
                      << std::left
                      << std::setw(8) << i
                      << std::setw(12) << v_bottom
                      << std::setw(12) << v_top
                      << std::setw(15) << relative_v
                      << std::setw(12) << status << std::endl;
        }
    }
    
    // 最终状态
    printSubSeparator();
    std::cout << "\n最终状态：" << std::endl;
    printBlockState(bottom_block);
    printBlockState(top_block);
    
    // 结果分析
    double v_bottom = bottom_block->velocity[0];
    double v_top = top_block->velocity[0];
    double relative_v = v_top - v_bottom;
    
    std::cout << "\n结果分析：" << std::endl;
    std::cout << "  施加外力: " << pulling_force << " N" << std::endl;
    std::cout << "  相对速度: " << relative_v << " m/s" << std::endl;
    
    if (std::abs(relative_v) < 0.1) {
        std::cout << "  ? 上层板块成功跟随下层板块" << std::endl;
        std::cout << "  ? 静摩擦力足够大，保持相对静止" << std::endl;
    } else {
        std::cout << "  ? 上层板块相对滑动" << std::endl;
        std::cout << "  说明: 牵引力过大或摩擦力不足" << std::endl;
    }
    
    delete bottom_block;
    delete top_block;
}

// 测试5：斜面上的板块模型
void test_blocks_on_slope() {
    printSeparator();
    std::cout << "测试5：斜面上的板块模型" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(30.0);  // 30度斜面
    world.setTimeStep(1.0 / 60.0);
    world.ground.setFriction(0.15, 0.2);  // 较小摩擦，允许滑动
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  30° 斜面" << std::endl;
    std::cout << "  两个板块叠放在斜面上" << std::endl;
    std::cout << "  重力分量: g·sin(30°) = 5 m/s?" << std::endl;
    
    // 创建板块
    AABB* bottom_block = new AABB(6.0, 12.0, 2.5, 10.0, 0.0);  // 较重
    bottom_block->setName("BottomBlock");
    bottom_block->setFraction(0.15);
    bottom_block->setStaticFraction(0.2);
    
    AABB* top_block = new AABB(2.0, 6.0, 1.5, 10.0, 0.0);  // 较轻
    top_block->setName("TopBlock");
    top_block->setFraction(0.2);
    top_block->setStaticFraction(0.25);
    
    world.addDynamicShape(bottom_block);
    world.addDynamicShape(top_block);
    
    world.placeShapeOnGround(*bottom_block, world.ground);
    // 放置上层板块
    double bottom_top = bottom_block->getTop();
    top_block->setCentre(10.0, bottom_top + top_block->getHeight() / 2.0 + 0.1);
    top_block->setSupporter(bottom_block);
    top_block->setIsSupported(true);
    
    std::cout << "\n初始状态：" << std::endl;
    printBlockState(bottom_block);
    printBlockState(top_block);
    
    // 运行模拟
    std::cout << "\n模拟过程（每20帧显示）：" << std::endl;
    std::cout << std::left 
              << std::setw(8) << "帧数"
              << std::setw(12) << "下层速度"
              << std::setw(12) << "上层速度"
              << std::setw(15) << "相对速度"
              << std::setw(15) << "下层加速度" << std::endl;
    printSubSeparator();
    
    for (int i = 0; i < 240; ++i) {
        world.update(world.dynamicShapeList, world.ground);
        
        if (i % 20 == 0) {
            double v_bottom = bottom_block->velocity[0];
            double v_top = top_block->velocity[0];
            double relative_v = v_top - v_bottom;
            double a_bottom = bottom_block->totalforce[0] / bottom_block->getMass();
            
            std::cout << std::fixed << std::setprecision(3)
                      << std::left
                      << std::setw(8) << i
                      << std::setw(12) << v_bottom
                      << std::setw(12) << v_top
                      << std::setw(15) << relative_v
                      << std::setw(15) << a_bottom << std::endl;
        }
    }
    
    // 最终状态
    printSubSeparator();
    std::cout << "\n最终状态：" << std::endl;
    printBlockState(bottom_block);
    printBlockState(top_block);
    
    // 结果分析
    double v_bottom = bottom_block->velocity[0];
    double v_top = top_block->velocity[0];
    double relative_v = v_top - v_bottom;
    
    std::cout << "\n结果分析：" << std::endl;
    std::cout << "  下层板块速度: " << v_bottom << " m/s" << std::endl;
    std::cout << "  上层板块速度: " << v_top << " m/s" << std::endl;
    std::cout << "  相对速度: " << relative_v << " m/s" << std::endl;
    
    // 理论分析
    double g_parallel = 10.0 * std::sin(30.0 * PI / 180.0);  // 5 m/s?
    std::cout << "\n理论分析：" << std::endl;
    std::cout << "  重力沿斜面分量: " << g_parallel << " m/s?" << std::endl;
    
    if (std::abs(relative_v) < 0.1) {
        std::cout << "  ? 两板块保持相对静止" << std::endl;
        std::cout << "  说明: 板块间摩擦力足够" << std::endl;
    } else if (relative_v > 0.1) {
        std::cout << "  ? 上层板块滑得更快" << std::endl;
        std::cout << "  说明: 上层受到的阻力相对较小" << std::endl;
    } else {
        std::cout << "  ? 下层板块滑得更快" << std::endl;
        std::cout << "  说明: 下层承受更大的摩擦力" << std::endl;
    }
    
    // 能量分析
    double ke_total = 0.5 * bottom_block->getMass() * v_bottom * v_bottom +
                      0.5 * top_block->getMass() * v_top * v_top;
    std::cout << "\n能量状态：" << std::endl;
    std::cout << "  总动能: " << ke_total << " J" << std::endl;
    
    delete bottom_block;
    delete top_block;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   板块模型测试套件" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n本测试验证多个板块之间的摩擦力传递" << std::endl;
    std::cout << "包括水平面和斜面上的板块运动" << std::endl;
    std::cout << std::endl;
    
    test_basic_two_blocks();
    std::cout << "\n\n";
    
    test_three_blocks_stack();
    std::cout << "\n\n";
    
    test_block_collision();
    std::cout << "\n\n";
    
    test_pulling_blocks();
    std::cout << "\n\n";
    
    test_blocks_on_slope();
    
    printSeparator();
    std::cout << "所有板块模型测试完成！" << std::endl;
    printSeparator();
    
    return 0;
}
