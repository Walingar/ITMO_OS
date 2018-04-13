cmake_minimum_required(VERSION 3.0)
project(ITMO_OS)

set(CMAKE_CXX_STANDARD 14)

set(UTILS src/utils/execute.h)

add_executable(HW1 src/shell/shell.cpp ${UTILS})
add_executable(HW2 src/find/find.cpp ${UTILS})