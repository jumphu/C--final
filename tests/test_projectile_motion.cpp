/*=========================================================================================================
 * 斜面抛体运动测试 - 测试物体从斜面抛出的平抛和斜抛运动
 * 
 * 测试场景：
 * 1. 平抛运动：物体从斜面边缘水平抛出
 * 2. 斜抛运动：物体以一定角度从斜面抛出
 * 3. 沿斜面方向抛出：初速度沿斜面方向
 * 4. 垂直斜面抛出：初速度垂直于斜面
 * 5. 不同角度抛出对比
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

struct TrajectoryPoint {
    double t;     // 时间
    double x;     // X坐标
    double y;     // Y坐标
    double vx;    // X方向速度
    double vy;    // Y方向速度
};

// 记录轨迹点
void recordTrajectory(Circle* ball, double time, std::vector<TrajectoryPoint>& trajectory) {
    TrajectoryPoint point;
    point.t = time;
    ball->getCentre(point.x, point.y);
    ball->getVelocity(point.vx, point.vy);
    trajectory.push_back(point);
}

// 打印轨迹数据
void printTrajectory(const std::vector<TrajectoryPoint>& trajectory, int step = 1) {
    std::cout << std::left 
              << std::setw(10) << "时间(s)"
              << std::setw(12) << "X(m)"
              << std::setw(12) << "Y(m)"
              << std::setw(12) << "Vx(m/s)"
              << std::setw(12) << "Vy(m/s)"
              << std::setw(12) << "速率(m/s)" << std::endl;
    printSubSeparator();
    
    for (size_t i = 0; i < trajectory.size(); i += step) {
        const auto& p = trajectory[i];
        double speed = std::sqrt(p.vx * p.vx + p.vy * p.vy);
        std::cout << std::fixed << std::setprecision(3)
                  << std::left
                  << std::setw(10) << p.t
                  << std::setw(12) << p.x
                  << std::setw(12) << p.y
                  << std::setw(12) << p.vx
                  << std::setw(12) << p.vy
                  << std::setw(12) << speed << std::endl;
    }
}

// 理论计算：平抛运动
void calculateHorizontalProjection(double v0, double h, double g, 
                                   double& t_flight, double& x_range, double& v_final) {
    // 自由落体时间: h = 0.5 * g * t^2
    t_flight = std::sqrt(2.0 * h / g);
    // 水平射程: x = v0 * t
    x_range = v0 * t_flight;
    // 落地速度: v = sqrt(v0^2 + (g*t)^2)
    double vy = g * t_flight;
    v_final = std::sqrt(v0 * v0 + vy * vy);
}

// 理论计算：斜抛运动
void calculateObliqueProjection(double v0, double angle_deg, double h0, double g,
                                double& t_flight, double& x_range, double& v_final) {
    double angle_rad = angle_deg * PI / 180.0;
    double v0x = v0 * std::cos(angle_rad);
    double v0y = v0 * std::sin(angle_rad);
    
    // 使用一元二次方程求解落地时间: h0 + v0y*t - 0.5*g*t^2 = 0
    // -0.5*g*t^2 + v0y*t + h0 = 0
    double a = -0.5 * g;
    double b = v0y;
    double c = h0;
    double discriminant = b * b - 4 * a * c;
    
    if (discriminant >= 0) {
        t_flight = (-b + std::sqrt(discriminant)) / (2 * a);
        x_range = v0x * t_flight;
        double vy_final = v0y - g * t_flight;
        v_final = std::sqrt(v0x * v0x + vy_final * vy_final);
    } else {
        t_flight = x_range = v_final = 0;
    }
}

// 测试1：水平平抛运动
void test_horizontal_projection() {
    printSeparator();
    std::cout << "测试1：水平平抛运动" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);  // 水平面
    world.setTimeStep(1.0 / 60.0);
    world.ground.setYLevel(0.0);
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  球从 20m 高度以 15 m/s 水平速度抛出" << std::endl;
    std::cout << "  重力加速度: 10 m/s?" << std::endl;
    std::cout << "  忽略空气阻力" << std::endl;
    
    // 创建球，初始位置在高处，水平速度
    double h0 = 20.0;
    double v0 = 15.0;
    Circle* ball = new Circle(1.0, 1.0, 0.0, h0, v0, 0.0);
    ball->setName("Ball");
    world.addDynamicShape(ball);
    
    std::cout << "\n初始状态：" << std::endl;
    std::cout << "  位置: (0, " << h0 << ") m" << std::endl;
    std::cout << "  速度: (" << v0 << ", 0) m/s" << std::endl;
    
    // 理论计算
    double t_theory, x_theory, v_theory;
    calculateHorizontalProjection(v0, h0, 10.0, t_theory, x_theory, v_theory);
    
    std::cout << "\n理论预测：" << std::endl;
    std::cout << "  飞行时间: " << t_theory << " s" << std::endl;
    std::cout << "  水平射程: " << x_theory << " m" << std::endl;
    std::cout << "  落地速度: " << v_theory << " m/s" << std::endl;
    
    // 运行模拟
    std::vector<TrajectoryPoint> trajectory;
    bool has_landed = false;
    double landing_time = 0;
    double landing_x = 0, landing_y = 0;
    double landing_vx = 0, landing_vy = 0;
    
    std::cout << "\n模拟运行..." << std::endl;
    for (int i = 0; i < 600; ++i) {
        double current_time = i * world.getTimeStep();
        
        // 每10帧记录一次轨迹
        if (i % 10 == 0) {
            recordTrajectory(ball, current_time, trajectory);
        }
        
        world.update(world.dynamicShapeList, world.ground);
        
        // 检查是否落地
        double x, y;
        ball->getCentre(x, y);
        if (ball->getBottom() <= world.ground.getYLevel() && !has_landed) {
            has_landed = true;
            landing_time = current_time;
            landing_x = x;
            landing_y = y;
            ball->getVelocity(landing_vx, landing_vy);
            
            std::cout << "\n球落地！第 " << i << " 帧" << std::endl;
            recordTrajectory(ball, current_time, trajectory);
            break;
        }
    }
    
    // 显示部分轨迹
    std::cout << "\n轨迹数据（每10帧显示一次）：" << std::endl;
    printTrajectory(trajectory);
    
    // 结果验证
    printSubSeparator();
    std::cout << "\n结果验证：" << std::endl;
    
    double landing_speed = std::sqrt(landing_vx * landing_vx + landing_vy * landing_vy);
    
    std::cout << "\n实际测量：" << std::endl;
    std::cout << "  飞行时间: " << landing_time << " s" << std::endl;
    std::cout << "  落地位置: (" << landing_x << ", " << landing_y << ") m" << std::endl;
    std::cout << "  落地速度: " << landing_speed << " m/s" << std::endl;
    std::cout << "  速度分量: (" << landing_vx << ", " << landing_vy << ") m/s" << std::endl;
    
    std::cout << "\n误差分析：" << std::endl;
    double time_error = std::abs(landing_time - t_theory) / t_theory * 100;
    double range_error = std::abs(landing_x - x_theory) / x_theory * 100;
    double speed_error = std::abs(landing_speed - v_theory) / v_theory * 100;
    
    std::cout << "  飞行时间误差: " << time_error << "%" << std::endl;
    std::cout << "  水平射程误差: " << range_error << "%" << std::endl;
    std::cout << "  落地速度误差: " << speed_error << "%" << std::endl;
    
    if (time_error < 2.0 && range_error < 2.0 && speed_error < 2.0) {
        std::cout << "\n? 平抛运动模拟准确" << std::endl;
    } else {
        std::cout << "\n? 存在较大误差" << std::endl;
    }
    
    delete ball;
}

// 测试2：斜抛运动（向上抛）
void test_oblique_projection_upward() {
    printSeparator();
    std::cout << "测试2：斜抛运动（向上45°）" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);
    world.setTimeStep(1.0 / 60.0);
    world.ground.setYLevel(0.0);
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  球从 10m 高度以 20 m/s、45° 角度抛出" << std::endl;
    std::cout << "  重力加速度: 10 m/s?" << std::endl;
    
    // 初始条件
    double h0 = 10.0;
    double v0 = 20.0;
    double angle = 45.0;
    double angle_rad = angle * PI / 180.0;
    double v0x = v0 * std::cos(angle_rad);
    double v0y = v0 * std::sin(angle_rad);
    
    Circle* ball = new Circle(1.0, 1.0, 0.0, h0, v0x, v0y);
    ball->setName("Ball");
    world.addDynamicShape(ball);
    
    std::cout << "\n初始状态：" << std::endl;
    std::cout << "  位置: (0, " << h0 << ") m" << std::endl;
    std::cout << "  速度: (" << v0x << ", " << v0y << ") m/s" << std::endl;
    std::cout << "  抛射角: " << angle << "°" << std::endl;
    
    // 理论计算
    double t_theory, x_theory, v_theory;
    calculateObliqueProjection(v0, angle, h0, 10.0, t_theory, x_theory, v_theory);
    
    std::cout << "\n理论预测：" << std::endl;
    std::cout << "  飞行时间: " << t_theory << " s" << std::endl;
    std::cout << "  水平射程: " << x_theory << " m" << std::endl;
    std::cout << "  最高点高度: " << (h0 + v0y * v0y / (2 * 10.0)) << " m" << std::endl;
    std::cout << "  落地速度: " << v_theory << " m/s" << std::endl;
    
    // 运行模拟
    std::vector<TrajectoryPoint> trajectory;
    bool has_landed = false;
    double max_height = h0;
    double landing_time = 0;
    
    std::cout << "\n模拟运行..." << std::endl;
    for (int i = 0; i < 600; ++i) {
        double current_time = i * world.getTimeStep();
        
        // 每5帧记录一次
        if (i % 5 == 0) {
            recordTrajectory(ball, current_time, trajectory);
        }
        
        world.update(world.dynamicShapeList, world.ground);
        
        double x, y;
        ball->getCentre(x, y);
        
        // 记录最高点
        if (y > max_height) {
            max_height = y;
        }
        
        // 检查落地
        if (ball->getBottom() <= world.ground.getYLevel() && !has_landed) {
            has_landed = true;
            landing_time = current_time;
            recordTrajectory(ball, current_time, trajectory);
            std::cout << "\n球落地！第 " << i << " 帧" << std::endl;
            break;
        }
    }
    
    // 显示轨迹
    std::cout << "\n轨迹数据（部分）：" << std::endl;
    printTrajectory(trajectory, 5);  // 每5个点显示1个
    
    // 结果验证
    printSubSeparator();
    std::cout << "\n结果验证：" << std::endl;
    
    const auto& landing = trajectory.back();
    double landing_speed = std::sqrt(landing.vx * landing.vx + landing.vy * landing.vy);
    
    std::cout << "实际测量：" << std::endl;
    std::cout << "  飞行时间: " << landing.t << " s" << std::endl;
    std::cout << "  水平射程: " << landing.x << " m" << std::endl;
    std::cout << "  最高点高度: " << max_height << " m" << std::endl;
    std::cout << "  落地速度: " << landing_speed << " m/s" << std::endl;
    
    std::cout << "\n误差分析：" << std::endl;
    double range_error = std::abs(landing.x - x_theory) / x_theory * 100;
    double speed_error = std::abs(landing_speed - v_theory) / v_theory * 100;
    
    std::cout << "  水平射程误差: " << range_error << "%" << std::endl;
    std::cout << "  落地速度误差: " << speed_error << "%" << std::endl;
    
    if (range_error < 2.0 && speed_error < 2.0) {
        std::cout << "\n? 斜抛运动模拟准确" << std::endl;
    } else {
        std::cout << "\n? 存在较大误差" << std::endl;
    }
    
    delete ball;
}

// 测试3：从斜面抛出（沿斜面方向）
void test_projection_along_slope() {
    printSeparator();
    std::cout << "测试3：沿斜面方向抛出" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(30.0);  // 30度斜面
    world.setTimeStep(1.0 / 60.0);
    world.ground.setYLevel(0.0);
    
    double slope_angle = 30.0;
    double angle_rad = slope_angle * PI / 180.0;
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  30° 斜面，球从斜面上以 10 m/s 沿斜面向上抛出" << std::endl;
    std::cout << "  重力加速度: 10 m/s?" << std::endl;
    
    // 初始位置和速度
    double x0 = 0.0;
    double y0 = 5.0;  // 在斜面上
    double v0 = 10.0;  // 沿斜面方向的速度
    double vx = v0 * std::cos(angle_rad);  // 水平分量
    double vy = v0 * std::sin(angle_rad);  // 垂直分量
    
    Circle* ball = new Circle(1.0, 1.0, x0, y0, vx, vy);
    ball->setName("Ball");
    world.addDynamicShape(ball);
    
    std::cout << "\n初始状态：" << std::endl;
    std::cout << "  位置: (" << x0 << ", " << y0 << ") m" << std::endl;
    std::cout << "  速度: (" << vx << ", " << vy << ") m/s" << std::endl;
    std::cout << "  沿斜面速度: " << v0 << " m/s" << std::endl;
    
    // 在倾斜坐标系中，沿斜面的加速度
    double a_along_slope = -10.0 * std::sin(angle_rad);  // g·sin(θ)
    std::cout << "\n理论分析：" << std::endl;
    std::cout << "  沿斜面加速度: " << a_along_slope << " m/s?" << std::endl;
    
    // 运行模拟
    std::vector<TrajectoryPoint> trajectory;
    bool returned_to_slope = false;
    double max_distance_along_slope = 0;
    
    std::cout << "\n模拟运行..." << std::endl;
    for (int i = 0; i < 300; ++i) {
        double current_time = i * world.getTimeStep();
        
        if (i % 10 == 0) {
            recordTrajectory(ball, current_time, trajectory);
        }
        
        world.update(world.dynamicShapeList, world.ground);
        
        double x, y;
        ball->getCentre(x, y);
        
        // 计算沿斜面的距离
        double s_along_slope = std::sqrt(x * x + y * y);
        if (s_along_slope > max_distance_along_slope) {
            max_distance_along_slope = s_along_slope;
        }
        
        // 检查是否回到斜面
        if (i > 10 && ball->getIsSupported() && !returned_to_slope) {
            returned_to_slope = true;
            recordTrajectory(ball, current_time, trajectory);
            std::cout << "\n球返回斜面！第 " << i << " 帧" << std::endl;
            std::cout << "  位置: (" << x << ", " << y << ") m" << std::endl;
            break;
        }
    }
    
    // 显示轨迹
    std::cout << "\n轨迹数据：" << std::endl;
    printTrajectory(trajectory);
    
    // 结果验证
    printSubSeparator();
    std::cout << "\n结果验证：" << std::endl;
    
    if (returned_to_slope) {
        const auto& landing = trajectory.back();
        std::cout << "? 球成功返回斜面" << std::endl;
        std::cout << "  飞行时间: " << landing.t << " s" << std::endl;
        std::cout << "  着陆位置: (" << landing.x << ", " << landing.y << ") m" << std::endl;
        std::cout << "  沿斜面最大距离: " << max_distance_along_slope << " m" << std::endl;
        
        // 理论计算：沿斜面的最大距离 = v0^2 / (2 * |a|)
        double s_theory = v0 * v0 / (2 * std::abs(a_along_slope));
        std::cout << "\n理论最大距离: " << s_theory << " m" << std::endl;
        double error = std::abs(max_distance_along_slope - s_theory) / s_theory * 100;
        std::cout << "误差: " << error << "%" << std::endl;
        
        if (error < 5.0) {
            std::cout << "\n? 沿斜面抛出模拟准确" << std::endl;
        }
    } else {
        std::cout << "? 球未返回斜面（可能飞得太远）" << std::endl;
    }
    
    delete ball;
}

// 测试4：垂直斜面抛出
void test_projection_perpendicular_to_slope() {
    printSeparator();
    std::cout << "测试4：垂直斜面抛出" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(30.0);
    world.setTimeStep(1.0 / 60.0);
    world.ground.setYLevel(0.0);
    
    double slope_angle = 30.0;
    double angle_rad = slope_angle * PI / 180.0;
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  30° 斜面，球以 8 m/s 垂直斜面向上抛出" << std::endl;
    
    // 垂直斜面的方向是 (斜面法向量)
    double x0 = 0.0;
    double y0 = 5.0;
    double v0 = 8.0;
    
    // 垂直斜面向上的速度分量
    // 法向量: (-sin(θ), cos(θ))
    double vx = -v0 * std::sin(angle_rad);
    double vy = v0 * std::cos(angle_rad);
    
    Circle* ball = new Circle(1.0, 1.0, x0, y0, vx, vy);
    ball->setName("Ball");
    world.addDynamicShape(ball);
    
    std::cout << "\n初始状态：" << std::endl;
    std::cout << "  位置: (" << x0 << ", " << y0 << ") m" << std::endl;
    std::cout << "  速度: (" << vx << ", " << vy << ") m/s" << std::endl;
    std::cout << "  垂直斜面速度: " << v0 << " m/s" << std::endl;
    
    // 运行模拟
    std::vector<TrajectoryPoint> trajectory;
    double max_height = y0;
    
    std::cout << "\n模拟运行..." << std::endl;
    for (int i = 0; i < 300; ++i) {
        double current_time = i * world.getTimeStep();
        
        if (i % 10 == 0) {
            recordTrajectory(ball, current_time, trajectory);
        }
        
        world.update(world.dynamicShapeList, world.ground);
        
        double x, y;
        ball->getCentre(x, y);
        
        if (y > max_height) {
            max_height = y;
        }
        
        // 检查是否回到斜面
        if (i > 10 && ball->getIsSupported()) {
            recordTrajectory(ball, current_time, trajectory);
            std::cout << "\n球返回斜面！第 " << i << " 帧" << std::endl;
            break;
        }
    }
    
    // 显示轨迹
    std::cout << "\n轨迹数据：" << std::endl;
    printTrajectory(trajectory);
    
    // 结果验证
    printSubSeparator();
    std::cout << "\n结果验证：" << std::endl;
    
    const auto& landing = trajectory.back();
    std::cout << "  飞行时间: " << landing.t << " s" << std::endl;
    std::cout << "  最大高度: " << max_height << " m" << std::endl;
    std::cout << "  着陆位置: (" << landing.x << ", " << landing.y << ") m" << std::endl;
    
    // 理论分析：垂直斜面的运动
    // 垂直方向的加速度成分: a_perp = -g·cos(θ)
    double a_perp = -10.0 * std::cos(angle_rad);
    double t_up = v0 / std::abs(a_perp);
    double h_max_theory = y0 + v0 * v0 / (2 * std::abs(a_perp));
    
    std::cout << "\n理论分析：" << std::endl;
    std::cout << "  垂直斜面加速度: " << a_perp << " m/s?" << std::endl;
    std::cout << "  上升时间: " << t_up << " s" << std::endl;
    std::cout << "  理论最大高度: " << h_max_theory << " m" << std::endl;
    std::cout << "  实际最大高度: " << max_height << " m" << std::endl;
    
    double error = std::abs(max_height - h_max_theory) / h_max_theory * 100;
    std::cout << "  高度误差: " << error << "%" << std::endl;
    
    if (error < 5.0) {
        std::cout << "\n? 垂直斜面抛出模拟准确" << std::endl;
    }
    
    delete ball;
}

// 测试5：综合测试 - 不同角度抛出
void test_various_projection_angles() {
    printSeparator();
    std::cout << "测试5：不同角度抛出对比" << std::endl;
    printSeparator();
    
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);
    world.setTimeStep(1.0 / 60.0);
    world.ground.setYLevel(0.0);
    
    std::cout << "场景设置：" << std::endl;
    std::cout << "  从 10m 高度以 15 m/s 初速度，不同角度抛出" << std::endl;
    std::cout << "  比较射程和飞行时间" << std::endl;
    
    double h0 = 10.0;
    double v0 = 15.0;
    std::vector<double> angles = {0, 15, 30, 45, 60, 75, 90};
    
    std::cout << "\n" << std::left
              << std::setw(12) << "角度(°)"
              << std::setw(15) << "飞行时间(s)"
              << std::setw(15) << "水平射程(m)"
              << std::setw(15) << "落地速度(m/s)" << std::endl;
    printSubSeparator();
    
    for (double angle : angles) {
        double angle_rad = angle * PI / 180.0;
        double vx = v0 * std::cos(angle_rad);
        double vy = v0 * std::sin(angle_rad);
        
        Circle* ball = new Circle(1.0, 1.0, 0.0, h0, vx, vy);
        world.dynamicShapeList.clear();
        world.addDynamicShape(ball);
        
        // 运行直到落地
        bool landed = false;
        double landing_time = 0;
        double landing_x = 0;
        double landing_speed = 0;
        
        for (int i = 0; i < 600; ++i) {
            world.update(world.dynamicShapeList, world.ground);
            
            if (ball->getBottom() <= 0 && !landed) {
                landed = true;
                landing_time = i * world.getTimeStep();
                double x, y;
                ball->getCentre(x, y);
                landing_x = x;
                landing_speed = ball->getSpeed();
                break;
            }
        }
        
        std::cout << std::fixed << std::setprecision(2)
                  << std::left
                  << std::setw(12) << angle
                  << std::setw(15) << landing_time
                  << std::setw(15) << landing_x
                  << std::setw(15) << landing_speed << std::endl;
        
        delete ball;
    }
    
    std::cout << "\n观察：" << std::endl;
    std::cout << "  - 0°（水平）: 最短飞行时间，中等射程" << std::endl;
    std::cout << "  - 30-45°: 较好的射程和飞行时间平衡" << std::endl;
    std::cout << "  - 90°（垂直）: 射程为0，最长飞行时间" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   斜面抛体运动测试套件" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n本测试验证从斜面抛出物体的运动轨迹" << std::endl;
    std::cout << "包括平抛、斜抛、沿斜面抛出等场景" << std::endl;
    std::cout << std::endl;
    
    test_horizontal_projection();
    std::cout << "\n\n";
    
    test_oblique_projection_upward();
    std::cout << "\n\n";
    
    test_projection_along_slope();
    std::cout << "\n\n";
    
    test_projection_perpendicular_to_slope();
    std::cout << "\n\n";
    
    test_various_projection_angles();
    
    printSeparator();
    std::cout << "所有抛体运动测试完成！" << std::endl;
    printSeparator();
    
    return 0;
}
