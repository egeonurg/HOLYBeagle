# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(tcp_app VERSION 1.0 LANGUAGES C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# Enable warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Add the executable (Ensure .cpp files are used)
add_executable(tcp_app 1_src/main.cpp)
target_include_directories(tcp_app PRIVATE ${CMAKE_SOURCE_DIR}/1_src/tcp)