#ifndef _PHYSICALWORLD_H_
#define _PHYSICALWORLD_H_

#include <vector>
#include <string>
#include "shapes.h"

struct PhysicalWorld {
public:
	// 重力加速度
	double gravity;

	// 垂直方向的重力分量（用于倾斜时的正压力计算）
	double gravity_vertical;
	
	// 整个坐标系的倾斜角度（角度制，-90 to 90）, 0表示水平
	double inclineAngle;

	//静摩擦力,最大静摩擦力等于滑动摩擦力
	double StaticFriction;
	
	// 默认时间步长（秒）
	double timeStep;
	
	// 物理世界的边界 [left, right, bottom, top]
	double bounds[4];
	
	// 物理世界中的动态形状列表（会受重力影响）
	std::vector<Shape*> dynamicShapeList;
	
	// 物理世界中的静态形状列表（不受重力影响）
	std::vector<Shape*> staticShapeList;
	
	Ground ground;  // 物理世界的地面
	
	// 构造函数，默认重力为9.8，默认时间步长为1/60秒（60 FPS），默认边界为 [-1000, 1000, -1000, 1000]
	PhysicalWorld() : gravity(9.8), gravity_vertical(9.8), inclineAngle(0.0), timeStep(1.0/60.0), bounds{-1000.0, 1000.0, -1000.0, 1000.0}, isPaused(false) {}
	
	// 带边界的构造函数
	PhysicalWorld(double left, double right, double bottom, double top) 
		: gravity(9.8), gravity_vertical(9.8), inclineAngle(0.0), timeStep(1.0/60.0), bounds{left, right, bottom, top}, isPaused(false) {}
	
	// 析构函数
	~PhysicalWorld() {}
	
	// 设置重力加速度
	void setGravity(double g) {
		gravity = g;
		// 同时更新垂直分量（考虑倾斜角度）
		const double PI = 3.14159265358979323846;
		double angleRad = inclineAngle * PI / 180.0;
		gravity_vertical = gravity * std::cos(angleRad);
	}
	
	// 获取当前重力加速度
	double getGravity() const {
		return gravity;
	}
	
	// 设置倾斜角度（角度制，-90 到 90）
	void setInclineAngle(double angle) {
		if (angle >= -90.0 && angle <= 90.0) {
			inclineAngle = angle;
			// 同时更新垂直分量
			const double PI = 3.14159265358979323846;
			double angleRad = inclineAngle * PI / 180.0;
			gravity_vertical = gravity * std::cos(angleRad);
		}
	}
	
	// 获取当前倾斜角度
	double getInclineAngle() const {
		return inclineAngle;
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



	// ========== 状态管理方法 ==========
	void start();
	void Pause();      // 暂停物理模拟
	void Continue();   // 继续物理模拟
	void Stop();
	
	// 获取暂停状态
	bool getIsPaused() const { return isPaused; }

	// ========== 物理更新方法 ==========
	// update() 函数 - 两个重载版本
	void update(std::vector<Shape*>& shapeList, const Ground& ground);  // 使用成员变量 timeStep
	void update(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground);  // 使用参数 deltaTime

	// ========== 形状管理方法 ==========
	// 添加形状到物理世界
	void addDynamicShape(Shape* shape);
	void addStaticShape(Shape* shape);
	
	// 移除形状
	void removeDynamicShape(Shape* shape);
	void removeStaticShape(Shape* shape);
	
	// 通过名称查找形状
	Shape* findShapeByName(const std::string& name);
	Shape* findDynamicShapeByName(const std::string& name);
	Shape* findStaticShapeByName(const std::string& name);
	
	// 通过类型查找所有形状
	std::vector<Shape*> findShapesByType(const std::string& type);
	std::vector<Shape*> findDynamicShapesByType(const std::string& type);
	std::vector<Shape*> findStaticShapesByType(const std::string& type);
	
	// 获取形状数量
	size_t getDynamicShapeCount() const { return dynamicShapeList.size(); }
	size_t getStaticShapeCount() const { return staticShapeList.size(); }
	size_t getTotalShapeCount() const { return dynamicShapeList.size() + staticShapeList.size(); }
	
	// 清空所有形状
	void clearDynamicShapes();
	void clearStaticShapes();
	void clearAllShapes();
	
	// 打印所有形状信息（调试用）
	void printAllShapes() const;
	void printDynamicShapes() const;
	void printStaticShapes() const;

	// ========== 简化的放置物体接口 ==========
	// 直接通过类型和名称创建并放置动态形状
	Shape* placeDynamicShapeByType(const std::string& type, const std::string& name, 
	                                 double x_pos, double y_pos, 
	                                 double mass, double size1, double size2 = 0.0);
	
	// 直接通过类型和名称创建并放置静态形状
	Shape* placeStaticShapeByType(const std::string& type, const std::string& name,
	                                double x_pos, double y_pos,
	                                double mass, double size1, double size2 = 0.0);
	
	// 创建并放置墙壁（静态形状）
	Wall* placeWall(const std::string& name, double x_pos, double y_pos,
	                 double width, double height, double friction = 0.0);
	
	// 处理动态物体与 Wall 的碰撞
	void handleWallCollision(Shape& dynamicShape, const Wall& wall);

	// 原有的放置函数（保持向后兼容）
	void placeDynamicShape(Shape& shape, double x_pos, double y_pos);
	void placeStaticShape(Shape& shape, double x_pos, double y_pos);
	void placeShapeOnGround(Shape& shape, const Ground& ground);
	
	// ========== 板块模型放置方法 ==========
	// 将一个形状放置在另一个形状的上方
	// 参数：
	//   topShape - 要放置在上方的形状
	//   bottomShape - 底部支撑的形状
	//   offsetX - 水平偏移量（相对于底部形状中心，默认0表示居中对齐）
	void placeShapeOnShape(Shape& topShape, Shape& bottomShape, double offsetX = 0.0);

	//==========当倾斜角度不为0时，要将按斜面角度正交相对坐标投射到标准相对坐标==========
	std::vector<double> inclineToStandard(double x_rel, double y_rel) const;

private:
	// ========== 暂停状态管理 ==========
	bool isPaused;  // 是否暂停
	
	// 形状状态结构
	struct ShapeState {
		double x, y;           // 位置
		double vx, vy;         // 速度
		double mass;           // 质量
		bool isSupported;      // 支撑状态
		Shape* supporter;      // 支撑物
		double normalForce[2]; // 正压力
	};
	
	// 暂停时保存的状态
	std::vector<ShapeState> savedStates;
	
	// 状态保存和恢复
	void saveStates();
	void restoreStates();

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
	
	// 第三阶段的子步骤（统一的重力处理）
	void handleSupportedShapeWithGravity(Shape* shape, double deltaTime, const Ground& ground);
	void handleAirborneShapeWithGravity(Shape* shape, double deltaTime);
	
	// 旧的方法保留（向后兼容，但不再使用）
	void handleSupportedShape(Shape* shape, double deltaTime, const Ground& ground);
	void handleAirborneShape(Shape* shape, double deltaTime);
	
	void applyFrictionOnSupporter(Shape* shape, Shape* supporter, double normalForce, double friction, double static_friction, double drivingForce);
	
	// 第四阶段：碰撞检测和处理
	void handleAllCollisions(std::vector<Shape*>& shapeList);
	void separateOverlappingShapes(Shape& shape1, Shape& shape2, double nx, double ny, double distance);
	
	// 碰撞处理方法（公开方法内部调用）
	void Collisions(Shape& shape1, Shape& shape2);
	void resolveCollision(Shape& shape1, Shape& shape2);
	void resolveCollisionWithWall(Shape& dynamicShape, const Wall& wall);  // 动态物体与墙壁碰撞
	
	// 边界碰撞处理
	void handleBoundaryCollision(Shape& shape);
	bool isInBounds(const Shape& shape) const;
	bool checkBoundaryCollision(const Shape& shape) const;

	// ========== 辅助方法 ==========
	// 根据类型、名称和参数创建形状对象
	Shape* createShape(const std::string& type, const std::string& name,
	                   double mass, double size1, double size2, bool isDynamic);
	
	// 根据类型字符串识别并返回标准化的类型名称
	std::string parseShapeType(const std::string& type) const;
	
	// 生成唯一名字（当未提供名字时）
	std::string generateUniqueName(const std::string& type);
	
	// 检查名字是否已存在
	bool isNameExists(const std::string& name) const;


	//===========处理倾斜情况========
	// 统一的倾斜重力处理函数（整合垂直和水平分量）
	void applyInclineGravity(Shape* shape, double inclineAngle, const Ground& ground);
	
	// 旧的辅助函数（已整合到applyInclineGravity中，保留用于向后兼容）
	void applyGravitySin(Shape* shape, double inclineAngle);
};

#endif