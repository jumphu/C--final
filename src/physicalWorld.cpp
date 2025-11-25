#include "physicalWorld.h"
#include "shapes.h"
#include <iostream>
#include <cmath>

// PhysicalWorld类的setGravity和getGravity已经在头文件中内联定义，这里不需要重复

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

void PhysicalWorld::start() {
	//std::cout << "物理世界开始运行，重力加速度为: " << gravity << " m/s²" << std::endl;
}

/*=========================================================================================================
 * update() 函数 - 版本1：使用成员变量 timeStep
 * 简洁模式：自动使用内部时间步长
 *=========================================================================================================*/
void PhysicalWorld::update(std::vector<Shape*>& shapeList, const Ground& ground) {
	// 委托到版本2，使用成员变量 timeStep
	update(shapeList, timeStep, ground);
}

/*=========================================================================================================
 * update() 函数 - 版本2：使用参数 deltaTime
 * 灵活模式：可以临时指定时间步长
 *=========================================================================================================*/
void PhysicalWorld::update(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground) {
	// ========== 第一阶段：重置支撑状态 ==========
	resetSupportStates(shapeList);
	
	// ========== 第二阶段：检测支撑关系 ==========
	detectSupportRelations(shapeList, ground);
	
	// ========== 第二点五阶段：计算正压力（从上往下累积）==========
	calculateNormalForces(shapeList);
	
	// ========== 第三阶段：物理更新 ==========
	updatePhysics(shapeList, deltaTime, ground);
	
	// ========== 第四阶段：碰撞检测和处理 ==========
	handleAllCollisions(shapeList);
}

/*=========================================================================================================
 * 第一阶段：重置支撑状态
 * 在每帧开始时清空所有物体的支撑状态
 *=========================================================================================================*/
void PhysicalWorld::resetSupportStates(std::vector<Shape*>& shapeList) {
	for (auto& shape : shapeList) {
		shape->resetSupportStatus();
	}
}

/*=========================================================================================================
 * 第二阶段：检测支撑关系
 * 检测每个物体是否被地面或其他物体支撑
 *=========================================================================================================*/
void PhysicalWorld::detectSupportRelations(std::vector<Shape*>& shapeList, const Ground& ground) {
	for (auto& shape : shapeList) {
		// 检查与地面的支撑
		if (shape->HasCollidedWithGround(ground.getYLevel())) {
			shape->setIsSupported(true);
			shape->setSupporter(nullptr); // 地面没有 Shape 对象，设为 nullptr
		}
		
		// 检查与其他物体的支撑（从下往上检查）
		for (auto& other : shapeList) {
			if (shape != other) {
				shape->checkSupportStatus(*other);
			}
		}
	}
}

/*=========================================================================================================
 * 第二点五阶段：计算正压力（从上往下累积）
 * 计算每个物体对其支撑物施加的正压力（包括上方所有物体的重力）
 *=========================================================================================================*/
void PhysicalWorld::calculateNormalForces(std::vector<Shape*>& shapeList) {
	// 清空所有物体的 normalforce
	for (auto& shape : shapeList) {
		shape->normalforce[0] = 0.0;
		shape->normalforce[1] = 0.0;
	}
	
	// 从上往下递归计算正压力
	for (auto& shape : shapeList) {
		calculateNormalForceRecursive(shape, shapeList);
	}
}

/*=========================================================================================================
 * 递归计算正压力
 * 计算物体及其上方所有物体的总重力，并记录到 normalforce 中
 *=========================================================================================================*/
void PhysicalWorld::calculateNormalForceRecursive(Shape* shape, std::vector<Shape*>& shapeList) {
	// 如果已经计算过，直接返回
	if (shape->normalforce[1] != 0.0) {
		return;
	}
	
	// 计算自身重力
	double totalWeight = shape->getMass() * gravity;
	
	// 查找压在这个物体上面的所有物体
	for (auto& other : shapeList) {
		if (other != shape && other->getSupporter() == shape) {
			// other 在 shape 上面
			// 递归计算 other 的正压力
			calculateNormalForceRecursive(other, shapeList);
			
			// ✅ 修复：只累加 other 的 normalforce（已经包含了 other 及其上方所有物体的重力）
			totalWeight += std::abs(other->normalforce[1]);
		}
	}
	
	// 记录这个物体对下方施加的正压力（向下为负）
	shape->normalforce[1] = -totalWeight;
}

/*=========================================================================================================
 * 第三阶段：物理更新
 * 根据物体的支撑状态，施加相应的力并更新速度和位置
 *=========================================================================================================*/
void PhysicalWorld::updatePhysics(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground) {
	for (auto& shape : shapeList) {
		// 清空上一帧的力累加器
		shape->clearTotalForce();
		
		// 根据支撑状态分别处理
		if (shape->getIsSupported()) {
			handleSupportedShape(shape, deltaTime, ground);
		} else {
			handleAirborneShape(shape, deltaTime);
		}
		
		// 应用累加的力到速度
		shape->applyTotalForce(deltaTime);
		
		// 更新位置
		shape->update(deltaTime);
		
		// 检查与边界的碰撞
		handleBoundaryCollision(*shape);
	}
}

/*=========================================================================================================
 * 处理被支撑的物体（在地面或其他物体上）
 *=========================================================================================================*/
void PhysicalWorld::handleSupportedShape(Shape* shape, double deltaTime, const Ground& ground) {
	// 获取当前位置和速度
	double x, y, vx, vy;
	shape->getCentre(x, y);
	shape->getVelocity(vx, vy);
	
	// 1. 处理位置修正
	if (shape->HasCollidedWithGround(ground.getYLevel())) {
		placeShapeOnGround(*shape, ground);
	}
	
	// 2. 处理垂直速度（反弹或静止）
	double r;
	shape->getRestitution(r);
	
	if (vy < -0.01) {  // 有明显向下速度
		vy = -vy * r;  // 弹性反弹
	} else {
		vy = 0.0;      // 静止在支撑物上
	}
	shape->setVelocity(vx, vy);
	
	// 3. 施加摩擦力
	double friction = 0.0;
	
	if (shape->HasCollidedWithGround(ground.getYLevel())) {
		// ✅ 在地面上：使用正确的正压力（包括上方物体的质量）
		friction = ground.getFriction();
		
		// 获取上方物体施加的压力
		double fx, fy;
		shape->getNormalForce(fx, fy);
		
		// 正压力 = 自身重力 + 上方物体施加的压力
		double normalForce = shape->getMass() * gravity + std::abs(fy);
		
		shape->applyFriction(normalForce, friction);
		
	} else if (shape->getSupporter() != nullptr) {
		// ✅ 在其他物体上：使用正确的正压力（包括上方物体的质量）
		Shape* supporter = shape->getSupporter();
		supporter->getFraction(friction);
		
		// 获取上方物体施加的压力
		double fx, fy;
		shape->getNormalForce(fx, fy);
		
		// 正压力 = 自身重力 + 上方物体施加的压力
		double normalForce = shape->getMass() * gravity + std::abs(fy);
		
		// 获取支撑物速度
		double supVx, supVy;
		supporter->getVelocity(supVx, supVy);
		
		// 使用相对速度摩擦力（包含牛顿第三定律反作用力）
		applyFrictionOnSupporter(shape, supporter, normalForce, friction);
	}
}

/*=========================================================================================================
 * 处理在空中的物体（自由落体）
 *=========================================================================================================*/
void PhysicalWorld::handleAirborneShape(Shape* shape, double deltaTime) {
	// 在空中：只施加重力
	shape->applyGravity(gravity);
}

/*=========================================================================================================
 * 在支撑物上施加摩擦力（包括牛顿第三定律反作用力）
 * 使用 Shape 的 applyFrictionRelative() 方法
 *=========================================================================================================*/
void PhysicalWorld::applyFrictionOnSupporter(Shape* shape, Shape* supporter, double normalForce, double friction) {
	// 获取支撑物速度
	double supVx, supVy;
	supporter->getVelocity(supVx, supVy);
	
	// ✅ 使用 Shape::applyFrictionRelative() 计算相对摩擦力
	shape->applyFrictionRelative(normalForce, friction, supVx, supVy);
	
	// ✅ 计算反作用力（牛顿第三定律）
	// 获取刚刚施加到 shape 上的摩擦力
	double shapeVx, shapeVy;
	shape->getVelocity(shapeVx, shapeVy);
	
	// 计算相对速度
	double relVx = shapeVx - supVx;
	double relSpeed = std::sqrt(relVx * relVx);
	
	if (relSpeed > 1e-6) {
		// 摩擦力大小
		double frictionMag = friction * normalForce;
		// 摩擦力方向（与相对速度相反）
		double dirX = -relVx / relSpeed;
		
		// 对支撑物施加反作用力
		// 如果不想让支撑物移动，请把支撑物质量设为 INFINITY
		supporter->addToTotalForce(-frictionMag * dirX, 0.0);
	}
}

void PhysicalWorld::Stop() {
	//std::cout << "物理世界停止运行。" << std::endl;
}

/*=========================================================================================================
 * 碰撞检测和处理函数 - 检测并处理所有物体之间的碰撞
 *=========================================================================================================*/
void PhysicalWorld::handleAllCollisions(std::vector<Shape*>& shapeList) {
	const double MAX_INTERACTION_DISTANCE = 200.0;
	
	for (size_t i = 0; i < shapeList.size(); i++) {
		for (size_t j = i + 1; j < shapeList.size(); j++) {
			// 获取两个物体的位置
			double x1, y1, x2, y2;
			shapeList[i]->getCentre(x1, y1);
			shapeList[j]->getCentre(x2, y2);
			
			// 快速距离检查（优化：跳过距离太远的物体对）
			double dx = x2 - x1;
			double dy = y2 - y1;
			double distanceSquared = dx * dx + dy * dy;
			
			if (distanceSquared > MAX_INTERACTION_DISTANCE * MAX_INTERACTION_DISTANCE) {
				continue; // 距离太远，跳过
			}
			
			// 碰撞检测
			if (shapeList[i]->check_collision(*shapeList[j])) {
				// 检查是否存在支撑关系
				bool isSupportRelation = false;
				
				if (shapeList[i]->getSupporter() == shapeList[j]) {
					isSupportRelation = true;  // i 在 j 上面
				} else if (shapeList[j]->getSupporter() == shapeList[i]) {
					isSupportRelation = true;  // j 在 i 上面
				}
				
				// 只有非支撑关系才处理碰撞
				if (!isSupportRelation) {
					resolveCollision(*shapeList[i], *shapeList[j]);
				}
			}
		}
	}
}

/*=========================================================================================================
 * 碰撞解决函数 - 使用运动学方法处理两个形状之间的碰撞
 * 不使用冲量，而是直接计算碰撞后的速度
 *=========================================================================================================*/
void PhysicalWorld::Collisions(Shape& shape1, Shape& shape2) {
	resolveCollision(shape1, shape2);
}

void PhysicalWorld::resolveCollision(Shape& shape1, Shape& shape2) {
	// 获取质量
	double m1 = shape1.getMass();
	double m2 = shape2.getMass();
	
	// 获取碰撞前的速度
	double v1x, v1y, v2x, v2y;
	shape1.getVelocity(v1x, v1y);
	shape2.getVelocity(v2x, v2y);
	
	// 获取位置信息，计算碰撞方向
	double x1, y1, x2, y2;
	shape1.getCentre(x1, y1);
	shape2.getCentre(x2, y2);
	
	// 碰撞法向量
	double nx = x2 - x1;
	double ny = y2 - y1;
	double distance = std::sqrt(nx * nx + ny * ny);
	
	if (distance < 0.0001) return; // 避免除零错误
	
	// 归一化法向量
	nx /= distance;
	ny /= distance;
	
	// 计算切向量（垂直于法向量）
	double tx = -ny;
	double ty = nx;
	
	// ========== 分解速度到法向和切向 ==========
	double v1n = v1x * nx + v1y * ny;  // 物体1的法向速度
	double v1t = v1x * tx + v1y * ty;  // 物体1的切向速度
	
	double v2n = v2x * nx + v2y * ny;  // 物体2的法向速度
	double v2t = v2x * tx + v2y * ty;  // 物体2的切向速度
	
	// 如果物体正在分离，不处理
	if (v2n - v1n > 0) return;
	
	// ========== 计算碰撞后的法向速度（一维弹性碰撞公式）==========
	double r1, r2;
	shape1.getRestitution(r1);
	shape2.getRestitution(r2);
	double e = (r1 + r2) / 2.0;  // 平均弹性系数
	
	double v1n_new, v2n_new;
	
	if (std::isinf(m1) && std::isinf(m2)) {
		v1n_new = v1n;
		v2n_new = v2n;
	} else if (std::isinf(m1)) {
		v1n_new = v1n;
		v2n_new = v1n - e * (v2n - v1n);
	} else if (std::isinf(m2)) {
		v2n_new = v2n;
		v1n_new = v2n - e * (v1n - v2n);
	} else {
		double totalMass = m1 + m2;
		v1n_new = ((m1 - e*m2)*v1n + (1+e)*m2*v2n) / totalMass;
		v2n_new = ((m2 - e*m1)*v2n + (1+e)*m1*v1n) / totalMass;
	}
	
	// 碰撞时不考虑摩擦，切向速度不变
	double v1t_new = v1t;
	double v2t_new = v2t;
	
	// ========== 重新组合速度（法向 + 切向）==========
	double v1x_new = v1n_new * nx + v1t_new * tx;
	double v1y_new = v1n_new * ny + v1t_new * ty;
	
	double v2x_new = v2n_new * nx + v2t_new * tx;
	double v2y_new = v2n_new * ny + v2t_new * ty;
	
	// 应用新速度
	shape1.setVelocity(v1x_new, v1y_new);
	shape2.setVelocity(v2x_new, v2y_new);
	
	// ========== 分离物体，避免重叠 ==========
	separateOverlappingShapes(shape1, shape2, nx, ny, distance);
}

/*=========================================================================================================
 * 分离重叠物体函数 - 根据形状类型计算重叠量并分离
 *=========================================================================================================*/
void PhysicalWorld::separateOverlappingShapes(Shape& shape1, Shape& shape2, double nx, double ny, double distance) {
	const double separationPercent = 0.8;
	double overlap = 0.0;
	
	// 获取位置
	double x1, y1, x2, y2;
	shape1.getCentre(x1, y1);
	shape2.getCentre(x2, y2);
	
	// 尝试识别形状类型并计算重叠量
	const Circle* c1 = dynamic_cast<const Circle*>(&shape1);
	const Circle* c2 = dynamic_cast<const Circle*>(&shape2);
	const AABB* a1 = dynamic_cast<const AABB*>(&shape1);
	const AABB* a2 = dynamic_cast<const AABB*>(&shape2);
	
	// 情况1：Circle 与 Circle
	if (c1 && c2) {
		overlap = c1->getRadius() + c2->getRadius() - distance;
	}
	// 情况2：AABB 与 AABB
	else if (a1 && a2) {
		double overlapX = (a1->getWidth() + a2->getWidth()) / 2.0 - std::abs(x2 - x1);
		double overlapY = (a1->getHeight() + a2->getHeight()) / 2.0 - std::abs(y2 - y1);
		
		if (overlapX < overlapY) {
			overlap = overlapX;
			nx = (x2 > x1) ? 1.0 : -1.0;
			ny = 0.0;
		} else {
			overlap = overlapY;
			nx = 0.0;
			ny = (y2 > y1) ? 1.0 : -1.0;
		}
	}
	// 情况3：Circle 与 AABB
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
		
		if (distToClosest < 0.0001) {
			overlap = circle->getRadius();
		}
	}
	
	// 应用位置修正
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

/*=========================================================================================================
 * 边界碰撞处理函数
 *=========================================================================================================*/
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
			vx = 0.0;
			vy = 0.0;
			
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
			vx = 0.0;
			vy = 0.0;
			
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

/*=========================================================================================================
 * 边界检查函数
 *=========================================================================================================*/
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
