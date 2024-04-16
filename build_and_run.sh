#!/bin/bash

# Creating a build directory and entering it
mkdir -p build
cd build

# Running CMake and make
cmake ..
make

# Running the executable
./HelloWorld
