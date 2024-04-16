@echo off

REM Creating a build directory and entering it
if not exist build mkdir build
cd build

REM Running CMake and building the project
cmake .. -G "MinGW Makefiles"
cmake --build .

REM Running the executable
HelloWorld.exe

REM Pause the command line to view the output
pause
