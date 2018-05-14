cmake_minimum_required(VERSION 3.0)
project(ITMO_OS)

set(CMAKE_CXX_STANDARD 14)

set(UTILS src/utils/execute.h)

add_executable(HW1 src/shell/shell.cpp ${UTILS})
add_executable(HW2 src/find/find.cpp ${UTILS})
add_executable(HW3 src/jitcompiler/compiler.cpp)
add_executable(HW5_server src/sync_socket/server.cpp)
add_executable(HW5_client src/sync_socket/client.cpp)
add_executable(HW6_server src/multi_socket/echo_server.cpp)
add_executable(HW6_client src/multi_socket/echo_client.cpp)
add_executable(HW8 src/signal_handler/main.cpp)