#include <graphics.h>
#include <iostream>
#include <vector>
#include <memory>

#include "Renderer.h"
#include "PhysicsRenderBridge.h"
#include "allbottums.h"

extern std::unique_ptr<IPhysicalWorldAdapter> createDefaultAdapter();

int main() {
    // 1. Initialize Renderer (Window creation is inside Renderer ctor)
    // 1200x800 window, 50 pixels per meter
    Renderer renderer(1200, 800, 50.0); 

    // 2. Initialize Adapter & Bridge
    auto adapter = createDefaultAdapter();
    PhysicsRenderBridge bridge(std::move(adapter), &renderer);
    
    // Match Renderer's coordinate system: Origin (0,0) at Bottom-Left (0, 800)
    bridge.initialize(50.0, {0, 800}); 

    // 3. Initialize Buttons
    int btnX, startY, pauseY, stopY, btnW, btnH;
    // Control Buttons (Start/Pause/Stop) from control_buttons.cpp
    initButtons(btnX, startY, pauseY, stopY, btnW, btnH, 10);
    
    // Parameter Buttons (Color, Gravity...) from fourBottums1.cpp
    initBtns(1200, 800);

    // 4. Main Loop
    const double dt = 1.0/60.0;
    
    while (true) {
        // --- Input ---
        handleMouseInput(btnX, startY, pauseY, stopY, btnW, btnH);
        updBtns(); // Update parameter buttons
        
        checkButtonStates(); 

        // Handle Mouse Clicks for creating shapes
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            POINT p;
            GetCursorPos(&p);
            ScreenToClient(GetHWnd(), &p);
            
            bool onButtons = (p.x > 1200 - 250); 
            if (!onButtons) {
                 // Create logic here if needed
            }
        }
        
        // Debug: Space to create random ball
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
             std::vector<UserInput> inputs;
             UserInput in;
             in.type = UserInput::CREATE_SHAPE;
             in.shape_type = "Circle";
             POINT p; GetCursorPos(&p); ScreenToClient(GetHWnd(), &p);
             in.mouse_position = p;
             inputs.push_back(in);
             bridge.syncInputToPhysics(inputs);
             Sleep(200); 
        }

        // --- Logic ---
        bridge.updateButtonStates();
        
        // Physics Step
        if (!bridge.isPaused()) {
             bridge.update(dt);
        }

        // --- Render ---
        renderer.BeginFrame(); // Clears device
        
        // Sync & Draw World
        bridge.syncPhysicsToRender();
        const auto& objs = bridge.getRenderObjects();
        
        // Draw Ground
        renderer.DrawText("Ground Level (y=0)", 10, 780);
        setlinecolor(BLACK);
        line(0, 800, 1200, 800);
        
        for (const auto& pair : objs) {
            const RenderObject& obj = pair.second;
            if (obj.type == RenderObject::BALL) {
                BallData bd;
                bd.x = obj.ball.x;
                bd.y = obj.ball.y;
                bd.radius = obj.ball.radius;
                bd.vx = obj.ball.vx;
                bd.vy = obj.ball.vy;
                bd.mass = obj.ball.mass;
                bd.color = obj.color;
                renderer.DrawBall(bd);
            }
            else if (obj.type == RenderObject::BLOCK) {
                 BlockData bd;
                 bd.cx = obj.block.cx;
                 bd.cy = obj.block.cy;
                 bd.width = obj.block.width;
                 bd.height = obj.block.height;
                 bd.angle = obj.block.angle;
                 bd.mass = obj.block.mass;
                 bd.vx = obj.block.vx;
                 bd.vy = obj.block.vy;
                 bd.color = obj.color;
                 renderer.DrawBlock(bd);
            }
        }

        // Draw Buttons
        drawButtons(btnX, startY, pauseY, stopY, btnW, btnH);
        drawBtns(); // Draw parameter buttons

        renderer.EndFrame();
    }
    
    return 0;
}
