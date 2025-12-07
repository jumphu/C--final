// adapter.cpp - Implementation of PhysicsVisualAdapter
#include "adapter.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>

// ==================== ObjectConnection Methods ====================

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

// ==================== PhysicsVisualAdapter Methods ====================

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
}

PhysicsVisualAdapter::~PhysicsVisualAdapter() {
    cleanup();
}

bool PhysicsVisualAdapter::initialize(int screenWidth, int screenHeight) {
    double worldWidth = screenWidth / renderer->GetScale();
    double worldHeight = screenHeight / renderer->GetScale();
    
    physicsWorld = new PhysicalWorld(
        -worldWidth/2, worldWidth/2,
        -worldHeight/2, worldHeight/2
    );
    
    if (!physicsWorld) {
        return false;
    }
    
    background = new DigitalRainBackgroundIntegrated();
    
    musicPlayer = new MusicPlayer(screenWidth - 80, 80, 60);
    
    physicsWorld->setGravity(uiParams.gravity);
    physicsWorld->setTimeStep(1.0/60.0); 
    
    // Initialize buttons and store layout
    int startX, pauseX, stopX, btnY, btnW, btnH;
    initButtons(startX, pauseX, stopX, btnY, btnW, btnH);
    btnLayout.startX = startX;
    btnLayout.pauseX = pauseX;
    btnLayout.stopX = stopX;
    btnLayout.btnY = btnY;
    btnLayout.btnW = btnW;
    btnLayout.btnH = btnH;
    
    int sceneBtnX, s1Y, s2Y, s3Y, sW, sH;
    initSceneModelButtons(sceneBtnX, s1Y, s2Y, s3Y, sW, sH, 20); 
    btnLayout.sceneBtnX = sceneBtnX;
    btnLayout.s1Y = s1Y;
    btnLayout.s2Y = s2Y;
    btnLayout.s3Y = s3Y;
    btnLayout.sW = sW;
    btnLayout.sH = sH;
    
    return true;
}

void PhysicsVisualAdapter::updateFrame(float deltaTime) {
    handleParameterChanges();
    handleButtonClicks();
    
    if (isSimulationRunning && !isSimulationPaused) {
        accumulatedTime += deltaTime;
        const double fixedTimeStep = physicsWorld->getTimeStep();
        
        while (accumulatedTime >= fixedTimeStep) {
            if (physicsWorld) {
                physicsWorld->update(physicsWorld->dynamicShapeList, 
                                    fixedTimeStep, 
                                    physicsWorld->ground);
            }
            accumulatedTime -= fixedTimeStep;
        }
        
        for (auto& pair : objectConnections) {
            ObjectConnection& conn = pair.second;
            conn.updateFromPhysics();
        }
    }
    
    if (background) {
        // background update
    }
    
    lastUpdateTime += deltaTime;
}

void PhysicsVisualAdapter::renderFrame() {
    if (!renderer) return;
    
    renderer->BeginFrame();
    
    if (background) {
        background->UpdateAndDraw();
    } else {
        renderer->Clear(WHITE);
    }
    
    // Draw Ground Line
    if (physicsWorld) {
        double groundY = physicsWorld->ground.getYLevel();
        int p1y = renderer->WorldToScreenY(groundY);
        int p2y = renderer->WorldToScreenY(groundY);
        setlinecolor(BLACK);
        setlinestyle(PS_SOLID, 2);
        line(0, p1y, renderer->GetWidth(), p2y);
    }

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
                BlockData block = conn.getBlockData();
                block.angle = 0;
                renderer->DrawBlock(block);
                break;
            }
            default:
                break;
        }
    }
    
    // Draw UI using stored layout
    drawButtons(btnLayout.startX, btnLayout.pauseX, btnLayout.stopX, btnLayout.btnY, btnLayout.btnW, btnLayout.btnH);
    drawSceneModelButtons(btnLayout.sceneBtnX, btnLayout.s1Y, btnLayout.s2Y, btnLayout.s3Y, btnLayout.sW, btnLayout.sH);

    if (musicPlayer) {
        musicPlayer->Draw();
    }
    
    renderer->EndFrame();
}

void PhysicsVisualAdapter::handleParameterChanges() {
    // Stub
}

void PhysicsVisualAdapter::handleButtonClicks() {
    // Check global button states
    if (getStartButtonState()) {
        startSimulation();
        resetButtonStates();
    }
    if (getPauseButtonState()) {
        pauseSimulation();
        resetButtonStates();
    }
    if (getStopButtonState()) {
        stopSimulation();
        resetSimulation(); // Stop usually means reset in this context
        resetButtonStates();
    }
    
    // Scene buttons
    if (getSphereCreationButtonState()) {
        switchScene(SCENE_SPHERE_CREATION);
        // Reset state handled by helper in spacebottum.cpp if we called checkSceneModelButtonStates
        // But here we access flags directly via getter. 
        // We need to manually reset them or call the check function.
        // spacebottum.cpp's checkSceneModelButtonStates does logic we can't easily hook into without calling it.
        // But we are reading flags directly. So we should reset them.
        // wait, sphere_creationClicked is extern bool.
        extern bool sphere_creationClicked;
        sphere_creationClicked = false;
    }
    if (getTwoStarsButtonState()) {
        switchScene(SCENE_TWO_OBJECTS);
        extern bool two_starsClicked;
        two_starsClicked = false;
    }
    if (getSolarSysButtonState()) {
        switchScene(SCENE_SOLAR_SYS);
        extern bool solar_sysClicked;
        solar_sysClicked = false;
    }
}

void PhysicsVisualAdapter::onMouseClick(int screenX, int screenY, bool leftButton) {
    if (!leftButton) return;
    
    // Check Music Player
    if (musicPlayer && musicPlayer->HandleMouseInput(screenX, screenY, WM_LBUTTONDOWN)) {
        return;
    }
    
    // Check UI Buttons - Start/Pause/Stop
    if (isInButton(screenX, screenY, btnLayout.startX, btnLayout.btnY, btnLayout.btnW, btnLayout.btnH)) {
        extern bool isStartClicked; isStartClicked = true; return;
    }
    if (isInButton(screenX, screenY, btnLayout.pauseX, btnLayout.btnY, btnLayout.btnW, btnLayout.btnH)) {
        extern bool isPauseClicked; isPauseClicked = true; return;
    }
    if (isInButton(screenX, screenY, btnLayout.stopX, btnLayout.btnY, btnLayout.btnW, btnLayout.btnH)) {
        extern bool isStopClicked; isStopClicked = true; return;
    }
    
    // Check UI Buttons - Scenes
    if (isInButton(screenX, screenY, btnLayout.sceneBtnX, btnLayout.s1Y, btnLayout.sW, btnLayout.sH)) {
        extern bool sphere_creationClicked; sphere_creationClicked = true; return;
    }
    if (isInButton(screenX, screenY, btnLayout.sceneBtnX, btnLayout.s2Y, btnLayout.sW, btnLayout.sH)) {
        extern bool two_starsClicked; two_starsClicked = true; return;
    }
    if (isInButton(screenX, screenY, btnLayout.sceneBtnX, btnLayout.s3Y, btnLayout.sW, btnLayout.sH)) {
        extern bool solar_sysClicked; solar_sysClicked = true; return;
    }
    
    // Check Objects
    int objectId = findObjectAtScreen(screenX, screenY);
    if (objectId != -1) {
        onMouseDragStart(screenX, screenY);
    }
}

void PhysicsVisualAdapter::onMouseDragStart(int screenX, int screenY) {
    draggedObjectId = findObjectAtScreen(screenX, screenY);
    if (draggedObjectId != -1) {
        isDragging = true;
        // Optional: pause on drag
    }
}

void PhysicsVisualAdapter::onMouseDragUpdate(int screenX, int screenY) {
    if (!isDragging || draggedObjectId == -1) return;
    
    auto it = objectConnections.find(draggedObjectId);
    if (it == objectConnections.end()) return;
    
    ObjectConnection& conn = it->second;
    if (!conn.physicsObject) return;
    
    double worldX = renderer->ScreenToWorldX(screenX);
    double worldY = renderer->ScreenToWorldY(screenY);
    
    conn.physicsObject->setCentre(worldX, worldY);
    conn.physicsObject->setVelocity(0, 0);
    
    conn.lastX = worldX;
    conn.lastY = worldY;
    conn.lastVx = 0;
    conn.lastVy = 0;
}

void PhysicsVisualAdapter::onMouseDragEnd(int screenX, int screenY) {
    isDragging = false;
    draggedObjectId = -1;
}

void PhysicsVisualAdapter::onMouseMove(int screenX, int screenY) {
    if (musicPlayer) {
        musicPlayer->HandleMouseInput(screenX, screenY, WM_MOUSEMOVE);
    }
    
    // Update Hover States
    extern bool isStartHovered, isPauseHovered, isStopHovered;
    isStartHovered = isInButton(screenX, screenY, btnLayout.startX, btnLayout.btnY, btnLayout.btnW, btnLayout.btnH);
    isPauseHovered = isInButton(screenX, screenY, btnLayout.pauseX, btnLayout.btnY, btnLayout.btnW, btnLayout.btnH);
    isStopHovered = isInButton(screenX, screenY, btnLayout.stopX, btnLayout.btnY, btnLayout.btnW, btnLayout.btnH);
    
    extern bool sphere_creationHovered, two_starsHovered, solar_sysHovered;
    sphere_creationHovered = isInButton(screenX, screenY, btnLayout.sceneBtnX, btnLayout.s1Y, btnLayout.sW, btnLayout.sH);
    two_starsHovered = isInButton(screenX, screenY, btnLayout.sceneBtnX, btnLayout.s2Y, btnLayout.sW, btnLayout.sH);
    solar_sysHovered = isInButton(screenX, screenY, btnLayout.sceneBtnX, btnLayout.s3Y, btnLayout.sW, btnLayout.sH);
}

void PhysicsVisualAdapter::onKeyPress(char key) {
    switch (key) {
        case ' ': 
            if (isSimulationRunning) {
                if (isSimulationPaused) pauseSimulation();
                else startSimulation();
            } else {
                startSimulation();
            }
            break;
        case 'r':
        case 'R':
            resetSimulation();
            break;
        case '1': switchScene(SCENE_SINGLE_OBJECT); break;
        case '2': switchScene(SCENE_TWO_OBJECTS); break;
        case '3': switchScene(SCENE_SPHERE_CREATION); break;
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
    
    if (physicsWorld) {
        physicsWorld->clearAllShapes();
    }
    
    switch (scene) {
        case SCENE_SINGLE_OBJECT:
            // Slope and ball
            createPhysicsObject(OBJ_SLOPE, 0, -5, 20.0, 0.3, 1000.0, RGB(100, 100, 100), false);
            createPhysicsObject(OBJ_CIRCLE, -8, 5, 1.0, 0.0, 1.0, RGB(255, 0, 0), true);
            break;
            
        case SCENE_TWO_OBJECTS:
            // Two balls colliding
            {
                int id1 = createPhysicsObject(OBJ_CIRCLE, -10, 5, 1.0, 0.0, 1.0, RGB(255, 0, 0), true);
                int id2 = createPhysicsObject(OBJ_CIRCLE, 10, 5, 1.5, 0.0, 2.0, RGB(0, 0, 255), true);
                // Add initial velocity manually if needed
                Shape* s1 = objectConnections[id1].physicsObject;
                if(s1) s1->setVelocity(5, 0);
                Shape* s2 = objectConnections[id2].physicsObject;
                if(s2) s2->setVelocity(-5, 0);
            }
            break;
            
        case SCENE_SPHERE_CREATION:
            // Multiple balls
            for (int i = 0; i < 5; i++) {
                createPhysicsObject(OBJ_CIRCLE, -10 + i * 5, 10 + i*2, 1.0, 0.0, 1.0, 
                                   RGB(rand() % 255, rand() % 255, rand() % 255), true);
            }
            break;
            
        case SCENE_SOLAR_SYS:
             // Solar system demo
             // Sun
             createPhysicsObject(OBJ_CIRCLE, 0, 0, 3.0, 0.0, 10000.0, RGB(255, 200, 0), false); // Static sun? Or super heavy dynamic?
             // Earth-like
             {
                 int id = createPhysicsObject(OBJ_CIRCLE, 15, 0, 1.0, 0.0, 10.0, RGB(0, 100, 255), true);
                 Shape* s = objectConnections[id].physicsObject;
                 if(s) s->setVelocity(0, 25); // Orbit velocity
             }
             break;

        default:
            break;
    }
}

void PhysicsVisualAdapter::cleanupScene() {
    objectConnections.clear();
    nextObjectId = 1;
    if (physicsWorld) {
        physicsWorld->clearAllShapes();
    }
}

int PhysicsVisualAdapter::createPhysicsObject(PhysicsObjectType type,
                                            double x, double y,
                                            double param1, double param2,
                                            double mass,
                                            COLORREF color,
                                            bool isDynamic) {
    if (!physicsWorld) return -1;
    
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
            return -1;
    }
    
    if (!shape) return -1;
    
    std::string name = typeStr + "_" + std::to_string(nextObjectId);
    shape->setName(name);
    
    if (isDynamic) {
        physicsWorld->addDynamicShape(shape);
    } else {
        physicsWorld->addStaticShape(shape);
    }
    
    ObjectConnection conn;
    conn.adapterId = nextObjectId;
    conn.physicsObject = shape;
    conn.type = type;
    conn.color = color;
    conn.updateFromPhysics();
    
    objectConnections[nextObjectId] = conn;
    
    return nextObjectId++;
}

int PhysicsVisualAdapter::findObjectAtScreen(int screenX, int screenY) const {
    double worldX = renderer->ScreenToWorldX(screenX);
    double worldY = renderer->ScreenToWorldY(screenY);
    
    for (const auto& pair : objectConnections) {
        const ObjectConnection& conn = pair.second;
        if (!conn.physicsObject) continue;
        
        switch (conn.type) {
            case OBJ_CIRCLE: {
                double dx = worldX - conn.lastX;
                double dy = worldY - conn.lastY;
                if ((dx*dx + dy*dy) <= (conn.radius * conn.radius)) return pair.first;
                break;
            }
            case OBJ_AABB: {
                if (worldX >= conn.lastX - conn.width/2 && worldX <= conn.lastX + conn.width/2 &&
                    worldY >= conn.lastY - conn.height/2 && worldY <= conn.lastY + conn.height/2)
                    return pair.first;
                break;
            }
            default: break;
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
        if (isSimulationPaused) physicsWorld->Pause();
        else physicsWorld->Continue();
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
    // Stub
}

void PhysicsVisualAdapter::cleanup() {
    objectConnections.clear();
    if (physicsWorld) { delete physicsWorld; physicsWorld = nullptr; }
    if (background) { delete background; background = nullptr; }
    if (musicPlayer) { musicPlayer->Cleanup(); delete musicPlayer; musicPlayer = nullptr; }
}