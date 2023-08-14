cmake_minimum_required (VERSION 3.8)
project("libmp2t"
    VERSION 1.0 
    DESCRIPTION "MPEG-2 TS parser library"
    LANGUAGES C CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (WIN32)
    set(CMAKE_INSTALL_LIBDIR $ENV{APPDATA})
endif()

file(GLOB SRC_LIST
    *.h
    *.cpp)
add_library(libmp2t STATIC ${SRC_LIST})
set_property(TARGET libmp2t PROPERTY POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    target_link_libraries(libmp2t wsock32 ws2_32)
    add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0)
else()
    set_target_properties(libmp2t 
        PROPERTIES
        OUTPUT_NAME mp2t)
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