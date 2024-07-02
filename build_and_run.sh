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

# Navigate back to the project root directory to install Python requirements
cd ../..
python -m pip install -r requirements.txt

# Navigate to the output directory and run Python script with the latest file
cd output
LATEST_FILE=$(ls -t | head -n1)
python ../QF633project_part_2_question_4_v5.py "$LATEST_FILE"