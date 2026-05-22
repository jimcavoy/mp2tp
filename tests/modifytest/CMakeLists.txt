cmake_minimum_required (VERSION 3.21)

project(modifytest
  VERSION 1.0.0
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
  COMMAND modifytest
)


