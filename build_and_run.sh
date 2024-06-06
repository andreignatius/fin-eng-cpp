#!/bin/bash

# Create output folder
mkdir -p output

# Creating a build directory and entering it
mkdir -p build
cd build

# Running CMake and make
cmake ..
make

# Go to exe folder
cd bin

# Running the executable
./main
