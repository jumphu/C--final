#include "shapes.h"
#include <iostream>
#include <vector>
#include <memory>

void print_shape_info(const Shape& shape) {
    double x, y, vx, vy;
    shape.getCentre(x, y);
    shape.getVelocity(vx, vy);
    
    std::cout << "质量: " << shape.getMass() 
              << ", 质心: (" << x << ", " << y << ")"
              << ", 速度: (" << vx << ", " << vy << ")" << std::endl;
}
/*
int main() {
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
}
*/