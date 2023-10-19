cmake_minimum_required(VERSION 3.27.7)

project(client)

configure_file("W:/repos/hnet/handler/file_handler.txt"
    ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

set(SOURCES src/client.cpp src/main.cpp)

add_executable(client ${SOURCES})

if(WIN32)
  target_link_libraries(client wsock32 ws2_32)
  target_link_libraries(client wininet)
endif()

target_include_directories(client PRIVATE  ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(client core)
