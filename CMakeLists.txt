cmake_minimum_required(VERSION 3.14)
project(p2p C)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-Werror")

add_executable(p2p main.c peer.h peer.c utility.c utility.h)
# Függvények kipróbálására hoztam létre
add_executable(test test.c utility.h utility.c)

if(WIN32)
    target_link_libraries(p2p wsock32 ws2_32)
endif()