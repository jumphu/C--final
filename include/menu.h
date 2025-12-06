#pragma once
/*************************************************************
 * menu.h
 *
 * 目的：
 *   声明 menu.cpp 中用到的所有枚举、结构体，并引入所有
 *   按钮模块、背景模块、音乐模块、渲染模块的声明。
 *   不新增任何变量或函数。
 *************************************************************/

// -------------------- 外部头文件 --------------------
#include "Renderer.h"
#include "background_integrated.h"
#include "music.h"
#include "allbuttons.h"     // ★ 已包含 4 按钮、8 按钮、三按钮、三场景按钮

#include <string>
#include <vector>

// ========================================================
//                     枚举类型
// ========================================================
enum class AppState { MENU, DEMO_RUNNING, DEMO_PAUSED };

enum class ModelType { 
    SLOPE = 0, 
    COLLISION = 1, 
    MODEL3 = 2               // ★ menu.cpp 中使用到
};

// ========================================================
//                 物理参数结构体声明
// ========================================================

// 斜面物理模型参数
struct SlopeParams {
    double top_x, top_y;
    double bottom_x, bottom_y;
    double mu;
    BallData ball;           // Renderer.h 中定义
};

// 两球碰撞物理模型参数
struct CollisionParams {
    BallData b1, b2;
};

// 注意：第三模型没有结构体，不新增任何内容
// 若未来需要，可在 menu.cpp 内部定义


// ========================================================
//    allbuttons.h 已提供全部按钮函数声明（无需重复）
// --------------------------------------------------------
//  - Start / Pause / Stop 三按钮
//  - 四按钮（单物体参数）
//  - 八按钮（双物体参数）
//  - 选择物理场景按钮（model1/model2/model3）
//  - ★ Model3 的三子场景按钮（sphere_creation / two_stars / solar_sys）
//  - 所有 extern 状态变量
// ========================================================


// ========================================================
//   background_integrated.h / music.h / Renderer.h 提供类声明
// ========================================================
// class DigitalRainBackgroundIntegrated;
// class MusicPlayer;
// class Renderer;


// ========================================================
// menu.cpp 不需要额外前置声明
// ========================================================

