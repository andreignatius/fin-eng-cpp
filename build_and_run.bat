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

REM Navigate back to the project root directory to install Python requirements
REM You may need to adjust python path depending on your environment
cd ..\..
python -m pip install -r requirements.txt

REM Navigate back to the output directory and run Python script with the latest file
cd output
FOR /F "delims=" %%i IN ('dir /b /od /a-d') DO SET LATEST_FILE=%%i
cd ..
python portfolio_optimization.py output\%LATEST_FILE%

REM Pause the command line to view the output
pause