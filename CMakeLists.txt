# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.21)

project (mp2tp
	VERSION 0.2.0
	DESCRIPTION "MPEG-2 TS Parser library"
	LANGUAGES C CXX
)

# Include sub-projects.
add_subdirectory (mp2tp)

add_executable(mp2tptest)

file(GLOB SRC_LIST
	src/*.cpp)
	
target_sources(mp2tptest PRIVATE ${SRC_LIST})

if(WIN32)
    list(APPEND EXTRA_LIBS mp2tp wsock32 ws2_32)
else()
    list(APPEND EXTRA_LIBS mp2tp)
endif()
	
target_link_libraries(mp2tptest PRIVATE ${EXTRA_LIBS})

# Test cases
enable_testing()

set(args -i${PROJECT_SOURCE_DIR}/sample/foreman_cif_klv.ts -n100 -olabels.txt)
add_test(NAME Read 
	COMMAND mp2tptest ${args} )
set_tests_properties(Read
	PROPERTIES PASS_REGULAR_EXPRESSION "TS Packets Read: 147"
	)

add_test(NAME NoFile 
	COMMAND mp2tptest -iC:/users/joe/samples/foo.ts)
set_tests_properties(NoFile
	PROPERTIES PASS_REGULAR_EXPRESSION "Error: Fail to open input file, foo.ts"
	)

set(argsAll -i${PROJECT_SOURCE_DIR}/sample/foreman_cif_klv.ts -n0 -olabels.txt)
add_test(NAME ReadAll 
	COMMAND mp2tptest ${argsAll} )
set_tests_properties(ReadAll
	PROPERTIES PASS_REGULAR_EXPRESSION "TS Packets Read: 4234"
	)