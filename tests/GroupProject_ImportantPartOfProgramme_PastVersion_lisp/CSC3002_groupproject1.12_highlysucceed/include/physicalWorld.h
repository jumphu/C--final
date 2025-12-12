#ifndef _PHYSICALWORLD_H_
#define _PHYSICALWORLD_H_

#include <vector>
#include <string>
#include "shapes.h"

struct PhysicalWorld {
public:
	// �������ٶ�
	double gravity;

	// ��ֱ���������������������бʱ����ѹ�����㣩
	double gravity_vertical;
	
	// ��������ϵ����б�Ƕȣ��Ƕ��ƣ�-90 to 90��, 0��ʾˮƽ
	double inclineAngle;

	//��Ħ����,���Ħ�������ڻ���Ħ����
	double StaticFriction;
	
	// Ĭ��ʱ�䲽�����룩
	double timeStep;
	
	// ��������ı߽� [left, right, bottom, top]
	double bounds[4];
	
	// ���������еĶ�̬��״�б�����������Ӱ�죩
	std::vector<Shape*> dynamicShapeList;
	
	// ���������еľ�̬��״�б�����������Ӱ�죩
	std::vector<Shape*> staticShapeList;
	
	Ground ground;  // ��������ĵ���
	
	// ���캯����Ĭ������Ϊ9.8��Ĭ��ʱ�䲽��Ϊ1/60�루60 FPS����Ĭ�ϱ߽�Ϊ [-1000, 1000, -1000, 1000]
	PhysicalWorld() : gravity(9.8), gravity_vertical(9.8), inclineAngle(0.0), timeStep(1.0/60.0), bounds{-1000.0, 1000.0, -1000.0, 1000.0}, isPaused(false) {}
	
	// ���߽�Ĺ��캯��
	PhysicalWorld(double left, double right, double bottom, double top) 
		: gravity(9.8), gravity_vertical(9.8), inclineAngle(0.0), timeStep(1.0/60.0), bounds{left, right, bottom, top}, isPaused(false) {}
	
	// ��������
	~PhysicalWorld() {}
	
	// �����������ٶ�
	void setGravity(double g) {
		gravity = g;
		// ͬʱ���´�ֱ������������б�Ƕȣ�
		const double PI = 3.14159265358979323846;
		double angleRad = inclineAngle * PI / 180.0;
		gravity_vertical = gravity * std::cos(angleRad);
	}
	
	// ��ȡ��ǰ�������ٶ�
	double getGravity() const {
		return gravity;
	}
	
	// ������б�Ƕȣ��Ƕ��ƣ�-90 �� 90��
	void setInclineAngle(double angle) {
		if (angle >= -90.0 && angle <= 90.0) {
			inclineAngle = angle;
			// ͬʱ���´�ֱ����
			const double PI = 3.14159265358979323846;
			double angleRad = inclineAngle * PI / 180.0;
			gravity_vertical = gravity * std::cos(angleRad);
		}
	}
	
	// ��ȡ��ǰ��б�Ƕ�
	double getInclineAngle() const {
		return inclineAngle;
	}
	
	// ����ʱ�䲽��
	void setTimeStep(double dt) {
		if (dt > 0.0) {
			timeStep = dt;
		}
	}
	
	// ��ȡ��ǰʱ�䲽��
	double getTimeStep() const {
		return timeStep;
	}
	
	// ���ñ߽�
	void setBounds(double left, double right, double bottom, double top) {
		bounds[0] = left;
		bounds[1] = right;
		bounds[2] = bottom;
		bounds[3] = top;
	}
	
	// ��ȡ�߽�
	void getBounds(double& left, double& right, double& bottom, double& top) const {
		left = bounds[0];
		right = bounds[1];
		bottom = bounds[2];
		top = bounds[3];
	}



	// ========== ״̬�������� ==========
	void start();
	void Pause();      // ��ͣ����ģ��
	void Continue();   // ��������ģ��
	void Stop();
	
	// ��ȡ��ͣ״̬
	bool getIsPaused() const { return isPaused; }

	// ========== �������·��� ==========
	// update() ���� - �������ذ汾
	void update(std::vector<Shape*>& shapeList, const Ground& ground);  // ʹ�ó�Ա���� timeStep
	void update(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground);  // ʹ�ò��� deltaTime

	// ========== ��״�������� ==========
	// ������״����������
	void addDynamicShape(Shape* shape);
	void addStaticShape(Shape* shape);
	
	// �Ƴ���״
	void removeDynamicShape(Shape* shape);
	void removeStaticShape(Shape* shape);
	
	// ͨ�����Ʋ�����״
	Shape* findShapeByName(const std::string& name);
	Shape* findDynamicShapeByName(const std::string& name);
	Shape* findStaticShapeByName(const std::string& name);
	
	// ͨ�����Ͳ���������״
	std::vector<Shape*> findShapesByType(const std::string& type);
	std::vector<Shape*> findDynamicShapesByType(const std::string& type);
	std::vector<Shape*> findStaticShapesByType(const std::string& type);
	
	// ��ȡ��״����
	size_t getDynamicShapeCount() const { return dynamicShapeList.size(); }
	size_t getStaticShapeCount() const { return staticShapeList.size(); }
	size_t getTotalShapeCount() const { return dynamicShapeList.size() + staticShapeList.size(); }
	
	// ���������״
	void clearDynamicShapes();
	void clearStaticShapes();
	void clearAllShapes();
	
	// ��ӡ������״��Ϣ�������ã�
	void printAllShapes() const;
	void printDynamicShapes() const;
	void printStaticShapes() const;

	// ========== �򻯵ķ�������ӿ� ==========
	// ֱ��ͨ�����ͺ����ƴ��������ö�̬��״
	Shape* placeDynamicShapeByType(const std::string& type, const std::string& name, 
	                                 double x_pos, double y_pos, 
	                                 double mass, double size1, double size2 = 0.0);
	
	// ֱ��ͨ�����ͺ����ƴ��������þ�̬��״
	Shape* placeStaticShapeByType(const std::string& type, const std::string& name,
	                                double x_pos, double y_pos,
	                                double mass, double size1, double size2 = 0.0);
	
	// ����������ǽ�ڣ���̬��״��
	Wall* placeWall(const std::string& name, double x_pos, double y_pos,
	                 double width, double height, double friction = 0.0);
	
	// ������̬������ Wall ����ײ
	// ... 之前的代码 ...
    
    // 处理动态物体与 Wall 的碰撞
    void handleWallCollision(Shape& dynamicShape, const Wall& wall);

    // 【新增】处理动态物体与 Slope 的碰撞
    void resolveCollisionWithSlope(Shape& dynamicShape, const Slope& slope);

    // ... 之后的代码 ...

	// ԭ�еķ��ú��������������ݣ�
	void placeDynamicShape(Shape& shape, double x_pos, double y_pos);
	void placeStaticShape(Shape& shape, double x_pos, double y_pos);
	void placeShapeOnGround(Shape& shape, const Ground& ground);
	
	// ========== ���ģ�ͷ��÷��� ==========
	// ��һ����״��������һ����״���Ϸ�
	// ������
	//   topShape - Ҫ�������Ϸ�����״
	//   bottomShape - �ײ�֧�ŵ���״
	//   offsetX - ˮƽƫ����������ڵײ���״���ģ�Ĭ��0��ʾ���ж��룩
	void placeShapeOnShape(Shape& topShape, Shape& bottomShape, double offsetX = 0.0);

	//==========����б�ǶȲ�Ϊ0ʱ��Ҫ����б��Ƕ������������Ͷ�䵽��׼�������==========
	std::vector<double> inclineToStandard(double x_rel, double y_rel) const;

private:
	// ========== ��ͣ״̬���� ==========
	bool isPaused;  // �Ƿ���ͣ
	
	// ��״״̬�ṹ
	struct ShapeState {
		double x, y;           // λ��
		double vx, vy;         // �ٶ�
		double mass;           // ����
		bool isSupported;      // ֧��״̬
		Shape* supporter;      // ֧����
		double normalForce[2]; // ��ѹ��
	};
	
	// ��ͣʱ�����״̬
	std::vector<ShapeState> savedStates;
	
	// ״̬����ͻָ�
	void saveStates();
	void restoreStates();

	// ========== update() �����ĸ����׶� ==========
	// ��һ�׶Σ�����֧��״̬
	void resetSupportStates(std::vector<Shape*>& shapeList);
	
	// �ڶ��׶Σ����֧�Ź�ϵ
	void detectSupportRelations(std::vector<Shape*>& shapeList, const Ground& ground);
	
	// �ڶ�����׶Σ�������ѹ�������������ۻ���
	void calculateNormalForces(std::vector<Shape*>& shapeList);
	void calculateNormalForceRecursive(Shape* shape, std::vector<Shape*>& shapeList);
	
	// �����׶Σ���������
	void updatePhysics(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground);
	
	// �����׶ε��Ӳ��裨ͳһ������������
	void handleSupportedShapeWithGravity(Shape* shape, double deltaTime, const Ground& ground);
	void handleAirborneShapeWithGravity(Shape* shape, double deltaTime);
	
	// �ɵķ��������������ݣ�������ʹ�ã�
	void handleSupportedShape(Shape* shape, double deltaTime, const Ground& ground);
	void handleAirborneShape(Shape* shape, double deltaTime);
	
	void applyFrictionOnSupporter(Shape* shape, Shape* supporter, double normalForce, double friction, double static_friction, double drivingForce);
	
	// ���Ľ׶Σ���ײ���ʹ���
	void handleAllCollisions(std::vector<Shape*>& shapeList);
	void separateOverlappingShapes(Shape& shape1, Shape& shape2, double nx, double ny, double distance);
	
	// ��ײ�������������������ڲ����ã�
	void Collisions(Shape& shape1, Shape& shape2);
	void resolveCollision(Shape& shape1, Shape& shape2);
	void resolveCollisionWithWall(Shape& dynamicShape, const Wall& wall);  // ��̬������ǽ����ײ
	
	// �߽���ײ����
	void handleBoundaryCollision(Shape& shape);
	bool isInBounds(const Shape& shape) const;
	bool checkBoundaryCollision(const Shape& shape) const;

	// ========== �������� ==========
	// �������͡����ƺͲ���������״����
	Shape* createShape(const std::string& type, const std::string& name,
	                   double mass, double size1, double size2, bool isDynamic);
	
	// ���������ַ���ʶ�𲢷��ر�׼������������
	std::string parseShapeType(const std::string& type) const;
	
	// ����Ψһ���֣���δ�ṩ����ʱ��
	std::string generateUniqueName(const std::string& type);
	
	// ��������Ƿ��Ѵ���
	bool isNameExists(const std::string& name) const;


	//===========������б���========
	// ͳһ����б�����������������ϴ�ֱ��ˮƽ������
	void applyInclineGravity(Shape* shape, double inclineAngle, const Ground& ground);
	
	// �ɵĸ��������������ϵ�applyInclineGravity�У��������������ݣ�
	void applyGravitySin(Shape* shape, double inclineAngle);
};

#endif