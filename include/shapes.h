#ifndef _shapes_h
#define _shapes_h

#include <iostream>
#include <cmath>
#include <string>

extern const double PI;

struct Shape {
public:
    
    std::string name;
    std::string type;
    
    double mass;
    double mass_centre[2]; // mass_centre[0]: x, mass_centre[1]: y
    double velocity[2];    // velocity[0]: vx, velocity[1]: vy
	double fraction;      // 动摩擦系数 (kinetic friction)
    double static_fraction; // 静摩擦系数 (static friction)
	double restitution = 1; // 恢复系数，默认值为1
    double totalforce[2];  // 合力累加器: totalforce[0]: fx, totalforce[1]: fy
    double normalforce[2]; // 给下方物体施加的弹力: normalforce[0]: fx, normalforce[1]: fy
    bool isSupported;      // 是否被支撑（是否在地面或其他物体上）
    Shape* supporter;      // 支撑物指针（记录是什么在支撑我）
	  
    /*Constructors:Shape()
    默认构造函数，质量为1，质心在原点,速度为0
    包含方法:move(), turn()，check_collision(),getMass(),getCentre(),getVelocity()等
    */
    
    // 默认构造函数：质量为1，质心在原点，速度为0，合力为0
    Shape() : name("Shape"), type("Shape"), mass(1.0), mass_centre{0.0, 0.0}, velocity{0.0, 0.0}, totalforce{0.0, 0.0}, normalforce{0.0, 0.0}, fraction(0.0), static_fraction(0.0), isSupported(false), supporter(nullptr) {}
    
    // 单参数构造函数：指定质量，质心在原点，速度为0，合力为0
    Shape(double m) : name("Shape"), type("Shape"), mass(m), mass_centre{0.0, 0.0}, velocity{0.0, 0.0}, totalforce{0.0, 0.0}, normalforce{0.0, 0.0}, fraction(0.0), static_fraction(0.0), isSupported(false), supporter(nullptr) {}
    
    // 三参数构造函数：指定质量和质心坐标，速度为0，合力为0
    Shape(double m, double x, double y) : name("Shape"), type("Shape"), mass(m), mass_centre{x, y}, velocity{0.0, 0.0}, totalforce{0.0, 0.0}, normalforce{0.0, 0.0}, fraction(0.0), static_fraction(0.0), isSupported(false), supporter(nullptr) {}
    
    // 五参数构造函数：完全指定所有属性，合力为0
    Shape(double m, double x, double y, double vx, double vy) : name("Shape"), type("Shape"), mass(m), mass_centre{x, y}, velocity{vx, vy}, totalforce{0.0, 0.0}, normalforce{0.0, 0.0}, fraction(0.0), static_fraction(0.0), isSupported(false), supporter(nullptr) {}

    // 虚析构函数
    virtual ~Shape() {}

public:
    // 公有方法 - 可以被子类覆写
    virtual void move(double dx, double dy);
    virtual void turn(double angle);
    
    // 纯虚函数 - 必须在子类中实现
    virtual bool check_collision(const Shape& other) const = 0;
    
    // 信息获取方法（const方法，不修改对象状态）
    double getMass() const;
    void getCentre(double& x, double& y) const;
    void getVelocity(double& vx, double& vy) const;
	void getFraction(double& f) const;
    void getStaticFraction(double& sf) const { sf = static_fraction; }
	void getRestitution(double& r) const;
    void getTotalForce(double& fx, double& fy) const;
    void getNormalForce(double& fx, double& fy) const;
    bool getIsSupported() const { return isSupported; }

    std::string getName() const { return name; }
    std::string getType() const { return type; }
    
    // 几何查询方法 - 获取物体底部Y坐标（由子类实现）"
    virtual double getBottom() const = 0;
	virtual double getTop() const = 0;

    // 设置方法
    void setName(const std::string& n) { name = n; }

    void setMass(double m);
    void setCentre(double x, double y);
    void setVelocity(double vx, double vy);
	void setFraction(double f) { fraction = f; }
    void setStaticFraction(double sf) { static_fraction = sf; }
	void setRestitution(double r) { restitution = r; }
    void setIsSupported(bool supported) { isSupported = supported; } // 设置支撑状态

    // 力相关方法
    void addToTotalForce(double fx, double fy);   // 累加力到合力
    void clearTotalForce();                        // 清空合力累加器
    void applyTotalForce(double deltaTime);        // 应用累加的合力到速度

    // 基础物理更新方法
    virtual void update(double deltaTime);  // 根据速度更新位置
    void applyForce(double fx, double fy);  // 累加力（不立即应用）
    void applyGravity(double g = 9.8);      // 累加重力
    void applyFriction(double normalForce, double kineticFriction, double staticFriction, double drivingForce);  // 应用摩擦力（区分静摩擦和动摩擦）
    void applyFrictionRelative(double normalForce, double kineticFriction, double staticFriction, double otherVx, double otherVy, double drivingForce);  // 应用相对摩擦力

	void applyNormalForce(double normalForce);  // 应用支撑力（垂直向上）
	
    // 支撑状态判定方法
    void checkSupportStatus(const Shape& supporter);  // 检查是否被特定物体支撑
    void resetSupportStatus() { isSupported = false; supporter = nullptr; } // 重置支撑状态（每帧开始时调用）
    Shape* getSupporter() const { return supporter; }  // 获取支撑物
    void setSupporter(Shape* sup) { supporter = sup; } // 设置支撑物

	bool HasCollidedWithGround(double ground_y) const;
};

/*=========================================================================================================
 * DynamicShape - 动态形状基类
 * 可以移动、受力、改变速度的物体
 *=========================================================================================================*/
struct DynamicShape : public Shape {
public:
    // 默认构造函数
    DynamicShape() : Shape() { type = "DynamicShape"; }
    
    // 单参数构造函数：指定质量
    DynamicShape(double m) : Shape(m) { type = "DynamicShape"; }
    
    // 三参数构造函数：指定质量和位置
    DynamicShape(double m, double x, double y) : Shape(m, x, y) { type = "DynamicShape"; }
    
    // 五参数构造函数：完全指定质量、位置、速度
    DynamicShape(double m, double x, double y, double vx, double vy) : Shape(m, x, y, vx, vy) { type = "DynamicShape"; }
    
    // 虚析构函数
    virtual ~DynamicShape() {}
    
    // 动态形状特有的方法
    bool isMoving() const;
    double getSpeed() const;
    double getKineticEnergy() const;
    void stop();
    void applyImpulse(double impulseX, double impulseY);
    void limitSpeed(double maxSpeed);
    void getMomentum(double& px, double& py) const;

	
};

/*=========================================================================================================
 * StaticShape - 静态形状基类
 * 不能移动、质量无穷大、速度始终为0的物体（如地面、墙壁）
 *=========================================================================================================*/
struct StaticShape : public Shape {
public:
    // 默认构造函数：质量无穷大，速度为0
    StaticShape() : Shape(INFINITY, 0.0, 0.0) {
        type = "StaticShape";
        velocity[0] = 0.0;
        velocity[1] = 0.0;
    }
    
    // 三参数构造函数：指定位置，质量无穷大
    StaticShape(double x, double y) : Shape(INFINITY, x, y) {
        type = "StaticShape";
        velocity[0] = 0.0;
        velocity[1] = 0.0;
    }
    
    // 虚析构函数
    virtual ~StaticShape() {}
    
    // 重写的方法
    virtual void update(double deltaTime) override;
    virtual void move(double dx, double dy) override;
    void setVelocity(double vx, double vy);
    void setMass(double m);
    
    // 静态形状特有的方法
    bool isStatic() const { return true; }
    bool isFixed() const { return true; }
    void editorMove(double dx, double dy);
    void editorSetCentre(double x, double y);
};

/*=========================================================================================================
 * Circle - 圆形类（动态形状）
 * 可以移动、滚动、碰撞的圆形物体
 *=========================================================================================================*/
struct Circle : public DynamicShape {
public:
    double radius;

    // 构造函数
    Circle() : DynamicShape(), radius(1.0) { type = "Circle"; name = "Circle"; }
    Circle(double r) : DynamicShape(), radius(r) { type = "Circle"; name = "Circle"; }
    Circle(double r, double x, double y) : DynamicShape(1.0, x, y), radius(r) { type = "Circle"; name = "Circle"; }
    Circle(double m, double r, double x, double y) : DynamicShape(m, x, y), radius(r) { type = "Circle"; name = "Circle"; }
    Circle(double m, double r, double x, double y, double vx, double vy) : DynamicShape(m, x, y, vx, vy), radius(r) { type = "Circle"; name = "Circle"; }

    // 覆写父类的方法
    virtual bool check_collision(const Shape& other) const override;
    virtual double getBottom() const override { return mass_centre[1] - radius; }
	virtual double getTop() const override { return mass_centre[1] + radius; }
    
    // Circle特有的getter方法
    double getRadius() const;
    double getArea() const;
    double getCircumference() const;
    double getDiameter() const { return 2.0 * radius; }
};

/*=========================================================================================================
 * AABB - 轴对齐包围盒（动态形状）
 * 可以移动、滑动、碰撞的矩形物体
 *=========================================================================================================*/
struct AABB : public DynamicShape {
public:
    double width;
    double height;

    // 构造函数
    AABB() : DynamicShape(), width(1.0), height(1.0) { type = "AABB"; name = "AABB"; }
    AABB(double w, double h) : DynamicShape(), width(w), height(h) { type = "AABB"; name = "AABB"; }
    AABB(double w, double h, double x, double y) : DynamicShape(1.0, x, y), width(w), height(h) { type = "AABB"; name = "AABB"; }
    AABB(double m, double w, double h, double x, double y) : DynamicShape(m, x, y), width(w), height(h) { type = "AABB"; name = "AABB"; }
    AABB(double m, double w, double h, double x, double y, double vx, double vy) : DynamicShape(m, x, y, vx, vy), width(w), height(h) { type = "AABB"; name = "AABB"; }

    // 覆写父类的方法
    virtual bool check_collision(const Shape& other) const override;
    virtual double getBottom() const override { return mass_centre[1] - height / 2.0; }

    // AABB特有的getter方法
    double getWidth() const { return width; }
    double getHeight() const { return height; }
    double getLeft() const { return mass_centre[0] - width / 2.0; }
    double getRight() const { return mass_centre[0] + width / 2.0; }
    double getTop() const { return mass_centre[1] + height / 2.0; }
    double getNormalForceABS() const { return std::abs(normalforce[1]); }
    
    // 几何计算方法
    double getArea() const;
    double getPerimeter() const;
    double getDiagonal() const;

    // AABB特有的计算摩擦力的方法
	Shape* getCompressedShapeDown() const;
	Shape* getCompressedShapeUp() const;
    void applyFrictionUP();
	void applyFrictionDOWN();
};

/*=========================================================================================================
 * Slope - 斜坡类（动态形状）
 * 可以移动的斜坡（虽然通常用作静态）
 *=========================================================================================================*/
struct Slope : public DynamicShape {
public:
    double length;
    double angle; // 斜坡与水平线的夹角，单位为弧度
    
    // 构造函数
    Slope() : DynamicShape(), length(1.0), angle(0.0) { type = "Slope"; name = "Slope"; }
    Slope(double l, double a) : DynamicShape(), length(l), angle(a) { type = "Slope"; name = "Slope"; }
    Slope(double l, double a, double x, double y) : DynamicShape(1.0, x, y), length(l), angle(a) { type = "Slope"; name = "Slope"; }
    Slope(double m, double l, double a, double x, double y) : DynamicShape(m, x, y), length(l), angle(a) { type = "Slope"; name = "Slope"; }
    Slope(double m, double l, double a, double x, double y, double vx, double vy) : DynamicShape(m, x, y, vx, vy), length(l), angle(a) { type = "Slope"; name = "Slope"; }

    // 覆写父类的方法
    virtual bool check_collision(const Shape& other) const override;
    virtual double getBottom() const override { return mass_centre[1]; }  // 简化：使用质心作为底部
    virtual double getTop() const override { return mass_centre[1] + getHeight(); }  // 顶部 = 质心 + 高度
    
    // Slope特有的getter方法
    double getLength() const { return length; }
    double getAngle() const { return angle; }
    double getAngleDegrees() const;
    double getHeight() const;
    double getBase() const;
    double getSlope() const;
};

/*=========================================================================================================
 * Ground - 地面类（静态形状）
 * 不能移动的地面，质量无穷大
 *=========================================================================================================*/
struct Ground : public StaticShape {
public:
	double y_level;   // 地面的y坐标（与mass_centre[1]同步）"

    // 构造函数
	Ground() : StaticShape(0.0, 0.0), y_level(0.0) { type = "Ground"; name = "Ground"; fraction = 0.0; static_fraction = 0.0; }
	Ground(double y) : StaticShape(0.0, y), y_level(y) { type = "Ground"; name = "Ground"; fraction = 0.0; static_fraction = 0.0; }
	Ground(double y, double f) : StaticShape(0.0, y), y_level(y) { type = "Ground"; name = "Ground"; fraction = f; static_fraction = f; }
	Ground(double y, double f, double sf) : StaticShape(0.0, y), y_level(y) { type = "Ground"; name = "Ground"; fraction = f; static_fraction = sf; }

	// Getter方法
	double getYLevel() const { return y_level; }
	double getFriction() const { return fraction; }
    double getStaticFriction() const { return static_fraction; }
    double getWidth() const { return INFINITY; }

	// Setter方法
	void setYLevel(double y);
	void setFriction(double f) { fraction = f; static_fraction = f; }
    void setFriction(double f, double sf) { fraction = f; static_fraction = sf; }
    
    // 覆写父类方法
    virtual bool check_collision(const Shape& other) const override;
    virtual double getBottom() const override { return y_level; }
    virtual double getTop() const override { return y_level; }  // 地面顶部就是地面本身
    
    // 支撑相关方法
    bool isPointOnGround(double x, double y) const { return y <= y_level; }
    void getNormal(double& nx, double& ny) const;
    void applyNormalForceToUpShape(Shape& shape);
    void checkAndSetSupportStatus(Shape& shape) const;

	// 析构函数
	virtual ~Ground() {}
};

/*=========================================================================================================
 * Wall - 墙壁类（静态形状）
 * 矩形墙壁，不能移动，质量无穷大，用于边界和障碍物
 *=========================================================================================================*/
struct Wall : public StaticShape {
public:
    double width;
    double height;

    // 构造函数
    Wall() : StaticShape(), width(1.0), height(1.0) { 
        type = "Wall"; 
        name = "Wall"; 
        fraction = 0.0;  // 默认无摩擦
    }
    
    Wall(double w, double h) : StaticShape(), width(w), height(h) { 
        type = "Wall"; 
        name = "Wall"; 
        fraction = 0.0;
    }
    
    Wall(double w, double h, double x, double y) : StaticShape(x, y), width(w), height(h) { 
        type = "Wall"; 
        name = "Wall"; 
        fraction = 0.0;
    }
    
    Wall(double w, double h, double x, double y, double f) : StaticShape(x, y), width(w), height(h) { 
        type = "Wall"; 
        name = "Wall"; 
        fraction = f;
        static_fraction = f; // 静摩擦系数默认等于动摩擦系数
    }
    
    Wall(double w, double h, double x, double y, double f, double sf) : StaticShape(x, y), width(w), height(h) { 
        type = "Wall"; 
        name = "Wall"; 
        fraction = f;
        static_fraction = sf;
    }

    // 覆写父类方法
    virtual bool check_collision(const Shape& other) const override;
    virtual double getBottom() const override { return mass_centre[1] - height / 2.0; }
    virtual double getTop() const override { return mass_centre[1] + height / 2.0; }
    
    // Wall特有的getter方法
    double getWidth() const { return width; }
    double getHeight() const { return height; }
    double getLeft() const { return mass_centre[0] - width / 2.0; }
    double getRight() const { return mass_centre[0] + width / 2.0; }
    
    // 几何计算方法
    double getArea() const;
    double getPerimeter() const;
    double getDiagonal() const;
    
    // 检查点是否在墙内
    bool containsPoint(double x, double y) const;
    
    // 获取到点的最近距离
    double distanceToPoint(double x, double y) const;

    // 虚析构函数
    virtual ~Wall() {}
};


#endif
