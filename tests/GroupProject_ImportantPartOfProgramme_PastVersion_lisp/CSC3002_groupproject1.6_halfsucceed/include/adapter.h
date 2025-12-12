#ifndef ADAPTER_H
#define ADAPTER_H

#include "Renderer.h"
// 这里的名字必须和硬盘上的文件一致
#include "allbottums.h" 
#include "background_integrated.h"
#include "music.h"
#include "shapes.h"
#include "physicalWorld.h"

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

// 场景模式枚举
enum SceneMode {
    SCENE_SINGLE_OBJECT,
    SCENE_TWO_OBJECTS,
    SCENE_SPHERE_CREATION,
    SCENE_TWO_STARS,
    SCENE_SOLAR_SYS,
    SCENE_PENDULUM,
    SCENE_NONE
};

// 物理对象类型
enum PhysicsObjectType {
    OBJ_CIRCLE,
    OBJ_AABB,
    OBJ_SLOPE,
    OBJ_GROUND,
    OBJ_WALL,
    OBJ_PENDULUM,
    OBJ_GENERIC
};

// 对象连接信息结构
struct ObjectConnection {
    int adapterId;
    Shape* physicsObject;
    PhysicsObjectType type;
    
    double lastX, lastY;
    double lastVx, lastVy;
    double lastAngle;
    
    double radius;
    double width, height;
    double length;
    double slopeAngle;
    
    COLORREF color;
    bool isVisible;
    
    ObjectConnection() 
        : adapterId(-1), physicsObject(nullptr), type(OBJ_GENERIC),
          lastX(0), lastY(0), lastVx(0), lastVy(0), lastAngle(0),
          radius(0), width(0), height(0), length(0), slopeAngle(0),
          color(RGB(0, 0, 0)), isVisible(true) {}
    
    void updateFromPhysics();
    BallData getBallData() const;
    BlockData getBlockData() const;
    RampData getRampData() const;
};

// 主适配器类
class PhysicsVisualAdapter {
private:
    Renderer* renderer;
    PhysicalWorld* physicsWorld;
    DigitalRainBackgroundIntegrated* background;
    MusicPlayer* musicPlayer;
    
    std::unordered_map<int, ObjectConnection> objectConnections;
    int nextObjectId;
    
    SceneMode currentScene;
    bool isSimulationRunning;
    bool isSimulationPaused;
    
    // 【修复关键点 1】 定义参数结构体
    struct UIParameters {
        float gravity = 9.8f;
        float friction = 0.1f;
        float timeScale = 1.0f;
    };
    UIParameters uiParams; // 实例化变量
    
    int draggedObjectId;
    bool isDragging;
    double lastUpdateTime;
    double accumulatedTime;
    
    // 【修复关键点 2】 定义按钮位置结构体
    struct ButtonPositions {
        int startBtnX, pauseBtnX, stopBtnX, btnY, btnW, btnH;
        int chooseBtnX, model1BtnY, model2BtnY, model3BtnY, chooseBtnW, chooseBtnH;
        int spaceBtnX, sphereCreationBtnY, twoStarsBtnY, solarSysBtnY, spaceBtnW, spaceBtnH;
    };
    ButtonPositions buttonPos; // 实例化变量
    
    // 内部辅助方法
    void handleParameterChanges();
    void handleButtonClicks();
    void initializeScene(SceneMode scene);
    void cleanupScene();
    int createPhysicsObject(PhysicsObjectType type, double x, double y,
                           double param1, double param2, double mass,
                           COLORREF color, bool isDynamic = true);
    int findObjectAtScreen(int screenX, int screenY) const;
    COLORREF stringToColor(const std::string& colorStr) const;
    void applyUIParametersToPhysics();
    
public:
    PhysicsVisualAdapter(Renderer* rendererPtr);
    ~PhysicsVisualAdapter();
    
    bool initialize(int screenWidth, int screenHeight);
    void updateFrame(float deltaTime);
    void renderFrame();
    
    void onMouseClick(int screenX, int screenY, bool leftButton = true);
    void onMouseDragStart(int screenX, int screenY);
    void onMouseDragUpdate(int screenX, int screenY);
    void onMouseDragEnd(int screenX, int screenY);
    void onMouseMove(int screenX, int screenY);
    void onKeyPress(char key);
    
    void switchScene(SceneMode newScene);
    void startSimulation();
    void pauseSimulation();
    void stopSimulation();
    void resetSimulation();
    
    void setGravity(float gravity);
    void setFriction(float friction);
    void setTimeScale(float scale);
    
    SceneMode getCurrentScene() const { return currentScene; }
    bool isRunning() const { return isSimulationRunning; }
    bool getIsDragging() const { return isDragging; }
    void printDebugInfo() const;
    void cleanup();
};

#endif