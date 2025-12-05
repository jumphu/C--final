/*=========================================================================================================
 * Slope Placement Example
 * 演示如何使用Slope类的放置方法在斜面上放置物体
 *=========================================================================================================*/

#include <iostream>
#include "shapes.h"
#include "physicalWorldWithSlope.h"

int main() {
    std::cout << "========== 斜面放置物体示例 ==========" << std::endl;
    std::cout << std::endl;
    
    // 创建一个斜面：长度10，角度30度（约0.524弧度），位置在(0, 0)
    double angleInDegrees = 30.0;
    double angleInRadians = angleInDegrees * 3.14159265359 / 180.0;
    Slope slope(INFINITY, 10.0, angleInRadians, 0.0, 0.0);
    slope.setName("MainSlope");
    slope.setFraction(0.3);  // 摩擦系数0.3
    
    std::cout << "创建斜面：" << std::endl;
    std::cout << "  名称: " << slope.getName() << std::endl;
    std::cout << "  长度: " << slope.getLength() << " m" << std::endl;
    std::cout << "  角度: " << slope.getAngleDegrees() << " 度" << std::endl;
    std::cout << "  高度: " << slope.getHeight() << " m" << std::endl;
    std::cout << "  底边: " << slope.getBase() << " m" << std::endl;
    std::cout << "  摩擦系数: " << slope.fraction << std::endl;
    std::cout << std::endl;
    
    // 获取斜面的端点
    double bottomX, bottomY, topX, topY;
    slope.getBottomPoint(bottomX, bottomY);
    slope.getTopPoint(topX, topY);
    std::cout << "斜面端点：" << std::endl;
    std::cout << "  底部: (" << bottomX << ", " << bottomY << ")" << std::endl;
    std::cout << "  顶部: (" << topX << ", " << topY << ")" << std::endl;
    std::cout << std::endl;
    
    // 获取斜面的法向量和切向量
    double nx, ny, tx, ty;
    slope.getNormal(nx, ny);
    slope.getTangent(tx, ty);
    std::cout << "斜面向量：" << std::endl;
    std::cout << "  法向量: (" << nx << ", " << ny << ")" << std::endl;
    std::cout << "  切向量: (" << tx << ", " << ty << ")" << std::endl;
    std::cout << std::endl;
    
    // 创建一些形状
    Circle ball1(1.0, 0.5, 0.0, 0.0);  // 质量1kg，半径0.5m
    ball1.setName("Ball1");
    
    Circle ball2(2.0, 0.8, 0.0, 0.0);  // 质量2kg，半径0.8m
    ball2.setName("Ball2");
    
    AABB box1(1.5, 1.0, 1.0, 0.0, 0.0);  // 质量1.5kg，宽1m，高1m
    box1.setName("Box1");
    
    std::cout << "========== 测试1：在斜面底部放置物体 ==========" << std::endl;
    slope.placeShapeAtBottom(ball1);
    double x1, y1;
    ball1.getCentre(x1, y1);
    std::cout << "Ball1 放置在底部：" << std::endl;
    std::cout << "  位置: (" << x1 << ", " << y1 << ")" << std::endl;
    std::cout << "  底部Y坐标: " << ball1.getBottom() << std::endl;
    std::cout << "  摩擦系数: " << ball1.fraction << std::endl;
    std::cout << std::endl;
    
    std::cout << "========== 测试2：在斜面顶部放置物体 ==========" << std::endl;
    slope.placeShapeAtTop(ball2);
    double x2, y2;
    ball2.getCentre(x2, y2);
    std::cout << "Ball2 放置在顶部：" << std::endl;
    std::cout << "  位置: (" << x2 << ", " << y2 << ")" << std::endl;
    std::cout << "  底部Y坐标: " << ball2.getBottom() << std::endl;
    std::cout << std::endl;
    
    std::cout << "========== 测试3：在斜面中间位置放置物体 ==========" << std::endl;
    slope.placeShapeAtPosition(box1, 0.5);  // 0.5表示中间位置
    double x3, y3;
    box1.getCentre(x3, y3);
    std::cout << "Box1 放置在中间（ratio=0.5）：" << std::endl;
    std::cout << "  位置: (" << x3 << ", " << y3 << ")" << std::endl;
    std::cout << "  底部Y坐标: " << box1.getBottom() << std::endl;
    std::cout << std::endl;
    
    std::cout << "========== 测试4：在指定X坐标放置物体 ==========" << std::endl;
    Circle ball3(1.0, 0.6, 0.0, 0.0);
    ball3.setName("Ball3");
    double targetX = 3.0;
    slope.placeShapeAtX(ball3, targetX);
    double x4, y4;
    ball3.getCentre(x4, y4);
    std::cout << "Ball3 放置在 X=" << targetX << "：" << std::endl;
    std::cout << "  位置: (" << x4 << ", " << y4 << ")" << std::endl;
    std::cout << "  底部Y坐标: " << ball3.getBottom() << std::endl;
    std::cout << std::endl;
    
    std::cout << "========== 测试5：检查物体是否在斜面上 ==========" << std::endl;
    std::cout << "Ball1 在斜面上? " << (slope.isSupportingShape(ball1) ? "是" : "否") << std::endl;
    std::cout << "Ball2 在斜面上? " << (slope.isSupportingShape(ball2) ? "是" : "否") << std::endl;
    std::cout << "Box1 在斜面上? " << (slope.isSupportingShape(box1) ? "是" : "否") << std::endl;
    std::cout << "Ball3 在斜面上? " << (slope.isSupportingShape(ball3) ? "是" : "否") << std::endl;
    std::cout << std::endl;
    
    std::cout << "========== 测试6：获取斜面上不同X位置的表面高度 ==========" << std::endl;
    for (double x = 0.0; x <= 10.0; x += 2.0) {
        double surfaceY = slope.getSurfaceYAtX(x);
        std::cout << "X=" << x << " 处的表面高度: Y=" << surfaceY << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "========== 测试7：使用物理世界管理斜面和物体 ==========" << std::endl;
    PhysicalWorldWithSlope world;
    world.setGravity(9.8);
    
    // 创建并添加斜面到物理世界
    Slope* worldSlope = world.placeSlope("WorldSlope", 0.0, 0.0, 15.0, angleInRadians, 0.2);
    std::cout << "在物理世界中创建斜面: " << worldSlope->getName() << std::endl;
    
    // 创建物体并放置在斜面上
    Circle* worldBall = new Circle(1.0, 0.5, 0.0, 0.0);
    worldBall->setName("WorldBall");
    worldSlope->placeShapeAtPosition(*worldBall, 0.3);  // 放置在斜面30%位置
    world.addDynamicShape(worldBall);
    
    double wx, wy;
    worldBall->getCentre(wx, wy);
    std::cout << "在斜面上放置球：" << worldBall->getName() << std::endl;
    std::cout << "  位置: (" << wx << ", " << wy << ")" << std::endl;
    std::cout << std::endl;
    
    std::cout << "========== 所有测试完成 ==========" << std::endl;
    
    // 清理
    delete worldBall;
    
    return 0;
}
