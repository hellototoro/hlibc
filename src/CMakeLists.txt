cmake_minimum_required(VERSION 3.15)
project(hlibc VERSION 0.1.0)

include(CTest)
enable_testing()

option(HLIBC_DISABLE_HEAP "Disable heap-backed container constructors" OFF)

add_library(
    hlibc STATIC
    common/hstatic_storage.c
    list/hlist.c
    stack/hstack.c
    queue/hqueue.c
)

target_include_directories(hlibc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(hlibc PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES)
if(HLIBC_DISABLE_HEAP)
    target_compile_definitions(hlibc PUBLIC HLIBC_DISABLE_HEAP=1)
else()
    target_compile_definitions(hlibc PUBLIC HLIBC_DISABLE_HEAP=0)
endif()

if(BUILD_TESTING)
    add_executable(
        hlibc_container_tests
        tests/container_tests.c
    )
    target_link_libraries(hlibc_container_tests PRIVATE hlibc)
    set_target_properties(hlibc_container_tests PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES)
    add_test(NAME hlibc_container_tests COMMAND hlibc_container_tests)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
