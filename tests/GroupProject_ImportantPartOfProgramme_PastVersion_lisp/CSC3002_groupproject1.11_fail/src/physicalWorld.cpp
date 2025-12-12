#include "physicalWorld.h"
#include "physicalWorld.h"
#include "shapes.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>

// PhysicalWorld类的setGravity和getGravity已经在头文件中内联定义，这里不需要重复

//放置物体类的实现
void PhysicalWorld::placeDynamicShape(Shape& shape, double x_pos, double y_pos) {
	double f = ground.getFriction();
	shape.fraction = f;

	// 尝试将shape转换为Circle
	const Circle* circle = dynamic_cast<const Circle*>(&shape);
	if (circle) {
		shape.setCentre(x_pos, y_pos + circle->getRadius());
		return;
	}

	// 尝试将shape转换为AABB
	const AABB* aabb = dynamic_cast<const AABB*>(&shape);
	if (aabb) {
		shape.setCentre(x_pos, y_pos + aabb->getHeight() / 2);
		return;
	}
}

void PhysicalWorld::placeStaticShape(Shape& shape, double x_pos, double y_pos) {
	shape.setCentre(x_pos, y_pos);
}

void PhysicalWorld::placeShapeOnGround(Shape& shape, const Ground& ground) {
	double x, y;
	shape.getCentre(x, y);
	
	double f = ground.getFriction();
	shape.fraction = f;
	
	// 尝试将shape转换为Circle
	const Circle* circle = dynamic_cast<const Circle*>(&shape);
	if (circle) {
		shape.setCentre(x, ground.getYLevel() + circle->getRadius());
		return;
	}
	
	// 尝试将shape转换为AABB
	const AABB* aabb = dynamic_cast<const AABB*>(&shape);
	if (aabb) {
		shape.setCentre(x, ground.getYLevel() + aabb->getHeight() / 2);
		return;
	}
}

/*=========================================================================================================
 * placeShapeOnShape() - 将一个形状放置在另一个形状的上方
 *=========================================================================================================*/
void PhysicalWorld::placeShapeOnShape(Shape& topShape, Shape& bottomShape, double offsetX) {
	// 1. 获取底部形状的信息
	double bottomX, bottomY;
	bottomShape.getCentre(bottomX, bottomY);
	double bottomTop = bottomShape.getTop();  // 底部形状的顶部Y坐标
	
	// 2. 获取底部形状的摩擦系数并设置给上方形状
	double friction;
	bottomShape.getFraction(friction);
	topShape.setFraction(friction);
	
	double staticFriction;
	bottomShape.getStaticFraction(staticFriction);
	topShape.setStaticFraction(staticFriction);
	
	// 3. 计算上方形状的底部Y坐标（应该放置的位置）
	double topShapeBottom = topShape.getBottom();  // 当前底部位置
	double topShapeY;
	topShape.getCentre(topShapeY, topShapeY);  // 获取当前中心Y坐标
	
	// 计算从中心到底部的距离
	double centerToBottom = topShapeY - topShapeBottom;
	
	// 4. 计算新的中心位置
	double newY = bottomTop + centerToBottom;  // 新的中心Y坐标
	double newX = bottomX + offsetX;            // 新的中心X坐标
	
	// 5. 考虑倾斜角度的修正
	if (std::abs(inclineAngle) > 0.01) {
		const double PI = 3.14159265358979323846;
		double angleRad = inclineAngle * PI / 180.0;
		double normalX = -std::sin(angleRad);
		double normalY = std::cos(angleRad);
		
		double distance = centerToBottom * 2.0;
		const Circle* topCircle = dynamic_cast<const Circle*>(&topShape);
		const AABB* topAABB = dynamic_cast<const AABB*>(&topShape);
		
		if (topCircle) distance = topCircle->getRadius();
		else if (topAABB) distance = topAABB->getHeight() / 2.0;
		
		const Circle* bottomCircle = dynamic_cast<const Circle*>(&bottomShape);
		const AABB* bottomAABB = dynamic_cast<const AABB*>(&bottomShape);
		const Wall* bottomWall = dynamic_cast<const Wall*>(&bottomShape);
		
		double bottomDistance = 0.0;
		if (bottomCircle) bottomDistance = bottomCircle->getRadius();
		else if (bottomAABB) bottomDistance = bottomAABB->getHeight() / 2.0;
		else if (bottomWall) bottomDistance = bottomWall->getHeight() / 2.0;
		
		double totalDistance = bottomDistance + distance + 0.01;
		newX = bottomX + offsetX + totalDistance * normalX;
		newY = bottomY + totalDistance * normalY;
	} else {
		newY += 0.01;
	}
	
	topShape.setCentre(newX, newY);
}

void PhysicalWorld::start() {
	isPaused = false;
}

/*=========================================================================================================
 * Pause() - 暂停物理模拟
 * 【关键修改】删除了 saveStates()，防止覆盖用户的拖拽操作
 *=========================================================================================================*/
void PhysicalWorld::Pause() {
	if (!isPaused) {
		// saveStates(); // 禁用自动存档
		isPaused = true;
	}
}

/*=========================================================================================================
 * Continue() - 继续物理模拟
 * 【关键修改】删除了 restoreStates()，让物体从当前（拖拽后）的位置继续运动
 *=========================================================================================================*/
void PhysicalWorld::Continue() {
	if (isPaused) {
		// restoreStates(); // 禁用自动读档
		isPaused = false;
	}
}

void PhysicalWorld::Stop() {
	isPaused = false;
	savedStates.clear();
}

void PhysicalWorld::saveStates() {
	savedStates.clear();
	for (auto& shape : dynamicShapeList) {
		ShapeState state;
		shape->getCentre(state.x, state.y);
		shape->getVelocity(state.vx, state.vy);
		state.mass = shape->getMass();
		state.isSupported = shape->getIsSupported();
		state.supporter = shape->getSupporter();
		shape->getNormalForce(state.normalForce[0], state.normalForce[1]);
		savedStates.push_back(state);
	}
}

void PhysicalWorld::restoreStates() {
	if (savedStates.size() != dynamicShapeList.size()) return;
	for (size_t i = 0; i < dynamicShapeList.size(); i++) {
		Shape* shape = dynamicShapeList[i];
		const ShapeState& state = savedStates[i];
		shape->setCentre(state.x, state.y);
		shape->setVelocity(state.vx, state.vy);
		shape->setMass(state.mass);
		shape->setIsSupported(state.isSupported);
		shape->setSupporter(state.supporter);
		shape->normalforce[0] = state.normalForce[0];
		shape->normalforce[1] = state.normalForce[1];
	}
}

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
	for (auto& shape : shapeList) {
		shape->resetSupportStatus();
	}
}

void PhysicalWorld::detectSupportRelations(std::vector<Shape*>& shapeList, const Ground& ground) {
	for (auto& shape : shapeList) {
		if (shape->HasCollidedWithGround(ground.getYLevel())) {
			shape->setIsSupported(true);
			shape->setSupporter(nullptr);
		}
		for (auto& other : shapeList) {
			if (shape != other) {
				shape->checkSupportStatus(*other);
			}
		}
	}
}

void PhysicalWorld::calculateNormalForces(std::vector<Shape*>& shapeList) {
	for (auto& shape : shapeList) {
		shape->normalforce[0] = 0.0;
		shape->normalforce[1] = 0.0;
	}
	for (auto& shape : shapeList) {
		calculateNormalForceRecursive(shape, shapeList);
	}
}

void PhysicalWorld::calculateNormalForceRecursive(Shape* shape, std::vector<Shape*>& shapeList) {
	if (shape->normalforce[1] != 0.0) return;
	
	const double PI = 3.14159265358979323846;
	double angleRad = inclineAngle * PI / 180.0;
	double totalWeight = shape->getMass() * gravity * std::cos(angleRad);
	
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
	
	if (shape->HasCollidedWithGround(ground.getYLevel())) {
		placeShapeOnGround(*shape, ground);
	}
	
	double r;
	shape->getRestitution(r);
	
	if (vy < -0.01) vy = -vy * r;
	else vy = 0.0;
	shape->setVelocity(vx, vy);
	
	const double PI = 3.14159265358979323846;
	double angleRad = inclineAngle * PI / 180.0;
	double drivingForce = shape->getMass() * gravity * std::sin(angleRad);
	
	double friction = 0.0;
    double static_friction = 0.0;
	
	if (shape->HasCollidedWithGround(ground.getYLevel())) {
		friction = ground.getFriction();
        static_friction = ground.getStaticFriction();
		double fx, fy;
		shape->getNormalForce(fx, fy);
		double normalForce = std::abs(fy);
        shape->applyFriction(normalForce, friction, static_friction, drivingForce);
	} else if (shape->getSupporter() != nullptr) {
		Shape* supporter = shape->getSupporter();
		supporter->getFraction(friction);
        supporter->getStaticFraction(static_friction);
		double fx, fy;
		shape->getNormalForce(fx, fy);
		double normalForce = std::abs(fy);
		double supVx, supVy;
		supporter->getVelocity(supVx, supVy);
		applyFrictionOnSupporter(shape, supporter, normalForce, friction, static_friction, drivingForce);
	}
	shape->addToTotalForce(drivingForce, 0.0);
}

void PhysicalWorld::handleAirborneShape(Shape* shape, double deltaTime) {
	shape->applyGravity(gravity);
}

void PhysicalWorld::applyFrictionOnSupporter(Shape* shape, Shape* supporter, double normalForce, double friction, double static_friction, double drivingForce) {
	double supVx, supVy;
	supporter->getVelocity(supVx, supVy);
	shape->applyFrictionRelative(normalForce, friction, static_friction, supVx, supVy, drivingForce);
	
	double shapeVx, shapeVy;
	shape->getVelocity(shapeVx, shapeVy);
	double relVx = shapeVx - supVx;
	double relSpeed = std::sqrt(relVx * relVx);
	
	if (relSpeed > 1e-6) {
		double frictionMag = friction * normalForce;
		double dirX = -relVx / relSpeed;
		supporter->addToTotalForce(-frictionMag * dirX, 0.0);
	}
}

/*=========================================================================================================
 * 碰撞检测和处理函数 - 检测并处理所有物体之间的碰撞
 * 【关键修改】增加了“动态物体 vs 静态物体”的检测循环，修复墙壁和斜面失效的问题
 *=========================================================================================================*/
void PhysicalWorld::handleAllCollisions(std::vector<Shape*>& shapeList) {
	const double MAX_INTERACTION_DISTANCE = 200.0;
	
    // 1. 动态 vs 动态
	for (size_t i = 0; i < shapeList.size(); i++) {
		for (size_t j = i + 1; j < shapeList.size(); j++) {
			double x1, y1, x2, y2;
			shapeList[i]->getCentre(x1, y1);
			shapeList[j]->getCentre(x2, y2);
			
			double dx = x2 - x1;
			double dy = y2 - y1;
			double distanceSquared = dx * dx + dy * dy;
			
			if (distanceSquared > MAX_INTERACTION_DISTANCE * MAX_INTERACTION_DISTANCE) continue; 
			
			if (shapeList[i]->check_collision(*shapeList[j])) {
				bool isSupportRelation = false;
				if (shapeList[i]->getSupporter() == shapeList[j]) isSupportRelation = true;
				else if (shapeList[j]->getSupporter() == shapeList[i]) isSupportRelation = true;
				
				if (!isSupportRelation) {
					resolveCollision(*shapeList[i], *shapeList[j]);
				}
			}
		}
	}

    // 2. 【新增】动态 vs 静态 (墙壁、斜坡)
    for (size_t i = 0; i < shapeList.size(); i++) {
        for (size_t k = 0; k < staticShapeList.size(); k++) {
            Shape* dynamicShape = shapeList[i];
            Shape* staticShape = staticShapeList[k];

            // 碰撞检测
            if (dynamicShape->check_collision(*staticShape)) {
                // 优先检查是否是墙壁
                Wall* wall = dynamic_cast<Wall*>(staticShape);
                // 优先检查是否是斜坡
                Slope* slope = dynamic_cast<Slope*>(staticShape);
                
                if (wall) {
                    resolveCollisionWithWall(*dynamicShape, *wall);
                } 
                else if (slope) {
                    resolveCollisionWithSlope(*dynamicShape, *slope);
                }
                else {
                    resolveCollision(*dynamicShape, *staticShape);
                }
            }
        }
    }
}

/*=========================================================================================================
 * 碰撞解决函数
 *=========================================================================================================*/
void PhysicalWorld::Collisions(Shape& shape1, Shape& shape2) {
	resolveCollision(shape1, shape2);
}

void PhysicalWorld::resolveCollision(Shape& shape1, Shape& shape2) {
	double m1 = shape1.getMass();
	double m2 = shape2.getMass();
	
	double v1x, v1y, v2x, v2y;
	shape1.getVelocity(v1x, v1y);
	shape2.getVelocity(v2x, v2y);
	
	double x1, y1, x2, y2;
	shape1.getCentre(x1, y1);
	shape2.getCentre(x2, y2);
	
	double nx = x2 - x1;
	double ny = y2 - y1;
	double distance = std::sqrt(nx * nx + ny * ny);
	
	if (distance < 0.0001) {
        nx = 1.0; ny = 0.0; distance = 0.0001; // 防止除零
    } else {
        nx /= distance;
        ny /= distance;
    }
	
	double tx = -ny;
	double ty = nx;
	
	double v1n = v1x * nx + v1y * ny;
	double v1t = v1x * tx + v1y * ty;
	double v2n = v2x * nx + v2y * ny;
	double v2t = v2x * tx + v2y * ty;
	
	if (v2n - v1n > 0) return;
	
	double r1, r2;
	shape1.getRestitution(r1);
	shape2.getRestitution(r2);
	double e = (r1 + r2) / 2.0;
	
	double v1n_new, v2n_new;
	
	if (std::isinf(m1) && std::isinf(m2)) {
		v1n_new = v1n; v2n_new = v2n;
	} else if (std::isinf(m1)) {
		v1n_new = v1n; v2n_new = v1n - e * (v2n - v1n);
	} else if (std::isinf(m2)) {
		v2n_new = v2n; v1n_new = v2n - e * (v1n - v2n);
	} else {
		double totalMass = m1 + m2;
		v1n_new = ((m1 - e*m2)*v1n + (1+e)*m2*v2n) / totalMass;
		v2n_new = ((m2 - e*m1)*v2n + (1+e)*m1*v1n) / totalMass;
	}
	
	double v1t_new = v1t;
	double v2t_new = v2t;
	
	double v1x_new = v1n_new * nx + v1t_new * tx;
	double v1y_new = v1n_new * ny + v1t_new * ty;
	double v2x_new = v2n_new * nx + v2t_new * tx;
	double v2y_new = v2n_new * ny + v2t_new * ty;
	
	shape1.setVelocity(v1x_new, v1y_new);
	shape2.setVelocity(v2x_new, v2y_new);
	
	separateOverlappingShapes(shape1, shape2, nx, ny, distance);
}

void PhysicalWorld::separateOverlappingShapes(Shape& shape1, Shape& shape2, double nx, double ny, double distance) {
	const double separationPercent = 0.8;
	double overlap = 0.0;
	
	double x1, y1, x2, y2;
	shape1.getCentre(x1, y1);
	shape2.getCentre(x2, y2);
	
	const Circle* c1 = dynamic_cast<const Circle*>(&shape1);
	const Circle* c2 = dynamic_cast<const Circle*>(&shape2);
	const AABB* a1 = dynamic_cast<const AABB*>(&shape1);
	const AABB* a2 = dynamic_cast<const AABB*>(&shape2);
	
	if (c1 && c2) {
		overlap = c1->getRadius() + c2->getRadius() - distance;
	}
	else if (a1 && a2) {
		double overlapX = (a1->getWidth() + a2->getWidth()) / 2.0 - std::abs(x2 - x1);
		double overlapY = (a1->getHeight() + a2->getHeight()) / 2.0 - std::abs(y2 - y1);
		if (overlapX < overlapY) {
			overlap = overlapX;
			nx = (x2 > x1) ? 1.0 : -1.0; ny = 0.0;
		} else {
			overlap = overlapY;
			nx = 0.0; ny = (y2 > y1) ? 1.0 : -1.0;
		}
	}
	else if ((c1 && a2) || (a1 && c2)) {
		const Circle* circle = c1 ? c1 : c2;
		const AABB* aabb = a1 ? a1 : a2;
		
		double circleX, circleY, aabbX, aabbY;
		circle->getCentre(circleX, circleY);
		aabb->getCentre(aabbX, aabbY);
		
		double closestX = std::max(aabbX - aabb->getWidth() / 2.0,
		                           std::min(circleX, aabbX + aabb->getWidth() / 2.0));
		double closestY = std::max(aabbY - aabb->getHeight() / 2.0,
		                           std::min(circleY, aabbY + aabb->getHeight() / 2.0));
		
		double dx = circleX - closestX;
		double dy = circleY - closestY;
		double distToClosest = std::sqrt(dx * dx + dy * dy);
		
		overlap = circle->getRadius() - distToClosest;
		if (distToClosest < 0.0001) overlap = circle->getRadius();
	}
	
	if (overlap > 0) {
		double m1 = shape1.getMass();
		double m2 = shape2.getMass();
		double totalInvMass = 1.0 / m1 + 1.0 / m2;
		double correctionX = (overlap * separationPercent / totalInvMass) * nx;
		double correctionY = (overlap * separationPercent / totalInvMass) * ny;
		shape1.setCentre(x1 - correctionX / m1, y1 - correctionY / m1);
		shape2.setCentre(x2 + correctionX / m2, y2 + correctionY / m2);
	}
}

void PhysicalWorld::handleBoundaryCollision(Shape& shape) {
	double x, y, vx, vy;
	shape.getCentre(x, y);
	shape.getVelocity(vx, vy);
	bool outOfBounds = false;
	
	const Circle* circle = dynamic_cast<const Circle*>(&shape);
	const AABB* aabb = dynamic_cast<const AABB*>(&shape);
	
	if (circle) {
		double radius = circle->getRadius();
		if (x + radius < bounds[0] || x - radius > bounds[1] ||
		    y + radius < bounds[2] || y - radius > bounds[3]) {
			outOfBounds = true;
			vx = 0.0; vy = 0.0;
			if (x + radius < bounds[0]) x = bounds[0] - radius;
			else if (x - radius > bounds[1]) x = bounds[1] + radius;
			if (y + radius < bounds[2]) y = bounds[2] - radius;
			else if (y - radius > bounds[3]) y = bounds[3] + radius;
		}
	}
	else if (aabb) {
		double halfWidth = aabb->getWidth() / 2.0;
		double halfHeight = aabb->getHeight() / 2.0;
		if (x + halfWidth < bounds[0] || x - halfWidth > bounds[1] ||
		    y + halfHeight < bounds[2] || y - halfHeight > bounds[3]) {
			outOfBounds = true;
			vx = 0.0; vy = 0.0;
			if (x + halfWidth < bounds[0]) x = bounds[0] - halfWidth;
			else if (x - halfWidth > bounds[1]) x = bounds[1] + halfWidth;
			if (y + halfHeight < bounds[2]) y = bounds[2] - halfHeight;
			else if (y - halfHeight > bounds[3]) y = bounds[3] + halfHeight;
		}
	}
	
	if (outOfBounds) {
		shape.setCentre(x, y);
		shape.setVelocity(vx, vy);
	}
}

bool PhysicalWorld::isInBounds(const Shape& shape) const {
	double x, y;
	shape.getCentre(x, y);
	const Circle* circle = dynamic_cast<const Circle*>(&shape);
	const AABB* aabb = dynamic_cast<const AABB*>(&shape);
	if (circle) {
		double r = circle->getRadius();
		return (x - r >= bounds[0] && x + r <= bounds[1] &&
		        y - r >= bounds[2] && y + r <= bounds[3]);
	}
	else if (aabb) {
		double hw = aabb->getWidth() / 2.0;
		double hh = aabb->getHeight() / 2.0;
		return (x - hw >= bounds[0] && x + hw <= bounds[1] &&
		        y - hh >= bounds[2] && y + hh <= bounds[3]);
	}
	return true;
}

bool PhysicalWorld::checkBoundaryCollision(const Shape& shape) const {
	return !isInBounds(shape);
}

void PhysicalWorld::addDynamicShape(Shape* shape) { if (shape != nullptr) dynamicShapeList.push_back(shape); }
void PhysicalWorld::addStaticShape(Shape* shape) { if (shape != nullptr) staticShapeList.push_back(shape); }
void PhysicalWorld::removeDynamicShape(Shape* shape) {
	auto it = std::find(dynamicShapeList.begin(), dynamicShapeList.end(), shape);
	if (it != dynamicShapeList.end()) dynamicShapeList.erase(it);
}
void PhysicalWorld::removeStaticShape(Shape* shape) {
	auto it = std::find(staticShapeList.begin(), staticShapeList.end(), shape);
	if (it != staticShapeList.end()) staticShapeList.erase(it);
}

Shape* PhysicalWorld::findShapeByName(const std::string& name) {
	Shape* result = findDynamicShapeByName(name);
	if (result != nullptr) return result;
	return findStaticShapeByName(name);
}
Shape* PhysicalWorld::findDynamicShapeByName(const std::string& name) {
	for (auto& shape : dynamicShapeList) { if (shape->getName() == name) return shape; }
	return nullptr;
}
Shape* PhysicalWorld::findStaticShapeByName(const std::string& name) {
	for (auto& shape : staticShapeList) { if (shape->getName() == name) return shape; }
	return nullptr;
}

std::vector<Shape*> PhysicalWorld::findShapesByType(const std::string& type) {
	std::vector<Shape*> result = findDynamicShapesByType(type);
	std::vector<Shape*> staticResult = findStaticShapesByType(type);
	result.insert(result.end(), staticResult.begin(), staticResult.end());
	return result;
}
std::vector<Shape*> PhysicalWorld::findDynamicShapesByType(const std::string& type) {
	std::vector<Shape*> result;
	for (auto& shape : dynamicShapeList) { if (shape->getType() == type) result.push_back(shape); }
	return result;
}
std::vector<Shape*> PhysicalWorld::findStaticShapesByType(const std::string& type) {
	std::vector<Shape*> result;
	for (auto& shape : staticShapeList) { if (shape->getType() == type) result.push_back(shape); }
	return result;
}

void PhysicalWorld::clearDynamicShapes() { dynamicShapeList.clear(); }
void PhysicalWorld::clearStaticShapes() { staticShapeList.clear(); }
void PhysicalWorld::clearAllShapes() { clearDynamicShapes(); clearStaticShapes(); }

void PhysicalWorld::printAllShapes() const {
	std::cout << "Dynamic: " << dynamicShapeList.size() << ", Static: " << staticShapeList.size() << std::endl;
	printDynamicShapes(); printStaticShapes();
}
void PhysicalWorld::printDynamicShapes() const {
	for (size_t i = 0; i < dynamicShapeList.size(); i++) {
		Shape* s = dynamicShapeList[i];
		double x, y; s->getCentre(x, y);
		std::cout << "Dyn[" << i << "] " << s->getName() << " (" << x << "," << y << ")" << std::endl;
	}
}
void PhysicalWorld::printStaticShapes() const {
	for (size_t i = 0; i < staticShapeList.size(); i++) {
		Shape* s = staticShapeList[i];
		double x, y; s->getCentre(x, y);
		std::cout << "Stat[" << i << "] " << s->getName() << " (" << x << "," << y << ")" << std::endl;
	}
}

std::string PhysicalWorld::parseShapeType(const std::string& type) const {
	std::string lowerType = type;
	std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
	if (lowerType == "circle") return "Circle";
	else if (lowerType == "aabb" || lowerType == "box" || lowerType == "rectangle" || lowerType == "rect") return "AABB";
	return "";
}

Shape* PhysicalWorld::createShape(const std::string& type, const std::string& name, double mass, double size1, double size2, bool isDynamic) {
	Shape* shape = nullptr;
	std::string standardType = parseShapeType(type);
	
	if (standardType == "Circle") {
		if (isDynamic) shape = new Circle(mass, size1, 0.0, 0.0);
		else {
			Circle* circle = new Circle(mass, size1, 0.0, 0.0);
			circle->setMass(INFINITY);
			shape = circle;
		}
	}
	else if (standardType == "AABB") {
		if (size2 == 0.0) size2 = size1;
		if (isDynamic) shape = new AABB(mass, size1, size2, 0.0, 0.0);
		else {
			AABB* box = new AABB(mass, size1, size2, 0.0, 0.0);
			box->setMass(INFINITY);
			shape = box;
		}
	}
	if (shape != nullptr) {
		if (name.empty()) shape->setName(generateUniqueName(standardType));
		else shape->setName(name);
	}
	return shape;
}

std::string PhysicalWorld::generateUniqueName(const std::string& type) {
	int counter = 1;
	std::string baseName = type;
	std::string uniqueName;
	do { uniqueName = baseName + "_" + std::to_string(counter); counter++; } while (isNameExists(uniqueName));
	return uniqueName;
}

bool PhysicalWorld::isNameExists(const std::string& name) const {
	for (const auto& s : dynamicShapeList) { if (s->getName() == name) return true; }
	for (const auto& s : staticShapeList) { if (s->getName() == name) return true; }
	return false;
}

Shape* PhysicalWorld::placeDynamicShapeByType(const std::string& type, const std::string& name, double x, double y, double m, double s1, double s2) {
	Shape* shape = createShape(type, name, m, s1, s2, true);
	if (shape) { addDynamicShape(shape); placeDynamicShape(*shape, x, y); }
	return shape;
}
Shape* PhysicalWorld::placeStaticShapeByType(const std::string& type, const std::string& name, double x, double y, double m, double s1, double s2) {
	Shape* shape = createShape(type, name, m, s1, s2, false);
	if (shape) { addStaticShape(shape); placeStaticShape(*shape, x, y); }
	return shape;
}

Wall* PhysicalWorld::placeWall(const std::string& name, double x, double y, double w, double h, double f) {
	Wall* wall = new Wall(w, h, x, y, f);
	if (wall) {
		if (name.empty()) wall->setName(generateUniqueName("Wall")); else wall->setName(name);
		wall->setMass(INFINITY);
		addStaticShape(wall);
	}
	return wall;
}

void PhysicalWorld::handleWallCollision(Shape& dynamicShape, const Wall& wall) {
	if (dynamicShape.check_collision(wall)) resolveCollisionWithWall(dynamicShape, wall);
}

void PhysicalWorld::resolveCollisionWithWall(Shape& dynamicShape, const Wall& wall) {
	double shapeX, shapeY, shapeVx, shapeVy;
	dynamicShape.getCentre(shapeX, shapeY);
	dynamicShape.getVelocity(shapeVx, shapeVy);
	
	double wallX, wallY;
	wall.getCentre(wallX, wallY);
	
	double restitution;
	dynamicShape.getRestitution(restitution);
	
	double nx = shapeX - wallX;
	double ny = shapeY - wallY;
	double distance = std::sqrt(nx * nx + ny * ny);
	
	if (distance < 0.0001) { nx = 0.0; ny = 1.0; } 
	else { nx /= distance; ny /= distance; }
	
	double vn = shapeVx * nx + shapeVy * ny;
	if (vn > 0) return;
	
	double vn_new = -vn * restitution;
	shapeVx = shapeVx - vn * nx + vn_new * nx;
	shapeVy = shapeVy - vn * ny + vn_new * ny;
	dynamicShape.setVelocity(shapeVx, shapeVy);
	
	// 位置修正
	double overlap = 0.0;
	const Circle* circle = dynamic_cast<const Circle*>(&dynamicShape);
	const AABB* aabb = dynamic_cast<const AABB*>(&dynamicShape);
	
	if (circle) {
		double closestX = std::max(wall.getLeft(), std::min(circle->mass_centre[0], wall.getRight()));
		double closestY = std::max(wall.getBottom(), std::min(circle->mass_centre[1], wall.getTop()));
		double dx = circle->mass_centre[0] - closestX;
		double dy = circle->mass_centre[1] - closestY;
		double dist = std::sqrt(dx*dx + dy*dy);
		overlap = circle->getRadius() - dist;
		if (dist < 0.0001) { overlap = circle->getRadius(); nx = (circle->mass_centre[0] > wallX) ? 1.0 : -1.0; ny = 0.0; }
        else { nx = dx/dist; ny = dy/dist; }
	} else if (aabb) {
        // 简化AABB推离
        double dx = shapeX - wallX; double dy = shapeY - wallY;
        double ox = (aabb->getWidth() + wall.getWidth())/2.0 - std::abs(dx);
        double oy = (aabb->getHeight() + wall.getHeight())/2.0 - std::abs(dy);
        if (ox < oy) { overlap = ox; nx = (dx > 0) ? 1.0 : -1.0; ny = 0.0; }
        else { overlap = oy; nx = 0.0; ny = (dy > 0) ? 1.0 : -1.0; }
    }
	
	if (overlap > 0) {
		dynamicShape.setCentre(shapeX + overlap * nx, shapeY + overlap * ny);
	}
}

void PhysicalWorld::applyGravitySin(Shape* shape, double inclineAngle) {
	const double PI = 3.14159265358979323846;
	double angleRad = inclineAngle * PI / 180.0;
	double forceMagnitude = shape->getMass() * gravity * std::sin(angleRad);
	shape->addToTotalForce(forceMagnitude, 0.0);
}

std::vector<double> PhysicalWorld::inclineToStandard(double x_rel, double y_rel) const {
	const double PI = 3.14159265358979323846;
	double angleRad = inclineAngle * PI / 180.0;
	double cosAngle = std::cos(angleRad);
	double sinAngle = std::sin(angleRad);
	double x_standard = x_rel * cosAngle - y_rel * sinAngle;
	double y_standard = x_rel * sinAngle + y_rel * cosAngle;
	return {x_standard, y_standard};
}

/*=========================================================================================================
 * 【新增】解决动态物体与斜坡的碰撞
 *=========================================================================================================*/
void PhysicalWorld::resolveCollisionWithSlope(Shape& dynamicShape, const Slope& slope) {
    double shapeX, shapeY, vx, vy;
    dynamicShape.getCentre(shapeX, shapeY);
    dynamicShape.getVelocity(vx, vy);
    
    double slopeX, slopeY;
    slope.getCentre(slopeX, slopeY);
    double angle = slope.getAngle();
    
    // 计算法向量
    double nx = -std::sin(angle);
    double ny = std::cos(angle);
    
    // 相对位置
    double dx = shapeX - slopeX;
    double dy = shapeY - slopeY;
    
    // 投影距离
    double distance = dx * nx + dy * ny;
    
    const Circle* circle = dynamic_cast<const Circle*>(&dynamicShape);
    double radius = circle ? circle->getRadius() : 1.0;
    
    // 法向速度
    double vNormal = vx * nx + vy * ny;
    
    // 只有当朝向斜面运动时才反弹
    if (vNormal < 0) {
        double restitution;
        dynamicShape.getRestitution(restitution);
        double j = -(1 + restitution) * vNormal;
        
        vx += j * nx;
        vy += j * ny;
        
        // 简单摩擦
        double tx = -ny; double ty = nx;
        double vTangent = vx * tx + vy * ty;
        vx -= vTangent * 0.05 * tx;
        vy -= vTangent * 0.05 * ty;
        
        dynamicShape.setVelocity(vx, vy);
    }
    
    // 位置修正
    double overlap = radius - distance;
    if (overlap > 0) {
        shapeX += overlap * nx;
        shapeY += overlap * ny;
        dynamicShape.setCentre(shapeX, shapeY);
    }
}