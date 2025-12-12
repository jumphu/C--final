#ifndef _SHAPES_H
#define _SHAPES_H

#include <iostream>
#include <cmath>
#include <string>
#include <algorithm> 

extern const double PI;

struct Shape {
public:
    std::string name;
    std::string type;
    
    double mass;
    double mass_centre[2]; 
    double velocity[2];    
    double fraction;       
    double static_fraction;
    double restitution;    
    double totalforce[2];  
    double normalforce[2]; 
    bool isSupported;      
    Shape* supporter;      

    // 构造函数保留在头文件里（方便）
    Shape() : name("Shape"), type("Shape"), mass(1.0), fraction(0.0), static_fraction(0.0), restitution(1.0), isSupported(false), supporter(nullptr) {
        mass_centre[0] = 0.0; mass_centre[1] = 0.0;
        velocity[0] = 0.0; velocity[1] = 0.0;
        totalforce[0] = 0.0; totalforce[1] = 0.0;
        normalforce[0] = 0.0; normalforce[1] = 0.0;
    }
    
    Shape(double m) : Shape() { mass = m; }
    Shape(double m, double x, double y) : Shape(m) { mass_centre[0]=x; mass_centre[1]=y; }
    Shape(double m, double x, double y, double vx, double vy) : Shape(m, x, y) { velocity[0]=vx; velocity[1]=vy; }

    virtual ~Shape() {}

    // 【关键修改】所有函数只保留声明（加分号），去掉 {} 及其内容
    virtual void move(double dx, double dy);
    virtual void turn(double angle);
    virtual bool check_collision(const Shape& other) const = 0;
    
    double getMass() const;
    void getCentre(double& x, double& y) const;
    void getVelocity(double& vx, double& vy) const;
    void getFraction(double& f) const;
    void getStaticFraction(double& sf) const; // 去掉了内联实现
    void getRestitution(double& r) const;
    void getTotalForce(double& fx, double& fy) const;
    void getNormalForce(double& fx, double& fy) const;
    bool getIsSupported() const; // 去掉了内联实现

    std::string getName() const; // 去掉了内联实现
    std::string getType() const; // 去掉了内联实现
    
    virtual double getBottom() const = 0;
    virtual double getTop() const = 0;

    void setName(const std::string& n); // 去掉了内联实现
    void setMass(double m);
    void setCentre(double x, double y);
    void setVelocity(double vx, double vy);
    void setFraction(double f); // 去掉了内联实现
    void setStaticFraction(double sf); // 去掉了内联实现
    void setRestitution(double r); // 去掉了内联实现
    void setIsSupported(bool supported); // 去掉了内联实现

    void addToTotalForce(double fx, double fy);
    void clearTotalForce();
    void applyTotalForce(double deltaTime);
    virtual void update(double deltaTime);
    void applyForce(double fx, double fy);
    void applyGravity(double g = 9.8);
    void applyFriction(double normalForce, double kineticFriction, double staticFriction, double drivingForce);
    void applyFrictionRelative(double normalForce, double kineticFriction, double staticFriction, double otherVx, double otherVy, double drivingForce);
    void applyNormalForce(double normalForce);
    
    void checkSupportStatus(const Shape& supporter);
    void resetSupportStatus(); // 去掉了内联实现
    Shape* getSupporter() const; // 去掉了内联实现
    void setSupporter(Shape* sup); // 去掉了内联实现
    bool HasCollidedWithGround(double ground_y) const;
};

struct DynamicShape : public Shape {
    DynamicShape() : Shape() { type = "DynamicShape"; }
    DynamicShape(double m) : Shape(m) { type = "DynamicShape"; }
    DynamicShape(double m, double x, double y) : Shape(m, x, y) { type = "DynamicShape"; }
    DynamicShape(double m, double x, double y, double vx, double vy) : Shape(m, x, y, vx, vy) { type = "DynamicShape"; }
    virtual ~DynamicShape() {}
    
    bool isMoving() const;
    double getSpeed() const;
    double getKineticEnergy() const;
    void stop();
    void applyImpulse(double impulseX, double impulseY);
    void limitSpeed(double maxSpeed);
    void getMomentum(double& px, double& py) const;
};

struct StaticShape : public Shape {
    StaticShape() : Shape(INFINITY, 0.0, 0.0) { type = "StaticShape"; velocity[0] = 0.0; velocity[1] = 0.0; }
    StaticShape(double x, double y) : Shape(INFINITY, x, y) { type = "StaticShape"; velocity[0] = 0.0; velocity[1] = 0.0; }
    virtual ~StaticShape() {}
    
    virtual void update(double deltaTime) override;
    virtual void move(double dx, double dy) override;
    void setVelocity(double vx, double vy);
    void setMass(double m);
    
    bool isStatic() const; // 去掉了内联实现
    bool isFixed() const; // 去掉了内联实现
    void editorMove(double dx, double dy);
    void editorSetCentre(double x, double y);
};

struct Circle : public DynamicShape {
    double radius;
    Circle() : DynamicShape(), radius(1.0) { type = "Circle"; name = "Circle"; }
    Circle(double r) : DynamicShape(), radius(r) { type = "Circle"; name = "Circle"; }
    Circle(double r, double x, double y) : DynamicShape(1.0, x, y), radius(r) { type = "Circle"; name = "Circle"; }
    Circle(double m, double r, double x, double y) : DynamicShape(m, x, y), radius(r) { type = "Circle"; name = "Circle"; }
    Circle(double m, double r, double x, double y, double vx, double vy) : DynamicShape(m, x, y, vx, vy), radius(r) { type = "Circle"; name = "Circle"; }

    virtual bool check_collision(const Shape& other) const override;
    virtual double getBottom() const override { return mass_centre[1] - radius; }
    virtual double getTop() const override { return mass_centre[1] + radius; }
    
    // 【关键修改】去掉 {}，只保留 ;
    double getRadius() const; 
    double getArea() const;
    double getCircumference() const;
    double getDiameter() const; // 去掉了内联实现
};

struct AABB : public DynamicShape {
    double width, height;
    AABB() : DynamicShape(), width(1.0), height(1.0) { type = "AABB"; name = "AABB"; }
    AABB(double w, double h) : DynamicShape(), width(w), height(h) { type = "AABB"; name = "AABB"; }
    AABB(double w, double h, double x, double y) : DynamicShape(1.0, x, y), width(w), height(h) { type = "AABB"; name = "AABB"; }
    AABB(double m, double w, double h, double x, double y) : DynamicShape(m, x, y), width(w), height(h) { type = "AABB"; name = "AABB"; }
    AABB(double m, double w, double h, double x, double y, double vx, double vy) : DynamicShape(m, x, y, vx, vy), width(w), height(h) { type = "AABB"; name = "AABB"; }

    virtual bool check_collision(const Shape& other) const override;
    virtual double getBottom() const override { return mass_centre[1] - height / 2.0; }
    
    // 【关键修改】去掉 {}，只保留 ;
    double getWidth() const; 
    double getHeight() const; 
    double getLeft() const; 
    double getRight() const; 
    double getTop() const; 
    double getNormalForceABS() const; 
    double getArea() const;
    double getPerimeter() const;
    double getDiagonal() const;
    Shape* getCompressedShapeDown() const;
    Shape* getCompressedShapeUp() const;
    void applyFrictionUP();
    void applyFrictionDOWN();
};

struct Slope : public DynamicShape {
    double length, angle;
    Slope() : DynamicShape(), length(1.0), angle(0.0) { type = "Slope"; name = "Slope"; }
    Slope(double l, double a) : DynamicShape(), length(l), angle(a) { type = "Slope"; name = "Slope"; }
    Slope(double l, double a, double x, double y) : DynamicShape(1.0, x, y), length(l), angle(a) { type = "Slope"; name = "Slope"; }
    Slope(double m, double l, double a, double x, double y) : DynamicShape(m, x, y), length(l), angle(a) { type = "Slope"; name = "Slope"; }
    Slope(double m, double l, double a, double x, double y, double vx, double vy) : DynamicShape(m, x, y, vx, vy), length(l), angle(a) { type = "Slope"; name = "Slope"; }

    virtual bool check_collision(const Shape& other) const override;
    virtual double getBottom() const override { return mass_centre[1]; }
    virtual double getTop() const override { return mass_centre[1] + getHeight(); }
    
    // 【关键修改】去掉 {}，只保留 ;
    double getLength() const; 
    double getAngle() const; 
    double getAngleDegrees() const;
    double getHeight() const;
    double getBase() const;
    double getSlope() const;
};

struct Ground : public StaticShape {
    double y_level;
    Ground() : StaticShape(0.0, 0.0), y_level(0.0) { type = "Ground"; name = "Ground"; fraction = 0.0; static_fraction = 0.0; }
    Ground(double y) : StaticShape(0.0, y), y_level(y) { type = "Ground"; name = "Ground"; fraction = 0.0; static_fraction = 0.0; }
    Ground(double y, double f) : StaticShape(0.0, y), y_level(y) { type = "Ground"; name = "Ground"; fraction = f; static_fraction = f; }
    Ground(double y, double f, double sf) : StaticShape(0.0, y), y_level(y) { type = "Ground"; name = "Ground"; fraction = f; static_fraction = sf; }
    virtual ~Ground() {}

    // 【关键修改】去掉 {}，只保留 ;
    double getYLevel() const; 
    double getFriction() const; 
    double getStaticFriction() const; 
    double getWidth() const; 
    
    void setYLevel(double y);
    void setFriction(double f); // 去掉了内联实现
    void setFriction(double f, double sf); // 去掉了内联实现
    
    virtual bool check_collision(const Shape& other) const override;
    virtual double getBottom() const override { return y_level; }
    virtual double getTop() const override { return y_level; }
    bool isPointOnGround(double x, double y) const; // 去掉了内联实现
    void getNormal(double& nx, double& ny) const;
    void applyNormalForceToUpShape(Shape& shape);
    void checkAndSetSupportStatus(Shape& shape) const;
};

struct Wall : public StaticShape {
    double width, height;
    Wall() : StaticShape(), width(1.0), height(1.0) { type = "Wall"; name = "Wall"; fraction = 0.0; }
    Wall(double w, double h) : StaticShape(), width(w), height(h) { type = "Wall"; name = "Wall"; fraction = 0.0; }
    Wall(double w, double h, double x, double y) : StaticShape(x, y), width(w), height(h) { type = "Wall"; name = "Wall"; fraction = 0.0; }
    Wall(double w, double h, double x, double y, double f) : StaticShape(x, y), width(w), height(h) { type = "Wall"; name = "Wall"; fraction = f; static_fraction = f; }
    Wall(double w, double h, double x, double y, double f, double sf) : StaticShape(x, y), width(w), height(h) { type = "Wall"; name = "Wall"; fraction = f; static_fraction = sf; }
    virtual ~Wall() {}

    virtual bool check_collision(const Shape& other) const override;
    virtual double getBottom() const override { return mass_centre[1] - height / 2.0; }
    virtual double getTop() const override { return mass_centre[1] + height / 2.0; }
    
    // 【关键修改】去掉 {}，只保留 ;
    double getWidth() const; 
    double getHeight() const; 
    double getLeft() const; 
    double getRight() const; 
    
    double getArea() const;
    double getPerimeter() const;
    double getDiagonal() const;
    bool containsPoint(double x, double y) const;
    double distanceToPoint(double x, double y) const;
};

#endif