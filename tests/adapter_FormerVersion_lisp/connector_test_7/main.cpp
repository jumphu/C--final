/**
 * ============================================================
 * 文件: main.cpp (EasyX版本)
 * 描述: 主程序入口文件，演示如何集成所有模块
 * 作用: 创建EasyX窗口、初始化所有系统、运行主循环
 * 技术栈: EasyX图形库 + Windows消息循环
 * 架构: MVC模式
 *   - 模型(Model): PhysicalWorld(第一部分)
 *   - 视图(View): Renderer(第二部分)
 *   - 控制器(Controller): PhysicsRenderBridge(你的代码)
 * ============================================================
 */

#include <graphics.h>          // EasyX图形库主头文件
#include <conio.h>             // 控制台输入输出，用于_kbhit()
#include <iostream>            // 标准输入输出，用于调试信息
#include <sstream>             // 字符串流，用于格式化文本
#include "PhysicsRenderBridge.h" // 你的接口桥梁头文件

// ==================== 自定义EasyX UI组件 ====================

/**
 * EasyXButton - 自定义EasyX按钮类
 * 作用: 模拟第二部分按钮系统的功能，用于演示集成
 * 设计: 简单按钮实现，包含悬停和点击视觉效果
 * 实际项目: 应替换为第二部分的Buttons.h实现
 * EasyX特性: 使用fillrectangle绘制按钮，outtextxy绘制文本
 */
class EasyXButton {
private:
    int x, y;              // 按钮左上角位置(像素)
    int width, height;     // 按钮尺寸(像素)
    std::string label;     // 按钮显示文本
    bool isHovered;        // 鼠标悬停状态
    bool isClicked;        // 鼠标点击状态
    
public:
    /**
     * 按钮构造函数
     * @param x,y 按钮左上角位置(像素，EasyX坐标系)
     * @param w,h 按钮宽度和高度(像素)
     * @param text 按钮显示文本
     * 设计说明: 使用EasyX的矩形和文本函数创建按钮外观
     */
    EasyXButton(int x, int y, int w, int h, std::string text) 
        : x(x), y(y), width(w), height(h), label(text), isHovered(false), isClicked(false) 
    {
        // 初始绘制按钮
        redraw();
    }
    
    /**
     * 更新按钮状态
     * @param mouse EasyX的MOUSEMSG结构，包含鼠标状态
     * 作用: 检测鼠标悬停和点击，更新按钮外观
     * EasyX事件处理: 使用MOUSEMSG的x,y坐标和mkLButton状态
     */
    void update(const MOUSEMSG& mouse) {
        // 检测鼠标是否在按钮区域内
        isHovered = (mouse.x >= x && mouse.x <= x + width && 
                     mouse.y >= y && mouse.y <= y + height);
        
        // 检测鼠标左键点击
        if (isHovered && mouse.mkLButton) {
            isClicked = true;
        } else {
            isClicked = false;
        }
        
        // 根据状态重绘按钮
        redraw();
    }
    
    /**
     * 重绘按钮
     * 作用: 根据当前状态绘制按钮外观
     * EasyX绘图函数:
     *   - setfillcolor: 设置填充颜色
     *   - fillrectangle: 绘制填充矩形
     *   - setlinecolor: 设置边框颜色
     *   - rectangle: 绘制矩形边框
     *   - outtextxy: 绘制文本
     */
    void redraw() {
        // 根据状态选择颜色
        COLORREF fillColor, borderColor;
        
        if (isClicked) {
            fillColor = RGB(100, 100, 100);  // 点击状态: 深灰色
            borderColor = RGB(50, 50, 50);   // 深色边框
        } else if (isHovered) {
            fillColor = RGB(200, 200, 200);  // 悬停状态: 浅灰色
            borderColor = RGB(100, 100, 100); // 中等灰色边框
        } else {
            fillColor = RGB(150, 150, 150);  // 正常状态: 中灰色
            borderColor = RGB(0, 0, 0);      // 黑色边框
        }
        
        // 绘制按钮主体
        setfillcolor(fillColor);
        setlinecolor(borderColor);
        fillrectangle(x, y, x + width, y + height);
        rectangle(x, y, x + width, y + height);
        
        // 绘制按钮文本(居中)
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);  // 透明背景模式
        
        // 计算文本居中位置
        int textWidth = textwidth(label.c_str());
        int textHeight = textheight(label.c_str());
        int textX = x + (width - textWidth) / 2;
        int textY = y + (height - textHeight) / 2;
        
        outtextxy(textX, textY, label.c_str());
    }
    
    // 状态获取方法
    bool isButtonClicked() const { return isClicked; }
    bool isButtonHovered() const { return isHovered; }
    std::string getLabel() const { return label; }
    
    // 获取按钮区域(用于碰撞检测)
    RECT getRect() const {
        return {x, y, x + width, y + height};
    }
};

// ==================== 辅助函数 ====================

/**
 * 绘制物理参数信息
 * 作用: 在屏幕上显示当前物理参数值，用于调试和教学展示
 * 位置: 屏幕左上角区域
 * 显示内容: 重力、摩擦、倾斜角度、暂停状态、物体数量
 */
void drawPhysicsParameters(const PhysicsRenderBridge& bridge, int x, int y) {
    std::stringstream ss;
    ss << "物理参数显示:\n";
    ss << "重力: " << bridge.getGravity() << " m/s²\n";
    ss << "摩擦: " << bridge.getFriction() << "\n";
    ss << "倾斜: " << bridge.getInclineAngle() << "°\n";
    ss << "暂停: " << (bridge.isPaused() ? "是" : "否") << "\n";
    ss << "物体数: " << bridge.getRenderStates().size();
    
    // 设置文本样式
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);
    settextstyle(14, 0, "Consolas");
    
    // 绘制文本(多行处理)
    std::string text = ss.str();
    int lineHeight = textheight("A");
    int currentY = y;
    
    size_t start = 0, end;
    while ((end = text.find('\n', start)) != std::string::npos) {
        std::string line = text.substr(start, end - start);
        outtextxy(x, currentY, line.c_str());
        currentY += lineHeight;
        start = end + 1;
    }
}

/**
 * 绘制音乐播放器状态
 * 作用: 显示音乐按钮的当前状态
 * 设计: 圆形按钮，绿色=播放，红色=停止
 */
void drawMusicPlayer(const MusicState& musicState) {
    // 绘制圆形按钮
    setfillcolor(musicState.is_playing ? GREEN : RED);
    setlinecolor(BLACK);
    fillcircle(musicState.position.x, musicState.position.y, musicState.radius);
    circle(musicState.position.x, musicState.position.y, musicState.radius);
    
    // 绘制播放/暂停图标
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);
    settextstyle(12, 0, "Consolas");
    
    if (musicState.is_playing) {
        // 播放状态: 显示"II"(暂停符号)
        outtextxy(musicState.position.x - 6, musicState.position.y - 8, "II");
    } else {
        // 停止状态: 显示">"(播放符号)
        outtextxy(musicState.position.x - 4, musicState.position.y - 8, ">");
    }
}

// ==================== 主程序入口 ====================

/**
 * 主函数 - EasyX版本程序入口点
 * 程序流程:
 *   1. 初始化EasyX图形窗口
 *   2. 创建物理世界和接口桥梁
 *   3. 创建UI组件
 *   4. 运行主消息循环
 *   5. 处理输入、更新物理、渲染显示
 *   6. 程序结束，关闭图形窗口
 */
int main() {
    // ========== 第一步: 初始化图形系统 ==========
    
    std::cout << "正在初始化EasyX图形系统..." << std::endl;
    
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    
    // 初始化EasyX窗口
    // EX_SHOWCONSOLE参数保留控制台窗口，方便调试输出
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT, EX_SHOWCONSOLE);
    setbkcolor(WHITE);      // 设置背景色为白色
    cleardevice();          // 清空屏幕
    
    // 启用批量绘图，减少闪烁
    BeginBatchDraw();
    
    std::cout << "EasyX窗口创建成功: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << std::endl;
    
    // ========== 第二步: 初始化核心系统 ==========
    
    // 2.1 创建物理世界(第一部分同学的代码)
    std::cout << "初始化物理世界..." << std::endl;
    PhysicalWorld physics_world;
    
    // 2.2 创建接口桥梁(你的代码)
    std::cout << "初始化接口桥梁..." << std::endl;
    PhysicsRenderBridge bridge(physics_world);
    bridge.initialize(100.0, {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2}); // 屏幕中心为原点
    
    // ========== 第三步: 创建UI组件 ==========
    
    std::cout << "创建用户界面组件..." << std::endl;
    
    // 3.1 创建控制按钮(模拟第二部分Buttons.h功能)
    EasyXButton startButton(50, 50, 100, 40, "开始");
    EasyXButton pauseButton(50, 100, 100, 40, "暂停");
    EasyXButton stopButton(50, 150, 100, 40, "停止");
    
    // 3.2 创建音乐播放器状态
    MusicState musicState;
    musicState.is_playing = false;
    musicState.is_hovering = false;
    musicState.position = {50, 250};  // 音乐按钮位置
    musicState.radius = 20;
    
    std::cout << "应用程序初始化完成，进入主循环..." << std::endl;
    std::cout << "操作说明:" << std::endl;
    std::cout << "  - 左键点击按钮: 控制模拟" << std::endl;
    std::cout << "  - 右键点击空白处: 创建新物体" << std::endl;
    std::cout << "  - 左键拖动物体: 改变位置" << std::endl;
    std::cout << "  - 点击音乐按钮: 切换音乐状态" << std::endl;
    
    // ========== 第四步: 主消息循环 ==========
    
    // EasyX使用消息循环而不是事件队列
    bool isRunning = true;
    
    while (isRunning && !_kbhit()) {  // 按任意键退出
        // 4.1 处理鼠标消息
        std::vector<UserInput> user_inputs;
        
        // 检查是否有鼠标消息
        while (MouseHit()) {
            MOUSEMSG mouse = GetMouseMsg();
            
            // 更新按钮状态
            startButton.update(mouse);
            pauseButton.update(mouse);
            stopButton.update(mouse);
            
            // 更新音乐按钮悬停状态
            int dx = mouse.x - musicState.position.x;
            int dy = mouse.y - musicState.position.y;
            musicState.is_hovering = (dx * dx + dy * dy <= musicState.radius * musicState.radius);
            
            // 根据鼠标消息创建用户输入事件
            switch (mouse.uMsg) {
                case WM_LBUTTONDOWN:  // 左键按下
                    // 检查音乐按钮点击
                    if (musicState.is_hovering) {
                        UserInput musicEvent;
                        musicEvent.type = UserInput::MUSIC_TOGGLE;
                        musicEvent.mouse_position = {mouse.x, mouse.y};
                        user_inputs.push_back(musicEvent);
                        std::cout << "音乐按钮被点击" << std::endl;
                    }
                    // 鼠标拖动开始事件在MOUSEMOVE中处理
                    break;
                    
                case WM_LBUTTONUP:    // 左键释放
                    // 鼠标拖动结束
                    {
                        UserInput dragEnd;
                        dragEnd.type = UserInput::MOUSE_DRAG_END;
                        dragEnd.mouse_position = {mouse.x, mouse.y};
                        user_inputs.push_back(dragEnd);
                    }
                    break;
                    
                case WM_MOUSEMOVE:    // 鼠标移动
                    // 如果左键按下，则是拖动事件
                    if (mouse.mkLButton) {
                        UserInput dragUpdate;
                        dragUpdate.type = UserInput::MOUSE_DRAG_UPDATE;
                        dragUpdate.mouse_position = {mouse.x, mouse.y};
                        user_inputs.push_back(dragUpdate);
                    }
                    break;
                    
                case WM_RBUTTONDOWN:  // 右键按下(创建新物体)
                    {
                        UserInput createEvent;
                        createEvent.type = UserInput::CREATE_SHAPE;
                        createEvent.mouse_position = {mouse.x, mouse.y};
                        createEvent.shape_type = "circle";  // 默认创建圆形
                        user_inputs.push_back(createEvent);
                        
                        std::cout << "在位置(" << mouse.x << "," << mouse.y 
                                  << ")创建新圆形" << std::endl;
                    }
                    break;
            }
        }
        
        // 4.2 构建按钮状态(从UI组件同步到桥梁)
        ButtonStates button_states;
        button_states.start_clicked = startButton.isButtonClicked();
        button_states.pause_clicked = pauseButton.isButtonClicked();
        button_states.stop_clicked = stopButton.isButtonClicked();
        button_states.start_hovered = startButton.isButtonHovered();
        button_states.pause_hovered = pauseButton.isButtonHovered();
        button_states.stop_hovered = stopButton.isButtonHovered();
        
        // 4.3 更新接口桥梁状态
        bridge.updateButtonStates(button_states);
        bridge.updateMusicState(musicState);
        
        // 4.4 处理按钮事件
        bridge.handleButtonEvents();
        
        // 4.5 同步用户输入到物理世界
        bridge.syncInputToPhysics(user_inputs);
        
        // 4.6 物理模拟更新(如果未暂停)
        if (!bridge.isPaused()) {
            physics_world.update(physics_world.dynamicShapeList, physics_world.ground);
        }
        
        // 4.7 同步物理状态到渲染状态
        bridge.syncPhysicsToRender();
        
        // ========== 第五步: 渲染帧 ==========
        
        // 5.1 清除屏幕(白色背景)
        cleardevice();
        
        // 5.2 渲染物理形状
        // 注意: 实际项目中应该调用第二部分的Renderer
        // 这里简化处理，直接使用EasyX函数绘制
        const auto& render_states = bridge.getRenderStates();
        
        // 设置绘图属性
        setlinestyle(PS_SOLID, 2);  // 实线，2像素宽
        
        for (const auto& pair : render_states) {
            const RenderState& state = pair.second;
            
            // 根据形状类型选择绘图函数
            if (state.type == "circle" || state.type == "ball") {
                // 绘制圆形
                setfillcolor(state.color);
                setlinecolor(BLACK);
                fillcircle(state.position.x, state.position.y, state.size.cx / 2);
                circle(state.position.x, state.position.y, state.size.cx / 2);
            } 
            else if (state.type == "rectangle" || state.type == "box") {
                // 绘制矩形
                setfillcolor(state.color);
                setlinecolor(BLACK);
                int left = state.position.x - state.size.cx / 2;
                int top = state.position.y - state.size.cy / 2;
                int right = left + state.size.cx;
                int bottom = top + state.size.cy;
                fillrectangle(left, top, right, bottom);
                rectangle(left, top, right, bottom);
            }
            // 可以添加更多形状类型的绘制...
        }
        
        // 5.3 渲染UI组件
        startButton.redraw();
        pauseButton.redraw();
        stopButton.redraw();
        
        // 5.4 渲染音乐播放器
        drawMusicPlayer(bridge.getMusicState());
        
        // 5.5 显示物理参数
        drawPhysicsParameters(bridge, 200, 50);
        
        // 5.6 刷新显示(批量绘图)
        FlushBatchDraw();
        
        // 5.7 控制帧率(~60 FPS)
        Sleep(16);
    }
    
    // ========== 第六步: 程序清理 ==========
    
    std::cout << "程序结束，清理资源..." << std::endl;
    
    // 结束批量绘图
    EndBatchDraw();
    
    // 关闭图形窗口
    closegraph();
    
    std::cout << "感谢使用物理模拟系统!" << std::endl;
    std::cout << "按任意键退出..." << std::endl;
    _getch();  // 等待用户按键
    
    return 0;
}