/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-25 12:35:41
 * @FilePath: /hlibc/stack/hstack.c
 * @Description: None
 * @other: None
 */

/*********************
 *      INCLUDES
 *********************/
#if !HLIBC_DISABLE_HEAP
#include <stdlib.h>
#endif
#include "hstack.h"
#include "../common/hstatic_storage.h"

/**********************
 *      TYPEDEFS
 **********************/
typedef struct hnode hstack_node_t;
struct hstack {
    uint32_t size;
    uint32_t capacity;
    uint32_t type_size;
    hlib_storage_mode_t storage_mode;
    hstack_node_t *free_list;
    hstack_node_t *top;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static hlib_status_t hstack_create_node(hstack_ptr_t stack, hcdata_ptr_t data_ptr, uint32_t data_size, copy_data_f copy_data, hstack_node_t **node);
static void hstack_release_node(hstack_ptr_t stack, hstack_node_t *node);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#if !HLIBC_DISABLE_HEAP
hstack_ptr_t hstack_create(uint32_t type_size)
{
    hstack_ptr_t stack = NULL;

    if (type_size == 0U) {
        return NULL;
    }

    stack = (hstack_ptr_t)malloc(sizeof(struct hstack));
    if (stack == NULL) {
        return NULL;
    }

    stack->size = 0U;
    stack->capacity = 0U;
    stack->type_size = type_size;
    stack->storage_mode = HLIB_STORAGE_HEAP;
    stack->free_list = NULL;
    stack->top = NULL;
    return stack;
}
#endif

size_t hstack_static_bytes(uint32_t type_size, uint32_t capacity)
{
    return hlib_static_bytes(sizeof(struct hstack), sizeof(hstack_node_t), type_size, capacity);
}

hstack_ptr_t hstack_init_static(void *buffer, size_t buffer_size, uint32_t type_size, uint32_t capacity)
{
    hlib_static_layout_t layout;
    hstack_ptr_t stack = NULL;

    if (type_size == 0U) {
        return NULL;
    }

    if (!hlib_static_prepare(buffer, buffer_size, sizeof(struct hstack), sizeof(hstack_node_t), type_size, capacity, &layout)) {
        return NULL;
    }

    stack = (hstack_ptr_t)layout.object_mem;
    stack->size = 0U;
    stack->capacity = capacity;
    stack->type_size = type_size;
    stack->storage_mode = HLIB_STORAGE_STATIC;
    stack->free_list = (capacity > 0U) ? (hstack_node_t *)layout.nodes_mem : NULL;
    stack->top = NULL;
    if (capacity > 0U) {
        hlib_init_snode_pool((hstack_node_t *)layout.nodes_mem, layout.data_mem, layout.data_stride, capacity);
    }
    return stack;
}

void hstack_destroy(hstack_ptr_t stack)
{
    if (stack == NULL) {
        return;
    }

    hstack_clear(stack);
#if !HLIBC_DISABLE_HEAP
    if (stack->storage_mode == HLIB_STORAGE_HEAP) {
        free(stack);
    }
#endif
}

/*=====================
 * Setter functions
 *====================*/

hlib_status_t hstack_push(hstack_ptr_t stack, hdata_ptr_t data_ptr, uint32_t data_size, copy_data_f copy_data)
{
    hstack_node_t *node = NULL;
    hlib_status_t status = HLIB_OK;

    if (stack == NULL || data_ptr == NULL || data_size != stack->type_size) {
        return HLIB_ERROR;
    }

    status = hstack_create_node(stack, data_ptr, data_size, copy_data, &node);
    if (status != HLIB_OK) {
        return status;
    }

    node->next = stack->top;
    stack->top = node;
    ++stack->size;
    return HLIB_OK;
}

hlib_status_t hstack_pop(hstack_ptr_t stack)
{
    hstack_node_t *node = NULL;

    if (stack == NULL || stack->top == NULL) {
        return HLIB_ERROR;
    }

    node = stack->top;
    stack->top = node->next;
    hstack_release_node(stack, node);
    --stack->size;
    return HLIB_OK;
}

void hstack_clear(hstack_ptr_t stack)
{
    while (stack != NULL && !hstack_empty(stack)) {
        hstack_pop(stack);
    }
}

/*=======================
 * Getter functions
 *======================*/

hdata_ptr_t hstack_top(hstack_ptr_t stack)
{
    return (stack != NULL && !hstack_empty(stack)) ? stack->top->data_ptr : NULL;
}

bool hstack_empty(hstack_ptr_t stack)
{
    return (stack == NULL) || (stack->size == 0U);
}

uint32_t hstack_size(hstack_ptr_t stack)
{
    return (stack != NULL) ? stack->size : 0U;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static hlib_status_t hstack_create_node(hstack_ptr_t stack, hcdata_ptr_t data_ptr, uint32_t data_size, copy_data_f copy_data, hstack_node_t **node)
{
    if (stack->storage_mode == HLIB_STORAGE_STATIC) {
        *node = hlib_snode_acquire(&stack->free_list);
        if (*node == NULL) {
            return HLIB_OVERFLOW;
        }

        hlib_copy_data_bytes((*node)->data_ptr, data_ptr, data_size, copy_data);
        return HLIB_OK;
    }

#if !HLIBC_DISABLE_HEAP
    *node = (hstack_node_t *)malloc(sizeof(hstack_node_t));
    if (*node == NULL) {
        return HLIB_ERROR;
    }

    (*node)->data_ptr = malloc(data_size);
    if ((*node)->data_ptr == NULL) {
        free(*node);
        *node = NULL;
        return HLIB_ERROR;
    }

    hlib_copy_data_bytes((*node)->data_ptr, data_ptr, data_size, copy_data);
    return HLIB_OK;
#else
    (void)data_ptr;
    (void)data_size;
    (void)copy_data;
    (void)node;
    return HLIB_ERROR;
#endif
}

static void hstack_release_node(hstack_ptr_t stack, hstack_node_t *node)
{
    if (stack->storage_mode == HLIB_STORAGE_STATIC) {
        hlib_snode_release(&stack->free_list, node);
        return;
    }

#if !HLIBC_DISABLE_HEAP
    free(node->data_ptr);
    free(node);
#endif
}
