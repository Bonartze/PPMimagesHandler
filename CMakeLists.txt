cmake_minimum_required(VERSION 3.26)
project(untitled74 CXX)

set(CMAKE_CXX_STANDARD 17)
set(HELLO_WORLD "Hello World")

add_executable(untitled74 main.cpp
        ppm_image.cpp
        img_lib.cpp ppm_image.h img_lib.h)
