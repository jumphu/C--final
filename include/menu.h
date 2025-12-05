#pragma once
/*************************************************************
 * menu.h
 * 
 * 说明：
 * 该头文件只用于声明 menu.cpp 中使用到的所有类型、结构体、
 * 变量、函数以及外部模块的声明。
 * 
 * 不做任何新变量/新函数定义。
 *************************************************************/

// -------------------- 依赖的外部模块 --------------------
#include "Renderer.h"
#include "background_integrated.h"
#include "allbottums.h"
#include "music.h"

#include <string>
#include <vector>

// -------------------- 枚举类型声明 --------------------

// menu.cpp 中的程序状态
enum class AppState { MENU, DEMO_RUNNING, DEMO_PAUSED };

// 当前选择的物理模型
enum class ModelType { SLOPE = 0, COLLISION = 1 };

// -------------------- 参数结构体声明 --------------------

// 斜面场景参数（menu.cpp 中声明）
struct SlopeParams {
    double top_x, top_y;
    double bottom_x, bottom_y;
    double mu;
    BallData ball;
};

// 碰撞场景参数
struct CollisionParams {
    BallData b1, b2;
};

// -------------------- menu.cpp 中使用到的外部模块声明 --------------------

// 背景模块
// class DigitalRainBackgroundIntegrated;    // 已在头文件中定义

// Renderer（渲染器）
// class Renderer;                           // 已在 Renderer.h 中定义

// 按钮系统 allbottums.h 已提供所有声明
// three buttons:  isStartClicked, etc.
// 4-button panel: initBtns / drawBtns / updBtns / getColor / getGrav...
// 8-button panel: initBtns2 / drawBtns2 / updBtns2 / getColor1...

// 音乐模块 music.h 已提供所有声明
// class MusicPlayer;


// menu.cpp 内部主程序使用的函数不需要声明（无独立函数）


// -------------------- menu.cpp 外部无额外函数需要声明 --------------------

// menu.cpp 的 main 由 menu.cpp 自身实现，无需在此声明。
// 若你希望声明 main，可启用：
// int main();

