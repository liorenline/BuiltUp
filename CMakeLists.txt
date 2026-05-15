cmake_minimum_required(VERSION 3.20)
project(BuiltUp)

set(CMAKE_CXX_STANDARD 20)

add_executable(BuiltUp
        main.cpp
        cpu.cpp
        mem.cpp
        disk.cpp
)