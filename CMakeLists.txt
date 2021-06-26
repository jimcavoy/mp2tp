# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
project ("mp2t")

# Include sub-projects.
add_subdirectory ("mp2tp")
add_subdirectory ("libmp2t")

enable_testing()

#message("project dir: ${PROJECT_SOURCE_DIR}")
add_test(NAME Usage COMMAND mp2tp)
set_tests_properties(Usage
	PROPERTIES PASS_REGULAR_EXPRESSION "Error: Fail to open file "
	)

set(args -i${PROJECT_SOURCE_DIR}/sample/foreman_cif_klv.ts -n100 -olabels.txt)
add_test(NAME Read 
	COMMAND mp2tp ${args} )
set_tests_properties(Read
	PROPERTIES PASS_REGULAR_EXPRESSION "OK"
	)