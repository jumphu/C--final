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

void PhysicalWorld::update(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground) {
	// ========== 第一阶段：物理更新 ==========
	for (auto& shape : shapeList) {
		// 应用重力
		shape->applyGravity(gravity);
		
		// 更新位置
		shape->update(deltaTime);
		
		// 检查与边界的碰撞
		handleBoundaryCollision(*shape);
		
		// 检查与地面的碰撞
		if (shape->HasCollidedWithGround(ground.getYLevel())) {
			// 简单处理：将形状放置在地面上，并将垂直速度设为0
			placeShapeOnGround(*shape, ground);
			double vx, vy;
			shape->getVelocity(vx, vy);
			
			// 应用摩擦力（在地面上时）
			double friction = ground.getFriction();
			vx *= (1.0 - friction * deltaTime); // 摩擦力随时间衰减
			
			// 应用弹性系数（与地面碰撞）
			double r;
			shape->getRestitution(r);
			vy = -vy * r; // 反弹，损失部分能量
			
			shape->setVelocity(vx, vy);
		}
	}

	// ========== 第二阶段：碰撞检测（优化版）==========
	// 优化1：粗略剔除 - 只检测可能碰撞的物体
	const double MAX_INTERACTION_DISTANCE = 200.0; // 最大交互距离
	
	for (size_t i = 0; i < shapeList.size(); i++) {
		for (size_t j = i + 1; j < shapeList.size(); j++) {
			// 优化：先做快速距离检查（避免昂贵的精确碰撞检测）
			double x1, y1, x2, y2;
			shapeList[i]->getCentre(x1, y1);
			shapeList[j]->getCentre(x2, y2);
			
			double dx = x2 - x1;
			double dy = y2 - y1;
			double distanceSquared = dx * dx + dy * dy;
			
			// 如果距离太远，直接跳过
			if (distanceSquared > MAX_INTERACTION_DISTANCE * MAX_INTERACTION_DISTANCE) {
				continue;
			}
			
			// 精确碰撞检测
			if (shapeList[i]->check_collision(*shapeList[j])) {
				Collisions(*shapeList[i], *shapeList[j]);
			}
		}
	}
}

void PhysicalWorld::Stop() {
	//std::cout << "物理世界停止运行。" << std::endl;
}

/*=========================================================================================================
 * 碰撞处理函数 - 处理两个形状之间的碰撞
 *=========================================================================================================*/
void PhysicalWorld::Collisions(Shape& shape1, Shape& shape2) {
	// 碰撞处理逻辑 - 2D弹性碰撞处理
	double m1 = shape1.getMass();
	double m2 = shape2.getMass();
	
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
	
	// 计算相对速度在法向量上的投影
	double relativeVelX = v2x - v1x;
	double relativeVelY = v2y - v1y;
	double velAlongNormal = relativeVelX * nx + relativeVelY * ny;
	
	// 如果物体正在分离，不处理
	if (velAlongNormal > 0) return;
	
	// 获取两个物体的弹性系数，取平均值
	double r1, r2;
	shape1.getRestitution(r1);
	shape2.getRestitution(r2);
	double restitution = (r1 + r2) / 2.0;
	
	// 获取摩擦系数
	double f1, f2;
	shape1.getFraction(f1);
	shape2.getFraction(f2);
	double friction = (f1 + f2) / 2.0;
	
	// 计算冲量标量
	double impulseScalar = -(1 + restitution) * velAlongNormal;
	impulseScalar /= (1.0 / m1 + 1.0 / m2);
	
	// 应用冲量
	double impulseX = impulseScalar * nx;
	double impulseY = impulseScalar * ny;
	
	shape1.setVelocity(v1x - impulseX / m1, v1y - impulseY / m1);
	shape2.setVelocity(v2x + impulseX / m2, v2y + impulseY / m2);
	
	// 应用摩擦力（切向）
	if (friction > 0.0) {
		// 计算切向量（垂直于法向量）
		double tx = -ny;
		double ty = nx;
		
		// 计算相对速度在切向上的投影
		double velAlongTangent = relativeVelX * tx + relativeVelY * ty;
		
		// 摩擦力冲量（有上限）
		double frictionImpulse = -friction * std::abs(impulseScalar) * velAlongTangent;
		frictionImpulse = std::max(-std::abs(impulseScalar), std::min(frictionImpulse, std::abs(impulseScalar)));
		
		double frictionX = frictionImpulse * tx;
		double frictionY = frictionImpulse * ty;
		
		// 应用摩擦力
		double v1x_new, v1y_new, v2x_new, v2y_new;
		shape1.getVelocity(v1x_new, v1y_new);
		shape2.getVelocity(v2x_new, v2y_new);
		
		shape1.setVelocity(v1x_new - frictionX / m1, v1y_new - frictionY / m1);
		shape2.setVelocity(v2x_new + frictionX / m2, v2y_new + frictionY / m2);
	}
	
	// ========== 分离物体，避免重叠 ==========
	const double separationPercent = 0.8; // 通常在 0.2 到 0.8 之间
	double overlap = 0.0;
	
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
		// 计算两个矩形在x和y方向的重叠
		double overlapX = (a1->getWidth() + a2->getWidth()) / 2.0 - std::abs(x2 - x1);
		double overlapY = (a1->getHeight() + a2->getHeight()) / 2.0 - std::abs(y2 - y1);
		
		// 选择较小的重叠作为分离方向
		if (overlapX < overlapY) {
			overlap = overlapX;
			// 调整法向量为x方向
			nx = (x2 > x1) ? 1.0 : -1.0;
			ny = 0.0;
		} else {
			overlap = overlapY;
			// 调整法向量为y方向
			nx = 0.0;
			ny = (y2 > y1) ? 1.0 : -1.0;
		}
	}
	// 情况3：Circle 与 AABB 或 AABB 与 Circle
	else if ((c1 && a2) || (a1 && c2)) {
		const Circle* circle = c1 ? c1 : c2;
		const AABB* aabb = a1 ? a1 : a2;
		
		// 获取圆心和矩形中心
		double circleX, circleY, aabbX, aabbY;
		circle->getCentre(circleX, circleY);
		aabb->getCentre(aabbX, aabbY);
		
		// 找到矩形上离圆心最近的点
		double closestX = std::max(aabbX - aabb->getWidth() / 2.0,
		                           std::min(circleX, aabbX + aabb->getWidth() / 2.0));
		double closestY = std::max(aabbY - aabb->getHeight() / 2.0,
		                           std::min(circleY, aabbY + aabb->getHeight() / 2.0));
		
		// 计算圆心到最近点的距离
		double dx = circleX - closestX;
		double dy = circleY - closestY;
		double distToClosest = std::sqrt(dx * dx + dy * dy);
		
		// 重叠量 = 半径 - 距离
		overlap = circle->getRadius() - distToClosest;
		
		// 如果圆心在矩形内部，需要特殊处理
		if (distToClosest < 0.0001) {
			// 使用质心连线作为法向量
			overlap = circle->getRadius();
		}
	}
	
	// 应用位置修正，分离重叠的物体
	if (overlap > 0) {
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
	
	// 尝试识别形状类型
	const Circle* circle = dynamic_cast<const Circle*>(&shape);
	const AABB* aabb = dynamic_cast<const AABB*>(&shape);
	
	// 对于圆形
	if (circle) {
		double radius = circle->getRadius();
		
		// 检查是否完全超出边界
		if (x + radius < bounds[0] || x - radius > bounds[1] ||
		    y + radius < bounds[2] || y - radius > bounds[3]) {
			// 完全超出边界，停止运动
			outOfBounds = true;
			vx = 0.0;
			vy = 0.0;
			
			// 将物体移回边界外的位置（保持在边界外侧）
			if (x + radius < bounds[0]) x = bounds[0] - radius;
			else if (x - radius > bounds[1]) x = bounds[1] + radius;
			
			if (y + radius < bounds[2]) y = bounds[2] - radius;
			else if (y - radius > bounds[3]) y = bounds[3] + radius;
		}
	}
	// 对于矩形
	else if (aabb) {
		double halfWidth = aabb->getWidth() / 2.0;
		double halfHeight = aabb->getHeight() / 2.0;
		
		// 检查是否完全超出边界
		if (x + halfWidth < bounds[0] || x - halfWidth > bounds[1] ||
		    y + halfHeight < bounds[2] || y - halfHeight > bounds[3]) {
			// 完全超出边界，停止运动
			outOfBounds = true;
			vx = 0.0;
			vy = 0.0;
			
			// 将物体移回边界外的位置（保持在边界外侧）
			if (x + halfWidth < bounds[0]) x = bounds[0] - halfWidth;
			else if (x - halfWidth > bounds[1]) x = bounds[1] + halfWidth;
			
			if (y + halfHeight < bounds[2]) y = bounds[2] - halfHeight;
			else if (y - halfHeight > bounds[3]) y = bounds[3] + halfHeight;
		}
	}
	
	// 只在完全超出边界时更新状态
	if (outOfBounds) {
		shape.setCentre(x, y);
		shape.setVelocity(vx, vy);
		
		// 可选：输出警告信息
		// std::cout << "警告：物体完全超出边界，已停止运动！" << std::endl;
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