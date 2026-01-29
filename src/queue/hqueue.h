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
#include "../common/hlibc_config.h"

/*********************
 *      MACROS
 *********************/

/*
 * 静态分配结构体大小常量
 */
#define HQUEUE_STRUCT_SIZE \
  32 /* size + capacity + type_size + head + tail + data_pool 指针 */

/**
 * 计算静态 queue 所需的 buffer 大小
 * @param type 数据类型
 * @param capacity 容器最大容量
 */
#define HQUEUE_CALC_BUFFER_SIZE(type, capacity) \
  (HQUEUE_STRUCT_SIZE + (capacity) * sizeof(type))

/**
 * 定义一个静态 queue（便捷宏）
 * @param name 变量名
 * @param type 数据类型
 * @param capacity 容器最大容量
 */
#define HQUEUE_DEFINE_STATIC(name, type, capacity)                       \
  static uint8_t name##_buffer[HQUEUE_CALC_BUFFER_SIZE(type, capacity)]; \
  hqueue_ptr_t name =                                                    \
      hqueue_create_static(name##_buffer, sizeof(name##_buffer), sizeof(type))

/**********************
 *      TYPEDEFS
 **********************/
typedef struct hqueue* hqueue_ptr_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if HLIBC_USE_STATIC_ALLOC == 0
/**
 * 创建一个 queue 容器（动态分配）
 * @param type_size 装入容器的数据类型的大小。例：`hqueue_create(sizeof(int));`
 * @return 返回新创建的 queue 容器
 */
extern hqueue_ptr_t hqueue_create(uint32_t type_size);

/**
 * 删除给定的 queue 容器（动态分配版本）
 * @param queue 一个由 `hqueue_create` 返回的容器
 */
extern void hqueue_destroy(hqueue_ptr_t queue);

#else /* HLIBC_USE_STATIC_ALLOC == 1 */

/**
 * 创建一个静态分配的 queue 容器（环形队列实现）
 * @param buffer 用户提供的内存缓冲区
 * @param buffer_size 缓冲区大小（使用 HQUEUE_CALC_BUFFER_SIZE 宏计算）
 * @param type_size 装入容器的数据类型的大小
 * @return 返回容器指针，失败返回 NULL
 */
extern hqueue_ptr_t hqueue_create_static(void* buffer, uint32_t buffer_size,
                                         uint32_t type_size);

/**
 * 销毁静态分配的 queue 容器（仅清理内容，不释放内存）
 * @param queue 一个由 `hqueue_create_static` 返回的容器
 */
extern void hqueue_destroy_static(hqueue_ptr_t queue);

/* 兼容性宏定义 */
#define hqueue_create(type_size) ((void)(type_size), (hqueue_ptr_t)NULL)
#define hqueue_destroy(queue) hqueue_destroy_static(queue)

#endif /* HLIBC_USE_STATIC_ALLOC */

/*=====================
 * Setter functions
 *====================*/

extern hlib_status_t hqueue_push(hqueue_ptr_t queue, hdata_ptr_t data_ptr, uint32_t data_size, copy_data_f copy_data);
extern hlib_status_t hqueue_pop(hqueue_ptr_t queue);

/**
 * 清理 queue 容器的所有内容
 * @param queue 一个由 `hqueue_create` 或 `hqueue_create_static` 返回的容器
 * ！！！慎用：调用此函数将会清理掉队列内容，对于普通数据而言并无影响；
 * 但是对于指针数据来说，一旦清空后便无法找到其指针，故而会造成内存泄漏，除非使用者有其他记录。
 */
extern void hqueue_clear(hqueue_ptr_t queue);

/*=======================
 * Getter functions
 *======================*/

extern hdata_ptr_t hqueue_front(hqueue_ptr_t queue);
extern hdata_ptr_t hqueue_rear(hqueue_ptr_t queue);
extern bool hqueue_empty(hqueue_ptr_t queue);
extern uint32_t hqueue_size(hqueue_ptr_t queue);

#if HLIBC_USE_STATIC_ALLOC
/**
 * 获取 queue 容器的最大容量（仅静态分配模式）
 */
extern uint32_t hqueue_capacity(hqueue_ptr_t queue);

/**
 * 检查 queue 容器是否已满（仅静态分配模式）
 */
extern bool hqueue_full(hqueue_ptr_t queue);
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
