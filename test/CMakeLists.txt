cmake_minimum_required(VERSION 2.8)

project(AStarTest CXX)

include_directories(../src)

set(SRC_LIST
  Test.cpp
  ../src/AStar.cpp
  ../src/Singleton.cpp
  ../src/BlockAllocator.cpp
)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

add_executable(AStarTest ${SRC_LIST})
