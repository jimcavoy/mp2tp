# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.21)

project (mp2tp
  VERSION 1.0.0
  DESCRIPTION "MPEG-2 TS Parser library"
  LANGUAGES CXX
)

# Include sub-projects.
add_subdirectory (mp2tp)

add_executable(mp2tpser)

target_sources(mp2tpser
  PRIVATE
    src/main.cpp
    src/TsDecoder.cpp
    src/TsDecoder.h
    src/TsWriter.cpp
    src/TsWriter.h
)

target_link_libraries(mp2tpser
  PRIVATE
    mp2tp
)

# Test cases
enable_testing()

add_subdirectory(tests)

add_test(NAME mp2tpser_read
  COMMAND mp2tpser -i${PROJECT_SOURCE_DIR}/sample/foreman_cif_klv.ts -n100 -olabels.txt )
set_tests_properties(mp2tpser_read
  PROPERTIES PASS_REGULAR_EXPRESSION "TS Packets Read: 147"
)

add_test(NAME mp2tpser_nofile
  COMMAND mp2tpser -iC:/users/joe/samples/foo.ts)
set_tests_properties(mp2tpser_nofile
  PROPERTIES PASS_REGULAR_EXPRESSION "Error: Fail to open input file, foo.ts"
)

add_test(NAME mp2tpser_readall
  COMMAND mp2tpser -i${PROJECT_SOURCE_DIR}/sample/foreman_cif_klv.ts -n0 -olabels.txt )
set_tests_properties(mp2tpser_readall
  PROPERTIES PASS_REGULAR_EXPRESSION "TS Packets Read: 4234"
)