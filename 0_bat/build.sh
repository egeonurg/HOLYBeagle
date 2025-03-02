#!/bin/bash

cd ..

ls 

cd 5_cross

mkdir build

cd build

cmake ..

echo "CMake Build completed. Output is in $BUILD_DIR"

cd .. 

cd 5_cross

cd build

make all

echo "Make Build completed. Output is in $BUILD_DIR"