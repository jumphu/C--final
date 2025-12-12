// connector.h
#ifndef CONNECTOR_H
#define CONNECTOR_H

struct PhysicsData {
    float x, y;        // 物理坐标
    float vx, vy;      // 速度
    float radius;      // 半径/大小
};

struct RenderData {
    int screenX, screenY;  // 屏幕坐标
    int screenSize;        // 屏幕上的大小
    bool isVisible;        // 是否可见
};

class PhysicsRenderer {
public:
    PhysicsRenderer();
    RenderData physicsToRender(const PhysicsData& physics);
    PhysicsData inputToPhysics(int mouseX, int mouseY);
    
private:
    float scaleFactor;
};

#endif