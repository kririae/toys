cmake_minimum_required(VERSION 3.21)
project(pbf3d)

file(GLOB source
  "src/*.cpp")
file(GLOB lib_source
  "libs/src/*.c*")

set(CMAKE_CXX_STANDARD 20)
include_directories(libs/include)

find_package(glfw3 3.3 REQUIRED)

add_executable(pbf3d ${source} ${lib_source})

MESSAGE(STATUS "find source: " ${source})
MESSAGE(STATUS "find lib_source: " ${lib_source})
target_link_libraries(pbf3d PUBLIC glfw ${CMAKE_DL_LIBS})