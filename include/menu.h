#pragma once
/*************************************************************
 * menu.h
 *
 * 目的：
 *  - 声明 menu.cpp 中会用到的所有类型（枚举、结构体）
 *  - 引入所有按钮模块、背景模块、音乐模块、渲染模块
 *  - 不新增任何变量或函数，只做声明
 *************************************************************/

// -------------------- 依赖外部模块 --------------------
#include "Renderer.h"
#include "background_integrated.h"
#include "allbuttons.h"
#include "music.h"

#include <string>
#include <vector>

// ========================================================
//                     枚举类型声明
// ========================================================

// 程序主状态
enum class AppState { MENU, DEMO_RUNNING, DEMO_PAUSED };

// 模型类型（原 menu.cpp）
enum class ModelType { SLOPE = 0, COLLISION = 1, MODEL3 = 2 };
// ★ 新增 MODEL3，保持 menu.cpp 能识别第三场景


// ========================================================
//                物理参数结构体（menu.cpp 用）
// ========================================================

// 斜面参数
struct SlopeParams {
    double top_x, top_y;
    double bottom_x, bottom_y;
    double mu;
    BallData ball;
};

// 碰撞参数
struct CollisionParams {
    BallData b1, b2;
};

// ------------------------
// 若第三个物理场景有参数结构体，按需补充：
// struct Model3Params {...};
// 但你要求不新增任何变量/函数，因此不添加
// ------------------------


// ========================================================
//              allbuttons.h 中已声明的全部内容
// ========================================================

// 三按钮（Start/Pause/Stop）
// extern bool isStartClicked;   etc...
// void drawButtons(...)
// void handleMouseInput(...)
// void initButtons(...)
// （这些在 allbuttons.h 已声明，不需重复）

// 4 按钮（单物体参数 UI）
// 8 按钮（双物体参数 UI）
// 两者在 allbuttons.h 中已全部声明，不需要重复

// ★ 3 个物理场景选择按钮（choose.cpp 部分）
// 这些函数与变量在 allbuttons.h 中 *已经声明*：
// extern bool model1Clicked, model2Clicked, model3Clicked;
// extern bool model1Hovered, model2Hovered, model3Hovered;
// bool getmodel1ButtonState();
// bool getmodel2ButtonState();
// bool getmodel3ButtonState();
// void drawButtons(...);         // choose.cpp 版本
// void handleMouseInput(...);    // choose.cpp 版本
// void initButtons(...);
// void resetButtonStates();
// （无需重复声明）

// 所以，本 menu.h 不需要再次声明它们


// ========================================================
//            背景模块（background_integrated）
// ========================================================
// class DigitalRainBackgroundIntegrated;


// ========================================================
//                   音乐模块（music.h）
// ========================================================
// class MusicPlayer;


// ========================================================
//                   Renderer（渲染器）
// ========================================================
// class Renderer;


// ========================================================
//  menu.cpp 没有独立函数，无需额外 forward declaration
// ========================================================

// 若需要，也可声明 main：
// int main();
// 但你要求不新增，这里不加。
