# Project-C: 形状处理库

一个用C++编写的形状处理库，支持圆形和矩形的创建、操作和碰撞检测。

## 项目结构

```
project-C/
├── src/                    # 源代码文件
│   ├── main.cpp           # 主程序演示
│   └── shapes.cpp         # 形状类实现
├── include/               # 头文件
│   └── project-C/
│       └── shapes.h       # 形状类声明
├── lib/                   # 静态库文件
├── bin/                   # 可执行文件输出目录
├── build/                 # 编译临时文件目录
├── tests/                 # 测试代码
├── docs/                  # 文档
├── examples/              # 示例代码
├── CMakeLists.txt         # CMake构建配置
├── README.md             # 项目说明（本文件）
└── .gitignore            # Git忽略文件配置
```

## 功能特性

### 支持的形状类型
- **Circle（圆形）**: 支持半径、质心、质量等属性
- **AABB（轴对齐边界框）**: 支持宽度、高度、质心、质量等属性

### 核心功能
- 形状创建和初始化
- 移动和旋转操作
- 精确的碰撞检测
- 面积和周长计算
- 多态性支持

## 编译说明

### 前置要求
- CMake 3.16 或更高版本
- C++17 兼容的编译器
  - Windows: Visual Studio 2019 或更高版本
  - Linux/macOS: GCC 7+ 或 Clang 6+
- vcpkg包管理器（推荐）

### vcpkg集成（推荐）

本项目支持vcpkg包管理器自动集成。确保已安装vcpkg并设置环境变量：

```cmd
# 设置vcpkg环境变量
setx VCPKG_ROOT "C:\path\to\your\vcpkg"
```

项目使用`vcpkg.json`管理依赖，无需手动导入第三方库到项目目录。详细配置请参考[vcpkg集成指南](docs/VCPKG_INTEGRATION.md)。

### 构建步骤

#### Windows (使用 vcpkg + Visual Studio) - 推荐
```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

#### Windows (使用 vcpkg + MinGW)
```cmd
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

#### Windows (使用 Visual Studio，无vcpkg)
```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

#### Linux/macOS
```bash
mkdir build
cd build
cmake ..
make
```

### 运行程序
```cmd
# Windows
bin\shape_demo.exe

# Linux/macOS
./bin/shape_demo
```

## API 使用示例

### 基本用法

```cpp
#include "project-C/shapes.h"
using namespace project_c;

// 创建圆形
Circle circle(2.0, 5.0, 0.0, 0.0);  // 质量, 半径, x, y

// 创建矩形
AABB rect(3.0, 4.0, 6.0, 0.0, 0.0);  // 质量, 宽度, 高度, x, y

// 移动形状
circle.move(1.0, 2.0);

// 碰撞检测
bool collision = circle.check_collision(rect);

// 获取属性
double area = circle.getArea();
double mass = circle.getMass();
```

### 多态性示例

```cpp
std::vector<std::unique_ptr<Shape>> shapes;
shapes.push_back(std::make_unique<Circle>(1.0, 2.0, 3.0));
shapes.push_back(std::make_unique<AABB>(2.0, 3.0, 4.0, 5.0));

for (auto& shape : shapes) {
    shape->move(1.0, 1.0);  // 多态调用
}
```

## 测试

运行单元测试：
```cmd
# 在build目录中
ctest
# 或者直接运行测试程序
bin/test_shapes
```

## 开发计划

- [ ] 添加三角形支持
- [ ] 实现物理仿真
- [ ] 添加图形渲染功能
- [ ] 优化碰撞检测算法
- [ ] 添加序列化支持

## 贡献

欢迎提交 Issue 和 Pull Request！

## 许可证

本项目采用 MIT 许可证 - 详情请看 [LICENSE](LICENSE) 文件。