cmake_minimum_required (VERSION 3.8)
project(libmp2t)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (WIN32)
    include_directories("C:/Program Files (x86)/GSL/include")
endif()

aux_source_directory(. SRC_LIST)
add_library(libmp2t STATIC ${SRC_LIST})

if(WIN32)
    target_link_libraries(libmp2t wsock32 ws2_32)
endif()

target_include_directories(libmp2t
          PUBLIC 
          "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
          "$<INSTALL_INTERFACE:include>"
          )

file(GLOB LIB_INCLUDE_FILES "./*.h")

# install the target and create export-set
install(TARGETS libmp2t 
    EXPORT libmp2tTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# install header file
install(FILES ${LIB_INCLUDE_FILES} DESTINATION include)

# generate and install export file
install(EXPORT libmp2tTargets
        FILE libmp2tTargets.cmake
        NAMESPACE lcss::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/lib/cmake/libmp2t
)