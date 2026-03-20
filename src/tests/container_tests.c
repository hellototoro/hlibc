#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "list/hlist.h"
#include "stack/hstack.h"
#include "queue/hqueue.h"

static int g_failures = 0;

#define EXPECT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "EXPECT_TRUE failed: %s (%s:%d)\n", #expr, __FILE__, __LINE__); \
            ++g_failures; \
        } \
    } while (0)

#define EXPECT_EQ_INT(actual, expected) \
    do { \
        int actual_value__ = (int)(actual); \
        int expected_value__ = (int)(expected); \
        if (actual_value__ != expected_value__) { \
            fprintf(stderr, "EXPECT_EQ_INT failed: %s=%d, %s=%d (%s:%d)\n", #actual, actual_value__, #expected, expected_value__, __FILE__, __LINE__); \
            ++g_failures; \
        } \
    } while (0)

#define EXPECT_PTR_EQ(actual, expected) \
    do { \
        const void *actual_value__ = (const void *)(actual); \
        const void *expected_value__ = (const void *)(expected); \
        if (actual_value__ != expected_value__) { \
            fprintf(stderr, "EXPECT_PTR_EQ failed: %s=%p, %s=%p (%s:%d)\n", #actual, actual_value__, #expected, expected_value__, __FILE__, __LINE__); \
            ++g_failures; \
        } \
    } while (0)

struct pointer_item {
    int id;
};

static void test_list_static(void)
{
    uint8_t buffer[HLIST_STATIC_SIZE(sizeof(int), 3)];
    hlist_ptr_t list = hlist_init_static(buffer, sizeof(buffer), sizeof(int), 3);
    int values[] = {10, 20, 30};
    int expected[] = {10, 20, 30};
    uint32_t index = 0U;

    EXPECT_TRUE(list != NULL);
    EXPECT_PTR_EQ(hlist_begin(list), hlist_end(list));
    EXPECT_TRUE(hlist_push_back(list, &values[1], sizeof(values[1])) == HLIB_OK);
    EXPECT_TRUE(hlist_push_front(list, &values[0], sizeof(values[0])) == HLIB_OK);
    EXPECT_TRUE(hlist_insert(list, hlist_end(list), &values[2], sizeof(values[2])) == HLIB_OK);
    EXPECT_TRUE(hlist_push_back(list, &values[0], sizeof(values[0])) == HLIB_OVERFLOW);
    EXPECT_EQ_INT(hlist_size(list), 3);
    EXPECT_EQ_INT(DATA_CAST(int)hlist_front(list), 10);
    EXPECT_EQ_INT(DATA_CAST(int)hlist_back(list), 30);

    for (hlist_iterator_ptr_t it = hlist_begin(list); it != hlist_end(list); hlist_iter_forward(&it)) {
        EXPECT_TRUE(index < 3U);
        EXPECT_EQ_INT(DATA_CAST(int)hlist_iter_data(it), expected[index]);
        ++index;
    }
    EXPECT_EQ_INT(index, 3);

    hlist_pop_front(list);
    EXPECT_TRUE(hlist_push_back(list, &values[0], sizeof(values[0])) == HLIB_OK);
    EXPECT_EQ_INT(DATA_CAST(int)hlist_back(list), 10);
    hlist_clear(list);
    EXPECT_TRUE(hlist_empty(list));
    EXPECT_PTR_EQ(hlist_begin(list), hlist_end(list));
    hlist_destroy(list);
}

static void test_list_pointer_static(void)
{
    uint8_t buffer[HLIST_STATIC_SIZE(sizeof(struct pointer_item *), 2)];
    struct pointer_item items[2];
    struct pointer_item *value = NULL;
    hlist_ptr_t list = hlist_init_static(buffer, sizeof(buffer), sizeof(struct pointer_item *), 2);

    items[0].id = 1;
    items[1].id = 2;

    value = &items[0];
    EXPECT_TRUE(hlist_push_back(list, &value, sizeof(value)) == HLIB_OK);
    value = &items[1];
    EXPECT_TRUE(hlist_push_back(list, &value, sizeof(value)) == HLIB_OK);

    EXPECT_PTR_EQ(DATA_CAST(struct pointer_item *)hlist_front(list), &items[0]);
    EXPECT_PTR_EQ(DATA_CAST(struct pointer_item *)hlist_back(list), &items[1]);
    hlist_destroy(list);
}

static void test_stack_static(void)
{
    uint8_t buffer[HSTACK_STATIC_SIZE(sizeof(int), 2)];
    hstack_ptr_t stack = hstack_init_static(buffer, sizeof(buffer), sizeof(int), 2);
    int value = 10;

    EXPECT_TRUE(stack != NULL);
    EXPECT_TRUE(hstack_push(stack, &value, sizeof(value), NULL) == HLIB_OK);
    value = 20;
    EXPECT_TRUE(hstack_push(stack, &value, sizeof(value), NULL) == HLIB_OK);
    value = 30;
    EXPECT_TRUE(hstack_push(stack, &value, sizeof(value), NULL) == HLIB_OVERFLOW);
    EXPECT_TRUE(hstack_push(stack, &value, sizeof(uint8_t), NULL) == HLIB_ERROR);
    EXPECT_EQ_INT(DATA_CAST(int)hstack_top(stack), 20);
    EXPECT_TRUE(hstack_pop(stack) == HLIB_OK);
    EXPECT_EQ_INT(DATA_CAST(int)hstack_top(stack), 10);
    EXPECT_TRUE(hstack_pop(stack) == HLIB_OK);
    EXPECT_TRUE(hstack_pop(stack) == HLIB_ERROR);
    EXPECT_TRUE(hstack_push(stack, &value, sizeof(value), NULL) == HLIB_OK);
    EXPECT_EQ_INT(DATA_CAST(int)hstack_top(stack), 30);
    hstack_destroy(stack);
}

static void test_queue_static(void)
{
    uint8_t buffer[HQUEUE_STATIC_SIZE(sizeof(int), 2)];
    hqueue_ptr_t queue = hqueue_init_static(buffer, sizeof(buffer), sizeof(int), 2);
    int value = 10;

    EXPECT_TRUE(queue != NULL);
    EXPECT_TRUE(hqueue_push(queue, &value, sizeof(value), NULL) == HLIB_OK);
    value = 20;
    EXPECT_TRUE(hqueue_push(queue, &value, sizeof(value), NULL) == HLIB_OK);
    value = 30;
    EXPECT_TRUE(hqueue_push(queue, &value, sizeof(value), NULL) == HLIB_OVERFLOW);
    EXPECT_TRUE(hqueue_push(queue, &value, sizeof(uint8_t), NULL) == HLIB_ERROR);
    EXPECT_EQ_INT(DATA_CAST(int)hqueue_front(queue), 10);
    EXPECT_EQ_INT(DATA_CAST(int)hqueue_rear(queue), 20);
    EXPECT_TRUE(hqueue_pop(queue) == HLIB_OK);
    EXPECT_EQ_INT(DATA_CAST(int)hqueue_front(queue), 20);
    EXPECT_TRUE(hqueue_pop(queue) == HLIB_OK);
    EXPECT_TRUE(hqueue_pop(queue) == HLIB_ERROR);
    EXPECT_TRUE(hqueue_push(queue, &value, sizeof(value), NULL) == HLIB_OK);
    EXPECT_EQ_INT(DATA_CAST(int)hqueue_front(queue), 30);
    hqueue_destroy(queue);
}

#if !HLIBC_DISABLE_HEAP
static void test_list_dynamic(void)
{
    hlist_ptr_t list = hlist_create(sizeof(int));
    int values[] = {1, 2, 3, 4};
    int expected[] = {1, 2, 4, 3};
    uint32_t index = 0U;
    hlist_iterator_ptr_t insert_pos = NULL;

    EXPECT_TRUE(list != NULL);
    EXPECT_TRUE(hlist_push_back(list, &values[1], sizeof(values[1])) == HLIB_OK);
    EXPECT_TRUE(hlist_push_front(list, &values[0], sizeof(values[0])) == HLIB_OK);
    EXPECT_TRUE(hlist_push_back(list, &values[2], sizeof(values[2])) == HLIB_OK);
    insert_pos = hlist_end(list);
    hlist_iter_backward(&insert_pos);
    EXPECT_TRUE(hlist_insert(list, insert_pos, &values[3], sizeof(values[3])) == HLIB_OK);
    EXPECT_TRUE(hlist_push_back(list, &values[0], sizeof(uint8_t)) == HLIB_ERROR);

    for (hlist_iterator_ptr_t it = hlist_begin(list); it != hlist_end(list); hlist_iter_forward(&it)) {
        EXPECT_TRUE(index < 4U);
        EXPECT_EQ_INT(DATA_CAST(int)hlist_iter_data(it), expected[index]);
        ++index;
    }
    EXPECT_EQ_INT(index, 4);
    EXPECT_EQ_INT(DATA_CAST(int)hlist_front(list), 1);
    EXPECT_EQ_INT(DATA_CAST(int)hlist_back(list), 3);
    hlist_destroy(list);
}

static void test_stack_dynamic(void)
{
    hstack_ptr_t stack = hstack_create(sizeof(int));
    int value = 1;

    EXPECT_TRUE(stack != NULL);
    EXPECT_TRUE(hstack_push(stack, &value, sizeof(value), NULL) == HLIB_OK);
    value = 2;
    EXPECT_TRUE(hstack_push(stack, &value, sizeof(value), NULL) == HLIB_OK);
    EXPECT_TRUE(hstack_push(stack, &value, sizeof(uint8_t), NULL) == HLIB_ERROR);
    EXPECT_EQ_INT(DATA_CAST(int)hstack_top(stack), 2);
    EXPECT_TRUE(hstack_pop(stack) == HLIB_OK);
    EXPECT_EQ_INT(DATA_CAST(int)hstack_top(stack), 1);
    EXPECT_TRUE(hstack_pop(stack) == HLIB_OK);
    EXPECT_TRUE(hstack_pop(stack) == HLIB_ERROR);
    hstack_destroy(stack);
}

static void test_queue_dynamic(void)
{
    hqueue_ptr_t queue = hqueue_create(sizeof(int));
    int value = 1;

    EXPECT_TRUE(queue != NULL);
    EXPECT_TRUE(hqueue_push(queue, &value, sizeof(value), NULL) == HLIB_OK);
    value = 2;
    EXPECT_TRUE(hqueue_push(queue, &value, sizeof(value), NULL) == HLIB_OK);
    EXPECT_TRUE(hqueue_push(queue, &value, sizeof(uint8_t), NULL) == HLIB_ERROR);
    EXPECT_EQ_INT(DATA_CAST(int)hqueue_front(queue), 1);
    EXPECT_EQ_INT(DATA_CAST(int)hqueue_rear(queue), 2);
    EXPECT_TRUE(hqueue_pop(queue) == HLIB_OK);
    EXPECT_EQ_INT(DATA_CAST(int)hqueue_front(queue), 2);
    EXPECT_TRUE(hqueue_pop(queue) == HLIB_OK);
    EXPECT_TRUE(hqueue_pop(queue) == HLIB_ERROR);
    hqueue_destroy(queue);
}
#endif

int main(void)
{
    test_list_static();
    test_list_pointer_static();
    test_stack_static();
    test_queue_static();

#if !HLIBC_DISABLE_HEAP
    test_list_dynamic();
    test_stack_dynamic();
    test_queue_dynamic();
#endif

    if (g_failures != 0) {
        fprintf(stderr, "container tests failed: %d\n", g_failures);
        return 1;
    }

    printf("container tests passed\n");
    return 0;
}
