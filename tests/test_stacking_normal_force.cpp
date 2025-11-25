#include "../include/physicalWorld.h"
#include "../include/shapes.h"
#include <iostream>
#include <iomanip>

using namespace std;

/*=========================================================================================================
 * 多层堆叠正压力测试
 * 测试多层物体堆叠时的正压力计算是否正确
 *=========================================================================================================*/

int main() {
    cout << "\n========================================" << endl;
    cout << "  多层堆叠正压力测试" << endl;
    cout << "========================================\n" << endl;
    
    PhysicalWorld world;
    world.setGravity(10.0);  // 使用10 m/s?便于计算
    Ground ground(0.0, 0.5);
    
    // 测试1：三层堆叠
    cout << "测试1：三层堆叠（每个10 kg）" << endl;
    cout << "  布局：" << endl;
    cout << "    ┌──────┐" << endl;
    cout << "    │ 箱子3 │ (10 kg)" << endl;
    cout << "    ├──────┤" << endl;
    cout << "    │ 箱子2 │ (10 kg)" << endl;
    cout << "    ├──────┤" << endl;
    cout << "    │ 箱子1 │ (10 kg)" << endl;
    cout << "    └──────┘" << endl;
    cout << "  ═══════════ 地面" << endl;
    cout << endl;
    
    {
        vector<Shape*> shapes;
        
        // 创建三个箱子（从下往上）
        AABB* box1 = new AABB(10.0, 2.0, 1.0, 0.0, 0.5);  // 底层，y=0.5
        AABB* box2 = new AABB(10.0, 2.0, 1.0, 0.0, 1.5);  // 中层，y=1.5
        AABB* box3 = new AABB(10.0, 2.0, 1.0, 0.0, 2.5);  // 顶层，y=2.5
        
        box1->setVelocity(0.0, 0.0);
        box2->setVelocity(0.0, 0.0);
        box3->setVelocity(0.0, 0.0);
        
        shapes.push_back(box1);
        shapes.push_back(box2);
        shapes.push_back(box3);
        
        // 运行一帧物理更新（建立支撑关系和计算正压力）
        world.update(shapes, 0.01, ground);
        
        // 检查支撑关系
        cout << "支撑关系：" << endl;
        cout << "  箱子1 被支撑: " << (box1->getIsSupported() ? "是" : "否");
        if (box1->getSupporter() == nullptr) cout << " (地面)" << endl;
        else cout << " (错误！)" << endl;
        
        cout << "  箱子2 被支撑: " << (box2->getIsSupported() ? "是" : "否");
        if (box2->getSupporter() == box1) cout << " (箱子1)" << endl;
        else cout << " (错误！)" << endl;
        
        cout << "  箱子3 被支撑: " << (box3->getIsSupported() ? "是" : "否");
        if (box3->getSupporter() == box2) cout << " (箱子2)" << endl;
        else cout << " (错误！)" << endl;
        
        // 检查正压力
        cout << "\n正压力计算：" << endl;
        
        double fx, fy;
        box3->getNormalForce(fx, fy);
        double box3_force = abs(fy);
        cout << "  箱子3 对箱子2 施加的压力: " << box3_force << " N" << endl;
        cout << "    预期: 100 N (10 kg × 10 m/s?)" << endl;
        cout << "    " << (abs(box3_force - 100.0) < 1.0 ? "? 正确" : "? 错误") << endl;
        
        box2->getNormalForce(fx, fy);
        double box2_force = abs(fy);
        cout << "\n  箱子2 对箱子1 施加的压力: " << box2_force << " N" << endl;
        cout << "    预期: 200 N (20 kg × 10 m/s?)" << endl;
        cout << "    " << (abs(box2_force - 200.0) < 1.0 ? "? 正确" : "? 错误") << endl;
        
        box1->getNormalForce(fx, fy);
        double box1_force = abs(fy);
        cout << "\n  箱子1 对地面 施加的压力: " << box1_force << " N" << endl;
        cout << "    预期: 300 N (30 kg × 10 m/s?)" << endl;
        cout << "    " << (abs(box1_force - 300.0) < 1.0 ? "? 正确" : "? 错误") << endl;
        
        // 清理
        for (auto shape : shapes) delete shape;
    }
    
    // 测试2：不同质量的堆叠
    cout << "\n\n测试2：不同质量堆叠" << endl;
    cout << "  布局：" << endl;
    cout << "    ┌──────┐" << endl;
    cout << "    │  5kg │" << endl;
    cout << "    ├──────┤" << endl;
    cout << "    │ 10kg │" << endl;
    cout << "    ├──────┤" << endl;
    cout << "    │ 15kg │" << endl;
    cout << "    └──────┘" << endl;
    cout << "  ═══════════ 地面" << endl;
    cout << endl;
    
    {
        vector<Shape*> shapes;
        
        AABB* heavy = new AABB(15.0, 2.0, 1.0, 0.0, 0.5);   // 15 kg
        AABB* medium = new AABB(10.0, 2.0, 1.0, 0.0, 1.5);  // 10 kg
        AABB* light = new AABB(5.0, 2.0, 1.0, 0.0, 2.5);    // 5 kg
        
        shapes.push_back(heavy);
        shapes.push_back(medium);
        shapes.push_back(light);
        
        world.update(shapes, 0.01, ground);
        
        double fx, fy;
        
        light->getNormalForce(fx, fy);
        double light_force = abs(fy);
        cout << "轻箱 对 中箱 施加: " << light_force << " N (预期50 N)" << endl;
        cout << "  " << (abs(light_force - 50.0) < 1.0 ? "?" : "?") << endl;
        
        medium->getNormalForce(fx, fy);
        double medium_force = abs(fy);
        cout << "中箱 对 重箱 施加: " << medium_force << " N (预期150 N)" << endl;
        cout << "  " << (abs(medium_force - 150.0) < 1.0 ? "?" : "?") << endl;
        
        heavy->getNormalForce(fx, fy);
        double heavy_force = abs(fy);
        cout << "重箱 对 地面 施加: " << heavy_force << " N (预期300 N)" << endl;
        cout << "  " << (abs(heavy_force - 300.0) < 1.0 ? "?" : "?") << endl;
        
        for (auto shape : shapes) delete shape;
    }
    
    // 测试3：摩擦力计算（使用正确的正压力）
    cout << "\n\n测试3：摩擦力计算验证" << endl;
    cout << "  三层堆叠，底层给一个初速度" << endl;
    cout << "  摩擦力应该基于总压力（30 kg）" << endl;
    cout << endl;
    
    {
        vector<Shape*> shapes;
        
        AABB* box1 = new AABB(10.0, 2.0, 1.0, 5.0, 0.5);
        AABB* box2 = new AABB(10.0, 2.0, 1.0, 5.0, 1.5);
        AABB* box3 = new AABB(10.0, 2.0, 1.0, 5.0, 2.5);
        
        box1->setVelocity(10.0, 0.0);  // 底层有初速度
        box2->setVelocity(0.0, 0.0);
        box3->setVelocity(0.0, 0.0);
        
        shapes.push_back(box1);
        shapes.push_back(box2);
        shapes.push_back(box3);
        
        // 获取初始位置
        double x_before, y;
        box1->getCentre(x_before, y);
        
        // 模拟10帧
        for (int i = 0; i < 10; i++) {
            world.update(shapes, 0.01, ground);
        }
        
        double x_after, vx, vy;
        box1->getCentre(x_after, y);
        box1->getVelocity(vx, vy);
        
        cout << "初始位置: x = " << x_before << endl;
        cout << "初始速度: vx = 10.0 m/s" << endl;
        cout << "10帧后位置: x = " << x_after << endl;
        cout << "10帧后速度: vx = " << vx << " m/s" << endl;
        cout << "\n由于摩擦力（基于300N正压力），速度应该明显减小" << endl;
        
        if (vx < 9.0) {
            cout << "? 正确：速度减小到 " << vx << " m/s" << endl;
        } else {
            cout << "? 错误：速度仍然是 " << vx << " m/s（摩擦力太小）" << endl;
        }
        
        for (auto shape : shapes) delete shape;
    }
    
    cout << "\n========================================" << endl;
    cout << "  测试完成！" << endl;
    cout << "========================================" << endl;
    
    return 0;
}
