# mp2tp

__mp2tp__ project provides a library to demultiplex and multiplex an
MPEG-2 Transport Stream [ISO/IEC 13818-1](https://www.iso.org/standard/74427.html#:~:text=ISO%2FIEC%2013818-1%3A2018%20specifies%20the%20system%20layer%20of%20the,the%20synchronization%20of%20multiple%20compressed%20streams%20on%20decoding%3B).

The project can be build and run on both Windows and Linux systems.

## Project Structure
The __mp2tp__ project consists of a static library and an example application.

### Static Library (mp2tp)
The static library __mp2tp__ provides classes to demultiplex and multiplex a MPEG-2 TS container.  

### Example Application (mp2tpser)
The example application, __mp2tpser__, is a MPEG-2 TS serializer that will read a MPEG-2 TS file from the 
command-line, and print out TS packets contained in the file to console or a file then exit.

The output format from __mp2tpser__ loosely follows what is described in the 
[standard](https://www.iso.org/standard/74427.html#:~:text=ISO%2FIEC%2013818-1%3A2018%20specifies%20the%20system%20layer%20of%20the,the%20synchronization%20of%20multiple%20compressed%20streams%20on%20decoding%3B), chapter 2.3, "Method of describing bit stream syntax".  It was a
deliberate decision because we want to avoid external dependency on third party libraries.

## How to Build

### Prerequisites
To build the project, it requires CMake, https://cmake.org/, to be installed on your system.

### To Build and Install
In __mp2tp__ root directory, build and install this project using CMake with the following commands on a terminal:

#### 1. Generate the build environment

    cmake -S . -B ./build

#### 2. Build the library and example application

    cmake --build ./build 

#### 3. Install the library

    cmake --install ./build

Add additional CMake parameters as required depending on your host development environment. 

The `--install` command will install a CMake package so it can be imported into other CMake projects.

### Run Test
To run the project's build-in test, enter the following:

    ctest --test-dir ./build