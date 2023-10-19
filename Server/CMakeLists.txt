cmake_minimum_required(VERSION 3.27.7)

project(server)




set(SOURCES src/server.cpp src/main.cpp)

add_executable(server ${SOURCES})
if(WIN32)
  target_link_libraries(server wsock32 ws2_32)
endif()


target_include_directories(server PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(server core)