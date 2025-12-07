cmake_minimum_required(VERSION 3.20)
project(ProjectC)

set(CMAKE_CXX_STANDARD 17)

# Include Directories
include_directories("include")
include_directories("include/easyx") 

# Link Directories (Must point to where EasyXa.lib is)
link_directories("lib")

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