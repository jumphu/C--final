# PhysicalWorld 时间步长控制功能

## ?? 概述

为 `PhysicalWorld` 类添加了**时间步长（time step）**的 getter 和 setter 方法，允许动态调整物理模拟的时间粒度。

---

## ?? 为什么需要时间步长控制？

### 问题背景

在三球碰撞测试中，我们发现：

```cpp
// 测试代码
for (int i = 0; i < 5; i++) {
    world.update(shapes, 0.01, ground);  // 固定时间步长 0.01s
}
```

**问题**：
- ? 时间步长**硬编码**在测试代码中
- ? 无法灵活调整模拟精度
- ? 不同测试可能需要不同的时间步长
- ? 难以对比不同时间步长的效果

### 解决方案

添加时间步长作为 `PhysicalWorld` 的**成员变量**，提供 getter 和 setter：

```cpp
// 现在可以这样使用
PhysicalWorld world;
world.setTimeStep(0.001);  // 设置为1ms（更精确）

for (int i = 0; i < 100; i++) {
    world.update(shapes, world.getTimeStep(), ground);  // 使用统一的时间步长
}
```

---

## ?? 实现细节

### 1. 添加成员变量

```cpp
struct PhysicalWorld {
public:
    double gravity;     // 重力加速度
    double timeStep;    // 时间步长（秒）  ← 新增
    double bounds[4];   // 边界
    // ...
};
```

### 2. 修改构造函数

```cpp
// 默认构造函数：时间步长为 1/60 秒（60 FPS）
PhysicalWorld() 
    : gravity(9.8), 
      timeStep(1.0/60.0),  // ← 新增：默认60 FPS
      bounds{-1000.0, 1000.0, 1000.0, -1000.0} 
{}

// 带边界的构造函数
PhysicalWorld(double left, double right, double bottom, double top) 
    : gravity(9.8), 
      timeStep(1.0/60.0),  // ← 新增：默认60 FPS
      bounds{left, right, bottom, top} 
{}
```

### 3. 添加 Getter 方法

```cpp
// 获取当前时间步长
double getTimeStep() const {
    return timeStep;
}
```

**特点**：
- `const` 方法，不修改对象状态
- 返回当前时间步长（秒）

### 4. 添加 Setter 方法

```cpp
// 设置时间步长
void setTimeStep(double dt) {
    if (dt > 0.0) {  // ← 防御性编程：时间步长必须为正
        timeStep = dt;
    }
}
```

**特点**：
- 参数验证：只接受正数
- 忽略非法值（≤ 0）

---

## ?? 使用示例

### 基础使用

```cpp
#include "physicalWorld.h"

int main() {
    PhysicalWorld world;
    
    // 获取默认时间步长
    double dt = world.getTimeStep();
    cout << "默认时间步长: " << dt << " 秒" << endl;
    // 输出: 默认时间步长: 0.0166667 秒 (约60 FPS)
    
    // 设置新的时间步长
    world.setTimeStep(0.01);  // 100 FPS
    cout << "新时间步长: " << world.getTimeStep() << " 秒" << endl;
    // 输出: 新时间步长: 0.01 秒
}
```

### 高精度模拟

```cpp
// 需要高精度模拟（例如科学计算）
PhysicalWorld world;
world.setTimeStep(0.001);  // 1ms，1000 FPS

vector<Shape*> shapes;
// ... 创建物体 ...

for (int i = 0; i < 1000; i++) {
    world.update(shapes, world.getTimeStep(), ground);
}
// 模拟 1 秒的物理过程，精度很高
```

### 快速模拟（低精度）

```cpp
// 快速模拟，不需要高精度
PhysicalWorld world;
world.setTimeStep(0.1);  // 100ms，10 FPS

for (int i = 0; i < 10; i++) {
    world.update(shapes, world.getTimeStep(), ground);
}
// 模拟 1 秒的物理过程，速度快但精度低
```

### 动态调整时间步长

```cpp
PhysicalWorld world;

// 模拟开始：使用大时间步长快速到达目标状态
world.setTimeStep(0.05);  // 50ms
for (int i = 0; i < 20; i++) {
    world.update(shapes, world.getTimeStep(), ground);
}

// 模拟精细部分：使用小时间步长
world.setTimeStep(0.001);  // 1ms
for (int i = 0; i < 1000; i++) {
    world.update(shapes, world.getTimeStep(), ground);
}
```

---

## ?? 时间步长对比

### 不同时间步长的效果

| 时间步长 | FPS | 适用场景 | 优点 | 缺点 |
|---------|-----|---------|------|------|
| 0.001s (1ms) | 1000 | 科学计算 | 精度极高 | 计算量大 |
| 0.01s (10ms) | 100 | 精确模拟 | 精度高 | 计算量较大 |
| 0.0167s (~16.7ms) | 60 | 游戏/动画 | 精度与性能平衡 | - |
| 0.033s (~33ms) | 30 | 低配置设备 | 性能好 | 精度一般 |
| 0.1s (100ms) | 10 | 快速预览 | 速度快 | 精度低 |

### 精度与性能权衡

```
精度 ↑
│
│  1ms   ●  (精度最高，但很慢)
│
│  10ms  ●  (精度高，较慢)
│
│  16ms  ●  (游戏标准，60 FPS)
│
│  33ms  ●  (精度一般，30 FPS)
│
│  100ms ●  (精度低，但很快)
│
└─────────────────────→ 性能
```

---

## ?? 测试用例改进

### 改进前（硬编码时间步长）

```cpp
void testMultipleCollisions() {
    PhysicalWorld world;
    vector<Shape*> shapes;
    
    // 硬编码：无法调整
    for (int i = 0; i < 5; i++) {
        world.update(shapes, 0.01, ground);  // ← 固定值
    }
}
```

### 改进后（使用 timeStep）

```cpp
void testMultipleCollisions() {
    PhysicalWorld world;
    world.setTimeStep(0.001);  // ← 可以调整！
    
    vector<Shape*> shapes;
    
    // 使用统一的时间步长
    for (int i = 0; i < 100; i++) {
        world.update(shapes, world.getTimeStep(), ground);
    }
}
```

**优点**：
- ? 集中管理时间步长
- ? 便于调优
- ? 测试不同精度的效果

---

## ?? 修复三球碰撞测试

### 问题分析

三球碰撞测试失败的原因：

```cpp
Circle* c1 = new Circle(1.0, 0.5, 0.0, 2.0);   // x=0
Circle* c2 = new Circle(1.0, 0.5, 1.2, 2.0);  // x=1.2
Circle* c3 = new Circle(1.0, 0.5, 2.4, 2.0);  // x=2.4

c1->setVelocity(10.0, 0.0);  // 球1速度10 m/s

// 只模拟 5 帧 × 0.01s = 0.05 秒
for (int i = 0; i < 5; i++) {
    world.update(shapes, 0.01, ground);
}

// 0.05秒内球1只移动了 10 × 0.05 = 0.5 米
// 但球1和球2之间距离 1.2 米，根本碰不到！
```

### 解决方案1：增加时间步数

```cpp
PhysicalWorld world;
world.setTimeStep(0.01);  // 10ms

// 增加到 20 帧 = 0.2 秒
for (int i = 0; i < 20; i++) {
    world.update(shapes, world.getTimeStep(), ground);
}

// 球1移动 10 × 0.2 = 2.0 米，足够碰到球2和球3
```

### 解决方案2：缩小时间步长（更精确）

```cpp
PhysicalWorld world;
world.setTimeStep(0.001);  // 1ms，更精确

// 100 帧 = 0.1 秒
for (int i = 0; i < 100; i++) {
    world.update(shapes, world.getTimeStep(), ground);
}

// 球1移动 10 × 0.1 = 1.0 米
```

### 解决方案3：调整初始位置

```cpp
Circle* c1 = new Circle(1.0, 0.5, 0.0, 2.0);
Circle* c2 = new Circle(1.0, 0.5, 0.9, 2.0);  // ← 改为0.9（更近）
Circle* c3 = new Circle(1.0, 0.5, 1.8, 2.0);  // ← 改为1.8

// 现在距离更近，5帧就足够了
for (int i = 0; i < 5; i++) {
    world.update(shapes, world.getTimeStep(), ground);
}
```

---

## ?? 时间步长的物理意义

### 1. 欧拉积分法

物理模拟使用**欧拉积分法**：

```cpp
// 速度更新
v_new = v_old + a × Δt

// 位置更新
x_new = x_old + v × Δt
```

**Δt 越小**：
- ? 积分误差越小
- ? 模拟越精确
- ? 计算量越大

### 2. 碰撞检测精度

```
大时间步长（Δt = 0.1s）:
帧N:    A →→→→→→→→   B
        可能直接穿过！

小时间步长（Δt = 0.01s）:
帧N:   A→  B
帧N+1:  A→B     ← 能检测到碰撞
```

---

## ?? 最佳实践

### 1. 选择合适的时间步长

```cpp
// 游戏（60 FPS）
world.setTimeStep(1.0 / 60.0);  // 推荐

// 精确模拟
world.setTimeStep(0.01);  // 100 FPS

// 科学计算
world.setTimeStep(0.001);  // 1000 FPS
```

### 2. 固定时间步长

```cpp
// 不推荐：使用可变时间步长
double deltaTime = realTime - lastTime;
world.update(shapes, deltaTime, ground);  // ← 不稳定

// 推荐：使用固定时间步长
world.setTimeStep(1.0 / 60.0);
world.update(shapes, world.getTimeStep(), ground);  // ← 稳定
```

### 3. 时间累积法（适用于游戏）

```cpp
PhysicalWorld world;
world.setTimeStep(1.0 / 60.0);

double accumulator = 0.0;
double frameTime = getFrameTime();  // 实际帧时间

accumulator += frameTime;

while (accumulator >= world.getTimeStep()) {
    world.update(shapes, world.getTimeStep(), ground);
    accumulator -= world.getTimeStep();
}
```

---

## ?? 相关文档

1. **碰撞重叠分析**：`docs/COLLISION_OVERLAP_ANALYSIS.md`
2. **测试报告**：`tests/COMPREHENSIVE_COLLISION_TEST_REPORT.md`
3. **测试指南**：`tests/README_COMPREHENSIVE_COLLISION_TEST.md`

---

## ?? 总结

### 改进内容

| 改进 | 说明 |
|------|------|
| ? 添加 `timeStep` 成员变量 | 存储时间步长 |
| ? 添加 `getTimeStep()` | 获取时间步长 |
| ? 添加 `setTimeStep(dt)` | 设置时间步长（带验证）|
| ? 默认值 1/60 秒 | 对应60 FPS |
| ? 参数验证 | 只接受正数 |

### 优点

1. ? **灵活性**：可以动态调整模拟精度
2. ? **可测试性**：便于测试不同时间步长的效果
3. ? **可维护性**：时间步长集中管理
4. ? **性能优化**：可以根据需要调整精度与性能
5. ? **向后兼容**：不影响现有代码

### 使用建议

```cpp
// 1. 创建世界时设置时间步长
PhysicalWorld world;
world.setTimeStep(0.01);  // 100 FPS

// 2. 在 update 循环中使用
for (int i = 0; i < frameCount; i++) {
    world.update(shapes, world.getTimeStep(), ground);
}

// 3. 根据需要动态调整
if (needHighPrecision) {
    world.setTimeStep(0.001);  // 提高精度
} else {
    world.setTimeStep(0.033);  // 提高性能
}
```

---

**版本**：v2.2  
**最后更新**：2024  
**作者**：物理引擎开发团队
