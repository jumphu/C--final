#include "shapes.h"
#include <iostream>
#include <cmath>
#include <cassert>
#include <algorithm>

const double PI = 3.14159265358979323846;

// ==================== Shape (基类) ====================
void Shape::move(double dx, double dy) { mass_centre[0] += dx; mass_centre[1] += dy; }
void Shape::turn(double angle) {
    double old_vx = velocity[0], old_vy = velocity[1];
    velocity[0] = old_vx * std::cos(angle) - old_vy * std::sin(angle);
    velocity[1] = old_vx * std::sin(angle) + old_vy * std::cos(angle);
}
void Shape::update(double deltaTime) { mass_centre[0] += velocity[0] * deltaTime; mass_centre[1] += velocity[1] * deltaTime; }
void Shape::applyForce(double fx, double fy) { totalforce[0] += fx; totalforce[1] += fy; }
void Shape::applyGravity(double g) { totalforce[1] -= g * mass; }

void Shape::applyFriction(double normalForce, double kineticFriction, double staticFriction, double drivingForce) {
    // 简化摩擦处理，交给 collision resolver
}

void Shape::applyFrictionRelative(double normalForce, double kineticFriction, double staticFriction, double otherVx, double otherVy, double drivingForce) {
    // 简化摩擦处理
}

// Getters & Setters
std::string Shape::getName() const { return name; }
std::string Shape::getType() const { return type; }
void Shape::setName(const std::string& n) { name = n; }
void Shape::setMass(double m) { mass = m; }
void Shape::setCentre(double x, double y) { mass_centre[0] = x; mass_centre[1] = y; }
void Shape::setVelocity(double vx, double vy) { velocity[0] = vx; velocity[1] = vy; }
void Shape::setFraction(double f) { fraction = f; }
void Shape::setStaticFraction(double sf) { static_fraction = sf; }
void Shape::setRestitution(double r) { restitution = r; }
void Shape::setIsSupported(bool supported) { isSupported = supported; }
void Shape::setSupporter(Shape* sup) { supporter = sup; }
double Shape::getMass() const { return mass; }
void Shape::getCentre(double& x, double& y) const { x = mass_centre[0]; y = mass_centre[1]; }
void Shape::getVelocity(double& vx, double& vy) const { vx = velocity[0]; vy = velocity[1]; }
void Shape::getFraction(double& f) const { f = fraction; }
void Shape::getStaticFraction(double& sf) const { sf = static_fraction; }
void Shape::getRestitution(double& r) const { r = restitution; }
void Shape::getTotalForce(double& fx, double& fy) const { fx = totalforce[0]; fy = totalforce[1]; }
void Shape::getNormalForce(double& fx, double& fy) const { fx = normalforce[0]; fy = normalforce[1]; }
bool Shape::getIsSupported() const { return isSupported; }
Shape* Shape::getSupporter() const { return supporter; }
void Shape::resetSupportStatus() { isSupported = false; supporter = nullptr; }

void Shape::addToTotalForce(double fx, double fy) { totalforce[0] += fx; totalforce[1] += fy; }
void Shape::clearTotalForce() { totalforce[0] = 0.0; totalforce[1] = 0.0; }

// 【关键修改】还原为纯粹的 F=ma，删除了之前强制归零 velocity 的逻辑
// 这解决了斜坡上无法加速的问题
void Shape::applyTotalForce(double deltaTime) {
    if (mass > 0.0) {
        // a = F / m
        double ax = totalforce[0] / mass;
        double ay = totalforce[1] / mass;
        
        // v = v + a * t
        velocity[0] += ax * deltaTime;
        velocity[1] += ay * deltaTime;
    }
    clearTotalForce();
}

void Shape::applyNormalForce(double normalForce) { addToTotalForce(0.0, normalForce); }

// 【关键修改】修改支撑检测，更严格
void Shape::checkSupportStatus(const Shape& supporter_candidate) {
    if (!check_collision(supporter_candidate)) return;
    // 必须严格在上方
    if (mass_centre[1] <= supporter_candidate.mass_centre[1]) return;
    
    // 只有垂直速度非常小才算支撑，防止运动中被粘住
    if (std::abs(velocity[1]) < 0.1 && std::abs(velocity[1] - supporter_candidate.velocity[1]) < 0.1) {
        isSupported = true;
        supporter = const_cast<Shape*>(&supporter_candidate);
    }
}
bool Shape::HasCollidedWithGround(double ground_y) const { return getBottom() <= ground_y; }

// ==================== DynamicShape ====================
bool DynamicShape::isMoving() const { return (std::abs(velocity[0]) > 1e-6 || std::abs(velocity[1]) > 1e-6); }
double DynamicShape::getSpeed() const { return std::sqrt(velocity[0] * velocity[0] + velocity[1] * velocity[1]); }
double DynamicShape::getKineticEnergy() const { double s = getSpeed(); return 0.5 * mass * s * s; }
void DynamicShape::stop() { velocity[0] = 0.0; velocity[1] = 0.0; }
void DynamicShape::applyImpulse(double impulseX, double impulseY) { if (mass > 0.0) { velocity[0] += impulseX / mass; velocity[1] += impulseY / mass; } }
void DynamicShape::limitSpeed(double maxSpeed) {
    double speed = getSpeed();
    if (speed > maxSpeed) {
        double ratio = maxSpeed / speed;
        velocity[0] *= ratio; velocity[1] *= ratio;
    }
}
void DynamicShape::getMomentum(double& px, double& py) const { px = mass * velocity[0]; py = mass * velocity[1]; }

// ==================== StaticShape ====================
void StaticShape::update(double deltaTime) {}
void StaticShape::move(double dx, double dy) {}
void StaticShape::setVelocity(double vx, double vy) { velocity[0] = 0.0; velocity[1] = 0.0; }
void StaticShape::setMass(double m) { mass = INFINITY; }
void StaticShape::editorMove(double dx, double dy) { mass_centre[0] += dx; mass_centre[1] += dy; }
void StaticShape::editorSetCentre(double x, double y) { mass_centre[0] = x; mass_centre[1] = y; }
bool StaticShape::isStatic() const { return true; }
bool StaticShape::isFixed() const { return true; }

// ==================== Circle ====================
double Circle::getRadius() const { return radius; }
double Circle::getArea() const { return PI * radius * radius; }
double Circle::getCircumference() const { return 2.0 * PI * radius; }
double Circle::getDiameter() const { return 2.0 * radius; }

bool Circle::check_collision(const Shape& other) const {
    const Circle* c = dynamic_cast<const Circle*>(&other);
    if (c) {
        double dx = mass_centre[0] - c->mass_centre[0];
        double dy = mass_centre[1] - c->mass_centre[1];
        return (dx*dx + dy*dy) <= (radius + c->radius)*(radius + c->radius);
    }
    const AABB* a = dynamic_cast<const AABB*>(&other);
    if (a) {
        double cx = std::max(a->getLeft(), std::min(mass_centre[0], a->getRight()));
        double cy = std::max(a->getBottom(), std::min(mass_centre[1], a->getTop()));
        double dx = mass_centre[0] - cx, dy = mass_centre[1] - cy;
        return (dx*dx + dy*dy) <= radius*radius;
    }
    const Wall* w = dynamic_cast<const Wall*>(&other);
    if (w) {
        double cx = std::max(w->getLeft(), std::min(mass_centre[0], w->getRight()));
        double cy = std::max(w->getBottom(), std::min(mass_centre[1], w->getTop()));
        double dx = mass_centre[0] - cx, dy = mass_centre[1] - cy;
        return (dx*dx + dy*dy) <= radius*radius;
    }
    const Slope* s = dynamic_cast<const Slope*>(&other);
    if (s) {
        // 斜坡检测逻辑
        double slopeX = s->mass_centre[0], slopeY = s->mass_centre[1];
        double slopeAngle = s->getAngle();
        double halfLen = s->getLength() / 2.0;
        
        // 转换到局部坐标系
        double dx = mass_centre[0] - slopeX;
        double dy = mass_centre[1] - slopeY;
        
        // 旋转
        double cosA = std::cos(-slopeAngle);
        double sinA = std::sin(-slopeAngle);
        double localX = dx * cosA - dy * sinA;
        double localY = dx * sinA + dy * cosA;
        
        // 线段检测
        double closestX = std::max(-halfLen, std::min(halfLen, localX));
        double closestY = 0.0;
        
        double distX = localX - closestX;
        double distY = localY - closestY;
        
        return (distX * distX + distY * distY) <= (radius * radius);
    }
    return false;
}

// ==================== AABB ====================
double AABB::getWidth() const { return width; }
double AABB::getHeight() const { return height; }
double AABB::getLeft() const { return mass_centre[0] - width / 2.0; }
double AABB::getRight() const { return mass_centre[0] + width / 2.0; }
double AABB::getTop() const { return mass_centre[1] + height / 2.0; }
double AABB::getNormalForceABS() const { return std::abs(normalforce[1]); }

bool AABB::check_collision(const Shape& other) const {
    const AABB* a = dynamic_cast<const AABB*>(&other);
    if (a) return !(getLeft() > a->getRight() || getRight() < a->getLeft() || getBottom() > a->getTop() || getTop() < a->getBottom());
    const Wall* w = dynamic_cast<const Wall*>(&other);
    if (w) return !(getLeft() > w->getRight() || getRight() < w->getLeft() || getBottom() > w->getTop() || getTop() < w->getBottom());
    const Circle* c = dynamic_cast<const Circle*>(&other);
    if (c) return c->check_collision(*this);
    // 忽略 AABB 与 斜坡的复杂碰撞
    return false;
}
double AABB::getArea() const { return width * height; }
double AABB::getPerimeter() const { return 2.0 * (width + height); }
double AABB::getDiagonal() const { return std::sqrt(width * width + height * height); }
Shape* AABB::getCompressedShapeDown() const { return nullptr; }
Shape* AABB::getCompressedShapeUp() const { return nullptr; }
void AABB::applyFrictionUP() {}
void AABB::applyFrictionDOWN() {}

// ==================== Slope ====================
double Slope::getLength() const { return length; }
double Slope::getAngle() const { return angle; }
double Slope::getAngleDegrees() const { return angle * 180.0 / PI; }
double Slope::getHeight() const { return length * std::sin(angle); }
double Slope::getBase() const { return length * std::cos(angle); }
double Slope::getSlope() const { return std::tan(angle); }
bool Slope::check_collision(const Shape& other) const { return false; }

// ==================== Ground ====================
bool Ground::check_collision(const Shape& other) const { return other.getBottom() <= y_level; }
void Ground::getNormal(double& nx, double& ny) const { nx = 0.0; ny = 1.0; }
void Ground::applyNormalForceToUpShape(Shape& shape) {
    double fx, fy; shape.getNormalForce(fx, fy);
    double normalForce = shape.getMass() * 9.8 + std::abs(fy);
    shape.applyNormalForce(normalForce);
}
void Ground::checkAndSetSupportStatus(Shape& shape) const { if (check_collision(shape)) shape.setIsSupported(true); }
void Ground::setYLevel(double y) { y_level = y; mass_centre[1] = y; }
double Ground::getYLevel() const { return y_level; }
double Ground::getFriction() const { return fraction; }
double Ground::getStaticFriction() const { return static_fraction; }
double Ground::getWidth() const { return INFINITY; }
void Ground::setFriction(double f) { fraction = f; static_fraction = f; }
void Ground::setFriction(double f, double sf) { fraction = f; static_fraction = sf; }
bool Ground::isPointOnGround(double x, double y) const { return y <= y_level; }

// ==================== Wall ====================
double Wall::getWidth() const { return width; }
double Wall::getHeight() const { return height; }
double Wall::getLeft() const { return mass_centre[0] - width / 2.0; }
double Wall::getRight() const { return mass_centre[0] + width / 2.0; }
double Wall::getArea() const { return width * height; }
double Wall::getPerimeter() const { return 2.0 * (width + height); }
double Wall::getDiagonal() const { return std::sqrt(width * width + height * height); }
bool Wall::containsPoint(double x, double y) const { return (x >= getLeft() && x <= getRight() && y >= getBottom() && y <= getTop()); }
double Wall::distanceToPoint(double x, double y) const {
    double dx = std::max(getLeft() - x, std::max(0.0, x - getRight()));
    double dy = std::max(getBottom() - y, std::max(0.0, y - getTop()));
    return std::sqrt(dx * dx + dy * dy);
}
bool Wall::check_collision(const Shape& other) const {
    const Circle* c = dynamic_cast<const Circle*>(&other);
    if (c) return c->check_collision(*this);
    const AABB* a = dynamic_cast<const AABB*>(&other);
    if (a) return a->check_collision(*this);
    const Wall* w = dynamic_cast<const Wall*>(&other);
    if (w) return !(getLeft() > w->getRight() || getRight() < w->getLeft() || getBottom() > w->getTop() || getTop() < w->getBottom());
    return false;
}