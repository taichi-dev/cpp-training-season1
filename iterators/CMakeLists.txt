cmake_minimum_required(VERSION 3.0)

project(iterators)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCEFILES "src/main.cpp")
add_executable(main ${SOURCEFILES})
