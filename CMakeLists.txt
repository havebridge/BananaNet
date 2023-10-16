cmake_minimum_required(VERSION 3.27.7)

project(server)
project(client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(core)

set_target_properties(core PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(server core)
target_link_libraries(client core)