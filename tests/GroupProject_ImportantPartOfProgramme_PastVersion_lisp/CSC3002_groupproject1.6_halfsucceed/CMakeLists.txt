cmake_minimum_required(VERSION 3.10)

project(PhysicsEngine)

set(CMAKE_CXX_STANDARD 17)

# 1. 【关键修复】移除 UNICODE 定义，解决 Renderer 报错
# add_compile_definitions(UNICODE _UNICODE) <--- 删掉或注释掉这行

# 2. 【关键修复】强制使用 UTF-8 编码，解决中文乱码和换行符报错
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>" "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/easyx)

link_directories(${CMAKE_SOURCE_DIR}/lib)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(PhysicsEngine ${SOURCES})

# 3. 链接库 (EasyXw.lib 是宽字符版，EasyXa.lib 是多字节版)
# 因为我们取消了 UNICODE，为了兼容性，建议使用 EasyXa.lib (如果有的话)。
# 但通常 EasyXw.lib 在非 Unicode 模式下也能通过转换工作。
# 如果链接报错，请尝试把 EasyXw.lib 改成 EasyXa.lib (需要你有这个文件)
target_link_libraries(PhysicsEngine 
    PRIVATE 
    EasyXa.lib 
    winmm.lib
)

set_property(TARGET PhysicsEngine PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")