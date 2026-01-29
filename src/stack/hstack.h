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
#include "../common/hlibc_config.h"

/*********************
 *      MACROS
 *********************/

/*
 * 静态分配结构体大小常量
 */
#define HSTACK_STRUCT_SIZE                             \
  32 /* size + capacity + type_size + data_pool 指针 \
      */

/**
 * 计算静态 stack 所需的 buffer 大小
 * @param type 数据类型
 * @param capacity 容器最大容量
 */
#define HSTACK_CALC_BUFFER_SIZE(type, capacity) \
  (HSTACK_STRUCT_SIZE + (capacity) * sizeof(type))

/**
 * 定义一个静态 stack（便捷宏）
 * @param name 变量名
 * @param type 数据类型
 * @param capacity 容器最大容量
 */
#define HSTACK_DEFINE_STATIC(name, type, capacity)                       \
  static uint8_t name##_buffer[HSTACK_CALC_BUFFER_SIZE(type, capacity)]; \
  hstack_ptr_t name =                                                    \
      hstack_create_static(name##_buffer, sizeof(name##_buffer), sizeof(type))

/**********************
 *      TYPEDEFS
 **********************/
typedef struct hstack* hstack_ptr_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if HLIBC_USE_STATIC_ALLOC == 0
/**
 * 创建一个 stack 容器（动态分配）
 * @param type_size 装入容器的数据类型的大小。例：`hstack_create(sizeof(int));`
 * @return 返回新创建的 stack 容器
 */
extern hstack_ptr_t hstack_create(uint32_t type_size);

/**
 * 删除给定的 stack 容器（动态分配版本）
 * @param stack 一个由 `hstack_create` 返回的容器
 */
extern void hstack_destroy(hstack_ptr_t stack);

#else /* HLIBC_USE_STATIC_ALLOC == 1 */

/**
 * 创建一个静态分配的 stack 容器
 * @param buffer 用户提供的内存缓冲区
 * @param buffer_size 缓冲区大小（使用 HSTACK_CALC_BUFFER_SIZE 宏计算）
 * @param type_size 装入容器的数据类型的大小
 * @return 返回容器指针，失败返回 NULL
 */
extern hstack_ptr_t hstack_create_static(void* buffer, uint32_t buffer_size,
                                         uint32_t type_size);

/**
 * 销毁静态分配的 stack 容器（仅清理内容，不释放内存）
 * @param stack 一个由 `hstack_create_static` 返回的容器
 */
extern void hstack_destroy_static(hstack_ptr_t stack);

/* 兼容性宏定义 */
#define hstack_create(type_size) ((void)(type_size), (hstack_ptr_t)NULL)
#define hstack_destroy(stack) hstack_destroy_static(stack)

#endif /* HLIBC_USE_STATIC_ALLOC */

/*=====================
 * Setter functions
 *====================*/

extern hlib_status_t hstack_push(hstack_ptr_t stack, hdata_ptr_t data_ptr, uint32_t data_size, copy_data_f copy_data);
extern hlib_status_t hstack_pop(hstack_ptr_t stack);

/**
 * 清理 stack 容器的所有内容
 * @param stack 一个由 `hstack_create` 或 `hstack_create_static` 返回的容器
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

#if HLIBC_USE_STATIC_ALLOC
/**
 * 获取 stack 容器的最大容量（仅静态分配模式）
 */
extern uint32_t hstack_capacity(hstack_ptr_t stack);

/**
 * 检查 stack 容器是否已满（仅静态分配模式）
 */
extern bool hstack_full(hstack_ptr_t stack);
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
