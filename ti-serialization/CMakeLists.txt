cmake_minimum_required(VERSION 3.12)

project(ti-serialization)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

add_executable(main "src/main.cpp" "src/serialization.h")
