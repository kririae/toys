cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0054 NEW)
project(pbf3d)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pedantic -pedantic-errors -Werror -Wextra -Wshadow -Wpointer-arith -Wformat=2 -Weffc++ -Wall")

if (${IS_CLANG_COMPILER})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wloop-analysis")
endif ()

file(GLOB source
  "src/*.cpp")
file(GLOB lib_source
  "libs/src/*.c*")

include_directories(libs/include)

find_package(glfw3 3.3 REQUIRED)

add_executable(pbf3d ${source} ${lib_source} src/common.hpp)

MESSAGE(STATUS "find source: " ${source})
MESSAGE(STATUS "find lib_source: " ${lib_source})
target_link_libraries(pbf3d PUBLIC glfw ${CMAKE_DL_LIBS})

# enable_testing()
# add_subdirectory("${PROJECT_SOURCE_DIR}/tests")