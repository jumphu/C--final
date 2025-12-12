#include "shapes.h"
#include <iostream>
#include <cmath>
#include <cassert>
#include <algorithm>

const double PI = 3.14159265358979323846;

// ==================== Shape ====================
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
    double vx = velocity[0];
    double speed = std::abs(vx);
    if (speed < 0.01) {
        double maxStaticFriction = staticFriction * normalForce;
        if (std::abs(drivingForce) <= maxStaticFriction) {
            addToTotalForce(-drivingForce, 0.0);
            velocity[0] = 0.0;
            return;
        }
    }
    double frictionMagnitude = kineticFriction * normalForce;
    if (speed < 1e-6) return;
    double dirX = -vx / speed;
    addToTotalForce(frictionMagnitude * dirX, 0.0);
}

void Shape::applyFrictionRelative(double normalForce, double kineticFriction, double staticFriction, double otherVx, double otherVy, double drivingForce) {
    double vx = velocity[0];
    double relVx = vx - otherVx;
    double relSpeed = std::abs(relVx);
    if (relSpeed < 0.01) {
        double maxStaticFriction = staticFriction * normalForce;
        if (std::abs(drivingForce) <= maxStaticFriction) {
            addToTotalForce(-drivingForce, 0.0);
            velocity[0] = otherVx;
            return;
        }
    }
    double frictionMagnitude = kineticFriction * normalForce;
    if (relSpeed < 1e-6) return;
    double dirX = -relVx / relSpeed;
    addToTotalForce(frictionMagnitude * dirX, 0.0);
}

double Shape::getMass() const { return mass; }
void Shape::getCentre(double& x, double& y) const { x = mass_centre[0]; y = mass_centre[1]; }
void Shape::getVelocity(double& vx, double& vy) const { vx = velocity[0]; vy = velocity[1]; }
void Shape::getFraction(double& f) const { f = fraction; }
void Shape::getRestitution(double& r) const { r = restitution; }
void Shape::getTotalForce(double& fx, double& fy) const { fx = totalforce[0]; fy = totalforce[1]; }
void Shape::getNormalForce(double& fx, double& fy) const { fx = normalforce[0]; fy = normalforce[1]; }
void Shape::setMass(double m) { mass = m; }
void Shape::setCentre(double x, double y) { mass_centre[0] = x; mass_centre[1] = y; }
void Shape::setVelocity(double vx, double vy) { velocity[0] = vx; velocity[1] = vy; }
void Shape::addToTotalForce(double fx, double fy) { totalforce[0] += fx; totalforce[1] += fy; }
void Shape::clearTotalForce() { totalforce[0] = 0.0; totalforce[1] = 0.0; }

void Shape::applyTotalForce(double deltaTime) {
    if (mass > 0.0) {
        double ax = totalforce[0] / mass;
        double ay = totalforce[1] / mass;
        double new_vx = velocity[0] + ax * deltaTime;
        double new_vy = velocity[1] + ay * deltaTime;
        
        if (velocity[0] != 0.0 && new_vx * velocity[0] < 0 && ax * velocity[0] < 0) velocity[0] = 0.0;
        else velocity[0] = new_vx;
        
        if (velocity[1] != 0.0 && new_vy * velocity[1] < 0 && ay * velocity[1] < 0) velocity[1] = 0.0;
        else velocity[1] = new_vy;
    }
    clearTotalForce();
}
void Shape::applyNormalForce(double normalForce) { addToTotalForce(0.0, normalForce); }

void Shape::checkSupportStatus(const Shape& supporter_candidate) {
    if (!check_collision(supporter_candidate)) return;
    if (mass_centre[1] <= supporter_candidate.mass_centre[1]) return;
    double relVy = velocity[1] - supporter_candidate.velocity[1];
    if (std::abs(relVy) < 0.5) {
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

// ==================== Circle ====================
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
        double dx = mass_centre[0] - s->mass_centre[0];
        double dy = mass_centre[1] - s->mass_centre[1];
        return std::sqrt(dx*dx + dy*dy) < (radius + s->getLength()/2.0);
    }
    return false;
}
double Circle::getRadius() const { return radius; }
double Circle::getArea() const { return PI * radius * radius; }
double Circle::getCircumference() const { return 2.0 * PI * radius; }

// ==================== AABB ====================
bool AABB::check_collision(const Shape& other) const {
    const AABB* a = dynamic_cast<const AABB*>(&other);
    if (a) {
        return !(getLeft() > a->getRight() || getRight() < a->getLeft() || getBottom() > a->getTop() || getTop() < a->getBottom());
    }
    const Wall* w = dynamic_cast<const Wall*>(&other);
    if (w) {
        return !(getLeft() > w->getRight() || getRight() < w->getLeft() || getBottom() > w->getTop() || getTop() < w->getBottom());
    }
    const Circle* c = dynamic_cast<const Circle*>(&other);
    if (c) return c->check_collision(*this);
    
    const Slope* s = dynamic_cast<const Slope*>(&other);
    if (s) {
        double dx = mass_centre[0] - s->mass_centre[0];
        double dy = mass_centre[1] - s->mass_centre[1];
        double diag = std::sqrt(width*width + height*height)/2.0;
        return std::sqrt(dx*dx + dy*dy) < (diag + s->getLength()/2.0);
    }
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
bool Slope::check_collision(const Shape& other) const { return false; }
double Slope::getAngleDegrees() const { return angle * 180.0 / PI; }
double Slope::getHeight() const { return length * std::sin(angle); }
double Slope::getBase() const { return length * std::cos(angle); }
double Slope::getSlope() const { return std::tan(angle); }

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

// ==================== Wall ====================
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