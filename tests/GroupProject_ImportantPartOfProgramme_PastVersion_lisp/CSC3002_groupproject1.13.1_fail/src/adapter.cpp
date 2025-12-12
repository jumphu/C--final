#include "adapter.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <cstring>
#include <iostream>

// ==================== 辅助函数 ====================
COLORREF stringToColorHelper(const std::string& colorStr) {
    if (colorStr == "Red") return RGB(255, 0, 0);
    else if (colorStr == "Blue") return RGB(0, 0, 255);
    else if (colorStr == "Yellow") return RGB(255, 255, 0);
    else if (colorStr == "Orange") return RGB(255, 165, 0);
    else if (colorStr == "Green") return RGB(0, 255, 0);
    else if (colorStr == "Purple") return RGB(128, 0, 128);
    else return RGB(128, 128, 128);
}

// ==================== ObjectConnection 方法实现 ====================
void ObjectConnection::updateFromPhysics() {
    if (!physicsObject) return;
    physicsObject->getCentre(lastX, lastY);
    physicsObject->getVelocity(lastVx, lastVy);
    
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
        case OBJ_WALL: {
            Wall* wall = dynamic_cast<Wall*>(physicsObject);
            if (wall) {
                width = wall->getWidth();
                height = wall->getHeight();
            }
            break;
        }
        default: break;
    }
}

BallData ObjectConnection::getBallData() const {
    BallData ball;
    ball.x = lastX; ball.y = lastY; ball.radius = radius;
    ball.vx = lastVx; ball.vy = lastVy;
    ball.mass = physicsObject ? physicsObject->getMass() : 1.0;
    ball.color = color;
    return ball;
}

BlockData ObjectConnection::getBlockData() const {
    BlockData block;
    block.cx = lastX; block.cy = lastY;
    block.width = width; block.height = height;
    block.angle = lastAngle;
    block.vx = lastVx; block.vy = lastVy;
    block.mass = physicsObject ? physicsObject->getMass() : 1.0;
    block.color = color;
    return block;
}

RampData ObjectConnection::getRampData() const {
    RampData ramp;
    double halfLength = length / 2.0;
    ramp.x1 = lastX - halfLength * cos(slopeAngle);
    ramp.y1 = lastY - halfLength * sin(slopeAngle);
    ramp.x2 = lastX + halfLength * cos(slopeAngle);
    ramp.y2 = lastY + halfLength * sin(slopeAngle);
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

PhysicsVisualAdapter::PhysicsVisualAdapter(Renderer* rendererPtr)
    : renderer(rendererPtr), physicsWorld(nullptr), background(nullptr), musicPlayer(nullptr),
      nextObjectId(1), currentScene(SCENE_NONE),
      isSimulationRunning(false), isSimulationPaused(false),
      draggedObjectId(-1), isDragging(false), lastUpdateTime(0.0), accumulatedTime(0.0) {
}

PhysicsVisualAdapter::~PhysicsVisualAdapter() {
    cleanup();
}

COLORREF PhysicsVisualAdapter::stringToColor(const std::string& colorStr) const {
    return stringToColorHelper(colorStr);
}

bool PhysicsVisualAdapter::initialize(int screenWidth, int screenHeight) {
    std::cout << "[Adapter] Init..." << std::endl;
    
    double worldWidth = screenWidth / renderer->GetScale();
    double worldHeight = screenHeight / renderer->GetScale();
    
    physicsWorld = new PhysicalWorld(
        -worldWidth/2, worldWidth/2,   
        -worldHeight/2, worldHeight/2
    );
    
    background = new DigitalRainBackgroundIntegrated();
    musicPlayer = new MusicPlayer(screenWidth - 80, 80, 60);
    
    physicsWorld->setGravity(uiParams.gravity);
    physicsWorld->setTimeStep(1.0/60.0);

    initCtrlButtons(buttonPos.startBtnX, buttonPos.pauseBtnX, buttonPos.stopBtnX, 
                    buttonPos.btnY, buttonPos.btnW, buttonPos.btnH);
    
    int margin = 20;
    initChooseButtons(buttonPos.chooseBtnX, buttonPos.model1BtnY, buttonPos.model2BtnY, 
                      buttonPos.model3BtnY, buttonPos.chooseBtnW, buttonPos.chooseBtnH, margin);
    
    initSpaceButtons(buttonPos.spaceBtnX, buttonPos.sphereCreationBtnY, 
                     buttonPos.twoStarsBtnY, buttonPos.solarSysBtnY, 
                     buttonPos.spaceBtnW, buttonPos.spaceBtnH, margin);
    
    initBtns(screenWidth, screenHeight);
    initBtns2(screenWidth, screenHeight);
    
    currentScene = SCENE_SINGLE_OBJECT;
    initializeScene(SCENE_SINGLE_OBJECT);
    
    return true;
}

void PhysicsVisualAdapter::updateFrame(float deltaTime) {
    handleParameterChanges();
    handleButtonClicks();
    
    if (isSimulationRunning && !isSimulationPaused && physicsWorld) {
        const double fixedTimeStep = physicsWorld->timeStep;
        accumulatedTime += deltaTime;
        while (accumulatedTime >= fixedTimeStep) {
            physicsWorld->update(physicsWorld->dynamicShapeList, fixedTimeStep, physicsWorld->ground);
            accumulatedTime -= fixedTimeStep;
        }
        
        for (auto& pair : objectConnections) {
            pair.second.updateFromPhysics();
        }
    }
    lastUpdateTime += deltaTime;
}

// 覆盖 src/adapter.cpp 中的 renderFrame 函数

void PhysicsVisualAdapter::renderFrame() {
    if (!renderer) return;
    
    renderer->BeginFrame();
    if (background) background->UpdateAndDraw();
    else renderer->Clear(WHITE);
    
    // 1. 绘制地面线
    double groundY = -9999.0; // 默认很远
    if (physicsWorld) {
        groundY = physicsWorld->ground.getYLevel();
        double worldLeft = renderer->ScreenToWorldX(0);
        double worldRight = renderer->ScreenToWorldX(renderer->GetWidth());
        
        // 只有普通场景才画地面
        if (groundY > -1000.0) { 
            renderer->DrawLine(worldLeft, groundY, worldRight, groundY, BLACK, 3);
        }
    }

    // 2. 绘制所有物体
    for (const auto& pair : objectConnections) {
        const ObjectConnection& conn = pair.second;
        if (!conn.isVisible) continue;
        switch (conn.type) {
            case OBJ_CIRCLE: renderer->DrawBall(conn.getBallData()); break;
            case OBJ_AABB:   renderer->DrawBlock(conn.getBlockData()); break;
            case OBJ_SLOPE:  renderer->DrawRamp(conn.getRampData()); break;
            case OBJ_WALL: {
                BlockData block = conn.getBlockData();
                block.angle = 0; 
                block.color = RGB(80, 80, 80);
                renderer->DrawBlock(block);
                break;
            }
            default: break;
        }
    }

    // =========================================================
    // 3. 【新增】在地面下方显示小球的物理信息
    // =========================================================
    // 仅在有地面的场景显示，且只显示圆形的信息
    if (groundY > -1000.0) {
        for (const auto& pair : objectConnections) {
            const ObjectConnection& conn = pair.second;
            
            // 只显示动态小球的信息
            if (conn.type == OBJ_CIRCLE && conn.physicsObject) {
                // 1. 获取物理数据
                double m = conn.physicsObject->getMass();
                double vx = conn.lastVx;
                double vy = conn.lastVy;
                double speed = sqrt(vx*vx + vy*vy);
                
                // 2. 格式化字符串
                // 显示：质量, 速度大小
                char infoText[100];
                sprintf(infoText, "M:%.1f kg | V:%.1f m/s", m, speed);
                
                // 3. 计算显示位置
                // X: 跟随小球
                int screenX = renderer->WorldToScreenX(conn.lastX);
                // Y: 固定在地面下方 20 像素处 (groundY 是物理坐标，要转成屏幕坐标)
                int screenY = renderer->WorldToScreenY(groundY) + 20;
                
                // 为了防止文字重叠，如果球在地面下(比如Model2初始化时)，稍微错开一点
                // 这里简单处理：直接画
                
                // 4. 调用渲染器绘制文字
                // 为了让文字居中，稍微往左偏一点
                renderer->DrawText(infoText, screenX - 60, screenY, 16);
                
                // 画一条细线连着球和文字，更有科技感
                int ballScreenY = renderer->WorldToScreenY(conn.lastY - conn.radius); // 球底部
                renderer->DrawLine(conn.lastX, conn.lastY - conn.radius, conn.lastX, groundY, RGB(200, 200, 200), 1);
            }
        }
    }

    // 4. 绘制按钮
    drawCtrlButtons(buttonPos.startBtnX, buttonPos.pauseBtnX, buttonPos.stopBtnX,
                    buttonPos.btnY, buttonPos.btnW, buttonPos.btnH);
    
    if (currentScene >= SCENE_SPHERE_CREATION && currentScene <= SCENE_SOLAR_SYS) {
        drawSpaceButtons(buttonPos.spaceBtnX, buttonPos.sphereCreationBtnY,
                         buttonPos.twoStarsBtnY, buttonPos.solarSysBtnY,
                         buttonPos.spaceBtnW, buttonPos.spaceBtnH);
    } else {
        drawChooseButtons(buttonPos.chooseBtnX, buttonPos.model1BtnY, buttonPos.model2BtnY,
                          buttonPos.model3BtnY, buttonPos.chooseBtnW, buttonPos.chooseBtnH);
    }
    
    switch (currentScene) {
        case SCENE_SINGLE_OBJECT: drawBtns(); break;
        case SCENE_TWO_OBJECTS:   drawBtns2(); break;
        default: break;
    }
    
    if (musicPlayer) musicPlayer->Draw();
    renderer->EndFrame();
}

void PhysicsVisualAdapter::handleParameterChanges() {
    switch (currentScene) {
        case SCENE_SINGLE_OBJECT: {
            float newGravity = getGrav();
            float newFriction = getFric();
            if (fabs(newGravity - uiParams.gravity) > 0.001f) {
                setGravity(newGravity);
                uiParams.gravity = newGravity;
            }
            if (fabs(newFriction - uiParams.friction) > 0.001f) {
                setFriction(newFriction);
                uiParams.friction = newFriction;
            }
            
            // 同步颜色和速度
            char* colorStr = getColor();
            float newSpeed = getSpeedVal();
            for (auto& pair : objectConnections) {
                ObjectConnection& conn = pair.second;
                if (conn.type == OBJ_CIRCLE && conn.physicsObject) {
                    conn.color = stringToColor(colorStr);
                    double currentVx, currentVy;
                    conn.physicsObject->getVelocity(currentVx, currentVy);
                    // 仅当差异较大时强制修改X速度，保留Y速度
                    if (fabs(currentVx - newSpeed) > 0.1f) {
                         conn.physicsObject->setVelocity(newSpeed, currentVy);
                    }
                }
            }
            break;
        }
        case SCENE_TWO_OBJECTS: {
            char* c1 = getColor1();
            char* c2 = getColor2();
            float r1 = getRadius1();
            float r2 = getRadius2();
            float m1 = getMass1();
            float m2 = getMass2();
            float s1 = getSpeed1();
            float s2 = getSpeed2();

            int circleCount = 0;
            for (int id = 1; id <= nextObjectId; ++id) {
                auto it = objectConnections.find(id);
                if (it == objectConnections.end()) continue;
                
                ObjectConnection& conn = it->second;
                if (conn.type == OBJ_CIRCLE && conn.physicsObject) {
                    circleCount++;
                    Circle* circle = dynamic_cast<Circle*>(conn.physicsObject);
                    if (!circle) continue;

                    if (circleCount == 1) { // 左球
                        conn.color = stringToColor(c1);
                        circle->radius = (r1 > 0.1 ? r1 : 0.1);
                        circle->setMass(m1 > 0 ? m1 : 1.0);
                        double vx, vy;
                        circle->getVelocity(vx, vy);
                        if (fabs(vx - s1) > 0.1f) circle->setVelocity(s1, vy);
                    }
                    else if (circleCount == 2) { // 右球
                        conn.color = stringToColor(c2);
                        circle->radius = (r2 > 0.1 ? r2 : 0.1);
                        circle->setMass(m2 > 0 ? m2 : 1.0);
                        double vx, vy;
                        circle->getVelocity(vx, vy);
                        if (fabs(vx - s2) > 0.1f) circle->setVelocity(s2, vy);
                    }
                }
            }
            break;
        }
        default: break;
    }
}

void PhysicsVisualAdapter::handleButtonClicks() {
    if (getStartButtonState()) {
        startSimulation();
        resetCtrlButtons();
    }
    if (getPauseButtonState()) {
        pauseSimulation();
        resetCtrlButtons();
    }
    if (getStopButtonState()) {
        stopSimulation();
        resetCtrlButtons();
    }
    
    if (getmodel1ButtonState()) {
        switchScene(SCENE_SINGLE_OBJECT);
        resetChooseButtons();
    }
    if (getmodel2ButtonState()) {
        switchScene(SCENE_TWO_OBJECTS);
        resetChooseButtons();
    }
    if (getmodel3ButtonState()) {
        switchScene(SCENE_SOLAR_SYS);
        resetChooseButtons();
    }
    
    if (getSphereCreationButtonState()) {
        switchScene(SCENE_SPHERE_CREATION);
        resetSpaceButtons();
    }
    if (getTwoStarsButtonState()) {
        switchScene(SCENE_TWO_STARS);
        resetSpaceButtons();
    }
    if (getSolarSysButtonState()) {
        switchScene(SCENE_SOLAR_SYS);
        resetSpaceButtons();
    }
}

void PhysicsVisualAdapter::onMouseClick(int screenX, int screenY, bool leftButton) {
    if (!leftButton) return;
    
    handleCtrlMouse(screenX, screenY, true,
                    buttonPos.startBtnX, buttonPos.pauseBtnX, buttonPos.stopBtnX,
                    buttonPos.btnY, buttonPos.btnW, buttonPos.btnH);
    
    if (currentScene >= SCENE_SPHERE_CREATION && currentScene <= SCENE_SOLAR_SYS) {
        handleSpaceMouse(screenX, screenY, true,
                         buttonPos.spaceBtnX, buttonPos.sphereCreationBtnY,
                         buttonPos.twoStarsBtnY, buttonPos.solarSysBtnY,
                         buttonPos.spaceBtnW, buttonPos.spaceBtnH);
    } else {
        handleChooseMouse(screenX, screenY, true,
                          buttonPos.chooseBtnX, buttonPos.model1BtnY, buttonPos.model2BtnY,
                          buttonPos.model3BtnY, buttonPos.chooseBtnW, buttonPos.chooseBtnH);
    }
    
    switch (currentScene) {
        case SCENE_SINGLE_OBJECT:
            updBtns(screenX, screenY, true); 
            break;
        case SCENE_TWO_OBJECTS:
            updBtns2(screenX, screenY, true);
            break;
        default:
            break;
    }
    
    if (musicPlayer && musicPlayer->HandleMouseInput(screenX, screenY, WM_LBUTTONDOWN)) {
        return;
    }
    
    int objectId = findObjectAtScreen(screenX, screenY);
    if (objectId != -1) onMouseDragStart(screenX, screenY);
}

void PhysicsVisualAdapter::onMouseDragStart(int screenX, int screenY) {
    draggedObjectId = findObjectAtScreen(screenX, screenY);
    if (draggedObjectId != -1) {
        isDragging = true;
        if (physicsWorld && !physicsWorld->getIsPaused()) physicsWorld->Pause();
    }
}

void PhysicsVisualAdapter::onMouseDragUpdate(int screenX, int screenY) {
    if (!isDragging || draggedObjectId == -1) return;
    auto it = objectConnections.find(draggedObjectId);
    if (it == objectConnections.end() || !it->second.physicsObject) return;
    
    double worldX = renderer->ScreenToWorldX(screenX);
    double worldY = renderer->ScreenToWorldY(screenY);
    
    it->second.physicsObject->setCentre(worldX, worldY);
    it->second.physicsObject->setVelocity(0, 0);
    it->second.updateFromPhysics();
}

void PhysicsVisualAdapter::onMouseDragEnd(int screenX, int screenY) {
    if (!isDragging) return;
    if (physicsWorld && physicsWorld->getIsPaused()) physicsWorld->Continue();
    isDragging = false;
    draggedObjectId = -1;
}

void PhysicsVisualAdapter::onMouseMove(int screenX, int screenY) {
    handleCtrlMouse(screenX, screenY, false, 
                    buttonPos.startBtnX, buttonPos.pauseBtnX, buttonPos.stopBtnX,
                    buttonPos.btnY, buttonPos.btnW, buttonPos.btnH);
                    
    if (currentScene >= SCENE_SPHERE_CREATION && currentScene <= SCENE_SOLAR_SYS) {
        handleSpaceMouse(screenX, screenY, false, 
                         buttonPos.spaceBtnX, buttonPos.sphereCreationBtnY,
                         buttonPos.twoStarsBtnY, buttonPos.solarSysBtnY,
                         buttonPos.spaceBtnW, buttonPos.spaceBtnH);
    } else {
        handleChooseMouse(screenX, screenY, false, 
                          buttonPos.chooseBtnX, buttonPos.model1BtnY, buttonPos.model2BtnY,
                          buttonPos.model3BtnY, buttonPos.chooseBtnW, buttonPos.chooseBtnH);
    }
    
    if (musicPlayer) musicPlayer->HandleMouseInput(screenX, screenY, WM_MOUSEMOVE);
}

void PhysicsVisualAdapter::onKeyPress(char key) {
    switch (key) {
        case ' ': 
            if (isSimulationRunning) isSimulationPaused ? pauseSimulation() : pauseSimulation();
            else startSimulation();
            break;
        case 'r': case 'R': resetSimulation(); break;
        case '1': switchScene(SCENE_SINGLE_OBJECT); break;
        case '2': switchScene(SCENE_TWO_OBJECTS); break;
        case '3': switchScene(SCENE_SOLAR_SYS); break;
        case '4': switchScene(SCENE_TWO_STARS); break;
        case '5': switchScene(SCENE_SOLAR_SYS); break;
        case 'd': case 'D': printDebugInfo(); break;
    }
}

void PhysicsVisualAdapter::switchScene(SceneMode newScene) {
    if (currentScene == newScene) return;
    stopSimulation();
    cleanupScene();
    currentScene = newScene;
    initializeScene(newScene);
    resetSimulation();
}

void PhysicsVisualAdapter::initializeScene(SceneMode scene) {
    objectConnections.clear();
    nextObjectId = 1;
    if (physicsWorld) physicsWorld->clearAllShapes();
    
    double screenW = renderer->GetWidth();
    double scale = renderer->GetScale();
    double worldHalfW = (screenW / 2.0) / scale; 
    
    double wallW = 100.0;
    double wallH = 100.0;
    double wallX = worldHalfW + (wallW / 2.0); 
    
    COLORREF wallColor = RGB(80, 80, 80);
    double groundLevel = -15.0; 
    
    if (physicsWorld) {
        if (scene == SCENE_TWO_STARS || scene == SCENE_SOLAR_SYS) {
            physicsWorld->ground.setYLevel(-9999.0);
        } else {
            physicsWorld->ground.setYLevel(groundLevel);
        }
    }

    switch (scene) {
        case SCENE_SINGLE_OBJECT:
            createPhysicsObject(OBJ_WALL, -wallX, 0, wallW, wallH, 0.0, wallColor, false);
            createPhysicsObject(OBJ_WALL, wallX, 0, wallW, wallH, 0.0, wallColor, false);
            createPhysicsObject(OBJ_SLOPE, 0, groundLevel + 5.0, 20.0, 0.3, 1000.0, RGB(100, 100, 100), false);
            createPhysicsObject(OBJ_CIRCLE, -8, groundLevel + 15.0, 1.0, 0.0, 1.0, RGB(255, 0, 0), true);
            break;
            
        case SCENE_TWO_OBJECTS:
            createPhysicsObject(OBJ_WALL, -wallX, 0, wallW, wallH, 0.0, wallColor, false);
            createPhysicsObject(OBJ_WALL, wallX, 0, wallW, wallH, 0.0, wallColor, false);
            createPhysicsObject(OBJ_CIRCLE, -10, groundLevel + 5.0, 1.0, 0.0, 1.0, RGB(255, 0, 0), true);
            createPhysicsObject(OBJ_CIRCLE, 10, groundLevel + 5.0, 1.5, 0.0, 2.0, RGB(0, 0, 255), true);
            break;
            
        case SCENE_SPHERE_CREATION:
            createPhysicsObject(OBJ_WALL, -wallX, 0, wallW, wallH, 0.0, wallColor, false);
            createPhysicsObject(OBJ_WALL, wallX, 0, wallW, wallH, 0.0, wallColor, false);
            for (int i = 0; i < 5; i++) {
                COLORREF randomColor = RGB(rand() % 256, rand() % 256, rand() % 256);
                createPhysicsObject(OBJ_CIRCLE, -15 + i * 8, 15, 1.0, 0.0, 1.0, randomColor, true);
            }
            break;
            
        case SCENE_TWO_STARS:
            createPhysicsObject(OBJ_CIRCLE, -15, 0, 3.0, 0.0, 100.0, RGB(255, 255, 0), true);
            createPhysicsObject(OBJ_CIRCLE, 15, 0, 2.0, 0.0, 50.0, RGB(0, 255, 255), true);
            break;
            
        case SCENE_SOLAR_SYS:
            createPhysicsObject(OBJ_CIRCLE, 0, 0, 5.0, 0.0, 1000.0, RGB(255, 255, 0), false); 
            createPhysicsObject(OBJ_CIRCLE, 20, 0, 1.0, 0.0, 1.0, RGB(0, 0, 255), true); 
            break;
            
        default: break;
    }
    applyUIParametersToPhysics();
}

int PhysicsVisualAdapter::findObjectAtScreen(int screenX, int screenY) const {
    double worldX = renderer->ScreenToWorldX(screenX);
    double worldY = renderer->ScreenToWorldY(screenY);
    
    for (const auto& pair : objectConnections) {
        const ObjectConnection& conn = pair.second;
        if (!conn.physicsObject) continue;
        
        if (conn.type == OBJ_CIRCLE) {
            double dx = worldX - conn.lastX;
            double dy = worldY - conn.lastY;
            if (sqrt(dx*dx + dy*dy) <= conn.radius) return pair.first;
        } 
        else if (conn.type == OBJ_AABB) {
            double hw = conn.width/2;
            double hh = conn.height/2;
            if (worldX >= conn.lastX - hw && worldX <= conn.lastX + hw &&
                worldY >= conn.lastY - hh && worldY <= conn.lastY + hh) return pair.first;
        }
        else if (conn.type == OBJ_SLOPE) {
            double halfLen = conn.length / 2.0;
            if (worldX >= conn.lastX - halfLen && worldX <= conn.lastX + halfLen &&
                worldY >= conn.lastY - 2.0 && worldY <= conn.lastY + 2.0) {
                return pair.first;
            }
        }
    }
    return -1;
}

// 【关键修复】补全了之前漏掉的 3 个函数
void PhysicsVisualAdapter::cleanupScene() {
    for (auto& pair : objectConnections) {
        if (pair.second.physicsObject) {
            delete pair.second.physicsObject;
            pair.second.physicsObject = nullptr;
        }
    }
    objectConnections.clear();
    nextObjectId = 1;
    if (physicsWorld) physicsWorld->clearAllShapes();
}

void PhysicsVisualAdapter::applyUIParametersToPhysics() {
    if (!physicsWorld) return;
    physicsWorld->setGravity(uiParams.gravity);
    for (auto& pair : objectConnections) {
        if (pair.second.physicsObject) pair.second.physicsObject->setFraction(uiParams.friction);
    }
}

int PhysicsVisualAdapter::createPhysicsObject(PhysicsObjectType type, double x, double y,
                                            double param1, double param2, double mass,
                                            COLORREF color, bool isDynamic) {
    if (!physicsWorld) return -1;
    Shape* shape = nullptr;
    
    switch (type) {
        case OBJ_CIRCLE: shape = new Circle(mass, param1, x, y); break;
        case OBJ_AABB:   shape = new AABB(mass, param1, param2, x, y); break;
        case OBJ_SLOPE:  shape = new Slope(mass, param1, param2, x, y); break;
        case OBJ_WALL:   shape = new Wall(param1, param2, x, y); break;
        default: return -1;
    }
    
    if (shape) {
        shape->setFraction(uiParams.friction);
        if (isDynamic) physicsWorld->addDynamicShape(shape);
        else physicsWorld->addStaticShape(shape);
        
        ObjectConnection conn;
        conn.adapterId = nextObjectId;
        conn.physicsObject = shape;
        conn.type = type;
        conn.color = color;
        conn.updateFromPhysics();
        objectConnections[nextObjectId] = conn;
    }
    return nextObjectId++;
}

void PhysicsVisualAdapter::startSimulation() {
    isSimulationRunning = true;
    isSimulationPaused = false;
    if (physicsWorld) physicsWorld->start();
}

void PhysicsVisualAdapter::pauseSimulation() {
    isSimulationPaused = !isSimulationPaused;
    if (physicsWorld) {
        isSimulationPaused ? physicsWorld->Pause() : physicsWorld->Continue();
    }
}

void PhysicsVisualAdapter::stopSimulation() {
    isSimulationRunning = false;
    isSimulationPaused = false;
    if (physicsWorld) physicsWorld->Stop();
}

void PhysicsVisualAdapter::resetSimulation() {
    stopSimulation();
    initializeScene(currentScene);
}

void PhysicsVisualAdapter::setGravity(float gravity) {
    uiParams.gravity = gravity;
    if (physicsWorld) physicsWorld->setGravity(gravity);
}

void PhysicsVisualAdapter::setFriction(float friction) {
    uiParams.friction = friction;
    for (auto& pair : objectConnections) {
        if (pair.second.physicsObject) pair.second.physicsObject->setFraction(friction);
    }
}

void PhysicsVisualAdapter::setTimeScale(float scale) {
    uiParams.timeScale = scale;
}

void PhysicsVisualAdapter::printDebugInfo() const {
    std::cout << "Debug Info: Scene=" << currentScene << ", Objs=" << objectConnections.size() << std::endl;
}

void PhysicsVisualAdapter::cleanup() {
    cleanupScene(); 
    if (physicsWorld) { delete physicsWorld; physicsWorld = nullptr; }
    if (background) { delete background; background = nullptr; }
    if (musicPlayer) { musicPlayer->Cleanup(); delete musicPlayer; musicPlayer = nullptr; }
}