cmake_minimum_required(VERSION 3.20)
project(ProjectC)

set(CMAKE_CXX_STANDARD 17)

# Include Directories
include_directories("include")
include_directories("include/easyx") 
include_directories("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/VS/include")

# Link Directories (Must point to where EasyXa.lib is)
link_directories("lib") 
link_directories("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/VS/lib/x64")

# Source Files
file(GLOB SOURCES 
    "src/*.cpp"
)

# Executable
add_executable(ProjectC
    ${SOURCES}
)

# Link Libraries
target_link_libraries(ProjectC
    EasyXa.lib
    winmm.lib # For music/sound if needed
)