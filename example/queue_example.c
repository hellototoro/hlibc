/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-25 19:07:37
 * @FilePath: /hlibc/example/queue_example.c
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include <stdint.h>
#include "queue/hqueue.h"

static void copy_data(void *dest, const void *src)
{
    int *d = (int *)dest;
    const int *s = (const int *)src;
    *d = *s;
}

#if !HLIBC_DISABLE_HEAP
void queue_example1(void)
{
    hqueue_ptr_t queue = hqueue_create(sizeof(int));
    int a = 10;

    hqueue_push(queue, &a, sizeof(a), copy_data);
    a = 20;
    hqueue_push(queue, &a, sizeof(a), copy_data);
    a = 30;
    hqueue_push(queue, &a, sizeof(a), copy_data);

    while (!hqueue_empty(queue)) {
        int x = DATA_CAST(int)hqueue_front(queue);
        hqueue_pop(queue);
        printf("%d ", x);
    }

    hqueue_destroy(queue);
    printf("\n");
}
#else
void queue_example1(void) {}
#endif

void queue_example2(void)
{
    uint8_t buffer[HQUEUE_STATIC_SIZE(sizeof(int), 3)];
    hqueue_ptr_t queue = hqueue_init_static(buffer, sizeof(buffer), sizeof(int), 3);
    int a = 10;
    hlib_status_t status = HLIB_OK;

    hqueue_push(queue, &a, sizeof(a), copy_data);
    a = 20;
    hqueue_push(queue, &a, sizeof(a), copy_data);
    a = 30;
    hqueue_push(queue, &a, sizeof(a), copy_data);
    a = 40;
    status = hqueue_push(queue, &a, sizeof(a), copy_data);
    printf("static queue overflow status = %d\n", (int)status);

    while (!hqueue_empty(queue)) {
        int x = DATA_CAST(int)hqueue_front(queue);
        hqueue_pop(queue);
        printf("%d ", x);
    }

    hqueue_destroy(queue);
    printf("\n");
}
