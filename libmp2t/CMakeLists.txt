cmake_minimum_required (VERSION 3.8)

if (WIN32)
    include_directories("C:/Program Files (x86)/GSL/include")
endif()

aux_source_directory(. SRC_LIST)
add_library(libmp2t ${SRC_LIST})

if(WIN32)
    target_link_libraries(libmp2t wsock32 ws2_32)
endif()

target_include_directories(libmp2t
          PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
          )