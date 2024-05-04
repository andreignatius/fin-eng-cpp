cmake_minimum_required(VERSION 3.10)  # Minimum required version of CMake

project(HelloWorld)  # Name of your project

# Set C++11 as the standard for the project
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Turn on this to use only C++11 standard without compiler extensions

add_executable(HelloWorld main.cpp)  # Add executable