cmake_minimum_required(VERSION 3.27.7)

set(CMAKE_CXX_STANDARD 20)

project(server)
project(client)

add_executable(server "UDP Server/src/main.cpp")
add_executable(client "UDP Client/src/main.cpp")

target_include_directories(server PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/UDP Server/include/")
target_include_directories(client PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/UDP Client/include/")
target_include_directories(client PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Core/")