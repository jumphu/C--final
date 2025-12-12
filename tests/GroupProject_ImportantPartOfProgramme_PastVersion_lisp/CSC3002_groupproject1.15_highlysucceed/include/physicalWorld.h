#ifndef _PHYSICALWORLD_H_
#define _PHYSICALWORLD_H_

#include <vector>
#include <string>
#include "shapes.h"

struct PhysicalWorld {
public:
	// ٶ
	double gravity;

	// ֱбʱѹ㣩
	double gravity_vertical;
	
	// ϵбǶȣǶƣ-90 to 90, 0ʾˮƽ
	double inclineAngle;

	//Ħ,ĦڻĦ
	double StaticFriction;
	
	// Ĭʱ䲽룩
	double timeStep;
	
	// ı߽ [left, right, bottom, top]
	double bounds[4];
	
	// еĶ̬״бӰ죩
	std::vector<Shape*> dynamicShapeList;
	
	// еľ̬״бӰ죩
	std::vector<Shape*> staticShapeList;
	
	Ground ground;  // ĵ
	
	// 캯ĬΪ9.8Ĭʱ䲽Ϊ1/60루60 FPSĬϱ߽Ϊ [-1000, 1000, -1000, 1000]
	PhysicalWorld() : gravity(9.8), gravity_vertical(9.8), inclineAngle(0.0), timeStep(1.0/60.0), bounds{-1000.0, 1000.0, -1000.0, 1000.0}, isPaused(false) {}
	
	// ߽Ĺ캯
	PhysicalWorld(double left, double right, double bottom, double top) 
		: gravity(9.8), gravity_vertical(9.8), inclineAngle(0.0), timeStep(1.0/60.0), bounds{left, right, bottom, top}, isPaused(false) {}
	
	// 
	~PhysicalWorld() {}
	
	// ٶ
	void setGravity(double g) {
		gravity = g;
		// ͬʱ´ֱбǶȣ
		const double PI = 3.14159265358979323846;
		double angleRad = inclineAngle * PI / 180.0;
		gravity_vertical = gravity * std::cos(angleRad);
	}
	
	// ȡǰٶ
	double getGravity() const {
		return gravity;
	}
	
	// бǶȣǶƣ-90  90
	void setInclineAngle(double angle) {
		if (angle >= -90.0 && angle <= 90.0) {
			inclineAngle = angle;
			// ͬʱ´ֱ
			const double PI = 3.14159265358979323846;
			double angleRad = inclineAngle * PI / 180.0;
			gravity_vertical = gravity * std::cos(angleRad);
		}
	}
	
	// ȡǰбǶ
	double getInclineAngle() const {
		return inclineAngle;
	}
	
	// ʱ䲽
	void setTimeStep(double dt) {
		if (dt > 0.0) {
			timeStep = dt;
		}
	}
	
	// ȡǰʱ䲽
	double getTimeStep() const {
		return timeStep;
	}
	
	// ñ߽
	void setBounds(double left, double right, double bottom, double top) {
		bounds[0] = left;
		bounds[1] = right;
		bounds[2] = bottom;
		bounds[3] = top;
	}
	
	// ȡ߽
	void getBounds(double& left, double& right, double& bottom, double& top) const {
		left = bounds[0];
		right = bounds[1];
		bottom = bounds[2];
		top = bounds[3];
	}



	// ========== ״̬ ==========
	void start();
	void Pause();      // ͣģ
	void Continue();   // ģ
	void Stop();
	
	// ȡͣ״̬
	bool getIsPaused() const { return isPaused; }

	// ========== · ==========
	// update()  - ذ汾
	void update(std::vector<Shape*>& shapeList, const Ground& ground);  // ʹóԱ timeStep
	void update(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground);  // ʹò deltaTime

	// ========== ״ ==========
	// ״
	void addDynamicShape(Shape* shape);
	void addStaticShape(Shape* shape);
	
	// Ƴ״
	void removeDynamicShape(Shape* shape);
	void removeStaticShape(Shape* shape);
	
	// ͨƲ״
	Shape* findShapeByName(const std::string& name);
	Shape* findDynamicShapeByName(const std::string& name);
	Shape* findStaticShapeByName(const std::string& name);
	
	// ͨͲ״
	std::vector<Shape*> findShapesByType(const std::string& type);
	std::vector<Shape*> findDynamicShapesByType(const std::string& type);
	std::vector<Shape*> findStaticShapesByType(const std::string& type);
	
	// ȡ״
	size_t getDynamicShapeCount() const { return dynamicShapeList.size(); }
	size_t getStaticShapeCount() const { return staticShapeList.size(); }
	size_t getTotalShapeCount() const { return dynamicShapeList.size() + staticShapeList.size(); }
	
	// ״
	void clearDynamicShapes();
	void clearStaticShapes();
	void clearAllShapes();
	
	// ӡ״Ϣã
	void printAllShapes() const;
	void printDynamicShapes() const;
	void printStaticShapes() const;

	// ========== 򻯵ķӿ ==========
	// ֱͨͺƴö̬״
	Shape* placeDynamicShapeByType(const std::string& type, const std::string& name, 
	                                 double x_pos, double y_pos, 
	                                 double mass, double size1, double size2 = 0.0);
	
	// ֱͨͺƴþ̬״
	Shape* placeStaticShapeByType(const std::string& type, const std::string& name,
	                                double x_pos, double y_pos,
	                                double mass, double size1, double size2 = 0.0);
	
	// ǽڣ̬״
	Wall* placeWall(const std::string& name, double x_pos, double y_pos,
	                 double width, double height, double friction = 0.0);
	
	// ̬ Wall ײ
	void handleWallCollision(Shape& dynamicShape, const Wall& wall);

	// ԭеķúݣ
	void placeDynamicShape(Shape& shape, double x_pos, double y_pos);
	void placeStaticShape(Shape& shape, double x_pos, double y_pos);
	void placeShapeOnGround(Shape& shape, const Ground& ground);
	
	// ========== ģͷ÷ ==========
	// һ״һ״Ϸ
	// 
	//   topShape - ҪϷ״
	//   bottomShape - ײ֧ŵ״
	//   offsetX - ˮƽƫڵײ״ģĬ0ʾж룩
	void placeShapeOnShape(Shape& topShape, Shape& bottomShape, double offsetX = 0.0);

	//==========бǶȲΪ0ʱҪбǶͶ䵽׼==========
	std::vector<double> inclineToStandard(double x_rel, double y_rel) const;

private:
	// ========== ͣ״̬ ==========
	bool isPaused;  // Ƿͣ
	
	// ״״̬ṹ
	struct ShapeState {
		double x, y;           // λ
		double vx, vy;         // ٶ
		double mass;           // 
		bool isSupported;      // ֧״̬
		Shape* supporter;      // ֧
		double normalForce[2]; // ѹ
	};
	
	// ͣʱ״̬
	std::vector<ShapeState> savedStates;
	
	// ״̬ͻָ
	void saveStates();
	void restoreStates();

	// ========== update() ĸ׶ ==========
	// һ׶Σ֧״̬
	void resetSupportStates(std::vector<Shape*>& shapeList);
	
	// ڶ׶Σ֧Źϵ
	void detectSupportRelations(std::vector<Shape*>& shapeList, const Ground& ground);
	
	// ڶ׶Σѹۻ
	void calculateNormalForces(std::vector<Shape*>& shapeList);
	void calculateNormalForceRecursive(Shape* shape, std::vector<Shape*>& shapeList);
	
	// ׶Σ
	void updatePhysics(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground);
	
	// ׶εӲ裨ͳһ
	void handleSupportedShapeWithGravity(Shape* shape, double deltaTime, const Ground& ground);
	void handleAirborneShapeWithGravity(Shape* shape, double deltaTime);
	
	// ɵķݣʹã
	void handleSupportedShape(Shape* shape, double deltaTime, const Ground& ground);
	void handleAirborneShape(Shape* shape, double deltaTime);
	
	void applyFrictionOnSupporter(Shape* shape, Shape* supporter, double normalForce, double friction, double static_friction, double drivingForce);
	
	// Ľ׶Σײʹ
	void handleAllCollisions(std::vector<Shape*>& shapeList);
	void separateOverlappingShapes(Shape& shape1, Shape& shape2, double nx, double ny, double distance);
	
	// ײڲã
	void Collisions(Shape& shape1, Shape& shape2);
	void resolveCollision(Shape& shape1, Shape& shape2);
	void resolveCollisionWithWall(Shape& dynamicShape, const Wall& wall);  // ̬ǽײ
	
	// ߽ײ
	void handleBoundaryCollision(Shape& shape);
	bool isInBounds(const Shape& shape) const;
	bool checkBoundaryCollision(const Shape& shape) const;

	// ==========  ==========
	// ͡ƺͲ״
	Shape* createShape(const std::string& type, const std::string& name,
	                   double mass, double size1, double size2, bool isDynamic);
	
	// ַʶ𲢷ر׼
	std::string parseShapeType(const std::string& type) const;
	
	// Ψһ֣δṩʱ
	std::string generateUniqueName(const std::string& type);
	
	// ǷѴ
	bool isNameExists(const std::string& name) const;


	//===========б========
	// ͳһбϴֱˮƽ
	void applyInclineGravity(Shape* shape, double inclineAngle, const Ground& ground);
	
	// ɵĸϵapplyInclineGravityУݣ
	void applyGravitySin(Shape* shape, double inclineAngle);
};

#endif