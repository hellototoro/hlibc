#ifndef __HLIBC_HSTATIC_STORAGE_H__
#define __HLIBC_HSTATIC_STORAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hlibc_type.h"

typedef struct {
    void *object_mem;
    void *nodes_mem;
    uint8_t *data_mem;
    size_t data_stride;
} hlib_static_layout_t;

size_t hlib_static_bytes(size_t object_size, size_t node_size, size_t type_size, uint32_t capacity);
bool hlib_static_prepare(void *buffer, size_t buffer_size, size_t object_size, size_t node_size, size_t type_size, uint32_t capacity, hlib_static_layout_t *layout);
void hlib_copy_data_bytes(hdata_ptr_t dest, hcdata_ptr_t src, uint32_t size, copy_data_f copy_data);

void hlib_init_snode_pool(struct hnode *nodes, uint8_t *data_base, size_t data_stride, uint32_t capacity);
void hlib_init_dnode_pool(struct hdnode *nodes, uint8_t *data_base, size_t data_stride, uint32_t capacity);
struct hnode *hlib_snode_acquire(struct hnode **free_list);
void hlib_snode_release(struct hnode **free_list, struct hnode *node);
struct hdnode *hlib_dnode_acquire(struct hdnode **free_list);
void hlib_dnode_release(struct hdnode **free_list, struct hdnode *node);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
