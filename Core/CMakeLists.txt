add_library(core INTERFACE)
target_include_directories(core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    if(NOT TARGET spdlog)
        set(spdlog_DIR "W:/repos/hnet/vendor/spdlog/build")
        find_package(spdlog REQUIRED)
    endif()

  target_link_libraries(core INTERFACE wsock32 ws2_32)
  target_link_libraries(core INTERFACE spdlog::spdlog_header_only)
endif()