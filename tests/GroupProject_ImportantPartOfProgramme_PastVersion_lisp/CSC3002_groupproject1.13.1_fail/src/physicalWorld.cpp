#include "physicalWorld.h"
#include "shapes.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>

//放置物体类的实现
void PhysicalWorld::placeDynamicShape(Shape& shape, double x_pos, double y_pos) {
	double f = ground.getFriction();
	shape.fraction = f;
	const Circle* circle = dynamic_cast<const Circle*>(&shape);
	if (circle) { shape.setCentre(x_pos, y_pos + circle->getRadius()); return; }
	const AABB* aabb = dynamic_cast<const AABB*>(&shape);
	if (aabb) { shape.setCentre(x_pos, y_pos + aabb->getHeight() / 2); return; }
}

void PhysicalWorld::placeStaticShape(Shape& shape, double x_pos, double y_pos) {
	shape.setCentre(x_pos, y_pos);
}

void PhysicalWorld::placeShapeOnGround(Shape& shape, const Ground& ground) {
	double x, y; shape.getCentre(x, y);
	double f = ground.getFriction(); shape.fraction = f;
	const Circle* circle = dynamic_cast<const Circle*>(&shape);
	if (circle) { shape.setCentre(x, ground.getYLevel() + circle->getRadius()); return; }
	const AABB* aabb = dynamic_cast<const AABB*>(&shape);
	if (aabb) { shape.setCentre(x, ground.getYLevel() + aabb->getHeight() / 2); return; }
}

void PhysicalWorld::placeShapeOnShape(Shape& topShape, Shape& bottomShape, double offsetX) {
	double bx, by; bottomShape.getCentre(bx, by);
	double tx, ty; topShape.getCentre(tx, ty);
	topShape.setCentre(bx + offsetX, bottomShape.getTop() + (ty - topShape.getBottom()));
}

void PhysicalWorld::start() { isPaused = false; }
void PhysicalWorld::Pause() { if (!isPaused) isPaused = true; }
void PhysicalWorld::Continue() { if (isPaused) isPaused = false; }
void PhysicalWorld::Stop() { isPaused = false; savedStates.clear(); }

void PhysicalWorld::saveStates() {}
void PhysicalWorld::restoreStates() {}

void PhysicalWorld::update(std::vector<Shape*>& shapeList, const Ground& ground) {
	if (isPaused) return;
	update(shapeList, timeStep, ground);
}

void PhysicalWorld::update(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground) {
	if (isPaused) return;
	resetSupportStates(shapeList);
	detectSupportRelations(shapeList, ground);
	calculateNormalForces(shapeList);
	updatePhysics(shapeList, deltaTime, ground);
	handleAllCollisions(shapeList);
}

void PhysicalWorld::resetSupportStates(std::vector<Shape*>& shapeList) {
	for (auto& shape : shapeList) shape->resetSupportStatus();
}

// 【关键手术 1】绝不让斜坡触发“支撑状态”
void PhysicalWorld::detectSupportRelations(std::vector<Shape*>& shapeList, const Ground& ground) {
	for (auto& shape : shapeList) {
		// 1. 检查地面
		if (shape->HasCollidedWithGround(ground.getYLevel())) {
			shape->setIsSupported(true);
			shape->setSupporter(nullptr);
		}
		
		// 2. 检查静态物体 (必须排除斜坡！)
		for (size_t k = 0; k < staticShapeList.size(); k++) {
			Shape* staticShape = staticShapeList[k];
			
			// 如果是斜坡，直接跳过！绝不让它成为 Supporter
			if (dynamic_cast<Slope*>(staticShape)) continue;
			
			// 只有墙壁或平台才能提供支撑
			shape->checkSupportStatus(*staticShape);
		}
		
		// 3. 检查动态物体
		for (auto& other : shapeList) {
			if (shape != other) shape->checkSupportStatus(*other);
		}
	}
}

void PhysicalWorld::calculateNormalForces(std::vector<Shape*>& shapeList) {
	for (auto& shape : shapeList) { shape->normalforce[0] = 0.0; shape->normalforce[1] = 0.0; }
	for (auto& shape : shapeList) calculateNormalForceRecursive(shape, shapeList);
}

void PhysicalWorld::calculateNormalForceRecursive(Shape* shape, std::vector<Shape*>& shapeList) {
	if (shape->normalforce[1] != 0.0) return;
	double totalWeight = shape->getMass() * gravity;
	for (auto& other : shapeList) {
		if (other != shape && other->getSupporter() == shape) {
			calculateNormalForceRecursive(other, shapeList);
			totalWeight += std::abs(other->normalforce[1]);
		}
	}
	shape->normalforce[1] = -totalWeight;
}

void PhysicalWorld::updatePhysics(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground) {
	for (auto& shape : shapeList) {
		shape->clearTotalForce();
		// 如果被支撑（如地面），走支撑逻辑（垂直速度归零）
		// 如果在斜坡上（isSupported=false），走空中逻辑（受重力加速）
		if (shape->getIsSupported()) {
			handleSupportedShape(shape, deltaTime, ground);
		} else {
			handleAirborneShape(shape, deltaTime);
		}
		shape->applyTotalForce(deltaTime);
		shape->update(deltaTime);
		handleBoundaryCollision(*shape);
	}
}

void PhysicalWorld::handleSupportedShape(Shape* shape, double deltaTime, const Ground& ground) {
	double x, y, vx, vy;
	shape->getCentre(x, y);
	shape->getVelocity(vx, vy);
	
	// 在地面或平台上，Y速度清零
	if (vy < -0.1) {
        double r; shape->getRestitution(r);
        vy = -vy * r; 
    } else {
        vy = 0.0;
    }
	shape->setVelocity(vx, vy);
	
	// 地面摩擦
	double friction = 0.1;
	if (std::abs(vx) > 0.001) {
	    double frictionForce = shape->getMass() * 9.8 * friction;
	    double dir = (vx > 0) ? -1.0 : 1.0;
	    shape->addToTotalForce(frictionForce * dir, 0.0);
	}
}

void PhysicalWorld::handleAirborneShape(Shape* shape, double deltaTime) {
	shape->applyGravity(gravity);
}

void PhysicalWorld::applyFrictionOnSupporter(Shape* shape, Shape* supporter, double normalForce, double friction, double static_friction, double drivingForce) {}

void PhysicalWorld::handleAllCollisions(std::vector<Shape*>& shapeList) {
	const double MAX_INTERACTION_DISTANCE = 300.0;
	
    // 动态 vs 动态
	for (size_t i = 0; i < shapeList.size(); i++) {
		for (size_t j = i + 1; j < shapeList.size(); j++) {
			double x1, y1, x2, y2;
			shapeList[i]->getCentre(x1, y1);
			shapeList[j]->getCentre(x2, y2);
			double dx = x2 - x1, dy = y2 - y1;
			if (dx*dx + dy*dy > MAX_INTERACTION_DISTANCE * MAX_INTERACTION_DISTANCE) continue; 
			
			if (shapeList[i]->check_collision(*shapeList[j])) {
				resolveCollision(*shapeList[i], *shapeList[j]);
			}
		}
	}

    // 动态 vs 静态
    for (size_t i = 0; i < shapeList.size(); i++) {
        for (size_t k = 0; k < staticShapeList.size(); k++) {
            Shape* dynamicShape = shapeList[i];
            Shape* staticShape = staticShapeList[k];

            if (dynamicShape->check_collision(*staticShape)) {
                Wall* wall = dynamic_cast<Wall*>(staticShape);
                Slope* slope = dynamic_cast<Slope*>(staticShape);
                
                if (wall) resolveCollisionWithWall(*dynamicShape, *wall);
                else if (slope) resolveCollisionWithSlope(*dynamicShape, *slope);
                else resolveCollision(*dynamicShape, *staticShape);
            }
        }
    }
}

void PhysicalWorld::Collisions(Shape& shape1, Shape& shape2) { resolveCollision(shape1, shape2); }

void PhysicalWorld::resolveCollision(Shape& shape1, Shape& shape2) {
	double m1 = shape1.getMass(); double m2 = shape2.getMass();
	double v1x, v1y, v2x, v2y; shape1.getVelocity(v1x, v1y); shape2.getVelocity(v2x, v2y);
	double x1, y1, x2, y2; shape1.getCentre(x1, y1); shape2.getCentre(x2, y2);
	
	double nx = x2 - x1, ny = y2 - y1;
	double dist = std::sqrt(nx*nx + ny*ny);
	if (dist < 0.0001) return;
	nx /= dist; ny /= dist;
	
    double v1n = v1x * nx + v1y * ny;
    double v2n = v2x * nx + v2y * ny;
    if (v2n - v1n > 0) return;
    
    double totalM = m1 + m2;
    double v1n_new = ((m1 - m2) * v1n + 2 * m2 * v2n) / totalM;
    double v2n_new = ((m2 - m1) * v2n + 2 * m1 * v1n) / totalM;
    
    double tx = -ny, ty = nx;
    double v1t = v1x * tx + v1y * ty;
    double v2t = v2x * tx + v2y * ty;
    
    shape1.setVelocity(v1n_new * nx + v1t * tx, v1n_new * ny + v1t * ty);
    shape2.setVelocity(v2n_new * nx + v2t * tx, v2n_new * ny + v2t * ty);
    
    separateOverlappingShapes(shape1, shape2, nx, ny, dist);
}

void PhysicalWorld::separateOverlappingShapes(Shape& shape1, Shape& shape2, double nx, double ny, double distance) {
	const Circle* c1 = dynamic_cast<const Circle*>(&shape1);
	const Circle* c2 = dynamic_cast<const Circle*>(&shape2);
	double overlap = 0;
	if (c1 && c2) overlap = c1->getRadius() + c2->getRadius() - distance;
	
	if (overlap > 0) {
	    double m1 = shape1.getMass();
	    double m2 = shape2.getMass();
	    double totalM = m1 + m2;
	    shape1.move(-nx * overlap * (m2/totalM), -ny * overlap * (m2/totalM));
	    shape2.move(nx * overlap * (m1/totalM), ny * overlap * (m1/totalM));
	}
}

void PhysicalWorld::handleBoundaryCollision(Shape& shape) { /* 留空，交给Wall */ }
bool PhysicalWorld::isInBounds(const Shape& shape) const { return true; }
bool PhysicalWorld::checkBoundaryCollision(const Shape& shape) const { return false; }

void PhysicalWorld::addDynamicShape(Shape* shape) { if (shape) dynamicShapeList.push_back(shape); }
void PhysicalWorld::addStaticShape(Shape* shape) { if (shape) staticShapeList.push_back(shape); }
void PhysicalWorld::removeDynamicShape(Shape* shape) {}
void PhysicalWorld::removeStaticShape(Shape* shape) {}

Shape* PhysicalWorld::findShapeByName(const std::string& name) { return nullptr; }
Shape* PhysicalWorld::findDynamicShapeByName(const std::string& name) { return nullptr; }
Shape* PhysicalWorld::findStaticShapeByName(const std::string& name) { return nullptr; }

std::vector<Shape*> PhysicalWorld::findShapesByType(const std::string& type) { return {}; }
std::vector<Shape*> PhysicalWorld::findDynamicShapesByType(const std::string& type) { return {}; }
std::vector<Shape*> PhysicalWorld::findStaticShapesByType(const std::string& type) { return {}; }

void PhysicalWorld::clearDynamicShapes() { dynamicShapeList.clear(); }
void PhysicalWorld::clearStaticShapes() { staticShapeList.clear(); }
void PhysicalWorld::clearAllShapes() { clearDynamicShapes(); clearStaticShapes(); }

void PhysicalWorld::printAllShapes() const {}
void PhysicalWorld::printDynamicShapes() const {}
void PhysicalWorld::printStaticShapes() const {}

std::string PhysicalWorld::parseShapeType(const std::string& type) const { return ""; }
Shape* PhysicalWorld::createShape(const std::string& type, const std::string& name, double mass, double size1, double size2, bool isDynamic) { return nullptr; }
std::string PhysicalWorld::generateUniqueName(const std::string& type) { return ""; }
bool PhysicalWorld::isNameExists(const std::string& name) const { return false; }

Shape* PhysicalWorld::placeDynamicShapeByType(const std::string& type, const std::string& name, double x, double y, double m, double s1, double s2) { return nullptr; }
Shape* PhysicalWorld::placeStaticShapeByType(const std::string& type, const std::string& name, double x, double y, double m, double s1, double s2) { return nullptr; }
Wall* PhysicalWorld::placeWall(const std::string& name, double x, double y, double w, double h, double f) { return nullptr; }

void PhysicalWorld::handleWallCollision(Shape& dynamicShape, const Wall& wall) {
    if (dynamicShape.check_collision(wall)) resolveCollisionWithWall(dynamicShape, wall);
}

void PhysicalWorld::resolveCollisionWithWall(Shape& dynamicShape, const Wall& wall) {
    double shapeX, shapeY, vx, vy;
    dynamicShape.getCentre(shapeX, shapeY);
    dynamicShape.getVelocity(vx, vy);
    
    // 简单的墙壁反弹：如果进入墙内部，反转速度
    if (shapeX < wall.getRight() && shapeX > wall.getLeft()) {
        vx = -vx * 0.8; 
        // 推出来
        if (shapeX < wall.mass_centre[0]) shapeX = wall.getLeft() - 1.0;
        else shapeX = wall.getRight() + 1.0;
        dynamicShape.setCentre(shapeX, shapeY);
        dynamicShape.setVelocity(vx, vy);
    }
}

void PhysicalWorld::applyGravitySin(Shape* shape, double inclineAngle) {}
std::vector<double> PhysicalWorld::inclineToStandard(double x_rel, double y_rel) const { return {0,0}; }

/*=========================================================================================================
 * 【关键手术 2】完全重写的斜坡碰撞逻辑
 * 采用 速度分解 (Projection) 而不是直接修改坐标
 *=========================================================================================================*/
void PhysicalWorld::resolveCollisionWithSlope(Shape& dynamicShape, const Slope& slope) {
    // 1. 获取状态
    double shapeX, shapeY, vx, vy;
    dynamicShape.getCentre(shapeX, shapeY);
    dynamicShape.getVelocity(vx, vy);
    
    double slopeX, slopeY;
    slope.getCentre(slopeX, slopeY);
    double angle = slope.getAngle();
    
    // 2. 计算斜坡法向量 (垂直向上/向外)
    // 假设 angle 是斜坡相对于水平面的角度 (radians)
    // 法向量 N = (-sin a, cos a)
    double nx = -std::sin(angle);
    double ny = std::cos(angle);
    
    // 切向量 T = (cos a, sin a)
    double tx = std::cos(angle);
    double ty = std::sin(angle);
    
    // 3. 计算相对位置，确保法线指向球
    double dx = shapeX - slopeX;
    double dy = shapeY - slopeY;
    
    // 如果法线反了，翻转它
    if (dx * nx + dy * ny < 0) {
        nx = -nx; ny = -ny;
    }
    
    // 4. 计算穿透深度 (Distance to Plane)
    double distance = dx * nx + dy * ny;
    const Circle* circle = dynamic_cast<const Circle*>(&dynamicShape);
    double radius = circle ? circle->getRadius() : 1.0;
    
    // 没碰到，走人
    if (distance > radius) return;
    
    // 5. 分解速度：法向速度 Vn 和 切向速度 Vt
    double vn = vx * nx + vy * ny; // 垂直于斜面
    double vt = vx * tx + vy * ty; // 平行于斜面
    
    // 如果已经在离开斜面，不处理碰撞 (防止吸附)
    if (vn > 0) return;
    
    // 6. 反弹 (Restitution)
    double restitution;
    dynamicShape.getRestitution(restitution);
    
    // 如果撞击速度很小，就不反弹，改为贴地滑行
    if (std::abs(vn) < 2.0) {
        restitution = 0.0;
    }
    
    // 新的法向速度 (反向)
    double vn_new = -vn * restitution;
    
    // 7. 摩擦 (Friction) -> 影响切向速度
    double friction;
    dynamicShape.getFraction(friction);
    
    // 简单的空气阻力式摩擦：保留大部分切向速度
    // 0.02 是一个很小的系数，保证球能滚起来
    double frictionFactor = 1.0 - (friction * 0.02);
    if (frictionFactor < 0) frictionFactor = 0;
    
    double vt_new = vt * frictionFactor;
    
    // 8. 合成新速度
    double new_vx = vn_new * nx + vt_new * tx;
    double new_vy = vn_new * ny + vt_new * ty;
    
    dynamicShape.setVelocity(new_vx, new_vy);
    
    // 9. 位置修正 (推出来)
    double overlap = radius - distance;
    // 稍微多推一点 (1.001)，防止浮点误差导致下一帧还陷在里面
    if (overlap > 0) {
        shapeX += overlap * nx * 1.001;
        shapeY += overlap * ny * 1.001;
        dynamicShape.setCentre(shapeX, shapeY);
    }
}