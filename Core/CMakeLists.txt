add_library(core INTERFACE)
target_include_directories(core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
  target_link_libraries(core INTERFACE wsock32 ws2_32)
endif()