pushd %~dp0
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64
cl /EHsc /utf-8 /std:c++17 /I include /I include\easyx src\*.cpp .lib\EasyXa.lib user32.lib gdi32.lib winmm.lib shell32.lib /Fe:out\PhysicsEngine.exe