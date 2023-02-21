cmake_minimum_required(VERSION 3.22)
project(powerplanner)

set(CMAKE_CXX_STANDARD 14)
IF(MSVC)
    SET(OPTIONS WIN32)
ENDIF(MSVC)

add_executable(powerplanner WIN32 main.cpp getPowerPlanNames.cpp getPowerPlanNames.h resource.h)
