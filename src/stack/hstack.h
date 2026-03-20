/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-25 12:35:47
 * @FilePath: /hlibc/stack/hstack.h
 * @Description: None
 * @other: None
 */
#ifndef __HLIBC_HSTACH_H__
#define __HLIBC_HSTACH_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../common/hcommon.h"
#include "../common/hlibc_type.h"

/*********************
 *      MACROS
 *********************/
#define HSTACK_STATIC_SIZE(type_size, capacity) \
    HLIB_STATIC_STORAGE_SIZE(sizeof(struct hstack_static_layout), sizeof(struct hnode), (type_size), (capacity))

/**********************
 *      TYPEDEFS
 **********************/
typedef struct hstack* hstack_ptr_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * 创建一个 stack 容器
 * @param type_size 装入容器的数据类型的大小。例：`hstack_create(sizeof(int));`
 * @return 返回新创建的 stack 容器
 */
#if !HLIBC_DISABLE_HEAP
extern hstack_ptr_t hstack_create(uint32_t type_size);
#endif

extern size_t hstack_static_bytes(uint32_t type_size, uint32_t capacity);
extern hstack_ptr_t hstack_init_static(void *buffer, size_t buffer_size, uint32_t type_size, uint32_t capacity);

/**
 * 删除给定的 stack 容器
 * @param stack 一个由 `hstack_create` 返回的容器
 */
extern void hstack_destroy(hstack_ptr_t stack);

/*=====================
 * Setter functions
 *====================*/

extern hlib_status_t hstack_push(hstack_ptr_t stack, hdata_ptr_t data_ptr, uint32_t data_size, copy_data_f copy_data);
extern hlib_status_t hstack_pop(hstack_ptr_t stack);

/**
 * 清理 stack 容器的所有内容
 * @param stack 一个由 `hstack_create` 返回的容器
 * ！！！慎用：调用此函数将会清理掉栈内容，对于普通数据而言并无影响；
 * 但是对于指针数据来说，一旦清空栈之后便无法找到其指针，故而会造成内存泄漏，除非使用者有其他记录。
 */
extern void hstack_clear(hstack_ptr_t stack);

/*=======================
 * Getter functions
 *======================*/

extern hdata_ptr_t hstack_top(hstack_ptr_t stack);
extern bool hstack_empty(hstack_ptr_t stack);
extern uint32_t hstack_size(hstack_ptr_t stack);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
