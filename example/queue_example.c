/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-25 19:07:37
 * @FilePath: /hlibc/example/queue_example.c
 * @Description: Queue examples supporting both static and dynamic allocation
 * @other: None
 */
#include <stdint.h>
#include <stdio.h>

#include "../src/common/hlibc_config.h"
#include "../src/queue/hqueue.h"


static void copy_data(void* dest, const void* src)
{
    int* d =  (int*)dest;
    const int* s = (int*)src;
    *d = *s;
}

void queue_example1(void)
{
#if HLIBC_USE_STATIC_ALLOC
  static uint8_t queue_buf[HQUEUE_CALC_BUFFER_SIZE(int, 8)];
  hqueue_ptr_t queue =
      hqueue_create_static(queue_buf, sizeof(queue_buf), sizeof(int));
#else
  hqueue_ptr_t queue = hqueue_create(sizeof(int));
#endif

    int a = 10;
    hqueue_push(queue, &a, sizeof(a), copy_data);
    a = 20;
    hqueue_push(queue, &a, sizeof(a), copy_data);
    a = 30;
    hqueue_push(queue, &a, sizeof(a), copy_data);
    int size = hqueue_size(queue);
    for (int i = 0; i < size; ++i) {
        int x = DATA_CAST(int)hqueue_front(queue);
        hqueue_pop(queue);
        printf("%d ", x);
    }
    hqueue_destroy(queue);
    printf("\n");
}
