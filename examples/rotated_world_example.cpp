/*=========================================================================================================
 * 相对坐标系示例
 * 演示如何使用 RotatedPhysicalWorld 和局部坐标系
 *=========================================================================================================*/

#include "../include/rotatedPhysicalWorld.h"
#include "../include/rotatedShapes.h"
#include <iostream>
#include <iomanip>

void printSeparator(const std::string& title) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << "========================================" << std::endl;
}

void printVector(const std::string& name, double x, double y) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  " << name << ": (" << x << ", " << y << ")" << std::endl;
}

/*=========================================================================================================
 * 示例1：坐标系转换
 *=========================================================================================================*/
void example1_coordinateTransform() {
    printSeparator("示例1：坐标系转换");
    
    // 创建一个30度倾斜的地面
    RotatedGround ground(10.0, 0.5, 5.0, 5.0, 30.0 * PI / 180.0, 0.3);
    
    RotatedPhysicalWorld world;
    world.addRotatedGround(&ground);
    
    std::cout << "地面设置：" << std::endl;
    std::cout << "  中心: (5.0, 5.0)" << std::endl;
    std::cout << "  角度: 30度" << std::endl;
    std::cout << "  长度: 10.0" << std::endl;
    
    // 测试点：地面中心
    std::cout << "\n测试点1：地面中心 (5.0, 5.0)" << std::endl;
    double localX1, localY1;
    world.worldToLocal(ground, 5.0, 5.0, localX1, localY1);
    printVector("局部坐标", localX1, localY1);
    
    // 测试点：地面左端
    double leftX, leftY;
    ground.getLeftPoint(leftX, leftY);
    std::cout << "\n测试点2：地面左端";
    printVector("世界坐标", leftX, leftY);
    double localX2, localY2;
    world.worldToLocal(ground, leftX, leftY, localX2, localY2);
    printVector("局部坐标", localX2, localY2);
    
    // 测试点：地面右端
    double rightX, rightY;
    ground.getRightPoint(rightX, rightY);
    std::cout << "\n测试点3：地面右端";
    printVector("世界坐标", rightX, rightY);
    double localX3, localY3;
    world.worldToLocal(ground, rightX, rightY, localX3, localY3);
    printVector("局部坐标", localX3, localY3);
    
    // 反向转换验证
    double worldX, worldY;
    world.localToWorld(ground, localX3, localY3, worldX, worldY);
    std::cout << "反向转换验证";
    printVector("世界坐标", worldX, worldY);
}

/*=========================================================================================================
 * 示例2：速度转换
 *=========================================================================================================*/
void example2_velocityTransform() {
    printSeparator("示例2：速度转换");
    
    RotatedGround ground(10.0, 0.5, 0.0, 0.0, 30.0 * PI / 180.0, 0.3);
    RotatedPhysicalWorld world;
    world.addRotatedGround(&ground);
    
    // 世界坐标系中的水平速度
    double worldVx = 5.0;
    double worldVy = 0.0;
    
    std::cout << "世界坐标系速度";
    printVector("(Vx, Vy)", worldVx, worldVy);
    
    // 转换到地面局部坐标系
    double localVx, localVy;
    world.worldVelocityToLocal(ground, worldVx, worldVy, localVx, localVy);
    
    std::cout << "地面局部坐标系速度";
    printVector("(沿地面, 垂直地面)", localVx, localVy);
    
    std::cout << "\n解释：" << std::endl;
    std::cout << "  沿地面速度: " << localVx << " m/s (物体沿斜坡移动)" << std::endl;
    std::cout << "  垂直地面速度: " << localVy << " m/s (物体离开/接近地面)" << std::endl;
    
    // 反向转换
    double backWorldVx, backWorldVy;
    world.localVelocityToWorld(ground, localVx, localVy, backWorldVx, backWorldVy);
    std::cout << "\n反向转换验证";
    printVector("(Vx, Vy)", backWorldVx, backWorldVy);
}

/*=========================================================================================================
 * 示例3：物体放置（使用相对坐标系）
 *=========================================================================================================*/
void example3_placeShape() {
    printSeparator("示例3：物体放置");
    
    RotatedGround ground(10.0, 0.5, 0.0, 0.0, 30.0 * PI / 180.0, 0.3);
    RotatedPhysicalWorld world;
    world.addRotatedGround(&ground);
    
    Circle ball(0.5);
    
    std::cout << "在不同位置放置球（半径0.5）：\n" << std::endl;
    
    // 放置在左端
    world.placeShapeOnGroundRatio(ball, ground, 0.0);
    double x1, y1;
    ball.getCentre(x1, y1);
    std::cout << "位置1 (左端, ratio=0.0)";
    printVector("世界坐标", x1, y1);
    
    // 放置在中间
    world.placeShapeOnGroundRatio(ball, ground, 0.5);
    double x2, y2;
    ball.getCentre(x2, y2);
    std::cout << "位置2 (中间, ratio=0.5)";
    printVector("世界坐标", x2, y2);
    
    // 放置在右端
    world.placeShapeOnGroundRatio(ball, ground, 1.0);
    double x3, y3;
    ball.getCentre(x3, y3);
    std::cout << "位置3 (右端, ratio=1.0)";
    printVector("世界坐标", x3, y3);
    
    // 使用局部坐标直接放置
    world.placeShapeOnGroundLocal(ball, ground, 2.0, 0.0);
    double x4, y4;
    ball.getCentre(x4, y4);
    std::cout << "\n位置4 (局部X=2.0)";
    printVector("世界坐标", x4, y4);
}

/*=========================================================================================================
 * 示例4：物理模拟（在斜坡上滚动）
 *=========================================================================================================*/
void example4_physics() {
    printSeparator("示例4：物理模拟");
    
    // 创建30度斜坡
    RotatedGround ground(10.0, 0.5, 0.0, 0.0, 30.0 * PI / 180.0, 0.1);
    
    RotatedPhysicalWorld world(9.8);
    world.addRotatedGround(&ground);
    
    // 创建球并放置在斜坡顶部
    Circle ball(0.5, 0.0, 0.0);
    ball.setMass(1.0);
    world.addShape(&ball);
    world.placeShapeOnGroundRatio(ball, ground, 0.9);  // 接近顶部
    
    std::cout << "初始状态：" << std::endl;
    double x0, y0;
    ball.getCentre(x0, y0);
    printVector("球位置", x0, y0);
    
    // 模拟几帧
    std::cout << "\n模拟球在斜坡上滚动（dt=0.1s）：\n" << std::endl;
    
    for (int i = 0; i <= 5; i++) {
        if (i > 0) {
            world.update(0.1);
        }
        
        double x, y, vx, vy;
        ball.getCentre(x, y);
        ball.getVelocity(vx, vy);
        
        // 转换到局部坐标系
        double localX, localY;
        world.worldToLocal(ground, x, y, localX, localY);
        
        double localVx, localVy;
        world.worldVelocityToLocal(ground, vx, vy, localVx, localVy);
        
        std::cout << "时刻 " << (i * 0.1) << "s:" << std::endl;
        printVector("  世界位置", x, y);
        printVector("  局部位置", localX, localY);
        printVector("  局部速度", localVx, localVy);
        std::cout << "  沿斜坡速度: " << localVx << " m/s" << std::endl;
        std::cout << std::endl;
    }
}

/*=========================================================================================================
 * 示例5：力的分解（局部坐标系）
 *=========================================================================================================*/
void example5_forceDecomposition() {
    printSeparator("示例5：力的分解");
    
    double angle = 30.0 * PI / 180.0;
    RotatedGround ground(10.0, 0.5, 0.0, 0.0, angle, 0.2);
    
    RotatedPhysicalWorld world(9.8);
    world.addRotatedGround(&ground);
    
    double g = 9.8;
    double m = 1.0;
    
    std::cout << "斜坡角度: 30度" << std::endl;
    std::cout << "物体质量: " << m << " kg" << std::endl;
    std::cout << "重力加速度: " << g << " m/s?\n" << std::endl;
    
    // 重力在局部坐标系中的分量
    double gravityAlong = g * std::sin(angle);      // 沿斜面向下
    double gravityPerp = g * std::cos(angle);       // 垂直斜面向下
    
    std::cout << "重力分解（局部坐标系）：" << std::endl;
    std::cout << "  沿斜面分量: " << gravityAlong << " m/s? (使物体下滑)" << std::endl;
    std::cout << "  垂直分量: " << gravityPerp << " m/s? (压向斜面)" << std::endl;
    
    // 法向力（平衡垂直分量）
    double normalForce = m * gravityPerp;
    std::cout << "\n法向力: " << normalForce << " N (向上)" << std::endl;
    
    // 摩擦力
    double frictionCoeff = 0.2;
    double frictionForce = frictionCoeff * normalForce;
    std::cout << "最大静摩擦力: " << frictionForce << " N (沿斜面向上)" << std::endl;
    
    // 净力
    double netForceAlong = m * gravityAlong - frictionForce;
    std::cout << "\n净力（沿斜面）: " << netForceAlong << " N" << std::endl;
    std::cout << "加速度: " << (netForceAlong / m) << " m/s?" << std::endl;
    
    if (netForceAlong > 0) {
        std::cout << "→ 物体将沿斜面下滑" << std::endl;
    } else {
        std::cout << "→ 物体将保持静止" << std::endl;
    }
}

/*=========================================================================================================
 * 示例6：检测物体是否在地面上
 *=========================================================================================================*/
void example6_onGroundDetection() {
    printSeparator("示例6：检测物体是否在地面上");
    
    RotatedGround ground(10.0, 0.5, 0.0, 0.0, 30.0 * PI / 180.0, 0.3);
    RotatedPhysicalWorld world;
    world.addRotatedGround(&ground);
    
    Circle ball1(0.5);
    Circle ball2(0.5);
    Circle ball3(0.5);
    
    // 球1：在地面上
    world.placeShapeOnGroundRatio(ball1, ground, 0.5);
    
    // 球2：在地面上方
    double x2, y2;
    ball1.getCentre(x2, y2);
    ball2.setCentre(x2, y2 + 2.0);
    
    // 球3：在地面外侧
    ball3.setCentre(10.0, 5.0);
    
    std::cout << "检测结果：\n" << std::endl;
    
    bool on1 = world.isOnRotatedGround(ball1, ground);
    std::cout << "球1: " << (on1 ? "在地面上 ?" : "不在地面上") << std::endl;
    
    bool on2 = world.isOnRotatedGround(ball2, ground);
    std::cout << "球2: " << (on2 ? "在地面上" : "不在地面上 ?") << std::endl;
    
    bool on3 = world.isOnRotatedGround(ball3, ground);
    std::cout << "球3: " << (on3 ? "在地面上" : "不在地面上 ?") << std::endl;
}

/*=========================================================================================================
 * 主函数
 *=========================================================================================================*/
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    相对坐标系物理世界示例" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        example1_coordinateTransform();
        example2_velocityTransform();
        example3_placeShape();
        example4_physics();
        example5_forceDecomposition();
        example6_onGroundDetection();
        
        printSeparator("所有示例完成");
        std::cout << "\n核心概念总结：" << std::endl;
        std::cout << "? 使用地面局部坐标系简化计算" << std::endl;
        std::cout << "? X轴沿地面，Y轴垂直地面" << std::endl;
        std::cout << "? 重力、法向力、摩擦力在局部坐标系中分解" << std::endl;
        std::cout << "? 世界坐标 ? 局部坐标自动转换" << std::endl;
        std::cout << "? 物理意义更清晰，代码更简洁" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n错误: " << e.what() << std::endl;
        return 1;
    }
}
