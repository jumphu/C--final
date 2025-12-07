# 场景管理器使用指南 (Scene Manager Guide)

我们已在 `src/main.cpp` 中实现了基于桥接模式 (Bridge Pattern) 的场景管理器。

## 功能说明

程序启动后进入 **主菜单 (Menu)**。您可以通过键盘数字键切换不同的物理模拟场景：

*   **1**: **小球碰撞 (Ball Collision)**
    *   两个小球（红色和蓝色）在空中或地面上。
*   **2**: **板块与墙碰撞 (Block vs Wall)**
    *   一个静态墙壁和一个动态板块。
*   **3**: **堆叠测试 (Stacking)**
    *   多个板块垂直堆叠测试物理引擎的支撑逻辑。
*   **4**: **斜面模拟 (Slope)**
    *   设置全局倾斜角 (`inclineAngle = 30°`) 并放置一个小球。
*   **M**: **返回菜单 (Return to Menu)**
    *   清空当前世界并重置状态。

## 代码结构

*   **`src/main.cpp`**: 包含 `main` 函数和 `setupScene` 函数。
*   **`setupScene(SceneType)`**: 负责调用 Bridge 接口来清空世界并创建特定场景的物体。
*   **`PhysicsRenderBridge`**: 新增了 `clearWorld()`, `setInclineAngle()`, `createWall()` 等接口以支持场景构建。

## 编译与运行

直接运行脚本：
```cmd
scripts\compile_full.bat
```
运行程序：
```cmd
bin\ProjectC.exe
```

## 扩展新场景

要在未来添加新场景：
1. 在 `src/main.cpp` 的 `SceneType` 枚举中添加新类型。
2. 在 `setupScene` 函数中添加对应的 `else if` 分支。
3. 在主循环的键盘检测部分添加按键绑定。
