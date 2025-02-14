cmake_minimum_required (VERSION 3.21)

project(mp2tdmux
    DESCRIPTION "MPEG-2 TS Demultiplexor: creates a file for each elementary stream in the container."
    LANGUAGES CXX
)

add_executable(mp2tdmux)

target_sources(mp2tdmux 
  PRIVATE 
    src/main.cpp
	src/AccessUnit.h
	src/AccessUnit.cpp
	src/MP2TFileDemux.h
	src/MP2TFileDemux.cpp)

target_link_libraries(mp2tdmux PRIVATE mp2tp)

# Test cases
add_test(NAME mp2tdmux_test
  COMMAND mp2tdmux ${PROJECT_SOURCE_DIR}/../../sample/foreman_cif_klv.ts
)

set_tests_properties( mp2tdmux_test
  PROPERTIES PASS_REGULAR_EXPRESSION 
    "TS Packets Read: 4234"
)