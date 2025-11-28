#include "shapes.h"
#include "physicalWorld.h"
#include <iostream>
#include <vector>
#include <memory>
#include <iomanip>

/*=========================================================================================================
 * 辅助函数：打印形状详细信息
 *=========================================================================================================*/
void print_shape_info(const Shape& shape) {
    double x, y, vx, vy;
    shape.getCentre(x, y);
    shape.getVelocity(vx, vy);
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "名称: " << shape.getName() 
              << ", 类型: " << shape.getType()
              << ", 质量: " << shape.getMass() 
              << ", 位置: (" << x << ", " << y << ")"
              << ", 速度: (" << vx << ", " << vy << ")" << std::endl;
}

/*=========================================================================================================
 * 辅助函数：打印分隔线
 *=========================================================================================================*/
void print_separator(const std::string& title = "") {
    std::cout << "\n========================================" << std::endl;
    if (!title.empty()) {
        std::cout << "  " << title << std::endl;
        std::cout << "========================================" << std::endl;
    }
}

/*=========================================================================================================
 * 演示1: 基本形状创建和操作
 *=========================================================================================================*/
void demo_basic_shapes() {
    print_separator("演示1: 基本形状创建和操作");
    
    // 创建不同的形状对象
    Circle circle1(2.0, 5.0, 0.0, 0.0);
    circle1.setName("圆形1");
    
    Circle circle2(1.5, 3.0, 10.0, 0.0);
    circle2.setName("圆形2");
    
    AABB rect1(3.0, 4.0, 6.0, 0.0, 0.0);
    rect1.setName("矩形1");
    
    AABB rect2(2.0, 3.0, 4.0, 15.0, 0.0);
    rect2.setName("矩形2");
    
    std::cout << "\n初始状态:" << std::endl;
    print_shape_info(circle1);
    std::cout << "  半径: " << circle1.getRadius() << ", 面积: " << circle1.getArea() << std::endl;
    
    print_shape_info(circle2);
    std::cout << "  半径: " << circle2.getRadius() << ", 面积: " << circle2.getArea() << std::endl;
    
    print_shape_info(rect1);
    std::cout << "  宽: " << rect1.getWidth() << ", 高: " << rect1.getHeight() 
              << ", 面积: " << rect1.getArea() << std::endl;
    
    print_shape_info(rect2);
    std::cout << "  宽: " << rect2.getWidth() << ", 高: " << rect2.getHeight() 
              << ", 面积: " << rect2.getArea() << std::endl;
    
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
}

/*=========================================================================================================
 * 演示2: 物理世界基础功能
 *=========================================================================================================*/
void demo_physical_world_basic() {
    print_separator("演示2: 物理世界基础功能");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.setTimeStep(1.0 / 60.0);
    
    std::cout << "物理世界设置:" << std::endl;
    std::cout << "  重力加速度: " << world.getGravity() << " m/s²" << std::endl;
    std::cout << "  时间步长: " << world.getTimeStep() << " 秒" << std::endl;
    
    Ground ground(0.0, 0.3);
    ground.setName("地面");
    std::cout << "  地面高度: " << ground.getYLevel() << std::endl;
    std::cout << "  地面摩擦系数: " << ground.getFriction() << std::endl;
    
    Circle* circle = new Circle(1.0, 2.0, 5.0, 10.0);
    circle->setName("测试圆形");
    circle->setFraction(0.2);
    
    AABB* box = new AABB(2.0, 3.0, 3.0, 15.0, 10.0);
    box->setName("测试矩形");
    box->setFraction(0.3);
    
    world.addDynamicShape(circle);
    world.addDynamicShape(box);
    
    std::cout << "\n添加的形状:" << std::endl;
    world.printDynamicShapes();
    
    std::cout << "\n通过名称查找形状:" << std::endl;
    Shape* found = world.findShapeByName("测试圆形");
    if (found) {
        std::cout << "找到形状: ";
        print_shape_info(*found);
    }
    
    std::cout << "\n查找所有圆形:" << std::endl;
    std::vector<Shape*> circles = world.findShapesByType("Circle");
    std::cout << "找到 " << circles.size() << " 个圆形" << std::endl;
    
    delete circle;
    delete box;
}

/*=========================================================================================================
 * 演示3: 重力和自由落体
 *=========================================================================================================*/
void demo_gravity_simulation() {
    print_separator("演示3: 重力和自由落体模拟");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    
    Ground ground(0.0, 0.1);
    ground.setName("地面");
    
    Circle* fallingBall = new Circle(1.0, 2.0, 10.0, 20.0);
    fallingBall->setName("自由落体球");
    fallingBall->setRestitution(0.8);
    
    std::vector<Shape*> shapes;
    shapes.push_back(fallingBall);
    
    std::cout << "初始状态:" << std::endl;
    print_shape_info(*fallingBall);
    
    std::cout << "\n模拟自由落体 (10帧):" << std::endl;
    for (int i = 0; i < 10; i++) {
        world.update(shapes, ground);
        
        if (i % 2 == 0) {
            std::cout << "第 " << (i+1) << " 帧: ";
            print_shape_info(*fallingBall);
        }
    }
    
    delete fallingBall;
}

/*=========================================================================================================
 * 演示4: 形状堆叠和支撑
 *=========================================================================================================*/
void demo_stacking() {
    print_separator("演示4: 形状堆叠和支撑");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    
    Ground ground(0.0, 0.3);
    ground.setName("地面");
    
    AABB* box1 = new AABB(2.0, 4.0, 3.0, 10.0, 1.5);
    box1->setName("底部方块");
    world.placeShapeOnGround(*box1, ground);
    
    AABB* box2 = new AABB(1.5, 3.5, 2.5, 10.0, 4.5);
    box2->setName("中间方块");
    
    AABB* box3 = new AABB(1.0, 3.0, 2.0, 10.0, 7.5);
    box3->setName("顶部方块");
    
    std::vector<Shape*> shapes;
    shapes.push_back(box1);
    shapes.push_back(box2);
    shapes.push_back(box3);
    
    std::cout << "初始堆叠状态:" << std::endl;
    for (auto shape : shapes) {
        print_shape_info(*shape);
    }
    
    std::cout << "\n模拟5帧后的状态:" << std::endl;
    for (int i = 0; i < 5; i++) {
        world.update(shapes, ground);
    }
    
    for (auto shape : shapes) {
        print_shape_info(*shape);
        std::cout << "  是否被支撑: " << (shape->getIsSupported() ? "是" : "否") << std::endl;
    }
    
    for (auto shape : shapes) {
        delete shape;
    }
}

/*=========================================================================================================
 * 演示5: 摩擦力和滑动
 *=========================================================================================================*/
void demo_friction() {
    print_separator("演示5: 摩擦力和滑动");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    
    Ground ground(0.0, 0.5);
    ground.setName("粗糙地面");
    
    AABB* slidingBox = new AABB(2.0, 4.0, 3.0, 5.0, 1.5);
    slidingBox->setName("滑动方块");
    slidingBox->setVelocity(10.0, 0.0);
    slidingBox->setFraction(0.4);
    
    world.placeShapeOnGround(*slidingBox, ground);
    
    std::vector<Shape*> shapes;
    shapes.push_back(slidingBox);
    
    std::cout << "初始状态 (水平速度 10 m/s):" << std::endl;
    print_shape_info(*slidingBox);
    
    std::cout << "\n滑动过程 (每5帧打印一次):" << std::endl;
    for (int i = 0; i < 30; i++) {
        world.update(shapes, ground);
        
        if (i % 5 == 0) {
            std::cout << "第 " << (i+1) << " 帧: ";
            print_shape_info(*slidingBox);
        }
    }
    
    delete slidingBox;
}

/*=========================================================================================================
 * 演示6: 碰撞检测和响应
 *=========================================================================================================*/
void demo_collision() {
    print_separator("演示6: 碰撞检测和响应");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.setBounds(-50, 50, 0, 50);
    
    Ground ground(0.0, 0.2);
    ground.setName("地面");
    
    Circle* ball1 = new Circle(1.0, 2.0, 10.0, 10.0);
    ball1->setName("球1");
    ball1->setVelocity(5.0, 0.0);
    ball1->setRestitution(0.9);
    
    Circle* ball2 = new Circle(1.0, 2.0, 20.0, 10.0);
    ball2->setName("球2");
    ball2->setVelocity(-3.0, 0.0);
    ball2->setRestitution(0.9);
    
    std::vector<Shape*> shapes;
    shapes.push_back(ball1);
    shapes.push_back(ball2);
    
    std::cout << "初始状态:" << std::endl;
    print_shape_info(*ball1);
    print_shape_info(*ball2);
    
    std::cout << "\n碰撞前:" << std::endl;
    std::cout << "是否碰撞: " << (ball1->check_collision(*ball2) ? "是" : "否") << std::endl;
    
    std::cout << "\n模拟碰撞过程:" << std::endl;
    for (int i = 0; i < 20; i++) {
        world.update(shapes, ground);
        
        if (ball1->check_collision(*ball2)) {
            std::cout << "第 " << (i+1) << " 帧发生碰撞!" << std::endl;
            print_shape_info(*ball1);
            print_shape_info(*ball2);
            break;
        }
        
        if (i % 5 == 0) {
            std::cout << "第 " << (i+1) << " 帧: ";
            double x1, y1, x2, y2;
            ball1->getCentre(x1, y1);
            ball2->getCentre(x2, y2);
            std::cout << "距离: " << std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)) << std::endl;
        }
    }
    
    delete ball1;
    delete ball2;
}

/*=========================================================================================================
 * 演示7: 综合场景
 *=========================================================================================================*/
void demo_complex_scene() {
    print_separator("演示7: 综合场景模拟");
    
    PhysicalWorld world;
    world.setGravity(9.8);
    world.setBounds(-30, 30, 0, 50);
    
    Ground ground(0.0, 0.3);
    ground.setName("地面");
    
    Circle* ball1 = new Circle(1.0, 2.0, -10.0, 20.0);
    ball1->setName("红球");
    ball1->setRestitution(0.7);
    
    Circle* ball2 = new Circle(1.5, 2.5, 10.0, 25.0);
    ball2->setName("蓝球");
    ball2->setRestitution(0.8);
    
    AABB* box1 = new AABB(3.0, 5.0, 4.0, 0.0, 2.0);
    box1->setName("大箱子");
    box1->setFraction(0.4);
    
    AABB* box2 = new AABB(2.0, 4.0, 3.0, 15.0, 1.5);
    box2->setName("小箱子");
    box2->setFraction(0.3);
    box2->setVelocity(-2.0, 0.0);
    
    std::vector<Shape*> shapes;
    shapes.push_back(ball1);
    shapes.push_back(ball2);
    shapes.push_back(box1);
    shapes.push_back(box2);
    
    world.placeShapeOnGround(*box1, ground);
    world.placeShapeOnGround(*box2, ground);
    
    std::cout << "初始场景:" << std::endl;
    for (auto shape : shapes) {
        print_shape_info(*shape);
    }
    
    std::cout << "\n模拟30帧 (每10帧打印一次):" << std::endl;
    for (int frame = 0; frame < 30; frame++) {
        world.update(shapes, ground);
        
        if (frame % 10 == 0) {
            std::cout << "\n第 " << (frame+1) << " 帧:" << std::endl;
            for (auto shape : shapes) {
                print_shape_info(*shape);
            }
        }
    }
    
    std::cout << "\n最终统计:" << std::endl;
    int supported = 0;
    for (auto shape : shapes) {
        if (shape->getIsSupported()) {
            supported++;
        }
    }
    std::cout << "被支撑的形状数量: " << supported << " / " << shapes.size() << std::endl;
    
    for (auto shape : shapes) {
        delete shape;
    }
}

/*=========================================================================================================
 * 主函数 - 运行所有演示
 *=========================================================================================================*/
int main() {
    std::cout << "╔══════════════════════════════════════╗" << std::endl;
    std::cout << "║    物理引擎演示程序 v1.0             ║" << std::endl;
    std::cout << "║    Physics Engine Demo Program      ║" << std::endl;
    std::cout << "╚══════════════════════════════════════╝" << std::endl;
    
    try {
        demo_basic_shapes();
        demo_physical_world_basic();
        demo_gravity_simulation();
        demo_stacking();
        demo_friction();
        demo_collision();
        demo_complex_scene();
        
        print_separator("所有演示完成!");
        std::cout << "程序成功运行!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n按回车键退出..." << std::endl;
    std::cin.get();
    
    return 0;
}
