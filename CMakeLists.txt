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

# Add LLVM packages.
find_package(LLVM REQUIRED CONFIG)

# Send a message updating on status of LLVM libraries.
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include the LLVM directories in our project.
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Find the libraries that correspond to the LLVM components we want to use.
llvm_map_components_to_libnames(llvm_libs support core irreader)