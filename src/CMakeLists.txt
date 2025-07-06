cmake_minimum_required(VERSION 3.20)
project(poker_solver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Find LibTorch (set this to the absolute path of your libtorch directory)
set(Torch_DIR "/Volumes/Case-Sensitive/Packages/libtorch/share/cmake/Torch")
find_package(Torch REQUIRED)

add_executable(poker_solver
  main.cpp
  # table.cpp
  # util.cpp
)

add_executable(test
  test.cpp
  showdown.cpp
  util.cpp
  # table.cpp
)

target_include_directories(poker_solver PUBLIC src)
target_include_directories(test PUBLIC src)

# Link LibTorch to your executable
target_link_libraries(poker_solver "${TORCH_LIBRARIES}")
set_property(TARGET poker_solver PROPERTY CXX_STANDARD 23)

target_link_libraries(test "${TORCH_LIBRARIES}")
set_property(TARGET test PROPERTY CXX_STANDARD 23)

# Optional: avoid missing symbols when using CUDA builds
set_property(TARGET poker_solver PROPERTY INTERFACE_LINK_LIBRARIES "${TORCH_LIBRARIES}")
set_property(TARGET test PROPERTY INTERFACE_LINK_LIBRARIES "${TORCH_LIBRARIES}")
