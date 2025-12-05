#include "menu.h"

// ========================================================
//              全局对象（菜单运行所需）
// ========================================================

Renderer renderer(1280, 720);
DigitalRainBackgroundIntegrated background;
MusicPlayer musicPlayer(1180, 80, 50);   // 右上角音乐按钮

// 当前程序状态
AppState appState = AppState::MENU;

// 当前物理模型（场景1、场景2、场景3）
ModelType currentModel = ModelType::SLOPE;

// Model3：三个子模型（0=sphere creation, 1=two stars, 2=solar sys）
int model3SubScene = -1;   // 未选择任何子模型


// ========================================================
//                     工具函数
// ========================================================

// 重置所有“选择模型”的按钮状态
void resetModelSelection()
{
    resetButtonStates();
}


// ========================================================
//                    场景绘制和模拟
// ========================================================


// ----------- 场景1：斜面 -----------
void drawSlopePreview(const SlopeParams& p)
{
    renderer.DrawRamp(p.top_x, p.top_y, p.bottom_x, p.bottom_y);
    renderer.DrawBall(p.ball);
}

void simulateSlope(SlopeParams& p)
{
    // 重力加速度
    const double g = 9.8;

    // 方向
    double dx = p.bottom_x - p.top_x;
    double dy = p.bottom_y - p.top_y;

    double L = sqrt(dx*dx + dy*dy);
    if (L < 1) return;

    double ux = dx / L;
    double uy = dy / L;

    // 重力沿斜面分量
    double a = g * (ux * 0 + uy * 1) - p.mu * g;

    // 更新速度
    p.ball.vx += a * ux * 0.016;
    p.ball.vy += a * uy * 0.016;

    // 更新位置
    p.ball.x += p.ball.vx * 0.016;
    p.ball.y += p.ball.vy * 0.016;

    // 到达底部停止
    if (p.ball.x > p.bottom_x - 1 && p.ball.y > p.bottom_y - 1)
    {
        p.ball.vx = p.ball.vy = 0;
    }
}


// ----------- 场景2：双球碰撞 -----------
void drawCollisionPreview(const CollisionParams& p)
{
    renderer.DrawBall(p.b1);
    renderer.DrawBall(p.b2);
}

void simulateCollision(CollisionParams& p)
{
    // 更新位置
    p.b1.x += p.b1.vx * 0.016;
    p.b2.x += p.b2.vx * 0.016;

    // 检查碰撞
    double dx = p.b1.x - p.b2.x;
    double dy = p.b1.y - p.b2.y;
    double dist = sqrt(dx*dx + dy*dy);

    if (dist < p.b1.r + p.b2.r)
    {
        // 一维弹性碰撞
        double v1 = p.b1.vx;
        double v2 = p.b2.vx;

        p.b1.vx = (v1*(p.b1.m - p.b2.m) + 2*p.b2.m*v2) / (p.b1.m + p.b2.m);
        p.b2.vx = (v2*(p.b2.m - p.b1.m) + 2*p.b1.m*v1) / (p.b1.m + p.b2.m);
    }
}


// ========================================================
//                     Model3 三子场景
// ========================================================

// ★ 你可以根据需要实现自己的动画
void drawScene3_sphereCreation()
{
    outtextxy(850, 300, "Sphere Creation Preview");
}

void drawScene3_twoStars()
{
    outtextxy(850, 300, "Binary Stars Preview");
}

void drawScene3_solarSystem()
{
    outtextxy(850, 300, "Solar System Preview");
}


// ========================================================
//                     UI 绘制：菜单状态
// ========================================================

void drawMenuUI()
{
    // 背景
    background.UpdateAndDraw();

    // 标题
    settextstyle(30, 0, "Consolas");
    outtextxy(50, 30, "Physics Visual Demo Menu");

    // 1. 物理场景三按钮 (model1 / model2 / model3)
    int btnX, m1Y, m2Y, m3Y, btnW, btnH;
    initButtons(btnX, m1Y, m2Y, m3Y, btnW, btnH, 80);
    drawButtons(btnX, m1Y, m2Y, m3Y, btnW, btnH);

    // 2. 根据模型类型显示详细设置 ----
    if (currentModel == ModelType::SLOPE)
    {
        outtextxy(350, 110, "Model: Slope");
        drawBtns();       // 4 按钮界面
    }
    else if (currentModel == ModelType::COLLISION)
    {
        outtextxy(350, 110, "Model: Collision");
        drawBtns2();      // 8 按钮界面
    }
    else if (currentModel == ModelType::MODEL3)
    {
        outtextxy(350, 110, "Model: Scene Model Set");

        // 子场景按钮（sphere / two-stars / solar system）
        int sbX, s1Y, s2Y, s3Y, sbW, sbH;
        initSceneModelButtons(sbX, s1Y, s2Y, s3Y, sbW, sbH);
        drawSceneModelButtons(sbX, s1Y, s2Y, s3Y, sbW, sbH);

        // 预览
        if (model3SubScene == 0) drawScene3_sphereCreation();
        if (model3SubScene == 1) drawScene3_twoStars();
        if (model3SubScene == 2) drawScene3_solarSystem();
    }

    // 3. Start/Pause/Stop 三按钮
    int bX, sY, pY, stY, BW, BH;
    initButtons(bX, sY, pY, stY, BW, BH, 20);
    drawButtons(bX, sY, pY, stY, BW, BH);

    // 4. 音乐按钮
    musicPlayer.Draw();
}


// ========================================================
//                 UI 输入（鼠标处理）
// ========================================================

void handleMenuMouse()
{
    ExMessage msg;
    while (peekmessage(&msg, EM_MOUSE))
    {
        // 音乐按钮
        if (musicPlayer.HandleMouseInput(msg.x, msg.y, msg.message))
            continue;

        // 模型选择按钮（Model1/Model2/Model3）
        int x, m1, m2, m3, w, h;
        initButtons(x, m1, m2, m3, w, h, 80);
        handleMouseInput(x, m1, m2, m3, w, h);

        if (getmodel1ButtonState()) { currentModel = ModelType::SLOPE; resetModelSelection(); }
        if (getmodel2ButtonState()) { currentModel = ModelType::COLLISION; resetModelSelection(); }
        if (getmodel3ButtonState()) { currentModel = ModelType::MODEL3; resetModelSelection(); }

        // Model3 子模型按钮（三选一）
        if (currentModel == ModelType::MODEL3)
        {
            int sbX, s1Y, s2Y, s3Y, sbW, sbH;
            initSceneModelButtons(sbX, s1Y, s2Y, s3Y, sbW, sbH);
            handleSceneModelMouseInput(sbX, s1Y, s2Y, s3Y, sbW, sbH);

            if (getSphereCreationButtonState()) { model3SubScene = 0; resetModelSelection(); }
            if (getTwoStarsButtonState())       { model3SubScene = 1; resetModelSelection(); }
            if (getSolarSysButtonState())       { model3SubScene = 2; resetModelSelection(); }
        }
    }
}


// ========================================================
//                 主菜单运行函数（供 main.cpp 调用）
// ========================================================

void runMenu()
{
    // 参数初始化
    SlopeParams slopeParams;
    slopeParams.top_x = 200; slopeParams.top_y = 200;
    slopeParams.bottom_x = 400; slopeParams.bottom_y = 450;
    slopeParams.mu = 0.2;
    slopeParams.ball = { 200, 200, 0, 0, 20, 3 };

    CollisionParams colParams;
    colParams.b1 = { 300, 350, 100, 0, 25, 3 };
    colParams.b2 = { 600, 350, -80, 0, 30, 3 };

    while (true)
    {
        renderer.BeginFrame();

        if (appState == AppState::MENU)
        {
            drawMenuUI();
            handleMenuMouse();
        }
        else if (appState == AppState::DEMO_RUNNING)
        {
            background.UpdateAndDraw();

            if (currentModel == ModelType::SLOPE)
            {
                simulateSlope(slopeParams);
                drawSlopePreview(slopeParams);
            }
            else if (currentModel == ModelType::COLLISION)
            {
                simulateCollision(colParams);
                drawCollisionPreview(colParams);
            }
            else if (currentModel == ModelType::MODEL3)
            {
                if (model3SubScene == 0) drawScene3_sphereCreation();
                if (model3SubScene == 1) drawScene3_twoStars();
                if (model3SubScene == 2) drawScene3_solarSystem();
            }
        }

        renderer.EndFrame();
        Sleep(10);
    }
}
