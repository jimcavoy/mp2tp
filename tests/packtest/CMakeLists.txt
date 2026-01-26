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
#add_test(NAME mp2tdmux_test
#  COMMAND mp2tdmux ${PROJECT_SOURCE_DIR}/../../sample/foreman_cif_klv.ts
#)

#set_tests_properties( mp2tdmux_test
#  PROPERTIES PASS_REGULAR_EXPRESSION 
#    "TS Packets Read: 4234"
#)