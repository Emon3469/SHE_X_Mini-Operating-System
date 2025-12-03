cmake_minimum_required(VERSION 3.6)
project(Simple_OS_Simulator)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        main.cpp
        Process.cpp
        sOS-Sim.cpp
        ProcessSchedulingAlgorithms.h
        FileManager.h
        PageReplacementAlgorithms.h Page.h)
add_executable(Simple_OS_Simulator ${SOURCE_FILES})