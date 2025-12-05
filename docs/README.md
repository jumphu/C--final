# 物理引擎文档索引

本目录包含物理引擎项目的所有文档。

---

## ?? 文档分类

### ?? 核心文档

| 文档 | 说明 |
|------|------|
| [PHYSICS_ENGINE_README.md](PHYSICS_ENGINE_README.md) | 物理引擎完整说明文档 |
| [COMPLETE_VERIFICATION_REPORT.md](COMPLETE_VERIFICATION_REPORT.md) | 完整验证报告 |
| [VERIFICATION_SUMMARY.md](VERIFICATION_SUMMARY.md) | 验证总结 |

---

### ? placeShapeOnShape 功能文档

新添加的板块模型放置功能：

| 文档 | 说明 |
|------|------|
| [placeShapeOnShape_guide.md](placeShapeOnShape_guide.md) | 完整使用指南（详细） |
| [placeShapeOnShape_quick_ref.md](placeShapeOnShape_quick_ref.md) | 快速参考手册 |
| [placeShapeOnShape_implementation_summary.md](placeShapeOnShape_implementation_summary.md) | 实现总结 |
| [placeShapeOnShape_regression_test.md](placeShapeOnShape_regression_test.md) | 回归测试报告 |

**推荐阅读顺序**：
1. 快速参考 → 2. 使用指南 → 3. 实现总结 → 4. 测试报告

---

### ?? 功能修复文档

#### 摩擦力系统

| 文档 | 说明 |
|------|------|
| [friction_fix_summary.md](friction_fix_summary.md) | 摩擦力修复总结 |
| [friction_fix_detailed.md](friction_fix_detailed.md) | 摩擦力修复详细报告 |

#### 堆叠系统

| 文档 | 说明 |
|------|------|
| [stacking_problem_root_cause.md](stacking_problem_root_cause.md) | 堆叠问题根本原因分析 |

---

### ?? 测试报告

#### 综合测试

| 文档 | 说明 |
|------|------|
| [final_verification_report.md](final_verification_report.md) | 最终验证报告 |
| [test_diagnosis_report.md](test_diagnosis_report.md) | 测试诊断报告 |
| [test_summary_after_static_friction.md](test_summary_after_static_friction.md) | 静摩擦力后测试总结 |

#### 专项测试

| 文档 | 说明 |
|------|------|
| [projectile_motion_test_report.md](projectile_motion_test_report.md) | 抛体运动测试报告 |
| [PROJECTILE_MOTION_SUMMARY.md](PROJECTILE_MOTION_SUMMARY.md) | 抛体运动测试总结 |
| [slope_collision_test_report.md](slope_collision_test_report.md) | 斜面碰撞测试报告 |
| [SLOPE_COLLISION_SUMMARY.md](SLOPE_COLLISION_SUMMARY.md) | 斜面碰撞测试总结 |
| [SLOPE_TEST_SUMMARY.md](SLOPE_TEST_SUMMARY.md) | 斜面测试总结 |

---

## ??? 按主题查找

### 查找板块模型相关文档
```
placeShapeOnShape_*.md
```

### 查找测试相关文档
```
*test*.md
*SUMMARY.md
```

### 查找修复相关文档
```
*fix*.md
*problem*.md
```

---

## ?? 快速导航

### 新手入门
1. 阅读 [PHYSICS_ENGINE_README.md](PHYSICS_ENGINE_README.md)
2. 查看 [placeShapeOnShape_quick_ref.md](placeShapeOnShape_quick_ref.md)

### 开发者
1. 阅读 [PHYSICS_ENGINE_README.md](PHYSICS_ENGINE_README.md)
2. 查看 [placeShapeOnShape_guide.md](placeShapeOnShape_guide.md)
3. 参考 [placeShapeOnShape_implementation_summary.md](placeShapeOnShape_implementation_summary.md)

### 测试人员
1. 查看 [COMPLETE_VERIFICATION_REPORT.md](COMPLETE_VERIFICATION_REPORT.md)
2. 阅读各专项测试报告

---

## ?? 文档统计

| 类型 | 数量 |
|------|------|
| 核心文档 | 3 |
| 功能文档 | 6 |
| 测试报告 | 9 |
| **总计** | **18** |

---

## ?? 文档更新记录

| 日期 | 更新内容 |
|------|----------|
| 2024-11-28 | 添加 placeShapeOnShape 功能文档 |
| 2024-11-28 | 整理所有文档到 docs 目录 |
| 2024-11-28 | 创建文档索引 |

---

## ?? 文档命名规范

- **大写开头的全大写文件**: 总结性文档（如 `README.md`, `SUMMARY.md`）
- **小写开头的文件**: 详细技术文档（如 `friction_fix_detailed.md`）
- **下划线分隔**: 多词文件名（如 `placeShapeOnShape_guide.md`）

---

## ?? 推荐阅读路线

### 路线 1: 功能使用
```
PHYSICS_ENGINE_README.md
    ↓
placeShapeOnShape_quick_ref.md
    ↓
placeShapeOnShape_guide.md
```

### 路线 2: 开发理解
```
PHYSICS_ENGINE_README.md
    ↓
friction_fix_summary.md
    ↓
stacking_problem_root_cause.md
    ↓
placeShapeOnShape_implementation_summary.md
```

### 路线 3: 测试验证
```
COMPLETE_VERIFICATION_REPORT.md
    ↓
各专项测试报告
    ↓
placeShapeOnShape_regression_test.md
```

---

**文档目录最后更新**: 2024年11月28日  
**总文档数**: 18  
**项目版本**: v1.0
