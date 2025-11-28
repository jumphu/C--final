cmake_minimum_required(VERSION 3.20)
project(ProjectC)

set(CMAKE_CXX_STANDARD 17)

# 你的 include 目录
include_directories("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/VS/include")

# 你的 lib 目录（必须指向 easyxa.lib 所在目录）
link_directories("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/VS/lib/x64")

# 源代码
add_executable(ProjectC
    src/main.cpp
 "src/shapes.h")

# 链接你真正拥有的库
target_link_libraries(ProjectC
    EasyXa.lib        # 你确认存在
)
