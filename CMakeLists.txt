cmake_minimum_required(VERSION 3.22)
project(powerplanner)

set(CMAKE_CXX_STANDARD 14)

add_executable(powerplanner main.cpp getPowerPlanNames.cpp getPowerPlanNames.h)
