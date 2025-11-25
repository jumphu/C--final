#include "shapes.h"
#include <iostream>
#include <cmath>
#include <assert.h>

// 常量定义     
const double PI = 3.14159265358979323846;

/*=========================================================================================================
 * Shape类方法实现
 * 基类形状类
 *=========================================================================================================*/
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
    // 累加力到合力数组（不立即应用到速度）
    totalforce[0] += fx;
    totalforce[1] += fy;
}

void Shape::applyGravity(double g) {
    // 重力向下（y方向负），累加到合力
    totalforce[0] += 0.0;
    totalforce[1] += -g * mass;
}

void Shape::applyFriction(double normalForce, double frictionCoefficient) {
    // 计算摩擦力大小: f = μ * N
    double frictionMagnitude = frictionCoefficient * normalForce;
    
    // 获取当前速度（相对于静止表面，即绝对速度）
    double vx = velocity[0];
    double vy = velocity[1];
    
    // 计算速度的大小
    double speed = std::sqrt(vx * vx + vy * vy);
    
    // 如果速度几乎为零，不施加摩擦力（避免除零和抖动）
    if (speed < 1e-6) {
        return;
    }
    
    // 摩擦力方向与速度方向相反
    // 归一化速度向量得到单位方向向量
    double dirX = -vx / speed;  // 负号表示与速度相反
    double dirY = -vy / speed;
    
    // 计算摩擦力分量: F_friction = f * direction
    double frictionX = frictionMagnitude * dirX;
    double frictionY = frictionMagnitude * dirY;
    
    // 检查摩擦力是否会使速度反向（防止振荡）
    // 如果摩擦力太大，会导致速度在下一时间步反向，这是不物理的
    // 我们不在这里截断，而是在 applyTotalForce 中处理
    
    // 累加摩擦力到合力
    addToTotalForce(frictionX, frictionY);
}

void Shape::applyFrictionRelative(double normalForce, double frictionCoefficient, double otherVx, double otherVy) {
    // 计算摩擦力大小: f = μ * N
    double frictionMagnitude = frictionCoefficient * normalForce;
    
    // 获取当前物体的速度
    double vx = velocity[0];
    double vy = velocity[1];
    
    // 计算相对速度（当前物体相对于接触表面的速度）
    double relVx = vx - otherVx;
    double relVy = vy - otherVy;
    
    // 计算相对速度的大小
    double relSpeed = std::sqrt(relVx * relVx + relVy * relVy);
    
    // 如果相对速度几乎为零，不施加摩擦力（避免除零和抖动）
    if (relSpeed < 1e-6) {
        return;
    }
    
    // 摩擦力方向与相对速度方向相反
    // 归一化相对速度向量得到单位方向向量
    double dirX = -relVx / relSpeed;  // 负号表示与相对速度相反
    double dirY = -relVy / relSpeed;
    
    // 计算摩擦力分量: F_friction = f * direction
    double frictionX = frictionMagnitude * dirX;
    double frictionY = frictionMagnitude * dirY;
    
    // 累加摩擦力到合力
    addToTotalForce(frictionX, frictionY);
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

void Shape::getFraction(double& f) const {
    f = fraction;
}

void Shape::getRestitution(double& r) const {
    r = restitution;
}

void Shape::getTotalForce(double& fx, double& fy) const {
    fx = totalforce[0];
    fy = totalforce[1];
}

void Shape::getNormalForce(double& fx, double& fy) const {
    fx = normalforce[0];
    fy = normalforce[1];
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

void Shape::addToTotalForce(double fx, double fy) {
    totalforce[0] += fx;
    totalforce[1] += fy;
}

void Shape::clearTotalForce() {
    totalforce[0] = 0.0;
    totalforce[1] = 0.0;
}

void Shape::applyTotalForce(double deltaTime) {
    // F = ma => a = F/m => v += (F/m) * dt
    if (mass > 0.0) {
        // 计算加速度
        double ax = totalforce[0] / mass;
        double ay = totalforce[1] / mass;
        
        // 计算新速度
        double new_vx = velocity[0] + ax * deltaTime;
        double new_vy = velocity[1] + ay * deltaTime;
        
        // 检查速度是否反向（防止摩擦力过大导致的振荡）
        // 如果速度反向，说明摩擦力过大，应该直接停止
        if (velocity[0] * new_vx < 0) {  // X方向速度反向
            velocity[0] = 0.0;
        } else {
            velocity[0] = new_vx;
        }
        
        if (velocity[1] * new_vy < 0) {  // Y方向速度反向
            velocity[1] = 0.0;
        } else {
            velocity[1] = new_vy;
        }
    }
    // 应用后清空合力累加器
    clearTotalForce();
}

void Shape::applyNormalForce(double normalForce) {
    // 累加法向力到合力（垂直向上）
    addToTotalForce(0.0, normalForce);
}

void Shape::checkSupportStatus(const Shape& supporter_candidate) {
    // 检查当前物体是否被supporter_candidate支撑
    // 判定条件：
    // 1. 两物体发生碰撞
    // 2. 当前物体在supporter上方
    // 3. 垂直相对速度很小（接近静止或同步运动）
    
    if (!check_collision(supporter_candidate)) {
        return; // 没有碰撞，不可能被支撑
    }
    
    // 检查是否在上方（当前物体的Y坐标大于supporter）
    if (mass_centre[1] <= supporter_candidate.mass_centre[1]) {
        return; // 不在上方
    }
    
    // 获取垂直相对速度
    double relVy = velocity[1] - supporter_candidate.velocity[1];
    
    // 如果相对Y速度很小（接近0或向下速度很小），认为被支撑
    if (std::abs(relVy) < 0.5) {  // 阈值可调整
        isSupported = true;
    }
}

bool Shape::HasCollidedWithGround(double ground_y) const {
    // 使用底部位置检查是否与地面碰撞
    return getBottom() <= ground_y;
}

/*=========================================================================================================
 * DynamicShape类方法实现
 *=========================================================================================================*/

bool DynamicShape::isMoving() const {
    return (std::abs(velocity[0]) > 1e-6 || std::abs(velocity[1]) > 1e-6);
}

double DynamicShape::getSpeed() const {
    return std::sqrt(velocity[0] * velocity[0] + velocity[1] * velocity[1]);
}

double DynamicShape::getKineticEnergy() const {
    double speed = getSpeed();
    return 0.5 * mass * speed * speed;
}

void DynamicShape::stop() {
    velocity[0] = 0.0;
    velocity[1] = 0.0;
}

void DynamicShape::applyImpulse(double impulseX, double impulseY) {
    if (mass > 0.0) {
        velocity[0] += impulseX / mass;
        velocity[1] += impulseY / mass;
    }
}

void DynamicShape::limitSpeed(double maxSpeed) {
    double speed = getSpeed();
    if (speed > maxSpeed) {
        double ratio = maxSpeed / speed;
        velocity[0] *= ratio;
        velocity[1] *= ratio;
    }
}

void DynamicShape::getMomentum(double& px, double& py) const {
    px = mass * velocity[0];
    py = mass * velocity[1];
}

/*=========================================================================================================
 * StaticShape类方法实现
 *=========================================================================================================*/

void StaticShape::update(double deltaTime) {
    // 静态物体不移动，什么都不做
}

void StaticShape::move(double dx, double dy) {
    // 静态物体不能移动，什么都不做
}

void StaticShape::setVelocity(double vx, double vy) {
    velocity[0] = 0.0;
    velocity[1] = 0.0;
}

void StaticShape::setMass(double m) {
    mass = INFINITY;
}

void StaticShape::editorMove(double dx, double dy) {
    mass_centre[0] += dx;
    mass_centre[1] += dy;
}

void StaticShape::editorSetCentre(double x, double y) {
    mass_centre[0] = x;
    mass_centre[1] = y;
}

/*=========================================================================================================
 * Circle类方法实现
 * 圆形类，继承自DynamicShape
 *=========================================================================================================*/

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
    
    // 无法识别的类型，报错并返回false
    assert(false && "Unknown shape type for collision detection");
    return false;
}

double Circle::getRadius() const {
    return radius;
}

double Circle::getArea() const {
    return PI * radius * radius;
}

double Circle::getCircumference() const {
    return 2.0 * PI * radius;
}

/*=========================================================================================================
 * AABB类方法实现
 * 轴对齐包围盒类（矩形），继承自DynamicShape
 *=========================================================================================================*/

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
    
    // 无法识别的类型，报错
    assert(false && "Unknown shape type for collision detection");
    return false;
}

double AABB::getArea() const {
    return width * height;
}

double AABB::getPerimeter() const {
    return 2.0 * (width + height);
}

double AABB::getDiagonal() const {
    return std::sqrt(width * width + height * height);
}

Shape* AABB::getCompressedShapeDown() const {
    // TODO: 实现获取下方被压缩物体的逻辑
    // 这需要访问物理世界中的所有物体
    return nullptr;
}

Shape* AABB::getCompressedShapeUp() const {
    // TODO: 实现获取上方压缩物体的逻辑
    // 这需要访问物理世界中的所有物体
    return nullptr;
}

void AABB::applyFrictionUP() {
    // 获取上方被压缩的物体
    Shape* compressedShapeUp = getCompressedShapeUp();
    
    if (compressedShapeUp) {
        // 获取上方物体给当前AABB施加的弹力
        double fx, fy;
        compressedShapeUp->getNormalForce(fx, fy);
        
        // 正压力就是弹力的Y分量（向下）
        double normalForce = std::abs(fy);
        
        if (normalForce < 1e-6) {
            return; // 没有正压力，不施加摩擦力
        }
        
        // 获取上方物体速度
        double vx2, vy2;
        compressedShapeUp->getVelocity(vx2, vy2);
        
        // 使用相对速度摩擦力
        applyFrictionRelative(normalForce, fraction, vx2, vy2);
    }
}

void AABB::applyFrictionDOWN() {
    // 获取下方被压缩的物体
    Shape* compressedShapeDown = getCompressedShapeDown();
    
    if (compressedShapeDown) {
        // 获取上方被压缩的物体
        Shape* compressedShapeUp = getCompressedShapeUp();
        
        // 计算正压力：自身重力 + 上方所有物体的重力
        double normalForce = mass * 9.8;
        
        // 如果上方有物体，累加其质量产生的压力
        if (compressedShapeUp) {
            normalForce += compressedShapeUp->getMass() * 9.8;
        }
        
        // 保存施加到下方物体的法向力（用于下方物体计算摩擦力）
        normalforce[0] = 0.0;
        normalforce[1] = -normalForce;  // 向下
        
        // 使用下方物体的摩擦系数
        double otherFraction;
        compressedShapeDown->getFraction(otherFraction);
        
        // 获取下方物体的速度
        double vx2, vy2;
        compressedShapeDown->getVelocity(vx2, vy2);
        
        // 使用相对速度摩擦力
        applyFrictionRelative(normalForce, otherFraction, vx2, vy2);
    }
}

/*=========================================================================================================
 * Slope类方法实现
 * 斜坡类，继承自DynamicShape
 *=========================================================================================================*/

bool Slope::check_collision(const Shape& other) const {
    // Slope的碰撞检测 - 简化实现
    // 实际应用中需要更复杂的算法
    double dx = mass_centre[0] - other.mass_centre[0];
    double dy = mass_centre[1] - other.mass_centre[1];
    double distance = std::sqrt(dx * dx + dy * dy);
    
    return distance < length; // 简单的距离判断
}

double Slope::getAngleDegrees() const {
    return angle * 180.0 / PI;
}

double Slope::getHeight() const {
    return length * std::sin(angle);
}

double Slope::getBase() const {
    return length * std::cos(angle);
}

double Slope::getSlope() const {
    return std::tan(angle);
}

/*=========================================================================================================
 * Ground类方法实现
 *=========================================================================================================*/

bool Ground::check_collision(const Shape& other) const {
    // 使用统一的getBottom()方法
    return other.getBottom() <= y_level;
}

void Ground::getNormal(double& nx, double& ny) const {
    nx = 0.0;
    ny = 1.0;  // 地面法向量向上
}

void Ground::applyNormalForceToUpShape(Shape& shape) {
    // 计算支撑力（等于物体及其上方所有物体的总重力）
    // 这里需要获取shape的normalforce，它应该包含了上方物体施加的压力
    double fx, fy;
    shape.getNormalForce(fx, fy);
    
    // 总支撑力 = 物体自身重力 + 上方物体施加的压力
    double normalForce = shape.getMass() * 9.8 + std::abs(fy);
    
    // 施加向上的支撑力
    shape.applyNormalForce(normalForce);
}

void Ground::checkAndSetSupportStatus(Shape& shape) const {
    if (check_collision(shape)) {
        shape.setIsSupported(true);
    }
}

void Ground::setYLevel(double y) {
    y_level = y;
    mass_centre[1] = y;  // 同步更新质心Y坐标
}

