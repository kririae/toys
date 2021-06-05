cmake_minimum_required(VERSION 3.20)
project(crepl C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-ldl -O2")

add_executable(crepl main.c stat.h stat.c)