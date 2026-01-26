cmake_minimum_required(VERSION 3.15)
project(hlibc_example VERSION 0.1.0)

# include(CTest)
# enable_testing()

SET(CMAKE_C_FLAGS "-O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS "-O0 -Wall -g2 -ggdb")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(../src ${PROJECT_BINARY_DIR}/hlibc_build)

include_directories(
    ${PROJECT_SOURCE_DIR}/../src
    ${PROJECT_SOURCE_DIR}
)

add_executable(hlibc_example main.c list_example.c queue_example.c stack_example.c)
target_link_libraries(hlibc_example PRIVATE hlibc)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
