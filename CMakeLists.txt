cmake_minimum_required(VERSION 3.15)

# Set the project name.
project(big_brain_compiler VERSION 1.1)

# Specify the C++ standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add LLVM packages.
find_package(LLVM REQUIRED CONFIG)

# Send a message updating on status of LLVM libraries.
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include the LLVM directories in our project.
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Gather LLVM libraries to link against.
execute_process(COMMAND llvm-config --libs OUTPUT_VARIABLE libs)
execute_process(COMMAND llvm-config --system-libs OUTPUT_VARIABLE sys_libs)
execute_process(COMMAND llvm-config --cxxflags OUTPUT_VARIABLE cxx_flags)
execute_process(COMMAND llvm-config --ldflags OUTPUT_VARIABLE ld_flags)

string(STRIP ${libs} libs)
string(STRIP ${sys_libs} sys_libs)
string(STRIP ${ld_flags} ld_flags)
string(STRIP ${cxx_flags} cxx_flags)

# Include project headers.
include_directories("${CMAKE_SOURCE_DIR}/include")

# Build the source directory and test directory.
add_subdirectory("${CMAKE_SOURCE_DIR}/src")


# Include the CTest library.
include(CTest)

add_subdirectory("${CMAKE_SOURCE_DIR}/test")
