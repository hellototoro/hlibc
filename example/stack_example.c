/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-25 18:48:08
 * @FilePath: /hlibc/example/stack_example.c
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include <stdint.h>
#include "stack/hstack.h"

#if !HLIBC_DISABLE_HEAP
void stack_example1(void)
{
    hstack_ptr_t stack = hstack_create(sizeof(int));
    int a = 10;

    hstack_push(stack, &a, sizeof(a), NULL);
    a = 20;
    hstack_push(stack, &a, sizeof(a), NULL);
    a = 30;
    hstack_push(stack, &a, sizeof(a), NULL);

    while (!hstack_empty(stack)) {
        int x = DATA_CAST(int)hstack_top(stack);
        hstack_pop(stack);
        printf("%d ", x);
    }

    hstack_destroy(stack);
    printf("\n");
}
#else
void stack_example1(void) {}
#endif

void stack_example2(void)
{
    uint8_t buffer[HSTACK_STATIC_SIZE(sizeof(int), 3)];
    hstack_ptr_t stack = hstack_init_static(buffer, sizeof(buffer), sizeof(int), 3);
    int a = 10;
    hlib_status_t status = HLIB_OK;

    hstack_push(stack, &a, sizeof(a), NULL);
    a = 20;
    hstack_push(stack, &a, sizeof(a), NULL);
    a = 30;
    hstack_push(stack, &a, sizeof(a), NULL);
    a = 40;
    status = hstack_push(stack, &a, sizeof(a), NULL);
    printf("static stack overflow status = %d\n", (int)status);

    while (!hstack_empty(stack)) {
        int x = DATA_CAST(int)hstack_top(stack);
        hstack_pop(stack);
        printf("%d ", x);
    }

    hstack_destroy(stack);
    printf("\n");
}
