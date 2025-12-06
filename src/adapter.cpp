// adapter.cpp - 最小可行物理可视化适配器实现
#include "adapter.h"
#include <algorithm>
#include <cmath>
#include <sstream>

// ==================== ObjectConnection 方法实现 ====================

void ObjectConnection::updateFromPhysics() {
    if (!physicsObject) return;
    
    // 获取当前位置和速度
    physicsObject->getCentre(lastX, lastY);
    physicsObject->getVelocity(lastVx, lastVy);
    
    // 根据类型获取特定属性
    switch (type) {
        case OBJ_CIRCLE: {
            Circle* circle = dynamic_cast<Circle*>(physicsObject);
            if (circle) radius = circle->getRadius();
            break;
        }
        case OBJ_AABB: {
            AABB* aabb = dynamic_cast<AABB*>(physicsObject);
            if (aabb) {
                width = aabb->getWidth();
                height = aabb->getHeight();
            }
            break;
        }
        case OBJ_SLOPE: {
            Slope* slope = dynamic_cast<Slope*>(physicsObject);
            if (slope) {
                length = slope->getLength();
                slopeAngle = slope->getAngle();
            }
            break;
        }
        default:
            break;
    }
}

BallData ObjectConnection::getBallData() const {
    BallData ball;
    ball.x = lastX;
    ball.y = lastY;
    ball.radius = radius;
    ball.vx = lastVx;
    ball.vy = lastVy;
    ball.mass = physicsObject ? physicsObject->getMass() : 1.0;
    ball.color = color;
    return ball;
}

BlockData ObjectConnection::getBlockData() const {
    BlockData block;
    block.cx = lastX;
    block.cy = lastY;
    block.width = width;
    block.height = height;
    block.angle = lastAngle;
    block.vx = lastVx;
    block.vy = lastVy;
    block.mass = physicsObject ? physicsObject->getMass() : 1.0;
    block.color = color;
    return block;
}

RampData ObjectConnection::getRampData() const {
    RampData ramp;
    // 斜坡从质心向两边延伸
    double halfLength = length / 2.0;
    ramp.x1 = lastX - halfLength * cos(slopeAngle);
    ramp.y1 = lastY - halfLength * sin(slopeAngle);
    ramp.x2 = lastX + halfLength * cos(slopeAngle);
    ramp.y2 = lastY + halfLength * sin(slopeAngle);
    
    // 获取摩擦系数
    if (physicsObject) {
        double frac;
        physicsObject->getFraction(frac);
        ramp.mu = frac;
    } else {
        ramp.mu = 0.1;
    }
    
    return ramp;
}

// ==================== PhysicsVisualAdapter 方法实现 ====================

// 构造函数
PhysicsVisualAdapter::PhysicsVisualAdapter(Renderer* rendererPtr)
    : renderer(rendererPtr),
      physicsWorld(nullptr),
      background(nullptr),
      musicPlayer(nullptr),
      nextObjectId(1),
      currentScene(SCENE_NONE),
      isSimulationRunning(false),
      isSimulationPaused(false),
      draggedObjectId(-1),
      isDragging(false),
      lastUpdateTime(0.0),
      accumulatedTime(0.0) {
    
    std::cout << "PhysicsVisualAdapter 创建" << std::endl;
}

// 析构函数
PhysicsVisualAdapter::~PhysicsVisualAdapter() {
    cleanup();
    std::cout << "PhysicsVisualAdapter 销毁" << std::endl;
}

// 初始化适配器
bool PhysicsVisualAdapter::initialize(int screenWidth, int screenHeight) {
    std::cout << "初始化适配器..." << std::endl;
    
    // 1. 创建物理世界
    // 【待确认】决策点3：物理世界的边界应该设置多大？
    // 当前方案：根据屏幕尺寸和渲染器缩放比例计算
    double worldWidth = screenWidth / renderer->GetScale();
    double worldHeight = screenHeight / renderer->GetScale();
    
    physicsWorld = new PhysicalWorld(
        -worldWidth/2, worldWidth/2,   // left, right
        -worldHeight/2, worldHeight/2  // bottom, top
    );
    
    if (!physicsWorld) {
        std::cerr << "错误：无法创建物理世界" << std::endl;
        return false;
    }
    
    // 2. 创建背景效果
    background = new DigitalRainBackgroundIntegrated();
    
    // 3. 创建音乐播放器
    musicPlayer = new MusicPlayer(screenWidth - 80, 80, 60);
    
    // 4. 设置初始物理参数
    physicsWorld->setGravity(uiParams.gravity);
    physicsWorld->setTimeStep(1.0/60.0); // 【待确认】决策点3：使用固定时间步长
    
    // 5. 初始化UI按钮
    // 这里需要调用 allbuttons.h 中的初始化函数
    // 由于 allbuttons.h 的具体实现未知，这里假设有初始化函数
    // initButtons(...);
    
    std::cout << "适配器初始化完成" << std::endl;
    std::cout << "世界边界: " << -worldWidth/2 << ", " << worldWidth/2 << ", "
              << -worldHeight/2 << ", " << worldHeight/2 << std::endl;
    
    return true;
}

// 主更新函数
void PhysicsVisualAdapter::updateFrame(float deltaTime) {
    // 1. 处理UI参数变化
    handleParameterChanges();
    
    // 2. 处理按钮点击
    handleButtonClicks();
    
    // 3. 如果模拟正在运行且未暂停，更新物理世界
    if (isSimulationRunning && !isSimulationPaused) {
        // 【待确认】决策点3：使用固定时间步长还是累积时间
        // 当前方案：使用固定时间步长，累积多余时间
        accumulatedTime += deltaTime;
        const double fixedTimeStep = physicsWorld->getTimeStep();
        
        while (accumulatedTime >= fixedTimeStep) {
            // 更新物理世界
            // 注意：physicalWorld.update() 需要形状列表和地面作为参数
            // 根据头文件，我们应该传递 dynamicShapeList 和 ground
            if (physicsWorld) {
                physicsWorld->update(physicsWorld->dynamicShapeList, 
                                    fixedTimeStep, 
                                    physicsWorld->ground);
            }
            
            accumulatedTime -= fixedTimeStep;
        }
        
        // 4. 同步所有物理对象到可视化系统
        for (auto& pair : objectConnections) {
            int adapterId = pair.first;
            ObjectConnection& conn = pair.second;
            
            // 更新物理状态
            conn.updateFromPhysics();
        }
    }
    
    // 5. 更新背景效果
    if (background) {
        // 背景更新在渲染时进行
    }
    
    // 6. 更新时间戳
    lastUpdateTime += deltaTime;
}

// 渲染函数
void PhysicsVisualAdapter::renderFrame() {
    if (!renderer) return;
    
    // 1. 开始新帧
    renderer->BeginFrame();
    
    // 2. 绘制背景
    if (background) {
        background->UpdateAndDraw();
    } else {
        renderer->Clear(WHITE);
    }
    
    // 3. 绘制所有物理对象
    for (const auto& pair : objectConnections) {
        const ObjectConnection& conn = pair.second;
        
        if (!conn.isVisible) continue;
        
        switch (conn.type) {
            case OBJ_CIRCLE: {
                BallData ball = conn.getBallData();
                renderer->DrawBall(ball);
                break;
            }
            case OBJ_AABB: {
                BlockData block = conn.getBlockData();
                renderer->DrawBlock(block);
                break;
            }
            case OBJ_SLOPE: {
                RampData ramp = conn.getRampData();
                renderer->DrawRamp(ramp);
                break;
            }
            case OBJ_WALL: {
                // 墙壁用矩形绘制
                BlockData block = conn.getBlockData();
                block.angle = 0; // 墙壁不旋转
                renderer->DrawBlock(block);
                break;
            }
            case OBJ_GROUND: {
                // 【待确认】决策点5：地面如何绘制？
                // 当前方案：不绘制地面，或者绘制一条线
                // 地面通常是y=0的线，但我们的物理世界可能有不同的地面高度
                break;
            }
            default:
                std::cerr << "警告：未知对象类型，无法绘制" << std::endl;
                break;
        }
    }
    
    // 4. 绘制UI按钮
    // 这里需要调用 allbuttons.h 中的绘制函数
    // drawButtons(...);
    
    // 5. 绘制音乐播放器
    if (musicPlayer) {
        musicPlayer->Draw();
    }
    
    // 6. 绘制调试信息（可选）
    // printDebugInfoOnScreen();
    
    // 7. 结束帧
    renderer->EndFrame();
}

// 处理UI参数变化
void PhysicsVisualAdapter::handleParameterChanges() {
    // 【待确认】决策点2：参数作用范围
    // 当前假设从 allbuttons.h 获取全局参数
    
    // 示例：获取重力参数并设置到物理世界
    // float newGravity = getGrav(); // 假设有这个函数
    // if (fabs(newGravity - uiParams.gravity) > 0.001f) {
    //     setGravity(newGravity);
    // }
    
    // 示例：获取摩擦系数
    // float newFriction = getFric();
    // if (fabs(newFriction - uiParams.friction) > 0.001f) {
    //     setFriction(newFriction);
    // }
}

// 处理按钮点击
void PhysicsVisualAdapter::handleButtonClicks() {
    // 检查 allbuttons.h 中的按钮状态
    
    // 示例：检查开始按钮
    // if (getStartButtonState()) {
    //     startSimulation();
    //     resetButtonStates(); // 重置按钮状态
    // }
    
    // 示例：检查暂停按钮
    // if (getPauseButtonState()) {
    //     pauseSimulation();
    //     resetButtonStates();
    // }
    
    // 示例：检查场景切换按钮
    // if (getScene1ButtonState()) {
    //     switchScene(SCENE_SINGLE_OBJECT);
    //     resetButtonStates();
    // }
}

// 鼠标点击事件
void PhysicsVisualAdapter::onMouseClick(int screenX, int screenY, bool leftButton) {
    if (!leftButton) return;
    
    // 1. 检查是否点击了UI按钮
    // 这里需要调用 allbuttons.h 中的函数检查按钮点击
    // 如果按钮被点击，处理按钮逻辑并返回
    
    // 2. 检查是否点击了音乐播放器
    if (musicPlayer && musicPlayer->HandleMouseInput(screenX, screenY, 0)) {
        return;
    }
    
    // 3. 检查是否点击了物理物体
    int objectId = findObjectAtScreen(screenX, screenY);
    if (objectId != -1) {
        // 开始拖拽物体
        onMouseDragStart(screenX, screenY);
    }
}

// 鼠标拖拽开始
void PhysicsVisualAdapter::onMouseDragStart(int screenX, int screenY) {
    draggedObjectId = findObjectAtScreen(screenX, screenY);
    if (draggedObjectId != -1) {
        isDragging = true;
        
        // 暂停物理模拟以便拖拽
        bool wasPaused = isSimulationPaused;
        isSimulationPaused = true;
        
        std::cout << "开始拖拽物体 ID: " << draggedObjectId << std::endl;
    }
}

// 鼠标拖拽更新
void PhysicsVisualAdapter::onMouseDragUpdate(int screenX, int screenY) {
    if (!isDragging || draggedObjectId == -1) return;
    
    // 查找被拖拽的物体
    auto it = objectConnections.find(draggedObjectId);
    if (it == objectConnections.end()) return;
    
    ObjectConnection& conn = it->second;
    if (!conn.physicsObject) return;
    
    // 将屏幕坐标转换为世界坐标
    double worldX = renderer->ScreenToWorldX(screenX);
    double worldY = renderer->ScreenToWorldY(screenY);
    
    // 更新物体位置
    conn.physicsObject->setCentre(worldX, worldY);
    
    // 拖拽时设置速度为零
    conn.physicsObject->setVelocity(0, 0);
    
    // 更新连接状态
    conn.lastX = worldX;
    conn.lastY = worldY;
    conn.lastVx = 0;
    conn.lastVy = 0;
}

// 鼠标拖拽结束
void PhysicsVisualAdapter::onMouseDragEnd(int screenX, int screenY) {
    if (!isDragging) return;
    
    std::cout << "结束拖拽物体 ID: " << draggedObjectId << std::endl;
    
    // 恢复物理模拟
    isSimulationPaused = false;
    isDragging = false;
    draggedObjectId = -1;
}

// 鼠标移动事件
void PhysicsVisualAdapter::onMouseMove(int screenX, int screenY) {
    // 更新按钮悬停状态
    // 这里需要调用 allbuttons.h 中的函数更新悬停状态
    
    // 可以添加物体高亮效果
}

// 键盘按键事件
void PhysicsVisualAdapter::onKeyPress(char key) {
    switch (key) {
        case ' ':  // 空格键：开始/暂停模拟
            if (isSimulationRunning) {
                if (isSimulationPaused) {
                    pauseSimulation();
                } else {
                    startSimulation();
                }
            } else {
                startSimulation();
            }
            break;
            
        case 'r':  // R键：重置模拟
        case 'R':
            resetSimulation();
            break;
            
        case '1':  // 数字键切换场景
            switchScene(SCENE_SINGLE_OBJECT);
            break;
            
        case '2':
            switchScene(SCENE_TWO_OBJECTS);
            break;
            
        case '3':
            switchScene(SCENE_SPHERE_CREATION);
            break;
            
        case 'm':  // M键：静音/取消静音
        case 'M':
            // 控制音乐播放器
            break;
            
        case 'd':  // D键：显示/隐藏调试信息
        case 'D':
            // 切换调试信息显示
            break;
    }
}

// 切换场景
void PhysicsVisualAdapter::switchScene(SceneMode newScene) {
    if (currentScene == newScene) return;
    
    std::cout << "切换场景: " << currentScene << " -> " << newScene << std::endl;
    
    // 停止当前模拟
    stopSimulation();
    
    // 清理当前场景
    cleanupScene();
    
    // 设置新场景
    currentScene = newScene;
    
    // 初始化新场景
    initializeScene(newScene);
    
    // 重置模拟状态
    resetSimulation();
}

// 初始化场景
void PhysicsVisualAdapter::initializeScene(SceneMode scene) {
    std::cout << "初始化场景: " << scene << std::endl;
    
    // 清除所有现有物体
    objectConnections.clear();
    nextObjectId = 1;
    
    if (physicsWorld) {
        physicsWorld->clearAllShapes();
    }
    
    // 根据场景类型创建物体
    switch (scene) {
        case SCENE_SINGLE_OBJECT:
            // 创建单个球体在斜面上
            createPhysicsObject(OBJ_SLOPE, 0, -5, 20.0, 0.3, 1000.0, RGB(100, 100, 100), false);
            createPhysicsObject(OBJ_CIRCLE, -8, 5, 1.0, 0.0, 1.0, RGB(255, 0, 0), true);
            break;
            
        case SCENE_TWO_OBJECTS:
            // 创建两个碰撞的球体
            createPhysicsObject(OBJ_CIRCLE, -10, 0, 1.0, 0.0, 1.0, RGB(255, 0, 0), true);
            createPhysicsObject(OBJ_CIRCLE, 10, 0, 1.5, 0.0, 2.0, RGB(0, 0, 255), true);
            break;
            
        case SCENE_SPHERE_CREATION:
            // 创建多个球体
            for (int i = 0; i < 5; i++) {
                createPhysicsObject(OBJ_CIRCLE, -20 + i * 8, 10, 1.0, 0.0, 1.0, 
                                   RGB(rand() % 256, rand() % 256, rand() % 256), true);
            }
            break;
            
        case SCENE_TWO_STARS:
            // 创建双星系统
            createPhysicsObject(OBJ_CIRCLE, -15, 0, 3.0, 0.0, 100.0, RGB(255, 255, 0), true);
            createPhysicsObject(OBJ_CIRCLE, 15, 0, 2.0, 0.0, 50.0, RGB(0, 255, 255), true);
            break;
            
        case SCENE_SOLAR_SYS:
            // 简化太阳系
            createPhysicsObject(OBJ_CIRCLE, 0, 0, 5.0, 0.0, 1000.0, RGB(255, 255, 0), false);
            createPhysicsObject(OBJ_CIRCLE, 20, 0, 1.0, 0.0, 1.0, RGB(0, 0, 255), true);
            break;
            
        default:
            std::cout << "未知场景类型" << std::endl;
            break;
    }
}

// 清理场景
void PhysicsVisualAdapter::cleanupScene() {
    std::cout << "清理场景" << std::endl;
    
    // 清除所有物体连接
    objectConnections.clear();
    nextObjectId = 1;
    
    // 清除物理世界中的物体
    if (physicsWorld) {
        physicsWorld->clearAllShapes();
    }
}

// 创建物理物体
int PhysicsVisualAdapter::createPhysicsObject(PhysicsObjectType type,
                                            double x, double y,
                                            double param1, double param2,
                                            double mass,
                                            COLORREF color,
                                            bool isDynamic) {
    if (!physicsWorld) return -1;
    
    // 根据类型创建形状
    Shape* shape = nullptr;
    std::string typeStr;
    
    switch (type) {
        case OBJ_CIRCLE:
            typeStr = "Circle";
            shape = new Circle(mass, param1, x, y);
            break;
            
        case OBJ_AABB:
            typeStr = "AABB";
            shape = new AABB(mass, param1, param2, x, y);
            break;
            
        case OBJ_SLOPE:
            typeStr = "Slope";
            shape = new Slope(mass, param1, param2, x, y);
            break;
            
        case OBJ_WALL:
            typeStr = "Wall";
            shape = new Wall(param1, param2, x, y);
            break;
            
        default:
            std::cerr << "错误：无法创建未知类型的物体" << std::endl;
            return -1;
    }
    
    if (!shape) {
        std::cerr << "错误：创建物体失败" << std::endl;
        return -1;
    }
    
    // 设置物体名称
    std::string name = typeStr + "_" + std::to_string(nextObjectId);
    shape->setName(name);
    
    // 添加到物理世界
    if (isDynamic) {
        physicsWorld->addDynamicShape(shape);
    } else {
        physicsWorld->addStaticShape(shape);
    }
    
    // 创建连接信息
    ObjectConnection conn;
    conn.adapterId = nextObjectId;
    conn.physicsObject = shape;
    conn.type = type;
    conn.color = color;
    
    // 初始化状态
    conn.updateFromPhysics();
    
    // 添加到连接表
    objectConnections[nextObjectId] = conn;
    
    std::cout << "创建物体: ID=" << nextObjectId 
              << ", 类型=" << typeStr 
              << ", 位置=(" << x << "," << y << ")"
              << ", 质量=" << mass << std::endl;
    
    return nextObjectId++;
}

// 查找屏幕位置的物体
int PhysicsVisualAdapter::findObjectAtScreen(int screenX, int screenY) const {
    double worldX = renderer->ScreenToWorldX(screenX);
    double worldY = renderer->ScreenToWorldY(screenY);
    
    // 简单查找：遍历所有物体，检查是否包含该点
    for (const auto& pair : objectConnections) {
        const ObjectConnection& conn = pair.second;
        
        if (!conn.physicsObject) continue;
        
        switch (conn.type) {
            case OBJ_CIRCLE: {
                double dx = worldX - conn.lastX;
                double dy = worldY - conn.lastY;
                double distance = sqrt(dx * dx + dy * dy);
                
                if (distance <= conn.radius) {
                    return pair.first;
                }
                break;
            }
                
            case OBJ_AABB: {
                double halfWidth = conn.width / 2;
                double halfHeight = conn.height / 2;
                
                if (worldX >= conn.lastX - halfWidth &&
                    worldX <= conn.lastX + halfWidth &&
                    worldY >= conn.lastY - halfHeight &&
                    worldY <= conn.lastY + halfHeight) {
                    return pair.first;
                }
                break;
            }
                
            default:
                // 其他类型暂不支持点击检测
                break;
        }
    }
    
    return -1;
}

// 开始模拟
void PhysicsVisualAdapter::startSimulation() {
    std::cout << "开始物理模拟" << std::endl;
    isSimulationRunning = true;
    isSimulationPaused = false;
    
    if (physicsWorld) {
        physicsWorld->start();  // 如果PhysicalWorld有start方法
    }
}

// 暂停模拟
void PhysicsVisualAdapter::pauseSimulation() {
    std::cout << "暂停物理模拟" << std::endl;
    isSimulationPaused = !isSimulationPaused;
    
    if (physicsWorld) {
        if (isSimulationPaused) {
            physicsWorld->Pause();
        } else {
            physicsWorld->Continue();
        }
    }
}

// 停止模拟
void PhysicsVisualAdapter::stopSimulation() {
    std::cout << "停止物理模拟" << std::endl;
    isSimulationRunning = false;
    isSimulationPaused = false;
    
    if (physicsWorld) {
        physicsWorld->Stop();  // 如果PhysicalWorld有stop方法
    }
}

// 重置模拟
void PhysicsVisualAdapter::resetSimulation() {
    std::cout << "重置物理模拟" << std::endl;
    
    // 停止当前模拟
    stopSimulation();
    
    // 重新初始化当前场景
    initializeScene(currentScene);
}

// 设置重力
void PhysicsVisualAdapter::setGravity(float gravity) {
    uiParams.gravity = gravity;
    
    if (physicsWorld) {
        physicsWorld->setGravity(gravity);
        std::cout << "设置重力: " << gravity << " m/s²" << std::endl;
    }
}

// 设置摩擦系数
void PhysicsVisualAdapter::setFriction(float friction) {
    uiParams.friction = friction;
    
    // 【待确认】决策点2：摩擦系数是全局还是针对单个物体？
    // 当前方案：应用到所有物体
    for (auto& pair : objectConnections) {
        ObjectConnection& conn = pair.second;
        if (conn.physicsObject) {
            conn.physicsObject->setFraction(friction);
        }
    }
    
    std::cout << "设置摩擦系数: " << friction << std::endl;
}

// 设置时间缩放
void PhysicsVisualAdapter::setTimeScale(float scale) {
    uiParams.timeScale = scale;
    std::cout << "设置时间缩放: " << scale << std::endl;
}

// 打印调试信息
void PhysicsVisualAdapter::printDebugInfo() const {
    std::cout << "=== 适配器调试信息 ===" << std::endl;
    std::cout << "当前场景: " << currentScene << std::endl;
    std::cout << "模拟状态: " << (isSimulationRunning ? "运行中" : "停止") 
              << (isSimulationPaused ? " (暂停)" : "") << std::endl;
    std::cout << "物体数量: " << objectConnections.size() << std::endl;
    std::cout << "重力: " << uiParams.gravity << " m/s²" << std::endl;
    std::cout << "摩擦系数: " << uiParams.friction << std::endl;
    std::cout << "时间缩放: " << uiParams.timeScale << std::endl;
    
    if (physicsWorld) {
        std::cout << "物理世界时间步长: " << physicsWorld->getTimeStep() << " s" << std::endl;
        std::cout << "动态形状数量: " << physicsWorld->getDynamicShapeCount() << std::endl;
        std::cout << "静态形状数量: " << physicsWorld->getStaticShapeCount() << std::endl;
    }
    
    std::cout << "=====================" << std::endl;
}

// 清理所有资源
void PhysicsVisualAdapter::cleanup() {
    std::cout << "清理适配器资源..." << std::endl;
    
    // 清理对象连接
    objectConnections.clear();
    
    // 清理物理世界
    if (physicsWorld) {
        delete physicsWorld;
        physicsWorld = nullptr;
    }
    
    // 清理背景效果
    if (background) {
        delete background;
        background = nullptr;
    }
    
    // 清理音乐播放器
    if (musicPlayer) {
        musicPlayer->Cleanup();
        delete musicPlayer;
        musicPlayer = nullptr;
    }
}