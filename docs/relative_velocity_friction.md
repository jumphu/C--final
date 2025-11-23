# 相对速度摩擦力实现文档

## 更新概述

将摩擦力计算从使用**绝对速度**改为使用**相对速度**，这是物理上更准确的实现。

## 为什么需要相对速度？

### 物理原理
摩擦力的大小和方向应该基于**两个接触表面之间的相对运动**，而不是物体的绝对运动。

**错误的实现（使用绝对速度）：**
```
物体速度: v? = 10 m/s
地面速度: v? = 10 m/s
摩擦力方向: 与 v? 相反 ?
```
在这种情况下，物体和地面以相同速度运动，不应该有相对滑动，因此摩擦力应该为零。

**正确的实现（使用相对速度）：**
```
相对速度: v_rel = v? - v? = 0
摩擦力方向: 无（因为没有相对运动）?
```

## 实现细节

### 1. 新增方法

#### `applyFrictionRelative(normalForce, frictionCoefficient, otherVx, otherVy)`

计算相对于运动表面的摩擦力。

**参数：**
- `normalForce`: 正压力 (N)
- `frictionCoefficient`: 摩擦系数 μ
- `otherVx`, `otherVy`: 接触表面的速度分量

**算法：**
```cpp
// 1. 计算相对速度
v_rel = v_object - v_surface

// 2. 如果相对速度接近零，无摩擦力
if (|v_rel| < 1e-6) return

// 3. 摩擦力方向 = -v_rel 的方向
direction = -v_rel / |v_rel|

// 4. 摩擦力 = μN × direction
F_friction = μ × N × direction
```

### 2. 更新的方法

#### `applyFriction(normalForce, frictionCoefficient)`

保持不变，用于相对于**静止**表面的摩擦力（绝对速度情况）。

实际上是 `applyFrictionRelative(normalForce, frictionCoefficient, 0, 0)` 的特殊情况。

#### AABB 方法更新

- `applyFrictionUP()`: 使用上方物体的速度计算相对速度
- `applyFrictionDOWN()`: 使用下方物体的速度计算相对速度

## 应用场景

### 场景 1: 传送带效果

```cpp
// 物体静止在向右移动的传送带上
Circle obj(1.0, 1.0, 0.0, 0.0);
obj.setVelocity(0.0, 0.0);  // 物体静止

double beltSpeed = 5.0;  // 传送带速度

// 相对速度 = 0 - 5 = -5 m/s（物体相对传送带向左）
// 摩擦力向右，将加速物体
obj.applyFrictionRelative(obj.getMass() * 9.8, 0.3, beltSpeed, 0.0);
```

**结果：** 物体被摩擦力加速，最终达到传送带速度。

### 场景 2: 运动平台

```cpp
// 物体在向右移动的平台上，速度比平台快
Circle obj(1.0, 1.0, 0.0, 0.0);
obj.setVelocity(10.0, 0.0);

double platformSpeed = 3.0;

// 相对速度 = 10 - 3 = 7 m/s
// 摩擦力向左，减慢相对速度
obj.applyFrictionRelative(obj.getMass() * 9.8, 0.2, platformSpeed, 0.0);
```

**结果：** 物体被摩擦力减速，最终与平台同速。

### 场景 3: 两个接触物体

```cpp
// AABB 在另一个 AABB 上面
AABB topBox(1.0, 2.0, 2.0, 0.0, 5.0);
AABB bottomBox(2.0, 4.0, 2.0, 0.0, 0.0);

topBox.setVelocity(8.0, 0.0);
bottomBox.setVelocity(3.0, 0.0);

// topBox 使用相对速度计算摩擦力
double bVx, bVy;
bottomBox.getVelocity(bVx, bVy);
topBox.applyFrictionRelative(topBox.getMass() * 9.8, 0.4, bVx, bVy);
```

**结果：** 上方盒子考虑下方盒子的运动，摩擦力基于相对滑动。

## 测试结果验证

### 测试 1: 无相对速度
- **条件**: 物体和表面速度相同
- **预期**: 摩擦力为零
- **结果**: ? 通过

### 测试 2: 在运动表面滑动
- **条件**: 物体 10 m/s，表面 3 m/s
- **相对速度**: 7 m/s
- **预期**: 摩擦力 = μN = 0.3 × 9.8 = 2.94 N（向左）
- **结果**: ? 通过（2.940 N）

### 测试 3: 传送带加速物体
- **条件**: 物体初始静止，传送带 10 m/s
- **预期**: 物体被加速到传送带速度
- **结果**: ? 通过（约 2 秒达到 10 m/s）

### 测试 4: 绝对 vs 相对摩擦对比
- **条件**: 两个物体初始 10 m/s，地面 4 m/s
- **绝对摩擦**: 减速到 0
- **相对摩擦**: 减速到地面速度 4 m/s
- **结果**: ? 行为符合预期

### 测试 5: 逆向传送带
- **条件**: 物体 15 m/s，传送带 5 m/s
- **预期**: 物体减速到传送带速度
- **结果**: ? 通过

## API 对比

### 旧 API（仅绝对速度）
```cpp
shape.applyFriction(normalForce, mu);
// 总是相对于静止参考系
```

### 新 API（支持相对速度）
```cpp
// 相对于静止表面（与旧 API 兼容）
shape.applyFriction(normalForce, mu);

// 相对于运动表面（新功能）
shape.applyFrictionRelative(normalForce, mu, surfaceVx, surfaceVy);
```

## 数学推导

### 相对速度定义
```
v_rel = v_A - v_B
```

### 摩擦力方向
```
f? = -v?_rel = -(v_rel / |v_rel|)
```

### 摩擦力向量
```
F_friction = μ × N × f?
           = μ × N × (-v_rel / |v_rel|)
```

### 分量形式
```
F_x = μN × (-(vx_A - vx_B) / |v_rel|)
F_y = μN × (-(vy_A - vy_B) / |v_rel|)
```

## 物理意义

### 绝对摩擦（错误）
- 假设接触表面总是静止
- 不适用于运动平台、传送带等场景
- 会产生非物理的行为

### 相对摩擦（正确）
- 考虑接触表面的运动
- 摩擦力阻止**相对滑动**
- 符合真实物理行为

## 实际应用

### 1. 传送带系统
```cpp
// 更新传送带上的所有物体
for (auto& obj : objectsOnBelt) {
    obj->clearTotalForce();
    obj->applyGravity(9.8);
    obj->applyFrictionRelative(
        obj->getMass() * 9.8, 
        beltFriction, 
        beltVelocityX, 
        beltVelocityY
    );
    obj->applyTotalForce(deltaTime);
    obj->update(deltaTime);
}
```

### 2. 堆叠物体
```cpp
// 下方物体
bottomBox.clearTotalForce();
bottomBox.applyGravity(9.8);
bottomBox.applyFriction(bottomBox.getMass() * 9.8, groundFriction);

// 上方物体（考虑下方物体的运动）
double bottomVx, bottomVy;
bottomBox.getVelocity(bottomVx, bottomVy);

topBox.clearTotalForce();
topBox.applyGravity(9.8);
topBox.applyFrictionRelative(
    topBox.getMass() * 9.8,
    contactFriction,
    bottomVx,
    bottomVy
);
```

### 3. 运动平台
```cpp
// 平台以正弦运动
double platformVx = amplitude * cos(omega * time);

for (auto& obj : objectsOnPlatform) {
    obj->applyFrictionRelative(
        obj->getMass() * 9.8,
        platformFriction,
        platformVx,
        0.0
    );
}
```

## 性能考虑

### 计算复杂度
- 相对速度计算: O(1)
- 与绝对速度方法相同

### 优化建议
1. 对于静止表面，使用 `applyFriction()` 避免额外参数传递
2. 缓存接触对象的速度，避免重复查询
3. 使用空间划分减少接触检测

## 向后兼容性

? 完全兼容旧代码
- `applyFriction()` 保持不变
- 新代码可选择使用 `applyFrictionRelative()`

## 总结

| 特性 | 绝对摩擦 | 相对摩擦 |
|------|---------|---------|
| 物理准确性 | ? 不准确 | ? 准确 |
| 传送带支持 | ? 不支持 | ? 支持 |
| 运动平台 | ? 不支持 | ? 支持 |
| API 复杂度 | 简单 | 稍复杂 |
| 性能 | 相同 | 相同 |
| 兼容性 | N/A | ? 完全兼容 |

## 文件修改

- ? `include/shapes.h` - 添加 `applyFrictionRelative()` 声明
- ? `src/shapes.cpp` - 实现相对速度摩擦力逻辑
- ? `tests/test_relative_friction.cpp` - 完整测试套件

## 参考资料

- 经典力学：摩擦力总是阻止相对运动
- 动摩擦力: F = μN，方向与相对速度相反
- 传送带问题是相对速度的典型应用
