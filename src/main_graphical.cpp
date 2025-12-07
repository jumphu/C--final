// main_graphical.cpp
#pragma execution_character_set("utf-8")
#include <graphics.h>
#include <iostream>
#include <memory>
#include "adapter.h"

int main() {
    const int WIN_WIDTH = 1280;
    const int WIN_HEIGHT = 800;
    const float SCALE = 50.0f;
    
    Renderer renderer(WIN_WIDTH, WIN_HEIGHT, SCALE);
    PhysicsVisualAdapter adapter(&renderer);
    
    if (!adapter.initialize(WIN_WIDTH, WIN_HEIGHT)) {
        std::cerr << "Init adapter failed" << std::endl;
        return 1;
    }
    
    adapter.switchScene(SCENE_SPHERE_CREATION);
    adapter.startSimulation();
    
    DWORD lastTime = GetTickCount();
    const int TARGET_FPS = 60;
    const int FRAME_DELAY = 1000 / TARGET_FPS;
    
    bool running = true;
    bool isLeftMouseDown = false;
    
    while (running) {
        DWORD currentTime = GetTickCount();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        
        ExMessage msg;
        while (peekmessage(&msg, EM_MOUSE | EM_KEY)) {
            if (msg.message == WM_MOUSEMOVE) {
                adapter.onMouseMove(msg.x, msg.y);
                if (isLeftMouseDown) {
                    adapter.onMouseDragUpdate(msg.x, msg.y);
                }
            }
            else if (msg.message == WM_LBUTTONDOWN) {
                isLeftMouseDown = true;
                adapter.onMouseClick(msg.x, msg.y, true);
            }
            else if (msg.message == WM_LBUTTONUP) {
                isLeftMouseDown = false;
                adapter.onMouseDragEnd(msg.x, msg.y);
            }
            else if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_ESCAPE) {
                    running = false;
                }
                char key = 0;
                if (msg.vkcode >= '0' && msg.vkcode <= '9') key = msg.vkcode;
                else if (msg.vkcode >= 'A' && msg.vkcode <= 'Z') key = msg.vkcode;
                else if (msg.vkcode == VK_SPACE) key = ' ';
                
                if (key != 0) {
                    adapter.onKeyPress(key);
                }
            }
        }
        
        adapter.updateFrame(deltaTime);
        adapter.renderFrame();
        
        DWORD frameTime = GetTickCount() - currentTime;
        if (frameTime < FRAME_DELAY) {
            Sleep(FRAME_DELAY - frameTime);
        }
    }
    
    adapter.cleanup();
    closegraph();
    
    // Force terminate process to ensure all threads (e.g. MCI audio) are closed
    ExitProcess(0);
    
    return 0;
}