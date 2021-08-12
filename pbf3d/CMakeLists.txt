cmake_minimum_required(VERSION 3.21)
project(pbf3d)
set(CMAKE_CXX_STANDARD 20)

enable_testing()

# options
set(USE_COMPACT_N_SEARCH ON)

find_package(OpenMP)
if (OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif ()

add_subdirectory("${PROJECT_SOURCE_DIR}/src")
add_subdirectory("${PROJECT_SOURCE_DIR}/samples")
add_subdirectory("${PROJECT_SOURCE_DIR}/tests")

if (USE_COMPACT_N_SEARCH)
  message(STATUS "try to use external `COMPACT_N_SEARCH`")
  message(STATUS "external `COMPACT_N_SEARCH` currently not supported, use internal library")
  # add_subdirectory("${PROJECT_SOURCE_DIR}/libs/CompactNSearch")
  # target_link_libraries(pbf3d_lib PUBLIC CompactNSearch)
endif (USE_COMPACT_N_SEARCH)

target_link_libraries(pbf3d PUBLIC pbf3d_lib)
