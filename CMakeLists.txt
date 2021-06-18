# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("mp2t")

# Include sub-projects.
add_subdirectory ("mp2tp")
add_subdirectory ("libmp2t")

enable_testing()

add_test(NAME Usage COMMAND mp2tp)
set_tests_properties(Usage
	PROPERTIES PASS_REGULAR_EXPRESSION "usage: mp2tp <MPEG_transport_stream_file>"
	)
