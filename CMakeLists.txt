CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(cracktools)

set(SOURCES crack.cpp main.cpp opts.cpp passthrough.cpp process.cpp)
set(CMAKE_CXX_FLAGS -std=c++11)

add_executable(brute ${SOURCES})

