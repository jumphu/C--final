#include "../include/physicalWorld.h"
#include <iostream>
#include <cmath>
#include <iomanip>

/*=========================================================================================================
 * 斜面滑动测试 - 验证物体在斜面上的运动是否符合物理规律
 * 
 * 测试内容：
 * 1. 无摩擦斜面滑动（理想情况）
 * 2. 有摩擦斜面滑动
 * 3. 不同角度下的滑动
 * 4. 临界角测试（恰好不滑动的角度）
 * 5. 多物体在斜面上的运动
 *=========================================================================================================*/

// 辅助函数：角度转弧度
double degToRad(double deg) {
    return deg * 3.14159265358979323846 / 180.0;
}

// 辅助函数：弧度转角度
double radToDeg(double rad) {
    return rad * 180.0 / 3.14159265358979323846;
}

// 打印测试标题
void printTestHeader(const std::string& title) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << "========================================" << std::endl;
}

// 打印分隔线
void printDivider() {
    std::cout << "----------------------------------------" << std::endl;
}

/*=========================================================================================================
 * 测试1：无摩擦斜面滑动（理想情况）
 * 理论：a = g * sin(θ)
 * 速度：v = a * t = g * sin(θ) * t
 * 位移：s = 0.5 * a * t^2 = 0.5 * g * sin(θ) * t^2
 *=========================================================================================================*/
void test1_frictionless_incline() {
    printTestHeader("测试1：无摩擦斜面滑动");
    
    double angle = 30.0;  // 倾斜角度
    double g = 10.0;      // 重力加速度
    double testTime = 2.0; // 测试时间（秒）
    int frameRate = 60;    // 帧率
    int totalFrames = static_cast<int>(testTime * frameRate);
    
    std::cout << "设置：" << std::endl;
    std::cout << "  倾斜角度: " << angle << "°" << std::endl;
    std::cout << "  重力加速度: " << g << " m/s?" << std::endl;
    std::cout << "  摩擦系数: 0.0 (无摩擦)" << std::endl;
    std::cout << "  测试时间: " << testTime << " 秒" << std::endl;
    
    // 理论计算
    double angleRad = degToRad(angle);
    double theoreticalAccel = g * sin(angleRad);
    double theoreticalVelocity = theoreticalAccel * testTime;
    double theoreticalDisplacement = 0.5 * theoreticalAccel * testTime * testTime;
    
    std::cout << "\n理论值：" << std::endl;
    std::cout << "  加速度: " << theoreticalAccel << " m/s?" << std::endl;
    std::cout << "  最终速度: " << theoreticalVelocity << " m/s" << std::endl;
    std::cout << "  位移: " << theoreticalDisplacement << " m" << std::endl;
    
    // 创建物理世界
    PhysicalWorld world;
    world.setGravity(g);
    world.setInclineAngle(angle);
    world.setTimeStep(1.0 / frameRate);
    
    // 设置地面（无摩擦）
    world.ground.setYLevel(0.0);
    world.ground.setFriction(0.0);
    
    // 创建球体（质量1kg，半径5m）- 放在地面上
    Shape* ball = world.placeDynamicShapeByType("Circle", "TestBall", 0.0, 0.0, 1.0, 5.0);
    ball->setFraction(0.0);  // 无摩擦
    ball->setVelocity(0.0, 0.0);  // 确保初速度为0
    ball->setRestitution(0.0);  // 完全非弹性碰撞，避免弹跳
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(ball);
    
    // 记录初始位置
    double x0, y0;
    ball->getCentre(x0, y0);
    
    std::cout << "\n初始状态：" << std::endl;
    std::cout << "  位置: (" << x0 << ", " << y0 << ")" << std::endl;
    std::cout << "  物体底部Y: " << ball->getBottom() << std::endl;
    std::cout << "  地面Y: " << world.ground.getYLevel() << std::endl;
    
    // 运行模拟
    world.start();
    
    // 先运行几帧让物体稳定在地面上
    for (int i = 0; i < 10; i++) {
        world.update(shapeList, world.ground);
    }
    
    // 记录稳定后的初始位置，并重置速度
    ball->getCentre(x0, y0);
    ball->setVelocity(0.0, 0.0);  // 重置速度为0
    std::cout << "  稳定后位置: (" << x0 << ", " << y0 << ")" << std::endl;
    std::cout << "  被支撑: " << (ball->getIsSupported() ? "是" : "否") << std::endl;
    
    // 运行测试
    for (int i = 0; i < totalFrames; i++) {
        world.update(shapeList, world.ground);
    }
    
    // 获取最终状态
    double xf, yf, vx, vy;
    ball->getCentre(xf, yf);
    ball->getVelocity(vx, vy);
    
    // 计算实际值
    double actualDisplacementX = xf - x0;
    double actualDisplacementY = yf - y0;
    double actualSpeed = sqrt(vx * vx + vy * vy);
    
    // 沿斜面的速度（主要是水平分量）
    double vParallel = fabs(vx);  // 在斜面上，主要是水平运动
    
    std::cout << "\n实际测量值：" << std::endl;
    std::cout << "  位置变化: ΔX = " << actualDisplacementX << " m, ΔY = " << actualDisplacementY << " m" << std::endl;
    std::cout << "  最终速度: vx = " << vx << " m/s, vy = " << vy << " m/s" << std::endl;
    std::cout << "  沿斜面速度 (vx): " << vParallel << " m/s" << std::endl;
    std::cout << "  被支撑: " << (ball->getIsSupported() ? "是" : "否") << std::endl;
    
    // 误差分析（使用vx作为沿斜面的速度）
    std::cout << "\n误差分析：" << std::endl;
    double velocityError = fabs(vParallel - theoreticalVelocity) / theoreticalVelocity * 100.0;
    std::cout << "  理论速度: " << theoreticalVelocity << " m/s" << std::endl;
    std::cout << "  实际速度: " << vParallel << " m/s" << std::endl;
    std::cout << "  速度误差: " << velocityError << "%" << std::endl;
    
    // 判断是否通过
    bool passed = velocityError < 5.0;  // 允许5%误差
    std::cout << "\n测试结果: " << (passed ? "? 通过" : "? 失败") << std::endl;
}

/*=========================================================================================================
 * 测试2：有摩擦斜面滑动
 * 理论：a = g * (sin(θ) - μ * cos(θ))
 * 其中 μ 是动摩擦系数
 *=========================================================================================================*/
void test2_friction_incline() {
    printTestHeader("测试2：有摩擦斜面滑动");
    
    double angle = 30.0;       // 倾斜角度
    double g = 10.0;           // 重力加速度
    double friction = 0.2;     // 摩擦系数
    double testTime = 2.0;     // 测试时间（秒）
    int frameRate = 60;
    int totalFrames = static_cast<int>(testTime * frameRate);
    
    std::cout << "设置：" << std::endl;
    std::cout << "  倾斜角度: " << angle << "°" << std::endl;
    std::cout << "  重力加速度: " << g << " m/s?" << std::endl;
    std::cout << "  摩擦系数: " << friction << std::endl;
    std::cout << "  测试时间: " << testTime << " 秒" << std::endl;
    
    // 理论计算
    double angleRad = degToRad(angle);
    double theoreticalAccel = g * (sin(angleRad) - friction * cos(angleRad));
    double theoreticalVelocity = theoreticalAccel * testTime;
    
    std::cout << "\n理论值：" << std::endl;
    std::cout << "  加速度: " << theoreticalAccel << " m/s?" << std::endl;
    std::cout << "  最终速度: " << theoreticalVelocity << " m/s" << std::endl;
    
    // 创建物理世界
    PhysicalWorld world;
    world.setGravity(g);
    world.setInclineAngle(angle);
    world.setTimeStep(1.0 / frameRate);
    
    // 设置地面（有摩擦）
    world.ground.setYLevel(0.0);
    world.ground.setFriction(friction);
    
    // 创建球体 - 放在地面上
    Shape* ball = world.placeDynamicShapeByType("Circle", "TestBall", 0.0, 0.0, 1.0, 5.0);
    ball->setFraction(friction);
    ball->setVelocity(0.0, 0.0);
    ball->setRestitution(0.0);  // 完全非弹性碰撞
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(ball);
    
    // 运行模拟
    world.start();
    
    // 先让物体稳定
    for (int i = 0; i < 10; i++) {
        world.update(shapeList, world.ground);
    }
    
    double x0, y0;
    ball->getCentre(x0, y0);
    ball->setVelocity(0.0, 0.0);  // 重置速度为0
    std::cout << "\n稳定后初始状态：" << std::endl;
    std::cout << "  位置: (" << x0 << ", " << y0 << ")" << std::endl;
    std::cout << "  被支撑: " << (ball->getIsSupported() ? "是" : "否") << std::endl;
    
    // 运行测试
    for (int i = 0; i < totalFrames; i++) {
        world.update(shapeList, world.ground);
    }
    
    // 获取最终状态
    double xf, yf, vx, vy;
    ball->getCentre(xf, yf);
    ball->getVelocity(vx, vy);
    
    double actualSpeed = fabs(vx);  // 沿斜面的速度（水平分量）
    
    std::cout << "\n实际测量值：" << std::endl;
    std::cout << "  最终速度: vx = " << vx << " m/s, vy = " << vy << " m/s" << std::endl;
    std::cout << "  沿斜面速度: " << actualSpeed << " m/s" << std::endl;
    std::cout << "  被支撑: " << (ball->getIsSupported() ? "是" : "否") << std::endl;
    
    // 误差分析
    std::cout << "\n误差分析：" << std::endl;
    double velocityError = fabs(actualSpeed - theoreticalVelocity) / (theoreticalVelocity + 0.01) * 100.0;
    std::cout << "  理论速度: " << theoreticalVelocity << " m/s" << std::endl;
    std::cout << "  实际速度: " << actualSpeed << " m/s" << std::endl;
    std::cout << "  速度误差: " << velocityError << "%" << std::endl;
    
    bool passed = velocityError < 10.0;
    std::cout << "\n测试结果: " << (passed ? "? 通过" : "? 失败") << std::endl;
}

/*=========================================================================================================
 * 测试3：不同角度下的滑动对比
 *=========================================================================================================*/
void test3_different_angles() {
    printTestHeader("测试3：不同角度下的滑动对比");
    
    double angles[] = {15.0, 30.0, 45.0, 60.0};
    double g = 10.0;
    double testTime = 1.0;
    int frameRate = 60;
    int totalFrames = static_cast<int>(testTime * frameRate);
    
    std::cout << "测试不同角度下物体的加速度和速度\n" << std::endl;
    std::cout << std::setw(10) << "角度(°)" 
              << std::setw(15) << "理论加速度" 
              << std::setw(15) << "理论速度"
              << std::setw(15) << "实际速度"
              << std::setw(12) << "误差(%)" << std::endl;
    printDivider();
    
    for (double angle : angles) {
        // 理论计算
        double angleRad = degToRad(angle);
        double theoreticalAccel = g * sin(angleRad);
        double theoreticalVelocity = theoreticalAccel * testTime;
        
        // 创建物理世界
        PhysicalWorld world;
        world.setGravity(g);
        world.setInclineAngle(angle);
        world.setTimeStep(1.0 / frameRate);
        world.ground.setYLevel(0.0);
        world.ground.setFriction(0.0);
        
        Shape* ball = world.placeDynamicShapeByType("Circle", "Ball", 0.0, 0.0, 1.0, 5.0);
        ball->setFraction(0.0);
        ball->setVelocity(0.0, 0.0);
        ball->setRestitution(0.0);
        
        std::vector<Shape*> shapeList;
        shapeList.push_back(ball);
        
        // 运行模拟
        world.start();
        
        // 先稳定
        for (int i = 0; i < 10; i++) {
            world.update(shapeList, world.ground);
        }
        
        ball->setVelocity(0.0, 0.0);  // 重置速度为0
        
        // 运行测试
        for (int i = 0; i < totalFrames; i++) {
            world.update(shapeList, world.ground);
        }
        
        // 获取结果
        double vx, vy;
        ball->getVelocity(vx, vy);
        double actualSpeed = fabs(vx);  // 沿斜面速度
        double error = fabs(actualSpeed - theoreticalVelocity) / theoreticalVelocity * 100.0;
        
        std::cout << std::setw(10) << std::fixed << std::setprecision(1) << angle
                  << std::setw(15) << std::setprecision(3) << theoreticalAccel
                  << std::setw(15) << theoreticalVelocity
                  << std::setw(15) << actualSpeed
                  << std::setw(12) << std::setprecision(2) << error << std::endl;
    }
    
    std::cout << "\n? 对比测试完成" << std::endl;
}

/*=========================================================================================================
 * 测试4：临界角测试（物体恰好开始滑动的角度）
 * 临界条件：tan(θ) = μ_s （静摩擦系数）
 *=========================================================================================================*/
void test4_critical_angle() {
    printTestHeader("测试4：临界角测试");
    
    double friction = 0.3;  // 静摩擦系数
    double criticalAngle = radToDeg(atan(friction));
    
    std::cout << "摩擦系数 μ = " << friction << std::endl;
    std::cout << "理论临界角 θ_c = arctan(μ) = " << criticalAngle << "°" << std::endl;
    std::cout << "\n测试不同角度下物体是否滑动：\n" << std::endl;
    
    // 测试几个角度
    double testAngles[] = {
        criticalAngle - 5.0,  // 小于临界角
        criticalAngle,         // 等于临界角
        criticalAngle + 5.0    // 大于临界角
    };
    
    double g = 10.0;
    double testTime = 2.0;
    int frameRate = 60;
    int totalFrames = static_cast<int>(testTime * frameRate);
    
    for (double angle : testAngles) {
        std::cout << "角度 = " << std::fixed << std::setprecision(1) << angle << "°: ";
        
        PhysicalWorld world;
        world.setGravity(g);
        world.setInclineAngle(angle);
        world.setTimeStep(1.0 / frameRate);
        world.ground.setYLevel(0.0);
        world.ground.setFriction(friction);
        
        Shape* ball = world.placeDynamicShapeByType("Circle", "Ball", 0.0, 0.0, 1.0, 5.0);
        ball->setFraction(friction);
        ball->setVelocity(0.0, 0.0);
        ball->setRestitution(0.0);
        
        std::vector<Shape*> shapeList;
        shapeList.push_back(ball);
        
        double x0, y0;
        
        // 运行模拟
        world.start();
        
        // 先稳定
        for (int i = 0; i < 10; i++) {
            world.update(shapeList, world.ground);
        }
        
        ball->getCentre(x0, y0);
        
        // 运行测试
        for (int i = 0; i < totalFrames; i++) {
            world.update(shapeList, world.ground);
        }
        
        double xf, yf, vx, vy;
        ball->getCentre(xf, yf);
        ball->getVelocity(vx, vy);
        
        double displacement = fabs(xf - x0);
        double speed = fabs(vx);
        
        if (angle < criticalAngle - 1.0) {
            std::cout << "应该不滑动或缓慢滑动 - ";
        } else if (angle > criticalAngle + 1.0) {
            std::cout << "应该明显滑动 - ";
        } else {
            std::cout << "临界状态 - ";
        }
        
        if (speed > 0.5) {
            std::cout << "? 正在滑动 (v = " << std::setprecision(2) << speed << " m/s, Δx = " << displacement << " m)" << std::endl;
        } else {
            std::cout << "○ 几乎静止 (v = " << std::setprecision(2) << speed << " m/s, Δx = " << displacement << " m)" << std::endl;
        }
    }
    
    std::cout << "\n? 临界角测试完成" << std::endl;
}

/*=========================================================================================================
 * 测试5：多物体在斜面上的运动（质量不同）
 * 理论：在无空气阻力情况下，不同质量的物体加速度相同
 *=========================================================================================================*/
void test5_multiple_objects() {
    printTestHeader("测试5：多物体斜面滑动（不同质量）");
    
    double angle = 30.0;
    double g = 10.0;
    double testTime = 1.5;
    int frameRate = 60;
    int totalFrames = static_cast<int>(testTime * frameRate);
    
    std::cout << "设置：倾斜角度 = " << angle << "°, 无摩擦" << std::endl;
    std::cout << "理论：不同质量的物体应该以相同加速度下滑\n" << std::endl;
    
    // 创建物理世界
    PhysicalWorld world;
    world.setGravity(g);
    world.setInclineAngle(angle);
    world.setTimeStep(1.0 / frameRate);
    world.ground.setYLevel(0.0);
    world.ground.setFriction(0.0);
    
    // 创建不同质量的物体 - 放在地面上
    Shape* ball1 = world.placeDynamicShapeByType("Circle", "轻球", 0.0, 0.0, 1.0, 5.0);
    Shape* ball2 = world.placeDynamicShapeByType("Circle", "中球", 0.0, 0.0, 5.0, 5.0);
    Shape* ball3 = world.placeDynamicShapeByType("Circle", "重球", 0.0, 0.0, 10.0, 5.0);
    
    ball1->setFraction(0.0);
    ball2->setFraction(0.0);
    ball3->setFraction(0.0);
    
    ball1->setVelocity(0.0, 0.0);
    ball2->setVelocity(0.0, 0.0);
    ball3->setVelocity(0.0, 0.0);
    
    ball1->setRestitution(0.0);
    ball2->setRestitution(0.0);
    ball3->setRestitution(0.0);
    
    std::vector<Shape*> shapeList = {ball1, ball2, ball3};
    
    // 运行模拟
    world.start();
    
    // 先稳定
    for (int i = 0; i < 10; i++) {
        world.update(shapeList, world.ground);
    }
    
    // 运行测试
    for (int i = 0; i < totalFrames; i++) {
        world.update(shapeList, world.ground);
    }
    
    // 获取结果
    std::cout << std::setw(12) << "物体" 
              << std::setw(12) << "质量(kg)"
              << std::setw(15) << "速度(m/s)"
              << std::setw(15) << "位置X(m)" << std::endl;
    printDivider();
    
    for (Shape* ball : shapeList) {
        double x, y, vx, vy;
        ball->getCentre(x, y);
        ball->getVelocity(vx, vy);
        double speed = fabs(vx);  // 沿斜面速度
        
        std::cout << std::setw(12) << ball->getName()
                  << std::setw(12) << std::fixed << std::setprecision(1) << ball->getMass()
                  << std::setw(15) << std::setprecision(3) << speed
                  << std::setw(15) << std::setprecision(2) << x << std::endl;
    }
    
    // 检查速度是否相近
    double v1, v2, v3;
    double vx, vy;
    ball1->getVelocity(vx, vy);
    v1 = fabs(vx);
    ball2->getVelocity(vx, vy);
    v2 = fabs(vx);
    ball3->getVelocity(vx, vy);
    v3 = fabs(vx);
    
    double maxDiff = fabs(v1 - v2);
    if (fabs(v2 - v3) > maxDiff) maxDiff = fabs(v2 - v3);
    if (fabs(v1 - v3) > maxDiff) maxDiff = fabs(v1 - v3);
    double avgSpeed = (v1 + v2 + v3) / 3.0;
    double relativeDiff = (maxDiff / (avgSpeed + 0.01)) * 100.0;
    
    std::cout << "\n速度差异分析：" << std::endl;
    std::cout << "  平均速度: " << avgSpeed << " m/s" << std::endl;
    std::cout << "  最大差异: " << maxDiff << " m/s" << std::endl;
    std::cout << "  相对差异: " << relativeDiff << "%" << std::endl;
    
    bool passed = relativeDiff < 2.0;  // 允许2%差异
    std::cout << "\n测试结果: " << (passed ? "? 通过（质量不影响加速度）" : "? 失败") << std::endl;
}

/*=========================================================================================================
 * 测试6：长时间滑动轨迹追踪
 *=========================================================================================================*/
void test6_trajectory_tracking() {
    printTestHeader("测试6：滑动轨迹追踪");
    
    double angle = 30.0;
    double g = 10.0;
    double testTime = 3.0;
    int frameRate = 60;
    int totalFrames = static_cast<int>(testTime * frameRate);
    int printInterval = 30;  // 每0.5秒打印一次
    
    std::cout << "倾斜角度: " << angle << "°, 无摩擦" << std::endl;
    std::cout << "追踪 " << testTime << " 秒内的运动轨迹\n" << std::endl;
    
    PhysicalWorld world;
    world.setGravity(g);
    world.setInclineAngle(angle);
    world.setTimeStep(1.0 / frameRate);
    
    Ground ground(0.0, 0.0);
    
    Shape* ball = world.placeDynamicShapeByType("Circle", "Ball", 0.0, 0.0, 1.0, 5.0);
    ball->setFraction(0.0);
    ball->setVelocity(0.0, 0.0);
    ball->setRestitution(0.0);
    
    std::vector<Shape*> shapeList;
    shapeList.push_back(ball);
    
    std::cout << std::setw(10) << "时间(s)"
              << std::setw(12) << "X(m)"
              << std::setw(12) << "Y(m)"
              << std::setw(12) << "Vx(m/s)"
              << std::setw(12) << "Vy(m/s)"
              << std::setw(12) << "支撑状态" << std::endl;
    printDivider();
    
    world.start();
    
    // 先稳定
    for (int i = 0; i < 10; i++) {
        world.update(shapeList, world.ground);
    }
    
    // 记录起始位置
    double x_start, y_start;
    ball->getCentre(x_start, y_start);
    
    for (int i = 0; i <= totalFrames; i++) {
        if (i % printInterval == 0) {
            double x, y, vx, vy;
            ball->getCentre(x, y);
            ball->getVelocity(vx, vy);
            double time = i / static_cast<double>(frameRate);
            
            std::cout << std::setw(10) << std::fixed << std::setprecision(2) << time
                      << std::setw(12) << std::setprecision(2) << x
                      << std::setw(12) << std::setprecision(2) << y
                      << std::setw(12) << std::setprecision(2) << vx
                      << std::setw(12) << std::setprecision(2) << vy
                      << std::setw(12) << (ball->getIsSupported() ? "支撑" : "空中") << std::endl;
        }
        
        if (i < totalFrames) {
            world.update(shapeList, world.ground);
        }
    }
    
    std::cout << "\n? 轨迹追踪完成" << std::endl;
}

/*=========================================================================================================
 * 主函数
 *=========================================================================================================*/
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    斜面滑动物理测试套件" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n本测试验证物体在斜面上的运动是否符合物理规律" << std::endl;
    std::cout << "包括重力分解、摩擦力、加速度等" << std::endl;
    
    try {
        test1_frictionless_incline();
        test2_friction_incline();
        test3_different_angles();
        test4_critical_angle();
        test5_multiple_objects();
        test6_trajectory_tracking();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "   ? 所有斜面滑动测试完成！" << std::endl;
        std::cout << "========================================" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "\n? 测试出错: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
