# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Define the project
project(tcp_app VERSION 1.0 LANGUAGES C CXX)

# Set system type
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Define toolchain path
set(TOOLCHAIN_PATH "/usr/bin")
set(TCP_SOURCE_DIR "../1_src/tcp")

# Specify the compilers
set(CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/arm-linux-gnueabihf-g++")

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# ARM-specific compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard")

# Add definitions
add_definitions(-DTCP_CROSS_COMPILE)

# Add the executable
add_executable(tcp_app ../1_src/main.cpp ${TCP_SOURCE_DIR}/TCPClientHandler.cpp ${TCP_SOURCE_DIR}/TCPServerHandler.cpp)

# Link against static libraries
target_link_libraries(tcp_app PRIVATE
    -static-libstdc++
    -static-libgcc
    -static
    pthread
)

# Include directories
target_include_directories(tcp_app PRIVATE ${TCP_SOURCE_DIR})