# 实现可视化菜单与物理引擎接口的实施指南

本指南旨在指导开发者在现有项目中实现可视化菜单（UI）控制物理引擎（PhysicalWorld）的功能，并建立从物理引擎到可视化渲染（Renderer）的状态同步。

## 一、 整体架构

我们将采用 **Adapter/Bridge (桥接) 模式** 来解耦 UI、渲染器和物理引擎。
*   **输入流**: UI 按钮状态 -> `PhysicsRenderBridge` (适配器) -> `PhysicalWorld`。
*   **输出流**: `PhysicalWorld` -> `PhysicsRenderBridge` (适配器) -> `Renderer`。

## 二、 涉及文件清单

需要修改或创建以下 5 类文件：

1.  **桥接模块 (新建/核心)**
    *   `src/PhysicsRenderBridge.cpp`: 实现物理世界与渲染、UI 之间的逻辑转换。
    *   `include/PhysicsRenderBridge.h`: 定义桥接接口。
2.  **主程序 (重构)**
    *   `src/main.cpp`: 重写主循环，驱动整个系统。
3.  **UI 模块 (修改)**
    *   `include/allbottums.h`: 修正拼写，清理接口，确保包含所有按钮声明。
    *   `src/control_buttons.cpp`: 确保提供获取按钮状态的接口（Getter）。
    *   `src/additional_buttons.cpp` (如果有): 确保提供参数输入的获取接口。
4.  **构建系统 (修改)**
    *   `CMakeLists.txt`: 将新文件加入构建列表。
5.  **辅助文件**
    *   `include/PhysicalWorld.h`: 确认 `getGravity`, `setGravity` 等接口可用。

---

## 三、 具体实施步骤

### 步骤 1: 创建 PhysicsRenderBridge.cpp

这是最关键的一步。你需要实现 `syncInputToPhysics` 和 `syncPhysicsToRender`。

**文件**: `src/PhysicsRenderBridge.cpp`

```cpp
#include "PhysicsRenderBridge.h"
#include <iostream>

// 构造函数
PhysicsRenderBridge::PhysicsRenderBridge(std::unique_ptr<IPhysicalWorldAdapter> adapter, Renderer* renderer)
    : physics_adapter_(std::move(adapter)), renderer_(renderer), pixels_per_meter_(100.0) {
    // 默认缩放: 100像素 = 1米
}

// 核心功能1: 将物理对象同步给渲染器
void PhysicsRenderBridge::syncPhysicsToRender() {
    // 1. 获取物理世界所有形状
    std::vector<Shape*> shapes = physics_adapter_->getAllShapes();
    
    // 2. 清空旧的渲染列表 (或者只更新位置以提高性能)
    render_objects_.clear();

    for (Shape* shape : shapes) {
        RenderObject obj;
        obj.name = shape->getName();
        // 这里的颜色获取可能需要根据 Shape 的扩展属性，或者在 Bridge 中维护一个 Map
        obj.color = RED; // 示例默认值

        // 根据类型转换
        if (shape->getType() == "Circle") {
            obj.type = RenderObject::BALL;
            double x, y;
            shape->getCentre(x, y); // 获取物理坐标 (米)
            obj.ball.x = x;
            obj.ball.y = y;
            // 获取半径需要向下转型
            Circle* c = dynamic_cast<Circle*>(shape);
            if(c) obj.ball.radius = c->getRadius();
        } 
        else if (shape->getType() == "AABB") {
            obj.type = RenderObject::BLOCK;
            double x, y;
            shape->getCentre(x, y);
            obj.block.cx = x;
            obj.block.cy = y;
             AABB* b = dynamic_cast<AABB*>(shape);
             if(b) {
                 obj.block.width = b->getWidth();
                 obj.block.height = b->getHeight();
             }
        }
        // ... 添加其他形状的处理 (Wall, Ground)

        // 存入渲染列表
        render_objects_[obj.name] = obj;
    }
}

// 核心功能2: 将 UI 输入同步给物理引擎
void PhysicsRenderBridge::syncInputToPhysics(const std::vector<UserInput>& inputs) {
    // 1. 同步全局按钮状态 (Start/Stop/Pause)
    // 注意：这里假设你已经在 main.cpp 中调用了 UI 的 update 函数并获取了状态
    if (button_states_.start_clicked) {
        physics_adapter_->start();
    }
    if (button_states_.pause_clicked) {
        physics_adapter_->pause();
    }
    if (button_states_.stop_clicked) {
        physics_adapter_->stop();
        physics_adapter_->clear(); // 停止通常意味着重置
    }

    // 2. 同步参数修改 (重力、摩擦力)
    // 假设 button_states_ 已经从 UI 模块获取了最新值
    double new_gravity = button_states_.gravity_coefficient; 
    // 只有当值改变时才调用 setGravity，避免不必要的开销
    if (std::abs(physics_adapter_->getGravity() - new_gravity) > 0.01) {
        physics_adapter_->setGravity(new_gravity);
    }
}

// 辅助: 物理坐标 -> 屏幕坐标
POINT PhysicsRenderBridge::physicsToScreen(double phys_x, double phys_y) const {
    POINT p;
    // 屏幕原点通常在左上角，物理原点可能在左下角或中心
    // 假设物理原点 (0,0) 对应屏幕中心 (400, 300)
    p.x = 400 + (int)(phys_x * pixels_per_meter_);
    p.y = 600 - (int)(phys_y * pixels_per_meter_); // Y轴翻转
    return p;
}
```

### 步骤 2: 重构 main.cpp

重写 `src/main.cpp` 以连接所有组件。

**文件**: `src/main.cpp`

```cpp
#include <graphics.h>
#include <vector>
#include <iostream>

#include "Renderer.h"
#include "physicalWorld.h"
#include "PhysicsRenderBridge.h"
#include "allbottums.h" // 注意文件名拼写

// 简单的 Adapter 实现，包装 PhysicalWorld
class MyWorldAdapter : public IPhysicalWorldAdapter {
    PhysicalWorld* world;
public:
    MyWorldAdapter() { world = new PhysicalWorld(); }
    ~MyWorldAdapter() { delete world; }
    
    // 实现接口...
    std::vector<Shape*> getAllShapes() const override { 
        // 合并 dynamic 和 static 列表返回
        std::vector<Shape*> all;
        all.insert(all.end(), world->dynamicShapeList.begin(), world->dynamicShapeList.end());
        // ...
        return all;
    }
    void start() override { world->Continue(); } // PhysicalWorld 使用 Continue 代表开始
    void pause() override { world->Pause(); }
    void update(double dt) override { world->update(world->dynamicShapeList, dt, world->ground); }
    // ... 其他实现
};

int main() {
    // 1. 初始化图形窗口
    initgraph(800, 600, EX_SHOWCONSOLE);

    // 2. 初始化模块
    Renderer renderer(800, 600, 100.0);
    auto adapter = std::make_unique<MyWorldAdapter>();
    PhysicsRenderBridge bridge(std::move(adapter), &renderer);

    // 3. 初始化 UI 按钮位置
    int btnX, startY, pauseY, stopY, btnW, btnH;
    initButtons(btnX, startY, pauseY, stopY, btnW, btnH, 20);

    // 4. 主循环
    BeginBatchDraw();
    while (true) {
        // --- A. 输入处理 ---
        handleMouseInput(btnX, startY, pauseY, stopY, btnW, btnH);
        
        // 更新 Bridge 的按钮状态
        // 这里需要 Bridge 提供一个 updateButtonState 的方法来手动设置
        // 或者直接让 Bridge 访问全局变量 (虽然不优雅，但现有代码如此)
        bridge.getButtonStatesMutable().start_clicked = getStartButtonState();
        bridge.getButtonStatesMutable().pause_clicked = getPauseButtonState();
        // ...

        // --- B. 逻辑更新 ---
        // 将 UI 输入应用到物理世界
        bridge.syncInputToPhysics({}); 
        
        // 物理模拟步进 (如果未暂停)
        if (!bridge.isPaused()) {
             // 实际上 update 在 bridge 内部还是外部调用取决于设计
             // 建议在 Bridge 中提供 UpdatePhysics(dt)
             // bridge.UpdatePhysics(0.016); 
        }

        // --- C. 渲染 ---
        cleardevice();
        
        // 1. 同步物理对象到渲染列表
        bridge.syncPhysicsToRender();
        
        // 2. 绘制所有物理对象
        const auto& objs = bridge.getRenderObjects();
        for(const auto& pair : objs) {
            const RenderObject& obj = pair.second;
            if(obj.type == RenderObject::BALL) {
                 renderer.DrawBall(obj.ball); // 需要适配 DrawBall 接收的数据结构
            }
            // ...
        }

        // 3. 绘制 UI
        drawButtons(btnX, startY, pauseY, stopY, btnW, btnH);
        
        FlushBatchDraw();
        Sleep(16); // ~60 FPS
    }
    EndBatchDraw();
    return 0;
}
```

### 步骤 3: 修正 UI 接口

确保 `control_buttons.cpp` 能被正确链接。
如果在 `main.cpp` 中使用了 `getStartButtonState()`，则必须在 `allbottums.h` 中正确声明它。

**文件**: `include/allbottums.h` (建议重命名为 `allbuttons.h`)

```cpp
#ifndef ALL_BUTTONS_H
#define ALL_BUTTONS_H

// 确保声明了这些函数
bool getStartButtonState();
bool getPauseButtonState();
bool getStopButtonState();
void resetButtonStates();

// ... 其他声明
#endif
```

## 四、 关键注意事项

1.  **坐标系转换**: 物理引擎使用**米**和**标准笛卡尔坐标**（Y轴向上），EasyX 使用**像素**和**屏幕坐标**（Y轴向下）。必须在 Bridge 或 Renderer 中做明确的转换。建议在 Bridge 的 `physicsToScreen` 中统一处理。
2.  **对象生命周期**: `PhysicalWorld` 管理 Shape 的内存（通常是 `new` 出来的）。`Renderer` 只是读取数据进行绘制，不要在 Renderer 中 `delete` 物理对象。
3.  **全局变量**: 现有的 UI 代码大量使用了全局变量。在重构初期可以容忍，但建议尽快将其封装到类中。

通过以上步骤，你就可以建立起一个从“点击按钮”到“改变物理重力”，再到“屏幕上球体下落速度变化”的完整闭环。
