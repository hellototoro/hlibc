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
#include <string.h>
#include "hlist.h"
#include "../common/hlibc_type.h"

#if HLIBC_USE_STATIC_ALLOC == 0
#include <stdlib.h>
#endif

/*********************
 *      MACROS
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct hdnode list_dnode_t;

struct hlist {
    uint32_t list_size;
    uint32_t type_size;
    list_dnode_t head;
#if HLIBC_USE_STATIC_ALLOC
    uint32_t capacity;       /* 最大容量 */
    list_dnode_t* node_pool; /* 节点池指针 */
    uint8_t* data_pool;      /* 数据池指针 */
    uint8_t* node_used;      /* 节点使用标记数组 */
#endif
};

/**********************
 *   GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static list_dnode_t* create_dnode(hlist_ptr_t list, const hdata_ptr_t data_ptr,
                                  uint32_t data_size);
static hlib_status_t _insert(hlist_ptr_t list, list_dnode_t* position, const hdata_ptr_t data_ptr, uint32_t data_size);
static void _delete(hlist_ptr_t list, list_dnode_t* position);
#if HLIBC_USE_STATIC_ALLOC
static void free_dnode(hlist_ptr_t list, list_dnode_t* node);
static int32_t get_node_index(hlist_ptr_t list, list_dnode_t* node);
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if HLIBC_USE_STATIC_ALLOC == 0
/* ==================== 动态分配实现 ==================== */

hlist_ptr_t hlist_create(uint32_t type_size)
{
    hlist_ptr_t list = (hlist_ptr_t)malloc(sizeof(struct hlist));
    if (list == NULL) return NULL;
    list->head.data_ptr = NULL;
    list->head.prev = &list->head;
    list->head.next = &list->head;
    list->list_size = 0;
    list->type_size = type_size;
    return list;
}

void hlist_destroy(hlist_ptr_t list)
{
    hlist_clear(list);
    free(list);
}

#else /* HLIBC_USE_STATIC_ALLOC == 1 */
/* ==================== 静态分配实现 ==================== */

hlist_ptr_t hlist_create_static(void* buffer, uint32_t buffer_size,
                                uint32_t type_size) {
  if (buffer == NULL || type_size == 0) return NULL;

  /* 计算可用容量 */
  uint32_t header_size = sizeof(struct hlist);
  if (buffer_size <= header_size) return NULL;

  uint32_t remaining = buffer_size - header_size;
  uint32_t per_node_size = sizeof(list_dnode_t) + type_size + sizeof(uint8_t);
  uint32_t capacity = remaining / per_node_size;

  if (capacity == 0) return NULL;

  /* 初始化 list 结构 */
  hlist_ptr_t list = (hlist_ptr_t)buffer;
  uint8_t* ptr = (uint8_t*)buffer + header_size;

  list->list_size = 0;
  list->type_size = type_size;
  list->capacity = capacity;

  /* 分配节点池 */
  list->node_pool = (list_dnode_t*)ptr;
  ptr += capacity * sizeof(list_dnode_t);

  /* 分配数据池 */
  list->data_pool = ptr;
  ptr += capacity * type_size;

  /* 分配使用标记数组 */
  list->node_used = ptr;
  memset(list->node_used, 0, capacity);

  /* 初始化头节点 */
  list->head.data_ptr = NULL;
  list->head.prev = &list->head;
  list->head.next = &list->head;

  return list;
}

void hlist_destroy_static(hlist_ptr_t list) {
  if (list == NULL) return;
  hlist_clear(list);
  /* 静态分配不释放内存，只重置状态 */
  memset(list->node_used, 0, list->capacity);
}

#endif /* HLIBC_USE_STATIC_ALLOC */

/*=====================
 * Setter functions
 *====================*/

hlib_status_t hlist_insert(hlist_ptr_t list, hlist_iterator_ptr_t position, const hdata_ptr_t data_ptr, uint32_t data_size)
{
    return _insert(list, position->prev, data_ptr, data_size);
}

hlib_status_t hlist_push_back(hlist_ptr_t list, const hdata_ptr_t data_ptr, uint32_t data_size)
{
    return _insert(list, list->head.prev, data_ptr, data_size);
}

hlib_status_t hlist_push_front(hlist_ptr_t list, const hdata_ptr_t data_ptr, uint32_t data_size)
{
    return _insert(list, &list->head, data_ptr, data_size);
}

void hlist_pop_back(hlist_ptr_t list)
{
    _delete(list, list->head.prev);
}

void hlist_pop_front(hlist_ptr_t list)
{
    _delete(list, list->head.next);
}

void hlist_clear(hlist_ptr_t list)
{
    while(!hlist_empty(list)) {
        _delete(list, list->head.next);
    }
}

/*=======================
 * Getter functions
 *======================*/

hdata_ptr_t hlist_back(hlist_ptr_t list)
{
    return list->head.prev->data_ptr;
}

hdata_ptr_t hlist_front(hlist_ptr_t list)
{
    return list->head.next->data_ptr;
}

hlist_iterator_ptr_t hlist_begin(hlist_ptr_t list)
{
    return list->head.next;
}

hlist_iterator_ptr_t hlist_end(hlist_ptr_t list)
{
    return list->head.prev;
}

bool hlist_empty(hlist_ptr_t list)
{
    return (list->list_size == 0);
}

uint32_t hlist_size(hlist_ptr_t list)
{
    return list->list_size;
}

#if HLIBC_USE_STATIC_ALLOC
uint32_t hlist_capacity(hlist_ptr_t list) { return list->capacity; }

bool hlist_full(hlist_ptr_t list) {
  return (list->list_size >= list->capacity);
}
#endif

/*=======================
 * Other functions
 *======================*/

/* 迭代器 */
hdata_ptr_t hlist_iter_data(hlist_iterator_ptr_t iter)
{
    return iter->data_ptr;
}

void hlist_iter_forward(hlist_iterator_ptr_t *iter)
{
    *iter = (*iter)->next;
}

void hlist_iter_forward_to(hlist_iterator_ptr_t *iter, int step)
{
    while(step) {
        hlist_iter_forward(iter);
        --step;
    }
}

void hlist_iter_backward(hlist_iterator_ptr_t *iter)
{
    *iter = (*iter)->prev;
}

void hlist_iter_backward_to(hlist_iterator_ptr_t *iter, int step)
{
    while(step) {
        hlist_iter_backward(iter);
        --step;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if HLIBC_USE_STATIC_ALLOC == 0
/* ==================== 动态分配内部函数 ==================== */

static list_dnode_t *create_dnode(hlist_ptr_t list, const hdata_ptr_t data_ptr, uint32_t data_size)
{
  (void)list; /* 动态模式不需要 list 参数 */
  list_dnode_t* node = (list_dnode_t*)malloc(sizeof(list_dnode_t));
  if (node == NULL) return NULL;
  node->data_ptr = (hdata_ptr_t)malloc(data_size);
  if (node->data_ptr == NULL) {
    free(node);
    return NULL;
  }
    memcpy(node->data_ptr, data_ptr, data_size);
    return node;
}

static hlib_status_t _insert(hlist_ptr_t list, list_dnode_t* position, const hdata_ptr_t data_ptr, uint32_t data_size)
{
    if(data_size != list->type_size) return HLIB_ERROR;
    list_dnode_t* node = create_dnode(list, data_ptr, data_size);
    if (node == NULL) return HLIB_ERROR;
    node->next = position->next;
    position->next->prev = node;
    node->prev = position;
    position->next = node;
    ++list->list_size;
    return HLIB_OK;
}

static void _delete(hlist_ptr_t list, list_dnode_t* position)
{
    if (hlist_empty(list)) return;
    position->prev->next = position->next;
    position->next->prev = position->prev;
    free(position->data_ptr);
    free(position);
    --list->list_size;
}

#else /* HLIBC_USE_STATIC_ALLOC == 1 */
/* ==================== 静态分配内部函数 ==================== */

static int32_t get_node_index(hlist_ptr_t list, list_dnode_t* node) {
  if (node < list->node_pool) return -1;
  int32_t index = (int32_t)(node - list->node_pool);
  if (index >= (int32_t)list->capacity) return -1;
  return index;
}

static list_dnode_t* create_dnode(hlist_ptr_t list, const hdata_ptr_t data_ptr,
                                  uint32_t data_size) {
  /* 查找空闲节点 */
  for (uint32_t i = 0; i < list->capacity; i++) {
    if (!list->node_used[i]) {
      list->node_used[i] = 1;
      list_dnode_t* node = &list->node_pool[i];
      node->data_ptr = list->data_pool + i * list->type_size;
      memcpy(node->data_ptr, data_ptr, data_size);
      return node;
    }
  }
  return NULL; /* 内存池已满 */
}

static void free_dnode(hlist_ptr_t list, list_dnode_t* node) {
  int32_t index = get_node_index(list, node);
  if (index >= 0) {
    list->node_used[index] = 0;
  }
}

static hlib_status_t _insert(hlist_ptr_t list, list_dnode_t* position,
                             const hdata_ptr_t data_ptr, uint32_t data_size) {
  if (data_size != list->type_size) return HLIB_ERROR;
  if (list->list_size >= list->capacity) return HLIB_OVERFLOW;

  list_dnode_t* node = create_dnode(list, data_ptr, data_size);
  if (node == NULL) return HLIB_OVERFLOW;

  node->next = position->next;
  position->next->prev = node;
  node->prev = position;
  position->next = node;
  ++list->list_size;
  return HLIB_OK;
}

static void _delete(hlist_ptr_t list, list_dnode_t* position) {
  if (hlist_empty(list)) return;
  position->prev->next = position->next;
  position->next->prev = position->prev;
  free_dnode(list, position);
  --list->list_size;
}

#endif /* HLIBC_USE_STATIC_ALLOC */
