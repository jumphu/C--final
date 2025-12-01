# PhysicalWorldWithSlope 测试文件清单

## 已创建的测试文件

### 测试代码文件

1. **tests/test_slope_physics.cpp**
   - 类型：完整测试套件
   - 测试数量：10个独立测试
   - 代码行数：~900行
   - 功能：全面测试所有功能

2. **tests/test_slope_quick.cpp**
   - 类型：快速验证测试
   - 测试数量：1个集成测试
   - 代码行数：~200行
   - 功能：快速验证核心功能

### 运行脚本

1. **run_slope_test.bat**
   - 编译并运行完整测试套件
   - 适用于Windows系统

2. **run_slope_quick_test.bat**
   - 编译并运行快速测试
   - 适用于Windows系统

### 文档文件

1. **tests/TEST_SLOPE_PHYSICS_DOCUMENTATION.md**
   - 详细的测试文档
   - 包含所有测试说明
   - 包含预期结果和错误排查

## 快速开始

### 运行完整测试
```batch
cd C:\project-C
run_slope_test.bat
```

### 运行快速测试
```batch
cd C:\project-C
run_slope_quick_test.bat
```

## 测试覆盖范围

### ? 已测试功能

#### 基础功能
- [x] 斜面创建和属性设置
- [x] 斜面几何计算（高度、底边、角度）
- [x] 斜面摩擦系数设置

#### 放置功能
- [x] 在底部放置物体
- [x] 在顶部放置物体
- [x] 按比例放置物体
- [x] 在指定X坐标放置物体

#### 几何计算
- [x] 法向量计算
- [x] 切向量计算
- [x] 表面高度计算
- [x] 端点坐标获取

#### 检测功能
- [x] 点在斜面上检测
- [x] 形状支撑检测
- [x] 边界检测

#### 物理模拟
- [x] 重力分解（沿斜面和垂直斜面）
- [x] 摩擦力计算
- [x] 物体在斜面上滑动
- [x] 多物体同时模拟

#### 斜面管理
- [x] 添加斜面
- [x] 移除斜面
- [x] 查找斜面
- [x] 清空斜面
- [x] 获取斜面数量

#### 状态管理
- [x] 启动物理世界
- [x] 暂停模拟
- [x] 继续模拟
- [x] 停止模拟

### ?? 测试统计

| 测试类别 | 测试数量 | 状态 |
|---------|---------|------|
| 基础功能测试 | 3 | ? 完成 |
| 放置功能测试 | 4 | ? 完成 |
| 几何计算测试 | 4 | ? 完成 |
| 物理模拟测试 | 5 | ? 完成 |
| 管理功能测试 | 2 | ? 完成 |
| **总计** | **18** | **? 完成** |

## 测试结果预期

### 完整测试（test_slope_physics.cpp）

**预期输出**：
```
========================================
   PhysicalWorldWithSlope 测试套件
========================================

测试1：创建斜面和基本属性
========================================
[PASS] 斜面创建成功
[PASS] 斜面名称正确
[PASS] 斜面长度正确
...
? 测试通过

测试2：在斜面上放置物体
========================================
[PASS] 在斜面底部放置球
[PASS] 在斜面顶部放置球
...
? 测试通过

...（其他8个测试）...

测试总结：
总测试数: 10
通过: 10
失败: 0
通过率: 100.0%

?? 所有测试通过！
```

### 快速测试（test_slope_quick.cpp）

**预期输出**：
```
========== 快速测试：斜面物理世界 ==========

? 创建物理世界成功
? 创建斜面成功
  名称: TestSlope
  长度: 10 m
  角度: 30 度
  ...

? 在斜面顶部放置球
  位置: (8.66, 5.5)

? 球正确放置在斜面上

========== 物理模拟 ==========
...

? 所有快速测试通过！
```

## 使用示例

### 示例1：运行单个测试
```cpp
// 只测试斜面创建
bool result = test_slope_creation();
if (result) {
    std::cout << "测试通过" << std::endl;
}
```

### 示例2：自定义测试参数
```cpp
// 修改重力
PhysicalWorldWithSlope world;
world.setGravity(5.0);  // 火星重力

// 修改时间步长
world.setTimeStep(1.0/120.0);  // 120 FPS
```

### 示例3：添加调试输出
```cpp
// 在测试中添加详细输出
world.printAllShapes();
world.printDynamicShapes();
```

## 已知问题和限制

### 当前限制
1. 仅支持Circle和AABB形状
2. 斜面假设为直线（不支持曲面）
3. 容差值固定，可能需要调整

### 待改进项
1. 添加更多形状类型支持
2. 改进支撑检测算法
3. 优化物理计算性能

## 贡献测试

如需添加新测试：

1. **在 test_slope_physics.cpp 中添加**：
```cpp
bool test_new_feature() {
    std::cout << "\n测试X：新功能测试" << std::endl;
    printSeparator();
    
    // 测试代码
    bool test1 = ...;
    printTestResult("测试描述", test1);
    
    return test1;
}
```

2. **在 main() 中注册**：
```cpp
TestCase tests[] = {
    // ...existing tests...
    {"新功能测试", test_new_feature}
};
```

## 测试环境要求

### 编译器
- GCC 4.8+ 或 MSVC 2015+
- 支持 C++11 标准

### 依赖
- shapes.h / shapes.cpp
- physicalWorldWithSlope.h / physicalWorldWithSlope.cpp

### 操作系统
- Windows 7+
- Linux (通过修改批处理文件为shell脚本)
- macOS (通过修改批处理文件为shell脚本)

## 持续集成

### 建议的CI配置
```yaml
# .github/workflows/test.yml
- name: Run Slope Tests
  run: |
    g++ -std=c++11 -I./include -o test tests/test_slope_physics.cpp src/*.cpp
    ./test
```

## 相关链接

- [Slope放置方法文档](../docs/SLOPE_PLACEMENT_METHODS.md)
- [实现总结](../docs/SLOPE_PLACEMENT_IMPLEMENTATION_SUMMARY.md)
- [快速参考](../docs/SLOPE_QUICK_REFERENCE.md)

---

## 测试检查清单

在提交代码前，请确认：

- [ ] 所有测试编译通过
- [ ] 所有测试运行通过
- [ ] 无内存泄漏
- [ ] 无编译警告
- [ ] 文档已更新
- [ ] 代码已注释

---

**版本**：1.0  
**状态**：? 完成  
**维护者**：项目团队
