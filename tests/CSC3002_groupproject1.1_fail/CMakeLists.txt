cmake_minimum_required(VERSION 3.10)

# 项目名称
project(PhysicsEngine)

# 设置 C++ 标准 (C++17 是比较好的选择)
set(CMAKE_CXX_STANDARD 17)

# 设置 Unicode 字符集 (EasyX 需要)
add_compile_definitions(UNICODE _UNICODE)

# 1. 头文件路径
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/easyx)

# 2. 库文件路径 (告诉 CMake 去哪找 EasyXw.lib)
link_directories(${CMAKE_SOURCE_DIR}/lib)

# 3. 自动查找 src 下所有的 .cpp 文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 4. 创建可执行文件
add_executable(PhysicsEngine ${SOURCES})

# 5. 链接库文件
# EasyXw.lib: EasyX 图形库
# winmm.lib: Windows 多媒体库 (你的 music.cpp 用到了 mciSendString，必须链接这个！)
# user32.lib / gdi32.lib: EasyX 依赖的 Windows 基础库
target_link_libraries(PhysicsEngine 
    PRIVATE 
    EasyXw.lib 
    winmm.lib
)

# 6. 配置工作目录 (可选)
# 让 VS 调试时，工作目录默认为项目根目录，这样才能读取到 bgm.mp3
set_property(TARGET PhysicsEngine PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")