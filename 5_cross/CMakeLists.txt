# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

project(tcp_app VERSION 1.0 LANGUAGES C CXX)

# Set system type
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Define toolchain path
set(TOOLCHAIN_PATH "/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin")

# Specify the compilers
set(CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/arm-none-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/arm-none-linux-gnueabihf-g++")

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# ARM-specific compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard")

add_definitions(-DCROSS_COMPILE_BBBI)

# Add the executable (Ensure .cpp files are used)
add_executable(tcp_app ../1_src/main.cpp ../1_src/tcp/TCPPackageHandler.cpp)
target_link_libraries(tcp_app pthread)
target_include_directories(tcp_app PRIVATE ${CMAKE_SOURCE_DIR} ../1_src/tcp)

