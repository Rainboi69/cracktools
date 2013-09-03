CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(cracktools)

set(CMAKE_INSTALL_PREFIX "/usr")

set(SOURCES crack.cpp main.cpp opts.cpp passthrough.cpp process.cpp log.cpp)
set(CMAKE_CXX_FLAGS "-std=c++11 -g")

add_executable(brute ${SOURCES})

add_executable(test test.cpp)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/brute DESTINATION bin)

