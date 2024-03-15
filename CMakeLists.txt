# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.21)

project (mp2tp
	VERSION 1.0.0
	DESCRIPTION "MPEG-2 TS Parser library"
	LANGUAGES C CXX
)

# Include sub-projects.
add_subdirectory (mp2tp)

add_executable(mp2tpser)

target_sources(mp2tpser PRIVATE 
    src/main.cpp
    src/TsDecoder.cpp
    src/TsDecoder.h
    src/TsWriter.cpp
    src/TsWriter.h
)

if(WIN32)
    list(APPEND EXTRA_LIBS mp2tp wsock32 ws2_32)
else()
    list(APPEND EXTRA_LIBS mp2tp)
endif()

target_link_libraries(mp2tpser PRIVATE ${EXTRA_LIBS})

# Test cases
enable_testing()

set(args -i${PROJECT_SOURCE_DIR}/sample/foreman_cif_klv.ts -n100 -olabels.txt)
add_test(NAME Read 
	COMMAND mp2tpser ${args} )
set_tests_properties(Read
	PROPERTIES PASS_REGULAR_EXPRESSION "TS Packets Read: 147"
)

add_test(NAME NoFile 
	COMMAND mp2tpser -iC:/users/joe/samples/foo.ts)
set_tests_properties(NoFile
	PROPERTIES PASS_REGULAR_EXPRESSION "Error: Fail to open input file, foo.ts"
)

set(argsAll -i${PROJECT_SOURCE_DIR}/sample/foreman_cif_klv.ts -n0 -olabels.txt)
add_test(NAME ReadAll 
	COMMAND mp2tpser ${argsAll} )
set_tests_properties(ReadAll
	PROPERTIES PASS_REGULAR_EXPRESSION "TS Packets Read: 4234"
)