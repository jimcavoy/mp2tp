cmake_minimum_required (VERSION 3.21)

project(mp2tp
    VERSION 0.2.0
    DESCRIPTION "MPEG-2 TS parser library"
    LANGUAGES C CXX)

include(GNUInstallDirs)

# This is the default install directory for config-file package cmake files
# It is relative to install prefix.
set(mp2tp_INSTALL_CMAKEDIR cmake CACHE PATH "Installation directory for config-file package cmake files")

add_library(mp2tp STATIC)

file(GLOB SRC_LIST
	src/*.cpp)
	
target_sources(mp2tp PRIVATE ${SRC_LIST})

set_property(TARGET mp2tp PROPERTY POSITION_INDEPENDENT_CODE ON)

# specify the C++ standard
target_compile_features(
    mp2tp
    PUBLIC cxx_std_14
)

if(WIN32)
    set(WS wsock32 ws2_32)
endif()

target_link_libraries(mp2tp ${WS})

target_include_directories(mp2tp
          PRIVATE src/mp2tp
		  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          )

install(TARGETS mp2tp
        EXPORT mp2tp_export
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install (
	DIRECTORY ${PROJECT_SOURCE_DIR}/include/
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install (EXPORT mp2tp_export
        FILE mp2tp-config.cmake
        NAMESPACE lcss::
        DESTINATION ${mp2tp_INSTALL_CMAKEDIR}
)

# Defines write_basic_package_version_file
include(CMakePackageConfigHelpers)

# Create a package version file for the package. This will allow consumers to specify
# version argument to select between several versions of the package via specifying VERSION argument
# to find_package().
# We're not explicitly specifying the VERSION parameter to allow `write_basic_package_version_file` to 
# get it from project(...) version (which is 0.2.0)
write_basic_package_version_file(
  "mp2tp-config-version.cmake"
  # Package compatibility strategy. SameMajorVersion is essentially `semantic versioning`.
  COMPATIBILITY SameMajorVersion
)

# Install command for deploying Config-file package files into the target system. It must be present
# in the same directory as `mp2tp_lib-config.cmake` file.
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/mp2tp-config-version.cmake"
  DESTINATION "${mp2tp_INSTALL_CMAKEDIR}"
)
