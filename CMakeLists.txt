cmake_minimum_required(VERSION 3.0)

# Set the project name.
project(big_brain_compiler VERSION 1.0)

# Specify the C++ standard.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include project headers.
include_directories("${CMAKE_SOURCE_DIR}/include")

# Build the source directory.
add_subdirectory("${CMAKE_SOURCE_DIR}/src")