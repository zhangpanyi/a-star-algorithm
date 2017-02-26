project(Test)

cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

set(CXX_FLAGS
  -g
  -Wall
  -Wextra
  -Werror
  -Wconversion
  -Wno-unused-parameter
  -Wold-style-cast
  -Woverloaded-virtual
  -Wpointer-arith
  -Wshadow
  -Wwrite-strings
  -march=native
  -rdynamic
)

set(CURRENT_PROJECT_SRC_LISTS 
  test.cpp
  ../astar/a_star.cpp
  ../astar/blockallocator.cpp
)

include_directories(
    ../astar
)
 
add_executable(Test ${CURRENT_HEADERS} ${CURRENT_PROJECT_SRC_LISTS})
