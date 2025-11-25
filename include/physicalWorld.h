#ifndef _physicalWorld_h_
#define _physicalWorld_h_

#include <vector>
#include "shapes.h"

struct PhysicalWorld {
public:
	// 重力加速度
	double gravity;
	
	// 默认时间步长（秒）
	double timeStep;
	
	// 物理世界的边界 [left, right, bottom, top]
	double bounds[4];
	
	// 物理世界中的动态形状列表（会受重力影响）
	std::vector<Shape*> dynamicShapeList;
	
	// 物理世界中的静态形状列表（不受重力影响）
	std::vector<Shape*> staticShapeList;
	
	Ground ground;  // 物理世界的地面
	
	// 构造函数，默认重力为9.8，默认时间步长为1/60秒（60 FPS），默认边界为 [-1000, 1000, 0, 1000]
	PhysicalWorld() : gravity(9.8), timeStep(1.0/60.0), bounds{-1000.0, 1000.0, 1000.0, -1000.0} {}
	
	// 带边界的构造函数
	PhysicalWorld(double left, double right, double bottom, double top) 
		: gravity(9.8), timeStep(1.0/60.0), bounds{left, right, bottom, top} {}
	
	// 析构函数
	~PhysicalWorld() {}
	
	// 设置重力加速度
	void setGravity(double g) {
		gravity = g;
	}
	
	// 获取当前重力加速度
	double getGravity() const {
		return gravity;
	}
	
	// 设置时间步长
	void setTimeStep(double dt) {
		if (dt > 0.0) {
			timeStep = dt;
		}
	}
	
	// 获取当前时间步长
	double getTimeStep() const {
		return timeStep;
	}
	
	// 设置边界
	void setBounds(double left, double right, double bottom, double top) {
		bounds[0] = left;
		bounds[1] = right;
		bounds[2] = bottom;
		bounds[3] = top;
	}
	
	// 获取边界
	void getBounds(double& left, double& right, double& bottom, double& top) const {
		left = bounds[0];
		right = bounds[1];
		bottom = bounds[2];
		top = bounds[3];
	}

	void placeShapeOnGround(Shape& shape, const Ground& ground);
	void start();
	
	// update() 函数 - 两个重载版本
	void update(std::vector<Shape*>& shapeList, const Ground& ground);  // 使用成员变量 timeStep
	void update(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground);  // 使用参数 deltaTime
	
	void Stop();
	
	// 碰撞处理方法 - public，以便测试
	void Collisions(Shape& shape1, Shape& shape2);  // 保留用于向后兼容
	void resolveCollision(Shape& shape1, Shape& shape2);  // 公开用于测试
	
	// 边界碰撞处理
	void handleBoundaryCollision(Shape& shape);

private:
	// ========== update() 函数的各个阶段 ==========
	// 第一阶段：重置支撑状态
	void resetSupportStates(std::vector<Shape*>& shapeList);
	
	// 第二阶段：检测支撑关系
	void detectSupportRelations(std::vector<Shape*>& shapeList, const Ground& ground);
	
	// 第二点五阶段：计算正压力（从上往下累积）
	void calculateNormalForces(std::vector<Shape*>& shapeList);
	void calculateNormalForceRecursive(Shape* shape, std::vector<Shape*>& shapeList);
	
	// 第三阶段：物理更新
	void updatePhysics(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground);
	
	// 第三阶段的子步骤
	void handleSupportedShape(Shape* shape, double deltaTime, const Ground& ground);
	void handleAirborneShape(Shape* shape, double deltaTime);
	void applyFrictionOnSupporter(Shape* shape, Shape* supporter, double normalForce, double friction);
	
	// 碰撞检测和处理
	void handleAllCollisions(std::vector<Shape*>& shapeList);
	void separateOverlappingShapes(Shape& shape1, Shape& shape2, double nx, double ny, double distance);
	
	// 检查形状是否在边界内
	bool isInBounds(const Shape& shape) const;
	
	// 检查形状是否与边界碰撞
	bool checkBoundaryCollision(const Shape& shape) const;
};

#endif