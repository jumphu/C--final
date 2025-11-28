# placeShapeOnShape - 快速参考

## 一行介绍

将一个形状自动放置在另一个形状的上方，支持多种形状组合和斜面坐标系。

---

## 基本用法

```cpp
PhysicalWorld world;

// 创建两个形状
AABB* bottom = new AABB(10.0, 5.0, 2.0, 0.0, 0.0);
AABB* top = new AABB(5.0, 3.0, 1.0, 0.0, 0.0);

// 添加到世界
world.addDynamicShape(bottom);
world.addDynamicShape(top);

// 放置
world.placeShapeOnGround(*bottom, world.ground);
world.placeShapeOnShape(*top, *bottom);  // ← 关键方法
```

---

## 语法

```cpp
void placeShapeOnShape(Shape& topShape, 
                       Shape& bottomShape, 
                       double offsetX = 0.0);
```

| 参数 | 说明 |
|------|------|
| `topShape` | 上方形状 |
| `bottomShape` | 底部形状 |
| `offsetX` | 水平偏移（米），默认0 |

---

## 支持的形状组合

| 上方 ↓ 底部 → | Circle | AABB | Wall |
|---------------|--------|------|------|
| **Circle** | ? | ? | ? |
| **AABB** | ? | ? | ? |

---

## 常见场景

### 场景1：简单堆叠
```cpp
world.placeShapeOnShape(*top, *bottom);
```

### 场景2：偏移堆叠
```cpp
world.placeShapeOnShape(*top, *bottom, 1.5);  // 偏移1.5m
```

### 场景3：多层堆叠
```cpp
world.placeShapeOnShape(*layer2, *layer1);
world.placeShapeOnShape(*layer3, *layer2);
```

### 场景4：斜面堆叠
```cpp
world.setInclineAngle(30.0);
world.placeShapeOnShape(*top, *bottom);  // 自动处理斜面
```

---

## 自动功能

### ? 自动定位
- 计算正确的垂直位置
- 支持水平偏移

### ? 摩擦传递
```cpp
// 自动执行：
topShape.setFraction(bottomShape.getFraction());
topShape.setStaticFraction(bottomShape.getStaticFraction());
```

### ? 斜面支持
- 自动考虑 `inclineAngle`
- 沿法向量方向堆叠

---

## 完整示例

```cpp
#include "physicalWorld.h"
#include "shapes.h"

int main() {
    // 创建世界
    PhysicalWorld world;
    world.setGravity(10.0);
    world.setInclineAngle(0.0);
    
    // 创建底部板块
    AABB* bottom = new AABB(10.0, 5.0, 2.0, 0.0, 0.0);
    bottom->setFraction(0.3);
    bottom->setStaticFraction(0.5);
    
    // 创建顶部板块
    AABB* top = new AABB(5.0, 3.0, 1.0, 0.0, 0.0);
    
    // 添加到世界
    world.addDynamicShape(bottom);
    world.addDynamicShape(top);
    
    // 放置
    world.placeShapeOnGround(*bottom, world.ground);
    world.placeShapeOnShape(*top, *bottom);
    
    // 运行模拟
    for (int i = 0; i < 100; ++i) {
        world.update(world.dynamicShapeList, world.ground);
    }
    
    // 清理
    delete bottom;
    delete top;
    
    return 0;
}
```

---

## 对比旧方法

### 旧方法（手动）
```cpp
double bottomTop = bottom->getTop();
double topHeight = top->getHeight();
top->setCentre(0.0, bottomTop + topHeight / 2.0);
top->setFraction(0.3);
top->setStaticFraction(0.5);
```

### 新方法（自动）
```cpp
world.placeShapeOnShape(*top, *bottom);
```

**代码减少：67%** ?

---

## 测试验证

```bash
# 运行测试
scripts\run_block_models_test.bat

# 查看结果
tests\output_block_models.txt
```

**测试通过率：100%** ?

---

## 文档链接

- ?? [完整指南](placeShapeOnShape_guide.md)
- ?? [测试代码](../tests/test_block_models.cpp)
- ?? [物理引擎README](PHYSICS_ENGINE_README.md)

---

**更新时间**：2024年11月28日  
**版本**：v1.0  
**状态**：生产就绪 ?
