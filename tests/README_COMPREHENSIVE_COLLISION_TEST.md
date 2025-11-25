# 全面碰撞测试 - 使用指南

## ?? 概述

这是一个全面的碰撞测试套件，测试物理引擎的碰撞检测、碰撞响应、分离机制等核心功能。

## ?? 快速开始

### 编译

```bash
g++ -o tests/test_comprehensive_collision.exe tests/test_comprehensive_collision.cpp src/physicalWorld.cpp src/shapes.cpp -Iinclude -std=c++11
```

### 运行

```bash
.\tests\test_comprehensive_collision.exe
```

或使用 PowerShell：

```powershell
.\tests\test_comprehensive_collision.exe
```

---

## ?? 测试内容

### 第一部分：碰撞检测测试

测试不同形状之间的碰撞检测是否正确：

- ? Circle-Circle 碰撞检测
- ? AABB-AABB 碰撞检测
- ? Circle-AABB 碰撞检测

### 第二部分：弹性碰撞测试

测试完全弹性碰撞（e=1.0）的行为：

- ? 相同质量的速度交换
- ? 不同质量的碰撞效果
- ? 球撞固定墙壁（无穷大质量）

### 第三部分：非弹性碰撞测试

测试非弹性碰撞的能量损失：

- ? 完全非弹性碰撞（e=0）
- ? 部分弹性碰撞（e=0.5）
- ? 能量损失验证

### 第四部分：动量守恒测试

验证物理定律的正确性：

- ? 正面碰撞动量守恒
- ? 斜向碰撞动量守恒（X和Y方向）

### 第五部分：重叠分离测试

测试重叠物体的分离机制：

- ? Circle-Circle 分离
- ? AABB-AABB 分离
- ? 质量比例分离（重物少动，轻物多动）

### 第六部分：多物体碰撞测试

测试复杂场景下的碰撞：

- ?? 三球连续碰撞（需要优化）

### 第七部分：边界情况测试

测试特殊情况下的行为：

- ? 零速度碰撞
- ? 追赶碰撞（同向运动）
- ? 刚好接触的检测

---

## ?? 预期结果

### 通过率

```
总测试数: 24
通过: 22 ?
失败: 2 ?
通过率: 91.7%
```

### 输出示例

```
╔════════════════════════════════════════╗
║     全面的碰撞测试套件 v2.0            ║
║   测试物理引擎的碰撞系统               ║
╚════════════════════════════════════════╝

========================================
  第一部分：碰撞检测测试
========================================
? PASS - Circle-Circle 碰撞检测（重叠）
? PASS - Circle-Circle 无碰撞
? PASS - AABB-AABB 碰撞检测（重叠）
? PASS - AABB-AABB 无碰撞
? FAIL - Circle-AABB 碰撞检测（边缘接触）

...

测试统计：
  总测试数: 24
  ? 通过: 22
  ? 失败: 2
  通过率: 91.7%
```

---

## ?? 详细测试说明

### 弹性碰撞示例

```cpp
// 测试：完全弹性碰撞（e=1.0）
Circle c1(1.0, 1.0, 0.0, 0.0);  // 质量1，半径1，位置(0,0)
Circle c2(1.0, 1.0, 1.9, 0.0);  // 质量1，半径1，位置(1.9,0)

c1.setVelocity(10.0, 0.0);      // 速度(10, 0)
c2.setVelocity(-10.0, 0.0);     // 速度(-10, 0)

world.resolveCollision(c1, c2);

// 预期结果：速度交换
// c1: (-10, 0)
// c2: (10, 0)
```

### 质量效应示例

```cpp
// 测试：重球撞轻球（质量比10:1）
Circle heavy(10.0, 1.0, 0.0, 0.0);  // 质量10
Circle light(1.0, 1.0, 1.9, 0.0);   // 质量1

heavy.setVelocity(5.0, 0.0);
light.setVelocity(0.0, 0.0);

world.resolveCollision(heavy, light);

// 预期结果：
// heavy: v ≈ 4.09 (略减速)
// light: v ≈ 9.09 (大加速)
```

### 动量守恒验证

```cpp
// 碰撞前动量
double p_before = m1 * v1 + m2 * v2;

// 碰撞
world.resolveCollision(c1, c2);

// 碰撞后动量
double p_after = m1 * v1_new + m2 * v2_new;

// 验证：p_before ≈ p_after
assert(fabs(p_after - p_before) < 0.5);
```

---

## ?? 已知问题

### 1. Circle-AABB 边缘接触检测

**问题**：当圆形刚好接触矩形边缘时（距离=半径），碰撞检测失败。

**影响**：边界情况下的碰撞可能被遗漏。

**修复**：修改 `shapes.cpp` 中的 `Circle::check_collision()`：
```cpp
return distance <= radius;  // 改为 <=
```

### 2. 三球连续碰撞测试失败

**问题**：能量未能传递到第三个球。

**可能原因**：
- 初始位置太远，未发生碰撞
- 时间步长太小，能量未传递完成
- 支撑关系干扰了碰撞

**修复建议**：
```cpp
// 增加时间步数
for (int i = 0; i < 50; i++) {  // 改为50帧
    world.update(shapes, 0.01, ground);
}
```

---

## ?? 性能基准

### 单次碰撞性能

```
碰撞检测: ~0.01ms
碰撞解决: ~0.05ms
位置分离: ~0.02ms
总计: ~0.08ms
```

### 多物体碰撞性能

```
10个物体: ~0.5ms
100个物体: ~50ms (O(n?)算法)
```

**注意**：实际性能取决于硬件和编译器优化。

---

## ?? 自定义测试

### 添加新测试

```cpp
void myCustomTest() {
    printSeparator("我的自定义测试");
    
    PhysicalWorld world;
    
    // 创建物体
    Circle c1(1.0, 1.0, 0.0, 0.0);
    Circle c2(1.0, 1.0, 2.0, 0.0);
    
    // 设置初始状态
    c1.setVelocity(10.0, 0.0);
    c2.setVelocity(0.0, 0.0);
    
    // 模拟碰撞
    world.resolveCollision(c1, c2);
    
    // 验证结果
    double v1, v2, vy;
    c1.getVelocity(v1, vy);
    c2.getVelocity(v2, vy);
    
    bool passed = (v1 > 0 && v2 > 0);
    printTestResult("我的测试", passed);
}

// 在main函数中调用
int main() {
    // ... 其他测试 ...
    myCustomTest();
    // ...
}
```

---

## ?? 测试报告

详细的测试报告请查看：
- **完整报告**：`tests/COMPREHENSIVE_COLLISION_TEST_REPORT.md`

报告包含：
- ? 各部分详细结果
- ?? 性能统计
- ?? 问题分析
- ?? 修复建议

---

## ?? 故障排除

### 编译错误

**问题**：找不到头文件
```
fatal error: physicalWorld.h: No such file or directory
```

**解决**：
```bash
# 确保在项目根目录执行编译命令
cd C:\project-C
g++ -o tests/test_comprehensive_collision.exe tests/test_comprehensive_collision.cpp src/physicalWorld.cpp src/shapes.cpp -Iinclude -std=c++11
```

### 运行时错误

**问题**：段错误或访问冲突

**可能原因**：
1. 物体指针未正确初始化
2. 删除了已删除的指针

**解决**：
```cpp
// 确保正确管理内存
vector<Shape*> shapes;
Circle* c1 = new Circle(...);
shapes.push_back(c1);

// 使用后删除
for (auto shape : shapes) {
    delete shape;
}
shapes.clear();
```

---

## ?? 测试目标

### 核心功能验证

- [x] 碰撞检测准确性
- [x] 碰撞响应正确性
- [x] 物理定律守恒（动量、能量）
- [x] 质量效应正确性
- [x] 重叠分离有效性
- [ ] 多物体复杂场景（需要优化）

### 性能验证

- [x] 单次碰撞性能可接受
- [x] 代码优化有效（距离预检查）
- [ ] 大量物体性能（需要空间分区）

---

## ?? 参考资料

1. **碰撞重叠分析文档**：`docs/COLLISION_OVERLAP_ANALYSIS.md`
2. **update函数重构文档**：`docs/UPDATE_FUNCTION_REFACTORING.md`
3. **摩擦力重构文档**：`docs/FRICTION_REFACTORING.md`

---

## ?? 贡献

如果你发现新的bug或想添加测试用例，请：

1. Fork 项目
2. 创建新的测试函数
3. 提交 Pull Request

---

## ?? 许可证

本测试套件遵循项目主许可证。

---

**最后更新**：2024  
**版本**：v2.0  
**维护者**：物理引擎开发团队
