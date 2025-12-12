#include "PhysicsRenderBridge.h"
#include "Renderer.h"
#include "ALL_BUTTONS_H"
#include "MusicPlayer.h"
#include "DigitalRainBackground.h"

int main() {
    // 1. 初始化EasyX
    initgraph(800, 600, EX_SHOWCONSOLE);
    setbkcolor(WHITE);
    cleardevice();
    
    // 2. 创建各模块
    // 2.1 创建渲染器
    Renderer renderer(800, 600, 100.0);  // 100 pixels per meter
    
    // 2.2 创建物理世界适配器
    auto physics_adapter = std::make_unique<DefaultPhysicalWorldAdapter>();
    
    // 2.3 创建桥梁
    PhysicsRenderBridge bridge(std::move(physics_adapter), &renderer);
    bridge.initialize(100.0, {400, 300});
    
    // 2.4 创建其他组件
    MusicPlayer music_player;
    DigitalRainBackground background;
    
    // 3. 初始化按钮
    initButtons(50, 50, 100, 40, 20);  // 从ALL_BUTTONS_H
    initBtns(800, 600);  // 附加按钮组1
    initBtns2(800, 600); // 附加按钮组2
    
    // 4. 主循环
    bool running = true;
    while (running && !_kbhit()) {
        // 4.1 处理输入
        std::vector<UserInput> inputs;
        handleMouseInput();  // 更新按钮状态
        updBtns();           // 更新附加按钮1
        updBtns2();          // 更新附加按钮2
        
        // 4.2 更新桥梁状态
        bridge.updateButtonStates();
        
        // 4.3 更新物理
        bridge.syncInputToPhysics(inputs);
        
        // 4.4 物理更新（简化）
        auto adapter = dynamic_cast<DefaultPhysicalWorldAdapter*>(
            bridge.getPhysicsAdapter());
        if (adapter) {
            adapter->update(0.016);  // 假设60FPS
        }
        
        // 4.5 同步到渲染
        bridge.syncPhysicsToRender();
        
        // 4.6 渲染
        renderer.BeginFrame();
        renderer.Clear(WHITE);
        
        // 渲染背景
        background.UpdateAndDraw();
        
        // 渲染物理对象
        const auto& render_objects = bridge.getRenderObjects();
        for (const auto& pair : render_objects) {
            const auto& obj = pair.second;
            
            switch (obj.type) {
                case RenderObject::BALL: {
                    BallData ball;
                    ball.x = obj.ball.x;
                    ball.y = obj.ball.y;
                    ball.radius = obj.ball.radius;
                    ball.vx = obj.ball.vx;
                    ball.vy = obj.ball.vy;
                    ball.mass = obj.ball.mass;
                    ball.color = obj.color;
                    renderer.DrawBall(ball);
                    break;
                }
                case RenderObject::BLOCK: {
                    BlockData block;
                    block.cx = obj.block.cx;
                    block.cy = obj.block.cy;
                    block.width = obj.block.width;
                    block.height = obj.block.height;
                    block.angle = obj.block.angle;
                    block.vx = obj.block.vx;
                    block.vy = obj.block.vy;
                    block.mass = obj.block.mass;
                    block.color = obj.color;
                    renderer.DrawBlock(block);
                    break;
                }
                case RenderObject::RAMP: {
                    RampData ramp;
                    ramp.x1 = obj.ramp.x1;
                    ramp.y1 = obj.ramp.y1;
                    ramp.x2 = obj.ramp.x2;
                    ramp.y2 = obj.ramp.y2;
                    ramp.mu = obj.ramp.mu;
                    renderer.DrawRamp(ramp);
                    break;
                }
            }
        }
        
        // 渲染按钮
        drawButtons();  // 主控制按钮
        drawBtns();     // 附加按钮组1
        drawBtns2();    // 附加按钮组2
        
        // 渲染音乐播放器
        music_player.Draw();
        
        renderer.EndFrame();
        
        Sleep(16);
    }
    
    closegraph();
    return 0;
}