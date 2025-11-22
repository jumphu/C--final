cmake_minimum_required(VERSION 3.16)

project(ProjectC VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# 包含目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 创建shapes库
add_library(shapes src/shapes.cpp)
target_include_directories(shapes PUBLIC ${PROJECT_SOURCE_DIR}/include)

# 主程序
add_executable(shape_demo src/main.cpp)
target_link_libraries(shape_demo shapes)

# 简单EasyX演示
add_executable(simple_easyx_demo examples/simple_easyx_demo.cpp)
target_link_libraries(simple_easyx_demo shapes)

# 如果在Windows上，尝试链接EasyX库
if(WIN32)
    target_link_libraries(simple_easyx_demo graphics)
endif()