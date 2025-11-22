#ifndef _shapes_h
#define _shapes_h

#include <iostream>
#include <cmath>

extern const double PI;

struct Shape {
public:
    double mass;
    double mass_centre[2]; // mass_centre[0]: x, mass_centre[1]: y
    double velocity[2];    // velocity[0]: vx, velocity[1]: vy
	double fraction;      // 摩擦系数
	  
    /*Constructors:Shape()
    默认构造函数，质量为1，质心在原点,速度为0
    包含方法:move(), turn()，check_collision(),getMass(),getCentre(),getVelocity()等
    */
    
    // 默认构造函数：质量为1，质心在原点，速度为0
    Shape() : mass(1.0), mass_centre{0.0, 0.0}, velocity{0.0, 0.0} {}
    
    // 单参数构造函数：指定质量，质心在原点，速度为0
    Shape(double m) : mass(m), mass_centre{0.0, 0.0}, velocity{0.0, 0.0} {}
    
    // 三参数构造函数：指定质量和质心坐标，速度为0
    Shape(double m, double x, double y) : mass(m), mass_centre{x, y}, velocity{0.0, 0.0} {}
    
    // 五参数构造函数：完全指定所有属性
    Shape(double m, double x, double y, double vx, double vy) : mass(m), mass_centre{x, y}, velocity{vx, vy} {}

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
    
    // 设置方法
    void setMass(double m);
    void setCentre(double x, double y);
    void setVelocity(double vx, double vy);

    // 基础物理更新方法
    virtual void update(double deltaTime);  // 根据速度更新位置
    void applyForce(double fx, double fy);  // 施加力
    void applyGravity(double g = 9.8);      // 重力

};


struct Circle : public Shape {
public:
    double radius;

    // 默认构造函数
    Circle() : Shape(), radius(1.0) {}

    // 单参数构造函数（只指定半径）
    Circle(double r) : Shape(), radius(r) {}

    // 三参数构造函数（半径 + 圆心坐标，质量默认为1）
    Circle(double r, double x, double y) : Shape(1.0, x, y), radius(r) {}

    // 四参数构造函数（质量 + 半径 + 圆心坐标）
    Circle(double m, double r, double x, double y) : Shape(m, x, y), radius(r) {}

    // 六参数构造函数（完全指定：质量 + 半径 + 圆心 + 速度）
    Circle(double m, double r, double x, double y, double vx, double vy) : Shape(m, x, y, vx, vy), radius(r) {}

    // 覆写父类的方法 - 圆形特定的碰撞检测
    virtual bool check_collision(const Shape& other) const override;
    
    // Circle特有的getter方法（const方法，不修改对象）
    double getRadius() const;
};

struct AABB : public Shape {
public:
    double width;
    double height;

    // 默认构造函数
    AABB() : Shape(), width(1.0), height(1.0) {}
    // 双参数构造函数（只指定宽度和高度）
    AABB(double w, double h) : Shape(), width(w), height(h) {}
    // 四参数构造函数（宽度 + 高度 + 质心坐标，质量默认为1）
    AABB(double w, double h, double x, double y) : Shape(1.0, x, y), width(w), height(h) {}
    // 五参数构造函数（质量 + 宽度 + 高度 + 质心坐标）
    AABB(double m, double w, double h, double x, double y) : Shape(m, x, y), width(w), height(h) {}
    // 七参数构造函数（完全指定：质量 + 宽度 + 高度 + 质心 + 速度）
    AABB(double m, double w, double h, double x, double y, double vx, double vy) : Shape(m, x, y, vx, vy), width(w), height(h) {}

    // 覆写父类的方法 - AABB特定的碰撞检测
    virtual bool check_collision(const Shape& other) const override;

    // AABB特有的getter方法（const方法，不修改对象）
    double getWidth() const { return width; }
    double getHeight() const { return height; }
};

struct Slope : public Shape {
public:
    double length;
    double angle; // 斜坡与水平线的夹角，单位为弧度
    
    // 默认构造函数
    Slope() : Shape(), length(1.0), angle(0.0) {}
    // 双参数构造函数（只指定长度和角度）
    Slope(double l, double a) : Shape(), length(l), angle(a) {}
    // 四参数构造函数（长度 + 角度 + 质心坐标，质量默认为1）
    Slope(double l, double a, double x, double y) : Shape(1.0, x, y), length(l), angle(a) {}
    // 五参数构造函数（质量 + 长度 + 角度 + 质心坐标）
    Slope(double m, double l, double a, double x, double y) : Shape(m, x, y), length(l), angle(a) {}
    // 七参数构造函数（完全指定：质量 + 长度 + 角度 + 质心 + 速度）
    Slope(double m, double l, double a, double x, double y, double vx, double vy) : Shape(m, x, y, vx, vy), length(l), angle(a) {}

    // 覆写父类的方法
    virtual bool check_collision(const Shape& other) const override;
    
    // Slope特有的getter方法（const方法，不修改对象）
    double getLength() const { return length; }
    double getAngle() const { return angle; }
};

struct Ground {
public:
	double y_level;   // 地面的y坐标
	double friction;  // 地面的摩擦系数,默认值为0

    // 默认构造函数：地面在y=0，无摩擦力
	Ground() : y_level(0.0), friction(0.0) {}

	// 单参数构造函数：指定地面高度，摩擦系数默认为0
	Ground(double y) : y_level(y), friction(0.0) {}

	// 双参数构造函数：同时指定地面高度和摩擦系数
	Ground(double y, double f) : y_level(y), friction(f) {}

	// Getter方法
	double getYLevel() const { return y_level; }
	double getFriction() const { return friction; }

	// Setter方法
	void setYLevel(double y) { y_level = y; }
	void setFriction(double f) { friction = f; }

	~Ground() {}
};


#endif
