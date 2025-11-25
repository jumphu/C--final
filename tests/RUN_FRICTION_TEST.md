# 摩擦力滑行测试 - 运行指南

## 方法1：使用批处理脚本 (推荐)
```bash
.\compile_and_run_friction_test.bat
```

## 方法2：使用PowerShell脚本
```powershell
.\run_friction_test.ps1
```

## 方法3：手动编译和运行

### 步骤1：编译
在项目根目录下执行：
```bash
g++ -o tests/test_friction_sliding.exe tests/test_friction_sliding.cpp src/physicalWorld.cpp src/shapes.cpp -Iinclude -std=c++11
```

### 步骤2：运行测试
```bash
.\tests\test_friction_sliding.exe
```

## 预期输出

测试将执行以下4个场景：

### 测试1：基础摩擦滑行
- 初速度：20 m/s
- 摩擦系数：0.3
- 质量：10 kg
- 预期滑行距离：约68米
- 预期停止时间：约6.8秒

### 测试2：不同摩擦系数对比
- 测试摩擦系数：0.1, 0.3, 0.5, 0.7
- 验证：摩擦系数越大，滑行距离越短

### 测试3：不同初速度对比
- 测试初速度：5, 10, 20, 30 m/s
- 验证：滑行距离与初速度平方成正比

### 测试4：能量守恒验证
- 验证：初始动能 = 摩擦力做功
- 预期能量误差：< 5%

## 故障排除

### 如果编译失败
1. 检查是否安装了 g++ 编译器
   ```bash
   g++ --version
   ```

2. 检查文件路径是否正确
   ```bash
   dir tests\test_friction_sliding.cpp
   dir src\physicalWorld.cpp
   dir src\shapes.cpp
   ```

3. 检查头文件路径
   ```bash
   dir include\physicalWorld.h
   dir include\shapes.h
   ```

### 如果运行时出错
1. 检查是否有编译警告
2. 查看测试输出中的错误信息
3. 确认物理参数设置合理

## 测试结果判定

### 成功标准
- ? 模拟结果与理论计算误差 < 5%
- ? 能量守恒误差 < 5%
- ? 所有测试用例通过

### 如果测试失败
检查以下几点：
1. 摩擦力是否正确施加
2. 时间步长是否足够小
3. 数值积分是否准确
4. 速度更新逻辑是否正确

## 调试建议

### 减少输出频率
修改 `testBasicFrictionSliding()` 中的输出间隔：
```cpp
if (step % 100 == 0) {  // 改为更大的值，如 500
    printShapeInfo("", *box, step);
}
```

### 增加调试信息
在关键位置添加调试输出：
```cpp
std::cout << "摩擦力: " << friction << std::endl;
std::cout << "当前速度: " << vx << std::endl;
```

### 检查物理参数
确认以下参数：
- 重力加速度 g = 9.8 m/s?
- 摩擦系数 μ 在合理范围 (0.1 ~ 1.0)
- 时间步长 Δt = 0.01 s
- 速度阈值 = 0.01 m/s

## 理论验证公式

### 滑行距离
```
s = v?? / (2μg)
```

### 停止时间
```
t = v? / (μg)
```

### 速度随时间变化
```
v(t) = v? - μgt
```

### 能量守恒
```
?mv?? = μmgs
```

## 进一步测试

如果基础测试通过，可以尝试：
1. 更大的速度和摩擦系数
2. 不同质量的物体
3. 不同时间步长的影响
4. 斜面摩擦滑行
5. 多物体摩擦交互
