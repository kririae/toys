set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pedantic -pedantic-errors -Werror -Wextra -Wshadow -Wpointer-arith -Wformat=2 -Weffc++ -Wall")
if (${IS_CLANG_COMPILER})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wloop-analysis")
endif ()

find_package(glfw3 3.3 REQUIRED)
include_directories("${PROJECT_SOURCE_DIR}/libs/include")

file(GLOB source
  "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB lib_source
  "${PROJECT_SOURCE_DIR}/libs/src/*.c*")
list(REMOVE_ITEM source "${PROJECT_SOURCE_DIR}/src/main.cpp")

add_library(pbf3d_lib ${source} ${lib_source})
target_link_libraries(pbf3d_lib PUBLIC glfw ${CMAKE_DL_LIBS})
add_executable(pbf3d "${PROJECT_SOURCE_DIR}/src/main.cpp")

MESSAGE(STATUS "find source: " ${source})
MESSAGE(STATUS "find lib_source: " ${lib_source})
target_link_libraries(pbf3d PUBLIC pbf3d_lib)
