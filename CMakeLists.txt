cmake_minimum_required(VERSION 3.10)
project(p2p C)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-Werror")

set(SOURCE_FILES modules/peer.h modules/peer.c utility.c utility.h modules/webio.h modules/webio.c modules/config.h modules/config.c lib/map.h lib/map.c lib/tcp-listener.c lib/tcp-listener.h)

option(BUILD_DOC "Build Documentation" ON)
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(BUILD_DOC_DIR ${CMAKE_SOURCE_DIR}/build/docs)
    if(NOT EXISTS ${BUILD_DOC_DIR})
        file(MAKE_DIRECTORY ${BUILD_DOC_DIR})
    endif()

    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    message("Doxygen build started")
    add_custom_target(Doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else(DOXYGEN_FOUND)
    message("Doxygen needs to be installed to generate the documentation.")
endif(DOXYGEN_FOUND)

add_executable(p2p main.c modules/peer.h modules/peer.c utility.c utility.h modules/webio.h modules/webio.c modules/config.h modules/config.c lib/map.h lib/map.c lib/tcp-listener.c lib/tcp-listener.h lib/net.h modules/server.c modules/server.h)
add_executable(p2p-2 main.c modules/peer.h modules/peer.c utility.c utility.h modules/webio.h modules/webio.c modules/config.h modules/config.c lib/map.h lib/map.c lib/tcp-listener.c lib/tcp-listener.h lib/net.h modules/server.c modules/server.h)
add_executable(p2p-3 main.c modules/peer.h modules/peer.c utility.c utility.h modules/webio.h modules/webio.c modules/config.h modules/config.c lib/map.h lib/map.c lib/tcp-listener.c lib/tcp-listener.h lib/net.h modules/server.c modules/server.h)
set_target_properties( p2p-2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/p2p-2 )
set_target_properties( p2p-3 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/p2p-3 )
# Függvények kipróbálására hoztam létre
add_executable(test2 test.c utility.h utility.c lib/map.h lib/map.c lib/tcp-listener.c lib/tcp-listener.h)

    add_custom_command(TARGET p2p PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/htdocs/ $<TARGET_FILE_DIR:p2p>/htdocs/)
    add_custom_command(TARGET p2p-2 PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/htdocs/ $<TARGET_FILE_DIR:p2p-2>/htdocs/)
    add_custom_command(TARGET p2p-3 PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/htdocs/ $<TARGET_FILE_DIR:p2p-3>/htdocs/)

target_compile_definitions(p2p-2 PUBLIC RANDOM_PORT)
target_compile_definitions(p2p-3 PUBLIC RANDOM_PORT)

#find_package(OpenSSL REQUIRED)
#include_directories(${OPENSSL_INCLUDE_DIR})
#target_link_libraries(test ${OPENSSL_LIBRARIES})
#target_link_libraries(p2p ${OPENSSL_LIBRARIES})
#target_link_libraries(p2p-2 ${OPENSSL_LIBRARIES})
#target_link_libraries(p2p-3 ${OPENSSL_LIBRARIES})

if(WIN32)
    target_link_libraries(p2p wsock32 ws2_32)
    target_link_libraries(p2p-2 wsock32 ws2_32)
    target_link_libraries(p2p-3 wsock32 ws2_32)
endif()
