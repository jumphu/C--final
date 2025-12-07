#include <graphics.h>
#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include "Renderer.h"
#include "PhysicsRenderBridge.h"
#include "allbottums.h"

extern std::unique_ptr<IPhysicalWorldAdapter> createDefaultAdapter();

enum SceneType {
    MENU = 0,
    SCENE_BALL_COLLISION = 1,
    SCENE_BLOCK_WALL = 2,
    SCENE_STACKING = 3,
    SCENE_SLOPE = 4
};

SceneType currentScene = MENU;

void setupScene(SceneType scene, PhysicsRenderBridge& bridge) {
    bridge.clearWorld();
    bridge.setInclineAngle(0.0); // Reset slope by default

    if (scene == SCENE_BALL_COLLISION) {
        // Two balls colliding
        // Left ball moving right
        bridge.createBall(4.0, 4.0, 0.5, 1.0, "Red");
        // Right ball moving left (velocity is not exposed in createBall, so they start static?)
        // Wait, createBall sets velocity to 0. 
        // We need a way to set velocity or create with velocity.
        // The current Adapter createBall doesn't take velocity.
        // But we can find the shape and set velocity, or add createBallWithVelocity.
        // For now, let's just place them. They will fall if gravity is on.
        // To make them collide horizontally, we need initial velocity.
        // But let's stick to the current API capabilities or gravity.
        bridge.createBall(8.0, 4.0, 0.5, 1.0, "Blue");
        // We can use Space to spawn more.
        
    } else if (scene == SCENE_BLOCK_WALL) {
        // Block falling on Wall? Or Block sliding?
        // Wall
        bridge.createWall(10.0, 2.0, 1.0, 4.0, 0.5); // x=10, y=2, w=1, h=4
        // Block
        bridge.createBlock(2.0, 2.0, 1.0, 1.0, 1.0, "Blue"); // x=2, y=2
        
    } else if (scene == SCENE_STACKING) {
        // Block on Block
        bridge.createBlock(6.0, 1.0, 2.0, 1.0, 10.0, "Green"); // Base block
        bridge.createBlock(6.0, 3.0, 1.0, 1.0, 1.0, "Blue");   // Top block
        bridge.createBlock(6.0, 5.0, 0.8, 0.8, 0.5, "Red");    // Top-top block
        
    } else if (scene == SCENE_SLOPE) {
        // Slope Scene
        // Set incline angle in World
        bridge.setInclineAngle(30.0); // 30 degrees
        
        // Place ball
        bridge.createBall(4.0, 8.0, 0.5, 1.0, "Red");
    }
}

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
    initButtons(btnX, startY, pauseY, stopY, btnW, btnH, 10);
    initBtns(1200, 800);

    // 4. Main Loop
    const double dt = 1.0/60.0;
    
    while (true) {
        // --- Input ---
        handleMouseInput(btnX, startY, pauseY, stopY, btnW, btnH);
        updBtns(); 
        checkButtonStates(); 
        
        // Scene Switching
        if (GetAsyncKeyState('1') & 0x8000) {
            currentScene = SCENE_BALL_COLLISION;
            setupScene(currentScene, bridge);
            Sleep(200);
        } else if (GetAsyncKeyState('2') & 0x8000) {
            currentScene = SCENE_BLOCK_WALL;
            setupScene(currentScene, bridge);
            Sleep(200);
        } else if (GetAsyncKeyState('3') & 0x8000) {
            currentScene = SCENE_STACKING;
            setupScene(currentScene, bridge);
            Sleep(200);
        } else if (GetAsyncKeyState('4') & 0x8000) {
            currentScene = SCENE_SLOPE;
            setupScene(currentScene, bridge);
            Sleep(200);
        } else if (GetAsyncKeyState('M') & 0x8000) {
            currentScene = MENU;
            bridge.clearWorld();
            Sleep(200);
        }
        
        // Mouse Creation (only in scenes)
        if (currentScene != MENU && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            POINT p;
            GetCursorPos(&p);
            ScreenToClient(GetHWnd(), &p);
            bool onButtons = (p.x > 1200 - 250) || (p.y > 600); // Approximate button area check
            if (!onButtons) {
                 // Logic to create shape at mouse position could go here
            }
        }

        // --- Logic ---
        bridge.updateButtonStates();
        
        if (currentScene != MENU && !bridge.isPaused()) {
             bridge.update(dt);
        }

        // --- Render ---
        renderer.BeginFrame(); 
        
        if (currentScene == MENU) {
            renderer.DrawText("Physics Demo Menu", 400, 200, 30);
            renderer.DrawText("Press 1: Ball Collision", 400, 300, 20);
            renderer.DrawText("Press 2: Block vs Wall", 400, 350, 20);
            renderer.DrawText("Press 3: Stacking", 400, 400, 20);
            renderer.DrawText("Press 4: Slope Simulation", 400, 450, 20);
        } else {
            // Draw Scene Name
            std::string title = "Scene: " + std::to_string(currentScene) + " (Press M for Menu)";
            renderer.DrawText(title, 10, 10);
            
            // Sync & Draw World
            bridge.syncPhysicsToRender();
            const auto& objs = bridge.getRenderObjects();
            
            // Draw Ground
            renderer.DrawText("Ground Level", 10, 780);
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
                else if (obj.type == RenderObject::BLOCK || obj.type == RenderObject::WALL) {
                     BlockData bd;
                     // PhysicsRenderBridge sets WALL to use block struct
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
        }

        // Always draw control buttons
        drawButtons(btnX, startY, pauseY, stopY, btnW, btnH);
        if (currentScene != MENU) {
            drawBtns(); // Parameter buttons
        }

        renderer.EndFrame();
    }
    
    return 0;
}