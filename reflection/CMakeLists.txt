cmake_minimum_required(VERSION 3.12)

project(reflection)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE TRUE)
add_compile_options("-fno-rtti")

add_library(reflect_lib STATIC
    "src/reflect.cpp"
    "src/reflect.hpp")

add_executable(main "src/main.cpp")
target_link_libraries(main PRIVATE reflect_lib)
