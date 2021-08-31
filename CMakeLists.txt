# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
project ("mp2t")

# Include sub-projects.
add_subdirectory ("mp2tp")
add_subdirectory ("libmp2t")

# Test cases
enable_testing()

set(args -i${PROJECT_SOURCE_DIR}/sample/foreman_cif_klv.ts -n100 -olabels.txt)
add_test(NAME Read 
	COMMAND mp2tp ${args} )
set_tests_properties(Read
	PROPERTIES PASS_REGULAR_EXPRESSION "TS Packets Read: 147"
	)

add_test(NAME NoFile 
	COMMAND mp2tp -iC:/users/joe/samples/foo.ts)
set_tests_properties(NoFile
	PROPERTIES PASS_REGULAR_EXPRESSION "Error: Fail to open input file, foo.ts"
	)

set(argsAll -i${PROJECT_SOURCE_DIR}/sample/foreman_cif_klv.ts -n0 -olabels.txt)
add_test(NAME ReadAll 
	COMMAND mp2tp ${argsAll} )
set_tests_properties(ReadAll
	PROPERTIES PASS_REGULAR_EXPRESSION "TS Packets Read: 4234"
	)