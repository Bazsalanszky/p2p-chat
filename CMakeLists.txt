cmake_minimum_required(VERSION 3.14)
project(p2p C)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-Werror")

add_executable(p2p main.c peer.h peer.c utility.c utility.h modules/webio/webio.h modules/webio/webio.c modules/config.h modules/config.c modules/map.h modules/map.c)
add_executable(p2p-2 main.c peer.h peer.c utility.c utility.h modules/webio/webio.h modules/webio/webio.c modules/config.h modules/config.c modules/map.h modules/map.c)
add_executable(p2p-3 main.c peer.h peer.c utility.c utility.h modules/webio/webio.h modules/webio/webio.c modules/config.h modules/config.c modules/map.h modules/map.c)
set_target_properties( p2p-2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/p2p-2 )
set_target_properties( p2p-3 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/p2p-3 )
# Függvények kipróbálására hoztam létre
add_executable(test test.c utility.h utility.c modules/map.h modules/map.c)

target_compile_definitions(p2p-2 PUBLIC RANDOM_PORT)
target_compile_definitions(p2p-3 PUBLIC RANDOM_PORT)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(test ${OPENSSL_LIBRARIES})
target_link_libraries(p2p ${OPENSSL_LIBRARIES})
target_link_libraries(p2p-2 ${OPENSSL_LIBRARIES})
target_link_libraries(p2p-3 ${OPENSSL_LIBRARIES})

if(WIN32)
    target_link_libraries(p2p wsock32 ws2_32)
    target_link_libraries(p2p-2 wsock32 ws2_32)
    target_link_libraries(p2p-3 wsock32 ws2_32)
endif()