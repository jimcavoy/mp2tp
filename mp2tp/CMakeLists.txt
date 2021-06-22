# CMakeList.txt : CMake project for mp2tp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(mp2tp)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source to this project's executable.
add_executable (mp2tp "mp2tp.cpp" "TsDecoder.h" "TsDecoder.cpp" "TsWriter.h" "TsWriter.cpp")

list(APPEND EXTRA_LIBS libmp2t)

target_link_libraries(mp2tp PUBLIC ${EXTRA_LIBS})

# TODO: Add tests and install targets if needed.
install(TARGETS mp2tp 
    EXPORT mp2tpTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)