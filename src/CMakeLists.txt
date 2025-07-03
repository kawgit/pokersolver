cmake_minimum_required(VERSION 3.20)
project(MyProject LANGUAGES CXX)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fast optimization for Release builds
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Default to Release if no build type is set
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Gather all .cpp and .h files in src/ and include main.cpp
file(GLOB_RECURSE SRC_FILES
    "main.cpp"
    "src/*.cpp"
    "src/*.h"
)

# Create executable
add_executable(MyExecutable ${SRC_FILES})
