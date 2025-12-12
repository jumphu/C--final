#include "shapes.h"
#include <iostream>
#include <cmath>
#include <assert.h>
#include <string>

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

void Shape::applyFriction(double normalForce, double kineticFriction, double staticFriction, double drivingForce) {
    double vx = velocity[0];
    double speed = std::abs(vx);

    // 检查是否处于静止状态（使用更小的阈值）
    if (speed < 0.01) {  // 降低阈值从 0.1 到 0.01
        double maxStaticFriction = staticFriction * normalForce;
        if (std::abs(drivingForce) <= maxStaticFriction) {
            // 静摩擦力足以抵消驱动力
            addToTotalForce(-drivingForce, 0.0);
            // 同时将速度设为0，防止微小的累积
            velocity[0] = 0.0;
            return;
        }
    }

    // 应用动摩擦力
    double frictionMagnitude = kineticFriction * normalForce;
    if (speed < 1e-6) {
        return;
    }
    double dirX = -vx / speed;
    addToTotalForce(frictionMagnitude * dirX, 0.0);
}

void Shape::applyFrictionRelative(double normalForce, double kineticFriction, double staticFriction, double otherVx, double otherVy, double drivingForce) {
    double vx = velocity[0];
    double relVx = vx - otherVx;
    double relSpeed = std::abs(relVx);

    // 检查是否处于相对静止状态（使用更小的阈值）
    if (relSpeed < 0.01) {  // 降低阈值从 0.1 到 0.01
        double maxStaticFriction = staticFriction * normalForce;
        if (std::abs(drivingForce) <= maxStaticFriction) {
            // 静摩擦力足以抵消驱动力
            addToTotalForce(-drivingForce, 0.0);
            // 同时将相对速度设为0，防止微小的累积
            velocity[0] = otherVx;
            return;
        }
    }
    
    // 应用动摩擦力
    double frictionMagnitude = kineticFriction * normalForce;
    if (relSpeed < 1e-6) {
        return;
    }
    double dirX = -relVx / relSpeed;
    addToTotalForce(frictionMagnitude * dirX, 0.0);
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

// 暂时注释掉，调试编译问题
/*
void Shape::getStaticFriction(double& sf) const {
    sf = static_fraction;
}
*/

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
        
        // 计算速度变化
        double deltaVx = ax * deltaTime;
        double deltaVy = ay * deltaTime;
        
        // 计算新速度
        double new_vx = velocity[0] + deltaVx;
        double new_vy = velocity[1] + deltaVy;
        
        // 检查是否由于摩擦力导致速度反向
        // 只有当速度方向改变，且加速度与原速度方向相反时，才认为是摩擦力导致的过度减速
        // 这种情况下应该将速度设为0，而不是反向
        
        // X方向检查
        if (velocity[0] != 0.0 && new_vx * velocity[0] < 0) {
            // 速度反向了
            // 检查是否是摩擦力导致的（加速度与速度反向）
            if (ax * velocity[0] < 0) {
                // 加速度与速度反向，可能是摩擦力过大
                velocity[0] = 0.0;
            } else {
                // 加速度与速度同向或垂直，这是正常的物理行为
                velocity[0] = new_vx;
            }
        } else {
            velocity[0] = new_vx;
        }
        
        // Y方向检查
        if (velocity[1] != 0.0 && new_vy * velocity[1] < 0) {
            // 速度反向了
            if (ay * velocity[1] < 0) {
                // 加速度与速度反向，可能是摩擦力过大
                velocity[1] = 0.0;
            } else {
                velocity[1] = new_vy;
            }
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
        // ✅ 修复：设置支撑者指针
        supporter = const_cast<Shape*>(&supporter_candidate);
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
        // ✅ 修复：使用 <= 以包含相切情况
        return distance <= (radius + other_circle->radius);
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
        
        // ✅ 修复：使用 <= 以包含相切情况
        return distance <= radius;
    }
    
    // 尝试将other转换为Wall（静态矩形障害物）
    const Wall* other_wall = dynamic_cast<const Wall*>(&other);
    if (other_wall) {
        // Circle与Wall的碰撞检测
        double closest_x = std::max(other_wall->getLeft(),
                                   std::min(mass_centre[0], other_wall->getRight()));
        double closest_y = std::max(other_wall->getBottom(),
                                   std::min(mass_centre[1], other_wall->getTop()));
        
        double dx = mass_centre[0] - closest_x;
        double dy = mass_centre[1] - closest_y;
        double distance = std::sqrt(dx * dx + dy * dy);
        
        // 更精确：包含相切
        return distance <= radius;
    }
    
    // 尝试将other转换为Slope（静态斜坡）

const Slope* other_slope = dynamic_cast<const Slope*>(&other);
    if (other_slope) {
        // Circle与Slope的碰撞检测（简化：使用距离判断）
        double dx = mass_centre[0] - other_slope->mass_centre[0];
        double dy = mass_centre[1] - other_slope->mass_centre[1];
        double distance = std::sqrt(dx * dx + dy * dy);
        
        // 如果圆心到斜坡质心的距离小于半径+斜坡长度，可能碰撞
        return distance < (radius + other_slope->getLength() / 2.0);
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
    
    // 尝试将other转换为Wall（静态矩形障害物）
    const Wall* other_wall = dynamic_cast<const Wall*>(&other);
    if (other_wall) {
        // AABB与Wall的碰撞检测
        double left1 = mass_centre[0] - width/2;
        double right1 = mass_centre[0] + width/2;
        double top1 = mass_centre[1] + height/2;
        double bottom1 = mass_centre[1] - height/2;
        
        double left2 = other_wall->getLeft();
        double right2 = other_wall->getRight();
        double top2 = other_wall->getTop();
        double bottom2 = other_wall->getBottom();
        
        return !(left1 > right2 || right1 < left2 || bottom1 > top2 || top1 < bottom2);
    }
    
    // 尝试将other转换为Circle
    const Circle* other_circle = dynamic_cast<const Circle*>(&other);
    if (other_circle) {
        // AABB与Circle的碰撞检测（复用Circle的方法）
        return other_circle->check_collision(*this);
    }
    
    // 尝试将other转换为Slope（静态斜坡）
    const Slope* other_slope = dynamic_cast<const Slope*>(&other);
    if (other_slope) {
        // AABB与Slope的碰撞检测（简化版本）
        double dx = mass_centre[0] - other_slope->mass_centre[0];
        double dy = mass_centre[1] - other_slope->mass_centre[1];
        double distance = std::sqrt(dx * dx + dy * dy);
        
        // 简化判定：如果距离小于AABB对角线的一半 + 斜坡长度的一半
        double aabbRadius = std::sqrt(width * width + height * height) / 2.0;
        return distance < (aabbRadius + other_slope->getLength() / 2.0);
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
    // 此函数逻辑复杂且当前未使用，暂时注释以解决编译问题
    /*
    Shape* compressedShape = getCompressedShapeUp();
    if (compressedShape) {
        double normalForce = compressedShape->getNormalForceABS();
        double fraction = compressedShape->fraction;
        double vx2, vy2;
        compressedShape->getVelocity(vx2, vy2);
        applyFrictionRelative(normalForce, fraction, static_fraction, vx2, vy2, totalforce[0]);
    }
    */
}

void AABB::applyFrictionDOWN() {
    // 此函数逻辑复杂且当前未使用，暂时注释以解决编译问题
    /*
	Shape* compressedShape = getCompressedShapeDown();
	if (compressedShape) {
		double normalForce = getNormalForceABS();
		double otherFraction = compressedShape->fraction;
		double vx2, vy2;
		compressedShape->getVelocity(vx2, vy2);
		applyFrictionRelative(normalForce, otherFraction, static_fraction, vx2, vy2, totalforce[0]);
	}
    */
}

/*=========================================================================================================
 * Slope类方法实现
 * 斜坡类，继承自StaticShape（静态物体）
 *=========================================================================================================*/

bool Slope::check_collision(const Shape& other) const {
    // Slope 不主动检测与动态物体的碰撞
    return false;
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

double Wall::getArea() const {
    return width * height;
}

double Wall::getPerimeter() const {
    return 2.0 * (width + height);
}

double Wall::getDiagonal() const {
    return std::sqrt(width * width + height * height);
}

bool Wall::containsPoint(double x, double y) const {
    return (x >= getLeft() && x <= getRight() && 
            y >= getBottom() && y <= getTop());
}

double Wall::distanceToPoint(double x, double y) const {
    double dx = std::max(getLeft() - x, std::max(0.0, x - getRight()));
    double dy = std::max(getBottom() - y, std::max(0.0, y - getTop()));
    return std::sqrt(dx * dx + dy * dy);
}

/*=========================================================================================================
 * Wall::check_collision - 检测墙壁与其他形状的碰撞
 * 
 * 墙壁是静态矩形，需要与各种动态形状进行碰撞检测
 *=========================================================================================================*/
bool Wall::check_collision(const Shape& other) const {
    // 尝试将other转换为Circle
    const Circle* other_circle = dynamic_cast<const Circle*>(&other);
    if (other_circle) {
        // 墙壁（矩形）与圆形的碰撞检测
        // 找到墙壁上离圆心最近的点
        double circleX, circleY;
        other_circle->getCentre(circleX, circleY);
        
        double closestX = std::max(getLeft(), std::min(circleX, getRight()));
        double closestY = std::max(getBottom(), std::min(circleY, getTop()));
        
        double dx = circleX - closestX;
        double dy = circleY - closestY;
        double distance = std::sqrt(dx * dx + dy * dy);
        
        return distance <= other_circle->getRadius();
    }
    
    // 尝试将other转换为AABB
    const AABB* other_aabb = dynamic_cast<const AABB*>(&other);
    if (other_aabb) {
        // 墙壁（矩形）与AABB（矩形）的碰撞检测
        double left1 = getLeft();
        double right1 = getRight();
        double top1 = getTop();
        double bottom1 = getBottom();
        
        double left2 = other_aabb->getLeft();
        double right2 = other_aabb->getRight();
        double top2 = other_aabb->getTop();
        double bottom2 = other_aabb->getBottom();
        
        return !(left1 > right2 || right1 < left2 || bottom1 > top2 || top1 < bottom2);
    }
    
    // 尝试将other转换为Wall
    const Wall* other_wall = dynamic_cast<const Wall*>(&other);
    if (other_wall) {
        // 墙壁与墙壁的碰撞检测（矩形与矩形）
        double left1 = getLeft();
        double right1 = getRight();
        double top1 = getTop();
        double bottom1 = getBottom();
        
        double left2 = other_wall->getLeft();
        double right2 = other_wall->getRight();
        double top2 = other_wall->getTop();
        double bottom2 = other_wall->getBottom();
        
        return !(left1 > right2 || right1 < left2 || bottom1 > top2 || top1 < bottom2);
    }
    
    // 尝试将other转换为Slope（静态斜坡）
    const Slope* other_slope = dynamic_cast<const Slope*>(&other);
    if (other_slope) {
        // 墙壁与斜坡的碰撞检测（简化版本）
        double dx = mass_centre[0] - other_slope->mass_centre[0];
        double dy = mass_centre[1] - other_slope->mass_centre[1];
        double distance = std::sqrt(dx * dx + dy * dy);
        
        // 简化判定：如果距离小于墙壁对角线的一半 + 斜坡长度的一半
        double wallRadius = getDiagonal() / 2.0;
        return distance < (wallRadius + other_slope->getLength() / 2.0);
    }
    
    // 无法识别的类型，返回false
    return false;
}

