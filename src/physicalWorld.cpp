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
 * 
 * 参数说明：
 *   topShape    - 要放置在上方的形状
 *   bottomShape - 底部支撑的形状
 *   offsetX     - 水平偏移量（相对于底部形状中心，默认0表示居中对齐）
 * 
 * 功能：
 *   1. 计算底部形状的顶部位置
 *   2. 计算上方形状应该放置的位置（考虑形状类型和大小）
 *   3. 设置上方形状的摩擦系数与底部形状相同
 *   4. 支持在倾斜坐标系中的放置（考虑 inclineAngle）
 * 
 * 支持的形状组合：
 *   - Circle 在 Circle 上
 *   - Circle 在 AABB 上
 *   - AABB 在 Circle 上
 *   - AABB 在 AABB 上
 *   - Circle 在 Wall 上
 *   - AABB 在 Wall 上
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
	// 在倾斜坐标系中，"垂直"方向实际上是垂直于斜面的方向
	if (std::abs(inclineAngle) > 0.01) {
		// 将坐标转换到倾斜坐标系，然后再转换回标准坐标系
		const double PI = 3.14159265358979323846;
		double angleRad = inclineAngle * PI / 180.0;
		
		// 在倾斜坐标系中，垂直堆叠意味着沿着法向量方向
		// 法向量：(-sin(θ), cos(θ))
		double normalX = -std::sin(angleRad);
		double normalY = std::cos(angleRad);
		
		// 计算底部形状顶部到上方形状底部的距离
		double distance = centerToBottom * 2.0;  // 上方形状的高度/直径
		
		// 识别上方形状的类型以获取准确的尺寸
		const Circle* topCircle = dynamic_cast<const Circle*>(&topShape);
		const AABB* topAABB = dynamic_cast<const AABB*>(&topShape);
		
		if (topCircle) {
			distance = topCircle->getRadius();
		} else if (topAABB) {
			distance = topAABB->getHeight() / 2.0;
		}
		
		// 识别底部形状的类型
		const Circle* bottomCircle = dynamic_cast<const Circle*>(&bottomShape);
		const AABB* bottomAABB = dynamic_cast<const AABB*>(&bottomShape);
		const Wall* bottomWall = dynamic_cast<const Wall*>(&bottomShape);
		
		double bottomDistance = 0.0;
		if (bottomCircle) {
			bottomDistance = bottomCircle->getRadius();
		} else if (bottomAABB) {
			bottomDistance = bottomAABB->getHeight() / 2.0;
		} else if (bottomWall) {
			bottomDistance = bottomWall->getHeight() / 2.0;
		}
		
		// 计算总的垂直距离（沿法向量方向）
		double totalDistance = bottomDistance + distance + 0.01;  // 加上小间隙避免初始重叠
		
		// 沿法向量方向放置
		newX = bottomX + offsetX + totalDistance * normalX;
		newY = bottomY + totalDistance * normalY;
	} else {
		// 水平情况下，添加小间隙避免初始重叠
		newY += 0.01;
	}
	
	// 6. 设置上方形状的新位置
	topShape.setCentre(newX, newY);
	
	// 7. 可选：打印调试信息
	#ifdef DEBUG_PLACEMENT
	std::cout << "放置形状: " << topShape.getName() 
	          << " 在 " << bottomShape.getName() << " 上方" << std::endl;
	std::cout << "  位置: (" << newX << ", " << newY << ")" << std::endl;
	std::cout << "  偏移: " << offsetX << std::endl;
	std::cout << "  倾斜角度: " << inclineAngle << "°" << std::endl;
	#endif
}

void PhysicalWorld::start() {
	//std::cout << "物理世界开始运行，重力加速度为: " << gravity << " m/s²" << std::endl;
	isPaused = false;
}

/*=========================================================================================================
 * Pause() - 暂停物理模拟
 * 保存所有形状的当前状态，并设置暂停标志
 *=========================================================================================================*/
void PhysicalWorld::Pause() {
	if (!isPaused) {
		saveStates();
		isPaused = true;
		//std::cout << "物理模拟已暂停" << std::endl;
	}
}

/*=========================================================================================================
 * Continue() - 继续物理模拟
 * 清除暂停标志，恢复之前保存的状态
 *=========================================================================================================*/
void PhysicalWorld::Continue() {
	if (isPaused) {
		restoreStates();
		isPaused = false;
		//std::cout << "物理模拟继续运行" << std::endl;
	}
}

void PhysicalWorld::Stop() {
	//std::cout << "物理世界停止运行。" << std::endl;
	isPaused = false;
	savedStates.clear();
}

/*=========================================================================================================
 * saveStates() - 保存所有形状的状态
 * 遍历所有动态形状，保存它们的位置、速度、质量、支撑状态等信息
 *=========================================================================================================*/
void PhysicalWorld::saveStates() {
	savedStates.clear();
	
	for (auto& shape : dynamicShapeList) {
		ShapeState state;
		
		// 保存位置
		shape->getCentre(state.x, state.y);
		
		// 保存速度
		shape->getVelocity(state.vx, state.vy);
		
		// 保存质量
		state.mass = shape->getMass();
		
		// 保存支撑状态
		state.isSupported = shape->getIsSupported();
		state.supporter = shape->getSupporter();
		
		// 保存正压力
		shape->getNormalForce(state.normalForce[0], state.normalForce[1]);
		
		savedStates.push_back(state);
	}
}

/*=========================================================================================================
 * restoreStates() - 恢复所有形状的状态
 * 将之前保存的状态恢复到形状对象中
 *=========================================================================================================*/
void PhysicalWorld::restoreStates() {
	if (savedStates.size() != dynamicShapeList.size()) {
		std::cerr << "警告：保存的状态数量与当前形状数量不匹配" << std::endl;
		return;
	}
	
	for (size_t i = 0; i < dynamicShapeList.size(); i++) {
		Shape* shape = dynamicShapeList[i];
		const ShapeState& state = savedStates[i];
		
		// 恢复位置
		shape->setCentre(state.x, state.y);
		
		// 恢复速度
		shape->setVelocity(state.vx, state.vy);
		
		// 恢复质量（通常不需要，但保持一致性）
		shape->setMass(state.mass);
		
		// 恢复支撑状态
		shape->setIsSupported(state.isSupported);
		shape->setSupporter(state.supporter);
		
		// 恢复正压力
		shape->normalforce[0] = state.normalForce[0];
		shape->normalforce[1] = state.normalForce[1];
	}
}
/*=========================================================================================================
 * update() 函数 - 版本1：使用成员变量 timeStep
 * 简洁模式：自动使用内部时间步长
 *=========================================================================================================*/
void PhysicalWorld::update(std::vector<Shape*>& shapeList, const Ground& ground) {
	// 如果暂停，直接返回，不执行任何更新
	if (isPaused) {
		return;
	}
	
	// 委托到版本2，使用成员变量 timeStep
	update(shapeList, timeStep, ground);
}

/*=========================================================================================================
 * update() 函数 - 版本2：使用参数 deltaTime
 * 灵活模式：可以临时指定时间步长
 *=========================================================================================================*/
void PhysicalWorld::update(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground) {
	// 如果暂停，直接返回，不执行任何更新
	if (isPaused) {
		return;
	}
	
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
    
    // 处理与静态物体的碰撞
    for (auto& dyn : shapeList) {
        for (auto& stat : staticShapeList) {
            // 碰撞检测
            if (dyn->check_collision(*stat)) {
                // 解决碰撞 (静态物体质量无穷大，resolveCollision应该能处理)
                // 注意：resolveCollision 假设两个物体都是 DynamicShape? 
                // 不，它使用 Shape&，并检查 mass。
                // 但是如果 stat 是 Wall，resolveCollision 可能不够精确（使用一维弹性碰撞）。
                // Wall 有专门的 resolveCollisionWithWall。
                
                const Wall* wall = dynamic_cast<const Wall*>(stat);
                if (wall) {
                    resolveCollisionWithWall(*dyn, *wall);
                } else {
                    // 对于其他静态物体（如 Slope），使用通用解决
                    resolveCollision(*dyn, *stat);
                }
            }
        }
    }
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
	
	// 计算自身在垂直于斜面方向的重力分量
	const double PI = 3.14159265358979323846;
	double angleRad = inclineAngle * PI / 180.0;
	double totalWeight = shape->getMass() * gravity * std::cos(angleRad);
	
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
	
	// 2.5. 计算沿斜面的重力分量（驱动力），但先不施加
	const double PI = 3.14159265358979323846;
	double angleRad = inclineAngle * PI / 180.0;
    
    // 如果支撑物是斜坡，叠加其角度
    if (shape->getSupporter()) {
        const Slope* slopeSup = dynamic_cast<const Slope*>(shape->getSupporter());
        if (slopeSup) {
            // 注意：Slope的angle通常是正值（如果斜坡左高右低？或者...）
            // 假设Slope angle是与水平面的夹角。
            // 如果物体在斜坡上，重力沿斜面的分量是 mg * sin(angle)
            angleRad += slopeSup->getAngle();
        }
    }

	double drivingForce = shape->getMass() * gravity * std::sin(angleRad);
	
	// 3. 施加摩擦力（传入即将施加的驱动力）
	double friction = 0.0;
    double static_friction = 0.0;
	
	if (shape->HasCollidedWithGround(ground.getYLevel())) {
		// 在地面上：使用正确的正压力
		friction = ground.getFriction();
        static_friction = ground.getStaticFriction();
		
		double fx, fy;
		shape->getNormalForce(fx, fy);
		
		double normalForce = std::abs(fy);
		
        // 新的摩擦力逻辑：传入即将施加的驱动力
        shape->applyFriction(normalForce, friction, static_friction, drivingForce);
		
	} else if (shape->getSupporter() != nullptr) {
		// 在其他物体上
		Shape* supporter = shape->getSupporter();
		supporter->getFraction(friction);
        supporter->getStaticFraction(static_friction);
		
		double fx, fy;
		shape->getNormalForce(fx, fy);
		
		double normalForce = std::abs(fy);
		
		// 获取支撑物速度
		double supVx, supVy;
		supporter->getVelocity(supVx, supVy);
		
		// 使用相对速度摩擦力：传入即将施加的驱动力
		applyFrictionOnSupporter(shape, supporter, normalForce, friction, static_friction, drivingForce);
	}
	
	// 4. 最后施加沿斜面的重力分量（驱动力）
	shape->addToTotalForce(drivingForce, 0.0);
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
void PhysicalWorld::applyFrictionOnSupporter(Shape* shape, Shape* supporter, double normalForce, double friction, double static_friction, double drivingForce) {
	// 获取支撑物速度
	double supVx, supVy;
	supporter->getVelocity(supVx, supVy);
	
	// ✅ 使用 Shape::applyFrictionRelative() 计算相对摩擦力
	shape->applyFrictionRelative(normalForce, friction, static_friction, supVx, supVy, drivingForce);
	
	// ✅ 计算反作用力（牛顿第三定律）
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
		supporter->addToTotalForce(-frictionMag * dirX, 0.0);
	}
}

/*=========================================================================================================
 * 碰撞检测和处理函数 - 检测并处理所有物体之间的碰撞
 *=========================================================================================================*/
void PhysicalWorld::handleAllCollisions(std::vector<Shape*>& shapeList) {
	const double MAX_INTERACTION_DISTANCE = 200.0;
	//请增加详细注释
	
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

/*=========================================================================================================
 * 形状管理方法实现 - 添加形状
 *=========================================================================================================*/
void PhysicalWorld::addDynamicShape(Shape* shape) {
	if (shape != nullptr) {
		dynamicShapeList.push_back(shape);
	}
}

void PhysicalWorld::addStaticShape(Shape* shape) {
	if (shape != nullptr) {
		staticShapeList.push_back(shape);
	}
}

/*=========================================================================================================
 * 形状管理方法实现 - 移除形状
 *=========================================================================================================*/
void PhysicalWorld::removeDynamicShape(Shape* shape) {
	auto it = std::find(dynamicShapeList.begin(), dynamicShapeList.end(), shape);
	if (it != dynamicShapeList.end()) {
		dynamicShapeList.erase(it);
	}
}

void PhysicalWorld::removeStaticShape(Shape* shape) {
	auto it = std::find(staticShapeList.begin(), staticShapeList.end(), shape);
	if (it != staticShapeList.end()) {
		staticShapeList.erase(it);
	}
}

/*=========================================================================================================
 * 形状管理方法实现 - 通过名称查找形状
 *=========================================================================================================*/
Shape* PhysicalWorld::findShapeByName(const std::string& name) {
	// 首先在动态形状中查找
	Shape* result = findDynamicShapeByName(name);
	if (result != nullptr) {
		return result;
	}
	// 如果没找到，在静态形状中查找
	return findStaticShapeByName(name);
}

Shape* PhysicalWorld::findDynamicShapeByName(const std::string& name) {
	for (auto& shape : dynamicShapeList) {
		if (shape->getName() == name) {
			return shape;
		}
	}
	return nullptr;
}

Shape* PhysicalWorld::findStaticShapeByName(const std::string& name) {
	for (auto& shape : staticShapeList) {
		if (shape->getName() == name) {
			return shape;
		}
	}
	return nullptr;
}

/*=========================================================================================================
 * 形状管理方法实现 - 通过类型查找所有形状
 *=========================================================================================================*/
std::vector<Shape*> PhysicalWorld::findShapesByType(const std::string& type) {
	std::vector<Shape*> result;
	
	// 在动态形状中查找
	std::vector<Shape*> dynamicResult = findDynamicShapesByType(type);
	result.insert(result.end(), dynamicResult.begin(), dynamicResult.end());
	
	// 在静态形状中查找
	std::vector<Shape*> staticResult = findStaticShapesByType(type);
	result.insert(result.end(), staticResult.begin(), staticResult.end());
	
	return result;
}

std::vector<Shape*> PhysicalWorld::findDynamicShapesByType(const std::string& type) {
	std::vector<Shape*> result;
	for (auto& shape : dynamicShapeList) {
		if (shape->getType() == type) {
			result.push_back(shape);
		}
	}
	return result;
}

std::vector<Shape*> PhysicalWorld::findStaticShapesByType(const std::string& type) {
	std::vector<Shape*> result;
	for (auto& shape : staticShapeList) {
		if (shape->getType() == type) {
			result.push_back(shape);
		}
	}
	return result;
}

/*=========================================================================================================
 * 形状管理方法实现 - 清空所有形状
 *=========================================================================================================*/
void PhysicalWorld::clearDynamicShapes() {
	dynamicShapeList.clear();
}

void PhysicalWorld::clearStaticShapes() {
	staticShapeList.clear();
}

void PhysicalWorld::clearAllShapes() {
	clearDynamicShapes();
	clearStaticShapes();
}

/*=========================================================================================================
 * 形状管理方法实现 - 打印所有形状信息（调试用）
 *=========================================================================================================*/
void PhysicalWorld::printAllShapes() const {
	std::cout << "========== 物理世界中的所有形状 ==========" << std::endl;
	std::cout << "动态形状数量: " << dynamicShapeList.size() << std::endl;
	std::cout << "静态形状数量: " << staticShapeList.size() << std::endl;
	std::cout << "总形状数量: " << getTotalShapeCount() << std::endl;
	std::cout << std::endl;
	
	printDynamicShapes();
	std::cout << std::endl;
	printStaticShapes();
}

void PhysicalWorld::printDynamicShapes() const {
	std::cout << "========== 动态形状列表 ==========" << std::endl;
	for (size_t i = 0; i < dynamicShapeList.size(); i++) {
		Shape* shape = dynamicShapeList[i];
		double x, y, vx, vy;
		shape->getCentre(x, y);
		shape->getVelocity(vx, vy);
		
		std::cout << "[" << i << "] "
		          << "名称: " << shape->getName() << ", "
		          << "类型: " << shape->getType() << ", "
		          << "位置: (" << x << ", " << y << "), "
		          << "速度: (" << vx << ", " << vy << "), "
		          << "质量: " << shape->getMass()
		          << std::endl;
	}
}

void PhysicalWorld::printStaticShapes() const {
	std::cout << "========== 静态形状列表 ==========" << std::endl;
	for (size_t i = 0; i < staticShapeList.size(); i++) {
		Shape* shape = staticShapeList[i];
		double x, y;
		shape->getCentre(x, y);
		
		std::cout << "[" << i << "] "
		          << "名称: " << shape->getName() << ", "
		          << "类型: " << shape->getType() << ", "
		          << "位置: (" << x << ", " << y << "), "
		          << "质量: " << shape->getMass()
		          << std::endl;
	}
}

/*=========================================================================================================
 * 私有方法：根据类型字符串识别并返回标准化的类型名称
 * 
 * 参数说明：
 *   type - 输入的类型字符串（不区分大小写）
 * 
 * 返回值：
 *   "Circle" - 如果是圆形类型
 *   "AABB"   - 如果是矩形/盒子类型
 *   ""       - 如果类型无法识别
 *=========================================================================================================*/
std::string PhysicalWorld::parseShapeType(const std::string& type) const {
	// 转换为小写进行比较
	std::string lowerType = type;
	std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
	
	// 识别圆形类型
	if (lowerType == "circle") {
		return "Circle";
	}
	// 识别矩形/盒子类型
	else if (lowerType == "aabb" || lowerType == "box" || lowerType == "rectangle" || lowerType == "rect") {
		return "AABB";
	}
	
	// 未识别的类型
	return "";
}

/*=========================================================================================================
 * 私有方法：根据类型和参数创建形状对象
 *=========================================================================================================*/
Shape* PhysicalWorld::createShape(const std::string& type, const std::string& name,
                                   double mass, double size1, double size2, bool isDynamic) {
	Shape* shape = nullptr;
	
	// 使用 parseShapeType 识别类型
	std::string standardType = parseShapeType(type);
	
	if (standardType == "Circle") {
		// Circle: size1 = 半径
		if (isDynamic) {
			shape = new Circle(mass, size1, 0.0, 0.0);
		} else {
			Circle* circle = new Circle(mass, size1, 0.0, 0.0);
			circle->setMass(INFINITY);  // 静态形状质量为无穷大
			shape = circle;
		}
	}
	else if (standardType == "AABB") {
		// AABB: size1 = 宽度, size2 = 高度
		if (size2 == 0.0) {
			size2 = size1;  // 如果没有提供高度，默认为正方形
		}
		if (isDynamic) {
			shape = new AABB(mass, size1, size2, 0.0, 0.0);
		} else {
			AABB* box = new AABB(mass, size1, size2, 0.0, 0.0);
			box->setMass(INFINITY);  // 静态形状质量为无穷大
			shape = box;
		}
	}
	
	if (shape != nullptr) {
		// 设置名称（如果为空则自动生成）
		if (name.empty()) {
			shape->setName(generateUniqueName(standardType));
		} else {
			shape->setName(name);
		}
	}
	
	return shape;
}

/*=========================================================================================================
 * 私有方法：生成唯一名字
 *=========================================================================================================*/
std::string PhysicalWorld::generateUniqueName(const std::string& type) {
	int counter = 1;
	std::string baseName = type;
	std::string uniqueName;
	
	do {
		uniqueName = baseName + "_" + std::to_string(counter);
		counter++;
	} while (isNameExists(uniqueName));
	
	return uniqueName;
}

/*=========================================================================================================
 * 私有方法：检查名字是否已存在
 *=========================================================================================================*/
bool PhysicalWorld::isNameExists(const std::string& name) const {
	// 在动态形状中查找
	for (const auto& shape : dynamicShapeList) {
		if (shape->getName() == name) {
			return true;
		}
	}
	
	// 在静态形状中查找
	for (const auto& shape : staticShapeList) {
		if (shape->getName() == name) {
			return true;
		}
	}
	
	return false;
}

/*=========================================================================================================
 * 公共方法：通过类型创建并放置动态形状
 *=========================================================================================================*/
Shape* PhysicalWorld::placeDynamicShapeByType(const std::string& type, const std::string& name,
                                                double x_pos, double y_pos,
                                                double mass, double size1, double size2) {
	// 创建形状
	Shape* shape = createShape(type, name, mass, size1, size2, true);
	
	if (shape == nullptr) {
		std::cerr << "错误：无法创建类型为 '" << type << "' 的形状" << std::endl;
		return nullptr;
	}
	
	// 添加到动态形状列表
	addDynamicShape(shape);
	
	// 放置形状
	placeDynamicShape(*shape, x_pos, y_pos);
	
	return shape;
}

/*=========================================================================================================
 * 公共方法：通过类型创建并放置静态形状
 *=========================================================================================================*/
Shape* PhysicalWorld::placeStaticShapeByType(const std::string& type, const std::string& name,
                                               double x_pos, double y_pos,
                                               double mass, double size1, double size2) {
	// 创建形状
	Shape* shape = createShape(type, name, mass, size1, size2, false);
	
	if (shape == nullptr) {
		std::cerr << "错误：无法创建类型为 '" << type << "' 的形状" << std::endl;
		return nullptr;
	}
	
	// 添加到静态形状列表
	addStaticShape(shape);
	
	// 放置形状
	placeStaticShape(*shape, x_pos, y_pos);
	
	return shape;
}

/*=========================================================================================================
 * 公共方法：创建并放置墙壁（静态形状）
 * 
 * 参数说明：
 *   name     - 墙壁的名称（如果为空则自动生成唯一名称）
 *   x_pos    - 墙壁中心的x坐标
 *   y_pos    - 墙壁中心的y坐标
 *   width    - 墙壁的宽度
 *   height   - 墙壁的高度
 *   friction - 墙壁的摩擦系数（默认为0.0）
 * 
 * 返回值：
 *   指向新创建的Wall对象的指针，如果失败则返回nullptr
 *=========================================================================================================*/
Wall* PhysicalWorld::placeWall(const std::string& name, double x_pos, double y_pos,
                                 double width, double height, double friction) {
	// 创建墙壁对象
	Wall* wall = new Wall(width, height, x_pos, y_pos, friction);
	
	if (wall == nullptr) {
		std::cerr << "错误：无法创建墙壁" << std::endl;
		return nullptr;
	}
	
	// 设置名称（如果为空则自动生成）
	if (name.empty()) {
		wall->setName(generateUniqueName("Wall"));
	} else {
		wall->setName(name);
	}
	
	// 墙壁是静态形状，质量设为无穷大
	wall->setMass(INFINITY);
	
	// 添加到静态形状列表
	addStaticShape(wall);
	
	return wall;
}

/*=========================================================================================================
 * 公共方法：处理动态物体与 Wall 的碰撞
 * 
 * 参数说明：
 *   dynamicShape - 动态形状对象
 *   wall         - 墙壁对象
 * 
 * 功能：
 *   检测动态物体与墙壁的碰撞，如果发生碰撞则调用解决碰撞的方法
 *=========================================================================================================*/
void PhysicalWorld::handleWallCollision(Shape& dynamicShape, const Wall& wall) {
	// 检查碰撞（使用Shape基类的check_collision方法）
	if (dynamicShape.check_collision(wall)) {
		// 解决碰撞
		resolveCollisionWithWall(dynamicShape, wall);
	}
}

/*=========================================================================================================
 * 私有方法：解决动态物体与墙壁的碰撞
 * 
 * 参数说明：
 *   dynamicShape - 动态形状对象
 *   wall         - 墙壁对象
 * 
 * 功能：
 *   计算碰撞响应，应用速度变化和位置修正，使物体从墙壁中分离
 *=========================================================================================================*/
void PhysicalWorld::resolveCollisionWithWall(Shape& dynamicShape, const Wall& wall) {
	// 获取动态物体的位置和速度
	double shapeX, shapeY, shapeVx, shapeVy;
	dynamicShape.getCentre(shapeX, shapeY);
	dynamicShape.getVelocity(shapeVx, shapeVy);
	
	// 获取墙壁的位置
	double wallX, wallY;
	wall.getCentre(wallX, wallY);
	
	// 获取动态物体的弹性系数
	double restitution;
	dynamicShape.getRestitution(restitution);
	
	// 计算碰撞法向量（从墙壁指向动态物体）
	double nx = shapeX - wallX;
	double ny = shapeY - wallY;
	double distance = std::sqrt(nx * nx + ny * ny);
	
	if (distance < 0.0001) {
		// 如果物体中心与墙壁中心重合，使用默认法向量
		nx = 0.0;
		ny = 1.0;
		distance = 1.0;
	} else {
		// 归一化法向量
		nx /= distance;
		ny /= distance;
	}
	
	// 计算速度在法向上的分量
	double vn = shapeVx * nx + shapeVy * ny;
	
	// 如果物体正在远离墙壁，不处理碰撞
	if (vn > 0) {
		return;
	}
	
	// 计算反弹后的速度（墙壁质量无穷大，只有动态物体速度改变）
	double vn_new = -vn * restitution;
	
	// 更新速度（法向速度反弹，切向速度保持）
	shapeVx = shapeVx - vn * nx + vn_new * nx;
	shapeVy = shapeVy - vn * ny + vn_new * ny;
	
	dynamicShape.setVelocity(shapeVx, shapeVy);
	
	// ========== 分离物体，避免重叠 ==========
	double overlap = 0.0;
	
	// 识别动态物体的类型并计算重叠量
	const Circle* circle = dynamic_cast<const Circle*>(&dynamicShape);
	const AABB* aabb = dynamic_cast<const AABB*>(&dynamicShape);
	
	if (circle) {
		// 圆形与墙壁（矩形）碰撞
		double circleX, circleY;
		circle->getCentre(circleX, circleY);
		
		// 找到墙壁上离圆心最近的点
		double closestX = std::max(wall.getLeft(), std::min(circleX, wall.getRight()));
		double closestY = std::max(wall.getBottom(), std::min(circleY, wall.getTop()));
		
		// 计算圆心到最近点的距离
		double dx = circleX - closestX;
		double dy = circleY - closestY;
		double distToClosest = std::sqrt(dx * dx + dy * dy);
		
		// 计算重叠量
		overlap = circle->getRadius() - distToClosest;
		
		// 如果圆心在墙壁内部，使用不同的法向量
		if (distToClosest < 0.0001) {
			overlap = circle->getRadius();
		}
	}
	else if (aabb) {
		// 矩形与墙壁（矩形）碰撞
		double overlapX = (aabb->getWidth() + wall.getWidth()) / 2.0 - std::abs(shapeX - wallX);
		double overlapY = (aabb->getHeight() + wall.getHeight()) / 2.0 - std::abs(shapeY - wallY);
		
		// 选择较小的重叠量作为分离方向
		if (overlapX < overlapY) {
			overlap = overlapX;
			nx = (shapeX > wallX) ? 1.0 : -1.0;
			ny = 0.0;
		} else {
			overlap = overlapY;
			nx = 0.0;
			ny = (shapeY > wallY) ? 1.0 : -1.0;
		}
	}
	
	// 应用位置修正（只修正动态物体）
	if (overlap > 0) {
		const double separationPercent = 0.8;
		double correctionX = overlap * separationPercent * nx;
		double correctionY = overlap * separationPercent * ny;
		
		dynamicShape.setCentre(shapeX + correctionX, shapeY + correctionY);
	}
}

/*=========================================================================================================
 * 施加沿斜面的重力分量（驱动力）
 * 
 * 参数说明：
 *   shape        - 要施加力的形状对象
 *   inclineAngle - 倾斜角度（角度制，-90 到 90）
 * 
 * 功能：
 *   计算并施加重力在斜面方向的分量：F = m * g * sin(θ)
 *   - 当角度为0时（水平），sin(0)=0，不施加力
 *   - 当角度为正时，力沿正x方向
 *   - 当角度为负时，力沿负x方向
 *=========================================================================================================*/
void PhysicalWorld::applyGravitySin(Shape* shape, double inclineAngle) {
	// 将角度转换为弧度
	const double PI = 3.14159265358979323846;
	double angleRad = inclineAngle * PI / 180.0;
	
	// 计算沿斜面的重力分量：F = m * g * sin(θ)
	double forceMagnitude = shape->getMass() * gravity * std::sin(angleRad);
	
	// 施加力（沿x方向，累加到合力）
	// 正角度：力指向正x方向
	// 负角度：力指向负x方向
	shape->addToTotalForce(forceMagnitude, 0.0);
}

/*=========================================================================================================
 * 坐标转换：倾斜坐标系转标准坐标系
 * 
 * 参数说明：
 *   x_rel - 倾斜坐标系中的x坐标（沿斜面方向）
 *   y_rel - 倾斜坐标系中的y坐标（垂直斜面方向）
 * 
 * 返回值：
 *   vector<double> {x_standard, y_standard} - 标准坐标系中的坐标
 * 
 * 功能：
 *   将倾斜坐标系中的相对坐标转换为标准（水平-垂直）坐标系
 *   使用旋转矩阵进行坐标变换
 *=========================================================================================================*/
std::vector<double> PhysicalWorld::inclineToStandard(double x_rel, double y_rel) const {
	// 将角度转换为弧度
	const double PI = 3.14159265358979323846;
	double angleRad = inclineAngle * PI / 180.0;
	
	// 旋转矩阵变换
	// x_standard = x_rel * cos(θ) - y_rel * sin(θ)
	// y_standard = x_rel * sin(θ) + y_rel * cos(θ)
	double cosAngle = std::cos(angleRad);
	double sinAngle = std::sin(angleRad);
	
	double x_standard = x_rel * cosAngle - y_rel * sinAngle;
	double y_standard = x_rel * sinAngle + y_rel * cosAngle;
	
	return {x_standard, y_standard};
}
