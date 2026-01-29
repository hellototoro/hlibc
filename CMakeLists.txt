cmake_minimum_required(VERSION 3.15)
project(hlibc VERSION 0.1.0)

include(CTest)
enable_testing()

# ============================================================
# 编译选项
# ============================================================

# 静态分配选项（用于 MCU 等无动态内存分配的环境）
option(HLIBC_USE_STATIC_ALLOC "Use static memory allocation (no malloc/free)" OFF)

# 是否编译示例程序
option(HLIBC_BUILD_EXAMPLES "Build example programs" ON)

# ============================================================
# 输出目录设置
# ============================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ============================================================
# hlibc 库
# ============================================================
add_library(hlibc STATIC
    src/list/hlist.c
    src/stack/hstack.c
    src/queue/hqueue.c
)

target_include_directories(hlibc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 传递编译选项
if(HLIBC_USE_STATIC_ALLOC)
    target_compile_definitions(hlibc PUBLIC HLIBC_USE_STATIC_ALLOC=1)
    message(STATUS "hlibc: Using static memory allocation")
else()
    target_compile_definitions(hlibc PUBLIC HLIBC_USE_STATIC_ALLOC=0)
    message(STATUS "hlibc: Using dynamic memory allocation (malloc/free)")
endif()

# ============================================================
# 示例程序（可选）
# ============================================================
if(HLIBC_BUILD_EXAMPLES)
    add_executable(hlibc_example
        example/main.c
        example/list_example.c
        example/queue_example.c
        example/stack_example.c
    )
    target_link_libraries(hlibc_example PRIVATE hlibc)
    set_target_properties(hlibc_example PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    message(STATUS "hlibc: Building examples")
endif()

# ============================================================
# 打包配置
# ============================================================
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
