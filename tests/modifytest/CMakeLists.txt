cmake_minimum_required (VERSION 3.21)

project(modifytest
  VERSION 1.0.1
  DESCRIPTION "MPEG-2 TS set and get test"
  LANGUAGES CXX
)

add_executable(modifytest)

target_sources(modifytest 
  PRIVATE 
    src/main.cpp)

target_link_libraries(modifytest PRIVATE mp2tp)

# Test cases
add_test(NAME modifytest
  COMMAND modifytest ${PROJECT_SOURCE_DIR}/../../sample/oneklvset.ts
)

set_tests_properties( modifytest
  PROPERTIES PASS_REGULAR_EXPRESSION  
    "Add one second to PTS[\r\n\t ]*PTS 82108.6 sec \\(7389776209\\) \\(0x2d 0xe1 0xdd 0x12 0xa3\\) => 82109.6 sec \\(7389866209\\) \\(0x2d 0xe1 0xe1 0xd1 0xc3\\)[\r\n\t ]*Reset PID[\r\n\t ]*PID 482 => 257[\r\n\t ]*PID 482 => 257[\r\n\t ]*"
)


