#include "../include/shapes.h"
#include <iostream>
#include <iomanip>

void print_state(const std::string& name, const Shape& shape, double time) {
    double x, y, vx, vy, fx, fy;
    shape.getCentre(x, y);
    shape.getVelocity(vx, vy);
    shape.getTotalForce(fx, fy);
    
    std::cout << std::fixed << std::setprecision(3);
    std::cout << name << " @ t=" << time 
              << ": pos(" << x << ", " << y << ") "
              << "vel(" << vx << ", " << vy << ") "
              << "force(" << fx << ", " << fy << ")" << std::endl;
}

int main() {
    std::cout << "=== 相对速度摩擦力测试 ===" << std::endl;
    
    // 测试1: 两个物体以相同速度运动（无相对速度）
    std::cout << "\n=== 测试1: 无相对速度（两物体速度相同） ===" << std::endl;
    Circle obj1(1.0, 1.0, 0.0, 0.0);
    obj1.setVelocity(5.0, 0.0);  // 物体1速度 5 m/s
    
    double surfaceVx = 5.0;  // 表面速度也是 5 m/s
    double surfaceVy = 0.0;
    
    std::cout << "物体速度: (5.0, 0.0) m/s" << std::endl;
    std::cout << "表面速度: (5.0, 0.0) m/s" << std::endl;
    std::cout << "相对速度: (0.0, 0.0) m/s - 应该无摩擦力" << std::endl;
    
    obj1.clearTotalForce();
    obj1.applyFrictionRelative(9.8, 0.3, surfaceVx, surfaceVy);
    print_state("物体", obj1, 0.0);
    
    double fx, fy;
    obj1.getTotalForce(fx, fy);
    std::cout << "摩擦力应该为零: " << (std::abs(fx) < 1e-6 && std::abs(fy) < 1e-6 ? "? 通过" : "? 失败") << std::endl;
    
    // 测试2: 物体相对于运动表面滑动
    std::cout << "\n=== 测试2: 物体在运动表面上滑动 ===" << std::endl;
    Circle obj2(1.0, 1.0, 0.0, 0.0);
    obj2.setVelocity(10.0, 0.0);  // 物体速度 10 m/s
    
    surfaceVx = 3.0;  // 表面向右移动 3 m/s
    surfaceVy = 0.0;
    
    std::cout << "物体速度: (10.0, 0.0) m/s" << std::endl;
    std::cout << "表面速度: (3.0, 0.0) m/s" << std::endl;
    std::cout << "相对速度: (7.0, 0.0) m/s" << std::endl;
    
    obj2.clearTotalForce();
    obj2.applyFrictionRelative(9.8, 0.3, surfaceVx, surfaceVy);
    print_state("物体", obj2, 0.0);
    
    obj2.getTotalForce(fx, fy);
    std::cout << "摩擦力应该向左（负X）: " << (fx < 0 ? "? 通过" : "? 失败") << std::endl;
    std::cout << "摩擦力大小: " << std::abs(fx) << " N (理论值: " << 0.3 * 9.8 << " N)" << std::endl;
    
    // 测试3: 传送带效果（物体静止在运动传送带上）
    std::cout << "\n=== 测试3: 静止物体在传送带上 ===" << std::endl;
    Circle obj3(2.0, 1.0, 0.0, 0.0);
    obj3.setVelocity(0.0, 0.0);   // 物体静止
    
    surfaceVx = 5.0;  // 传送带向右移动 5 m/s
    surfaceVy = 0.0;
    
    std::cout << "物体速度: (0.0, 0.0) m/s (静止)" << std::endl;
    std::cout << "传送带速度: (5.0, 0.0) m/s" << std::endl;
    std::cout << "相对速度: (-5.0, 0.0) m/s (物体相对传送带向左)" << std::endl;
    
    obj3.clearTotalForce();
    obj3.applyFrictionRelative(obj3.getMass() * 9.8, 0.4, surfaceVx, surfaceVy);
    print_state("物体", obj3, 0.0);
    
    obj3.getTotalForce(fx, fy);
    std::cout << "摩擦力应该向右（正X）以减小相对速度: " << (fx > 0 ? "? 通过" : "? 失败") << std::endl;
    
    // 测试4: 模拟传送带上的物体逐渐加速
    std::cout << "\n=== 测试4: 传送带加速物体 ===" << std::endl;
    Circle obj4(1.0, 1.0, 0.0, 0.0);
    obj4.setVelocity(0.0, 0.0);
    
    double beltSpeed = 10.0;  // 传送带速度 10 m/s
    double deltaTime = 0.1;
    double mu = 0.5;
    
    std::cout << "传送带速度: " << beltSpeed << " m/s" << std::endl;
    std::cout << "摩擦系数: " << mu << std::endl;
    std::cout << "\n时间\t物体速度\t相对速度\t摩擦力" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    
    for (int i = 0; i <= 25; ++i) {
        double vx, vy;
        obj4.getVelocity(vx, vy);
        double relVx = vx - beltSpeed;
        
        obj4.getTotalForce(fx, fy);
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << i * deltaTime << "\t" 
                  << vx << "\t\t" 
                  << relVx << "\t\t"
                  << fx << std::endl;
        
        // 物理更新
        obj4.clearTotalForce();
        obj4.applyFrictionRelative(obj4.getMass() * 9.8, mu, beltSpeed, 0.0);
        obj4.applyTotalForce(deltaTime);
        obj4.update(deltaTime);
        
        // 当物体速度接近传送带速度时停止
        if (std::abs(vx - beltSpeed) < 0.1) {
            std::cout << "\n物体已接近传送带速度！" << std::endl;
            break;
        }
    }
    
    // 测试5: 对比绝对速度摩擦 vs 相对速度摩擦
    std::cout << "\n=== 测试5: 绝对摩擦 vs 相对摩擦对比 ===" << std::endl;
    
    Circle obj5a(1.0, 1.0, 0.0, 0.0);
    Circle obj5b(1.0, 1.0, 0.0, 0.0);
    
    obj5a.setVelocity(10.0, 0.0);
    obj5b.setVelocity(10.0, 0.0);
    
    double groundSpeed = 4.0;  // 地面移动速度
    
    std::cout << "两个相同物体，初始速度都是 10 m/s" << std::endl;
    std::cout << "地面速度: " << groundSpeed << " m/s" << std::endl;
    std::cout << "\n时间\t绝对摩擦\t相对摩擦" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    
    for (int i = 0; i <= 15; ++i) {
        double v5a, vy_dummy, v5b;
        obj5a.getVelocity(v5a, vy_dummy);
        obj5b.getVelocity(v5b, vy_dummy);
        
        std::cout << std::fixed << std::setprecision(3);
        std::cout << i * deltaTime << "\t" 
                  << v5a << "\t\t" 
                  << v5b << std::endl;
        
        // 物体A：使用绝对速度摩擦（相对于静止地面）
        obj5a.clearTotalForce();
        obj5a.applyFriction(obj5a.getMass() * 9.8, 0.3);
        obj5a.applyTotalForce(deltaTime);
        obj5a.update(deltaTime);
        
        // 物体B：使用相对速度摩擦（相对于运动地面）
        obj5b.clearTotalForce();
        obj5b.applyFrictionRelative(obj5b.getMass() * 9.8, 0.3, groundSpeed, 0.0);
        obj5b.applyTotalForce(deltaTime);
        obj5b.update(deltaTime);
        
        // 检查物体B是否减速到地面速度
        if (std::abs(v5b - groundSpeed) < 0.1) {
            std::cout << "\n物体B (相对摩擦) 已减速到地面速度 " << groundSpeed << " m/s" << std::endl;
            std::cout << "物体A (绝对摩擦) 继续减速到: " << v5a << " m/s" << std::endl;
            break;
        }
    }
    
    // 测试6: 逆向传送带
    std::cout << "\n=== 测试6: 逆向传送带（物体快于传送带） ===" << std::endl;
    Circle obj6(1.0, 1.0, 0.0, 0.0);
    obj6.setVelocity(15.0, 0.0);  // 物体速度快于传送带
    
    double beltSpeedSlow = 5.0;
    
    std::cout << "物体初始速度: 15 m/s" << std::endl;
    std::cout << "传送带速度: " << beltSpeedSlow << " m/s" << std::endl;
    std::cout << "\n时间\t物体速度\t相对速度" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    
    for (int i = 0; i <= 20; ++i) {
        double vx, vy;
        obj6.getVelocity(vx, vy);
        double relV = vx - beltSpeedSlow;
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << i * deltaTime << "\t" << vx << "\t\t" << relV << std::endl;
        
        obj6.clearTotalForce();
        obj6.applyFrictionRelative(obj6.getMass() * 9.8, 0.4, beltSpeedSlow, 0.0);
        obj6.applyTotalForce(deltaTime);
        obj6.update(deltaTime);
        
        if (std::abs(vx - beltSpeedSlow) < 0.2) {
            std::cout << "\n物体速度已接近传送带速度！" << std::endl;
            break;
        }
    }
    
    std::cout << "\n=== 测试完成 ===" << std::endl;
    return 0;
}
