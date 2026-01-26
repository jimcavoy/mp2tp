cmake_minimum_required (VERSION 3.21)

project(packtest
  VERSION 1.0.0
  DESCRIPTION "MPEG-2 TS Packetization test"
  LANGUAGES CXX
)

add_executable(packtest)

target_sources(packtest 
  PRIVATE 
    src/main.cpp)

target_link_libraries(packtest PRIVATE mp2tp)

# Test cases
add_test(NAME packtest_175
  COMMAND packtest 175
)

set_tests_properties( packtest_175
  PROPERTIES PASS_REGULAR_EXPRESSION 
    "1"
)

add_test(NAME packtest_543
  COMMAND packtest 543
)

set_tests_properties( packtest_543
  PROPERTIES PASS_REGULAR_EXPRESSION 
    "3"
)

add_test(NAME packtest_542
  COMMAND packtest 542
)

set_tests_properties( packtest_542
  PROPERTIES PASS_REGULAR_EXPRESSION 
    "4"
)