cmake_minimum_required(VERSION 3.20)
project(crepl C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-ldl -O2")

find_package(readline)

add_executable(crepl main.c stat.h stat.c)

target_link_libraries(crepl readline)