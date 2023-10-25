cmake_minimum_required(VERSION 3.27.7)

project(server)

set(SOURCES src/server.cpp src/main.cpp)

add_executable(server ${SOURCES})

if(WIN32)
    if(NOT TARGET spdlog)
        set(spdlog_DIR "W:/repos/hnet/vendor/spdlog/build")
        find_package(spdlog REQUIRED)
    endif()
  target_link_libraries(server wsock32 ws2_32)
  target_link_libraries(server spdlog::spdlog_header_only)
endif()

target_include_directories(server PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(server core)
