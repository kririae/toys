cmake_minimum_required(VERSION 3.21)
project(pbf3d)
set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_subdirectory("${PROJECT_SOURCE_DIR}/src")
add_subdirectory("${PROJECT_SOURCE_DIR}/samples")
add_subdirectory("${PROJECT_SOURCE_DIR}/tests")
