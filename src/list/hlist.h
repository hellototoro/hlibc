/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-12 14:02:54
 * @FilePath: /hlibc/list/hlist.h
 * @Description: None
 * @other: None
 */
#ifndef __HLIBC_HLIST_H__
#define __HLIBC_HLIST_H__

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
 * 静态分配结构体大小常量（用于计算 buffer 大小）
 * 注意：这些值必须与 .c 文件中的结构体大小匹配
 */
#define HLIST_STRUCT_SIZE \
  (4 + 4 +                \
   24) /* list_size + type_size + head + capacity + pools (约32字节对齐) */
#define HLIST_NODE_SIZE \
  24 /* hdnode: data_ptr + prev + next (指针大小按8字节算) */

/**
 * 计算静态 list 所需的 buffer 大小
 * @param type 数据类型
 * @param capacity 容器最大容量
 *
 * 内存布局: [hlist结构体][节点数组][数据数组][使用标记]
 */
#define HLIST_CALC_BUFFER_SIZE(type, capacity) \
  (64 + (capacity) * (HLIST_NODE_SIZE + sizeof(type) + 1))

/**
 * 定义一个静态 list（便捷宏）
 * @param name 变量名
 * @param type 数据类型
 * @param capacity 容器最大容量
 *
 * 使用示例:
 *   HLIST_DEFINE_STATIC(my_list, int, 32);
 *   hlist_push_back(my_list, &value, sizeof(int));
 */
#define HLIST_DEFINE_STATIC(name, type, capacity)                       \
  static uint8_t name##_buffer[HLIST_CALC_BUFFER_SIZE(type, capacity)]; \
  hlist_ptr_t name =                                                    \
      hlist_create_static(name##_buffer, sizeof(name##_buffer), sizeof(type))

/**********************
 *      TYPEDEFS
 **********************/
typedef struct hlist* hlist_ptr_t;
typedef struct hdnode* hlist_iterator_ptr_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if HLIBC_USE_STATIC_ALLOC == 0
/**
 * 创建一个 list 容器（动态分配）
 * @param type_size 装入容器的数据类型的大小。例：`hlist_create(sizeof(int));`
 * @return 返回新创建的容器
 */
extern hlist_ptr_t hlist_create(uint32_t type_size);

/**
 * 删除给定的 list 容器（动态分配版本）
 * @param list 一个由 `hlist_create` 返回的容器
 */
extern void hlist_destroy(hlist_ptr_t list);

#else /* HLIBC_USE_STATIC_ALLOC == 1 */

/**
 * 创建一个静态分配的 list 容器
 * @param buffer 用户提供的内存缓冲区
 * @param buffer_size 缓冲区大小（使用 HLIST_CALC_BUFFER_SIZE 宏计算）
 * @param type_size 装入容器的数据类型的大小
 * @return 返回容器指针，失败返回 NULL
 *
 * 使用示例:
 *   uint8_t buf[HLIST_CALC_BUFFER_SIZE(int, 32)];
 *   hlist_ptr_t list = hlist_create_static(buf, sizeof(buf), sizeof(int));
 */
extern hlist_ptr_t hlist_create_static(void* buffer, uint32_t buffer_size,
                                       uint32_t type_size);

/**
 * 销毁静态分配的 list 容器（仅清理内容，不释放内存）
 * @param list 一个由 `hlist_create_static` 返回的容器
 */
extern void hlist_destroy_static(hlist_ptr_t list);

/* 兼容性宏定义 */
#define hlist_create(type_size) \
  ((void)(type_size), (hlist_ptr_t)NULL) /* 静态模式下禁用 */
#define hlist_destroy(list) hlist_destroy_static(list)

#endif /* HLIBC_USE_STATIC_ALLOC */

/*=====================
 * Setter functions
 *====================*/

extern hlib_status_t hlist_insert(hlist_ptr_t list,
                                  hlist_iterator_ptr_t position,
                                  const hdata_ptr_t data_ptr,
                                  uint32_t data_size);
extern hlib_status_t hlist_push_back(hlist_ptr_t list,
                                     const hdata_ptr_t data_ptr,
                                     uint32_t data_size);
extern hlib_status_t hlist_push_front(hlist_ptr_t list,
                                      const hdata_ptr_t data_ptr,
                                      uint32_t data_size);
extern void hlist_pop_back(hlist_ptr_t list);
extern void hlist_pop_front(hlist_ptr_t list);
/**
 * 清理 list 容器的所有内容
 * @param list 一个由 `hlist_create` 或 `hlist_create_static` 返回的容器
 * ！！！慎用：调用此函数将会清理掉队列内容，对于普通数据而言并无影响；
 * 但是对于指针数据来说，一旦清空之后便无法找到其指针，故而会造成内存泄漏，除非使用者有其他记录。
 */
extern void hlist_clear(hlist_ptr_t list);

/*=======================
 * Getter functions
 *======================*/

extern hdata_ptr_t hlist_back(hlist_ptr_t list);
extern hdata_ptr_t hlist_front(hlist_ptr_t list);
extern hlist_iterator_ptr_t hlist_begin(hlist_ptr_t list);
extern hlist_iterator_ptr_t hlist_end(hlist_ptr_t list);
extern bool hlist_empty(hlist_ptr_t list);
extern uint32_t hlist_size(hlist_ptr_t list);

#if HLIBC_USE_STATIC_ALLOC
/**
 * 获取 list 容器的最大容量（仅静态分配模式）
 * @param list 一个由 `hlist_create_static` 返回的容器
 * @return 容器最大容量
 */
extern uint32_t hlist_capacity(hlist_ptr_t list);

/**
 * 检查 list 容器是否已满（仅静态分配模式）
 * @param list 一个由 `hlist_create_static` 返回的容器
 * @return true 表示已满，false 表示未满
 */
extern bool hlist_full(hlist_ptr_t list);
#endif

/*=======================
 * Other functions
 *======================*/

/* 迭代器 */
extern hdata_ptr_t hlist_iter_data(hlist_iterator_ptr_t iter);
extern void hlist_iter_forward(hlist_iterator_ptr_t *iter);
extern void hlist_iter_backward(hlist_iterator_ptr_t *iter);
extern void hlist_iter_forward_to(hlist_iterator_ptr_t *iter, int step);
extern void hlist_iter_backward_to(hlist_iterator_ptr_t *iter, int step);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
