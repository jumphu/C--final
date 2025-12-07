// adapter.cpp - 最小可行物理可视化适配器实现（修正版）
#include "adapter.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <cstring>

// ==================== 辅助函数 ====================

// 颜色字符串转换为COLORREF
COLORREF stringToColorHelper(const std::string& colorStr) {
    if (colorStr == "Red") return RGB(255, 0, 0);
    else if (colorStr == "Blue") return RGB(0, 0, 255);
    else if (colorStr == "Yellow") return RGB(255, 255, 0);
    else if (colorStr == "Orange") return RGB(255, 165, 0);
    else if (colorStr == "Green") return RGB(0, 255, 0);
    else if (colorStr == "Purple") return RGB(128, 0, 128);
    else return RGB(128, 128, 128); // 默认灰色
}

// ==================== ObjectConnection 方法实现 ====================

// 从物理对象更新状态
// 说明：从物理引擎读取物体最新状态，缓存到连接结构中
void ObjectConnection::updateFromPhysics() {
    if (!physicsObject) return;
    
    // 获取当前位置和速度
    physicsObject->getCentre(lastX, lastY);
    physicsObject->getVelocity(lastVx, lastVy);
    
    // 根据类型获取特定属性（通过动态类型转换）
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
            // 其他类型不需要特定属性
            break;
    }
}

// 转换为球体可视化数据
BallData ObjectConnection::getBallData() const {
    BallData ball;
    ball.x = lastX;            // 世界坐标x
    ball.y = lastY;            // 世界坐标y
    ball.radius = radius;      // 半径
    ball.vx = lastVx;          // x方向速度
    ball.vy = lastVy;          // y方向速度
    ball.mass = physicsObject ? physicsObject->getMass() : 1.0; // 质量
    ball.color = color;        // 颜色
    return ball;
}

// 转换为方块可视化数据
BlockData ObjectConnection::getBlockData() const {
    BlockData block;
    block.cx = lastX;          // 中心x坐标
    block.cy = lastY;          // 中心y坐标
    block.width = width;       // 宽度
    block.height = height;     // 高度
    block.angle = lastAngle;   // 旋转角度
    block.vx = lastVx;         // x方向速度
    block.vy = lastVy;         // y方向速度
    block.mass = physicsObject ? physicsObject->getMass() : 1.0; // 质量
    block.color = color;       // 颜色
    return block;
}

// 转换为斜坡可视化数据
RampData ObjectConnection::getRampData() const {
    RampData ramp;
    // 斜坡从质心向两边延伸，计算两个端点
    double halfLength = length / 2.0;
    ramp.x1 = lastX - halfLength * cos(slopeAngle); // 端点1 x
    ramp.y1 = lastY - halfLength * sin(slopeAngle); // 端点1 y
    ramp.x2 = lastX + halfLength * cos(slopeAngle); // 端点2 x
    ramp.y2 = lastY + halfLength * sin(slopeAngle); // 端点2 y
    
    // 获取摩擦系数
    if (physicsObject) {
        double frac;
        physicsObject->getFraction(frac);
        ramp.mu = frac;
    } else {
        ramp.mu = 0.1; // 默认摩擦系数
    }
    
    return ramp;
}

// ==================== PhysicsVisualAdapter 方法实现 ====================

// 构造函数：初始化所有成员变量
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
    
    std::cout << "[适配器] PhysicsVisualAdapter 创建" << std::endl;
}

// 析构函数：清理所有资源
PhysicsVisualAdapter::~PhysicsVisualAdapter() {
    cleanup();
    std::cout << "[适配器] PhysicsVisualAdapter 销毁" << std::endl;
}

// 颜色字符串转换为COLORREF
COLORREF PhysicsVisualAdapter::stringToColor(const std::string& colorStr) const {
    return stringToColorHelper(colorStr);
}

// 初始化适配器
bool PhysicsVisualAdapter::initialize(int screenWidth, int screenHeight) {
    std::cout << "[适配器] 初始化适配器..." << std::endl;
    
    // 1. 创建物理世界（根据屏幕尺寸和渲染器缩放比例计算世界边界）
    double worldWidth = screenWidth / renderer->GetScale();
    double worldHeight = screenHeight / renderer->GetScale();
    
    // 创建物理世界，边界设置为屏幕中心对称
    physicsWorld = new PhysicalWorld(
        -worldWidth/2, worldWidth/2,   // left, right
        -worldHeight/2, worldHeight/2  // bottom, top
    );
    
    if (!physicsWorld) {
        std::cerr << "[错误] 无法创建物理世界" << std::endl;
        return false;
    }
    
    // 2. 创建背景效果
    background = new DigitalRainBackgroundIntegrated();
    
    // 3. 创建音乐播放器（放置在右上角）
    musicPlayer = new MusicPlayer(screenWidth - 80, 80, 60);
    
    // 4. 设置初始物理参数
    physicsWorld->setGravity(uiParams.gravity);
    physicsWorld->setTimeStep(1.0/60.0); // 固定时间步长：60FPS
    
    // 5. 初始化UI按钮系统
    // 5.1 初始化控制按钮（开始/暂停/停止）
    initButtons(buttonPos.startBtnX, buttonPos.pauseBtnX, buttonPos.stopBtnX, 
                buttonPos.btnY, buttonPos.btnW, buttonPos.btnH);
    
    // 5.2 初始化场景选择按钮1（Model 1/2/3）
    int margin = 20;
    // 注意：这里调用的是choose.cpp中的initButtons，与threebottums.cpp中的同名函数参数不同
    // 实际项目中可能需要调整函数名称以避免冲突
    // 这里假设我们使用的是choose.cpp的initButtons
    ::initButtons(buttonPos.chooseBtnX, buttonPos.model1BtnY, buttonPos.model2BtnY, 
                  buttonPos.model3BtnY, buttonPos.chooseBtnW, buttonPos.chooseBtnH, margin);
    
    // 5.3 初始化行星体系场景按钮（sphere_creation/two_stars/solar_sys）
    initSceneModelButtons(buttonPos.spaceBtnX, buttonPos.sphereCreationBtnY, 
                          buttonPos.twoStarsBtnY, buttonPos.solarSysBtnY, 
                          buttonPos.spaceBtnW, buttonPos.spaceBtnH, margin);
    
    // 5.4 初始化单物体参数按钮
    initBtns(screenWidth, screenHeight);
    
    // 5.5 初始化双物体参数按钮
    initBtns2(screenWidth, screenHeight);
    
    // 6. 设置初始场景为单物体场景
    currentScene = SCENE_SINGLE_OBJECT;
    initializeScene(SCENE_SINGLE_OBJECT);
    
    std::cout << "[适配器] 适配器初始化完成" << std::endl;
    std::cout << "[适配器] 世界边界: " << -worldWidth/2 << ", " << worldWidth/2 << ", "
              << -worldHeight/2 << ", " << worldHeight/2 << std::endl;
    
    return true;
}

// 主更新函数（每帧调用）
void PhysicsVisualAdapter::updateFrame(float deltaTime) {
    // 1. 处理UI参数变化（从按钮系统读取参数并同步到物理引擎）
    handleParameterChanges();
    
    // 2. 处理按钮点击事件
    handleButtonClicks();
    
    // 3. 如果模拟正在运行且未暂停，更新物理世界
    if (isSimulationRunning && !isSimulationPaused) {
        // 使用固定时间步长进行稳定的物理模拟
        const double fixedTimeStep = physicsWorld->timeStep;  // 修正：直接访问timeStep成员
        
        // 累积时间
        accumulatedTime += deltaTime;
        
        // 执行固定时间步长更新
        while (accumulatedTime >= fixedTimeStep) {
            // 更新物理世界
            if (physicsWorld) {
                // 调用物理世界的update方法，传入动态形状列表和时间步长
                physicsWorld->update(physicsWorld->dynamicShapeList, 
                                    fixedTimeStep, 
                                    physicsWorld->ground);
            }
            
            // 减少累积时间
            accumulatedTime -= fixedTimeStep;
        }
        
        // 4. 同步所有物理对象到可视化系统
        for (auto& pair : objectConnections) {
            int adapterId = pair.first;
            ObjectConnection& conn = pair.second;
            
            // 更新物理状态缓存
            conn.updateFromPhysics();
        }
    }
    
    // 5. 更新时间戳（用于调试和性能监控）
    lastUpdateTime += deltaTime;
}

// 渲染函数（每帧调用）
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
        
        // 跳过不可见物体
        if (!conn.isVisible) continue;
        
        // 根据物体类型调用相应的绘制函数
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
                // 墙壁用矩形绘制，不旋转
                BlockData block = conn.getBlockData();
                block.angle = 0; // 墙壁不旋转
                renderer->DrawBlock(block);
                break;
            }
            default:
                std::cerr << "[警告] 未知对象类型，无法绘制" << std::endl;
                break;
        }
    }
    
    // 4. 绘制UI按钮
    // 4.1 绘制控制按钮（始终显示）
    drawButtons(buttonPos.startBtnX, buttonPos.pauseBtnX, buttonPos.stopBtnX,
                buttonPos.btnY, buttonPos.btnW, buttonPos.btnH);
    
    // 4.2 绘制场景选择按钮（根据当前场景类型显示不同的按钮组）
    if (currentScene >= SCENE_SPHERE_CREATION && currentScene <= SCENE_SOLAR_SYS) {
        // 显示行星体系场景按钮
        drawSceneModelButtons(buttonPos.spaceBtnX, buttonPos.sphereCreationBtnY,
                              buttonPos.twoStarsBtnY, buttonPos.solarSysBtnY,
                              buttonPos.spaceBtnW, buttonPos.spaceBtnH);
    } else {
        // 显示普通场景选择按钮（Model 1/2/3）
        drawButtons(buttonPos.chooseBtnX, buttonPos.model1BtnY, buttonPos.model2BtnY,
                    buttonPos.model3BtnY, buttonPos.chooseBtnW, buttonPos.chooseBtnH);
    }
    
    // 4.3 绘制场景参数按钮（根据当前场景显示对应的参数调节按钮）
    switch (currentScene) {
        case SCENE_SINGLE_OBJECT:
            drawBtns();  // 绘制单物体场景的四个参数按钮
            break;
        case SCENE_TWO_OBJECTS:
            drawBtns2(); // 绘制双物体场景的八个参数按钮
            break;
        default:
            // 其他场景可能没有参数按钮
            break;
    }
    
    // 5. 绘制音乐播放器
    if (musicPlayer) {
        musicPlayer->Draw();
    }
    
    // 6. 结束帧
    renderer->EndFrame();
}

// 处理UI参数变化
void PhysicsVisualAdapter::handleParameterChanges() {
    // 根据当前场景同步不同的参数
    switch (currentScene) {
        case SCENE_SINGLE_OBJECT: {
            // 获取单物体场景的参数
            float newGravity = getGrav();
            float newFriction = getFric();
            
            // 检查重力参数是否变化
            if (fabs(newGravity - uiParams.gravity) > 0.001f) {
                setGravity(newGravity);
                uiParams.gravity = newGravity;
                std::cout << "[参数] 重力设置为: " << newGravity << " m/s²" << std::endl;
            }
            
            // 检查摩擦系数是否变化
            if (fabs(newFriction - uiParams.friction) > 0.001f) {
                setFriction(newFriction);
                uiParams.friction = newFriction;
                std::cout << "[参数] 摩擦系数设置为: " << newFriction << std::endl;
            }
            
            // 获取速度参数（可用于设置物体初始速度）
            float newSpeed = getSpeedVal();
            // 这里可以根据需要更新物体的初始速度
            
            break;
        }
            
        case SCENE_TWO_OBJECTS: {
            // 获取双物体场景的参数
            char* color1Str = getColor1();
            char* color2Str = getColor2();
            float radius1 = getRadius1();
            float radius2 = getRadius2();
            float mass1 = getMass1();
            float mass2 = getMass2();
            float speed1 = getSpeed1();
            float speed2 = getSpeed2();
            
            // 如果有两个物体，更新它们的属性
            if (objectConnections.size() >= 2) {
                auto it = objectConnections.begin();
                ObjectConnection& obj1 = it->second;
                ++it;
                ObjectConnection& obj2 = it->second;
                
                // 更新颜色
                obj1.color = stringToColor(color1Str);
                obj2.color = stringToColor(color2Str);
                
                // 注意：半径和质量在创建后通常不能直接修改
                // 如果需要修改，可能需要重新创建物体
                
                std::cout << "[参数] 双物体参数更新: " 
                          << "物体1颜色=" << color1Str << ", "
                          << "物体2颜色=" << color2Str << std::endl;
            }
            
            break;
        }
            
        default:
            // 其他场景可能不需要参数同步
            break;
    }
    
    // 更新参数按钮状态
    switch (currentScene) {
        case SCENE_SINGLE_OBJECT:
            updBtns();  // 更新单物体场景按钮
            break;
        case SCENE_TWO_OBJECTS:
            updBtns2(); // 更新双物体场景按钮
            break;
        default:
            break;
    }
}

// 处理按钮点击事件
void PhysicsVisualAdapter::handleButtonClicks() {
    // 1. 检查控制按钮状态
    if (getStartButtonState()) {
        startSimulation();
        resetButtonStates();  // 重置按钮点击状态
        std::cout << "[按钮] 开始按钮被点击" << std::endl;
    }
    
    if (getPauseButtonState()) {
        pauseSimulation();
        resetButtonStates();
        std::cout << "[按钮] 暂停按钮被点击" << std::endl;
    }
    
    if (getStopButtonState()) {
        stopSimulation();
        resetButtonStates();
        std::cout << "[按钮] 停止按钮被点击" << std::endl;
    }
    
    // 2. 检查场景选择按钮1（Model 1/2/3）
    if (getmodel1ButtonState()) {
        switchScene(SCENE_SINGLE_OBJECT);
        resetButtonStates();
        std::cout << "[按钮] Model 1按钮被点击（单物体场景）" << std::endl;
    }
    
    if (getmodel2ButtonState()) {
        switchScene(SCENE_TWO_OBJECTS);
        resetButtonStates();
        std::cout << "[按钮] Model 2按钮被点击（双物体场景）" << std::endl;
    }
    
    if (getmodel3ButtonState()) {
        switchScene(SCENE_PENDULUM);
        resetButtonStates();
        std::cout << "[按钮] Model 3按钮被点击（钟摆场景）" << std::endl;
    }
    
    // 3. 检查行星体系场景按钮
    if (getSphereCreationButtonState()) {
        switchScene(SCENE_SPHERE_CREATION);
        resetButtonStates();
        std::cout << "[按钮] Sphere Creation按钮被点击" << std::endl;
    }
    
    if (getTwoStarsButtonState()) {
        switchScene(SCENE_TWO_STARS);
        resetButtonStates();
        std::cout << "[按钮] Two Stars按钮被点击" << std::endl;
    }
    
    if (getSolarSysButtonState()) {
        switchScene(SCENE_SOLAR_SYS);
        resetButtonStates();
        std::cout << "[按钮] Solar System按钮被点击" << std::endl;
    }
}

// 鼠标点击事件处理
void PhysicsVisualAdapter::onMouseClick(int screenX, int screenY, bool leftButton) {
    if (!leftButton) return;
    
    // 1. 处理控制按钮点击
    handleMouseInput(buttonPos.startBtnX, buttonPos.pauseBtnX, buttonPos.stopBtnX,
                     buttonPos.btnY, buttonPos.btnW, buttonPos.btnH);
    
    // 2. 处理场景选择按钮点击（根据当前场景选择正确的按钮组）
    if (currentScene >= SCENE_SPHERE_CREATION && currentScene <= SCENE_SOLAR_SYS) {
        handleSceneModelMouseInput(buttonPos.spaceBtnX, buttonPos.sphereCreationBtnY,
                                   buttonPos.twoStarsBtnY, buttonPos.solarSysBtnY,
                                   buttonPos.spaceBtnW, buttonPos.spaceBtnH);
    } else {
        handleMouseInput(buttonPos.chooseBtnX, buttonPos.model1BtnY, buttonPos.model2BtnY,
                         buttonPos.model3BtnY, buttonPos.chooseBtnW, buttonPos.chooseBtnH);
    }
    
    // 3. 检查音乐播放器点击
    if (musicPlayer && musicPlayer->HandleMouseInput(screenX, screenY, 0)) {
        std::cout << "[鼠标] 音乐播放器被点击" << std::endl;
        return;
    }
    
    // 4. 最后检查物理物体点击（用于拖拽）
    int objectId = findObjectAtScreen(screenX, screenY);
    if (objectId != -1) {
        onMouseDragStart(screenX, screenY);
    }
}

// 鼠标拖拽开始
void PhysicsVisualAdapter::onMouseDragStart(int screenX, int screenY) {
    draggedObjectId = findObjectAtScreen(screenX, screenY);
    if (draggedObjectId != -1) {
        isDragging = true;
        
        // 暂停物理模拟以便拖拽（避免物理引擎干扰拖拽操作）
        if (physicsWorld && !physicsWorld->getIsPaused()) {
            physicsWorld->Pause();  // 保存当前状态并暂停
            std::cout << "[拖拽] 物理模拟已暂停以便拖拽" << std::endl;
        }
        
        std::cout << "[拖拽] 开始拖拽物体 ID: " << draggedObjectId << std::endl;
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
    
    // 拖拽时设置速度为零（避免释放后物体飞出去）
    conn.physicsObject->setVelocity(0, 0);
    
    // 更新连接状态缓存
    conn.lastX = worldX;
    conn.lastY = worldY;
    conn.lastVx = 0;
    conn.lastVy = 0;
    
    // 可以添加视觉反馈，例如高亮被拖拽的物体
}

// 鼠标拖拽结束
void PhysicsVisualAdapter::onMouseDragEnd(int screenX, int screenY) {
    if (!isDragging) return;
    
    std::cout << "[拖拽] 结束拖拽物体 ID: " << draggedObjectId << std::endl;
    
    // 恢复物理模拟
    if (physicsWorld && physicsWorld->getIsPaused()) {
        physicsWorld->Continue();  // 恢复之前状态并继续模拟
        std::cout << "[拖拽] 物理模拟已恢复" << std::endl;
    }
    
    isDragging = false;
    draggedObjectId = -1;
}

// 鼠标移动事件（用于按钮悬停效果）
void PhysicsVisualAdapter::onMouseMove(int screenX, int screenY) {
    // 更新控制按钮悬停状态
    handleMouseInput(buttonPos.startBtnX, buttonPos.pauseBtnX, buttonPos.stopBtnX,
                     buttonPos.btnY, buttonPos.btnW, buttonPos.btnH);
    
    // 更新场景选择按钮悬停状态
    if (currentScene >= SCENE_SPHERE_CREATION && currentScene <= SCENE_SOLAR_SYS) {
        handleSceneModelMouseInput(buttonPos.spaceBtnX, buttonPos.sphereCreationBtnY,
                                   buttonPos.twoStarsBtnY, buttonPos.solarSysBtnY,
                                   buttonPos.spaceBtnW, buttonPos.spaceBtnH);
    } else {
        handleMouseInput(buttonPos.chooseBtnX, buttonPos.model1BtnY, buttonPos.model2BtnY,
                         buttonPos.model3BtnY, buttonPos.chooseBtnW, buttonPos.chooseBtnH);
    }
}

// 键盘按键事件处理
void PhysicsVisualAdapter::onKeyPress(char key) {
    switch (key) {
        case ' ':  // 空格键：开始/暂停切换
            if (isSimulationRunning) {
                if (isSimulationPaused) {
                    pauseSimulation(); // 如果已暂停，则继续
                } else {
                    pauseSimulation(); // 如果正在运行，则暂停
                }
            } else {
                startSimulation(); // 如果未运行，则开始
            }
            std::cout << "[键盘] 空格键: " 
                      << (isSimulationRunning ? (isSimulationPaused ? "继续模拟" : "暂停模拟") : "开始模拟") 
                      << std::endl;
            break;
            
        case 'r':  // R键：重置模拟
        case 'R':
            resetSimulation();
            std::cout << "[键盘] R键: 重置模拟" << std::endl;
            break;
            
        case '1':  // 数字键1：切换到单物体场景
            switchScene(SCENE_SINGLE_OBJECT);
            std::cout << "[键盘] 1键: 切换到单物体场景" << std::endl;
            break;
            
        case '2':  // 数字键2：切换到双物体场景
            switchScene(SCENE_TWO_OBJECTS);
            std::cout << "[键盘] 2键: 切换到双物体场景" << std::endl;
            break;
            
        case '3':  // 数字键3：切换到球体生成场景
            switchScene(SCENE_SPHERE_CREATION);
            std::cout << "[键盘] 3键: 切换到球体生成场景" << std::endl;
            break;
            
        case '4':  // 数字键4：切换到双星系统
            switchScene(SCENE_TWO_STARS);
            std::cout << "[键盘] 4键: 切换到双星系统" << std::endl;
            break;
            
        case '5':  // 数字键5：切换到太阳系
            switchScene(SCENE_SOLAR_SYS);
            std::cout << "[键盘] 5键: 切换到太阳系" << std::endl;
            break;
            
        case 'm':  // M键：音乐开关
        case 'M':
            // 这里可以控制音乐播放器
            // 由于MusicPlayer的具体实现未知，这里只打印日志
            std::cout << "[键盘] M键: 切换音乐播放状态" << std::endl;
            break;
            
        case 'd':  // D键：显示/隐藏调试信息
        case 'D':
            printDebugInfo();
            std::cout << "[键盘] D键: 显示调试信息" << std::endl;
            break;
            
        case 27:   // ESC键：退出程序（在主循环中处理）
            std::cout << "[键盘] ESC键: 退出程序" << std::endl;
            break;
    }
}

// 切换场景
void PhysicsVisualAdapter::switchScene(SceneMode newScene) {
    // 如果已经是当前场景，不需要切换
    if (currentScene == newScene) {
        std::cout << "[场景] 已经是当前场景，无需切换" << std::endl;
        return;
    }
    
    std::cout << "[场景] 切换场景: " << currentScene << " -> " << newScene << std::endl;
    
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
    
    std::cout << "[场景] 场景切换完成" << std::endl;
}

// 初始化场景
void PhysicsVisualAdapter::initializeScene(SceneMode scene) {
    std::cout << "[场景] 初始化场景: " << scene << std::endl;
    
    // 清除所有现有物体
    objectConnections.clear();
    nextObjectId = 1;
    
    if (physicsWorld) {
        physicsWorld->clearAllShapes();
    }
    
    // 根据场景类型创建物体
    switch (scene) {
        case SCENE_SINGLE_OBJECT:
            // 创建单物体场景：斜坡上的球
            // 斜坡（静态物体）
            createPhysicsObject(OBJ_SLOPE, 0, -5, 20.0, 0.3, 1000.0, RGB(100, 100, 100), false);
            // 球（动态物体）
            createPhysicsObject(OBJ_CIRCLE, -8, 5, 1.0, 0.0, 1.0, RGB(255, 0, 0), true);
            std::cout << "[场景] 单物体场景初始化完成" << std::endl;
            break;
            
        case SCENE_TWO_OBJECTS:
            // 创建双物体场景：两个碰撞的球
            createPhysicsObject(OBJ_CIRCLE, -10, 0, 1.0, 0.0, 1.0, RGB(255, 0, 0), true);
            createPhysicsObject(OBJ_CIRCLE, 10, 0, 1.5, 0.0, 2.0, RGB(0, 0, 255), true);
            std::cout << "[场景] 双物体场景初始化完成" << std::endl;
            break;
            
        case SCENE_SPHERE_CREATION:
            // 创建球体生成场景：多个不同颜色的球
            for (int i = 0; i < 5; i++) {
                COLORREF randomColor = RGB(rand() % 256, rand() % 256, rand() % 256);
                createPhysicsObject(OBJ_CIRCLE, -20 + i * 8, 10, 1.0, 0.0, 1.0, randomColor, true);
            }
            std::cout << "[场景] 球体生成场景初始化完成" << std::endl;
            break;
            
        case SCENE_TWO_STARS:
            // 创建双星系统：两个大质量球体
            createPhysicsObject(OBJ_CIRCLE, -15, 0, 3.0, 0.0, 100.0, RGB(255, 255, 0), true);
            createPhysicsObject(OBJ_CIRCLE, 15, 0, 2.0, 0.0, 50.0, RGB(0, 255, 255), true);
            std::cout << "[场景] 双星系统场景初始化完成" << std::endl;
            break;
            
        case SCENE_SOLAR_SYS:
            // 创建简化太阳系：太阳和行星
            createPhysicsObject(OBJ_CIRCLE, 0, 0, 5.0, 0.0, 1000.0, RGB(255, 255, 0), false); // 太阳（静态）
            createPhysicsObject(OBJ_CIRCLE, 20, 0, 1.0, 0.0, 1.0, RGB(0, 0, 255), true);      // 行星
            std::cout << "[场景] 太阳系场景初始化完成" << std::endl;
            break;
            
        case SCENE_PENDULUM:
            // 创建钟摆场景（待实现）
            std::cout << "[场景] 钟摆场景（待实现）" << std::endl;
            break;
            
        default:
            std::cout << "[警告] 未知场景类型，跳过初始化" << std::endl;
            break;
    }
    
    // 应用UI参数到新场景
    applyUIParametersToPhysics();
}

// 清理场景
void PhysicsVisualAdapter::cleanupScene() {
    std::cout << "[场景] 清理场景" << std::endl;
    
    // 清除所有物体连接
    objectConnections.clear();
    nextObjectId = 1;
    
    // 清除物理世界中的物体
    if (physicsWorld) {
        physicsWorld->clearAllShapes();
    }
}

// 应用UI参数到物理世界
void PhysicsVisualAdapter::applyUIParametersToPhysics() {
    if (!physicsWorld) return;
    
    // 应用重力
    physicsWorld->setGravity(uiParams.gravity);
    
    // 应用摩擦系数到所有动态物体
    for (auto& pair : objectConnections) {
        ObjectConnection& conn = pair.second;
        if (conn.physicsObject) {
            conn.physicsObject->setFraction(uiParams.friction);
        }
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
    
    // 根据类型创建相应的形状对象
    Shape* shape = nullptr;
    std::string typeStr;
    
    switch (type) {
        case OBJ_CIRCLE:
            typeStr = "Circle";
            shape = new Circle(mass, param1, x, y);  // param1 = 半径
            break;
            
        case OBJ_AABB:
            typeStr = "AABB";
            shape = new AABB(mass, param1, param2, x, y);  // param1 = 宽度, param2 = 高度
            break;
            
        case OBJ_SLOPE:
            typeStr = "Slope";
            shape = new Slope(mass, param1, param2, x, y);  // param1 = 长度, param2 = 角度
            break;
            
        case OBJ_WALL:
            typeStr = "Wall";
            shape = new Wall(param1, param2, x, y);  // param1 = 宽度, param2 = 高度
            break;
            
        default:
            std::cerr << "[错误] 无法创建未知类型的物体" << std::endl;
            return -1;
    }
    
    if (!shape) {
        std::cerr << "[错误] 创建物体失败" << std::endl;
        return -1;
    }
    
    // 设置物体名称（用于调试和查找）
    std::string name = typeStr + "_" + std::to_string(nextObjectId);
    shape->setName(name);
    
    // 设置摩擦系数
    shape->setFraction(uiParams.friction);
    
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
    
    // 初始化状态（从物理对象读取）
    conn.updateFromPhysics();
    
    // 添加到连接表
    objectConnections[nextObjectId] = conn;
    
    std::cout << "[物体] 创建物体: ID=" << nextObjectId 
              << ", 类型=" << typeStr 
              << ", 位置=(" << x << ", " << y << ")"
              << ", 质量=" << mass << std::endl;
    
    // 返回适配器ID，并递增下一个可用ID
    return nextObjectId++;
}

// 查找屏幕位置的物体
int PhysicsVisualAdapter::findObjectAtScreen(int screenX, int screenY) const {
    // 将屏幕坐标转换为世界坐标
    double worldX = renderer->ScreenToWorldX(screenX);
    double worldY = renderer->ScreenToWorldY(screenY);
    
    // 遍历所有物体，检查是否包含该点
    for (const auto& pair : objectConnections) {
        const ObjectConnection& conn = pair.second;
        
        if (!conn.physicsObject) continue;
        
        // 根据物体类型进行不同的碰撞检测
        switch (conn.type) {
            case OBJ_CIRCLE: {
                // 圆形：检查点到圆心的距离是否小于半径
                double dx = worldX - conn.lastX;
                double dy = worldY - conn.lastY;
                double distance = sqrt(dx * dx + dy * dy);
                
                if (distance <= conn.radius) {
                    return pair.first;
                }
                break;
            }
                
            case OBJ_AABB: {
                // 矩形：检查点是否在矩形边界内
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
    
    // 未找到物体
    return -1;
}

// 开始模拟
void PhysicsVisualAdapter::startSimulation() {
    std::cout << "[模拟] 开始物理模拟" << std::endl;
    isSimulationRunning = true;
    isSimulationPaused = false;
    
    if (physicsWorld) {
        physicsWorld->start();  // 启动物理世界
    }
}

// 暂停模拟
void PhysicsVisualAdapter::pauseSimulation() {
    // 切换暂停状态
    isSimulationPaused = !isSimulationPaused;
    
    if (physicsWorld) {
        if (isSimulationPaused) {
            physicsWorld->Pause();    // 暂停物理模拟
            std::cout << "[模拟] 暂停物理模拟" << std::endl;
        } else {
            physicsWorld->Continue(); // 继续物理模拟
            std::cout << "[模拟] 继续物理模拟" << std::endl;
        }
    }
}

// 停止模拟
void PhysicsVisualAdapter::stopSimulation() {
    std::cout << "[模拟] 停止物理模拟" << std::endl;
    isSimulationRunning = false;
    isSimulationPaused = false;
    
    if (physicsWorld) {
        physicsWorld->Stop();  // 停止物理世界
    }
}

// 重置模拟
void PhysicsVisualAdapter::resetSimulation() {
    std::cout << "[模拟] 重置物理模拟" << std::endl;
    
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
        std::cout << "[参数] 设置重力: " << gravity << " m/s²" << std::endl;
    }
}

// 设置摩擦系数
void PhysicsVisualAdapter::setFriction(float friction) {
    uiParams.friction = friction;
    
    // 应用到所有物体
    for (auto& pair : objectConnections) {
        ObjectConnection& conn = pair.second;
        if (conn.physicsObject) {
            conn.physicsObject->setFraction(friction);
        }
    }
    
    std::cout << "[参数] 设置摩擦系数: " << friction << std::endl;
}

// 设置时间缩放
void PhysicsVisualAdapter::setTimeScale(float scale) {
    uiParams.timeScale = scale;
    std::cout << "[参数] 设置时间缩放: " << scale << std::endl;
}

// 打印调试信息
void PhysicsVisualAdapter::printDebugInfo() const {
    std::cout << "========== 适配器调试信息 ==========" << std::endl;
    std::cout << "当前场景: " << currentScene << std::endl;
    std::cout << "模拟状态: " << (isSimulationRunning ? "运行中" : "停止") 
              << (isSimulationPaused ? " (暂停)" : "") << std::endl;
    std::cout << "物体数量: " << objectConnections.size() << std::endl;
    std::cout << "重力: " << uiParams.gravity << " m/s²" << std::endl;
    std::cout << "摩擦系数: " << uiParams.friction << std::endl;
    std::cout << "时间缩放: " << uiParams.timeScale << std::endl;
    std::cout << "拖拽状态: " << (isDragging ? "拖拽中" : "未拖拽") << std::endl;
    
    if (physicsWorld) {
        std::cout << "物理世界时间步长: " << physicsWorld->timeStep << " s" << std::endl;
        std::cout << "动态形状数量: " << physicsWorld->getDynamicShapeCount() << std::endl;
        std::cout << "静态形状数量: " << physicsWorld->getStaticShapeCount() << std::endl;
        std::cout << "暂停状态: " << (physicsWorld->getIsPaused() ? "是" : "否") << std::endl;
    }
    
    // 打印所有物体信息
    for (const auto& pair : objectConnections) {
        const ObjectConnection& conn = pair.second;
        std::cout << "  物体 ID" << conn.adapterId 
                  << ": 位置(" << conn.lastX << ", " << conn.lastY << ")"
                  << ", 速度(" << conn.lastVx << ", " << conn.lastVy << ")"
                  << ", 类型=" << conn.type << std::endl;
    }
    
    std::cout << "=====================================" << std::endl;
}

// 清理所有资源
void PhysicsVisualAdapter::cleanup() {
    std::cout << "[适配器] 清理适配器资源..." << std::endl;
    
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
    
    std::cout << "[适配器] 资源清理完成" << std::endl;
}
