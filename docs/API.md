# 形状处理库 API 文档

## 概述

`project_c` 命名空间包含了用于处理几何形状的类和函数。

## 类层次结构

```
Shape (基类)
├── Circle (圆形)
└── AABB (轴对齐边界框/矩形)
```

## 类详细说明

### Shape 类

基础形状类，所有具体形状的父类。

#### 属性
- `double mass`: 形状的质量
- `double mass_centre[2]`: 质心坐标 [x, y]
- `double velocity[2]`: 速度向量 [vx, vy]

#### 构造函数
```cpp
Shape()                                    // 默认构造函数
Shape(double m)                           // 指定质量
Shape(double m, double x, double y)       // 指定质量和位置
Shape(double m, double x, double y, double vx, double vy)  // 完全指定
```

#### 公有方法
```cpp
virtual void move(double dx, double dy)           // 移动形状
virtual void turn(double angle)                   // 旋转速度向量
virtual bool check_collision(const Shape& other)  // 碰撞检测
double getMass() const                            // 获取质量
void getCentre(double& x, double& y) const       // 获取质心
void getVelocity(double& vx, double& vy) const   // 获取速度
void setMass(double m)                            // 设置质量
void setCentre(double x, double y)               // 设置质心
void setVelocity(double vx, double vy)           // 设置速度
```

### Circle 类

圆形类，继承自 Shape。

#### 额外属性
- `double radius`: 圆的半径

#### 构造函数
```cpp
Circle()                                          // 默认构造函数
Circle(double r)                                  // 指定半径
Circle(double r, double x, double y)              // 半径和位置
Circle(double m, double r, double x, double y)    // 质量、半径和位置
Circle(double m, double r, double x, double y, double vx, double vy)  // 完全指定
```

#### 特有方法
```cpp
double getRadius() const            // 获取半径
void setRadius(double r)            // 设置半径
double getArea() const              // 计算面积
double getPerimeter() const         // 计算周长
```

### AABB 类

轴对齐边界框（矩形）类，继承自 Shape。

#### 额外属性
- `double width`: 矩形宽度
- `double height`: 矩形高度

#### 构造函数
```cpp
AABB()                                            // 默认构造函数
AABB(double w, double h)                          // 指定宽高
AABB(double w, double h, double x, double y)      // 宽高和位置
AABB(double m, double w, double h, double x, double y)  // 质量、宽高和位置
AABB(double m, double w, double h, double x, double y, double vx, double vy)  // 完全指定
```

#### 特有方法
```cpp
double getWidth() const             // 获取宽度
double getHeight() const            // 获取高度
void setWidth(double w)             // 设置宽度
void setHeight(double h)            // 设置高度
double getArea() const              // 计算面积
double getPerimeter() const         // 计算周长
```

## 碰撞检测

库实现了精确的碰撞检测算法：

- **Circle vs Circle**: 使用圆心距离与半径和的比较
- **AABB vs AABB**: 使用轴对齐边界框重叠检测
- **Circle vs AABB**: 使用最近点算法

## 使用示例

详细的使用示例请参考 `examples/` 目录中的文件。