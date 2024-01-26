cmake_minimum_required (VERSION 3.21)

project(mp2tp
    VERSION 0.2.0
    DESCRIPTION "MPEG-2 TS parser library"
    LANGUAGES C CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(mp2tp STATIC)

file(GLOB SRC_LIST
	src/*.cpp)
	
target_sources(mp2tp PRIVATE ${SRC_LIST})

set_property(TARGET mp2tp PROPERTY POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    set(WS wsock32 ws2_32)
endif()

target_link_libraries(mp2tp ${WS})

target_include_directories(mp2tp
          PRIVATE src/mp2tp
		  PUBLIC include
          )
		  
include(GNUInstallDirs)

install(TARGETS mp2tp)

install (
	DIRECTORY include/
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)