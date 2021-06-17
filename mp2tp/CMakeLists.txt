# CMakeList.txt : CMake project for mp2tp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

if (WIN32)
    include_directories("C:/Program Files (x86)/GSL/include")
endif()

# Add source to this project's executable.
add_executable (mp2tp "mp2tp.cpp" "TsDecoder.cpp")

list(APPEND EXTRA_LIBS libmp2t)

target_link_libraries(mp2tp PUBLIC ${EXTRA_LIBS})


# TODO: Add tests and install targets if needed.
