@echo off

REM Creating an output directory
if not exist output mkdir output

REM Creating a build directory and entering it
if not exist build mkdir build
cd build

REM Running CMake and building the project
cmake .. -G "MinGW Makefiles"
cmake --build .

REM Go to exe folder
cd bin

REM Running the executable
main.exe

REM Pause the command line to view the output
pause
