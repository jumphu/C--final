# 摩擦力实现文档

## 概述
本文档描述了物理引擎中摩擦力的实现，包括速度方向判定逻辑。

## 物理原理

### 摩擦力公式
```
F_friction = μ × N
```
其中：
- `μ` (mu): 摩擦系数
- `N`: 正压力（垂直于接触面的力）

### 摩擦力方向
摩擦力的方向**总是与速度方向相反**，用于阻止物体的相对运动。

## 实现细节

### 1. Shape 基类方法

#### `applyFriction(double normalForce, double frictionCoefficient)`
通用的摩擦力应用方法，适用于所有形状。

**算法步骤：**
1. 计算摩擦力大小：`f = μ × N`
2. 获取当前速度向量 `(vx, vy)`
3. 计算速度大小：`speed = sqrt(vx? + vy?)`
4. 如果速度接近零 (< 1e-6)，则不施加摩擦力（避免除零和抖动）
5. 计算速度的单位方向向量：`(vx/speed, vy/speed)`
6. 摩擦力方向 = -速度方向（取反）
7. 计算摩擦力分量并累加到合力

**代码示例：**
```cpp
void Shape::applyFriction(double normalForce, double frictionCoefficient) {
    double frictionMagnitude = frictionCoefficient * normalForce;
    
    double vx = velocity[0];
    double vy = velocity[1];
    double speed = std::sqrt(vx * vx + vy * vy);
    
    if (speed < 1e-6) return;  // 速度太小，不施加摩擦力
    
    // 归一化速度向量，取反得到摩擦力方向
    double dirX = -vx / speed;
    double dirY = -vy / speed;
    
    // 计算摩擦力分量
    double frictionX = frictionMagnitude * dirX;
    double frictionY = frictionMagnitude * dirY;
    
    addToTotalForce(frictionX, frictionY);
}
```

### 2. AABB 类特定方法

#### `applyFrictionUP()`
处理上方有物体压迫的摩擦力。

- 使用上方物体的质量计算正压力：`N = m_上 × g`
- 使用当前物体的摩擦系数

#### `applyFrictionDOWN()`
处理下方有物体支撑的摩擦力。

- 使用当前物体的质量计算正压力：`N = m × g`
- 使用下方物体的摩擦系数

## 使用示例

### 示例 1: 水平面摩擦
```cpp
Circle ball(1.0, 1.0, 0.0, 0.0);  // 质量1kg
ball.setVelocity(10.0, 0.0);      // 初始速度10 m/s
ball.setFraction(0.3);            // 摩擦系数0.3

double normalForce = ball.getMass() * 9.8;  // 正压力

// 每个时间步
ball.clearTotalForce();
ball.applyFriction(normalForce, 0.3);
ball.applyTotalForce(deltaTime);
ball.update(deltaTime);
```

### 示例 2: 斜向运动摩擦
```cpp
Circle ball(2.0, 1.0, 0.0, 0.0);
ball.setVelocity(6.0, 8.0);  // 速度方向为 atan2(8,6) ≈ 53°

double normalForce = ball.getMass() * 9.8;
ball.applyFriction(normalForce, 0.2);
// 摩擦力方向会自动计算为速度的反方向
```

### 示例 3: 重力 + 摩擦力组合
```cpp
Circle ball(1.0, 1.0, 0.0, 10.0);
ball.setVelocity(8.0, 5.0);

// 每个时间步
ball.clearTotalForce();
ball.applyGravity(9.8);                          // 累加重力
ball.applyFriction(ball.getMass() * 9.8, 0.1);  // 累加摩擦力
ball.applyTotalForce(deltaTime);                 // 应用所有力
ball.update(deltaTime);                          // 更新位置
```

## 关键特性

### 1. 速度方向判定
- 摩擦力方向通过归一化速度向量自动计算
- 无论速度方向如何，摩擦力总是精确地与之相反

### 2. 零速度处理
- 当速度接近零时（< 1e-6 m/s），不施加摩擦力
- 避免除零错误
- 防止数值抖动

### 3. 力累加机制
- 摩擦力通过 `addToTotalForce()` 累加到合力
- 可以与其他力（重力、推力等）组合
- 使用 `applyTotalForce()` 统一应用到速度

## 测试验证

### 测试 1: 水平摩擦减速
- **结果**: 速度均匀递减，符合 `a = -μg` 的理论值
- **验证**: ? 通过

### 测试 2: 斜向运动
- **结果**: 速度 X 和 Y 分量按相同比例减小，速度方向保持不变
- **验证**: ? 通过

### 测试 3: 方向验证
- **测试方法**: 计算速度向量与摩擦力向量的内积
- **预期结果**: 内积 = -1（完全相反）
- **实际结果**: 所有测试用例内积都精确等于 -1.0000
- **验证**: ? 通过

### 测试 4: 组合力
- **结果**: 重力和摩擦力正确组合，物体轨迹符合物理规律
- **验证**: ? 通过

## 数学推导

### 速度方向的单位向量
给定速度 `v = (vx, vy)`：

速度大小：
```
|v| = sqrt(vx? + vy?)
```

单位方向向量：
```
v? = (vx/|v|, vy/|v|)
```

摩擦力方向（与速度相反）：
```
F?_friction = -v? = (-vx/|v|, -vy/|v|)
```

摩擦力向量：
```
F_friction = f × F?_friction = (f × -vx/|v|, f × -vy/|v|)
```

### 内积验证
速度单位向量与摩擦力单位向量的内积：
```
v? · F?_friction = (vx/|v|) × (-vx/|v|) + (vy/|v|) × (-vy/|v|)
                 = -(vx?/|v|?) - (vy?/|v|?)
                 = -(vx? + vy?)/|v|?
                 = -|v|?/|v|?
                 = -1
```

这证明了摩擦力方向确实与速度方向完全相反。

## 注意事项

1. **摩擦系数范围**: 通常 `0 ≤ μ ≤ 1`，但代码不限制此范围
2. **正压力计算**: 需要根据具体情况正确计算（重力、接触力等）
3. **静摩擦 vs 动摩擦**: 当前实现为动摩擦，未区分静摩擦
4. **数值稳定性**: 使用阈值 1e-6 避免零速度时的数值问题

## 未来改进

1. 实现静摩擦与动摩擦的区分
2. 支持可变摩擦系数（依赖于速度）
3. 添加空气阻力（二次方阻力）
4. 优化性能（避免重复的平方根计算）
