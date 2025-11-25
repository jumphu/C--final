# PhysicalWorld timeStep 成员变量未使用问题

## ? 问题发现

### 当前实现

```cpp
// physicalWorld.h
struct PhysicalWorld {
    double timeStep;  // ? 成员变量已添加
    
    PhysicalWorld() : timeStep(1.0/60.0) {}  // ? 已初始化
    
    void setTimeStep(double dt);  // ? setter 已添加
    double getTimeStep() const;   // ? getter 已添加
};
```

```cpp
// physicalWorld.cpp
void PhysicalWorld::update(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground) {
    // ? 使用参数 deltaTime，没有使用成员变量 timeStep
    resetSupportStates(shapeList);
    detectSupportRelations(shapeList, ground);
    updatePhysics(shapeList, deltaTime, ground);  // ← 使用 deltaTime
    handleAllCollisions(shapeList);
}
```

### 问题

1. **不一致性**：添加了 `timeStep` 成员变量，但没有使用
2. **困惑性**：用户不知道应该用哪个（`timeStep` 还是 `deltaTime`）
3. **冗余性**：有两个地方可以控制时间步长

---

## ?? 推荐解决方案：重载版本

### 修改头文件

```cpp
// include/physicalWorld.h

// 版本1：使用成员变量 timeStep（新增）
void update(std::vector<Shape*>& shapeList, const Ground& ground);

// 版本2：使用参数 deltaTime（保留）
void update(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground);
```

### 修改源文件

```cpp
// src/physicalWorld.cpp

// 版本1：使用成员变量 timeStep
void PhysicalWorld::update(std::vector<Shape*>& shapeList, const Ground& ground) {
    update(shapeList, timeStep, ground);  // 委托到版本2
}

// 版本2：使用参数 deltaTime（原版本保持不变）
void PhysicalWorld::update(std::vector<Shape*>& shapeList, double deltaTime, const Ground& ground) {
    resetSupportStates(shapeList);
    detectSupportRelations(shapeList, ground);
    updatePhysics(shapeList, deltaTime, ground);
    handleAllCollisions(shapeList);
}
```

---

## ?? 使用示例

### 简洁模式（使用成员变量）

```cpp
PhysicalWorld world;
world.setTimeStep(0.01);

for (int i = 0; i < 100; i++) {
    world.update(shapes, ground);  // ? 自动使用 0.01s
}
```

### 灵活模式（自定义时间步长）

```cpp
for (int i = 0; i < 100; i++) {
    world.update(shapes, 0.001, ground);  // ? 临时使用 1ms
}
```

---

**版本**：v2.3  
**发现日期**：2024  
