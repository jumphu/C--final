# vcpkg集成指南

## vcpkg最佳实践

### 为什么不需要导入到项目目录？

1. **全局管理**: vcpkg作为系统级包管理器，安装的库可以被多个项目共享
2. **版本控制友好**: 避免将大量第三方库文件提交到git仓库
3. **自动化构建**: CMake自动找到并链接vcpkg安装的库
4. **依赖管理**: 通过`vcpkg.json`文件声明依赖，自动安装

### 配置步骤

#### 1. 确保vcpkg已正确安装
```cmd
# 检查vcpkg环境变量
echo %VCPKG_ROOT%

# 如果未设置，需要设置环境变量
setx VCPKG_ROOT "C:\path\to\your\vcpkg"
```

#### 2. 项目构建时自动集成
```cmd
# 在项目根目录执行
mkdir build
cd build

# CMake会自动使用vcpkg toolchain
cmake ..
cmake --build .
```

### 添加新依赖的方法

#### 方法1: 修改vcpkg.json（推荐）
在`vcpkg.json`中添加依赖：
```json
{
  "dependencies": [
    "fmt",
    "spdlog",
    "catch2"
  ]
}
```

#### 方法2: 手动安装
```cmd
vcpkg install fmt:x64-windows
vcpkg install spdlog:x64-windows
```

### 在CMakeLists.txt中使用库

```cmake
# 查找并链接vcpkg安装的库
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(your_target 
    fmt::fmt
    spdlog::spdlog
)
```

### 常用的vcpkg库示例

#### 日志库
```json
"spdlog"  # 高性能日志库
```

#### 格式化库
```json
"fmt"     # 现代C++格式化库
```

#### 测试框架
```json
"catch2"  # 轻量级测试框架
"gtest"   # Google测试框架
```

#### JSON处理
```json
"nlohmann-json"  # 现代JSON库
```

#### 图形处理
```json
"opencv"  # 计算机视觉库
"sfml"    # 简单快速媒体库
```

#### 网络库
```json
"cpprest"     # REST客户端库
"boost-beast" # HTTP/WebSocket库
```

### 项目结构保持不变

vcpkg集成后，您的项目结构依然保持标准：

```
project-C/
├── src/          # 只包含您的源代码
├── include/      # 只包含您的头文件  
├── vcpkg.json    # 依赖声明文件（小文件）
└── ...
```

**第三方库不会出现在您的项目目录中！**

### CI/CD集成

在CI环境中，可以这样配置：

```yaml
# GitHub Actions示例
- name: Setup vcpkg
  uses: microsoft/setup-vcpkg@v1
  
- name: Configure CMake
  run: cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
```

### 跨平台支持

vcpkg支持多平台，同一个`vcpkg.json`文件可以在Windows、Linux、macOS上使用：

```cmd
# Windows
vcpkg install --triplet x64-windows

# Linux  
vcpkg install --triplet x64-linux

# macOS
vcpkg install --triplet x64-osx
```