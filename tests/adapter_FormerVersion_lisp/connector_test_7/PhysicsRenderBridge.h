/**
 * ============================================================
 * 文件: PhysicsRenderBridge.h
 * 描述: EasyX版本接口桥梁模块的头文件
 * 作用: 定义物理世界与EasyX渲染系统之间的数据结构和接口
 * 技术栈: EasyX图形库 + Windows API数据类型
 * 关键适配: 使用POINT/COLORREF替代SFML类型，适配EasyX坐标系
 * ============================================================
 */

#ifndef PHYSICS_RENDER_BRIDGE_H
#define PHYSICS_RENDER_BRIDGE_H

#include <graphics.h>        // EasyX图形库核心头文件
#include <windows.h>         // Windows API，提供POINT等数据类型
#include <vector>
#include <string>
#include <unordered_map>
#include "PhysicalWorld.h"   // 第一部分同学的物理世界接口

// ==================== 数据结构定义 ====================

/**
 * ButtonStates - 按钮状态结构 (适配EasyX)
 * 作用: 存储第二部分按钮系统的状态，对应Buttons.h中的全局变量
 * 接口方向: 输入接口，从第二部分(按钮系统)传递到本桥梁
 * 对应第二部分: Buttons.h中的 extern bool 变量
 * 注意: 需要与Buttons.h中的变量名保持一致以便同步
 */
struct ButtonStates {
    bool start_clicked;    // 对应Buttons.h中的isStartClicked
    bool pause_clicked;    // 对应Buttons.h中的isPauseClicked
    bool stop_clicked;     // 对应Buttons.h中的isStopClicked
    bool start_hovered;    // 对应Buttons.h中的isStartHovered
    bool pause_hovered;    // 对应Buttons.h中的isPauseHovered
    bool stop_hovered;     // 对应Buttons.h中的isStopHovered
};

/**
 * MusicState - 音乐播放器状态结构 (适配EasyX)
 * 作用: 存储音乐播放器的状态信息，对应MusicPlayer类
 * 接口方向: 输入接口，从第二部分(音乐播放器)传递到本桥梁
 * 对应第二部分: MusicPlayer.h中的MusicPlayer类状态
 * 技术特点: 使用POINT存储位置，与EasyX坐标系兼容
 */
struct MusicState {
    bool is_playing;       // 播放状态，对应MusicPlayer::isPlaying
    bool is_hovering;      // 悬停状态，对应MusicPlayer::isHovering
    POINT position;        // 按钮位置(像素坐标)，对应MusicPlayer::posX/Y
    int radius;            // 按钮半径，对应MusicPlayer::radius
};

/**
 * RenderState - 渲染状态结构 (适配EasyX)
 * 作用: 存储单个物理对象的渲染信息，用于传递给渲染系统
 * 接口方向: 输出接口，从本桥梁传递到第二部分(渲染系统)
 * 技术特点: 
 *   - 使用POINT/SIZE替代sf::Vector2f
 *   - 使用COLORREF替代sf::Color
 *   - 坐标系: EasyX屏幕坐标系(原点在左上角，Y轴向下)
 */
struct RenderState {
    POINT position;        // 屏幕位置(像素坐标，EasyX坐标系)
    SIZE size;             // 尺寸(像素单位，cx=宽度，cy=高度)
    COLORREF color;        // 颜色(EasyX的COLORREF格式，如RED、RGB(255,0,0))
    std::string name;      // 形状的唯一标识符，用于查找和更新
    std::string type;      // 形状类型:"circle","rectangle","triangle","pendulum"
};

/**
 * UserInput - 用户输入事件结构 (适配EasyX)
 * 作用: 封装所有类型的用户输入，统一传递到物理世界
 * 接口方向: 输入接口，从用户交互通过本桥梁传递到第一部分
 * 技术特点: 
 *   - 使用POINT存储鼠标位置，与EasyX的MOUSEMSG兼容
 *   - 枚举类型对应不同的交互操作
 *   - 统一事件处理接口，简化物理世界的事件处理
 */
struct UserInput {
    enum InputType {
        MOUSE_DRAG_START,   // 鼠标开始拖动物体(左键按下)
        MOUSE_DRAG_UPDATE,  // 鼠标拖动物体中(左键按下并移动)
        MOUSE_DRAG_END,     // 鼠标结束拖动物体(左键释放)
        BUTTON_CLICK,       // 按钮点击事件(开始/暂停/停止)
        PARAMETER_CHANGE,   // 参数改变事件(重力、摩擦、倾斜角度等)
        CREATE_SHAPE,       // 创建新形状事件(右键点击创建)
        DELETE_SHAPE,       // 删除形状事件
        MUSIC_TOGGLE        // 音乐开关事件(点击音乐按钮)
    };
    
    InputType type;           // 事件类型
    POINT mouse_position;     // 鼠标位置(屏幕坐标，EasyX坐标系)
    std::string shape_name;   // 相关形状的名称(用于删除、拖动等操作)
    std::string parameter_name; // 参数名称("gravity","friction","incline"等)
    double parameter_value;   // 参数值(如重力值9.8，摩擦系数0.1等)
    std::string shape_type;   // 形状类型(仅用于CREATE_SHAPE事件)
};

// ==================== 核心接口类 ====================

/**
 * PhysicsRenderBridge - EasyX版本接口桥梁核心类
 * 职责: 管理物理世界与EasyX渲染系统之间的所有数据流和控制流
 * 设计模式: 中介者模式，解耦物理计算和图形渲染
 * 关键技术适配:
 *   1. 坐标系统转换: 物理坐标系 ↔ EasyX屏幕坐标系
 *   2. 数据类型转换: 物理数据 ↔ EasyX渲染数据
 *   3. 事件系统适配: 统一事件接口 ↔ 物理世界接口
 *   4. 状态同步机制: 双向实时状态同步
 */
class PhysicsRenderBridge {
private:
    // ========== 私有成员变量 ==========
    PhysicalWorld& physics_world_; // 引用第一部分物理世界对象(只读引用)
    double pixels_per_meter_;      // 坐标缩放比例:像素/米(默认100)
    POINT world_offset_;           // 世界坐标原点在屏幕上的位置(默认屏幕中心)
    
    // 渲染对象映射表:形状名称 -> 渲染状态(使用哈希表快速查找)
    std::unordered_map<std::string, RenderState> render_objects_;
    
    ButtonStates button_states_;   // 当前按钮状态(从第二部分同步)
    MusicState music_state_;       // 当前音乐状态(从第二部分同步)
    
    // 交互状态跟踪
    std::string currently_dragging_; // 当前被拖动的形状名称(空字符串表示无拖动)
    POINT drag_offset_;              // 拖动时的鼠标偏移量(像素)

public:
    // ========== 公共接口方法 ==========
    
    /**
     * 构造函数
     * @param physics_world 物理世界引用(第一部分)
     * 作用: 初始化接口桥梁，建立与物理世界的连接
     * 注意: 通过引用持有物理世界，不负责其生命周期管理
     */
    PhysicsRenderBridge(PhysicalWorld& physics_world);
    
    /**
     * 初始化函数
     * @param ppm 像素/米比例(默认100，即1米=100像素)
     * @param offset 世界坐标偏移量(默认屏幕中心{400,300})
     * 作用: 设置坐标转换参数，清空所有状态，准备开始模拟
     * 注意: 需要在程序开始时调用，或在重置模拟时调用
     */
    void initialize(double ppm = 100.0, POINT offset = {400, 300});
    
    // ========== 坐标转换接口 ==========
    
    /**
     * 物理坐标转屏幕坐标
     * @param physics_x, physics_y 物理坐标(米，Y轴向上)
     * @return POINT 屏幕坐标(像素，EasyX坐标系，Y轴向下)
     * 关键算法: 
     *   screen_x = physics_x * ppm + offset_x
     *   screen_y = height - physics_y * ppm + offset_y (Y轴翻转)
     * 使用场景: 将物理计算的位置转换为EasyX绘制位置
     */
    POINT physicsToScreen(double physics_x, double physics_y);
    
    /**
     * 屏幕坐标转物理坐标
     * @param screen_x, screen_y 屏幕坐标(像素，EasyX坐标系)
     * @return POINT 物理坐标(米，Y轴向上)
     * 关键算法: 坐标转换的逆运算
     * 使用场景: 将鼠标点击位置转换为物理世界坐标
     */
    POINT screenToPhysics(int screen_x, int screen_y);
    
    // ========== 核心同步接口 ==========
    
    /**
     * 物理到渲染同步
     * 作用: 从物理世界获取最新状态，更新渲染状态
     * 算法步骤:
     *   1. 遍历物理世界的动态形状列表，更新或创建渲染状态
     *   2. 遍历物理世界的静态形状列表，用灰色区分
     *   3. 清理已从物理世界删除的形状的渲染状态
     * 调用时机: 每次物理更新后，渲染前调用
     */
    void syncPhysicsToRender();
    
    /**
     * 输入到物理同步
     * @param user_inputs 用户输入事件列表
     * 作用: 将用户输入事件传递给物理世界处理
     * 事件来源: 鼠标事件、按钮点击、参数调整、音乐控制等
     * 处理方式: 根据事件类型分发给不同的处理函数
     */
    void syncInputToPhysics(const std::vector<UserInput>& user_inputs);
    
    // ========== 状态管理接口 ==========
    
    /**
     * 更新按钮状态
     * @param button_states 最新的按钮状态(从第二部分获取)
     * 接口方向: 输入接口，从第二部分按钮系统同步状态
     * 对应第二部分: Buttons.h中的按钮状态变量
     * 注意: 需要与第二部分同学协调变量命名和更新时机
     */
    void updateButtonStates(const ButtonStates& button_states);
    
    /**
     * 更新音乐状态
     * @param music_state 最新的音乐状态(从第二部分获取)
     * 接口方向: 输入接口，从第二部分音乐播放器同步状态
     * 对应第二部分: MusicPlayer.h中的MusicPlayer类
     */
    void updateMusicState(const MusicState& music_state);
    
    /**
     * 处理按钮事件
     * 作用: 检查按钮点击状态，触发相应的物理世界操作
     * 内部逻辑: 遍历button_states_，对点击的按钮调用物理世界对应方法
     * 按钮映射:
     *   - start_clicked → physics_world_.start()
     *   - pause_clicked → physics_world_.Pause()/Continue()
     *   - stop_clicked → physics_world_.Stop()
     */
    void handleButtonEvents();
    
    // ========== 状态获取接口 ==========
    
    /**
     * 获取渲染状态
     * @return 所有形状的渲染状态映射表
     * 接口方向: 输出接口，供第二部分渲染系统获取绘制数据
     * 使用方式: 第二部分遍历此映射，调用EasyX绘图函数绘制每个形状
     */
    const std::unordered_map<std::string, RenderState>& getRenderStates() const;
    
    const ButtonStates& getButtonStates() const;  // 获取当前按钮状态(调试用)
    const MusicState& getMusicState() const;      // 获取当前音乐状态(调试用)
    
    // ========== 形状管理接口 ==========
    
    /**
     * 创建渲染形状
     * @param name 形状唯一名称(通常来自物理世界的Shape::getName())
     * @param type 形状类型("circle","rectangle"等)
     * @param color 形状颜色(默认白色)
     * 作用: 在渲染状态映射中添加新形状的渲染状态
     * 调用时机: 当物理世界创建新形状时，或用户创建形状时
     */
    void createRenderShape(const std::string& name, const std::string& type, 
                          COLORREF color = RGB(255, 255, 255));
    
    void removeRenderShape(const std::string& name);  // 删除渲染形状
    
    // ========== 物理参数获取接口 ==========
    
    /**
     * 物理参数获取方法组
     * 作用: 从物理世界获取当前参数值，供UI显示或调试
     * 接口方向: 输出接口，供第二部分UI显示当前参数
     * 对应第一部分: PhysicalWorld.h中的公有成员变量
     */
    double getGravity() const;      // 获取当前重力值(m/s²)
    double getInclineAngle() const; // 获取当前倾斜角度(度)
    double getFriction() const;     // 获取当前静摩擦系数
    double getTimeStep() const;     // 获取当前时间步长(秒)
    bool isPaused() const;          // 获取当前暂停状态
    
    // ========== 交互功能接口 ==========
    
    /**
     * 查找指定位置的形状
     * @param screen_pos 屏幕坐标位置(像素)
     * @param tolerance 容差范围(像素，默认10)
     * @return 形状名称，如未找到返回空字符串
     * 作用: 支持鼠标交互，查找被点击或拖动的形状
     * 算法: 计算鼠标位置与每个形状中心的距离，小于容差则命中
     * 使用场景: 鼠标拖动、删除、选中形状等交互操作
     */
    std::string findShapeAtPosition(POINT screen_pos, int tolerance = 10);

private:
    // ========== 私有辅助方法 ==========
    
    /**
     * 应用用户输入到物理世界
     * @param input 用户输入事件
     * 内部方法，由syncInputToPhysics()调用
     * 职责: 根据输入类型分发给不同的处理方法
     * 设计模式: 命令模式，每个输入事件封装为一个命令
     */
    void applyUserInputToPhysics(const UserInput& input);
    
    void handleMouseDrag(const UserInput& input);      // 处理鼠标拖动事件
    void handleParameterChange(const UserInput& input); // 处理参数改变事件
    void handleShapeCreation(const UserInput& input);  // 处理形状创建事件
    void handleButtonClick(const std::string& button_name); // 处理按钮点击
    
    /**
     * 根据形状类型获取颜色
     * @param type 形状类型字符串
     * @return COLORREF EasyX颜色值
     * 内部方法: 为不同类型的形状分配默认颜色
     * 颜色方案: 
     *   - 圆形/球体: RED
     *   - 矩形/盒子: BLUE
     *   - 三角形: GREEN
     *   - 钟摆: YELLOW
     *   - 其他: MAGENTA
     */
    COLORREF getColorForShapeType(const std::string& type);
    
    /**
     * 从Shape对象更新渲染状态
     * @param shape 物理世界的Shape对象指针
     * @param render_state 要更新的渲染状态引用
     * 作用: 将物理世界的Shape对象属性转换为渲染状态属性
     * 关键转换: 坐标转换、尺寸缩放、类型映射
     * 注意: 假设Shape有getX(), getY(), getName(), getType()等方法
     */
    void updateRenderStateFromShape(Shape* shape, RenderState& render_state);
};

#endif // PHYSICS_RENDER_BRIDGE_H