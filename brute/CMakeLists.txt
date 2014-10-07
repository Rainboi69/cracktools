CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(cracktools)

set(CMAKE_INSTALL_PREFIX "/usr")

set(SOURCES crack.cpp main.cpp opts.cpp passthrough.cpp process.cpp log.cpp)
set(CMAKE_CXX_FLAGS "-std=c++11 -g")

add_executable(brute ${SOURCES})

add_executable(ex_arg examples/arg.c)
add_executable(ex_stdin examples/stdin.c)

set_target_properties(ex_arg PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32 -z execstack")
set_target_properties(ex_stdin PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32 -z execstack")
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/brute DESTINATION bin)

