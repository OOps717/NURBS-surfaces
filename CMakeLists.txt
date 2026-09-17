cmake_minimum_required(VERSION 3.16)
project(nurbs_surface LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(nurbs_surface main.cpp)
