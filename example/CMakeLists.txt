project(Example)

cmake_minimum_required(VERSION 2.8)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

include_directories(
    ../astar
)

set(CURRENT_PROJECT_SRC_LISTS 
  test.cpp
  ../astar/a_star.cpp
  ../astar/blockallocator.cpp
)
 
add_executable(Example ${CURRENT_HEADERS} ${CURRENT_PROJECT_SRC_LISTS})
