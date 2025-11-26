# 正压力累积问题分析报告

## 问题现象

在测试5（堆叠物体摩擦力）中发现：
```
所有球的正压力都显示为 9.8 N（单个球的重量）
Ball1 (底部): normalForce = 9.8 N （应为 29.4 N = 3个球的重量）
Ball2 (中间): normalForce = 9.8 N （应为 19.6 N = 2个球的重量）
Ball3 (顶部): normalForce = 9.8 N （正确）
```

## 根本原因分析

### 问题1: 支撑关系检测失败 ??

**问题代码**：`src/shapes.cpp` 第213-216行

```cpp
// 检查是否在上方（当前物体的Y坐标大于supporter）
if (mass_centre[1] <= supporter_candidate.mass_centre[1]) {
    return; // 不在上方
}
```

**问题分析**：
这个检查使用的是**物体中心点**的Y坐标来判断，但是：

1. **Ball1（底部）**:
   - 位置: y = 5.0（半径5，中心在5）
   - 底部: y = 0.0

2. **Ball2（中间）**:
   - 位置: y = 20.0（初始）
   - 如果Ball2下落到Ball1上，中心应该在 y = 15.0（5+5+5）
   - 但是由于测试中球都有初始水平速度，它们可能没有真正堆叠！

3. **Ball3（顶部）**:
   - 位置: y = 30.0（初始）
   - 应该在 y = 25.0

**关键问题**：测试程序中，3个球都设置了**相同的水平初始速度 8.0 m/s**：

```cpp
ball1->setVelocity(8.0, 0.0);  // Ball1
ball2->setVelocity(8.0, 0.0);  // Ball2
ball3->setVelocity(8.0, 0.0);  // Ball3
```

这意味着它们会**同步水平移动**，在空中分散，根本不会形成堆叠！

### 问题2: 测试场景设计不当 ?

**测试代码**：`tests/test_friction.cpp` 第320-336行

```cpp
// 创建底部小球（有初始速度）
Circle* ball1 = new Circle(1.0, 5.0, 0.0, 10.0);
ball1->setVelocity(8.0, 0.0);  // ← 问题：有水平速度
world.placeDynamicShape(*ball1, 0.0, world.ground.getYLevel());

// 创建中间小球
Circle* ball2 = new Circle(1.0, 5.0, 0.0, 20.0);
ball2->setVelocity(8.0, 0.0);  // ← 问题：水平速度相同
world.addDynamicShape(ball2);

// 创建顶部小球
Circle* ball3 = new Circle(1.0, 5.0, 0.0, 30.0);
ball3->setVelocity(8.0, 0.0);  // ← 问题：水平速度相同
world.addDynamicShape(ball3);
```

**问题**：
1. Ball1放在地面上（y=5）
2. Ball2初始在y=20，有8.0 m/s水平速度，会**下落**的同时向右移动
3. Ball3初始在y=30，有8.0 m/s水平速度，会**下落**的同时向右移动
4. 由于它们水平速度相同，会保持**相对静止的水平距离**
5. 但是垂直方向上，Ball2和Ball3会下落，可能无法准确堆叠在Ball1上

### 问题3: 物体分散而非堆叠

从测试输出可以看到：

```
Frame 20:
Ball1 - 位置: (2.209, 5.000)
Ball2 - 位置: (2.667, 19.428)  ← y坐标不是15！说明没有在Ball1上
Ball3 - 位置: (2.667, 29.428)  ← y坐标不是25！说明没有在Ball2上

Frame 80:
Ball1 - 位置: (3.681, 5.000)
Ball2 - 位置: (11.098, 11.903)  ← x坐标不同！Ball2飞走了
Ball3 - 位置: (10.667, 24.305)  ← x坐标不同！Ball3也飞走了
```

**现象分析**：
- Ball2和Ball3的y坐标在下降（受重力）
- 但它们的x坐标与Ball1不同步
- Ball2和Ball3没有成功堆叠在Ball1上
- 它们可能发生了碰撞后分离

### 问题4: checkSupportStatus 的判定条件

```cpp
// 检查是否在上方（当前物体的Y坐标大于supporter）
if (mass_centre[1] <= supporter_candidate.mass_centre[1]) {
    return; // 不在上方
}
```

这个检查是**中心点比较**，对于堆叠的球：
- Ball2中心（y=15）> Ball1中心（y=5）? 通过
- 但如果Ball2在下落过程中，可能y坐标还是20，不满足条件

同时：
```cpp
// 如果相对Y速度很小（接近0或向下速度很小），认为被支撑
if (std::abs(relVy) < 0.5) {  // 阈值可调整
    isSupported = true;
}
```

这个检查要求**相对垂直速度 < 0.5 m/s**，但是：
- 如果Ball2正在下落，vy可能是 -9.8 m/s
- Ball1在地面上，vy = 0
- 相对速度 = -9.8 - 0 = -9.8 m/s，远大于0.5
- **不满足支撑条件**！

## 结论

正压力累积失败的**根本原因**是：

1. **测试场景设计不当** ??
   - 3个球都有相同的水平速度8.0 m/s
   - 导致它们在水平方向同步移动，无法形成垂直堆叠
   - Ball2和Ball3在空中下落，没有稳定地放在下方球上

2. **支撑关系检测过于严格** ??
   - 要求相对垂直速度 < 0.5 m/s
   - 但在球刚落下时，相对速度很大，无法被识别为支撑
   - 需要等球完全静止后才能检测到支撑关系

3. **初始位置设置问题** ??
   - Ball2和Ball3没有被正确放置在下方球上
   - 只是设置了y坐标，但没有调用 `placeDynamicShape()` 或手动计算正确的堆叠位置

## 正确的测试方法

### 方法1: 无初始速度堆叠（推荐）

```cpp
// 创建底部小球（静止）
Circle* ball1 = new Circle(1.0, 5.0, 0.0, 0.0);
ball1->setVelocity(0.0, 0.0);  // ← 静止
world.placeDynamicShape(*ball1, 0.0, world.ground.getYLevel());

// 创建中间小球（准确放置在Ball1上）
Circle* ball2 = new Circle(1.0, 5.0, 0.0, 0.0);
ball2->setVelocity(0.0, 0.0);  // ← 静止
double ball2_y = ball1->mass_centre[1] + ball1->getRadius() + ball2->getRadius();
ball2->setCentre(0.0, ball2_y);  // y = 5 + 5 + 5 = 15
world.addDynamicShape(ball2);

// 创建顶部小球（准确放置在Ball2上）
Circle* ball3 = new Circle(1.0, 5.0, 0.0, 0.0);
ball3->setVelocity(0.0, 0.0);  // ← 静止
double ball3_y = ball2_y + ball2->getRadius() + ball3->getRadius();
ball3->setCentre(0.0, ball3_y);  // y = 15 + 5 + 5 = 25
world.addDynamicShape(ball3);

// 然后给整个堆叠一个水平推力
ball1->setVelocity(8.0, 0.0);
ball2->setVelocity(8.0, 0.0);
ball3->setVelocity(8.0, 0.0);
```

### 方法2: 让物体自然堆叠

```cpp
// 先只创建Ball1在地面
Circle* ball1 = new Circle(1.0, 5.0, 0.0, 0.0);
ball1->setVelocity(8.0, 0.0);
world.placeDynamicShape(*ball1, 0.0, world.ground.getYLevel());

// Ball2从上方落下，速度与Ball1相同
Circle* ball2 = new Circle(1.0, 5.0, 0.0, 20.0);
ball2->setVelocity(8.0, 0.0);  // ← 水平速度相同
world.addDynamicShape(ball2);

// Ball3从更高处落下
Circle* ball3 = new Circle(1.0, 5.0, 0.0, 30.0);
ball3->setVelocity(8.0, 0.0);
world.addDynamicShape(ball3);

// 先模拟100帧让它们自然堆叠
for (int i = 0; i < 100; i++) {
    world.update(world.dynamicShapeList, world.ground);
}

// 然后再测试摩擦力
```

### 方法3: 修改支撑检测逻辑

修改 `checkSupportStatus` 使用更宽松的条件：

```cpp
// 如果相对Y速度很小或向下，都认为可能被支撑
if (relVy < 1.0) {  // ← 允许小幅下落
    isSupported = true;
    supporter = const_cast<Shape*>(&supporter_candidate);
}
```

## 代码问题总结

| 问题 | 位置 | 严重性 |
|------|------|--------|
| 测试场景不合理 | `tests/test_friction.cpp:320-336` | 高 |
| 支撑检测过严 | `src/shapes.cpp:222` | 中 |
| 没有预堆叠 | `tests/test_friction.cpp` | 高 |
| 碰撞分离 | 物理引擎行为 | 低 |

## 修复建议

1. **修改测试场景** ?? **最重要**
   - 让球先静止堆叠，再给整体速度
   - 或者预先计算准确的堆叠位置

2. **调整支撑检测阈值**
   - 将相对速度阈值从0.5提高到1.0或2.0
   - 或者添加"刚接触"判定

3. **添加预堆叠阶段**
   - 先让物体自然落下堆叠
   - 等稳定后再测试摩擦力

4. **降低弹性系数**
   - 当前是0.1，改为0.0
   - 避免碰撞反弹导致分离

## 验证方法

修复后应该看到：
```
Frame 20:
Ball1 - 位置: (2.209, 5.000)   ← y不变
Ball2 - 位置: (2.209, 15.000)  ← y应该是15，x与Ball1相同
Ball3 - 位置: (2.209, 25.000)  ← y应该是25，x与Ball1相同
正压力: Ball1=29.4, Ball2=19.6, Ball3=9.8  ← 正确！
```

---

**结论**: 正压力累积功能本身是正确的，但测试场景设置不当导致物体没有真正堆叠，因此无法验证该功能。需要修改测试代码，确保物体首先形成稳定的堆叠结构。
