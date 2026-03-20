#include "hstatic_storage.h"

size_t hlib_static_bytes(size_t object_size, size_t node_size, size_t type_size, uint32_t capacity)
{
    return HLIB_STATIC_STORAGE_SIZE(object_size, node_size, type_size, capacity);
}

bool hlib_static_prepare(void *buffer, size_t buffer_size, size_t object_size, size_t node_size, size_t type_size, uint32_t capacity, hlib_static_layout_t *layout)
{
    uint8_t *aligned_buffer = NULL;
    size_t required_size = 0;
    size_t nodes_size = 0;
    uintptr_t aligned_address = 0U;

    if (buffer == NULL || layout == NULL) {
        return false;
    }

    required_size = hlib_static_bytes(object_size, node_size, type_size, capacity);
    if (buffer_size < required_size) {
        return false;
    }

    aligned_address = (uintptr_t)HLIB_ALIGN_UP((uintptr_t)buffer, HLIB_MAX_ALIGN);
    aligned_buffer = (uint8_t *)aligned_address;
    nodes_size = HLIB_ALIGN_UP(node_size * (size_t)capacity, HLIB_MAX_ALIGN);

    layout->object_mem = aligned_buffer;
    layout->nodes_mem = aligned_buffer + HLIB_ALIGN_UP(object_size, HLIB_MAX_ALIGN);
    layout->data_mem = (uint8_t *)layout->nodes_mem + nodes_size;
    layout->data_stride = HLIB_ALIGN_UP(type_size, HLIB_MAX_ALIGN);
    return true;
}

void hlib_copy_data_bytes(hdata_ptr_t dest, hcdata_ptr_t src, uint32_t size, copy_data_f copy_data)
{
    if (copy_data != NULL) {
        copy_data(dest, src);
        return;
    }
    memcpy(dest, src, size);
}

void hlib_init_snode_pool(struct hnode *nodes, uint8_t *data_base, size_t data_stride, uint32_t capacity)
{
    uint32_t i = 0;

    for (i = 0; i < capacity; ++i) {
        nodes[i].data_ptr = data_base + (data_stride * i);
        nodes[i].next = (i + 1U < capacity) ? &nodes[i + 1U] : NULL;
    }
}

void hlib_init_dnode_pool(struct hdnode *nodes, uint8_t *data_base, size_t data_stride, uint32_t capacity)
{
    uint32_t i = 0;

    for (i = 0; i < capacity; ++i) {
        nodes[i].data_ptr = data_base + (data_stride * i);
        nodes[i].prev = NULL;
        nodes[i].next = (i + 1U < capacity) ? &nodes[i + 1U] : NULL;
    }
}

struct hnode *hlib_snode_acquire(struct hnode **free_list)
{
    struct hnode *node = NULL;

    if (free_list == NULL || *free_list == NULL) {
        return NULL;
    }

    node = *free_list;
    *free_list = node->next;
    node->next = NULL;
    return node;
}

void hlib_snode_release(struct hnode **free_list, struct hnode *node)
{
    if (free_list == NULL || node == NULL) {
        return;
    }

    node->next = *free_list;
    *free_list = node;
}

struct hdnode *hlib_dnode_acquire(struct hdnode **free_list)
{
    struct hdnode *node = NULL;

    if (free_list == NULL || *free_list == NULL) {
        return NULL;
    }

    node = *free_list;
    *free_list = node->next;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

void hlib_dnode_release(struct hdnode **free_list, struct hdnode *node)
{
    if (free_list == NULL || node == NULL) {
        return;
    }

    node->prev = NULL;
    node->next = *free_list;
    *free_list = node;
}
