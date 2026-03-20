/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-12 14:02:41
 * @FilePath: /hlibc/list/hlist.c
 * @Description: None
 * @other: None
 */

/*********************
 *      INCLUDES
 *********************/
#if !HLIBC_DISABLE_HEAP
#include <stdlib.h>
#endif
#include "hlist.h"
#include "../common/hstatic_storage.h"

/**********************
 *      TYPEDEFS
 **********************/
typedef struct hdnode list_dnode_t;
struct hlist {
    uint32_t list_size;
    uint32_t capacity;
    uint32_t type_size;
    hlib_storage_mode_t storage_mode;
    list_dnode_t *free_list;
    list_dnode_t head;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void hlist_reset(hlist_ptr_t list);
static hlib_status_t hlist_insert_before(hlist_ptr_t list, list_dnode_t *position, hcdata_ptr_t data_ptr, uint32_t data_size);
static hlib_status_t hlist_create_node(hlist_ptr_t list, hcdata_ptr_t data_ptr, uint32_t data_size, list_dnode_t **node);
static void hlist_delete_node(hlist_ptr_t list, list_dnode_t *position);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#if !HLIBC_DISABLE_HEAP
hlist_ptr_t hlist_create(uint32_t type_size)
{
    hlist_ptr_t list = NULL;

    if (type_size == 0U) {
        return NULL;
    }

    list = (hlist_ptr_t)malloc(sizeof(struct hlist));
    if (list == NULL) {
        return NULL;
    }

    list->capacity = 0U;
    list->type_size = type_size;
    list->storage_mode = HLIB_STORAGE_HEAP;
    list->free_list = NULL;
    hlist_reset(list);
    return list;
}
#endif

size_t hlist_static_bytes(uint32_t type_size, uint32_t capacity)
{
    return hlib_static_bytes(sizeof(struct hlist), sizeof(list_dnode_t), type_size, capacity);
}

hlist_ptr_t hlist_init_static(void *buffer, size_t buffer_size, uint32_t type_size, uint32_t capacity)
{
    hlib_static_layout_t layout;
    hlist_ptr_t list = NULL;

    if (type_size == 0U) {
        return NULL;
    }

    if (!hlib_static_prepare(buffer, buffer_size, sizeof(struct hlist), sizeof(list_dnode_t), type_size, capacity, &layout)) {
        return NULL;
    }

    list = (hlist_ptr_t)layout.object_mem;
    list->capacity = capacity;
    list->type_size = type_size;
    list->storage_mode = HLIB_STORAGE_STATIC;
    list->free_list = (capacity > 0U) ? (list_dnode_t *)layout.nodes_mem : NULL;
    if (capacity > 0U) {
        hlib_init_dnode_pool((list_dnode_t *)layout.nodes_mem, layout.data_mem, layout.data_stride, capacity);
    }
    hlist_reset(list);
    return list;
}

void hlist_destroy(hlist_ptr_t list)
{
    if (list == NULL) {
        return;
    }

    hlist_clear(list);
#if !HLIBC_DISABLE_HEAP
    if (list->storage_mode == HLIB_STORAGE_HEAP) {
        free(list);
    }
#endif
}

/*=====================
 * Setter functions
 *====================*/

hlib_status_t hlist_insert(hlist_ptr_t list, hlist_iterator_ptr_t position, const hdata_ptr_t data_ptr, uint32_t data_size)
{
    if (list == NULL || position == NULL) {
        return HLIB_ERROR;
    }

    return hlist_insert_before(list, position, data_ptr, data_size);
}

hlib_status_t hlist_push_back(hlist_ptr_t list, const hdata_ptr_t data_ptr, uint32_t data_size)
{
    if (list == NULL) {
        return HLIB_ERROR;
    }

    return hlist_insert_before(list, &list->head, data_ptr, data_size);
}

hlib_status_t hlist_push_front(hlist_ptr_t list, const hdata_ptr_t data_ptr, uint32_t data_size)
{
    if (list == NULL) {
        return HLIB_ERROR;
    }

    return hlist_insert_before(list, list->head.next, data_ptr, data_size);
}

void hlist_pop_back(hlist_ptr_t list)
{
    if (list == NULL || hlist_empty(list)) {
        return;
    }

    hlist_delete_node(list, list->head.prev);
}

void hlist_pop_front(hlist_ptr_t list)
{
    if (list == NULL || hlist_empty(list)) {
        return;
    }

    hlist_delete_node(list, list->head.next);
}

void hlist_clear(hlist_ptr_t list)
{
    if (list == NULL) {
        return;
    }

    while (!hlist_empty(list)) {
        hlist_delete_node(list, list->head.next);
    }
}

/*=======================
 * Getter functions
 *======================*/

hdata_ptr_t hlist_back(hlist_ptr_t list)
{
    return (list != NULL && !hlist_empty(list)) ? list->head.prev->data_ptr : NULL;
}

hdata_ptr_t hlist_front(hlist_ptr_t list)
{
    return (list != NULL && !hlist_empty(list)) ? list->head.next->data_ptr : NULL;
}

hlist_iterator_ptr_t hlist_begin(hlist_ptr_t list)
{
    return (list != NULL) ? list->head.next : NULL;
}

hlist_iterator_ptr_t hlist_end(hlist_ptr_t list)
{
    return (list != NULL) ? &list->head : NULL;
}

bool hlist_empty(hlist_ptr_t list)
{
    return (list == NULL) || (list->list_size == 0U);
}

uint32_t hlist_size(hlist_ptr_t list)
{
    return (list != NULL) ? list->list_size : 0U;
}

/*=======================
 * Other functions
 *======================*/

hdata_ptr_t hlist_iter_data(hlist_iterator_ptr_t iter)
{
    return (iter != NULL) ? iter->data_ptr : NULL;
}

void hlist_iter_forward(hlist_iterator_ptr_t *iter)
{
    if (iter == NULL || *iter == NULL) {
        return;
    }

    *iter = (*iter)->next;
}

void hlist_iter_forward_to(hlist_iterator_ptr_t *iter, int step)
{
    while (step > 0) {
        hlist_iter_forward(iter);
        --step;
    }
}

void hlist_iter_backward(hlist_iterator_ptr_t *iter)
{
    if (iter == NULL || *iter == NULL) {
        return;
    }

    *iter = (*iter)->prev;
}

void hlist_iter_backward_to(hlist_iterator_ptr_t *iter, int step)
{
    while (step > 0) {
        hlist_iter_backward(iter);
        --step;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void hlist_reset(hlist_ptr_t list)
{
    list->list_size = 0U;
    list->head.data_ptr = NULL;
    list->head.prev = &list->head;
    list->head.next = &list->head;
}

static hlib_status_t hlist_insert_before(hlist_ptr_t list, list_dnode_t *position, hcdata_ptr_t data_ptr, uint32_t data_size)
{
    list_dnode_t *node = NULL;
    hlib_status_t status = HLIB_OK;

    if (list == NULL || position == NULL || data_ptr == NULL || data_size != list->type_size) {
        return HLIB_ERROR;
    }

    status = hlist_create_node(list, data_ptr, data_size, &node);
    if (status != HLIB_OK) {
        return status;
    }

    node->next = position;
    node->prev = position->prev;
    position->prev->next = node;
    position->prev = node;
    ++list->list_size;
    return HLIB_OK;
}

static hlib_status_t hlist_create_node(hlist_ptr_t list, hcdata_ptr_t data_ptr, uint32_t data_size, list_dnode_t **node)
{
    if (list->storage_mode == HLIB_STORAGE_STATIC) {
        *node = hlib_dnode_acquire(&list->free_list);
        if (*node == NULL) {
            return HLIB_OVERFLOW;
        }

        hlib_copy_data_bytes((*node)->data_ptr, data_ptr, data_size, NULL);
        return HLIB_OK;
    }

#if !HLIBC_DISABLE_HEAP
    *node = (list_dnode_t *)malloc(sizeof(list_dnode_t));
    if (*node == NULL) {
        return HLIB_ERROR;
    }

    (*node)->data_ptr = malloc(data_size);
    if ((*node)->data_ptr == NULL) {
        free(*node);
        *node = NULL;
        return HLIB_ERROR;
    }

    hlib_copy_data_bytes((*node)->data_ptr, data_ptr, data_size, NULL);
    return HLIB_OK;
#else
    (void)data_ptr;
    (void)data_size;
    (void)node;
    return HLIB_ERROR;
#endif
}

static void hlist_delete_node(hlist_ptr_t list, list_dnode_t *position)
{
    if (list == NULL || position == NULL || position == &list->head) {
        return;
    }

    position->prev->next = position->next;
    position->next->prev = position->prev;
    --list->list_size;

    if (list->storage_mode == HLIB_STORAGE_STATIC) {
        hlib_dnode_release(&list->free_list, position);
        return;
    }

#if !HLIBC_DISABLE_HEAP
    free(position->data_ptr);
    free(position);
#endif
}
