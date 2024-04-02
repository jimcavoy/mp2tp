cmake_minimum_required (VERSION 3.21)

project(mp2tp
  VERSION 1.0.0
  DESCRIPTION "MPEG-2 TS parser library"
  LANGUAGES CXX
)

include(GNUInstallDirs)

# set the postfix "d" for the resulting .so or .dll files when building the
# library in debug mode
set(CMAKE_DEBUG_POSTFIX d)

add_library(mp2tp STATIC)

target_sources(mp2tp
  PRIVATE
    include/mp2tp/AccessUnit.h
    include/mp2tp/libmp2tp.h
    include/mp2tp/tsadptfd.h
    include/mp2tp/tsmetadata.h
    include/mp2tp/tsnit.h
    include/mp2tp/tspacker.h
    include/mp2tp/tspat.h
    include/mp2tp/tspckt.h
    include/mp2tp/tspes.h
    include/mp2tp/tspmt.h
    include/mp2tp/tsprsr.h
    include/mp2tp/tstype.h
    src/AccessUnit.cpp
    src/stdafx.cpp
    src/stdafx.h
    src/targetver.h
    src/tsadptfd.cpp
    src/tsmetadata.cpp
    src/tsnit.cpp
    src/tspacker.cpp
    src/tspat.cpp
    src/tspckt.cpp
    src/tspes.cpp
    src/tspmt.cpp
    src/tsprsr.cpp
)

set_property(TARGET mp2tp PROPERTY POSITION_INDEPENDENT_CODE ON)

# specify the C++ standard
target_compile_features( mp2tp
  PUBLIC
    cxx_std_14
)

if(WIN32)
  target_link_libraries(mp2tp 
    PUBLIC
     wsock32 ws2_32
  )
endif()

target_include_directories(mp2tp
  PRIVATE
    src/mp2tp
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(TARGETS mp2tp
    EXPORT mp2tp_export
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install (
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install (EXPORT mp2tp_export
    FILE mp2tp-config.cmake
    NAMESPACE lcss::
    DESTINATION lib/cmake/mp2tp
)

if (WIN32)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/mp2tpd.pdb DESTINATION lib OPTIONAL)
endif()

# Defines write_basic_package_version_file
include(CMakePackageConfigHelpers)

# Create a package version file for the package. This will allow consumers to specify
# version argument to select between several versions of the package via specifying VERSION argument
# to find_package().
# We're not explicitly specifying the VERSION parameter to allow `write_basic_package_version_file` to 
# get it from project(...) version (which is 1.0.0)
write_basic_package_version_file(
  "mp2tp-config-version.cmake"
  # Package compatibility strategy. SameMajorVersion is essentially `semantic versioning`.
  COMPATIBILITY SameMajorVersion
)

# Install command for deploying Config-file package files into the target system. It must be present
# in the same directory as `mp2tp_lib-config.cmake` file.
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/mp2tp-config-version.cmake"
  DESTINATION lib/cmake/mp2tp
)
