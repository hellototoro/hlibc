/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-25 32:37:34
 * @FilePath: /hlibc/queue/hqueue.h
 * @Description: None
 * @other: None
 */
#ifndef __HLIBC_HQUEUE_H__
#define __HLIBC_HQUEUE_H__

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
#define HQUEUE_STATIC_SIZE(type_size, capacity) \
    HLIB_STATIC_STORAGE_SIZE(sizeof(struct hqueue_static_layout), sizeof(struct hnode), (type_size), (capacity))

/**********************
 *      TYPEDEFS
 **********************/
typedef struct hqueue* hqueue_ptr_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**
 * 创建一个 queue 容器
 * @param type_size 装入容器的数据类型的大小。例：`hqueue_create(sizeof(int));`
 * @return 返回新创建的 queue 容器
 */
#if !HLIBC_DISABLE_HEAP
extern hqueue_ptr_t hqueue_create(uint32_t type_size);
#endif

extern size_t hqueue_static_bytes(uint32_t type_size, uint32_t capacity);
extern hqueue_ptr_t hqueue_init_static(void *buffer, size_t buffer_size, uint32_t type_size, uint32_t capacity);

/**
 * 删除给定的 queue 容器
 * @param queue 一个由 `hqueue_create` 返回的容器
 */
extern void hqueue_destroy(hqueue_ptr_t queue);

/*=====================
 * Setter functions
 *====================*/

extern hlib_status_t hqueue_push(hqueue_ptr_t queue, hdata_ptr_t data_ptr, uint32_t data_size, copy_data_f copy_data);
extern hlib_status_t hqueue_pop(hqueue_ptr_t queue);

/**
 * 清理 queue 容器的所有内容
 * @param queue 一个由 `hqueue_create` 返回的容器
 * ！！！慎用：调用此函数将会清理掉队列内容，对于普通数据而言并无影响；
 * 但是对于指针数据来说，一旦清空栈之后便无法找到其指针，故而会造成内存泄漏，除非使用者有其他记录。
 */
extern void hqueue_clear(hqueue_ptr_t queue);

/*=======================
 * Getter functions
 *======================*/

extern hdata_ptr_t hqueue_front(hqueue_ptr_t queue);
extern hdata_ptr_t hqueue_rear(hqueue_ptr_t queue);
extern bool hqueue_empty(hqueue_ptr_t queue);
extern uint32_t hqueue_size(hqueue_ptr_t queue);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
