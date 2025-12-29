cmake_minimum_required(VERSION 3.10)
project(TCPServer VERSION 1.0.0)

# C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 可执行文件
add_executable(${PROJECT_NAME}
    app/main.cpp
    src/server/server.cpp
    src/server/sender.cpp
    src/server/receiver.cpp
    src/helper/segment.cpp
    src/helper/segment_header.cpp
    
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

