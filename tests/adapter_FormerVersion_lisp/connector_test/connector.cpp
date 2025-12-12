// connector.cpp
#include "connector.h"

PhysicsRenderer::PhysicsRenderer() {
    scaleFactor = 100.0f;
}

RenderData PhysicsRenderer::physicsToRender(const PhysicsData& physics) {
    RenderData render;
    render.screenX = static_cast<int>(physics.x * scaleFactor);
    render.screenY = static_cast<int>(physics.y * scaleFactor);
    render.screenSize = static_cast<int>(physics.radius * scaleFactor);
    render.isVisible = true;
    return render;
}

PhysicsData PhysicsRenderer::inputToPhysics(int mouseX, int mouseY) {
    PhysicsData physics;
    physics.x = mouseX / scaleFactor;
    physics.y = mouseY / scaleFactor;
    return physics;
}