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
        // 【关键修复】补上了墙壁的数据同步！之前漏了这个，导致墙壁宽高为0看不见
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

void PhysicsVisualAdapter::renderFrame() {
    if (!renderer) return;
    
    renderer->BeginFrame();
    if (background) background->UpdateAndDraw();
    else renderer->Clear(WHITE);
    
    // 绘制地面线
    if (physicsWorld) {
        double groundY = physicsWorld->ground.getYLevel();
        double worldLeft = renderer->ScreenToWorldX(0);
        double worldRight = renderer->ScreenToWorldX(renderer->GetWidth());
        renderer->DrawLine(worldLeft, groundY, worldRight, groundY, BLACK, 3);
    }

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
                // 墙壁用深灰色显示
                block.color = RGB(80, 80, 80);
                renderer->DrawBlock(block);
                break;
            }
            default: break;
        }
    }
    
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
            break;
        }
        case SCENE_TWO_OBJECTS: {
            char* color1Str = getColor1();
            char* color2Str = getColor2();
            if (objectConnections.size() >= 2) {
                auto it = objectConnections.begin();
                it->second.color = stringToColor(color1Str);
                (++it)->second.color = stringToColor(color2Str);
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

// 动态计算墙壁位置
// 覆盖 src/adapter.cpp 中的 initializeScene 函数

void PhysicsVisualAdapter::initializeScene(SceneMode scene) {
    objectConnections.clear();
    nextObjectId = 1;
    if (physicsWorld) physicsWorld->clearAllShapes();
    
    // 1. 计算墙壁位置 (左右边缘)
    double screenW = renderer->GetWidth();
    double scale = renderer->GetScale();
    double worldHalfW = (screenW / 2.0) / scale; 
    double wallX = worldHalfW - 1.0; // 墙壁位置
    double wallW = 2.0;
    double wallH = 100.0;
    COLORREF wallColor = RGB(80, 80, 80);

    // 2. 【关键修改】设定地面高度
    // 屏幕高度一半是 400px (假设800高)，除以scale(20) = 20米。
    // 我们把地面设在 y = -18 米处，这就很接近屏幕底部了。
    double groundLevel = -15.0; 
    
    if (physicsWorld) {
        // 对于太空场景，把地面藏到很远的地方；对于普通场景，设到底部
        if (scene == SCENE_TWO_STARS || scene == SCENE_SOLAR_SYS) {
            physicsWorld->ground.setYLevel(-9999.0); // 藏起来
        } else {
            physicsWorld->ground.setYLevel(groundLevel); // 设到底部
        }
    }

    switch (scene) {
        case SCENE_SINGLE_OBJECT:
            // 加墙
            createPhysicsObject(OBJ_WALL, -wallX, 0, wallW, wallH, 0.0, wallColor, false);
            createPhysicsObject(OBJ_WALL, wallX, 0, wallW, wallH, 0.0, wallColor, false);
            
            // 【修改】斜坡位置：放在地面上方一点点
            // y = groundLevel + 5 (即 -10)，这样就在地面上了
            createPhysicsObject(OBJ_SLOPE, 0, groundLevel + 5.0, 20.0, 0.3, 1000.0, RGB(100, 100, 100), false);
            
            // 【修改】小球位置：放在斜坡上方
            createPhysicsObject(OBJ_CIRCLE, -8, groundLevel + 15.0, 1.0, 0.0, 1.0, RGB(255, 0, 0), true);
            break;
            
        case SCENE_TWO_OBJECTS:
            createPhysicsObject(OBJ_WALL, -wallX, 0, wallW, wallH, 0.0, wallColor, false);
            createPhysicsObject(OBJ_WALL, wallX, 0, wallW, wallH, 0.0, wallColor, false);
            
            // 【修改】小球位置：放在地面上方
            createPhysicsObject(OBJ_CIRCLE, -10, groundLevel + 5.0, 1.0, 0.0, 1.0, RGB(255, 0, 0), true);
            createPhysicsObject(OBJ_CIRCLE, 10, groundLevel + 5.0, 1.5, 0.0, 2.0, RGB(0, 0, 255), true);
            break;
            
        case SCENE_SPHERE_CREATION:
            createPhysicsObject(OBJ_WALL, -wallX, 0, wallW, wallH, 0.0, wallColor, false);
            createPhysicsObject(OBJ_WALL, wallX, 0, wallW, wallH, 0.0, wallColor, false);
            
            // 小球从天花板掉下来 (y = 15)
            for (int i = 0; i < 5; i++) {
                COLORREF randomColor = RGB(rand() % 256, rand() % 256, rand() % 256);
                createPhysicsObject(OBJ_CIRCLE, -15 + i * 8, 15, 1.0, 0.0, 1.0, randomColor, true);
            }
            break;
            
        case SCENE_TWO_STARS:
            // 太空场景，物体悬浮在中心 (y=0)
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
        } else if (conn.type == OBJ_AABB) {
            double hw = conn.width/2;
            double hh = conn.height/2;
            if (worldX >= conn.lastX - hw && worldX <= conn.lastX + hw &&
                worldY >= conn.lastY - hh && worldY <= conn.lastY + hh) return pair.first;
        }
    }
    return -1;
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