cmake_minimum_required(VERSION 3.15)
project(hlibc VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(
    hlibc STATIC
    list/hlist.c
    stack/hstack.c
    queue/hqueue.c
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
