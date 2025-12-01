/*=========================================================================================================
 * RotatedAABB 使用示例
 * 演示如何使用 RotatedAABB 类创建旋转矩形并进行碰撞检测
 *=========================================================================================================*/

#include "../include/shapes.h"
#include <iostream>
#include <cmath>
#include <iomanip>

int main() {
    std::cout << "========== RotatedAABB 使用示例 ==========" << std::endl;
    std::cout << std::endl;
    
    // ========== 示例1：创建旋转矩形 ==========
    std::cout << "示例1：创建旋转矩形" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // 创建一个4x2的矩形，旋转30度
    double angle = 30.0 * PI / 180.0;
    RotatedAABB rect1(4.0, 2.0, 0.0, 0.0, angle);
    rect1.setName("Rect1");
    
    std::cout << "矩形 " << rect1.getName() << ":" << std::endl;
    std::cout << "  宽度: " << rect1.getWidth() << " m" << std::endl;
    std::cout << "  高度: " << rect1.getHeight() << " m" << std::endl;
    std::cout << "  旋转角度: " << rect1.getRotationDegrees() << " 度" << std::endl;
    std::cout << "  面积: " << rect1.getArea() << " m?" << std::endl;
    std::cout << "  对角线: " << rect1.getDiagonal() << " m" << std::endl;
    std::cout << std::endl;
    
    // ========== 示例2：获取旋转后的顶点 ==========
    std::cout << "示例2：获取旋转后的顶点" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    double vertices[4][2];
    rect1.getVertices(vertices);
    
    std::cout << "矩形的四个顶点（世界坐标）：" << std::endl;
    const char* vertexNames[] = {"左下", "右下", "右上", "左上"};
    for (int i = 0; i < 4; i++) {
        std::cout << "  " << vertexNames[i] << ": (" 
                  << std::fixed << std::setprecision(2)
                  << vertices[i][0] << ", " << vertices[i][1] << ")" << std::endl;
    }
    std::cout << std::endl;
    
    // ========== 示例3：获取边界框 ==========
    std::cout << "示例3：获取旋转后的轴对齐边界框" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    double minX, maxX, minY, maxY;
    rect1.getRotatedBounds(minX, maxX, minY, maxY);
    
    std::cout << "轴对齐边界框：" << std::endl;
    std::cout << "  X范围: [" << minX << ", " << maxX << "]" << std::endl;
    std::cout << "  Y范围: [" << minY << ", " << maxY << "]" << std::endl;
    std::cout << "  边界框宽度: " << (maxX - minX) << " m" << std::endl;
    std::cout << "  边界框高度: " << (maxY - minY) << " m" << std::endl;
    std::cout << std::endl;
    
    // ========== 示例4：旋转操作 ==========
    std::cout << "示例4：旋转操作" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    RotatedAABB rect2(3.0, 1.5, 5.0, 5.0, 0.0);
    rect2.setName("Rect2");
    
    std::cout << "初始角度: " << rect2.getRotationDegrees() << " 度" << std::endl;
    
    // 每次旋转15度
    for (int i = 1; i <= 6; i++) {
        rect2.turn(15.0 * PI / 180.0);
        std::cout << "旋转" << (i * 15) << "度: " << rect2.getRotationDegrees() << " 度" << std::endl;
    }
    std::cout << std::endl;
    
    // ========== 示例5：碰撞检测 - 与圆形 ==========
    std::cout << "示例5：碰撞检测 - 旋转矩形与圆形" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    RotatedAABB rect3(4.0, 2.0, 0.0, 0.0, 45.0 * PI / 180.0);
    rect3.setName("RotatedRect");
    
    // 创建几个圆
    Circle circle1(0.5, 0.0, 0.0);
    circle1.setName("Circle1");
    
    Circle circle2(1.0, 3.0, 0.0);
    circle2.setName("Circle2");
    
    Circle circle3(0.5, 5.0, 5.0);
    circle3.setName("Circle3");
    
    std::cout << "矩形 " << rect3.getName() << " (旋转45度):" << std::endl;
    std::cout << "  与 " << circle1.getName() << " (中心处): " 
              << (rect3.check_collision(circle1) ? "碰撞" : "不碰撞") << std::endl;
    std::cout << "  与 " << circle2.getName() << " (边缘处): " 
              << (rect3.check_collision(circle2) ? "碰撞" : "不碰撞") << std::endl;
    std::cout << "  与 " << circle3.getName() << " (远处): " 
              << (rect3.check_collision(circle3) ? "碰撞" : "不碰撞") << std::endl;
    std::cout << std::endl;
    
    // ========== 示例6：碰撞检测 - 与普通矩形 ==========
    std::cout << "示例6：碰撞检测 - 旋转矩形与普通矩形" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    RotatedAABB rect4(3.0, 2.0, 0.0, 0.0, 30.0 * PI / 180.0);
    rect4.setName("RotatedRect");
    
    AABB aabb1(2.0, 2.0, 2.0, 0.0);
    aabb1.setName("AABB1");
    
    AABB aabb2(2.0, 2.0, 10.0, 10.0);
    aabb2.setName("AABB2");
    
    std::cout << "矩形 " << rect4.getName() << " (旋转30度):" << std::endl;
    std::cout << "  与 " << aabb1.getName() << " (相邻): " 
              << (rect4.check_collision(aabb1) ? "碰撞" : "不碰撞") << std::endl;
    std::cout << "  与 " << aabb2.getName() << " (远处): " 
              << (rect4.check_collision(aabb2) ? "碰撞" : "不碰撞") << std::endl;
    std::cout << std::endl;
    
    // ========== 示例7：碰撞检测 - 两个旋转矩形 ==========
    std::cout << "示例7：碰撞检测 - 两个旋转矩形" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    RotatedAABB rect5(4.0, 2.0, 0.0, 0.0, 0.0);
    rect5.setName("Rect5");
    
    RotatedAABB rect6(3.0, 1.5, 2.0, 0.0, 45.0 * PI / 180.0);
    rect6.setName("Rect6");
    
    std::cout << rect5.getName() << " (0度) 与 " << rect6.getName() << " (45度): "
              << (rect5.check_collision(rect6) ? "碰撞" : "不碰撞") << std::endl;
    
    // 旋转第一个矩形
    rect5.setRotationDegrees(90.0);
    std::cout << rect5.getName() << " (90度) 与 " << rect6.getName() << " (45度): "
              << (rect5.check_collision(rect6) ? "碰撞" : "不碰撞") << std::endl;
    std::cout << std::endl;
    
    // ========== 示例8：点包含检测 ==========
    std::cout << "示例8：点包含检测" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    RotatedAABB rect7(6.0, 4.0, 0.0, 0.0, 30.0 * PI / 180.0);
    rect7.setName("TestRect");
    
    struct Point {
        double x, y;
        const char* name;
    };
    
    Point testPoints[] = {
        {0.0, 0.0, "中心点"},
        {2.0, 1.0, "内部点"},
        {5.0, 5.0, "外部点"},
        {3.0, 0.0, "边缘附近点"}
    };
    
    std::cout << "矩形 " << rect7.getName() << " (旋转30度)：" << std::endl;
    for (const auto& p : testPoints) {
        bool inside = rect7.containsPoint(p.x, p.y);
        double dist = rect7.distanceToPoint(p.x, p.y);
        std::cout << "  " << p.name << " (" << p.x << ", " << p.y << "): " 
                  << (inside ? "在内部" : "在外部") 
                  << ", 距离 = " << std::fixed << std::setprecision(2) << dist << " m" << std::endl;
    }
    std::cout << std::endl;
    
    // ========== 示例9：物理运动和旋转 ==========
    std::cout << "示例9：物理运动和旋转" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // 创建一个带有速度和旋转的矩形
    RotatedAABB movingRect(2.0, 1.0, 0.0, 0.0, 2.0, 1.0, 0.0);
    movingRect.setName("MovingRect");
    
    std::cout << "模拟物体运动和旋转：" << std::endl;
    std::cout << std::endl;
    
    for (int frame = 0; frame <= 5; frame++) {
        double x, y, vx, vy;
        movingRect.getCentre(x, y);
        movingRect.getVelocity(vx, vy);
        
        std::cout << "帧 " << frame << ":" << std::endl;
        std::cout << "  位置: (" << std::fixed << std::setprecision(2) << x << ", " << y << ")" << std::endl;
        std::cout << "  速度: (" << vx << ", " << vy << ") m/s" << std::endl;
        std::cout << "  角度: " << movingRect.getRotationDegrees() << " 度" << std::endl;
        std::cout << std::endl;
        
        // 更新物理
        if (frame < 5) {
            movingRect.update(0.1);  // 0.1秒时间步
            movingRect.turn(10.0 * PI / 180.0);  // 每帧旋转10度
        }
    }
    
    // ========== 示例10：实际应用场景 ==========
    std::cout << "示例10：实际应用 - 旋转门碰撞检测" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // 模拟一扇旋转门
    RotatedAABB door(0.1, 2.0, 5.0, 5.0, 0.0);  // 薄板门，宽0.1m，高2m
    door.setName("RotatingDoor");
    
    // 一个角色
    Circle character(0.5, 4.0, 5.0);
    character.setName("Character");
    
    std::cout << "门从0度旋转到180度：" << std::endl;
    std::cout << std::endl;
    
    for (double angle = 0.0; angle <= 180.0; angle += 30.0) {
        door.setRotationDegrees(angle);
        bool collision = door.check_collision(character);
        
        std::cout << "门角度 " << angle << " 度: " 
                  << (collision ? "碰到角色 ??" : "未碰到角色 ?") << std::endl;
    }
    std::cout << std::endl;
    
    // ========== 总结 ==========
    std::cout << "========================================" << std::endl;
    std::cout << "示例演示完成！" << std::endl;
    std::cout << std::endl;
    std::cout << "RotatedAABB 主要特性：" << std::endl;
    std::cout << "  ? 支持任意角度旋转" << std::endl;
    std::cout << "  ? 精确的顶点计算" << std::endl;
    std::cout << "  ? 轴对齐边界框" << std::endl;
    std::cout << "  ? 与多种形状的碰撞检测" << std::endl;
    std::cout << "  ? 点包含和距离计算" << std::endl;
    std::cout << "  ? 物理运动支持" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
