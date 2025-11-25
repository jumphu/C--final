# 摩擦力滑行测试 - 编译和运行脚本

Write-Host "========================================"  -ForegroundColor Cyan
Write-Host "  编译摩擦力滑行测试" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 编译命令
$compileCommand = "g++ -o tests\test_friction_sliding.exe tests\test_friction_sliding.cpp src\physicalWorld.cpp src\shapes.cpp -Iinclude -std=c++11"

Write-Host "执行编译命令..." -ForegroundColor Yellow
Write-Host $compileCommand -ForegroundColor Gray
Write-Host ""

# 执行编译
try {
    Invoke-Expression $compileCommand
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "========================================" -ForegroundColor Green
        Write-Host "  编译成功！" -ForegroundColor Green
        Write-Host "========================================" -ForegroundColor Green
        Write-Host ""
        
        Write-Host "开始运行测试..." -ForegroundColor Yellow
        Write-Host ""
        
        # 运行测试
        .\tests\test_friction_sliding.exe
        
        Write-Host ""
        Write-Host "========================================" -ForegroundColor Green
        Write-Host "  测试执行完成" -ForegroundColor Green
        Write-Host "========================================" -ForegroundColor Green
        
    } else {
        Write-Host "========================================" -ForegroundColor Red
        Write-Host "  编译失败！" -ForegroundColor Red
        Write-Host "========================================" -ForegroundColor Red
    }
    
} catch {
    Write-Host "发生错误: $_" -ForegroundColor Red
}

Write-Host ""
Write-Host "按任意键退出..." -ForegroundColor Gray
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
