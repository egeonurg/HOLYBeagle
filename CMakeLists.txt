# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(send_tcp VERSION 1.0 LANGUAGES C)

# Set C standard and enable warnings
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Add the executable
add_executable(send_tcp main.c)

# Install the executable (optional)
install(TARGETS send_tcp DESTINATION bin)