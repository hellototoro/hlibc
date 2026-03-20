/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-30 22:03:26
 * @FilePath: /hlibc/common/hlibc_type.h
 * @Description: None
 * @other: None
 */
#ifndef __HLIBC_TYPE_H__
#define __HLIBC_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hcommon.h"

struct hnode {
    hdata_ptr_t data_ptr;
    struct hnode *next;
};

struct hdnode {
    hdata_ptr_t data_ptr;
    struct hdnode *prev, *next;
};

struct hstack_static_layout {
    uint32_t size;
    uint32_t capacity;
    uint32_t type_size;
    hlib_storage_mode_t storage_mode;
    struct hnode *free_list;
    struct hnode *top;
};

struct hqueue_static_layout {
    uint32_t size;
    uint32_t capacity;
    uint32_t type_size;
    hlib_storage_mode_t storage_mode;
    struct hnode *free_list;
    struct hnode *front;
    struct hnode *rear;
};

struct hlist_static_layout {
    uint32_t list_size;
    uint32_t capacity;
    uint32_t type_size;
    hlib_storage_mode_t storage_mode;
    struct hdnode *free_list;
    struct hdnode head;
};

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
