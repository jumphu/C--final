#include "shapes.h"
#include <iostream>
#include <cmath>

// 常量定义
const double PI = 3.14159265358979323846;

// Shape类方法实现
void Shape::move(double dx, double dy) {
    mass_centre[0] += dx;
    mass_centre[1] += dy;
}

void Shape::turn(double angle) {
    // 旋转速度向量
    double old_vx = velocity[0];
    double old_vy = velocity[1];
    
    velocity[0] = old_vx * std::cos(angle) - old_vy * std::sin(angle);
    velocity[1] = old_vx * std::sin(angle) + old_vy * std::cos(angle);
}

// 物理更新方法
void Shape::update(double deltaTime) {
    // 根据速度更新位置: position += velocity * deltaTime
    mass_centre[0] += velocity[0] * deltaTime;
    mass_centre[1] += velocity[1] * deltaTime;
}

void Shape::applyForce(double fx, double fy) {
    // F = ma => a = F/m => v += (F/m) * dt
    // 这里简化为直接改变速度（假设dt=1）
    if (mass > 0.0) {
        velocity[0] += fx / mass;
        velocity[1] += fy / mass;
    }
}

void Shape::applyGravity(double g) {
    // 重力向下（y方向负）
    applyForce(0.0, -g * mass);
}

double Shape::getMass() const {
    return mass;
}

void Shape::getCentre(double& x, double& y) const {
    x = mass_centre[0];
    y = mass_centre[1];
}

void Shape::getVelocity(double& vx, double& vy) const {
    vx = velocity[0];
    vy = velocity[1];
}

void Shape::setMass(double m) {
    mass = m;
}

void Shape::setCentre(double x, double y) {
    mass_centre[0] = x;
    mass_centre[1] = y;
}

void Shape::setVelocity(double vx, double vy) {
    velocity[0] = vx;
    velocity[1] = vy;
}

// Circle类方法实现
bool Circle::check_collision(const Shape& other) const {
    // 尝试将other转换为Circle
    const Circle* other_circle = dynamic_cast<const Circle*>(&other);
    if (other_circle) {
        // Circle与Circle的碰撞检测
        double dx = mass_centre[0] - other_circle->mass_centre[0];
        double dy = mass_centre[1] - other_circle->mass_centre[1];
        double distance = std::sqrt(dx * dx + dy * dy);
        return distance < (radius + other_circle->radius);
    }
    
    // 尝试将other转换为AABB
    const AABB* other_aabb = dynamic_cast<const AABB*>(&other);
    if (other_aabb) {
        // Circle与AABB的碰撞检测
        double closest_x = std::max(other_aabb->mass_centre[0] - other_aabb->width/2,
                                   std::min(mass_centre[0], other_aabb->mass_centre[0] + other_aabb->width/2));
        double closest_y = std::max(other_aabb->mass_centre[1] - other_aabb->height/2,
                                   std::min(mass_centre[1], other_aabb->mass_centre[1] + other_aabb->height/2));
        
        double dx = mass_centre[0] - closest_x;
        double dy = mass_centre[1] - closest_y;
        double distance = std::sqrt(dx * dx + dy * dy);
        
        return distance < radius;
    }
    
    // 无法识别的类型，返回false（不碰撞）
    return false;
}

double Circle::getRadius() const {
    return radius;
}

// AABB类方法实现
bool AABB::check_collision(const Shape& other) const {
    // 尝试将other转换为AABB
    const AABB* other_aabb = dynamic_cast<const AABB*>(&other);
    if (other_aabb) {
        // AABB与AABB的碰撞检测
        double left1 = mass_centre[0] - width/2;
        double right1 = mass_centre[0] + width/2;
        double top1 = mass_centre[1] + height/2;
        double bottom1 = mass_centre[1] - height/2;
        
        double left2 = other_aabb->mass_centre[0] - other_aabb->width/2;
        double right2 = other_aabb->mass_centre[0] + other_aabb->width/2;
        double top2 = other_aabb->mass_centre[1] + other_aabb->height/2;
        double bottom2 = other_aabb->mass_centre[1] - other_aabb->height/2;
        
        return !(left1 > right2 || right1 < left2 || bottom1 > top2 || top1 < bottom2);
    }
    
    // 尝试将other转换为Circle
    const Circle* other_circle = dynamic_cast<const Circle*>(&other);
    if (other_circle) {
        // AABB与Circle的碰撞检测（复用Circle的方法）
        return other_circle->check_collision(*this);
    }
    
    // 无法识别的类型，返回false（不碰撞）
    return false;
}

// Slope类方法实现
bool Slope::check_collision(const Shape& other) const {
    // Slope的碰撞检测 - 简化实现
    // 实际应用中需要更复杂的算法
    double dx = mass_centre[0] - other.mass_centre[0];
    double dy = mass_centre[1] - other.mass_centre[1];
    double distance = std::sqrt(dx * dx + dy * dy);
    
    return distance < length; // 简单的距离判断
}