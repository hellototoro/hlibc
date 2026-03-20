/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-25 32:37:34
 * @FilePath: /hlibc/queue/hqueue.c
 * @Description: None
 * @other: None
 */

/*********************
 *      INCLUDES
 *********************/
#if !HLIBC_DISABLE_HEAP
#include <stdlib.h>
#endif
#include "hqueue.h"
#include "../common/hstatic_storage.h"

/**********************
 *      TYPEDEFS
 **********************/
typedef struct hnode queue_node_t;
struct hqueue {
    uint32_t size;
    uint32_t capacity;
    uint32_t type_size;
    hlib_storage_mode_t storage_mode;
    queue_node_t *free_list;
    queue_node_t *front;
    queue_node_t *rear;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static hlib_status_t hqueue_create_node(hqueue_ptr_t queue, hcdata_ptr_t data_ptr, uint32_t data_size, copy_data_f copy_data, queue_node_t **node);
static void hqueue_release_node(hqueue_ptr_t queue, queue_node_t *node);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#if !HLIBC_DISABLE_HEAP
hqueue_ptr_t hqueue_create(uint32_t type_size)
{
    hqueue_ptr_t queue = NULL;

    if (type_size == 0U) {
        return NULL;
    }

    queue = (hqueue_ptr_t)malloc(sizeof(struct hqueue));
    if (queue == NULL) {
        return NULL;
    }

    queue->size = 0U;
    queue->capacity = 0U;
    queue->type_size = type_size;
    queue->storage_mode = HLIB_STORAGE_HEAP;
    queue->free_list = NULL;
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}
#endif

size_t hqueue_static_bytes(uint32_t type_size, uint32_t capacity)
{
    return hlib_static_bytes(sizeof(struct hqueue), sizeof(queue_node_t), type_size, capacity);
}

hqueue_ptr_t hqueue_init_static(void *buffer, size_t buffer_size, uint32_t type_size, uint32_t capacity)
{
    hlib_static_layout_t layout;
    hqueue_ptr_t queue = NULL;

    if (type_size == 0U) {
        return NULL;
    }

    if (!hlib_static_prepare(buffer, buffer_size, sizeof(struct hqueue), sizeof(queue_node_t), type_size, capacity, &layout)) {
        return NULL;
    }

    queue = (hqueue_ptr_t)layout.object_mem;
    queue->size = 0U;
    queue->capacity = capacity;
    queue->type_size = type_size;
    queue->storage_mode = HLIB_STORAGE_STATIC;
    queue->free_list = (capacity > 0U) ? (queue_node_t *)layout.nodes_mem : NULL;
    queue->front = NULL;
    queue->rear = NULL;
    if (capacity > 0U) {
        hlib_init_snode_pool((queue_node_t *)layout.nodes_mem, layout.data_mem, layout.data_stride, capacity);
    }
    return queue;
}

void hqueue_destroy(hqueue_ptr_t queue)
{
    if (queue == NULL) {
        return;
    }

    hqueue_clear(queue);
#if !HLIBC_DISABLE_HEAP
    if (queue->storage_mode == HLIB_STORAGE_HEAP) {
        free(queue);
    }
#endif
}

/*=====================
 * Setter functions
 *====================*/

hlib_status_t hqueue_push(hqueue_ptr_t queue, hdata_ptr_t data_ptr, uint32_t data_size, copy_data_f copy_data)
{
    queue_node_t *node = NULL;
    hlib_status_t status = HLIB_OK;

    if (queue == NULL || data_ptr == NULL || data_size != queue->type_size) {
        return HLIB_ERROR;
    }

    status = hqueue_create_node(queue, data_ptr, data_size, copy_data, &node);
    if (status != HLIB_OK) {
        return status;
    }

    node->next = NULL;
    if (hqueue_empty(queue)) {
        queue->front = node;
        queue->rear = node;
    } else {
        queue->rear->next = node;
        queue->rear = node;
    }

    ++queue->size;
    return HLIB_OK;
}

hlib_status_t hqueue_pop(hqueue_ptr_t queue)
{
    queue_node_t *node = NULL;

    if (queue == NULL || hqueue_empty(queue)) {
        return HLIB_ERROR;
    }

    node = queue->front;
    queue->front = node->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    hqueue_release_node(queue, node);
    --queue->size;
    return HLIB_OK;
}

void hqueue_clear(hqueue_ptr_t queue)
{
    while (queue != NULL && !hqueue_empty(queue)) {
        hqueue_pop(queue);
    }
}

/*=======================
 * Getter functions
 *======================*/

hdata_ptr_t hqueue_front(hqueue_ptr_t queue)
{
    return (queue != NULL && !hqueue_empty(queue)) ? queue->front->data_ptr : NULL;
}

hdata_ptr_t hqueue_rear(hqueue_ptr_t queue)
{
    return (queue != NULL && !hqueue_empty(queue)) ? queue->rear->data_ptr : NULL;
}

bool hqueue_empty(hqueue_ptr_t queue)
{
    return (queue == NULL) || (queue->size == 0U);
}

uint32_t hqueue_size(hqueue_ptr_t queue)
{
    return (queue != NULL) ? queue->size : 0U;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static hlib_status_t hqueue_create_node(hqueue_ptr_t queue, hcdata_ptr_t data_ptr, uint32_t data_size, copy_data_f copy_data, queue_node_t **node)
{
    if (queue->storage_mode == HLIB_STORAGE_STATIC) {
        *node = hlib_snode_acquire(&queue->free_list);
        if (*node == NULL) {
            return HLIB_OVERFLOW;
        }

        hlib_copy_data_bytes((*node)->data_ptr, data_ptr, data_size, copy_data);
        return HLIB_OK;
    }

#if !HLIBC_DISABLE_HEAP
    *node = (queue_node_t *)malloc(sizeof(queue_node_t));
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

static void hqueue_release_node(hqueue_ptr_t queue, queue_node_t *node)
{
    if (queue->storage_mode == HLIB_STORAGE_STATIC) {
        hlib_snode_release(&queue->free_list, node);
        return;
    }

#if !HLIBC_DISABLE_HEAP
    free(node->data_ptr);
    free(node);
#endif
}
