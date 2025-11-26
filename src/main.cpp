#include "shapes.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include "Renderer.h"
#include <chrono>
#include <thread>

using namespace std::chrono;

/*void print_shape_info(const Shape& shape) {
    double x, y, vx, vy;
    shape.getCentre(x, y);
    shape.getVelocity(vx, vy);
    
    std::cout << "质量: " << shape.getMass() 
              << ", 质心: (" << x << ", " << y << ")"
              << ", 速度: (" << vx << ", " << vy << ")" << std::endl;
}
*/


// 世界 -> 屏幕 坐标变换（y向上为正的物理坐标）
const double SCALE = 60.0; // 1 米 = 60 像素
const int WIN_W = 800;
const int WIN_H = 600;

// 将物理坐标 (wx, wy) 转为屏幕坐标 (sx, sy)
inline int wx2sx(double wx) { return (int)std::round(wx * SCALE); }
inline int wy2sy(double wy) { return WIN_H - (int)std::round(wy * SCALE); }

// 2D 向量小工具
struct Vec2 {
    double x, y;
    Vec2() : x(0), y(0) {}
    Vec2(double _x, double _y) : x(_x), y(_y) {}
    Vec2 operator+(const Vec2& o) const { return { x + o.x, y + o.y }; }
    Vec2 operator-(const Vec2& o) const { return { x - o.x, y - o.y }; }
    Vec2 operator*(double s) const { return { x * s, y * s }; }
    Vec2 operator/(double s) const { return { x / s, y / s }; }
};
inline double dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
inline double len(const Vec2& v) { return std::sqrt(v.x * v.x + v.y * v.y); }
inline Vec2 norm(const Vec2& v) { double L = len(v); return L == 0 ? Vec2{ 0,0 } : v / L; }

int main() {
    /*
    std::cout << "=== 形状演示程序 ===" << std::endl;
    
    // 创建不同的形状对象
    Circle circle1(2.0, 5.0, 0.0, 0.0);  // 质量2，半径5，在原点
    Circle circle2(1.5, 3.0, 10.0, 0.0); // 质量1.5，半径3，在(10,0)
    
    AABB rect1(3.0, 4.0, 6.0, 0.0, 0.0); // 质量3，宽4，高6，在原点
    AABB rect2(2.0, 3.0, 4.0, 15.0, 0.0); // 质量2，宽3，高4，在(15,0)
    
    std::cout << "\n初始状态:" << std::endl;
    std::cout << "圆形1: ";
    print_shape_info(circle1);
    std::cout << "  半径: " << circle1.getRadius() << std::endl;
    
    std::cout << "圆形2: ";
    print_shape_info(circle2);
    std::cout << "  半径: " << circle2.getRadius() << std::endl;
    
    std::cout << "矩形1: ";
    print_shape_info(rect1);
    std::cout << "  宽: " << rect1.getWidth() << ", 高: " << rect1.getHeight() << std::endl;
    
    std::cout << "矩形2: ";
    print_shape_info(rect2);
    std::cout << "  宽: " << rect2.getWidth() << ", 高: " << rect2.getHeight() << std::endl;
    
    // 碰撞检测测试
    std::cout << "\n碰撞检测:" << std::endl;
    std::cout << "圆形1与圆形2碰撞: " << (circle1.check_collision(circle2) ? "是" : "否") << std::endl;
    std::cout << "圆形1与矩形1碰撞: " << (circle1.check_collision(rect1) ? "是" : "否") << std::endl;
    std::cout << "矩形1与矩形2碰撞: " << (rect1.check_collision(rect2) ? "是" : "否") << std::endl;
    
    // 移动测试
    std::cout << "\n移动测试:" << std::endl;
    circle1.move(3.0, 4.0);
    std::cout << "圆形1移动后: ";
    print_shape_info(circle1);
    
    rect1.move(-2.0, 3.0);
    std::cout << "矩形1移动后: ";
    print_shape_info(rect1);
    
    // 旋转测试
    std::cout << "\n旋转测试:" << std::endl;
    circle1.setVelocity(5.0, 0.0);  // 设置初始速度
    std::cout << "圆形1旋转前速度: ";
    print_shape_info(circle1);
    
    circle1.turn(PI/4);  // 旋转45度
    std::cout << "圆形1旋转45度后: ";
    print_shape_info(circle1);
    
    // 使用多态性演示
    std::cout << "\n多态性演示:" << std::endl;
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(2.0, 1.0, 1.0));
    shapes.push_back(std::make_unique<AABB>(2.0, 3.0, 2.0, 2.0));
    shapes.push_back(std::make_unique<Circle>(1.5, 2.0, 5.0, 5.0));
    
    for (size_t i = 0; i < shapes.size(); ++i) {
        std::cout << "形状 " << (i+1) << ": ";
        print_shape_info(*shapes[i]);
        
        // 检查与其他形状的碰撞
        for (size_t j = i + 1; j < shapes.size(); ++j) {
            bool collision = shapes[i]->check_collision(*shapes[j]);
            std::cout << "  与形状" << (j+1) << "碰撞: " << (collision ? "是" : "否") << std::endl;
        }
    }
    
    std::cout << "\n=== 演示结束 ===" << std::endl;
    return 0;
    */
    // 初始化 EasyX 窗口
    initgraph(WIN_W, WIN_H);
    setbkcolor(WHITE);
    cleardevice();

    // 斜坡：定义两个点（物理世界坐标，单位：米）
    // p1 是上端，p2 是下端（保证 p1.y > p2.y）
    Vec2 p1(1.0, 4.0);  // 米
    Vec2 p2(7.0, 2.0);  // 米

    // 计算斜坡切线与法线单位向量
    Vec2 v = p2 - p1;
    double rampLength = len(v);
    Vec2 t = norm(v);              // 切线，指向下端
    Vec2 n = Vec2(-t.y, t.x);      // 切线逆时针旋转90°为法线（指向“上方”）

    // 小球参数（物理）
    double radius_m = 0.15; // 半径 m（根据需要调整）
    double mass = 1.0;      // 质量 kg（无量纲化时不影响）
    // 对于实心球，I = (2/5) m r^2，所以滚动加速度因子为 1/(1 + I/(m r^2)) = 1/(1 + 2/5) = 5/7
    const double I_over_mr2 = 2.0 / 5.0;
    const double rollFactor = 1.0 / (1.0 + I_over_mr2); // = 5/7 ≈ 0.7142857

    // 重力
    const double g = 9.81; // m/s^2

    // 在斜坡上的位置用标量 s 表示：球心投影到切线方向从 p1 开始的弧长（米）
    double s = 0.1; // 初始离顶部有一点距离，避免直接穿透
    double v_s = 0.0; // 切线方向速度（m/s）

    // 时间步与渲染频率
    const double dt = 1.0 / 240.0;  // 物理积分步长（更小更稳定）
    const double render_dt = 1.0 / 60.0; // 渲染每帧 ~60 FPS

    double acc_t = 0.0;

    // 计算简易摩擦系数（滚动摩擦很小，若需要可调整）
    const double rolling_friction = 0.02; // 经验值，减小球最终速度

    // 主循环：用固定步长多次积分然后渲染
    auto lastRender = high_resolution_clock::now();
    bool running = true;
    BeginBatchDraw();

    while (running) {
        // 检查按键（Esc 退出）
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

        // 多次小步长推进以保证稳定（render_dt / dt 次）
        double accum = 0.0;
        auto t0 = high_resolution_clock::now();
        // target step count
        int steps = (int)std::round(render_dt / dt);
        if (steps < 1) steps = 1;
        for (int i = 0; i < steps; ++i) {
            // 斜坡局部角度：使用切线与重力 gvec 的投影
            // 重力向量在物理坐标为 (0, -g)
            // 切向分量 a_t = (gvec dot t) * rollFactor = (-g * t.y) * rollFactor
            acc_t = (-g * t.y) * rollFactor;

            // 简单滚动摩擦阻力（与速度方向相反）
            double friction_acc = 0.0;
            if (v_s > 1e-6) friction_acc = -rolling_friction * g;
            else if (v_s < -1e-6) friction_acc = rolling_friction * g;

            // 合并加速度（沿切线）
            double a_total = acc_t + friction_acc;

            // 积分（半隐式欧拉可以稍稳定）
            v_s += a_total * dt;
            s += v_s * dt;

            // 边界：不要让球跑出斜坡两端（如果到达端点，停在端点）
            if (s < 0.0) {
                s = 0.0;
                v_s = 0.0;
            }
            if (s > rampLength - 1e-6) {
                s = rampLength; // 到底
                v_s = 0.0;
            }
        }

        // 渲染
        cleardevice();

        // 绘制斜坡线（像素坐标）
        int x1 = wx2sx(p1.x), y1 = wy2sy(p1.y);
        int x2 = wx2sx(p2.x), y2 = wy2sy(p2.y);
        setlinecolor(BLACK);
        setlinestyle(PS_SOLID, 4);
        line(x1, y1, x2, y2);

        // 球心在物理坐标：p = p1 + t * s + n * radius_m（把球放在斜坡法线上）
        Vec2 center = p1 + t * s + n * radius_m;
        int sx = wx2sx(center.x);
        int sy = wy2sy(center.y);
        int r_px = (int)std::round(radius_m * SCALE);

        // 绘制球（填充）
        setfillcolor(RGB(220, 50, 50));
        solidcircle(sx, sy, r_px);

        // 绘制速度向量（调试）
        int vx_px = (int)std::round((v_s * SCALE) * 0.2); // 缩短可视化
        setlinecolor(BLUE);
        line(sx, sy, sx + vx_px * (int)t.x, sy - vx_px * (int)t.y); // 注意屏幕方向

        FlushBatchDraw();

        // 控制渲染频率（睡眠一点）

        std::this_thread::sleep_for(milliseconds((int)(render_dt * 1000.0)));
    }

    EndBatchDraw();
    closegraph();
    return 0;
}

