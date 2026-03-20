/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-17 20:18:07
 * @FilePath: /hlibc/example/list_example.c
 * @Description: None
 * @other: None
 */
#include <stdio.h>
#include <stdint.h>
#if !HLIBC_DISABLE_HEAP
#include <stdlib.h>
#endif
#include "list/hlist.h"

struct test_str
{
    char a;
    double b;
};

#if !HLIBC_DISABLE_HEAP
void list_example1(void)
{
    hlist_ptr_t list = hlist_create(sizeof(int));
    int a[] = {1, 2, 3, 4, 5, 6};
    size_t i = 0U;

    for (i = 0U; i < sizeof(a) / sizeof(a[0]); ++i) {
        hlist_push_back(list, &a[i], sizeof(a[i]));
    }

    for (hlist_iterator_ptr_t it = hlist_begin(list); it != hlist_end(list); hlist_iter_forward(&it)) {
        printf("%d ", DATA_CAST(int)hlist_iter_data(it));
    }
    printf("\n");
    hlist_destroy(list);
}

void list_example2(void)
{
    struct test_str t1;
    hlist_ptr_t list = hlist_create(sizeof(struct test_str));

    t1.a = 'a';
    t1.b = 10;
    hlist_push_back(list, &t1, sizeof(t1));

    t1.a = 'b';
    t1.b = 20;
    hlist_push_front(list, &t1, sizeof(t1));

    for (hlist_iterator_ptr_t it = hlist_begin(list); it != hlist_end(list); hlist_iter_forward(&it)) {
        struct test_str x = DATA_CAST(struct test_str)hlist_iter_data(it);
        printf("a = %c, b = %.1f\n", x.a, x.b);
    }

    hlist_destroy(list);
}

void list_example3(void)
{
    hlist_ptr_t list = hlist_create(sizeof(struct test_str *));
    struct test_str *t1 = (struct test_str *)malloc(sizeof(struct test_str));
    struct test_str *t2 = (struct test_str *)malloc(sizeof(struct test_str));

    t1->a = 'a';
    t1->b = 10;
    hlist_push_back(list, &t1, sizeof(t1));

    t2->a = 'b';
    t2->b = 20;
    hlist_push_back(list, &t2, sizeof(t2));

    for (hlist_iterator_ptr_t it = hlist_begin(list); it != hlist_end(list); hlist_iter_forward(&it)) {
        struct test_str *x = DATA_CAST(struct test_str *)hlist_iter_data(it);
        printf("ptr => a = %c, b = %.1f\n", x->a, x->b);
    }

    while (!hlist_empty(list)) {
        struct test_str *x = DATA_CAST(struct test_str *)hlist_back(list);
        hlist_pop_back(list);
        free(x);
    }

    hlist_destroy(list);
}
#else
void list_example1(void) {}
void list_example2(void) {}
void list_example3(void) {}
#endif

void list_example4(void)
{
    uint8_t buffer[HLIST_STATIC_SIZE(sizeof(struct test_str *), 2)];
    struct test_str values[2];
    struct test_str *p = NULL;
    hlist_ptr_t list = hlist_init_static(buffer, sizeof(buffer), sizeof(struct test_str *), 2);
    hlib_status_t status = HLIB_OK;

    values[0].a = 'x';
    values[0].b = 1.5;
    values[1].a = 'y';
    values[1].b = 2.5;

    p = &values[0];
    hlist_push_back(list, &p, sizeof(p));
    p = &values[1];
    hlist_push_front(list, &p, sizeof(p));

    p = &values[0];
    status = hlist_push_back(list, &p, sizeof(p));
    printf("static list overflow status = %d\n", (int)status);

    for (hlist_iterator_ptr_t it = hlist_begin(list); it != hlist_end(list); hlist_iter_forward(&it)) {
        struct test_str *x = DATA_CAST(struct test_str *)hlist_iter_data(it);
        printf("static ptr => a = %c, b = %.1f\n", x->a, x->b);
    }

    hlist_destroy(list);
}
