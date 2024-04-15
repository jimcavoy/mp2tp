# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.21)

project (mp2tp
  VERSION 1.0.1
  DESCRIPTION "MPEG-2 TS Parser library project."
  LANGUAGES CXX
)

# Include sub-projects.
add_subdirectory (mp2tp)

add_executable(mp2tpser)

target_sources(mp2tpser
  PRIVATE
    src/main.cpp
    src/TsDecoder.cpp
    src/TsDecoder.h
    src/TsWriter.cpp
    src/TsWriter.h
)

target_link_libraries(mp2tpser
  PRIVATE
    mp2tp
)

option(ENABLE_DOCUMENTATION "Enable documentation" OFF)

if (ENABLE_DOCUMENTATION)
############################################################################
# Doxygen configuration
############################################################################
# Locate Doxygen in the environment. This will set DOXYGEN_FOUND variable 
# as a result. When Doxygen is found, DOXYGEN_VERSION variable will be 
# also set to found Doxygen version.
find_package(Doxygen REQUIRED)

# Override the default doxygen output directory, which is CMAKE_CURRENT_BINARY_DIR. 
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs")
# Enable HTML documentation generation
set(DOXYGEN_GENERATE_HTML YES)
# Enable MAN page generation
set(DOXYGEN_GENERATE_MAN NO)
# Enable markdown support
set(DOXYGEN_MARKDOWN_SUPPORT YES)
# Enable autolink support
set(DOXYGEN_AUTOLINK_SUPPORT YES)
# Enable dot graph support
set(DOXYGEN_HAVE_DOT YES)
# Include collaboration diagrams in class documentation
set(DOXYGEN_COLLABORATION_GRAPH NO)
# Include class diagrams in class documentation
set(DOXYGEN_CLASS_GRAPH YES)
# Generate UML-like graphs
set(DOXYGEN_UML_LOOK YES)
# Include type and parameter information in UML graphs
set(DOXYGEN_DOT_UML_DETAILS YES)
# Max per-line length for graph content
set(DOXYGEN_DOT_WRAP_THRESHOLD 100)
# Enables drawing call graphs for the functions
set(DOXYGEN_CALL_GRAPH YES)
# Suppress the output messages
set(DOXYGEN_QUIET YES)
############################################################################

doxygen_add_docs(
  mp2tp_docs
  Index.md mp2tp/ src/
  ALL
  COMMENT "Generating documentation for mp2tp library"
)
endif()


# Test cases
enable_testing()

add_subdirectory(tests)

add_test(NAME mp2tpser_read
  COMMAND mp2tpser -i${PROJECT_SOURCE_DIR}/sample/foreman_cif_klv.ts -n100 -olabels.txt )
set_tests_properties(mp2tpser_read
  PROPERTIES PASS_REGULAR_EXPRESSION "TS Packets Read: 147"
)

add_test(NAME mp2tpser_nofile
  COMMAND mp2tpser -iC:/users/joe/samples/foo.ts)
set_tests_properties(mp2tpser_nofile
  PROPERTIES PASS_REGULAR_EXPRESSION "Error: Fail to open input file, foo.ts"
)

add_test(NAME mp2tpser_readall
  COMMAND mp2tpser -i${PROJECT_SOURCE_DIR}/sample/foreman_cif_klv.ts -n0 -olabels.txt )
set_tests_properties(mp2tpser_readall
  PROPERTIES PASS_REGULAR_EXPRESSION "TS Packets Read: 4234"
)