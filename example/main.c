/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-12 14:04:29
 * @FilePath: /hlibc/main.c
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include "example.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    printf("---------list data struct test---------\n");
#if !HLIBC_DISABLE_HEAP
    list_example1();
    list_example2();
    list_example3();
#endif
    list_example4();

    printf("---------stack data struct test---------\n");
#if !HLIBC_DISABLE_HEAP
    stack_example1();
#endif
    stack_example2();

    printf("---------queue data struct test---------\n");
#if !HLIBC_DISABLE_HEAP
    queue_example1();
#endif
    queue_example2();
}
